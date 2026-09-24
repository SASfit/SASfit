import numpy as np
import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

from sasfit_inversion.kernels import build_size_distribution_kernel
from sasfit_inversion.solvers import maxent


def sphere_form_factor_squared(q, R):
    qr = q * R
    qr = np.where(qr == 0, 1e-10, qr)
    F = 3 * (np.sin(qr) - qr * np.cos(qr)) / qr**3
    return F**2


def main():
    rng = np.random.default_rng(0)

    s = np.linspace(5, 400, 150)
    N_true = (
        1.0 * np.exp(-0.5 * ((s - 80) / 15) ** 2)
        + 0.4 * np.exp(-0.5 * ((s - 220) / 40) ** 2)
    )
    N_true /= N_true.max()

    q = np.geomspace(0.005, 0.5, 80)
    A = build_size_distribution_kernel(q, s, sphere_form_factor_squared, alpha=0.0)

    b_clean = A @ N_true
    db = 0.02 * b_clean + 1e-6 * b_clean.max()
    b = b_clean + rng.normal(scale=db)

    # constant prior: a flat, featureless prior (deliberately uninformative)
    prior = np.full_like(s, N_true.mean())
    result_const = maxent.solve_constant_prior(A, b, db, prior=prior, lam=0.01, max_iterations=2000)
    corr_const = np.corrcoef(result_const.x, N_true)[0, 1]
    print(f"[constant prior] final chi2_r={result_const.chi2_r_history[-1]:.3f}, "
          f"correlation={corr_const:.3f}")

    result_adapt = maxent.solve_adaptive_prior(A, b, db, lam=0.01, sigma2=1.0, max_iterations=2000)
    corr_adapt = np.corrcoef(result_adapt.x, N_true)[0, 1]
    print(f"[adaptive prior] final chi2_r={result_adapt.chi2_r_history[-1]:.3f}, "
          f"correlation={corr_adapt:.3f}")

    assert corr_const > 0.7, "constant-prior MaxEnt should recover the true distribution reasonably"
    assert corr_adapt > 0.7, "adaptive-prior MaxEnt should recover the true distribution reasonably"
    print("OK")


if __name__ == "__main__":
    main()
