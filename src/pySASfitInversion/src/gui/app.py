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
    QFileDialog, QGroupBox, QSplitter, QMessageBox, QStatusBar,
)
from PySide6.QtCore import Qt
from matplotlib.backends.backend_qtagg import FigureCanvasQTAgg
from matplotlib.figure import Figure

from sasfit_inversion.io_utils import load_sas_data, SASData
from sasfit_inversion.background import fit_background, BackgroundFitResult
from sasfit_inversion.kernel_registry import KERNEL_REGISTRY
from sasfit_inversion.kernels import build_size_distribution_kernel
from sasfit_inversion.solver_registry import SOLVER_REGISTRY


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("pySASfitInversion")
        self.resize(1200, 800)

        self.data: SASData | None = None
        self.bg_result: BackgroundFitResult | None = None
        self.fixed_bg_value: float | None = None

        splitter = QSplitter(Qt.Horizontal)
        splitter.addWidget(self._build_control_panel())
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

        self.r_min_spin = QDoubleSpinBox()
        self.r_min_spin.setRange(0.001, 1e6)
        self.r_min_spin.setValue(1.0)
        self.r_max_spin = QDoubleSpinBox()
        self.r_max_spin.setRange(0.001, 1e6)
        self.r_max_spin.setValue(500.0)
        self.r_max_spin.setToolTip(
            "On data load, this is suggested as pi/q_min (the largest Dmax the "
            "data's low-q reach can actually constrain -- Glatter's consistency "
            "condition q_min <= pi/Dmax). Feel free to override it."
        )
        self.r_n_spin = QSpinBox()
        self.r_n_spin.setRange(10, 2000)
        self.r_n_spin.setValue(150)
        solve_form.addRow("r min:", self.r_min_spin)
        solve_form.addRow("r max:", self.r_max_spin)
        self.r_max_hint_label = QLabel("")
        self.r_max_hint_label.setWordWrap(True)
        solve_form.addRow("", self.r_max_hint_label)
        solve_form.addRow("r grid points:", self.r_n_spin)

        self.solver_combo = QComboBox()
        for key, spec in SOLVER_REGISTRY.items():
            self.solver_combo.addItem(spec.label, userData=key)
        solve_form.addRow("Solver:", self.solver_combo)

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

        # Suggest Dmax from the data's q_min via Glatter's consistency
        # condition q_min <= pi/Dmax (equivalently Dmax <= pi/q_min): beyond
        # this Dmax the data doesn't reach low enough in q to actually
        # constrain p(r) there, and larger-Dmax "improvements" tend to be
        # the solver overfitting rather than resolving real structure (see
        # tests/diagnose_hansen_dmax_scan.py for the empirical confirmation
        # of this on tests/data/test.dat). This is only a starting point --
        # the user can freely override r max afterwards.
        suggested_dmax = float(np.pi / self.data.q.min())
        self.r_max_spin.setValue(suggested_dmax)
        self.r_max_hint_label.setText(
            f"suggested from q_min = {self.data.q.min():.4g}: "
            f"Dmax ≤ π/q_min ≈ {suggested_dmax:.4g} "
            f"(override freely)"
        )

        self._plot_iq()
        self.status.showMessage(f"Loaded {path}")

    def _on_fixed_bg_toggled(self, checked: bool):
        self.fixed_bg_spin.setEnabled(checked)
        self.fit_bg_button.setEnabled(not checked and self.data is not None)

    def _on_kernel_changed(self, index: int):
        key = self.kernel_combo.itemData(index)
        if key is not None:
            self.alpha_spin.setValue(KERNEL_REGISTRY[key].default_alpha)

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

            r_grid = np.linspace(self.r_min_spin.value(), self.r_max_spin.value(), self.r_n_spin.value())
            A = build_size_distribution_kernel(self.data.q, r_grid, kernel_func, alpha=alpha)

            solver_key = self.solver_combo.currentData()
            result = SOLVER_REGISTRY[solver_key].run(A, I_sub, self.data.dI)
        except Exception:
            QMessageBox.critical(self, "Solve failed", traceback.format_exc())
            return

        self._last_r_grid = r_grid
        self._last_alpha = alpha
        self._last_result = result
        self._last_I_sub = I_sub

        chi2_final = result.chi2_r_history[-1] if result.chi2_r_history else float("nan")
        self.solve_result_label.setText(
            f"chi2_r = {chi2_final:.4g}\n"
            f"{result.diagnostics.get('lambda_selection', '')}\n"
            f"iterations: {result.n_iterations}, converged: {result.converged}"
        )
        self.status.showMessage(f"Solved: chi2_r={chi2_final:.4g}")

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
        if hasattr(self, "_last_result"):
            r = self._last_r_grid
            x = self._last_result.x
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
