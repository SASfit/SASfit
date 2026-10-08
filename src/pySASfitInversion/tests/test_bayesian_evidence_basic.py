import numpy as np
import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "src"))

from sasfit_inversion.kernels import build_size_distribution_kernel
from sasfit_inversion.regularization import second_derivative_operator
from sasfit_inversion.solvers import bayesian_evidence


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
    K = build_size_distribution_kernel(q, s, sphere_form_factor_squared, alpha=0.0)

    b_clean = K @ N_true
    sigma = 0.03 * b_clean + 1e-6 * b_clean.max()
    b = b_clean + rng.normal(scale=sigma)

    L = second_derivative_operator(len(s)).toarray()

    lam_grid = np.geomspace(1e-6, 1e6, 80)
    best, all_results = bayesian_evidence.evidence_search(K, b, sigma, L, lam_grid)

    corr = np.corrcoef(best.p_map, N_true)[0, 1]
    print(f"[Bayesian evidence] lambda={best.lam:.5g}, "
          f"correlation={corr:.3f}, Ng={best.n_good_params:.1f} "
          f"(N={len(s)}, M={len(q)})")

    assert corr > 0.7, "evidence-selected lambda should recover the truth reasonably well"
    assert 0 < best.n_good_params < len(s), "Ng should be a sensible fraction of N"
    print("OK")


if __name__ == "__main__":
    main()
