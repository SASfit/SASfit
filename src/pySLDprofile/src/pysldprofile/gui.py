"""Graphical front end for :class:`pysldprofile.DensityProfileFit`.

Start with ``pysldprofile-gui [datafile]`` (after ``pip install -e .[gui]``),
``python -m pysldprofile.gui`` or ``python run_gui.py`` in the package folder.

Requires PySide6 and matplotlib.
"""

from __future__ import annotations

import copy
import os
import sys
import traceback

import numpy as np
from PySide6 import QtCore, QtGui, QtWidgets

import matplotlib

matplotlib.use("QtAgg")
from matplotlib.backends.backend_qtagg import FigureCanvasQTAgg, NavigationToolbar2QT  # noqa: E402
from matplotlib.figure import Figure  # noqa: E402

from .density import DensityProfileFit  # noqa: E402
from .io_utils import load_data  # noqa: E402
from .resolution import Resolution, SlitResolution  # noqa: E402
from .background import Background, Constant, OrnsteinZernike, PowerLaw  # noqa: E402

Q_UNITS = [("as in file", 1.0), ("file in 1/nm  ->  1/Å", 0.1), ("file in 1/Å  ->  1/nm", 10.0)]


class _Abort(Exception):
    pass


def geo_exponent(fitter):
    from .geometry import exponent

    return f"{exponent(fitter.geometry, fitter.basis):.4g}"


class FitWorker(QtCore.QThread):
    """Runs DensityProfileFit.fit in a background thread."""

    message = QtCore.Signal(str)
    step = QtCore.Signal()
    done = QtCore.Signal(object, object)  # (result, fitter)
    failed = QtCore.Signal(str)

    def __init__(self, fitter: DensityProfileFit, fit_kwargs: dict, job=None):
        super().__init__()
        self.fitter = fitter
        self.fit_kwargs = fit_kwargs
        self.job = job  # optional callable(fitter) -> result (default: fitter.fit(**fit_kwargs))
        self._stop = False

    def stop(self):
        self._stop = True

    def _progress(self, msg):
        if self._stop:
            raise _Abort()
        if msg:
            self.message.emit(msg)
            if msg.startswith("r_max=") or msg.startswith("p="):
                self.step.emit()

    def run(self):
        self.fitter.progress = self._progress
        try:
            res = self.job(self.fitter) if self.job is not None else self.fitter.fit(**self.fit_kwargs)
        except _Abort:
            self.failed.emit("stopped by user")
            return
        except Exception:  # pragma: no cover - reported in GUI
            self.failed.emit(traceback.format_exc())
            return
        finally:
            self.fitter.progress = None
        self.done.emit(res, self.fitter)


def _dspin(value, lo, hi, decimals=4, step=None):
    w = QtWidgets.QDoubleSpinBox()
    w.setDecimals(decimals)
    w.setRange(lo, hi)
    w.setValue(value)
    if step:
        w.setSingleStep(step)
    w.setKeyboardTracking(False)
    return w


def _ispin(value, lo, hi):
    w = QtWidgets.QSpinBox()
    w.setRange(lo, hi)
    w.setValue(value)
    return w


class MainWindow(QtWidgets.QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("pySLDprofile – density profile from SAS data (Hansen 2016)")
        self.resize(1400, 860)
        self.raw = None  # (q, I, s, dq) as read
        self.data = None  # (q, I, s, dq) after unit conversion and q-range cut
        self.result = None
        self.worker = None
        self.filename = None

        # ---------------- controls
        ctrl = QtWidgets.QWidget()
        cl = QtWidgets.QVBoxLayout(ctrl)

        g = QtWidgets.QGroupBox("Data")
        f = QtWidgets.QFormLayout(g)
        self.btn_load = QtWidgets.QPushButton("Load data file…")
        self.btn_load.clicked.connect(self.on_load)
        self.lbl_file = QtWidgets.QLabel("no file loaded")
        self.lbl_file.setWordWrap(True)
        self.cb_units = QtWidgets.QComboBox()
        for name, _ in Q_UNITS:
            self.cb_units.addItem(name)
        self.cb_units.currentIndexChanged.connect(self.on_units)
        self.sp_qmin = _dspin(0.0, 0.0, 1e6, 5)
        self.sp_qmax = _dspin(1.0, 0.0, 1e6, 5)
        self.sp_qmin.valueChanged.connect(self.apply_qrange)
        self.sp_qmax.valueChanged.connect(self.apply_qrange)
        self.lbl_npts = QtWidgets.QLabel("")
        f.addRow(self.btn_load)
        f.addRow(self.lbl_file)
        f.addRow("q units", self.cb_units)
        f.addRow("q min", self.sp_qmin)
        f.addRow("q max", self.sp_qmax)
        f.addRow("", self.lbl_npts)
        cl.addWidget(g)

        g = QtWidgets.QGroupBox("Model")
        f = QtWidgets.QFormLayout(g)
        self.cb_geom = QtWidgets.QComboBox()
        self.cb_geom.addItems(["sphere", "cylinder", "lamella"])
        self.sp_n = _ispin(80, 10, 400)
        self.cb_reg = QtWidgets.QComboBox()
        self.cb_reg.addItems(["smooth", "maxent"])
        self.cb_prior = QtWidgets.QComboBox()
        self.cb_prior.addItems(["uniform", "previous result", "hollow shell"])
        self.sp_shell = _dspin(0.3, 0.0, 0.99, 2, 0.05)
        self.cb_basis = QtWidgets.QComboBox()
        self.cb_basis.addItems(["ρ(r)   (p = 0, Hansen 2016)", "r^(d-1)·ρ(r)   radial mass", "r^p·ρ(r)   custom exponent p"])
        self.cb_basis.setToolTip(
            "Quantity r^p rho(r) represented by the N step values and smoothed by the regularisation.\n"
            "p > 0 allows a centre cusp rho ~ 1/r^p, needed for coil- or star-like profiles whose\n"
            "I(q) decays as a power law without form-factor minima (Gaussian coil: p = d-1 = 2,\n"
            "Daoud-Cotton star: p = 4/3). 'Scan p' compares exponents by their Bayesian evidence.")
        self.cb_basis.currentIndexChanged.connect(lambda i: self.cb_main.setCurrentIndex(1 if i != 0 else 0))
        self.cb_basis.currentIndexChanged.connect(self._update_enabled)
        self.sp_p = _dspin(4.0 / 3.0, -2.0, 2.95, 3, 0.05)
        self.btn_scan_p = QtWidgets.QPushButton("Scan p")
        self.btn_scan_p.setToolTip("Evidence for p = 0 … d-0.2 at fixed r_max (last result's r_max, or the r_max field)")
        self.btn_scan_p.clicked.connect(self.on_scan_p)
        self.p_scan = None
        self.cb_outer = QtWidgets.QComboBox()
        self.cb_outer.addItems(["free", "zero"])
        self.cb_bg = QtWidgets.QComboBox()
        self.cb_bg.addItems(["none", "fixed value", "fit"])
        self.cb_bg.setToolTip("Flat background: none, a fixed value subtracted in the model, or a free parameter")
        self.cb_bg.currentIndexChanged.connect(self._update_enabled)
        self.sp_bg = _dspin(0.0, -1e12, 1e12, 6)
        self.btn_bg_take = QtWidgets.QPushButton("use fitted")
        self.btn_bg_take.setToolTip("copy the background of the last fit into the value field")
        self.btn_bg_take.clicked.connect(self.on_take_bg)
        self.cb_reg.currentTextChanged.connect(self._update_enabled)
        self.cb_prior.currentTextChanged.connect(self._update_enabled)
        f.addRow("geometry", self.cb_geom)
        f.addRow("number of points N", self.sp_n)
        self.cb_grid = QtWidgets.QComboBox()
        self.cb_grid.addItems(["uniform", "power law  t = (k/N)^γ", "adaptive (two-pass)"])
        self.cb_grid.setToolTip(
            "Shell grid in r.\n"
            "uniform: Hansen's choice; optimal when the spacing r_max/N is at or below pi/q_max.\n"
            "power law: γ > 1 finer at the centre, γ < 1 finer towards r_max.\n"
            "adaptive: a first, over-smoothed fit sets the point density proportional to the\n"
            "  local slope of the represented profile (equidistribution), with shells never\n"
            "  narrower than min(pi/q_max, r_max/N). Pays off for small N; the grid is built at the\n"
            "  r_max of the last result (or the r_max start value) and scales with r_max.")
        self.cb_grid.currentIndexChanged.connect(self._update_enabled)
        self.sp_gamma = _dspin(1.5, 0.2, 5.0, 2, 0.1)
        self.sp_beta = _dspin(3.0, 0.0, 50.0, 1, 0.5)
        self.sp_beta.setToolTip("adaptivity strength beta in M = 1 + beta |c'|/<|c'|>")
        self.ck_ticks = QtWidgets.QCheckBox("show shells")
        self.ck_ticks.setToolTip("mark the shell boundaries on the profile plot")
        self.ck_ticks.toggled.connect(self.redraw)
        f.addRow("r grid", self.cb_grid)
        f.addRow("   γ  /  adaptivity β", self._pair(self.sp_gamma, self.sp_beta))
        f.addRow("", self.ck_ticks)
        f.addRow("regularisation", self.cb_reg)
        f.addRow("max-ent prior", self.cb_prior)
        f.addRow("   r_in / r_max (shell)", self.sp_shell)
        f.addRow("represent / smooth", self.cb_basis)
        f.addRow("   exponent p", self._pair(self.sp_p, self.btn_scan_p))
        f.addRow("outer end at r_max", self.cb_outer)
        cl.addWidget(g)

        g = QtWidgets.QGroupBox("Background  B(q) = c + A·q^-n + I_L/(1+q²ξ²)")
        f = QtWidgets.QFormLayout(g)
        f.addRow("constant c", self.cb_bg)
        f.addRow("   value", self._pair(self.sp_bg, self.btn_bg_take))
        self.ck_pl = QtWidgets.QCheckBox("power law  A·q^-n")
        self.ck_pl.setToolTip("low-q upturn of large aggregates / Porod (n = 4), fractals (1 < n < 4).\n"
                              "A is always fitted (start from the data); n fixed or fitted.\n"
                              "Beware: n close to the profile's own asymptote (e.g. 2 for coils) is degenerate.")
        self.sp_pl_n = _dspin(4.0, 0.1, 6.0, 3, 0.1)
        self.ck_pl_fitn = QtWidgets.QCheckBox("fit n")
        self.ck_oz = QtWidgets.QCheckBox("Ornstein–Zernike  I_L/(1+q²ξ²)")
        self.ck_oz.setToolTip("Lorentzian from concentration fluctuations / network mesh. I_L always fitted;\n"
                              "ξ fixed or fitted (start value 0 = automatic, 1/median(q)).")
        self.sp_oz_xi = _dspin(0.0, 0.0, 1e6, 3)
        self.ck_oz_fitxi = QtWidgets.QCheckBox("fit ξ")
        self.ck_oz_fitxi.setChecked(True)
        for w in (self.ck_pl, self.ck_oz, self.ck_pl_fitn, self.ck_oz_fitxi):
            w.toggled.connect(self._update_enabled)
        f.addRow(self.ck_pl)
        f.addRow("   exponent n", self._pair(self.sp_pl_n, self.ck_pl_fitn))
        f.addRow(self.ck_oz)
        f.addRow("   ξ (start)", self._pair(self.sp_oz_xi, self.ck_oz_fitxi))
        cl.addWidget(g)

        g = QtWidgets.QGroupBox("Instrumental resolution")
        f = QtWidgets.QFormLayout(g)
        self.cb_res = QtWidgets.QComboBox()
        self.cb_res.addItems(["none", "pinhole, σ_q from 4th column", "pinhole, constant σ_q", "slit (Gaussian beam profile)"])
        self.sp_res = _dspin(0.01, 0.0, 1e3, 5)
        self.cb_res.currentTextChanged.connect(self._update_enabled)
        self.cb_kernel = QtWidgets.QComboBox()
        self.cb_kernel.addItems(["rician", "gaussian"])
        self.cb_kernel.setToolTip(
            "Pinhole resolution kernel (same operator as pyOZgui).\n"
            "rician (default, correct): Q is broadened in the 2-D detector plane and radially\n"
            "  averaged, so |Q| about Q0 is Rician: (Q/s^2) exp(-(Q^2+Q0^2)/2s^2) I0(Q Q0/s^2).\n"
            "gaussian: its large Q/dQ limit; puts weight at Q < 0 where dQ/Q is large.\n"
            "The background is not smeared.")
        self.ck_fwhm = QtWidgets.QCheckBox("σ_q given as FWHM")
        self.ck_fwhm.setToolTip("dQ is read as the kernel's sigma (pyOZgui convention); tick if the\n"
                                "reduction writes the full width at half maximum (sigma = FWHM/2.3548)")
        f.addRow("type", self.cb_res)
        f.addRow("kernel", self._pair(self.cb_kernel, self.ck_fwhm))
        f.addRow("σ_q  /  slit width c", self.sp_res)
        cl.addWidget(g)

        g = QtWidgets.QGroupBox("Maximum radius r_max (D = 2 r_max)")
        f = QtWidgets.QFormLayout(g)
        self.ck_auto = QtWidgets.QCheckBox("start from Guinier estimate")
        self.ck_auto.setChecked(True)
        self.sp_rmax = _dspin(5.0, 1e-3, 1e6, 4)
        self.btn_guinier = QtWidgets.QPushButton("Guinier estimate")
        self.btn_guinier.clicked.connect(self.on_guinier)
        self.ck_fix = QtWidgets.QCheckBox("keep r_max fixed")
        self.sp_lo = _dspin(0.7, 0.1, 1.0, 2, 0.05)
        self.sp_hi = _dspin(1.5, 1.0, 5.0, 2, 0.05)
        self.sp_nc = _ispin(15, 3, 80)
        self.sp_nf = _ispin(15, 3, 80)
        for w in (self.ck_auto, self.ck_fix):
            w.toggled.connect(self._update_enabled)
        row = QtWidgets.QHBoxLayout()
        row.addWidget(self.sp_rmax)
        row.addWidget(self.btn_guinier)
        f.addRow(self.ck_auto)
        f.addRow("r_max", row)
        f.addRow(self.ck_fix)
        f.addRow("scan from (× r_max)", self.sp_lo)
        f.addRow("scan to (× r_max)", self.sp_hi)
        f.addRow("points coarse / fine", self._pair(self.sp_nc, self.sp_nf))
        cl.addWidget(g)

        row = QtWidgets.QHBoxLayout()
        self.btn_run = QtWidgets.QPushButton("Run")
        self.btn_run.setStyleSheet("font-weight: bold")
        self.btn_stop = QtWidgets.QPushButton("Stop")
        self.btn_save = QtWidgets.QPushButton("Save results…")
        self.btn_run.clicked.connect(self.on_run)
        self.btn_stop.clicked.connect(self.on_stop)
        self.btn_save.clicked.connect(self.on_save)
        for b in (self.btn_run, self.btn_stop, self.btn_save):
            row.addWidget(b)
        cl.addLayout(row)
        self.progress = QtWidgets.QProgressBar()
        cl.addWidget(self.progress)

        # display controls live in a bar above the plots (see below)
        self.ck_logx = QtWidgets.QCheckBox("log q")
        self.ck_logy = QtWidgets.QCheckBox("log I")
        self.ck_logx.setChecked(True)
        self.ck_logy.setChecked(True)
        self.cb_main = QtWidgets.QComboBox()
        self.cb_main.addItems(["ρ(r)", "r^(d-1)·ρ(r)  radial mass"])
        self.cb_main.setToolTip("Quantity on the left axis of the profile plot")
        self.cb_over = QtWidgets.QComboBox()
        self.cb_over.addItems(["no overlay", "p(r)", "ρ(r)", "r^(d-1)·ρ(r)  radial mass"])
        self.cb_over.setToolTip("Additional curve on a right-hand axis of the profile plot")
        self.ck_plogr = QtWidgets.QCheckBox("log r")
        yscales = ["linear", "log", "symlog", "asinh"]
        ytip = ("y-axis scale of the profile plot:\n"
                "  log    - positive values only; a cusp rho ~ r^-p is a straight line with log r\n"
                "  symlog - linear for |y| < threshold, logarithmic beyond (keeps sign and zero)\n"
                "  asinh  - smooth transition: ~linear for |y| << threshold, ~log for |y| >> threshold\n"
                "threshold: absolute value in y units; empty or 'auto' = 1e-3 x max|y|")
        self.cb_pys = QtWidgets.QComboBox()
        self.cb_pys.addItems(yscales)
        self.cb_pys.setToolTip(ytip)
        self.le_pthr = QtWidgets.QLineEdit()
        self.le_pthr.setPlaceholderText("auto")
        self.le_pthr.setFixedWidth(80)
        self.le_pthr.setToolTip(ytip)
        self.le_pthr.setValidator(QtGui.QDoubleValidator(0.0, 1e300, 12))
        self.cb_oys = QtWidgets.QComboBox()
        self.cb_oys.addItems(yscales)
        self.cb_oys.setToolTip(ytip)
        self.le_othr = QtWidgets.QLineEdit()
        self.le_othr.setPlaceholderText("auto")
        self.le_othr.setFixedWidth(80)
        self.le_othr.setToolTip(ytip)
        self.le_othr.setValidator(QtGui.QDoubleValidator(0.0, 1e300, 12))
        for w in (self.ck_logx, self.ck_logy, self.ck_plogr):
            w.toggled.connect(self.redraw)
        for w in (self.cb_main, self.cb_over, self.cb_pys, self.cb_oys):
            w.currentIndexChanged.connect(self.redraw)
        for w in (self.le_pthr, self.le_othr):
            w.editingFinished.connect(self.redraw)
        cl.addStretch(1)

        scroll = QtWidgets.QScrollArea()
        scroll.setWidget(ctrl)
        scroll.setWidgetResizable(True)
        scroll.setMinimumWidth(410)

        # ---------------- plots + text
        self.fig = Figure(figsize=(10, 7), layout="constrained")
        self.canvas = FigureCanvasQTAgg(self.fig)
        toolbar = NavigationToolbar2QT(self.canvas, self)
        self.summary = QtWidgets.QPlainTextEdit()
        self.summary.setReadOnly(True)
        self.summary.setFont(QtGui.QFontDatabase.systemFont(QtGui.QFontDatabase.FixedFont))
        self.log = QtWidgets.QPlainTextEdit()
        self.log.setReadOnly(True)
        self.log.setFont(QtGui.QFontDatabase.systemFont(QtGui.QFontDatabase.FixedFont))
        tabs = QtWidgets.QTabWidget()
        tabs.addTab(self.summary, "Result")
        tabs.addTab(self.log, "Log")

        right = QtWidgets.QWidget()
        rl = QtWidgets.QVBoxLayout(right)
        bar = QtWidgets.QHBoxLayout()
        bar.addWidget(toolbar)
        bar.addStretch(1)
        for lab, w in (("I(q):", None), (None, self.ck_logx), (None, self.ck_logy)):
            if lab:
                bar.addWidget(QtWidgets.QLabel(lab))
            if w is not None:
                bar.addWidget(w)
        rl.addLayout(bar)
        bar2 = QtWidgets.QHBoxLayout()
        for lab, w in (("profile:", self.cb_main), (None, self.ck_plogr), ("  y:", self.cb_pys),
                       ("threshold:", self.le_pthr), ("     overlay (right axis):", self.cb_over),
                       ("  y:", self.cb_oys), ("threshold:", self.le_othr)):
            if lab:
                bar2.addWidget(QtWidgets.QLabel(lab))
            bar2.addWidget(w)
        bar2.addStretch(1)
        rl.addLayout(bar2)
        split = QtWidgets.QSplitter(QtCore.Qt.Vertical)
        split.addWidget(self.canvas)
        split.addWidget(tabs)
        split.setSizes([650, 200])
        rl.addWidget(split)

        main = QtWidgets.QSplitter()
        main.addWidget(scroll)
        main.addWidget(right)
        main.setSizes([420, 980])
        self.setCentralWidget(main)

        self._update_enabled()
        self.redraw()

    @staticmethod
    def _pair(a, b):
        w = QtWidgets.QWidget()
        h = QtWidgets.QHBoxLayout(w)
        h.setContentsMargins(0, 0, 0, 0)
        h.addWidget(a)
        h.addWidget(b)
        return w

    # ------------------------------------------------------------------ state
    def _update_enabled(self, *args):
        maxent = self.cb_reg.currentText() == "maxent"
        self.cb_prior.setEnabled(maxent)
        self.sp_shell.setEnabled(maxent and self.cb_prior.currentText() == "hollow shell")
        self.sp_res.setEnabled(self.cb_res.currentIndex() in (2, 3))
        self.cb_kernel.setEnabled(self.cb_res.currentIndex() in (1, 2))
        self.ck_fwhm.setEnabled(self.cb_res.currentIndex() in (1, 2))
        self.sp_gamma.setEnabled(self.cb_grid.currentIndex() == 1)
        self.sp_beta.setEnabled(self.cb_grid.currentIndex() == 2)
        self.sp_p.setEnabled(self.cb_basis.currentIndex() == 2)
        self.sp_bg.setEnabled(self.cb_bg.currentIndex() == 1)
        self.btn_bg_take.setEnabled(self.cb_bg.currentIndex() == 1 and self.result is not None)
        for w in (self.sp_pl_n, self.ck_pl_fitn):
            w.setEnabled(self.ck_pl.isChecked())
        for w in (self.sp_oz_xi, self.ck_oz_fitxi):
            w.setEnabled(self.ck_oz.isChecked())
        self.sp_rmax.setEnabled(not self.ck_auto.isChecked())
        fix = self.ck_fix.isChecked()
        for w in (self.sp_lo, self.sp_hi, self.sp_nc, self.sp_nf):
            w.setEnabled(not fix)
        running = self.worker is not None
        self.btn_run.setEnabled(not running and self.data is not None)
        self.btn_scan_p.setEnabled(not running and self.data is not None)
        self.btn_stop.setEnabled(running)
        self.btn_save.setEnabled(not running and self.result is not None)

    def log_msg(self, msg):
        self.log.appendPlainText(msg)

    # ------------------------------------------------------------------ data
    def on_load(self):
        fn, _ = QtWidgets.QFileDialog.getOpenFileName(self, "Load SAS data (q, I, σ [, σ_q])", self.filename or "", "Data (*.dat *.txt *.csv *.pdh *.*)")
        if fn:
            self.load_file(fn)

    def load_file(self, fn):
        try:
            q, I, s, dq = load_data(fn)
        except Exception as e:
            QtWidgets.QMessageBox.critical(self, "Load error", str(e))
            return
        self.filename = fn
        self.raw = (q, I, s, dq)
        self.result = None
        self.lbl_file.setText(os.path.basename(fn) + ("   (4 columns: σ_q found)" if dq is not None else ""))
        self.log_msg(f"loaded {fn}: {q.size} points")
        if dq is not None:
            self.cb_res.setCurrentIndex(1)
        self.on_units()

    def on_units(self, *args):
        if self.raw is None:
            return
        sc = Q_UNITS[self.cb_units.currentIndex()][1]
        q = self.raw[0] * sc
        for w in (self.sp_qmin, self.sp_qmax):
            w.blockSignals(True)
        self.sp_qmin.setValue(q.min())
        self.sp_qmax.setValue(q.max())
        step = (q.max() - q.min()) / 200
        self.sp_qmin.setSingleStep(step)
        self.sp_qmax.setSingleStep(step)
        for w in (self.sp_qmin, self.sp_qmax):
            w.blockSignals(False)
        self.apply_qrange()

    def apply_qrange(self, *args):
        if self.raw is None:
            return
        sc = Q_UNITS[self.cb_units.currentIndex()][1]
        q, I, s, dq = self.raw
        q = q * sc
        dq = dq * sc if dq is not None else None
        m = (q >= self.sp_qmin.value() - 1e-12) & (q <= self.sp_qmax.value() + 1e-12)
        self.data = (q[m], I[m], s[m], dq[m] if dq is not None else None)
        self.lbl_npts.setText(f"{m.sum()} of {q.size} points used")
        self._update_enabled()
        self.redraw()

    def _fitter(self, grid=None):
        return self._fitter_factory()(grid)

    def _fitter_factory(self):
        """Read all settings now (GUI thread) and return grid -> DensityProfileFit."""
        q, I, s, dq = self.data
        ires = self.cb_res.currentIndex()
        if ires == 1:
            if dq is None:
                raise ValueError("the data file has no 4th column (σ_q)")
            res = Resolution(dq, kernel=self.cb_kernel.currentText(), fwhm=self.ck_fwhm.isChecked())
        elif ires == 2:
            res = Resolution(self.sp_res.value(), kernel=self.cb_kernel.currentText(), fwhm=self.ck_fwhm.isChecked())
        elif ires == 3:
            res = SlitResolution(self.sp_res.value())
        else:
            res = None
        n = self.sp_n.value()
        reg = self.cb_reg.currentText()
        prior = None
        if reg == "maxent":
            pc = self.cb_prior.currentText()
            if pc == "previous result":
                if self.result is None:
                    raise ValueError("no previous result to use as prior")
                prior = np.clip(self.result.best.rho, 1e-3 * np.abs(self.result.best.rho).max(), None)
            elif pc == "hollow shell":
                ri = self.sp_shell.value()
                prior = lambda x: np.where(x < ri, 0.05, 1.0)  # noqa: E731
        geom, outer = self.cb_geom.currentText(), self.cb_outer.currentText()
        basis = ("rho", "mass", float(self.sp_p.value()))[self.cb_basis.currentIndex()]
        bg_template = self._background()  # snapshot of the widget settings
        bgf = lambda: copy.deepcopy(bg_template)  # noqa: E731

        def make(grid=None):
            return DensityProfileFit(
                q, I, s, geometry=geom, n=n, regularization=reg, prior=prior,
                outer=outer, background=bgf(), resolution=res, basis=basis, grid=grid,
            )

        return make

    def _background(self):
        terms = []
        ic = self.cb_bg.currentIndex()
        if ic == 1:
            terms.append(Constant(self.sp_bg.value(), fit=False))
        elif ic == 2:
            terms.append(Constant())
        if self.ck_pl.isChecked():
            terms.append(PowerLaw(n=self.sp_pl_n.value(), fit_n=self.ck_pl_fitn.isChecked()))
        if self.ck_oz.isChecked():
            xi = self.sp_oz_xi.value()
            terms.append(OrnsteinZernike(xi=xi if xi > 0 else None, fit_xi=self.ck_oz_fitxi.isChecked()))
        return Background(terms)

    def on_guinier(self):
        if self.data is None:
            return
        try:
            rm = self._fitter().guess_r_max()
        except Exception as e:
            QtWidgets.QMessageBox.warning(self, "Guinier", str(e))
            return
        self.sp_rmax.setValue(rm)
        self.log_msg(f"Guinier estimate r_max = {rm:.5g}")

    # ------------------------------------------------------------------ fit
    def on_run(self):
        if self.data is None:
            return
        gmode = self.cb_grid.currentIndex()
        try:
            make = self._fitter_factory()
            fitter = make(("power", self.sp_gamma.value()) if gmode == 1 else None)
        except Exception as e:
            QtWidgets.QMessageBox.critical(self, "Setup error", str(e))
            return
        rm = fitter.guess_r_max() if self.ck_auto.isChecked() else self.sp_rmax.value()
        self.sp_rmax.setValue(rm)
        fix = self.ck_fix.isChecked()
        kw = dict(r_max=rm, fix_r_max=fix, n_coarse=self.sp_nc.value(), n_fine=self.sp_nf.value(),
                  r_max_range=(self.sp_lo.value() * rm, self.sp_hi.value() * rm))
        nsteps = 1 if fix else kw["n_coarse"] + kw["n_fine"]
        self.progress.setRange(0, nsteps)
        self.progress.setValue(0)
        self.p_scan = None
        bgtxt = ("none", f"fixed {self.sp_bg.value():.5g}", "fit")[self.cb_bg.currentIndex()]
        if self.ck_pl.isChecked():
            bgtxt += f" + A q^-n (n {'fit, start ' if self.ck_pl_fitn.isChecked() else '= '}{self.sp_pl_n.value():.3g})"
        if self.ck_oz.isChecked():
            bgtxt += " + OZ" + (" (xi fit)" if self.ck_oz_fitxi.isChecked() else " (xi fixed)")
        self.log_msg(f"--- run: {fitter.geometry}, N={fitter.n}, {fitter.regularization}, outer={fitter.outer}, "
                     f"represented r^{geo_exponent(fitter)}*rho, background {bgtxt}, r_max start {rm:.5g}" + (" (fixed)" if fix else "")
                     + ("", f", power-law grid gamma={self.sp_gamma.value():.3g}", ", adaptive grid")[gmode])
        if gmode == 2:
            rm_ad = self.result.best.r_max if self.result is not None else rm
            beta = self.sp_beta.value()

            def job(f_uniform, rm_ad=rm_ad, beta=beta, kw=kw, make=make):
                # pass 1: over-smoothed uniform solution -> grid; pass 2: full fit on that grid
                sol_u, _ = f_uniform.best_alpha(rm_ad)
                g = f_uniform.adaptive_grid(rm_ad, beta=beta, solution=f_uniform.solve(sol_u.alpha * np.exp(2.0), rm_ad, x0=sol_u.x))
                f_ad = make(g)
                f_ad.progress = f_uniform.progress
                sol_a, _ = f_ad.best_alpha(rm_ad)
                f_uniform._report(
                    f"grid comparison at r_max={rm_ad:.5g}:  uniform lnEv={sol_u.log_evidence:.2f} chi2/M={sol_u.chi2 / f_uniform.q.size:.3f}"
                    f"  |  adaptive lnEv={sol_a.log_evidence:.2f} chi2/M={sol_a.chi2 / f_uniform.q.size:.3f}")
                try:
                    return f_ad.fit(**kw)
                finally:
                    f_ad.progress = None

            self._start_worker(FitWorker(fitter, kw, job=job))
            return
        self._start_worker(FitWorker(fitter, kw))

    def _start_worker(self, worker):
        self.worker = worker
        self.worker.message.connect(self.log_msg)
        self.worker.step.connect(lambda: self.progress.setValue(self.progress.value() + 1))
        self.worker.done.connect(self.on_done)
        self.worker.failed.connect(self.on_failed)
        self.worker.finished.connect(self._worker_finished)
        self._update_enabled()
        self.statusBar().showMessage("fitting …")
        self.worker.start()

    def on_stop(self):
        if self.worker is not None:
            self.worker.stop()
            self.statusBar().showMessage("stopping …")

    def _worker_finished(self):
        self.worker = None
        self._update_enabled()

    def on_take_bg(self):
        if self.result is not None:
            self.sp_bg.setValue(self.result.background[0])

    def on_scan_p(self):
        if self.data is None:
            return
        try:
            fitter = self._fitter()
        except Exception as e:
            QtWidgets.QMessageBox.critical(self, "Setup error", str(e))
            return
        if self.result is not None:
            rm = self.result.best.r_max
            src = "last result"
        else:
            rm = self.sp_rmax.value() if not self.ck_auto.isChecked() else fitter.guess_r_max()
            src = "r_max field" if not self.ck_auto.isChecked() else "Guinier estimate"
        dim = {"sphere": 3, "cylinder": 2, "lamella": 1}[fitter.geometry]
        ps = np.round(np.arange(0.0, dim - 0.19, 0.2), 3)
        if dim == 3:
            ps = np.unique(np.r_[ps, 4.0 / 3.0])
        self.progress.setRange(0, ps.size)
        self.progress.setValue(0)
        self.log_msg(f"--- scan of exponent p at fixed r_max = {rm:.5g} ({src}): p = {', '.join(f'{v:.3g}' for v in ps)}")
        self._start_worker(FitWorker(fitter, {}, job=lambda f: ("pscan", f.scan_exponent(ps, rm), rm)))

    def on_done(self, res, fitter):
        if isinstance(res, tuple) and res and res[0] == "pscan":
            (p, le, sols), rm = res[1], res[2]
            self.p_scan = (p, le, rm)
            i = int(np.argmax(le))
            self.log_msg("   p      ln evidence   chi2/M    Rg")
            for pi, li, si in zip(p, le, sols):
                self.log_msg(f"  {pi:5.3f}  {li:12.4f}  {si.chi2 / len(self.data[0]):7.3f}  {si.Rg():9.5g}")
            self.log_msg(f"best p = {p[i]:.4g}  (set as custom exponent)")
            self.sp_p.setValue(p[i])
            self.cb_basis.setCurrentIndex(2)
            self.progress.setValue(self.progress.maximum())
            self.statusBar().showMessage(f"best exponent p = {p[i]:.4g}", 8000)
            self.redraw()
            return
        self.result = res
        self.progress.setValue(self.progress.maximum())
        self.summary.setPlainText(res.summary())
        self.log_msg(res.summary())
        self.statusBar().showMessage("done", 5000)
        self.redraw()

    def on_failed(self, msg):
        self.log_msg(msg)
        self.statusBar().showMessage("fit aborted", 5000)
        if msg != "stopped by user":
            QtWidgets.QMessageBox.critical(self, "Fit failed", msg[-2000:])

    def on_save(self):
        if self.result is None:
            return
        base = os.path.splitext(self.filename)[0] + "_sld" if self.filename else "sld"
        fn, _ = QtWidgets.QFileDialog.getSaveFileName(self, "Save results (file prefix)", base, "All files (*)")
        if not fn:
            return
        prefix = os.path.splitext(fn)[0]
        try:
            self.result.save(prefix)
            self.fig.savefig(prefix + ".png", dpi=150)
        except Exception as e:
            QtWidgets.QMessageBox.critical(self, "Save error", str(e))
            return
        self.log_msg(f"saved {prefix}_rho.dat, _fit.dat, _pr.dat, _rmax.dat, _params.txt, .png")

    # ------------------------------------------------------------------ plots
    @staticmethod
    def _dexp(res):
        return {"sphere": 2, "cylinder": 1, "lamella": 0}[res.geometry]

    def _profile_curve(self, res, kind):
        """(r, y, dy or None, legend label) for kind in rho / mass / pr."""
        if kind == "rho":
            return res.r, res.rho, res.rho_err, r"$\rho(r)$"
        if kind == "mass":
            d = self._dexp(res)
            return res.r, res.mass, res.mass_err, (rf"$r^{d}\rho(r)$" if d else r"$\rho(r)$")
        rp, p = res.best.pddf()
        return rp, p, None, "p(r)"

    def _profile_ylabel(self, res, kind):
        if kind == "rho":
            return r"SLD contrast $\rho(r)$"
        if kind == "mass":
            return rf"radial mass $r^{{{self._dexp(res)}}}\rho(r)$"
        return "p(r)"

    @staticmethod
    def _threshold(le, y):
        try:
            t = float(le.text())
            if t > 0:
                return t
        except ValueError:
            pass
        ok = np.isfinite(y)
        m = np.nanmax(np.abs(y[ok])) if ok.any() else 1.0
        return 1e-3 * m if m > 0 else 1.0

    def _apply_yscale(self, ax, x, y, kind, res, scale, le_thr):
        """Set the profile y-scale. For 'linear' and a diverging centre cusp the view is
        limited to r > 5 % of r_max so that the rest of the profile stays readable."""
        from .geometry import exponent

        ok = np.isfinite(y)
        if not ok.any():
            return
        if scale == "log":
            pos = ok & (y > 0)
            if pos.any():
                ax.set_yscale("log")
                top = np.nanmax(y[pos])
                ax.set_ylim(max(np.nanmin(y[pos]), top * 1e-10) * 0.5, top * 2)
            return
        if scale in ("symlog", "asinh"):
            t = self._threshold(le_thr, y)
            if scale == "symlog":
                ax.set_yscale("symlog", linthresh=t)
            else:
                ax.set_yscale("asinh", linear_width=t)
            top = np.nanmax(y[ok])
            bot = np.nanmin(y[ok])
            ax.set_ylim(bot * 2 if bot < 0 else min(bot, 0.0), top * 2 if top > 0 else t)
            ax.axhspan(-t, t, color="0.85", alpha=0.35, lw=0, zorder=0)
            return
        if kind == "rho" and exponent(res.geometry, res.best.basis) > 0:
            sel = ok & (x > 0.05 * res.best.r_max)
            if sel.any():
                top = np.nanmax(np.abs(y[sel]))
                lo = min(np.nanmin(y[sel]), 0.0)
                ax.set_ylim(max(lo * 1.1, -0.3 * top), 1.15 * top)

    def redraw(self, *args):
        fig = self.fig
        fig.clear()
        gs = fig.add_gridspec(3, 2, height_ratios=[3, 1, 3])
        axI = fig.add_subplot(gs[0, 0])
        axR = fig.add_subplot(gs[1, 0], sharex=axI)
        axP = fig.add_subplot(gs[0:2, 1])
        axE = fig.add_subplot(gs[2, 0])
        axD = fig.add_subplot(gs[2, 1])
        res = self.result
        if self.data is not None:
            q, I, s, _ = self.data
            axI.errorbar(q, I, s, fmt="o", ms=3, color="0.45", ecolor="0.7", label="data", zorder=1)
        if res is not None:
            b = res.best
            axI.plot(res.q, res.I_fit, color="C0", lw=1.6, label="model I(q)", zorder=3)
            if b.bg_curve is not None and np.any(b.bg_curve != 0):
                axI.plot(res.q, b.bg_curve, color="C2", ls=":", lw=1.2, label="background")
                if np.any(b.bg_curve != b.bg_curve[0]):
                    axI.plot(res.q, res.I_fit - b.bg_curve, color="C0", ls="--", lw=0.9, alpha=0.7, label="particle")
            if self.data is not None and len(self.data[0]) == len(res.q):
                axR.plot(res.q, (self.data[1] - res.I_fit) / self.data[2], ".", color="C0", ms=3)
            main = ("rho", "mass")[self.cb_main.currentIndex()]
            x, y, dy, lab = self._profile_curve(res, main)
            axP.plot(x, y, color="C1", lw=1.6, label=lab)
            axP.fill_between(x, y - dy, y + dy, color="C1", alpha=0.25, lw=0)
            self._apply_yscale(axP, x, y, main, res, self.cb_pys.currentText(), self.le_pthr)
            if self.ck_plogr.isChecked():
                axP.set_xscale("log")
                axP.set_xlim(max(x[x > 0].min(), 1e-3 * res.best.r_max), None)
            axP.set_ylabel(self._profile_ylabel(res, main))
            axP.axvline(res.r_max[0], color="0.5", ls="--", lw=0.8)
            if self.ck_ticks.isChecked():
                ed = res.best.edges
                axP.plot(ed, np.zeros_like(ed), "|", color="0.3", ms=8, mew=0.8, transform=axP.get_xaxis_transform(),
                         clip_on=False, label=None)
            axP.text(res.r_max[0], axP.get_ylim()[1], " $r_{max}$", va="top", color="0.4", fontsize=8)
            handles, labels = axP.get_legend_handles_labels()
            over = (None, "pr", "rho", "mass")[self.cb_over.currentIndex()]
            if over is not None and over != main:
                ax2 = axP.twinx()
                x2, y2, dy2, lab2 = self._profile_curve(res, over)
                ax2.plot(x2, y2, color="C4", lw=1.2, ls="--", label=lab2)
                if dy2 is not None:
                    ax2.fill_between(x2, y2 - dy2, y2 + dy2, color="C4", alpha=0.15, lw=0)
                self._apply_yscale(ax2, x2, y2, over, res, self.cb_oys.currentText(), self.le_othr)
                ax2.set_ylabel(self._profile_ylabel(res, over), color="C4")
                ax2.tick_params(axis="y", colors="C4")
                h2, l2 = ax2.get_legend_handles_labels()
                handles, labels = handles + h2, labels + l2
            axP.legend(handles, labels, fontsize=8, loc="best")
            axE.plot(res.r_max_grid, res.r_max_posterior, "o-", ms=3, color="C3")
            axE.axvline(res.r_max[0], color="0.5", ls="--", lw=0.8)
            sols = res.solutions
            if sols and self.p_scan is None:
                la = np.array([np.log(s_.alpha) for s_ in sols])
                rm = np.array([s_.r_max for s_ in sols])
                le = np.array([s_.log_evidence for s_ in sols])
                sel = rm == b.r_max
                if sel.sum() > 1:
                    o = np.argsort(la[sel])
                    axD.plot(la[sel][o], le[sel][o] - le.max(), "o-", ms=3, color="C5")
            axI.legend(fontsize=8)
            fig.suptitle(
                f"{res.geometry}, {res.regularization}:  r_max = {res.r_max[0]:.4g} ± {res.r_max[1]:.2g},  "
                f"Rg = {res.Rg[0]:.4g} ± {res.Rg[1]:.2g},  χ²/M = {res.chi2_red:.3g}",
                fontsize=10,
            )
        axI.set_xscale("log" if self.ck_logx.isChecked() else "linear")
        axI.set_yscale("log" if self.ck_logy.isChecked() else "linear")
        from matplotlib.ticker import NullFormatter

        for ax in (axI, axR):
            ax.xaxis.set_minor_formatter(NullFormatter())
        axI.set_ylabel("I(q)")
        axI.tick_params(labelbottom=False)
        axR.axhline(0, color="k", lw=0.5)
        axR.set_ylabel("Δ/σ")
        axR.set_xlabel("q")
        axP.axhline(0, color="k", lw=0.5)
        axP.set_xlabel("r")
        if res is None:
            axP.set_ylabel(r"SLD contrast $\rho(r)$")
        axP.set_title("radial density profile", fontsize=9)
        axE.set_xlabel(r"$r_{max}$")
        axE.set_ylabel(r"P($r_{max}$)")
        if self.p_scan is not None:
            p, le, rm = self.p_scan
            axD.plot(p, le - le.max(), "o-", ms=4, color="C5")
            axD.axvline(p[int(np.argmax(le))], color="0.5", ls="--", lw=0.8)
            axD.set_xlabel(rf"exponent p in $r^p\rho(r)$   ($r_{{max}}$ = {rm:.4g})")
            lo = max(np.min(le - le.max()), -50)
            axD.set_ylim(lo - 2, 2)
        else:
            axD.set_xlabel(r"ln $\alpha$  (at best $r_{max}$)")
        axD.set_ylabel("ln evidence (rel.)")
        self.canvas.draw_idle()


def main(argv=None):
    argv = sys.argv if argv is None else argv
    app = QtWidgets.QApplication.instance() or QtWidgets.QApplication(argv)
    w = MainWindow()
    w.show()
    if len(argv) > 1 and os.path.isfile(argv[1]):
        w.load_file(argv[1])
    return app.exec()


if __name__ == "__main__":
    sys.exit(main())
