"""
SVD-based regularization methods: Truncated SVD (TSVD) and Tikhonov
regularization via filter factors, plus Generalized Cross-Validation (GCV)
for automatic parameter selection.

Source: P. C. Hansen, "Regularization Tools: A Matlab package for analysis
and solution of discrete ill-posed problems", Numerical Algorithms 6
(1994) and the Version 4.1 manual (RTv4manual.pdf), fetched directly from
https://people.compute.dtu.dk/pcha/Regutools/ on 2026-09-23. I could not
get the actual .m source (MathWorks File Exchange requires a login to
download), so these are implemented directly from the manual's equations,
not transliterated from Hansen's code -- cited equation numbers refer to
that manual.

For L = I (standard form, which is what's implemented here -- general-form
L needs the GSVD, eq. 2.11 in the manual, not yet implemented):

  SVD:            A = U diag(s) V^T                          (manual notation)
  TSVD (eq 2.45): x_k = sum_{i=1}^k (u_i^T b / s_i) v_i
  Tikhonov (eq 2.18, filter factors eq 2.49):
                  x_lambda = sum_i f_i (u_i^T b / s_i) v_i,   f_i = s_i^2/(s_i^2+lambda^2)
  GCV (eq 2.62-2.63):
                  G(param) = ||A x_reg - b||^2 / (m - sum_i f_i)^2
                  minimize over param (k for TSVD, lambda for Tikhonov)

CROSS-CHECKED 2026-09-23 against actual source: found Hansen's real IRtools
package (via .../PerChristianHansen/IRtools-master.zip) and its
Extra/TikGCV.m (J. Chung & J. Nagy 2007, updated S. Gazzola 2017). Its
denominator (m1 - m2 + sum(1 - f_i))^2 reduces, for our M>=N standard-form
case, to exactly (M - sum f_i)^2 -- matching gcv_function_tikhonov below
independently derived from the manual. Good agreement between the two
sources. (Note: tikhonov.zip in the same folder turned out NOT to be
Hansen's actual tikhonov.m -- it's an unrelated small demo script using a
different lambda convention (s^2/(s^2+lambda) rather than
s^2/(s^2+lambda^2)) -- flagging so it isn't mistaken for validated source.)

These give an independent, literature-grounded alternative to the
DR_linReg-reverse-engineered path, and are the natural place to plug in
GCV as a fourth lambda-selection criterion alongside the L-curve and
discrepancy-principle searches already in lambda_search.py.

STATUS: only standard-form (L = identity) is implemented here, so there's
no smoothness penalty -- eq. 2.45/2.49 as written use L=I. On the
synthetic sphere test this gets ~0.85-0.89 correlation to the truth,
noticeably worse than EM+smoothing's 0.996, precisely because it lacks a
derivative-based penalty. Hansen's general-form treatment (L = 2nd-
derivative operator, manual section 2.6, via the GSVD of the pair (A,L),
eq. 2.11) is what would close that gap and is the fairer comparison to
SASfit's own lin_Reg -- not implemented here yet; the GSVD is a real
additional piece of numerical machinery, not a small extension of this
module.
"""
from __future__ import annotations

from dataclasses import dataclass

import numpy as np


@dataclass
class SVDSystem:
    U: np.ndarray  # (M, r)
    s: np.ndarray  # (r,) singular values, descending
    Vt: np.ndarray  # (r, N)
    m: int
    n: int


def compute_svd(A: np.ndarray) -> SVDSystem:
    """Hansen's csvd: compact SVD, A = U diag(s) V^T."""
    U, s, Vt = np.linalg.svd(A, full_matrices=False)
    return SVDSystem(U=U, s=s, Vt=Vt, m=A.shape[0], n=A.shape[1])


def tsvd(svd: SVDSystem, b: np.ndarray, k: int) -> np.ndarray:
    """Truncated SVD solution, eq. 2.45: x_k = sum_{i=1}^k (u_i^T b / s_i) v_i."""
    coeffs = (svd.U[:, :k].T @ b) / svd.s[:k]
    return svd.Vt[:k, :].T @ coeffs


def tikhonov_filter_factors(s: np.ndarray, lam: float) -> np.ndarray:
    """Tikhonov filter factors, eq. 2.49: f_i = s_i^2 / (s_i^2 + lambda^2)."""
    return s**2 / (s**2 + lam**2)


def tikhonov(svd: SVDSystem, b: np.ndarray, lam: float) -> np.ndarray:
    """Tikhonov solution via filter factors, eq. 2.18."""
    f = tikhonov_filter_factors(svd.s, lam)
    coeffs = f * (svd.U.T @ b) / svd.s
    return svd.Vt.T @ coeffs


def gcv_function_tikhonov(svd: SVDSystem, b: np.ndarray, lam: float) -> float:
    """
    GCV function for Tikhonov, eq. 2.62 evaluated via the filter-factor
    shortcut eq. 2.63: trace(I - A A_I) = m - (n-p) - sum_i f_i, with p=0
    (L=I, no null-space component) so it simplifies to m - sum_i f_i.
    """
    f = tikhonov_filter_factors(svd.s, lam)
    x = tikhonov(svd, b, lam)
    Ax = svd.U @ (svd.s * (svd.Vt @ x))
    residual_norm2 = np.sum((Ax - b) ** 2)
    denom = (svd.m - np.sum(f)) ** 2
    return residual_norm2 / denom if denom > 0 else np.inf


def gcv_function_tsvd(svd: SVDSystem, b: np.ndarray, k: int) -> float:
    """GCV function for TSVD: same eq. 2.62, with f_i = 1 for i<=k, 0 otherwise."""
    x = tsvd(svd, b, k)
    Ax = svd.U[:, :] @ (svd.s * (svd.Vt @ x))
    residual_norm2 = np.sum((Ax - b) ** 2)
    denom = (svd.m - k) ** 2
    return residual_norm2 / denom if denom > 0 else np.inf


def gcv_select_tikhonov(svd: SVDSystem, b: np.ndarray, lam_grid: np.ndarray) -> tuple[float, np.ndarray]:
    """Pick the lambda in lam_grid minimizing the GCV function."""
    g = np.array([gcv_function_tikhonov(svd, b, lam) for lam in lam_grid])
    return float(lam_grid[np.argmin(g)]), g


def gcv_select_tsvd(svd: SVDSystem, b: np.ndarray, k_max: int) -> tuple[int, np.ndarray]:
    """Pick the truncation k in [1, k_max] minimizing the GCV function."""
    ks = np.arange(1, k_max + 1)
    g = np.array([gcv_function_tsvd(svd, b, k) for k in ks])
    return int(ks[np.argmin(g)]), g
