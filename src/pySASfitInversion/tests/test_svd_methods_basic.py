import numpy as np
import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "src"))

from sasfit_inversion.kernels import build_size_distribution_kernel
from sasfit_inversion.solvers import svd_methods


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

    svd = svd_methods.compute_svd(A)
    print(f"singular values span: {svd.s.max():.3e} to {svd.s.min():.3e} "
          f"(condition number {svd.s.max()/svd.s.min():.2e})")

    # TSVD with GCV-selected truncation
    k_opt, g_tsvd = svd_methods.gcv_select_tsvd(svd, b, k_max=40)
    x_tsvd = svd_methods.tsvd(svd, b, k_opt)
    corr_tsvd = np.corrcoef(x_tsvd, N_true)[0, 1]
    print(f"[TSVD + GCV] k={k_opt}, correlation={corr_tsvd:.3f}")

    # Tikhonov with GCV-selected lambda
    lam_grid = np.geomspace(1e-6, 1.0, 200)
    lam_opt, g_tik = svd_methods.gcv_select_tikhonov(svd, b, lam_grid)
    x_tik = svd_methods.tikhonov(svd, b, lam_opt)
    corr_tik = np.corrcoef(x_tik, N_true)[0, 1]
    print(f"[Tikhonov + GCV] lambda={lam_opt:.5f}, correlation={corr_tik:.3f}")

    assert corr_tsvd > 0.5, "TSVD+GCV should at least loosely recover the truth"
    assert corr_tik > 0.7, "Tikhonov+GCV should recover the truth reasonably well"
    print("OK")


if __name__ == "__main__":
    main()
