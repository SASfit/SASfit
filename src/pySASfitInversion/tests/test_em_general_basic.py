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
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "src"))

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


def test_real_data_j0_weighting():
    """test.dat with the j0 kernel (2026-10-10): before the chi2 row
    weighting, the unweighted shifted EM could not get below chi2_r ~5200
    even without smoothing (high-q signal only 1e-5 of the shifted data).
    Now: chi2_r = 1 via the discrepancy principle, and in B-spline mode the
    same p(r) as the Bayesian-evidence solver."""
    import os
    from sasfit_inversion.io_utils import load_sas_data
    from sasfit_inversion.background import fit_background
    from sasfit_inversion.kernel_registry import KERNEL_REGISTRY
    from sasfit_inversion.solver_registry import run_solver, SplineOptions
    d = load_sas_data(os.path.join(os.path.dirname(__file__), "data", "test.dat"))
    q, I, dI = d.q, d.I, d.dI
    b = I - fit_background(q, I, q_min=0.094016, q_max=0.385030, dI=dI).evaluate(q)
    rmax = np.pi / q.min()
    f = KERNEL_REGISTRY["sinc_4pi"].func
    A = build_size_distribution_kernel(q, np.linspace(0, rmax, 95), f, alpha=0.0)
    unw = em_general.solve(A, b, dI, smoothing_h=None, max_iterations=5000, weighting="none")
    wtd = em_general.solve(A, b, dI, smoothing_h=None, max_iterations=5000)
    print(f"[test.dat j0, no smoothing] chi2_r unweighted={unw.chi2_r_history[-1]:.4g} "
          f"weighted={wtd.chi2_r_history[-1]:.4g}")
    assert wtd.chi2_r_history[-1] < 2 < unw.chi2_r_history[-1]
    opt = SplineOptions(20, "log", build_size_distribution_kernel(
        q, np.linspace(0, rmax, 4 * 94 + 1), f, alpha=0.0))
    em_s = run_solver("em_general_signed_kernel", A, b, dI, spline=opt)
    be_s = run_solver("bayesian_evidence", A, b, dI, spline=opt)
    corr = np.corrcoef(em_s.x, be_s.x)[0, 1]
    print(f"[test.dat j0, spline] chi2_r={em_s.chi2_r_history[-1]:.4f}, corr with "
          f"Bayesian evidence={corr:.4f}")
    assert abs(em_s.chi2_r_history[-1] - 1) < 0.05 and corr > 0.99

    # Few r grid points (2026-10-10): with the integration grid tied to
    # q_max (>= 4 points per pi/q_max) and the acceleration safeguard, the
    # spline mode still works at n=15 (13 coefficients); before, the
    # Biggs-Andrews iteration drifted (chi2_r 234) and the coarse 57-point
    # integration grid undersampled j0.
    n = 15
    nq = max(4 * (n - 1) + 1, int(np.ceil(4 * rmax * q.max() / np.pi)) + 1)
    A15 = build_size_distribution_kernel(q, np.linspace(0, rmax, n), f, alpha=0.0)
    o15 = SplineOptions(13, "log", build_size_distribution_kernel(q, np.linspace(0, rmax, nq), f, alpha=0.0))
    e15 = run_solver("em_general_signed_kernel", A15, b, dI, spline=o15)
    b15 = run_solver("bayesian_evidence", A15, b, dI, spline=o15)
    c15 = np.corrcoef(e15.x, b15.x)[0, 1]
    print(f"[test.dat j0, n=15 spline] chi2_r={e15.chi2_r_history[-1]:.3f}, corr={c15:.4f}")
    assert e15.chi2_r_history[-1] < 2.5 and c15 > 0.99

    # MaxEnt regularization for the signed EM (2026-10-10): positive/negative
    # entropy, adaptive prior. Must match the smoothing variant and, in
    # spline mode, the Bayesian-evidence p(r).
    be = run_solver("bayesian_evidence", A, b, dI)
    me = run_solver("em_general_signed_kernel", A, b, dI, regularization="maxent_adaptive")
    cm = np.corrcoef(me.x, be.x)[0, 1]
    print(f"[test.dat j0, MaxEnt adaptive] chi2_r={me.chi2_r_history[-1]:.3f}, corr={cm:.4f}")
    assert abs(me.chi2_r_history[-1] - 1) < 0.05 and cm > 0.99
    assert "MaxEnt (adaptive prior)" in me.diagnostics["lambda_selection"]
    ms = run_solver("em_general_signed_kernel", A, b, dI, spline=opt, regularization="maxent_adaptive")
    cs = np.corrcoef(ms.x, be_s.x)[0, 1]
    print(f"[test.dat j0, MaxEnt adaptive, spline] chi2_r={ms.chi2_r_history[-1]:.3f}, corr={cs:.4f}")
    assert abs(ms.chi2_r_history[-1] - 1) < 0.05 and cs > 0.995
    # constant prior: must run and return a finite result (convergence is slow)
    mc = run_solver("em_general_signed_kernel", A, b, dI, spline=opt, regularization="maxent_constant")
    print(f"[test.dat j0, MaxEnt constant, spline] chi2_r={mc.chi2_r_history[-1]:.3f}, "
          f"corr={np.corrcoef(mc.x, be_s.x)[0, 1]:.4f}")
    assert np.all(np.isfinite(mc.x))


if __name__ == "__main__":
    test_paper_figure_7()
    test_signed_j0_kernel_sphere_correlation()
    test_real_data_j0_weighting()
    print("OK")
