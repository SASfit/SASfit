"""
Validates acceleration.py's Biggs-Andrews implementation two ways:

  1. kin_set_maa=0 must reduce to plain, unaccelerated Picard iteration
     (sanity-checks the order-0 branch against a hand-rolled Picard loop
     on the same fixed-point map).

  2. On this library's own EM+smoothing problem (the real JAC 2022
     benchmark dataset), acceleration should reach (approximately) the
     same fixed point as plain Picard iteration in dramatically fewer
     iterations -- the actual motivation for having this module at all.
"""
import numpy as np
import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

from sasfit_inversion.io_utils import load_sas_data
from sasfit_inversion.kernel_registry import KERNEL_REGISTRY
from sasfit_inversion.kernels import build_size_distribution_kernel
from sasfit_inversion.solvers import em, acceleration

DATA_PATH = os.path.join(os.path.dirname(__file__), "data", "test.dat")


def test_maa_zero_matches_plain_picard():
    rng = np.random.default_rng(0)
    A = rng.uniform(0.1, 1.0, size=(20, 15))
    x_true = rng.uniform(0.1, 1.0, size=15)
    b = A @ x_true

    def fp(x):
        Ax = np.where(A @ x <= 0, 1e-300, A @ x)
        return x * ((A.T @ (b / Ax)) / A.sum(axis=0))

    x0 = np.full(15, 0.5)

    x_picard = x0.copy()
    for _ in range(50):
        x_picard = fp(x_picard)

    result = acceleration.biggs_andrews(fp, x0, max_iterations=50, tol=0, kin_set_maa=0)

    diff = np.max(np.abs(result.x - x_picard)) / np.max(np.abs(x_picard))
    print(f"[maa=0 vs plain Picard] relative diff = {diff:.2e}")
    assert diff < 1e-10, "kin_set_maa=0 must reduce to plain Picard iteration"


def test_accelerates_real_em_problem():
    data = load_sas_data(DATA_PATH)
    I_sub = data.I - 0.1
    sphere_kernel = KERNEL_REGISTRY["sphere_rg"].func
    r_grid = np.linspace(1, 500, 150)
    A = build_size_distribution_kernel(data.q, r_grid, sphere_kernel, alpha=6.0)

    h = 9.067e-05  # the smoothing value validated in test_pipeline_end_to_end.py

    plain = em.solve(A, I_sub, data.dI, max_iterations=200_000, smoothing_h=h, tol=1e-10, accelerate=False)
    accel = em.solve(A, I_sub, data.dI, max_iterations=5_000, smoothing_h=h, tol=1e-10, accelerate=True)

    print(f"plain:  n_iter={plain.n_iterations} converged={plain.converged} chi2_r={plain.chi2_r_history[-1]:.4f}")
    print(f"accel:  n_iter={accel.n_iterations} converged={accel.converged} chi2_r={accel.chi2_r_history[-1]:.4f}")

    rel_diff = np.max(np.abs(plain.x - accel.x)) / np.max(np.abs(plain.x))
    print(f"relative solution difference: {rel_diff:.3f}")

    assert accel.converged, "the accelerated solve should actually reach its tolerance"
    assert accel.n_iterations < plain.n_iterations / 10, "acceleration should need far fewer iterations"
    assert rel_diff < 0.05, "both should land at (approximately) the same fixed point"


if __name__ == "__main__":
    test_maa_zero_matches_plain_picard()
    test_accelerates_real_em_problem()
    print("OK")
