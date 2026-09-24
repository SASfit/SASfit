import numpy as np
import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

from sasfit_inversion.kernels import build_size_distribution_kernel
from sasfit_inversion.solvers import em, lambda_search


def sphere_form_factor_squared(q, R):
    qr = q * R
    qr = np.where(qr == 0, 1e-10, qr)
    F = 3 * (np.sin(qr) - qr * np.cos(qr)) / qr**3
    return F**2


def main():
    rng = np.random.default_rng(0)

    s = np.linspace(5, 400, 100)
    N_true = np.exp(-0.5 * ((s - 80) / 15) ** 2)
    N_true /= N_true.max()

    q = np.geomspace(0.005, 0.5, 60)
    A = build_size_distribution_kernel(q, s, sphere_form_factor_squared, alpha=0.0)

    b_clean = A @ N_true
    db = 0.03 * b_clean + 1e-6 * b_clean.max()
    b = b_clean + rng.normal(scale=db)

    def solve_fn(h):
        return em.solve(A, b, db, max_iterations=800, smoothing_h=h)

    disc = lambda_search.discrepancy_principle_search(solve_fn, target_chi2_r=1.0, param_start=0.3)
    corr_disc = np.corrcoef(disc.result.x, N_true)[0, 1]
    print(f"[discrepancy principle] h={disc.param:.5f}, "
          f"chi2_r={disc.result.chi2_r_history[-1]:.3f}, correlation={corr_disc:.3f}, "
          f"n_evals={len(disc.trace)}")

    lcurve = lambda_search.l_curve_search(solve_fn, param_start=0.3, n_points=20)
    corr_lcurve = np.corrcoef(lcurve.result.x, N_true)[0, 1]
    print(f"[L-curve corner] h={lcurve.param:.5f}, "
          f"chi2_r={lcurve.result.chi2_r_history[-1]:.3f}, correlation={corr_lcurve:.3f}")

    assert abs(disc.result.chi2_r_history[-1] - 1.0) < 0.3, "discrepancy principle should land near chi2_r=1"
    assert corr_disc > 0.8
    print("OK")


if __name__ == "__main__":
    main()
