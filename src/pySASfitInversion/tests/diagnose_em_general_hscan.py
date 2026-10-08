"""
Diagnostic (not a pytest-style test): scan smoothing strength h directly
for em_general.solve on the real test.dat, to check whether the automatic
L-curve/discrepancy search in solver_registry._run_em_general is picking
an h appropriate for the solver's *converged* fixed point (now that it's
accelerated with a real tol), or whether it inherited an h range/strategy
that only made sense for the old always-run-10000-unaccelerated-iterations
behavior (which implicitly over-regularized via semiconvergence).

Run from the pySASfitInversion directory:
    python3 tests/diagnose_em_general_hscan.py
"""
import sys, os
import numpy as np

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "src"))

from sasfit_inversion.io_utils import load_sas_data
from sasfit_inversion.kernel_registry import KERNEL_REGISTRY
from sasfit_inversion.kernels import build_size_distribution_kernel
from sasfit_inversion.solvers import em_general, general_tikhonov, svd_methods
from sasfit_inversion.solvers.base import chi2_r as pkg_chi2_r
from sasfit_inversion.regularization import second_derivative_operator


def main():
    data = load_sas_data(os.path.join(os.path.dirname(__file__), "data", "test.dat"))
    q, I, dI = data.q, data.I, data.dI

    backgr = 0.1
    b = I - backgr
    db = dI

    r = np.linspace(1.0, 500.0, 150)
    A = build_size_distribution_kernel(q, r, KERNEL_REGISTRY["sinc_4pi"].func, alpha=0.0)
    print(f"kernel shape: {A.shape}, has negative entries: {(A < 0).any()}")
    print(f"b range: [{b.min():.4g}, {b.max():.4g}], db range: [{db.min():.4g}, {db.max():.4g}]")
    print()

    print("h scan, accelerate=True, tol=1e-8, max_iterations=20000:")
    print(f"{'h':>10} {'chi2_r':>12} {'n_iter':>8} {'converged':>10} {'offset_t':>12}")
    for h in np.geomspace(1e-4, 1.0, 16):
        result = em_general.solve(A, b, db, max_iterations=20000, smoothing_h=h, tol=1e-8, accelerate=True)
        chi2 = result.chi2_r_history[-1] if result.chi2_r_history else float("nan")
        print(f"{h:10.4g} {chi2:12.4g} {result.n_iterations:8d} {str(result.converged):>10} {result.diagnostics.get('offset_t', float('nan')):12.4g}")

    print()
    print("Same scan, no smoothing applied via em_general's own operator but")
    print("comparing accelerate=True vs accelerate=False at h=0.3 (the search's")
    print("param_start) to see if acceleration itself (not just h) changed the")
    print("converged answer at a fixed h:")
    for accel in (True, False):
        max_it = 20000 if accel else 300_000
        result = em_general.solve(A, b, db, max_iterations=max_it, smoothing_h=0.3, tol=1e-8, accelerate=accel)
        chi2 = result.chi2_r_history[-1] if result.chi2_r_history else float("nan")
        print(f"  accelerate={accel!s:5} max_iterations={max_it:7d} -> n_iter={result.n_iterations:7d} "
              f"converged={result.converged} chi2_r={chi2:.4g}")

    print()
    print("Sanity check: can ANY linear solver fit this same kernel matrix A well?")
    print("(general-form Tikhonov doesn't care whether the kernel is signed --")
    print(" if this also can't reach chi2_r~1, the problem is the kernel/data")
    print(" setup, not something EM-specific.)")
    n = A.shape[1]
    L = second_derivative_operator(n).toarray()
    transform = general_tikhonov.std_form(A, L, b)
    svd_s = svd_methods.compute_svd(transform.A_s)
    lam_grid = np.geomspace(svd_s.s.min() * 1e-3 + 1e-300, svd_s.s.max() * 10, 100)
    lam_opt, _ = general_tikhonov.general_tikhonov_gcv(A, L, b, lam_grid)
    x_tik = general_tikhonov.general_tikhonov(A, L, b, lam_opt)
    fitted_b_tik = A @ x_tik
    chi2_tik = pkg_chi2_r(b, fitted_b_tik, db)
    print(f"  general_tikhonov_gcv: lambda={lam_opt:.4g}, chi2_r={chi2_tik:.4g}")

    print()
    print("Also: unregularized least-squares solve of A x = b directly (no")
    print("positivity, no smoothing at all) -- the best ANY linear model fit to")
    print("this kernel could possibly do:")
    x_lstsq, *_ = np.linalg.lstsq(A / db[:, None], b / db, rcond=None)
    fitted_b_lstsq = A @ x_lstsq
    chi2_lstsq = pkg_chi2_r(b, fitted_b_lstsq, db)
    print(f"  plain lstsq: chi2_r={chi2_lstsq:.4g}")


if __name__ == "__main__":
    main()
