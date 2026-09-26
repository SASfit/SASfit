import numpy as np
import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

from sasfit_inversion.kernels import build_size_distribution_kernel
from sasfit_inversion.solvers import arls as arls_mod


def sphere_form_factor_squared(q, R):
    qr = q * R
    qr = np.where(qr == 0, 1e-10, qr)
    F = 3 * (np.sin(qr) - qr * np.cos(qr)) / qr**3
    return F**2


def run_case(s, N_true, q, A, label):
    rng = np.random.default_rng(0)
    b_clean = A @ N_true
    db = 0.03 * b_clean + 1e-6 * b_clean.max()
    b = b_clean + rng.normal(scale=db)

    x_arls = arls_mod.arls(A, b)
    corr_arls = np.corrcoef(x_arls, N_true)[0, 1]
    print(f"[{label}] arls:   correlation={corr_arls:.3f}, min={x_arls.min():.4f} (negative excursions allowed)")

    x_arlsnn = arls_mod.arlsnn(A, b)
    corr_nn = np.corrcoef(x_arlsnn, N_true)[0, 1]
    print(f"[{label}] arlsnn: correlation={corr_nn:.3f}, min={x_arlsnn.min():.4f} (should be >= 0)")

    return corr_arls, corr_nn, x_arlsnn


def main():
    # single-Gaussian case
    s1 = np.linspace(5, 400, 100)
    N1 = np.exp(-0.5 * ((s1 - 80) / 15) ** 2)
    N1 /= N1.max()
    q1 = np.geomspace(0.005, 0.5, 60)
    A1 = build_size_distribution_kernel(q1, s1, sphere_form_factor_squared, alpha=0.0)
    corr_arls_1, corr_nn_1, x_nn_1 = run_case(s1, N1, q1, A1, "single Gaussian")

    # bimodal case
    s2 = np.linspace(5, 400, 150)
    N2 = 1.0 * np.exp(-0.5 * ((s2 - 80) / 15) ** 2) + 0.4 * np.exp(-0.5 * ((s2 - 220) / 40) ** 2)
    N2 /= N2.max()
    q2 = np.geomspace(0.005, 0.5, 80)
    A2 = build_size_distribution_kernel(q2, s2, sphere_form_factor_squared, alpha=0.0)
    corr_arls_2, corr_nn_2, x_nn_2 = run_case(s2, N2, q2, A2, "bimodal")

    assert corr_nn_1 > 0.5, "arlsnn should do at least reasonably on the single-Gaussian case"
    assert x_nn_1.min() >= -1e-8, "arlsnn output should be non-negative"
    assert x_nn_2.min() >= -1e-8, "arlsnn output should be non-negative"
    print("OK")


if __name__ == "__main__":
    main()
