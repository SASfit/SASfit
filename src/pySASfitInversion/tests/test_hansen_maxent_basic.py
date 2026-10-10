import numpy as np
import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "src"))

from sasfit_inversion.kernels import build_size_distribution_kernel
from sasfit_inversion.solvers import hansen_maxent


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

    best_corr, best_lam, best_result = -1, None, None
    x0 = np.full(len(s), 0.01)  # Hansen's literal default (ones(n)) diverges badly
                                  # on this ill-conditioned kernel -- see module docstring
    for lam in np.geomspace(1e-3, 1.0, 15):
        result = hansen_maxent.solve(A, b, db, lam=lam, x0=x0, max_iterations=150)
        if len(result.chi2_r_history) == 0:
            continue
        corr = np.corrcoef(result.x, N_true)[0, 1]
        if corr > best_corr:
            best_corr, best_lam, best_result = corr, lam, result

    print(f"[Hansen CG MaxEnt] best lambda={best_lam:.5g}, correlation={best_corr:.3f}, "
          f"iterations={best_result.n_iterations}, converged={best_result.converged}, "
          f"final chi2_r={best_result.chi2_r_history[-1]:.3f}")

    # NOTE: correlation ceiling here is genuinely low (~0.65) -- see the
    # module docstring's PERFORMANCE FINDING. Not asserting a high bar.
    assert best_corr > 0.5, "should recover the truth at least loosely at some lambda"
    # Fixed solver (2026-10-10): same functional, 1/dI-weighted, flat
    # default model, L-BFGS-B + Newton polish, lambda by discrepancy principle.
    from sasfit_inversion.solver_registry import run_solver
    res = run_solver("hansen_maxent", A, b, db)
    corr_new = np.corrcoef(res.x, N_true)[0, 1]
    print(f"[MaxEnt, L-BFGS-B, discrepancy] chi2_r={res.chi2_r_history[-1]:.4f}, "
          f"correlation={corr_new:.3f}  ({res.diagnostics['lambda_selection']})")
    assert abs(res.chi2_r_history[-1] - 1.0) < 1e-3
    assert corr_new > 0.9 and corr_new > best_corr + 0.2
    assert np.all(res.x > 0)

    # Monotonic chi2_r(lambda^2) at fixed default model (was not before the
    # Newton polish on the ill-conditioned j0 kernel).
    lam2s = np.geomspace(1e-3, 1e2, 8) * 1.0
    m = hansen_maxent.flat_prior_level(A, b, db)
    c2 = [hansen_maxent.solve_lbfgsb(A, b, db, l2, m).chi2_r_history[-1] for l2 in lam2s]
    assert np.all(np.diff(c2) > -1e-6 * np.max(c2)), c2
    print("OK")


if __name__ == "__main__":
    main()
