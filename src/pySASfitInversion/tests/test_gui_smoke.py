"""
Headless GUI smoke test: drives the real MainWindow class (not just the
library it calls) through the full intended workflow -- load file, set a
fixed background, pick kernel+solver, solve, check plots got populated and
results are sane. Run with QT_QPA_PLATFORM=offscreen (no real display
needed), which this file sets before importing Qt.
"""
import os
os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

import sys
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "src"))

import numpy as np
from PySide6.QtWidgets import QApplication

from gui.app import MainWindow

DATA_PATH = os.path.join(os.path.dirname(__file__), "data", "test.dat")


def main():
    app = QApplication.instance() or QApplication(sys.argv)
    win = MainWindow()

    # --- load file ---
    win.load_file(DATA_PATH)
    assert win.data is not None, "data should be loaded"
    assert len(win.data.q) == 91, f"expected 91 points, got {len(win.data.q)}"
    print(f"loaded {len(win.data.q)} points via the GUI's load_file()")

    # --- fixed background = 0.1 ---
    win.use_fixed_bg.setChecked(True)
    win.fixed_bg_spin.setValue(0.1)
    assert win.fixed_bg_spin.isEnabled(), "fixed bg spinbox should enable when checkbox is checked"
    assert not win.fit_bg_button.isEnabled(), "fit-background button should disable in fixed mode"

    # --- select sphere kernel (alpha should auto-populate to 6) ---
    idx = win.kernel_combo.findData("sphere_rg")
    assert idx >= 0, "sphere_rg kernel should be in the dropdown"
    win.kernel_combo.setCurrentIndex(idx)
    assert win.alpha_spin.value() == 6.0, f"alpha should auto-set to 6, got {win.alpha_spin.value()}"

    # r always starts at 0 now (p(0)=0 is a hard physical requirement --
    # see gui/app.py's r_max_spin comment), so there's no r_min to set.
    win.r_max_spin.setValue(500.0)
    win.r_n_spin.setValue(150)

    # --- select EM+discrepancy solver and solve ---
    idx = win.solver_combo.findData("em_discrepancy")
    assert idx >= 0, "em_discrepancy solver should be in the dropdown"
    win.solver_combo.setCurrentIndex(idx)

    win._on_solve()
    assert hasattr(win, "_last_result"), "solve should populate _last_result"

    chi2 = win._last_result.chi2_r_history[-1]
    print(f"GUI solve produced chi2_r={chi2:.4f}")
    assert abs(chi2 - 1.0) < 0.1, f"expected chi2_r near 1.0 on this benchmark, got {chi2}"

    r_grid = win._last_r_grid
    x = win._last_result.x
    # r_grid[0] == 0 (grid always starts at the origin now) -- r**-3 is
    # singular there, so exclude that point from the peak search rather
    # than letting a spurious inf win np.argmax.
    nonzero = r_grid > 0
    Dv = x[nonzero] * r_grid[nonzero] ** (-3)
    main_peak_r = r_grid[nonzero][np.argmax(Dv)]
    print(f"main peak at r={main_peak_r:.1f}nm (expect ~75-85nm)")
    assert 60 < main_peak_r < 100, f"main peak should land near ~75-85nm, got {main_peak_r}"

    # --- check the plots actually got populated (not just computed) ---
    assert len(win.ax_iq.lines) > 0 or len(win.ax_iq.containers) > 0, "I(q) plot should have content"
    assert len(win.ax_dist.lines) > 0, "distribution plot should have content"

    # --- reduced solver list (2026-10-10) ---
    keys = [win.solver_combo.itemData(i) for i in range(win.solver_combo.count())]
    assert len(keys) == 9 and "bayesian_evidence_spline" not in keys, keys

    # --- PDDF path: j0 kernel, Bayesian evidence (2nd derivative) with the
    #     B-spline option, log knots, forced chi2_r=1 and bootstrap ---
    win.kernel_combo.setCurrentIndex(win.kernel_combo.findData("sinc_4pi"))
    win.use_fixed_bg.setChecked(True)
    win.fixed_bg_spin.setValue(0.1)
    win.solver_combo.setCurrentIndex(win.solver_combo.findData("bayesian_evidence"))
    pen_label, pen_combo = win.option_widgets["penalty"]
    assert not pen_combo.isHidden(), "penalty selector should show for Bayesian evidence"
    assert pen_combo.currentData() == "hansen_eq19", "eq. 19 should be the default penalty"
    assert win.option_widgets["prior"][1].isHidden()
    pen_combo.setCurrentIndex(pen_combo.findData("second_derivative"))
    win.spline_check.setChecked(True)
    assert not win.spline_spacing_combo.isHidden(), "spacing selector should show in spline mode"
    win.spline_spacing_combo.setCurrentIndex(win.spline_spacing_combo.findData("log"))
    win.spline_n_coeffs_spin.setValue(20)
    win.target_chi2_check.setChecked(True)
    win.mc_uncertainty_check.setChecked(True)
    win.n_boot_spin.setValue(10)
    win._on_solve()
    note = win._last_result.diagnostics["lambda_selection"]
    assert "log knots" in note and "integrated on" in note and "2nd derivative" in note, note
    assert win._last_boot is not None and win._last_boot.n_boot >= 8
    p = win._last_result.x
    print(f"spline/log GUI solve: chi2_r={win._last_result.chi2_r_history[-1]:.3f}, "
          f"bootstrap max std={win._last_boot.x_std.max():.3f} (p peak {p.max():.2f})")
    assert win._last_boot.x_std.max() < 0.1 * p.max()
    win.mc_uncertainty_check.setChecked(False)

    # --- each Bayesian-evidence penalty, spline mode, reports ln Z ---
    win.target_chi2_check.setChecked(False)
    for pen in ("hansen_printed", "hansen_eq19"):
        pen_combo.setCurrentIndex(pen_combo.findData(pen))
        win._on_solve()
        d = win._last_result.diagnostics
        print(f"Bayesian evidence [{pen}] GUI solve: chi2_r={win._last_result.chi2_r_history[-1]:.3f}, "
              f"ln Z={d['log_evidence_normalized']:.1f}")
        assert d["lambda_selection"].startswith("B-spline basis"), d["lambda_selection"]
        assert 0.7 < win._last_result.chi2_r_history[-1] < 1.3
    win.spline_check.setChecked(False)

    # --- EM option: L-curve selection via the dropdown ---
    win.kernel_combo.setCurrentIndex(win.kernel_combo.findData("sphere_rg"))
    win.r_max_spin.setValue(500.0)
    win.r_n_spin.setValue(150)
    win.solver_combo.setCurrentIndex(win.solver_combo.findData("em_discrepancy"))
    sel = win.option_widgets["selection"][1]
    sel.setCurrentIndex(sel.findData("lcurve"))
    win._on_solve()
    assert "L-curve" in win._last_result.diagnostics["lambda_selection"]
    sel.setCurrentIndex(sel.findData("discrepancy"))

    print("OK -- GUI smoke test passed: load -> fixed background -> kernel/solver "
          "selection -> solve -> plots, all via the real MainWindow class")


if __name__ == "__main__":
    main()
