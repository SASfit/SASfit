import numpy as np
import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

from sasfit_inversion.kernels import build_size_distribution_kernel
from sasfit_inversion.regularization import second_derivative_operator
from sasfit_inversion.solvers import general_tikhonov


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

    L = second_derivative_operator(len(s)).toarray()
    print(f"L shape: {L.shape} (n={len(s)}, expect n-2 rows -> null space dim 2)")

    lam_grid = np.geomspace(1e-4, 1e2, 100)
    lam_opt, g = general_tikhonov.general_tikhonov_gcv(A, L, b, lam_grid)
    x = general_tikhonov.general_tikhonov(A, L, b, lam_opt)

    corr = np.corrcoef(x, N_true)[0, 1]
    print(f"[General-form Tikhonov + GCV] lambda={lam_opt:.5f}, correlation={corr:.3f}")

    # Verify the transform is mathematically exact against a direct
    # normal-equations solve -- this is the real correctness check.
    lhs = A.T @ A + lam_opt**2 * (L.T @ L)
    rhs = A.T @ b
    x_direct = np.linalg.solve(lhs, rhs)
    obj_transform = np.sum((A @ x - b) ** 2) + lam_opt**2 * np.sum((L @ x) ** 2)
    obj_direct = np.sum((A @ x_direct - b) ** 2) + lam_opt**2 * np.sum((L @ x_direct) ** 2)
    print(f"objective match: transform={obj_transform:.6f}, direct={obj_direct:.6f}")

    assert np.allclose(x, x_direct, atol=1e-6), \
        "the QR transform must reproduce the direct normal-equations solution exactly"
    # NOTE: correlation itself tops out around 0.86 on this test problem --
    # see the module docstring's PERFORMANCE FINDING. Not asserting a high
    # correlation bar here since that would be asserting something false.
    print("OK (transform verified exact; see docstring re: correlation ceiling)")


if __name__ == "__main__":
    main()
