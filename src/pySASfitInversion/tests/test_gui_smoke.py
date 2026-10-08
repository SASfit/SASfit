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

    win.r_min_spin.setValue(1.0)
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
    Dv = x * r_grid ** (-3)
    main_peak_r = r_grid[np.argmax(Dv)]
    print(f"main peak at r={main_peak_r:.1f}nm (expect ~75-85nm)")
    assert 60 < main_peak_r < 100, f"main peak should land near ~75-85nm, got {main_peak_r}"

    # --- check the plots actually got populated (not just computed) ---
    assert len(win.ax_iq.lines) > 0 or len(win.ax_iq.containers) > 0, "I(q) plot should have content"
    assert len(win.ax_dist.lines) > 0, "distribution plot should have content"

    print("OK -- GUI smoke test passed: load -> fixed background -> kernel/solver "
          "selection -> solve -> plots, all via the real MainWindow class")


if __name__ == "__main__":
    main()
