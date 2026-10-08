import numpy as np
import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "src"))

from sasfit_inversion.kernels import build_size_distribution_kernel
from sasfit_inversion.solvers import em


def sphere_form_factor_squared(q, R):
    qr = q * R
    qr = np.where(qr == 0, 1e-10, qr)
    F = 3 * (np.sin(qr) - qr * np.cos(qr)) / qr**3
    return F**2


def main():
    rng = np.random.default_rng(0)

    # synthetic bimodal number-weighted distribution: two Gaussian populations
    s = np.linspace(5, 400, 150)
    N_true = (
        1.0 * np.exp(-0.5 * ((s - 80) / 15) ** 2)
        + 0.4 * np.exp(-0.5 * ((s - 220) / 40) ** 2)
    )
    N_true /= N_true.max()

    q = np.geomspace(0.005, 0.5, 80)
    A = build_size_distribution_kernel(q, s, sphere_form_factor_squared, alpha=0.0)

    b_clean = A @ N_true
    noise_level = 0.02
    db = noise_level * b_clean + 1e-6 * b_clean.max()
    b = b_clean + rng.normal(scale=db)

    result = em.solve(A, b, db, max_iterations=2000, smoothing_h=0.05)

    final_chi2 = result.chi2_r_history[-1]
    corr = np.corrcoef(result.x, N_true)[0, 1]

    print(f"final chi2_r = {final_chi2:.3f}")
    print(f"correlation with true distribution = {corr:.3f}")
    print(f"chi2_r trend (first vs last 5 iters): "
          f"{np.mean(result.chi2_r_history[:5]):.3f} -> "
          f"{np.mean(result.chi2_r_history[-5:]):.3f}")

    assert corr > 0.8, "recovered distribution should correlate reasonably with the truth"
    assert result.chi2_r_history[-1] < result.chi2_r_history[0], "chi2_r should improve"
    print("OK")


if __name__ == "__main__":
    main()
