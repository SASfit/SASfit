"""
pySASfitInversion GUI: a thin Qt (PySide6) layer over the sasfit_inversion
library. No numerics live here -- every computation (loading, background
fitting/subtraction, kernel construction, solving) calls straight into the
library, so the library stays usable headless/scriptable/testable on its
own (see tests/test_gui_smoke.py, which drives this same MainWindow class
programmatically with QT_QPA_PLATFORM=offscreen).

Layout: a left control panel (file loading, background section, kernel/
solver/grid selection) and a right panel with two stacked matplotlib
canvases -- I(q) (data + background + fit) on top, the recovered
distribution on the bottom.
"""
from __future__ import annotations

import os
import sys
import time
import traceback

# Allow running this file directly (`python gui/app.py`) as well as via
# `python -m gui.app` from the project root -- when run as a plain script,
# Python puts only gui/'s own directory on sys.path, not the project root
# where sasfit_inversion/ actually lives, so add it explicitly.
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))

import numpy as np
from PySide6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout, QFormLayout,
    QPushButton, QLabel, QDoubleSpinBox, QSpinBox, QComboBox, QCheckBox,
    QFileDialog, QGroupBox, QSplitter, QMessageBox, QStatusBar, QScrollArea,
    QFrame, QSizePolicy,
)
from PySide6.QtCore import Qt, QEvent, QTimer
from matplotlib.backends.backend_qtagg import FigureCanvasQTAgg
from matplotlib.figure import Figure

from sasfit_inversion.io_utils import load_sas_data, SASData
from sasfit_inversion.background import fit_background, BackgroundFitResult, background_model
from sasfit_inversion.kernel_registry import KERNEL_REGISTRY
from sasfit_inversion.kernels import (
    build_size_distribution_kernel, build_kernel_on_nodes, make_r_grid, quadrature_points,
)
from sasfit_inversion.solver_registry import SOLVER_REGISTRY, SplineOptions, run_solver
from sasfit_inversion.uncertainty import bootstrap_uncertainty
from sasfit_inversion.goodness_of_fit import estimate_errors, fit_statistics

try:
    from gui import help_texts
except ImportError:  # run as a plain script from inside src/gui
    import help_texts


# Guide line at x(r) = 0 in the distribution plot. Set explicitly (not taken
# from matplotlib's rcParams) so it stays a thin dotted gray line whatever
# style defaults are active; change it here, e.g. to {"linestyle": "-",
# "linewidth": 0.5, ...} for a thin solid line. zorder=0 keeps it behind the
# curves and error bands; it has no label, so it never appears in the legend.
# numpy >= 2.0 renamed trapz to trapezoid
_trapz = getattr(np, "trapezoid", None) or np.trapz

ZERO_LINE_STYLE = {"color": "0.5", "linestyle": ":", "linewidth": 0.8, "zorder": 0}


class InfoPanel(QFrame):
    """One text field for all explanations and detailed results
    (2026-10-10, replaces tooltips and inline hint labels). Shows exactly
    one text -- the one of the control under the mouse, or the details of
    the last load / background fit / solve.

    It sits in a vertical splitter below the controls, so its height can be
    dragged. `required_height()` is the height of the longest registered
    text at the current width: the panel never gets taller than that
    (maximum height) and, as long as the user has not dragged the splitter,
    it is kept at exactly that height. If it is made smaller, the text
    scrolls. `on_required_changed(h)` is called when the required height
    changes (width change, new data/background/result texts)."""

    MARGIN = 6

    def __init__(self, parent=None, on_required_changed=None):
        super().__init__(parent)
        self.setFrameShape(QFrame.StyledPanel)
        self.label = QLabel(help_texts.DEFAULT)
        self.label.setWordWrap(True)
        self.label.setTextFormat(Qt.RichText)
        self.label.setAlignment(Qt.AlignLeft | Qt.AlignTop)
        self.label.setContentsMargins(self.MARGIN, self.MARGIN, self.MARGIN, self.MARGIN)
        self.scroll = QScrollArea()
        self.scroll.setWidgetResizable(True)
        self.scroll.setFrameShape(QFrame.NoFrame)
        self.scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        self.scroll.setWidget(self.label)
        lay = QVBoxLayout(self)
        lay.setContentsMargins(0, 0, 0, 0)
        lay.addWidget(self.scroll)
        self.setMinimumHeight(40)
        self._texts: list[str] = [help_texts.DEFAULT]
        self._width = -1
        self._required = 0
        self._on_required_changed = on_required_changed
        self._measure = QLabel()          # never shown; used for sizing
        self._measure.setWordWrap(True)
        self._measure.setTextFormat(Qt.RichText)
        self._measure.setContentsMargins(self.MARGIN, self.MARGIN, self.MARGIN, self.MARGIN)

    def set_all_texts(self, texts) -> None:
        """All texts the panel may ever show (sets the required height)."""
        self._texts = [help_texts.DEFAULT] + [t for t in texts if t]
        self._width = -1
        self._update_height()

    def show_text(self, text: str | None) -> None:
        self.label.setText(text or help_texts.DEFAULT)
        self.scroll.verticalScrollBar().setValue(0)

    def text_height(self, text: str, label_width: int) -> int:
        """Height a label like self.label needs for `text` at `label_width`
        (measured with an identical hidden QLabel: QTextDocument alone
        underestimates QLabel's rich-text layout by ~10 %)."""
        m = self._measure
        m.setFont(self.label.font())
        m.setText(text)
        return m.heightForWidth(max(label_width, 50))

    def required_height(self, width: int | None = None) -> int:
        label_width = (self.width() if width is None else width) - 2 * self.frameWidth()
        return max(self.text_height(t, label_width) for t in self._texts) + 2 * self.frameWidth()

    def _update_height(self) -> None:
        if self.width() <= 0:
            return
        self._width = self.width()
        req = self.required_height()
        self.setMaximumHeight(req)
        if req != self._required:
            self._required = req
            if self._on_required_changed is not None:
                self._on_required_changed(req)

    def resizeEvent(self, event):
        super().resizeEvent(event)
        if self.width() != self._width:
            self._update_height()


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("pySASfitInversion")
        self.resize(1200, 800)

        self.data: SASData | None = None
        self.bg_result: BackgroundFitResult | None = None
        self.fixed_bg_value: float | None = None
        self.sigma: np.ndarray | None = None   # uncertainties actually used

        splitter = QSplitter(Qt.Horizontal)
        # The control panel keeps growing (kernel/solver/uncertainty options,
        # hint labels, ...) and was clipping at normal window heights --
        # wrap it in a scroll area so everything stays reachable instead of
        # being cut off at the bottom.
        # Help: one info panel below the controls shows the explanation of
        # the control under the mouse (help_texts.HELP); registered via
        # self._help(widget, key) while the controls are built.
        self._help_keys: dict = {}
        self._help_dynamic: dict[str, str] = {
            "data_info": help_texts.NO_DATA,
            "bg_result": help_texts.NO_BACKGROUND,
            "result": help_texts.NO_RESULT,
        }
        self._info_user_sized = False
        self.info_panel = InfoPanel(on_required_changed=self._fit_info_panel)
        control_scroll = QScrollArea()
        control_scroll.setWidgetResizable(True)
        control_panel = self._build_control_panel()
        # long drop-down entries must not widen the panel (no horizontal
        # scrolling); the full entry text is shown when the list opens
        for combo in control_panel.findChildren(QComboBox):
            combo.setSizeAdjustPolicy(QComboBox.AdjustToMinimumContentsLengthWithIcon)
            combo.setMinimumContentsLength(14)
        control_scroll.setWidget(control_panel)
        control_scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        # controls and info panel in a vertical splitter: the info panel's
        # height can be dragged like the plots (Joachim, 2026-10-10)
        self.left_splitter = QSplitter(Qt.Vertical)
        self.left_splitter.addWidget(control_scroll)
        self.left_splitter.addWidget(self.info_panel)
        self.left_splitter.setStretchFactor(0, 1)
        self.left_splitter.setStretchFactor(1, 0)
        self.left_splitter.setChildrenCollapsible(False)
        self.left_splitter.splitterMoved.connect(self._on_info_splitter_moved)
        splitter.addWidget(self.left_splitter)
        splitter.addWidget(self._build_plot_panel())
        splitter.setStretchFactor(1, 1)
        self.setCentralWidget(splitter)

        self.status = QStatusBar()
        self.setStatusBar(self.status)
        self._refresh_help_sizes()

    # ---------------------------------------------------------------- help

    def _on_info_splitter_moved(self, pos: int, index: int) -> None:
        """The user dragged the splitter: keep the chosen height from now on
        (dragging it to the full required height re-enables auto-fit)."""
        self._info_user_sized = self.info_panel.height() < self.info_panel.maximumHeight()

    def _fit_info_panel(self, required: int) -> None:
        """Give the info panel its required height unless the user has
        chosen a height by dragging the splitter."""
        # deferred: the splitter may be in the middle of its own layout pass
        QTimer.singleShot(0, self._apply_info_fit)

    def _apply_info_fit(self) -> None:
        if self._info_user_sized or not hasattr(self, "left_splitter"):
            return
        required = self.info_panel.maximumHeight()
        total = sum(self.left_splitter.sizes())
        if total <= 0:
            return
        h = min(required, max(total - 120, 40))
        self.left_splitter.setSizes([total - h, h])

    def _help(self, widget, key, form: QFormLayout | None = None) -> None:
        """Show help text `key` (str, or callable returning a key) while the
        mouse is over `widget` -- and over its row label in `form`."""
        targets = [widget]
        if form is not None:
            lab = form.labelForField(widget)
            if lab is not None:
                targets.append(lab)
        for w in targets:
            self._help_keys[w] = key
            w.installEventFilter(self)

    def help_text(self, key: str) -> str:
        """Help text with its dynamic part (suggestions, data/background/
        result details) filled in."""
        t = help_texts.text(key)
        if "{dynamic}" in t:
            t = t.replace("{dynamic}", self._help_dynamic.get(key, ""))
        return t

    def _set_info(self, key: str, html: str, show: bool = True) -> None:
        """Set the dynamic part of help text `key`, resize the info panel to
        fit, and (show=True) display it right away."""
        self._help_dynamic[key] = html
        if hasattr(self, "info_panel"):
            self._refresh_help_sizes()
            if show:
                self._show_help(key)

    @staticmethod
    def _table(rows) -> str:
        """Two-column rich-text table for the info panel; a row that is a
        plain string spans both columns (used for notes/warnings)."""
        cells = []
        for row in rows:
            if isinstance(row, str):
                cells.append(f"<tr><td colspan='2'>{row}</td></tr>")
            else:
                cells.append(f"<tr><td nowrap style='padding-right:8px'>{row[0]}</td>"
                             f"<td width='100%'>{row[1]}</td></tr>")
        return ("<table cellspacing='0' cellpadding='0' style='margin-top:2px'>"
                + "".join(cells) + "</table>")

    @staticmethod
    def _short_label(text: str) -> QLabel:
        """One-line status label that never widens the control column."""
        lab = QLabel(text)
        lab.setWordWrap(False)
        lab.setSizePolicy(QSizePolicy.Ignored, QSizePolicy.Preferred)
        return lab

    def _all_help_keys(self) -> list[str]:
        keys = [k for k in help_texts.HELP if not k.startswith(("solver:", "kernel:"))]
        keys += [f"solver:{k}" for k in SOLVER_REGISTRY]
        keys += [f"kernel:{k}" for k in KERNEL_REGISTRY]
        return keys

    def _refresh_help_sizes(self) -> None:
        self.info_panel.set_all_texts(
            [self.help_text(k) or getattr(SOLVER_REGISTRY.get(k[7:]), "description", "")
             for k in self._all_help_keys()])

    def _show_help(self, key: str | None) -> None:
        if key is None:
            self.info_panel.show_text(None)
            return
        text = self.help_text(key)
        if not text and key.startswith("solver:") and key[7:] in SOLVER_REGISTRY:
            spec = SOLVER_REGISTRY[key[7:]]
            text = f"<b>{spec.label}</b><br>{spec.description}"
        self.info_panel.show_text(text)

    def showEvent(self, event):
        super().showEvent(event)
        QTimer.singleShot(0, self._apply_info_fit)

    def eventFilter(self, obj, event):
        key = self._help_keys.get(obj) if hasattr(self, "_help_keys") else None
        if key is not None:
            if event.type() == QEvent.Enter:
                self._show_help(key() if callable(key) else key)
            elif event.type() == QEvent.Leave:
                self._show_help(None)
        return super().eventFilter(obj, event)

    # ------------------------------------------------------------------ UI

    def _build_control_panel(self) -> QWidget:
        panel = QWidget()
        layout = QVBoxLayout(panel)

        # --- file loading ---
        file_group = QGroupBox("Data")
        file_layout = QVBoxLayout(file_group)
        self.load_button = QPushButton("Load data file...")
        self.load_button.clicked.connect(self._on_load_file)
        self.data_info_label = self._short_label("no data loaded")
        file_layout.addWidget(self.load_button)
        file_layout.addWidget(self.data_info_label)
        # error bars: from the file, or an error model when the file has
        # none (or to cross-check them) -- goodness_of_fit.estimate_errors
        err_form = QFormLayout()
        self.error_model_combo = QComboBox()
        for key, text in [("file", "from file (ΔI column)"),
                          ("poly", "estimated: local cubic fits (5 points)"),
                          ("scatter", "estimated: neighbour interpolation (3 points)"),
                          ("poisson", "Poisson: σ² = I (counts)"),
                          ("relative", "relative error")]:
            self.error_model_combo.addItem(text, key)
        self.error_model_combo.currentIndexChanged.connect(lambda _: self._on_error_model_changed())
        err_form.addRow("Error bars:", self.error_model_combo)
        self.error_rel_spin = QDoubleSpinBox()
        self.error_rel_spin.setRange(0.01, 100.0)
        self.error_rel_spin.setDecimals(2)
        self.error_rel_spin.setSuffix(" %")
        self.error_rel_spin.setValue(3.0)
        self.error_rel_spin.valueChanged.connect(lambda _: self._on_error_model_changed())
        self.error_rel_label = QLabel("relative error:")
        err_form.addRow(self.error_rel_label, self.error_rel_spin)
        self.error_window_spin = QSpinBox()
        self.error_window_spin.setRange(1, 51)
        self.error_window_spin.setSingleStep(2)
        self.error_window_spin.setValue(15)
        self.error_window_spin.setSuffix(" points")
        self.error_window_spin.valueChanged.connect(lambda _: self._on_error_model_changed())
        self.error_window_label = QLabel("combined over:")
        err_form.addRow(self.error_window_label, self.error_window_spin)
        file_layout.addLayout(err_form)
        self._help(self.error_model_combo, "error_model", err_form)
        self._help(self.error_rel_spin, "error_rel", err_form)
        self._help(self.error_window_spin, "error_window", err_form)
        self.error_window_label.setVisible(False)
        self.error_window_spin.setVisible(False)
        self.error_rel_label.setVisible(False)
        self.error_rel_spin.setVisible(False)
        layout.addWidget(file_group)
        self._help(self.load_button, "load")
        self._help(self.data_info_label, "data_info")

        # --- background ---
        bg_group = QGroupBox("Background: backgr + c\u00b7q^(-\u03b1)")
        bg_form = QFormLayout(bg_group)

        self.bg_qmin = QDoubleSpinBox()
        self.bg_qmin.setDecimals(6)
        self.bg_qmin.setRange(0, 1e6)
        self.bg_qmax = QDoubleSpinBox()
        self.bg_qmax.setDecimals(6)
        self.bg_qmax.setRange(0, 1e6)
        bg_form.addRow("q min (fit range):", self.bg_qmin)
        bg_form.addRow("q max (fit range):", self.bg_qmax)
        self._help(self.bg_qmin, "bg_qmin", bg_form)
        self._help(self.bg_qmax, "bg_qmax", bg_form)

        self.fit_bg_button = QPushButton("Fit background")
        self.fit_bg_button.clicked.connect(self._on_fit_background)
        self.fit_bg_button.setEnabled(False)
        bg_form.addRow(self.fit_bg_button)
        self._help(self.fit_bg_button, "fit_bg")

        self.bg_fit_label = self._short_label("not fitted (nothing subtracted)")
        bg_form.addRow(self.bg_fit_label)
        self._help(self.bg_fit_label, "bg_result")

        self.use_fixed_bg = QCheckBox("Use fixed value instead")
        self.use_fixed_bg.toggled.connect(self._on_fixed_bg_toggled)
        self.use_fixed_bg.toggled.connect(lambda _: self._update_bg_info())
        bg_form.addRow(self.use_fixed_bg)
        self._help(self.use_fixed_bg, "use_fixed_bg")

        self.fixed_bg_spin = QDoubleSpinBox()
        self.fixed_bg_spin.setDecimals(6)
        self.fixed_bg_spin.setRange(-1e12, 1e12)
        self.fixed_bg_spin.setEnabled(False)
        self.fixed_bg_spin.valueChanged.connect(lambda _: self._update_bg_info(show=False))
        bg_form.addRow("Fixed value:", self.fixed_bg_spin)
        self._help(self.fixed_bg_spin, "fixed_bg", bg_form)

        layout.addWidget(bg_group)

        # --- kernel / grid / solver ---
        solve_group = QGroupBox("Inversion")
        solve_form = QFormLayout(solve_group)

        self.kernel_combo = QComboBox()
        for key, spec in KERNEL_REGISTRY.items():
            self.kernel_combo.addItem(spec.label, userData=key)
        self.kernel_combo.currentIndexChanged.connect(self._on_kernel_changed)
        solve_form.addRow("Kernel:", self.kernel_combo)
        self._help(self.kernel_combo, lambda: f"kernel:{self.kernel_combo.currentData()}", solve_form)
        self.kernel_combo.highlighted.connect(
            lambda i: self._show_help(f"kernel:{self.kernel_combo.itemData(i)}"))

        self.alpha_spin = QDoubleSpinBox()
        self.alpha_spin.setDecimals(2)
        self.alpha_spin.setRange(-20, 20)
        self.alpha_row_label = QLabel("\u03b1 (size-weighting exponent):")
        solve_form.addRow(self.alpha_row_label, self.alpha_spin)
        self._help(self.alpha_spin, "alpha", solve_form)
        self._on_kernel_changed(0)  # populate default alpha for the first kernel

        # r always runs from 0 to r_max -- p(0)=0 is a hard physical
        # requirement for a pair-distance distribution (p(r) = r^2 * the
        # pair-correlation function, so it vanishes quadratically at the
        # origin for ANY particle shape, just like p(Dmax)=0 at the outer
        # end), not a free choice, so there is no separate "r min" control
        # -- only r_max and the point count determine the grid.
        self.r_max_spin = QDoubleSpinBox()
        self.r_max_spin.setRange(0.001, 1e6)
        self.r_max_spin.setValue(500.0)
        self.r_n_spin = QSpinBox()
        self.r_n_spin.setRange(10, 2000)
        self.r_n_spin.setValue(150)
        solve_form.addRow("r max:", self.r_max_spin)
        solve_form.addRow("r grid points:", self.r_n_spin)
        self._help(self.r_max_spin, "r_max", solve_form)
        self._help(self.r_n_spin, "r_n", solve_form)
        # Spacing of the r grid points (point-grid solvers only; with the
        # B-spline option the knot spacing below is used instead).
        self.r_spacing_combo = QComboBox()
        for key, text in [("uniform", "uniform in r"),
                          ("quadratic", "quadratic (denser at small r)"),
                          ("log", "logarithmic (denser at small r)")]:
            self.r_spacing_combo.addItem(text, key)
        self.r_spacing_row_label = QLabel("r grid spacing:")
        solve_form.addRow(self.r_spacing_row_label, self.r_spacing_combo)
        self._help(self.r_spacing_combo, "r_spacing", solve_form)
        # Interpolation of p(r) between the grid points (point-grid solvers).
        self.r_interp_combo = QComboBox()
        for key, text in [("linear", "linear"),
                          ("cubic", "cubic spline (not-a-knot)")]:
            self.r_interp_combo.addItem(text, key)
        self.r_interp_row_label = QLabel("p(r) between grid points:")
        solve_form.addRow(self.r_interp_row_label, self.r_interp_combo)
        self._help(self.r_interp_combo, "r_interp", solve_form)

        self.solver_combo = QComboBox()
        for key, spec in SOLVER_REGISTRY.items():
            self.solver_combo.addItem(spec.label, userData=key)
        self.solver_combo.currentIndexChanged.connect(self._on_solver_changed)
        solve_form.addRow("Solver:", self.solver_combo)
        self._help(self.solver_combo, lambda: f"solver:{self.solver_combo.currentData()}", solve_form)
        self.solver_combo.highlighted.connect(
            lambda i: self._show_help(f"solver:{self.solver_combo.itemData(i)}"))

        # One dropdown per solver option (SolverSpec.options), shown only
        # for the solvers that have it (e.g. EM smoothing choice, MaxEnt
        # prior, Bayesian-evidence smoothness penalty).
        self.option_widgets = {}
        for spec in SOLVER_REGISTRY.values():
            for name, choices in (spec.options or {}).items():
                if name in self.option_widgets:
                    continue
                combo = QComboBox()
                for value, text in choices:
                    combo.addItem(text, value)
                label = QLabel((spec.option_labels or {}).get(name, name + ":"))
                solve_form.addRow(label, combo)
                self.option_widgets[name] = (label, combo)
                self._help(combo, f"option:{name}", solve_form)

        self.target_chi2_check = QCheckBox("Force chi2_r = 1 (discrepancy principle)")
        self.target_chi2_row_label = QLabel("")
        solve_form.addRow(self.target_chi2_row_label, self.target_chi2_check)
        self._help(self.target_chi2_check, "target_chi2")

        # Number of Glatter-style B-spline coefficients -- only meaningful
        # for the two B-spline-basis solvers. Independent of r grid points
        # (r_n_spin): the point grid is only the resolution the basis gets
        # evaluated/returned on, while this controls the basis's own
        # dimensionality (the actual regularization knob for those two
        # solvers, playing the same role lambda plays elsewhere). Left
        # unset (None passed through) auto-derives a default from r_n_spin
        # via solver_registry._spline_n_coeffs_default (~n/4, clipped to the
        # 10-30 literature range) -- this spinbox lets you override that.
        self.spline_check = QCheckBox("Represent p(r) by B-splines (any solver)")
        self.spline_check.toggled.connect(
            lambda _: self._on_solver_changed(self.solver_combo.currentIndex())
        )
        solve_form.addRow("", self.spline_check)
        self._help(self.spline_check, "spline")

        self.spline_n_coeffs_spin = QSpinBox()
        self.spline_n_coeffs_spin.setRange(4, 100)
        self.spline_n_coeffs_spin.setValue(20)
        self.spline_n_coeffs_row_label = QLabel("B-spline coefficients:")
        solve_form.addRow(self.spline_n_coeffs_row_label, self.spline_n_coeffs_spin)
        self._help(self.spline_n_coeffs_spin, "spline_n", solve_form)

        # Knot placement in r for the B-spline solvers. Uniform = Glatter's
        # original choice; quadratic/log put more knots at small r (better
        # for a PDDF that rises steeply and has a long smooth tail, worse
        # when the structure sits at large r, e.g. sphere size distributions).
        self.spline_spacing_combo = QComboBox()
        for key, text in [("uniform", "uniform in r"),
                          ("quadratic", "quadratic (denser at small r)"),
                          ("log", "logarithmic (denser at small r)")]:
            self.spline_spacing_combo.addItem(text, key)
        self.spline_spacing_row_label = QLabel("B-spline knot spacing:")
        solve_form.addRow(self.spline_spacing_row_label, self.spline_spacing_combo)
        self._help(self.spline_spacing_combo, "spline_spacing", solve_form)

        self._on_solver_changed(self.solver_combo.currentIndex())

        self.mc_uncertainty_check = QCheckBox("Estimate p(r) uncertainty (Monte Carlo)")
        self.mc_uncertainty_check.toggled.connect(self._on_mc_uncertainty_toggled)
        solve_form.addRow(self.mc_uncertainty_check)
        self._help(self.mc_uncertainty_check, "mc")
        self.n_boot_spin = QSpinBox()
        self.n_boot_spin.setRange(10, 1000)
        self.n_boot_spin.setValue(50)
        self.n_boot_spin.setSingleStep(10)
        self.n_boot_row_label = QLabel("bootstrap N:")
        solve_form.addRow(self.n_boot_row_label, self.n_boot_spin)
        self._help(self.n_boot_spin, "n_boot", solve_form)
        self._on_mc_uncertainty_toggled(False)

        self.solve_button = QPushButton("Solve")
        self.solve_button.clicked.connect(self._on_solve)
        self.solve_button.setEnabled(False)
        solve_form.addRow(self.solve_button)
        self._help(self.solve_button, "solve")

        self.solve_result_label = self._short_label("not solved yet")
        solve_form.addRow(self.solve_result_label)
        self._help(self.solve_result_label, "result")

        layout.addWidget(solve_group)
        layout.addStretch(1)
        return panel

    def _build_plot_panel(self) -> QWidget:
        panel = QWidget()
        layout = QVBoxLayout(panel)

        self.fig_iq = Figure(figsize=(5, 4))
        self.canvas_iq = FigureCanvasQTAgg(self.fig_iq)
        self.ax_iq = self.fig_iq.add_subplot(111)
        layout.addWidget(self.canvas_iq)

        self.fig_dist = Figure(figsize=(5, 4))
        self.canvas_dist = FigureCanvasQTAgg(self.fig_dist)
        self.ax_dist = self.fig_dist.add_subplot(111)
        layout.addWidget(self.canvas_dist)

        return panel

    # ------------------------------------------------------------ actions

    def _on_load_file(self):
        path, _ = QFileDialog.getOpenFileName(self, "Load SAS data file")
        if path:
            self.load_file(path)

    def load_file(self, path: str):
        """Load a data file and update the UI. Split out from _on_load_file
        so tests (and any future scripting/CLI use) can call it directly
        without going through a file dialog."""
        try:
            self.data = load_sas_data(path)
        except Exception as exc:
            QMessageBox.critical(self, "Load failed", str(exc))
            return

        has_dI = self.data.dI is not None and np.all(self.data.dI > 0)
        model = self.error_model_combo.findData("file" if has_dI else "poly")
        self.error_model_combo.blockSignals(True)
        self.error_model_combo.setCurrentIndex(model)
        self.error_model_combo.blockSignals(False)
        self._update_sigma()
        self.bg_qmin.setValue(self.data.q.min())
        self.bg_qmax.setValue(self.data.q.max())
        self.fit_bg_button.setEnabled(True)
        self.solve_button.setEnabled(True)
        self.bg_result = None

        self._suggest_r_grid()
        self._update_bg_info(show=False)
        q = self.data.q
        self.data_info_label.setText(
            f"{os.path.basename(path)} · {len(q)} points · q {q.min():.3g}–{q.max():.3g}")
        self._set_info("data_info", self._data_details(path))

        self._plot_iq()
        self.status.showMessage(f"Loaded {path}")

    def _suggest_r_grid(self):
        """(Re-)suggest r_max and r grid points from the loaded data and the
        CURRENTLY SELECTED KERNEL. Called on data load and whenever the
        kernel selection changes (switching kernels changes what the r/s
        grid variable physically means -- see below -- so the suggestion
        needs to be recomputed, not just computed once at load time)."""
        if self.data is None:
            return

        # Glatter's consistency condition q_min <= pi/Dmax (equivalently
        # Dmax <= pi/q_min) gives the largest PARTICLE DIAMETER / maximum
        # pairwise distance the data's low-q reach can actually constrain --
        # beyond this Dmax the data doesn't reach low enough in q, and
        # larger-Dmax "improvements" tend to be the solver overfitting
        # rather than resolving real structure (see
        # tests/diagnose_hansen_dmax_scan.py for the empirical confirmation
        # on tests/data/test.dat).
        dmax = float(np.pi / self.data.q.min())

        # Dmax is a DIAMETER/max-pairwise-distance scale. What that means
        # for r_max depends on what the kernel's r/s grid variable actually
        # is:
        #   - sinc_4pi: the grid IS the pair-distance variable r directly
        #     (p(r) has support r in [0, Dmax]) -- r_max = Dmax.
        #   - sphere_rg: the grid is the sphere RADIUS (sphere_rayleigh_gans_
        #     intensity(q, r) takes r as a radius, not a diameter) -- the
        #     largest radius a particle bounded by Dmax can have is Dmax/2,
        #     so r_max = Dmax/2. Using the diameter-scale Dmax directly here
        #     would suggest a radius range twice too large (Joachim,
        #     2026-10-09).
        kernel_key = self.kernel_combo.currentData()
        if kernel_key == "sphere_rg":
            r_max_suggested = dmax / 2.0
            basis_note = "D<sub>max</sub>/2 (radius)"
        else:
            r_max_suggested = dmax
            basis_note = "D<sub>max</sub> (pair distance)"

        self.r_max_spin.setValue(r_max_suggested)
        self._help_dynamic["r_max"] = (
            f"<br><i>Current data: q<sub>min</sub> = {self.data.q.min():.4g}, "
            f"D<sub>max</sub> ≈ π/q<sub>min</sub> ≈ {dmax:.4g}; suggested "
            f"r<sub>max</sub> = {basis_note} ≈ {r_max_suggested:.4g}.</i>"
        )

        # Suggest the r grid point count the same way: from a physically
        # meaningful quantity (the Shannon number of independent parameters
        # I(q) actually encodes over the real-space extent actually being
        # inverted, i.e. this kernel's own r_max), not an arbitrary fixed
        # default. N_s = (q_max - q_min)*r_max/pi (Shannon 1949 applied to
        # IFT, e.g. Svergun 1991/Glatter) is itself the suggested point
        # count -- it's the number of independent parameters the data can
        # actually resolve over that extent, so taking it directly (no
        # oversampling factor) is the natural default. The user can freely
        # override it.
        n_shannon = (self.data.q.max() - self.data.q.min()) * r_max_suggested / np.pi
        suggested_n_r = int(np.clip(
            round(n_shannon), self.r_n_spin.minimum(), self.r_n_spin.maximum()
        ))
        self.r_n_spin.setValue(suggested_n_r)
        self._help_dynamic["r_n"] = (
            f"<br><i>Current data: N<sub>s</sub> = (q<sub>max</sub> − "
            f"q<sub>min</sub>)·r<sub>max</sub>/π ≈ {n_shannon:.3g} → "
            f"{suggested_n_r} points; π/q<sub>max</sub> = "
            f"{np.pi / self.data.q.max():.3g}.</i>"
        )
        if hasattr(self, "info_panel"):
            self._refresh_help_sizes()

    def _on_fixed_bg_toggled(self, checked: bool):
        self.fixed_bg_spin.setEnabled(checked)
        self.fit_bg_button.setEnabled(not checked and self.data is not None)

    def _on_kernel_changed(self, index: int):
        key = self.kernel_combo.itemData(index)
        if key is not None:
            spec = KERNEL_REGISTRY[key]
            self.alpha_spin.setValue(spec.default_alpha if spec.uses_alpha else 0.0)
            # no size weighting for kernels whose unknown is not a size
            # distribution (j0 / PDDF): hide the alpha field
            self.alpha_row_label.setVisible(spec.uses_alpha)
            self.alpha_spin.setVisible(spec.uses_alpha)
        # Re-suggest r_max/r grid points too -- the sphere kernel's r_max
        # (a radius) differs from the PDDF kernel's (a pair-distance) by a
        # factor of 2 for the same data, see _suggest_r_grid. No-op before
        # any data is loaded.
        self._suggest_r_grid()

    # Solver keys that accept target_chi2_r (all Bayesian-evidence-family
    # solvers -- the three original ones plus the two new region-adaptive/
    # B-spline ones, which reuse the same _bayesian_evidence_pick_lambda/
    # discrepancy-principle machinery). Kept as one place so the checkbox's
    # visibility and _on_solve's kwarg-building can't drift apart.
    _TARGET_CHI2_SOLVERS = frozenset({"bayesian_evidence"})
    _SPLINE_SOLVERS = frozenset()  # solvers with a built-in spline basis (none listed now)
    # Multiplicative EM/MaxEnt updates assume a non-negative kernel. With a
    # signed kernel (e.g. 4*pi*j0(qr), negative for qr > pi) they diverge
    # (chi2_r ~1e6..1e10 on test.dat); use em_general_signed_kernel or a
    # Bayesian-evidence solver instead.
    _POSITIVE_KERNEL_SOLVERS = frozenset({"em_discrepancy", "maxent_em"})
    # Solvers that need p >= 0 (or a non-negative kernel): cubic
    # interpolation between grid points would overshoot -> linear forced.
    _LINEAR_ONLY_SOLVERS = frozenset({"em_discrepancy", "maxent_em", "hansen_maxent", "arlsnn"})

    def _on_solver_changed(self, index: int):
        key = self.solver_combo.itemData(index)
        opts = SOLVER_REGISTRY[key].options or {}
        for name, (label, combo) in self.option_widgets.items():
            label.setVisible(name in opts)
            combo.setVisible(name in opts)
        supports_target_chi2 = key in self._TARGET_CHI2_SOLVERS
        self.target_chi2_row_label.setVisible(supports_target_chi2)
        self.target_chi2_check.setVisible(supports_target_chi2)
        native = key in self._SPLINE_SOLVERS
        if hasattr(self, "spline_check"):
            self.spline_check.setEnabled(not native)
        is_spline = native or (hasattr(self, "spline_check") and self.spline_check.isChecked())
        if hasattr(self, "r_spacing_combo"):
            self.r_spacing_row_label.setVisible(not is_spline)
            self.r_spacing_combo.setVisible(not is_spline)
        if hasattr(self, "r_interp_combo"):
            self.r_interp_row_label.setVisible(not is_spline)
            self.r_interp_combo.setVisible(not is_spline)
            self.r_interp_combo.setEnabled(key not in self._LINEAR_ONLY_SOLVERS)
        self.spline_n_coeffs_row_label.setVisible(is_spline)
        self.spline_n_coeffs_spin.setVisible(is_spline)
        self.spline_spacing_row_label.setVisible(is_spline)
        self.spline_spacing_combo.setVisible(is_spline)

    def _on_mc_uncertainty_toggled(self, checked: bool):
        self.n_boot_row_label.setVisible(checked)
        self.n_boot_spin.setVisible(checked)

    def _on_fit_background(self):
        if self.data is None:
            return
        try:
            self.bg_result = fit_background(
                self.data.q, self.data.I,
                q_min=self.bg_qmin.value(), q_max=self.bg_qmax.value(),
                dI=self.sigma,
            )
        except Exception as exc:
            QMessageBox.critical(self, "Background fit failed", str(exc))
            return

        self._update_bg_info()
        self._plot_iq()

    def _get_background_curve(self, q: np.ndarray) -> np.ndarray:
        if self.use_fixed_bg.isChecked():
            return np.full_like(q, self.fixed_bg_spin.value(), dtype=float)
        if self.bg_result is not None:
            return self.bg_result.evaluate(q)
        return np.zeros_like(q, dtype=float)

    def _on_solve(self):
        if self.data is None:
            return
        try:
            I_sub = self.data.I - self._get_background_curve(self.data.q)

            kernel_key = self.kernel_combo.currentData()
            kernel_func = KERNEL_REGISTRY[kernel_key].func
            alpha = self.alpha_spin.value() if KERNEL_REGISTRY[kernel_key].uses_alpha else 0.0

            # r always starts at 0 -- see the comment where r_max_spin is
            # built for why p(0)=0 isn't an optional/adjustable choice.
            use_spline = (self.solver_combo.currentData() in self._SPLINE_SOLVERS
                          or self.spline_check.isChecked())
            r_spacing = "uniform" if use_spline else self.r_spacing_combo.currentData()
            r_grid = make_r_grid(self.r_max_spin.value(), self.r_n_spin.value(), r_spacing)
            dr_max = float(np.max(np.diff(r_grid)))
            r_interp = self.r_interp_combo.currentData()
            if self.solver_combo.currentData() in self._LINEAR_ONLY_SOLVERS or len(r_grid) < 4:
                r_interp = "linear"
            fine_nodes = (not use_spline) and (
                r_spacing != "uniform" or r_interp != "linear"
                or dr_max > 1.1 * np.pi / self.data.q.max())
            if fine_nodes:
                # p(r) linear or cubic between the grid points, kernel
                # integrated on a fine grid (see kernels.build_kernel_on_nodes)
                A = build_kernel_on_nodes(self.data.q, r_grid, kernel_func, alpha=alpha,
                                          interpolation=r_interp)
            else:
                A = build_size_distribution_kernel(self.data.q, r_grid, kernel_func, alpha=alpha)

            solver_key = self.solver_combo.currentData()
            solver_kwargs = {}
            for name in (SOLVER_REGISTRY[solver_key].options or {}):
                solver_kwargs[name] = self.option_widgets[name][1].currentData()
            if solver_key in self._TARGET_CHI2_SOLVERS and self.target_chi2_check.isChecked():
                solver_kwargs["target_chi2_r"] = 1.0
            spline = None
            if solver_key in self._SPLINE_SOLVERS or self.spline_check.isChecked():
                # Integrate the spline basis on a 4x finer r grid (same
                # [0, r_max], same kernel and alpha) -- knot spans near r=0
                # can be shorter than the display grid spacing.
                # At least 4x the r grid AND at least 4 points per Shannon
                # interval pi/q_max, so the kernel's fastest oscillation is
                # resolved however few r grid points are chosen.
                n_quad = quadrature_points(self.r_max_spin.value(), self.data.q.max(),
                                           self.r_n_spin.value())
                r_quad = np.linspace(0.0, self.r_max_spin.value(), n_quad)
                spline = SplineOptions(
                    n_coeffs=self.spline_n_coeffs_spin.value(),
                    spacing=self.spline_spacing_combo.currentData(),
                    A_quad=build_size_distribution_kernel(
                        self.data.q, r_quad, kernel_func, alpha=alpha
                    ),
                )
            t_start = time.perf_counter()
            result = run_solver(solver_key, A, I_sub, self.sigma, spline=spline, **solver_kwargs)
        except Exception:
            QMessageBox.critical(self, "Solve failed", traceback.format_exc())
            return

        self._last_r_grid = r_grid
        self._last_alpha = alpha
        self._last_kernel = kernel_key
        self._last_result = result
        self._last_I_sub = I_sub
        self._last_boot = None

        chi2_final = result.chi2_r_history[-1] if result.chi2_r_history else float("nan")
        if fine_nodes:
            representation = (
                f"{r_spacing} r grid, {len(r_grid)} points (step {np.min(np.diff(r_grid)):.3g}"
                f"–{dr_max:.3g}); p(r) {'piecewise linear' if r_interp == 'linear' else 'cubic spline'} "
                f"between points, kernel integrated on "
                f"{quadrature_points(r_grid[-1], self.data.q.max(), len(r_grid))} points")
        elif spline is not None:
            representation = (f"{spline.n_coeffs} cubic B-splines, {spline.spacing} knots, "
                              f"shown on {len(r_grid)} points")
        else:
            representation = (f"uniform r grid, {len(r_grid)} points (step {dr_max:.3g}), "
                              f"kernel evaluated at the points")
        warnings = []
        if solver_key in self._POSITIVE_KERNEL_SOLVERS and np.any(A < 0):
            warnings.append(
                "this solver needs a non-negative kernel, but the selected kernel "
                "has negative entries -- the result is not meaningful. Use the "
                "signed-kernel EM or the Bayesian-evidence solver.")
        if "not reachable" in result.diagnostics.get("lambda_selection", ""):
            warnings.append("χ²<sub>r</sub> = 1 was not reachable (see regularization).")
        self._last_result_info = dict(
            solver_key=solver_key, solver_kwargs=solver_kwargs, representation=representation,
            warnings=warnings, elapsed=time.perf_counter() - t_start, kernel_key=kernel_key,
            alpha=alpha)
        self._update_result_info()
        self.status.showMessage(f"Solved: chi2_r={chi2_final:.4g}")

        if self.mc_uncertainty_check.isChecked():
            n_boot = self.n_boot_spin.value()
            self.status.showMessage(
                f"Running Monte Carlo uncertainty estimate ({n_boot} re-solves, "
                "this can take a while)..."
            )
            QApplication.processEvents()
            try:
                def solve_fn(b_rep):
                    return run_solver(solver_key, A, b_rep, self.sigma, spline=spline, **solver_kwargs)
                boot = bootstrap_uncertainty(
                    solve_fn, I_sub, self.sigma, n_boot=n_boot,
                    center=result.fitted_b,
                )
                self._last_boot = boot
                self._update_result_info()
                self.status.showMessage(
                    f"Solved: chi2_r={chi2_final:.4g}  (bootstrap: {boot.n_boot} ok, "
                    f"{boot.n_failed} failed)"
                )
            except Exception as exc:
                QMessageBox.warning(
                    self, "Bootstrap uncertainty failed",
                    f"Single solve above still succeeded and is shown; the Monte "
                    f"Carlo uncertainty estimate itself failed:\n{exc}"
                )

        self._plot_iq()
        self._plot_distribution()

    # -------------------------------------------------- info panel details

    def _update_sigma(self) -> None:
        """Effective uncertainties self.sigma from the selected error model."""
        model = self.error_model_combo.currentData()
        rel = model == "relative"
        self.error_rel_label.setVisible(rel)
        self.error_rel_spin.setVisible(rel)
        est = model in ("poly", "scatter")
        self.error_window_label.setVisible(est)
        self.error_window_spin.setVisible(est)
        if self.data is None:
            self.sigma = None
            return
        has_dI = self.data.dI is not None and np.all(self.data.dI > 0)
        if model == "file" and not has_dI:
            QMessageBox.information(self, "No error bars",
                                    "This file has no (positive) ΔI column; using the "
                                    "estimate from local cubic fits instead.")
            self.error_model_combo.blockSignals(True)
            self.error_model_combo.setCurrentIndex(self.error_model_combo.findData("poly"))
            self.error_model_combo.blockSignals(False)
            model = "poly"
        if model == "file":
            self.sigma = np.asarray(self.data.dI, float)
        else:
            self.sigma = estimate_errors(self.data.q, self.data.I, model,
                                         relative=self.error_rel_spin.value() / 100.0,
                                         window=self.error_window_spin.value())

    def _on_error_model_changed(self) -> None:
        self._update_sigma()
        if self.data is None:
            return
        self._set_info("data_info", self._data_details(self.data.source_path))
        if self.bg_result is not None or self.use_fixed_bg.isChecked():
            self._update_bg_info(show=False)
        self._plot_iq()

    def _data_details(self, path: str) -> str:
        d = self.data
        q, I, dI = d.q, d.I, d.dI
        rows = [("file", os.path.basename(path)),
                ("folder", os.path.dirname(path) or "."),
                ("points", f"{len(q)} (skipped {d.n_lines_skipped} header/comment lines)"),
                ("q range", f"{q.min():.4g} – {q.max():.4g}")]
        if len(q) > 2 and np.all(q > 0):
            dl, dq = np.diff(np.log10(q)), np.diff(q)
            cv_log = np.std(dl) / abs(np.mean(dl))
            cv_lin = np.std(dq) / abs(np.mean(dq))
            if cv_log < cv_lin:
                rows.append(("q spacing", f"{'' if cv_log < 0.1 else 'roughly '}logarithmic, "
                                          f"{(len(q) - 1) / np.log10(q.max() / q.min()):.3g} points per decade"))
            else:
                rows.append(("q spacing", f"{'' if cv_lin < 0.1 else 'roughly '}linear, "
                                          f"Δq ≈ {np.mean(dq):.3g}"))
        rows.append(("I range", f"{I.min():.4g} – {I.max():.4g}"))
        model = self.error_model_combo.currentData()
        rows.append(("error bars", self.error_model_combo.currentText()
                     + (f" {self.error_rel_spin.value():g} %" if model == "relative" else "")
                     + (f", combined over {self.error_window_spin.value()} points"
                        if model in ("poly", "scatter") else "")))
        if dI is not None and np.all(dI > 0) and len(q) >= 5:
            ratio = estimate_errors(q, I, "poly", window=self.error_window_spin.value()) / dI
            rows.append(("cubic-fit estimate / file ΔI",
                         f"median {np.median(ratio):.2f} (low/mid/high q "
                         + " / ".join(f"{np.median(t):.2f}" for t in np.array_split(ratio, 3))
                         + ")"))
        if model != "file" and self.sigma is not None:
            srel = self.sigma / np.maximum(np.abs(I), 1e-300)
            rows.append(("σ/I used", f"{srel.min():.2%} – {srel.max():.2%} "
                                     f"(median {np.median(srel):.2%})"))
        if dI is not None:
            rel = np.abs(dI / np.where(I != 0, I, np.nan))
            rows.append(("ΔI range", f"{dI.min():.3g} – {dI.max():.3g}"))
            rows.append(("ΔI/I", f"{np.nanmin(rel):.2%} – {np.nanmax(rel):.2%} "
                                 f"(median {np.nanmedian(rel):.2%})"))
        else:
            rows.append("<i>no ΔI column in the file: error bars come from the "
                        "selected error model</i>")
        dmax = np.pi / q.min()
        n_s = (q.max() - q.min()) * dmax / np.pi
        q_hi = q[q >= q.min() + 2 / 3 * (q.max() - q.min())]
        ch_hi = (q.max() - q_hi.min()) * dmax / np.pi if len(q_hi) > 1 else np.nan
        rows += [("D<sub>max</sub> ≈ π/q<sub>min</sub>", f"{dmax:.4g}"),
                 ("resolution π/q<sub>max</sub>", f"{np.pi / q.max():.3g}"),
                 ("Shannon number N<sub>s</sub>", f"{n_s:.3g}")]
        if np.isfinite(ch_hi) and ch_hi > 0:
            rows.append(("points per Shannon channel",
                         f"{len(q) / n_s:.2g} overall, {len(q_hi) / ch_hi:.2g} in the "
                         f"upper third of the q range"))
            if len(q_hi) / ch_hi < 1:
                rows.append("<i>fewer than one point per Shannon channel at high q: a point "
                            "grid with N ≈ N<sub>s</sub> can follow the noise there; consider "
                            "B-splines.</i>")
        return self._table(rows)

    def _update_bg_info(self, show: bool = True) -> None:
        """Short background line + detailed info panel text."""
        if self.use_fixed_bg.isChecked():
            v = self.fixed_bg_spin.value()
            self.bg_fit_label.setText(f"fixed backgr = {v:.4g} (subtracted)")
            rows = [("mode", "fixed value"), ("subtracted", f"{v:.6g}")]
            if self.data is not None:
                rows.append(("I(q<sub>max</sub>) − backgr",
                             f"{self.data.I[np.argmax(self.data.q)] - v:.4g}"))
            self._set_info("bg_result", self._table(rows), show)
            return
        r = self.bg_result
        if r is None or self.data is None:
            self.bg_fit_label.setText("not fitted (nothing subtracted)")
            self._set_info("bg_result", help_texts.NO_BACKGROUND, show)
            return
        q, I, dI = self.data.q, self.data.I, self.sigma
        m = (q >= r.q_min) & (q <= r.q_max)
        model = background_model(q[m], r.backgr, r.c, r.alpha)
        chi2_bg = (float(np.mean(((I[m] - model) / dI[m]) ** 2))
                   if dI is not None and m.sum() > 3 else float("nan"))
        e = r.param_errors
        warnings = []
        risk = r.extrapolation_risk(q)
        if e[0] > 10 * max(abs(r.backgr), 1e-12):
            warnings.append("backgr and c are nearly degenerate (individually poorly "
                            "determined); their sum may still be fine.")
        i_hi = I[np.argmax(q)]
        rows = [("model", "backgr + c·q<sup>−α</sup>, weighted least squares"),
                ("fit range", f"{r.q_min:.4g} – {r.q_max:.4g} ({int(m.sum())} points)"),
                ("backgr", f"{r.backgr:.5g} ± {e[0]:.2g}"),
                ("c", f"{r.c:.5g} ± {e[1]:.2g}"),
                ("α", f"{r.alpha:.4g} ± {e[2]:.2g}"),
                ("χ²<sub>r</sub> of the fit", f"{chi2_bg:.3g}"),
                ("subtracted", f"backgr only = {r.backgr:.5g} "
                               f"({r.backgr / i_hi:.0%} of I at q<sub>max</sub>)")]
        if risk > 1.0:
            rows.append(("extrapolation", f"data extend {risk:.2g} fit-range widths (in log q) "
                                          f"outside the fit range; the power law would be "
                                          f"unreliable there, but only the constant is subtracted"))
        rows += [f"⚠ {w}" for w in warnings]
        self.bg_fit_label.setText(f"backgr = {r.backgr:.4g} ± {e[0]:.2g}"
                                  + ("  ⚠" if warnings else ""))
        self._set_info("bg_result", self._table(rows), show)

    def _update_result_info(self) -> None:
        """Short result line + detailed info panel text for the last solve."""
        res, info = self._last_result, self._last_result_info
        q, b, dI = self.data.q, self._last_I_sub, self.sigma
        chi2 = res.chi2_r_history[-1] if res.chi2_r_history else float("nan")
        z = ((res.fitted_b - b) / dI) ** 2
        thirds = np.array_split(np.arange(len(q)), 3)
        spec = SOLVER_REGISTRY[info["solver_key"]]
        opts = ", ".join(self.option_widgets[k][1].currentText()
                         for k in (spec.options or {}))
        diag = res.diagnostics
        bg = self._get_background_curve(q)
        st = fit_statistics(self.data.I, res.fitted_b + bg, dI)
        rows = [("χ²<sub>r</sub>", f"{chi2:.4g}"
                 + ("" if self.error_model_combo.currentData() == "file"
                    else f" (errors: {self.error_model_combo.currentText()})")),
                ("χ² per q third", " / ".join(f"{z[t].mean():.3g}" for t in thirds)),
                ("Poisson statistics", f"Pearson χ²/M = {st['pearson_r']:.3g}, "
                                       f"deviance (G)/M = {st['deviance_r']:.3g} "
                                       f"<i>(≈ 1 only if I is in counts)</i>"),
                ("KL divergence", f"{st['kl']:.3g} (normalized curves, scale-free; "
                                  f"SASfit G-test = {2 * st['kl']:.3g})")]
        if diag.get("noise_scale") is not None:
            rows.append(("error scale (evidence)", f"s = {diag['noise_scale']:.3g}: "
                                                   f"errors used = s·σ"))
        rows += [
                ("solver", spec.label + (f" ({opts})" if opts else "")),
                ("regularization", diag.get("lambda_selection", "") or "–")]
        if info["solver_kwargs"].get("target_chi2_r"):
            rows.append(("target", "χ²<sub>r</sub> = 1 forced"))
        rows += [("representation", info["representation"]),
                 ("kernel", KERNEL_REGISTRY[info["kernel_key"]].label
                  + (f", α = {info['alpha']:g}" if KERNEL_REGISTRY[info["kernel_key"]].uses_alpha else "")),
                 ("iterations", (f"{res.n_iterations}, converged: {'yes' if res.converged else 'NO'}"
                                 if res.n_iterations else "direct solution (no iterations)")),
                 ("time", f"{info['elapsed']:.2f} s")]
        r, x = self._last_r_grid, np.asarray(res.x, float)
        kspec = KERNEL_REGISTRY[info["kernel_key"]]
        if not kspec.uses_alpha:
            norm = _trapz(x, r)
            if norm > 0:
                rg = np.sqrt(max(_trapz(x * r ** 2, r) / (2 * norm), 0.0))
                rows.append(("from p(r)", f"I(0) = 4π∫p dr = {4 * np.pi * norm:.4g}, "
                                           f"R<sub>g</sub> = {rg:.4g}, maximum at r = {r[np.argmax(x)]:.4g}"))
        else:
            pos = r > 0
            rows.append(("maximum of x(r)", f"r = {r[np.argmax(x)]:.4g}"))
            a = info["alpha"]
            if pos.any():
                n_r = np.where(pos, x / np.where(pos, r, 1.0) ** a, 0.0)
                if _trapz(n_r, r) > 0:
                    rows.append(("number-weighted mean", f"⟨R⟩ = {_trapz(n_r * r, r) / _trapz(n_r, r):.4g}"))
                v_r = n_r * r ** 3
                rows.append(("volume-weighted maximum", f"R = {r[np.argmax(v_r)]:.4g}"))
        if diag.get("x_sigma") is not None:
            rows.append(("uncertainty", "analytic (Laplace) ±1σ band shown"))
        boot = getattr(self, "_last_boot", None)
        if boot is not None:
            rows.append(("bootstrap", f"{boot.n_boot} replicates ok, {boot.n_failed} failed; "
                                      f"max σ = {np.max(boot.x_std):.3g}"))
        rows += [f"⚠ {w}" for w in info["warnings"]]
        short = f"χ²ᵣ = {chi2:.4g}"
        if boot is not None:
            short += f" · bootstrap n = {boot.n_boot}"
        if info["warnings"]:
            short += "  ⚠"
        self.solve_result_label.setText(short)
        self._set_info("result", self._table(rows))

    # -------------------------------------------------------------- plots

    def _plot_iq(self):
        self.ax_iq.clear()
        if self.data is not None:
            self.ax_iq.errorbar(
                self.data.q, self.data.I, yerr=self.sigma,
                fmt=".", ms=3, alpha=0.5, label="data", color="black",
            )
            bg_curve = self._get_background_curve(self.data.q)
            if np.any(bg_curve):
                self.ax_iq.plot(self.data.q, bg_curve, "--", label="background", color="tab:orange")
            if hasattr(self, "_last_result"):
                fitted_total = self._last_result.fitted_b + bg_curve
                self.ax_iq.plot(self.data.q, fitted_total, "-", label="fit", color="tab:red")

        self.ax_iq.set_xscale("log")
        self.ax_iq.set_yscale("log")
        self.ax_iq.set_xlabel("q")
        self.ax_iq.set_ylabel("I(q)")
        self.ax_iq.legend(fontsize=8)
        self.fig_iq.tight_layout()
        self.canvas_iq.draw_idle()

    def _plot_distribution(self):
        self.ax_dist.clear()
        self.ax_dist.axhline(0.0, **ZERO_LINE_STYLE)
        if hasattr(self, "_last_result"):
            r = self._last_r_grid
            x = self._last_result.x

            boot = getattr(self, "_last_boot", None)
            if boot is not None:
                # Monte Carlo bootstrap: plot the ENSEMBLE MEAN (the actual
                # "average" over all re-solves, which for a nonlinear/
                # positivity-constrained solver need not equal the single
                # solve above) with a shaded +/-1 std band, and the single
                # solve as a thin dashed overlay for comparison.
                self.ax_dist.fill_between(
                    r, boot.x_mean - boot.x_std, boot.x_mean + boot.x_std,
                    color="tab:blue", alpha=0.25,
                    label=f"+/-1 sigma (bootstrap, n={boot.n_boot})",
                )
                self.ax_dist.plot(r, boot.x_mean, "-", color="tab:blue", label="bootstrap mean")
                self.ax_dist.plot(r, x, "--", color="tab:red", linewidth=1, label="single solve")
                self.ax_dist.legend(fontsize=8)
            else:
                x_sigma = self._last_result.diagnostics.get("x_sigma")
                if x_sigma is not None:
                    # Analytic Laplace/Hessian error band around the single
                    # MAP solution (Bayesian-evidence solvers only -- see
                    # bayesian_evidence.posterior_covariance).
                    self.ax_dist.fill_between(
                        r, x - x_sigma, x + x_sigma, color="tab:blue", alpha=0.25,
                        label="+/-1 sigma (analytic, Laplace/Hessian)",
                    )
                    self.ax_dist.plot(r, x, "-", color="tab:blue", label="MAP solution")
                    self.ax_dist.legend(fontsize=8)
                else:
                    self.ax_dist.plot(r, x, "-", color="tab:blue")

            self.ax_dist.set_xlabel("r")
            kspec = KERNEL_REGISTRY.get(getattr(self, "_last_kernel", None))
            if kspec is not None and not kspec.uses_alpha:
                self.ax_dist.set_ylabel(kspec.result_label)
            else:
                self.ax_dist.set_ylabel(f"x(r) = N(r)\u00b7r^{self._last_alpha:g}")
        self.fig_dist.tight_layout()
        self.canvas_dist.draw_idle()


def main():
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
