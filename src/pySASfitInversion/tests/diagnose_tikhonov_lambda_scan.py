"""
Diagnostic: scan general-form Tikhonov's lambda directly (bypassing GCV)
for the j0/PDDF kernel on the real test.dat, to check whether SOME
regularization strength gets chi2_r close to 1 -- if so, the earlier
GCV-selected lambda (chi2_r=3.46e4) and EM+smoothing's best h (chi2_r=586)
are both picking badly-tuned regularization for this kernel, not revealing
a fundamental inversion-quality ceiling.

Run from the pySASfitInversion directory:
    python3 tests/diagnose_tikhonov_lambda_scan.py
"""
import sys, os
import numpy as np

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "src"))

from sasfit_inversion.io_utils import load_sas_data
from sasfit_inversion.kernel_registry import KERNEL_REGISTRY
from sasfit_inversion.kernels import build_size_distribution_kernel
from sasfit_inversion.solvers import general_tikhonov, svd_methods
from sasfit_inversion.solvers.base import chi2_r as pkg_chi2_r
from sasfit_inversion.regularization import second_derivative_operator


def main():
    data = load_sas_data(os.path.join(os.path.dirname(__file__), "data", "test.dat"))
    q, I, dI = data.q, data.I, data.dI
    b = I - 0.1
    db = dI

    for n_r in (150, 61, 40):
        r = np.linspace(1.0, 500.0, n_r)
        A = build_size_distribution_kernel(q, r, KERNEL_REGISTRY["sinc_4pi"].func, alpha=0.0)
        L = second_derivative_operator(n_r).toarray()

        svd_A = svd_methods.compute_svd(A)
        print(f"--- n_r={n_r}: A condition number = {svd_A.s.max()/max(svd_A.s.min(),1e-300):.3e} ---")

        lam_grid = np.geomspace(1e-8, 1e8, 60)
        best_lam, best_chi2 = None, np.inf
        for lam in lam_grid:
            x = general_tikhonov.general_tikhonov(A, L, b, lam)
            fitted_b = A @ x
            chi2 = pkg_chi2_r(b, fitted_b, db)
            if abs(chi2 - 1.0) < abs(best_chi2 - 1.0):
                best_chi2, best_lam = chi2, lam

        print(f"  closest-to-1 over lambda grid: lambda={best_lam:.4g}, chi2_r={best_chi2:.4g}")

        # also report the full min/max chi2_r range achieved across the grid
        chi2_values = []
        for lam in lam_grid:
            x = general_tikhonov.general_tikhonov(A, L, b, lam)
            chi2_values.append(pkg_chi2_r(b, A @ x, db))
        chi2_values = np.array(chi2_values)
        print(f"  chi2_r range over lambda grid: [{chi2_values.min():.4g}, {chi2_values.max():.4g}]")
        print()


if __name__ == "__main__":
    main()
