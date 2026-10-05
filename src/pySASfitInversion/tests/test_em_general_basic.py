"""
Validates em_general.py's signed-kernel/signed-solution EM (Chae, Martin &
Walker 2018, Sec. 6) two ways:

  1. Reproduces the paper's own Fig. 7 example (a signed Gaussian-difference
     kernel k+/k- and a signed p built from two Beta densities) to
     near-exact accuracy, using the paper's exact k+/k- decomposition.

  2. An integration test against this library's own signed j0(qr) kernel
     (kernel_registry.py's "sinc_4pi"), recovering a sphere's analytic
     pair-correlation function gamma(r). Uses a noise floor that doesn't
     vanish at I(q)'s zero-crossings (a purely-proportional error model
     blows up chi2_r near those crossings for reasons that have nothing to
     do with the solver -- see em_general.py's module docstring history /
     the 2026-09-26 discussion).
"""
import numpy as np
import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

from scipy.stats import beta, norm

from sasfit_inversion.kernel_registry import KERNEL_REGISTRY
from sasfit_inversion.kernels import build_size_distribution_kernel
from sasfit_inversion.solvers import em_general


def test_paper_figure_7():
    sigma = 0.05
    theta = np.linspace(0.0025, 0.9975, 200)
    dth = theta[1] - theta[0]
    x = np.linspace(-1.5, 1.5, 240)

    phi = lambda z: norm.pdf(z, scale=sigma)
    K_pos = phi(x[:, None] - theta[None, :]) * dth
    K_neg = phi(x[:, None] + theta[None, :]) * dth
    K = K_pos - K_neg

    p_true = beta.pdf(theta, 2, 3) - beta.pdf(theta, 3, 2)
    b = K @ p_true
    db = np.full_like(b, 1e-3 * np.abs(b).max())

    result = em_general.solve(K, b, db, max_iterations=100, tol=0, K_pos=K_pos, K_neg=K_neg)
    corr = np.corrcoef(result.x, p_true)[0, 1]
    max_err = np.max(np.abs(result.x - p_true))
    print(f"[paper Fig. 7] corr={corr:.5f} max|err|={max_err:.4f} chi2_r={result.chi2_r_history[-1]:.4g}")

    assert corr > 0.999, "should closely reproduce the paper's own signed benchmark"
    assert max_err < 0.02, "pointwise error should be small after 100 iterations"


def test_signed_j0_kernel_sphere_correlation():
    R = 50.0
    r = np.linspace(1, 160, 120)
    gamma_true = np.where(r < 2 * R, 1 - 3 * r / (4 * R) + r**3 / (16 * R**3), 0.0)

    q = np.geomspace(0.004, 0.3, 90)
    A = build_size_distribution_kernel(q, r, KERNEL_REGISTRY["sinc_4pi"].func, alpha=0.0)
    assert (A < 0).any(), "this kernel is supposed to be signed -- sanity-check the test setup"

    rng = np.random.default_rng(1)
    I_true = A @ gamma_true
    db = 0.02 * np.abs(I_true) + 0.01 * np.abs(I_true).max()  # non-vanishing floor
    b = I_true + rng.normal(scale=db)

    result = em_general.solve(A, b, db, max_iterations=3000, smoothing_h=0.05, tol=0)
    corr = np.corrcoef(result.x, gamma_true)[0, 1]
    chi2 = result.chi2_r_history[-1]
    print(f"[signed j0 kernel] chi2_r={chi2:.3f} corr={corr:.4f}")

    assert corr > 0.99, "should recover the sphere correlation function's shape well"
    assert 0.3 < chi2 < 3.0, "chi2_r should land near 1 with a realistic noise floor"


if __name__ == "__main__":
    test_paper_figure_7()
    test_signed_j0_kernel_sphere_correlation()
    print("OK")
