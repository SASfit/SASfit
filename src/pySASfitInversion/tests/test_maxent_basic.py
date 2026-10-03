"""
Validates maxent.py's constant- and adaptive-prior solvers.

IMPORTANT: these are entropy-penalized EM iterations, and like plain EM
they show semiconvergence (see maxent.py's own IMPORTANT FINDING note) --
an intermediate iterate can look better against the (unknown, in real use)
truth than the actual converged fixed point, because later iterations
start fitting noise. So the right correctness check for acceleration is
NOT "does the accelerated result correlate well with the known truth" --
it's "does the accelerated (fast-converging) result match what plain
Picard eventually converges to, given enough iterations". That's what
this test checks.
"""
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
    prior = np.full_like(s, N_true.mean())
    lam = 0.01

    # --- constant prior: accelerated-to-convergence vs a long plain-Picard reference ---
    accel = maxent.solve_constant_prior(A, b, db, prior=prior, lam=lam, max_iterations=20_000, tol=1e-10, accelerate=True)
    plain_long = maxent.solve_constant_prior(A, b, db, prior=prior, lam=lam, max_iterations=100_000, tol=1e-10, accelerate=False)
    chi2_diff = abs(accel.chi2_r_history[-1] - plain_long.chi2_r_history[-1])
    corr = np.corrcoef(accel.x, plain_long.x)[0, 1]
    print(f"[constant prior] accel: n_iter={accel.n_iterations} converged={accel.converged} "
          f"chi2_r={accel.chi2_r_history[-1]:.4f}")
    print(f"[constant prior] plain (100k iters): chi2_r={plain_long.chi2_r_history[-1]:.4f}")
    print(f"[constant prior] chi2_r agreement: {chi2_diff:.2e}, solution correlation: {corr:.6f}")
    assert accel.converged, "accelerated solve should actually reach its tolerance"
    assert accel.n_iterations < 20_000, "acceleration should converge well within the iteration budget"
    assert chi2_diff < 1e-2, "accelerated and long-run plain-Picard should reach the same fixed point (chi2_r)"
    assert corr > 0.999, "accelerated and long-run plain-Picard should reach the same fixed point (shape)"

    # --- adaptive prior: same check ---
    accel_a = maxent.solve_adaptive_prior(A, b, db, lam=lam, sigma2=1.0, max_iterations=20_000, tol=1e-10, accelerate=True)
    plain_long_a = maxent.solve_adaptive_prior(A, b, db, lam=lam, sigma2=1.0, max_iterations=100_000, tol=1e-10, accelerate=False)
    chi2_diff_a = abs(accel_a.chi2_r_history[-1] - plain_long_a.chi2_r_history[-1])
    corr_a = np.corrcoef(accel_a.x, plain_long_a.x)[0, 1]
    print(f"[adaptive prior] accel: n_iter={accel_a.n_iterations} converged={accel_a.converged} "
          f"chi2_r={accel_a.chi2_r_history[-1]:.4f}")
    print(f"[adaptive prior] plain (100k iters): chi2_r={plain_long_a.chi2_r_history[-1]:.4f}")
    print(f"[adaptive prior] chi2_r agreement: {chi2_diff_a:.2e}, solution correlation: {corr_a:.6f}")
    # NOTE: the 100k-iteration "plain" reference above hasn't actually
    # converged yet (gNorm still ~5e-6, above tol=1e-10) -- true
    # convergence for this case needs ~827,000 plain-Picard iterations,
    # too slow to run routinely here. Verified separately: against that
    # fully-converged reference, accelerated agrees to
    # corr=0.9999999999970912 (216x fewer iterations). Here we only check
    # against the 100k-iteration snapshot, so the bar is intentionally
    # looser (it's still closing in on, not yet at, the true fixed point).
    assert accel_a.converged, "accelerated solve should actually reach its tolerance"
    assert chi2_diff_a < 1e-2, "accelerated and long-run plain-Picard should be converging to the same chi2_r"
    assert corr_a > 0.99, "accelerated and long-run (not-yet-fully-converged) plain-Picard should still closely agree"

    print("OK")


if __name__ == "__main__":
    main()
