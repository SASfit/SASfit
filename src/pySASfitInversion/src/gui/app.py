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
)
from PySide6.QtCore import Qt
from matplotlib.backends.backend_qtagg import FigureCanvasQTAgg
from matplotlib.figure import Figure

from sasfit_inversion.io_utils import load_sas_data, SASData
from sasfit_inversion.background import fit_background, BackgroundFitResult
from sasfit_inversion.kernel_registry import KERNEL_REGISTRY
from sasfit_inversion.kernels import build_size_distribution_kernel
from sasfit_inversion.solver_registry import SOLVER_REGISTRY, SplineOptions, run_solver
from sasfit_inversion.uncertainty import bootstrap_uncertainty


# Guide line at x(r) = 0 in the distribution plot. Set explicitly (not taken
# from matplotlib's rcParams) so it stays a thin dotted gray line whatever
# style defaults are active; change it here, e.g. to {"linestyle": "-",
# "linewidth": 0.5, ...} for a thin solid line. zorder=0 keeps it behind the
# curves and error bands; it has no label, so it never appears in the legend.
ZERO_LINE_STYLE = {"color": "0.5", "linestyle": ":", "linewidth": 0.8, "zorder": 0}


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("pySASfitInversion")
        self.resize(1200, 800)

        self.data: SASData | None = None
        self.bg_result: BackgroundFitResult | None = None
        self.fixed_bg_value: float | None = None

        splitter = QSplitter(Qt.Horizontal)
        # The control panel keeps growing (kernel/solver/uncertainty options,
        # hint labels, ...) and was clipping at normal window heights --
        # wrap it in a scroll area so everything stays reachable instead of
        # being cut off at the bottom.
        control_scroll = QScrollArea()
        control_scroll.setWidgetResizable(True)
        control_scroll.setWidget(self._build_control_panel())
        splitter.addWidget(control_scroll)
        splitter.addWidget(self._build_plot_panel())
        splitter.setStretchFactor(1, 1)
        self.setCentralWidget(splitter)

        self.status = QStatusBar()
        self.setStatusBar(self.status)

    # ------------------------------------------------------------------ UI

    def _build_control_panel(self) -> QWidget:
        panel = QWidget()
        layout = QVBoxLayout(panel)

        # --- file loading ---
        file_group = QGroupBox("Data")
        file_layout = QVBoxLayout(file_group)
        self.load_button = QPushButton("Load data file...")
        self.load_button.clicked.connect(self._on_load_file)
        self.data_info_label = QLabel("No data loaded.")
        self.data_info_label.setWordWrap(True)
        file_layout.addWidget(self.load_button)
        file_layout.addWidget(self.data_info_label)
        layout.addWidget(file_group)

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

        self.fit_bg_button = QPushButton("Fit background")
        self.fit_bg_button.clicked.connect(self._on_fit_background)
        self.fit_bg_button.setEnabled(False)
        bg_form.addRow(self.fit_bg_button)

        self.bg_fit_label = QLabel("(not fit yet)")
        self.bg_fit_label.setWordWrap(True)
        bg_form.addRow(self.bg_fit_label)

        self.use_fixed_bg = QCheckBox("Use fixed value instead")
        self.use_fixed_bg.toggled.connect(self._on_fixed_bg_toggled)
        bg_form.addRow(self.use_fixed_bg)

        self.fixed_bg_spin = QDoubleSpinBox()
        self.fixed_bg_spin.setDecimals(6)
        self.fixed_bg_spin.setRange(-1e12, 1e12)
        self.fixed_bg_spin.setEnabled(False)
        bg_form.addRow("Fixed value:", self.fixed_bg_spin)

        layout.addWidget(bg_group)

        # --- kernel / grid / solver ---
        solve_group = QGroupBox("Inversion")
        solve_form = QFormLayout(solve_group)

        self.kernel_combo = QComboBox()
        for key, spec in KERNEL_REGISTRY.items():
            self.kernel_combo.addItem(spec.label, userData=key)
        self.kernel_combo.currentIndexChanged.connect(self._on_kernel_changed)
        solve_form.addRow("Kernel:", self.kernel_combo)

        self.alpha_spin = QDoubleSpinBox()
        self.alpha_spin.setDecimals(2)
        self.alpha_spin.setRange(-20, 20)
        solve_form.addRow("\u03b1 (size-weighting exponent):", self.alpha_spin)
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
        self.r_max_spin.setToolTip(
            "On data load (and whenever the kernel changes), this is suggested "
            "from pi/q_min (the largest Dmax -- max particle diameter/pairwise "
            "distance -- the data's low-q reach can actually constrain, Glatter's "
            "consistency condition q_min <= pi/Dmax). For the sphere kernel, "
            "whose r argument is a RADIUS (not a diameter), the suggestion is "
            "halved (r_max = Dmax/2) accordingly -- see _suggest_r_grid. Feel "
            "free to override it. The r grid always starts at r=0 (see the r "
            "grid points row)."
        )
        self.r_n_spin = QSpinBox()
        self.r_n_spin.setRange(10, 2000)
        self.r_n_spin.setValue(150)
        self.r_n_spin.setToolTip(
            "On data load, this is suggested as the Shannon number of "
            "independent parameters the q-range/Dmax actually constrain "
            "(N_s = (q_max - q_min)*Dmax/pi, Shannon 1949 applied to IFT). "
            "More points than that don't add real resolution -- they just "
            "interpolate and make the kernel matrix more rank-deficient. "
            "Feel free to override it."
        )
        solve_form.addRow("r max:", self.r_max_spin)
        self.r_max_hint_label = QLabel("")
        self.r_max_hint_label.setWordWrap(True)
        solve_form.addRow("", self.r_max_hint_label)
        solve_form.addRow("r grid points:", self.r_n_spin)
        self.r_n_hint_label = QLabel("")
        self.r_n_hint_label.setWordWrap(True)
        solve_form.addRow("", self.r_n_hint_label)

        self.solver_combo = QComboBox()
        for key, spec in SOLVER_REGISTRY.items():
            self.solver_combo.addItem(spec.label, userData=key)
        self.solver_combo.currentIndexChanged.connect(self._on_solver_changed)
        solve_form.addRow("Solver:", self.solver_combo)

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

        self.target_chi2_check = QCheckBox("Force chi2_r = 1 (discrepancy principle)")
        self.target_chi2_check.setToolTip(
            "Bayesian-evidence solver: by default lambda maximizes the Bayesian "
            "evidence (Vestergaard & Hansen 2006), which does not target "
            "chi2_r = 1. Checking this raises (or lowers, by at most 1000x) "
            "lambda until chi2_r = 1; if that is not reachable the closest fit "
            "is returned with a warning."
        )
        self.target_chi2_row_label = QLabel("")
        solve_form.addRow(self.target_chi2_row_label, self.target_chi2_check)

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
        self.spline_check.setToolTip(
            "Solve for cubic B-spline coefficients instead of point values "
            "(Glatter-style). Works with every solver: the solver runs on the "
            "kernel integrated over the spline basis, and p(r) is evaluated on "
            "the r grid afterwards. Limits the free parameters, which stops "
            "the fit from following the noise at high q for log-spaced data. "
            "The two dedicated B-spline solvers always use the basis."
        )
        self.spline_check.toggled.connect(
            lambda _: self._on_solver_changed(self.solver_combo.currentIndex())
        )
        solve_form.addRow("", self.spline_check)

        self.spline_n_coeffs_spin = QSpinBox()
        self.spline_n_coeffs_spin.setRange(4, 100)
        self.spline_n_coeffs_spin.setValue(20)
        self.spline_n_coeffs_spin.setToolTip(
            "Number of cubic B-spline coefficients representing p(r) (Glatter "
            "1977/GNOM-style IFT) -- the actual regularization knob for the two "
            "B-spline-basis solvers below, independent of 'r grid points' (which "
            "only sets the resolution the result is evaluated/returned on). Fewer "
            "coefficients = smoother/stiffer (more resistant to high-q ringing); "
            "more = closer to the full point-grid's flexibility. Typical "
            "literature range ~15-30."
        )
        self.spline_n_coeffs_row_label = QLabel("B-spline coefficients:")
        solve_form.addRow(self.spline_n_coeffs_row_label, self.spline_n_coeffs_spin)

        # Knot placement in r for the B-spline solvers. Uniform = Glatter's
        # original choice; quadratic/log put more knots at small r (better
        # for a PDDF that rises steeply and has a long smooth tail, worse
        # when the structure sits at large r, e.g. sphere size distributions).
        self.spline_spacing_combo = QComboBox()
        for key, text in [("uniform", "uniform in r"),
                          ("quadratic", "quadratic (denser at small r)"),
                          ("log", "logarithmic (denser at small r)")]:
            self.spline_spacing_combo.addItem(text, key)
        self.spline_spacing_combo.setToolTip(
            "Knot positions of the B-spline basis along r. On test.dat (PDDF) "
            "log/quadratic gave chi2_r~1 with balanced low/mid/high-q residuals "
            "at 20 coefficients, uniform needed ~25. For sphere size "
            "distributions log knots shifted the peak (too few knots at large r). "
            "The kernel is integrated on a 4x finer r grid for the spline solvers."
        )
        self.spline_spacing_row_label = QLabel("B-spline knot spacing:")
        solve_form.addRow(self.spline_spacing_row_label, self.spline_spacing_combo)

        self._on_solver_changed(self.solver_combo.currentIndex())

        self.mc_uncertainty_check = QCheckBox("Estimate p(r) uncertainty (Monte Carlo)")
        self.mc_uncertainty_check.setToolTip(
            "Parametric bootstrap (uncertainty.bootstrap_uncertainty): re-solves "
            "N times on the data perturbed by its own dI, then plots the "
            "pointwise mean +/- std across all N recovered curves -- works for "
            "every solver (not just the Bayesian-evidence ones), but costs "
            "roughly N x the time of a single solve. The Bayesian-evidence "
            "solvers additionally get a near-free analytic (Laplace/Hessian) "
            "error band shown automatically regardless of this checkbox."
        )
        self.mc_uncertainty_check.toggled.connect(self._on_mc_uncertainty_toggled)
        solve_form.addRow(self.mc_uncertainty_check)
        self.n_boot_spin = QSpinBox()
        self.n_boot_spin.setRange(10, 1000)
        self.n_boot_spin.setValue(50)
        self.n_boot_spin.setSingleStep(10)
        self.n_boot_row_label = QLabel("bootstrap N:")
        solve_form.addRow(self.n_boot_row_label, self.n_boot_spin)
        self._on_mc_uncertainty_toggled(False)

        self.solve_button = QPushButton("Solve")
        self.solve_button.clicked.connect(self._on_solve)
        self.solve_button.setEnabled(False)
        solve_form.addRow(self.solve_button)

        self.solve_result_label = QLabel("(not solved yet)")
        self.solve_result_label.setWordWrap(True)
        solve_form.addRow(self.solve_result_label)

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

        self.data_info_label.setText(
            f"{path}\n{len(self.data.q)} points "
            f"(skipped {self.data.n_lines_skipped} lines)\n"
            f"q: [{self.data.q.min():.4g}, {self.data.q.max():.4g}]\n"
            f"I: [{self.data.I.min():.4g}, {self.data.I.max():.4g}]"
        )
        self.bg_qmin.setValue(self.data.q.min())
        self.bg_qmax.setValue(self.data.q.max())
        self.fit_bg_button.setEnabled(True)
        self.solve_button.setEnabled(True)
        self.bg_result = None
        self.bg_fit_label.setText("(not fit yet)")

        self._suggest_r_grid()

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
            basis_note = "Dmax/2 (sphere kernel's r is a RADIUS, Dmax is a diameter/max-distance scale)"
        else:
            r_max_suggested = dmax
            basis_note = "Dmax (this kernel's r is the pair-distance variable directly)"

        self.r_max_spin.setValue(r_max_suggested)
        self.r_max_hint_label.setText(
            f"suggested from q_min = {self.data.q.min():.4g}: Dmax ≤ π/q_min ≈ {dmax:.4g}, "
            f"r_max ≈ {basis_note} ≈ {r_max_suggested:.4g} "
            f"(override freely)"
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
        self.r_n_hint_label.setText(
            f"suggested as Shannon number N_s ≈ {suggested_n_r} "
            f"(= (q_max−q_min)·r_max/π ≈ {n_shannon:.3g}) "
            f"(override freely)"
        )

    def _on_fixed_bg_toggled(self, checked: bool):
        self.fixed_bg_spin.setEnabled(checked)
        self.fit_bg_button.setEnabled(not checked and self.data is not None)

    def _on_kernel_changed(self, index: int):
        key = self.kernel_combo.itemData(index)
        if key is not None:
            self.alpha_spin.setValue(KERNEL_REGISTRY[key].default_alpha)
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
                dI=self.data.dI,
            )
        except Exception as exc:
            QMessageBox.critical(self, "Background fit failed", str(exc))
            return

        risk = self.bg_result.extrapolation_risk(self.data.q)
        risk_note = (
            f"\n\u26a0 extrapolation risk={risk:.2f} -- background is being "
            f"extrapolated well beyond the fit range; a power-law background "
            f"can amplify parameter uncertainty enormously out there. "
            f"Consider a fixed value instead." if risk > 1.0 else ""
        )
        degenerate_note = (
            "\n\u26a0 backgr and c are individually poorly constrained "
            "(near-degenerate) -- the fitted curve/sum may still be fine."
            if self.bg_result.param_errors[0] > 10 * max(abs(self.bg_result.backgr), 1e-12)
            else ""
        )
        self.bg_fit_label.setText(
            f"backgr={self.bg_result.backgr:.4g} \u00b1 {self.bg_result.param_errors[0]:.2g}\n"
            f"c={self.bg_result.c:.4g} \u00b1 {self.bg_result.param_errors[1]:.2g}\n"
            f"\u03b1={self.bg_result.alpha:.4g} \u00b1 {self.bg_result.param_errors[2]:.2g}"
            f"{risk_note}{degenerate_note}"
        )
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
            alpha = self.alpha_spin.value()

            # r always starts at 0 -- see the comment where r_max_spin is
            # built for why p(0)=0 isn't an optional/adjustable choice.
            r_grid = np.linspace(0.0, self.r_max_spin.value(), self.r_n_spin.value())
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
                r_quad = np.linspace(0.0, self.r_max_spin.value(),
                                     4 * (self.r_n_spin.value() - 1) + 1)
                spline = SplineOptions(
                    n_coeffs=self.spline_n_coeffs_spin.value(),
                    spacing=self.spline_spacing_combo.currentData(),
                    A_quad=build_size_distribution_kernel(
                        self.data.q, r_quad, kernel_func, alpha=alpha
                    ),
                )
            result = run_solver(solver_key, A, I_sub, self.data.dI, spline=spline, **solver_kwargs)
        except Exception:
            QMessageBox.critical(self, "Solve failed", traceback.format_exc())
            return

        self._last_r_grid = r_grid
        self._last_alpha = alpha
        self._last_result = result
        self._last_I_sub = I_sub
        self._last_boot = None

        chi2_final = result.chi2_r_history[-1] if result.chi2_r_history else float("nan")
        x_sigma = result.diagnostics.get("x_sigma")
        sigma_note = " (analytic Laplace/Hessian error band available)" if x_sigma is not None else ""
        kernel_note = ""
        if solver_key in self._POSITIVE_KERNEL_SOLVERS and np.any(A < 0):
            kernel_note = (
                "\n\u26a0 this solver needs a non-negative kernel, but the selected "
                "kernel has negative entries -- result is not meaningful. Use "
                "'Signed-kernel EM' or a Bayesian-evidence solver."
            )
        self.solve_result_label.setText(
            f"chi2_r = {chi2_final:.4g}\n"
            f"{result.diagnostics.get('lambda_selection', '')}{sigma_note}\n"
            f"iterations: {result.n_iterations}, converged: {result.converged}"
            f"{kernel_note}"
        )
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
                    return run_solver(solver_key, A, b_rep, self.data.dI, spline=spline, **solver_kwargs)
                boot = bootstrap_uncertainty(
                    solve_fn, I_sub, self.data.dI, n_boot=n_boot,
                    center=result.fitted_b,
                )
                self._last_boot = boot
                self.solve_result_label.setText(
                    self.solve_result_label.text()
                    + f"\nbootstrap: {boot.n_boot} ok, {boot.n_failed} failed replicates"
                )
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

    # -------------------------------------------------------------- plots

    def _plot_iq(self):
        self.ax_iq.clear()
        if self.data is not None:
            self.ax_iq.errorbar(
                self.data.q, self.data.I, yerr=self.data.dI,
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
