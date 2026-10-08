"""
General-form Tikhonov regularization: min ||A x - b||^2 + lambda^2 ||L x||^2
for a non-square, rank-deficient L (e.g. our 2nd-derivative operator, which
has a 2-dimensional null space -- constants and linear ramps -- since it's
(n-2) x n).

This is the gap flagged in svd_methods.py: standard-form Tikhonov (L=I) has
no smoothness penalty and underperforms EM+smoothing on the synthetic
sphere test (~0.89 vs ~0.996 correlation). This module closes that gap.

Implemented via Hansen's std_form.m "method 1" (QR-based, not the full
GSVD) -- read directly from regu.zip (the genuine Regularization Tools
package, found 2026-09-23 in .../PerChristianHansen/regu.zip). Reference
cited there: P. C. Hansen, "Rank-Deficient and Discrete Ill-Posed
Problems", SIAM, 1997.

The transform (for L with p < n rows, i.e. a nontrivial null space of
dimension n-p):

  1. QR-factorize L^T = K R  (K is n x n orthogonal, R is n x p triangular).
     K(:,1:p) spans the row space of L; K(:,p+1:n) spans the null space.
  2. QR-factorize A @ K(:,p+1:n) = H T  (the action of A on L's null space).
  3. L_p = K(:,1:p) @ inv(R)^T           -- maps the standard-form solution
                                              back into the constrained part of x
  4. A_s = H(:,n-p+1:m)^T @ A @ L_p      -- transformed, standard-form kernel
     b_s = H(:,n-p+1:m)^T @ b
  5. Solve standard-form Tikhonov on (A_s, b_s) for x_s (ordinary SVD filter
     factors, reusing svd_methods.tikhonov).
  6. Recover x = L_p @ x_s + d, where
     d = K_null @ (T^-1 @ H(:,1:n-p)^T) @ (b - A @ L_p @ x_s)

NOT yet cross-checked numerically against Hansen's actual tikhonov.m output
on a shared test problem (I don't have a running MATLAB to generate a
reference numerically) -- validated here only via the synthetic sphere
sanity test and via each individual formula being copied directly from the
source rather than reconstructed from memory.

IMPLEMENTATION VERIFIED (2026-09-23): general_tikhonov's output matches a
direct normal-equations solve (A^T A + lambda^2 L^T L) x = A^T b to 1e-11,
and the penalized objective matches to machine precision -- the transform
itself is exact.

PERFORMANCE FINDING, not a bug: even at the best possible lambda (found by
exhaustive scan), this 2nd-derivative-penalized Tikhonov tops out at ~0.86
correlation on the single-Gaussian synthetic test and ~0.68 on the bimodal
one -- both well below EM+smoothing's ~0.996. This is consistent with
Fig. 5 of the JAC 2022 paper itself, where the Tikhonov-regularized volume
distributions visibly oscillate more than the EM ones: plain L2 Tikhonov
has no positivity constraint, so for a sharply-peaked or multi-modal
non-negative distribution it can (and does) fit noise with unphysical
negative excursions that EM's multiplicative, inherently-non-negative
update structurally cannot produce. Don't read this as "general_tikhonov
is broken" -- read it as "unconstrained L2 Tikhonov is a genuinely weaker
method than EM for this class of problem," which is also what your own
paper shows.
"""
from __future__ import annotations

from dataclasses import dataclass

import numpy as np
import scipy.linalg as sla

from .svd_methods import compute_svd, tikhonov as _standard_tikhonov, tikhonov_filter_factors


@dataclass
class StandardFormTransform:
    A_s: np.ndarray
    b_s: np.ndarray
    L_p: np.ndarray
    K_null: np.ndarray
    M: np.ndarray
    A: np.ndarray
    b: np.ndarray


def std_form(A: np.ndarray, L: np.ndarray, b: np.ndarray) -> StandardFormTransform:
    """QR-based general-to-standard-form transform (std_form.m, method 1)."""
    m, n = A.shape
    p, np_ = L.shape
    assert np_ == n, "A and L must have the same number of columns"
    assert p < n, "This implementation assumes L has a nontrivial null space (p < n); " \
                  "use svd_methods.tikhonov directly if L is square/invertible"

    # QR of L^T
    K, R = sla.qr(L.T)
    R = R[:p, :]

    K_row = K[:, :p]     # spans row space of L
    K_null = K[:, p:]    # spans null space of L (n-p columns)

    # QR of A @ K_null
    H, T = sla.qr(A @ K_null)
    T = T[: n - p, :]

    L_p = K_row @ np.linalg.inv(R.T)
    M = np.linalg.solve(T, H[:, : n - p].T)

    H_res = H[:, n - p :]  # last (m-(n-p)) columns
    A_s = H_res.T @ A @ L_p
    b_s = H_res.T @ b

    return StandardFormTransform(A_s=A_s, b_s=b_s, L_p=L_p, K_null=K_null, M=M, A=A, b=b)


def recover_x(transform: StandardFormTransform, x_s: np.ndarray) -> np.ndarray:
    """x = L_p @ x_s + d, d = K_null @ M @ (b - A @ L_p @ x_s)."""
    residual = transform.b - transform.A @ (transform.L_p @ x_s)
    d = transform.K_null @ (transform.M @ residual)
    return transform.L_p @ x_s + d


def general_tikhonov(A: np.ndarray, L: np.ndarray, b: np.ndarray, lam: float) -> np.ndarray:
    """
    Solve general-form Tikhonov min ||Ax-b||^2 + lambda^2||Lx||^2 via the
    QR standard-form transform + ordinary SVD filter factors.
    """
    transform = std_form(A, L, b)
    svd_s = compute_svd(transform.A_s)
    x_s = _standard_tikhonov(svd_s, transform.b_s, lam)
    return recover_x(transform, x_s)


def general_tikhonov_gcv(
    A: np.ndarray, L: np.ndarray, b: np.ndarray, lam_grid: np.ndarray
) -> tuple[float, np.ndarray]:
    """
    GCV-select lambda for the general-form problem, using the same GCV
    formula (eq. 2.62-2.63) applied in the transformed standard-form space
    -- valid because the transform is exact and A_s/b_s are the genuine
    standard-form representation of the same problem.
    """
    transform = std_form(A, L, b)
    svd_s = compute_svd(transform.A_s)

    def gcv(lam: float) -> float:
        f = tikhonov_filter_factors(svd_s.s, lam)
        x_s = _standard_tikhonov(svd_s, transform.b_s, lam)
        Ax_s = svd_s.U @ (svd_s.s * (svd_s.Vt @ x_s))
        residual_norm2 = np.sum((Ax_s - transform.b_s) ** 2)
        denom = (svd_s.m - np.sum(f)) ** 2
        return residual_norm2 / denom if denom > 0 else np.inf

    g = np.array([gcv(lam) for lam in lam_grid])
    return float(lam_grid[np.argmin(g)]), g
