"""
EM for signed kernels and signed solutions, following Chae, Martin & Walker
(2018), "On an algorithm for solving Fredholm integrals of the first kind",
Statistics and Computing, Section 6 ("General Fredholm equation") -- read
directly from the paper (copy in .../Expectation Maximation/), 2026-09-26.

Why this exists: the standard EM / Lucy-Richardson update (em.py) is a
multiplicative fixed-point iteration that is only valid for a non-negative
kernel AND a non-negative solution. In this package the sphere form-factor
kernel is always >= 0, but the j0(qr) = sin(qr)/(qr) kernel (a signed,
oscillating kernel) is not, and pair-distance-type solutions may be
negative as well.

The paper's construction (its Section 6), which this module implements:

  (a) Signed SOLUTION, non-negative kernel k  (paper eq. 10):
      shift the unknown by a constant t > 0:
          p~ = p + t,    f~(x) = f(x) + t * integral k(x,theta) dtheta,
      so that f~(x) = integral k(x,theta) p~(theta) dtheta with p~ >= 0
      (needs t > -min p). "The value of t rarely affects the convergence
      rate in practice", so t can simply be large.

  (b) Signed KERNEL: write k = k+ - k-, both >= 0. On Theta=[0,1]:
          f = int k+ p - int k- p
      Extend the domain to [0,2] with
          k~(x,theta) = k+(x,theta)      theta in [0,1]
                        k-(x,theta-1)    theta in (1,2]
          p~(theta)   = p(theta)         theta in [0,1]
                        -p(theta-1)      theta in (1,2]
      giving f = int_0^2 k~ p~ (paper eq. 12): a NON-NEGATIVE kernel, but
      the extended unknown p~ = [p, -p] is itself SIGNED -- so step (a)'s
      shift must be applied on top. (My first reconstruction of this module,
      written from notebook fragments before I had the paper, did (b) but
      omitted (a): it ran the multiplicative update directly on the signed
      vector [p, -p], which is invalid and diverged. The pysasem notebook's
      "subtract the offset value before duplicating p(r) for the negative
      kernel" is this same shift.)

Non-uniqueness caveat, from the paper itself: the extended problem has more
solutions than [p, -p]-structured ones, so "the restriction of [the
extended solution] on [0,1] may not be a solution of the original equation.
... In many examples, however, the simple approach (12) works well."
Following the pysasem notebook's practical rule, the structure [p, -p] is
re-imposed after every update by keeping only the first half (the k+ block)
and rebuilding the second half from it -- `structure="first_half"`; an
antisymmetrized alternative is available as `structure="antisym"`.

Implementation details beyond the paper:
  - Columns of the extended kernel that are entirely zero (e.g. a k- column
    for a theta where the kernel is never negative) would give 0/0 in the
    EM correction; those components are simply left unchanged.
  - Optional smoothing (same tridiagonal operator as em.py, JAC 2022 eq. 43)
    applied to the signed solution p each iteration, so the usual outer
    smoothing-parameter searches (lambda_search.py) work unchanged.
  - Default offset t: from a rough automatic signed estimate (ARLS) as
    t = 10 * max|p_est| -- generous on purpose, since results are nearly
    t-independent once t exceeds the solution's magnitude.

ACCELERATION (2026-10-03, per Joachim's request that every EM-family
technique use it, not just plain em.py): wrapped in Biggs-Andrews
acceleration (acceleration.py), default ON (accelerate=True), same
convention as em.py and maxent.py. The one wrinkle specific to this
solver: p is SIGNED (not just non-negative), and is kept inside
[-lim, lim] by a two-sided clip each iteration (lim = 0.999*t, so that
the shifted p~ = p + t stays strictly positive). acceleration.py's own
clip_nonnegative option only supports a one-sided floor, so that generic
clip is disabled here (clip_nonnegative=False) and the two-sided clip is
instead applied inside this module's own `one_step` closure, where both
the pre-step input and the post-step output are clipped. Validated
against test_em_general_basic.py: acceleration left the paper's own
Fig. 7 benchmark reproduction and the signed j0(qr)-kernel sphere-recovery
test unchanged to within numerical noise (corr=1.00000, max_err=0.0090,
improved slightly from 0.0118 pre-acceleration; signed kernel test
chi2_r=0.666, corr=0.9969, unchanged).
"""
from __future__ import annotations

import numpy as np

from .base import SolverResult, chi2_r, g_test
from .em import _smoothing_matrix
from . import arls
from . import acceleration


def split_kernel(K: np.ndarray) -> tuple[np.ndarray, np.ndarray]:
    """K = K+ - K-, with K+ = max(K,0) and K- = max(-K,0), both >= 0."""
    return np.where(K > 0, K, 0.0), np.where(K < 0, -K, 0.0)


def default_offset(K: np.ndarray, b: np.ndarray, factor: float = 10.0) -> float:
    """Generous offset t > max|p|, from a rough automatic signed estimate."""
    p_est = arls.arls(K, b)
    scale = float(np.max(np.abs(p_est)))
    if not np.isfinite(scale) or scale == 0.0:
        scale = float(np.linalg.norm(b) / max(np.linalg.norm(K, 1), 1e-300))
    return factor * max(scale, 1e-12)


def _em_step_safe(u: np.ndarray, A: np.ndarray, b: np.ndarray, col_sums: np.ndarray) -> np.ndarray:
    """Multiplicative EM update (as em.em_step) that leaves components with a
    zero kernel column unchanged instead of producing 0/0."""
    Au = A @ u
    Au = np.where(Au <= 0, np.finfo(float).eps, Au)
    ok = col_sums > 0
    correction = np.ones_like(u)
    correction[ok] = (A.T @ (b / Au))[ok] / col_sums[ok]
    return u * correction


def solve(
    K: np.ndarray,
    b: np.ndarray,
    db: np.ndarray,
    t: float | None = None,
    p0: np.ndarray | None = None,
    max_iterations: int = 10_000,
    smoothing_h: float | None = None,
    tol: float = 1e-8,
    structure: str = "first_half",
    K_pos: np.ndarray | None = None,
    K_neg: np.ndarray | None = None,
    accelerate: bool = True,
    kin_set_maa: int = 2,
) -> SolverResult:
    """
    Parameters
    ----------
    K : (M, N) signed kernel. Also fine if K >= 0 (then K- is empty and this
        reduces to the shifted-EM of the paper's eq. 10, allowing a signed p).
    b, db : data and uncertainties.
    t : offset (see module docstring); default from `default_offset`.
    p0 : initial signed solution (default: zeros, i.e. shifted start p~ = t).
    smoothing_h : if given, smooth p each iteration with the tridiagonal
        operator of em.py (0 < h <~ 0.3).
    tol, max_iterations : stop when ||p_new - p_old|| <= tol (same gNorm
        criterion as em.py / the C code's FP_step).
    structure : "first_half" (notebook's rule, default) or "antisym".
    K_pos, K_neg : optional explicit non-negative decomposition K = K_pos -
        K_neg (the paper's own example uses smooth Gaussians rather than the
        pointwise positive/negative parts); default is the pointwise split.
    accelerate : if True (default), wrap the per-iteration update in
        Biggs-Andrews acceleration (acceleration.py) instead of running it
        as plain Picard iteration. Same final fixed point, typically far
        fewer iterations. See module docstring for the two-sided-clip
        handling this requires.
    kin_set_maa : Biggs-Andrews extrapolation order/formula, passed through
        to acceleration.biggs_andrews when accelerate=True.
    """
    if structure not in ("first_half", "antisym"):
        raise ValueError("structure must be 'first_half' or 'antisym'")

    n = K.shape[1]
    if K_pos is None or K_neg is None:
        K_pos, K_neg = split_kernel(K)
    K_ext = np.hstack([K_pos, K_neg])          # (M, 2N), entirely >= 0
    col_sums = K_ext.sum(axis=0)
    row_sums = K_ext.sum(axis=1)

    if t is None:
        t = default_offset(K, b)
    b_shift = b + t * row_sums                  # f~ = f + t * int k~ dtheta

    S = _smoothing_matrix(n, smoothing_h) if smoothing_h is not None else None
    p0 = np.zeros(n) if p0 is None else np.asarray(p0, dtype=float).copy()
    lim = 0.999 * t                             # keep p~ = p + t strictly positive

    def one_step(p_in: np.ndarray) -> np.ndarray:
        p_in = np.clip(p_in, -lim, lim)
        u = np.concatenate([p_in + t, -p_in + t])   # shifted [p, -p]  (>= 0)
        u_new = _em_step_safe(u, K_ext, b_shift, col_sums)
        p_first = u_new[:n] - t
        if structure == "antisym":
            p_out = 0.5 * (p_first - (u_new[n:] - t))
        else:
            p_out = p_first
        if S is not None:
            p_out = S @ p_out
        return np.clip(p_out, -lim, lim)

    chi2_history, roughness_history, g_norm_history = [], [], []

    if accelerate:
        p_prev_box = [p0.copy()]

        def on_iterate(p_new):
            g_norm_history.append(float(np.linalg.norm(p_new - p_prev_box[0])))
            p_prev_box[0] = p_new.copy()
            fitted_b_local = K @ p_new
            chi2_history.append(chi2_r(b, fitted_b_local, db))
            roughness_history.append(float(np.sum(np.diff(p_new) ** 2)))

        result = acceleration.biggs_andrews(
            one_step, p0, max_iterations=max_iterations, tol=tol,
            kin_set_maa=kin_set_maa, clip_nonnegative=False,
            on_iterate=on_iterate,
        )
        p, n_iter, converged = result.x, result.n_iterations, result.converged
    else:
        p = p0.copy()
        converged = False
        n_iter = 0
        for n_iter in range(1, max_iterations + 1):
            p_old = p
            p = one_step(p)
            g_norm = float(np.linalg.norm(p - p_old))
            g_norm_history.append(g_norm)
            fitted_b = K @ p
            chi2_history.append(chi2_r(b, fitted_b, db))
            roughness_history.append(float(np.sum(np.diff(p) ** 2)))
            if g_norm <= tol:
                converged = True
                break

    fitted_b = K @ p
    try:
        g_final = g_test(np.abs(b) + 1e-300, np.abs(fitted_b) + 1e-300)
    except Exception:
        g_final = float("nan")
    return SolverResult(
        x=p, fitted_b=fitted_b, n_iterations=n_iter,
        chi2_r_history=chi2_history, roughness_history=roughness_history,
        converged=converged,
        diagnostics={
            "offset_t": t, "structure": structure,
            "g_norm_history": g_norm_history, "g_test_final": g_final,
            "method": "Chae, Martin & Walker 2018, Sec. 6 (kernel split + shift)",
        },
    )
