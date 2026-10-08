"""
Diagnostic: does Hansen (2000)'s boundary-constrained smoothness operator
(regularization.hansen_smoothness_cholesky), combined with the existing
Bayesian-evidence lambda selection (solvers/bayesian_evidence.py), actually
fix the extreme ill-conditioning (~1e16) seen with the plain
second_derivative_operator on the real j0/PDDF-kernel inversion of
test.dat?

Run from the pySASfitInversion directory:
    python3 tests/diagnose_hansen_evidence.py
"""
import sys, os
import numpy as np

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "src"))

from sasfit_inversion.io_utils import load_sas_data
from sasfit_inversion.kernel_registry import KERNEL_REGISTRY
from sasfit_inversion.kernels import build_size_distribution_kernel
from sasfit_inversion.solvers import bayesian_evidence
from sasfit_inversion.solvers.base import chi2_r as pkg_chi2_r
from sasfit_inversion.regularization import hansen_smoothness_cholesky, second_derivative_operator


def main():
    data = load_sas_data(os.path.join(os.path.dirname(__file__), "data", "test.dat"))
    q, I, dI = data.q, data.I, data.dI
    b = I - 0.1
    db = dI

    n_r = 150
    r = np.linspace(1.0, 500.0, n_r)
    A = build_size_distribution_kernel(q, r, KERNEL_REGISTRY["sinc_4pi"].func, alpha=0.0)

    print(f"kernel shape: {A.shape}")
    print()

    L_hansen = hansen_smoothness_cholesky(n_r)
    print(f"Hansen L shape: {L_hansen.shape}, L^T L condition number: "
          f"{np.linalg.cond(L_hansen.T @ L_hansen):.4g}")
    print()

    print("Bayesian-evidence lambda scan with Hansen's boundary-constrained operator:")
    print(f"{'lambda':>12} {'log_evidence':>14} {'chi2_r':>12} {'n_good_params':>14}")
    lam_grid = np.geomspace(1e-6, 1e4, 40)
    best, results = bayesian_evidence.evidence_search(A, b, db, L_hansen, lam_grid)
    for r_ in results:
        chi2 = pkg_chi2_r(b, A @ r_.p_map, db)
        marker = "  <== best evidence" if r_ is best else ""
        print(f"{r_.lam:12.4g} {r_.log_evidence:14.6g} {chi2:12.4g} {r_.n_good_params:14.4g}{marker}")

    print()
    best_chi2 = pkg_chi2_r(b, A @ best.p_map, db)
    print(f"Best-evidence lambda={best.lam:.4g}: chi2_r={best_chi2:.4g}, "
          f"n_good_params={best.n_good_params:.4g} (out of {n_r})")
    print(f"p(r) range: [{best.p_map.min():.4g}, {best.p_map.max():.4g}]")
    print(f"p(r) at endpoints: p(r_min)={best.p_map[0]:.4g}, p(r_max)={best.p_map[-1]:.4g}")

    print()
    print("For comparison, same evidence search with the OLD (rank-deficient,")
    print("no boundary condition) second_derivative_operator:")
    L_old = second_derivative_operator(n_r).toarray()
    try:
        best_old, _ = bayesian_evidence.evidence_search(A, b, db, L_old, lam_grid)
        chi2_old = pkg_chi2_r(b, A @ best_old.p_map, db)
        print(f"  lambda={best_old.lam:.4g}: chi2_r={chi2_old:.4g}")
    except Exception as exc:
        print(f"  failed: {exc!r}")


if __name__ == "__main__":
    main()
