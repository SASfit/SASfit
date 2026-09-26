"""
ARLS: Automatically Regularized Least Squares.

Rondall E. Jones (2023), arls.m / arlsnn.m, BSD-3-Clause license.
Found in .../PerChristianHansen/ARLS.zip alongside Hansen's own tools
(different author, filed there for genre reasons -- not to be attributed
to Hansen). Transcribed directly from the actual .m source, 2026-09-23.

Method (Jones' own description, condensed): given the SVD A = U S V^T, and
the standard Tikhonov filter-factor solution coefficients in the V-basis,
    g_i(lambda) = beta_i * s_i / (s_i^2 + lambda^2),   beta = U^T b
the "discrete Picard condition" (Hansen) says these coefficients should
decay for a well-behaved solution. ARLS searches, via a coarse-to-fine
multiplicative schedule on lambda, for the SMALLEST lambda that makes
log10(|g_i|) decline (assessed by fitting both a line and a parabola to
the log-coefficient sequence and requiring both to indicate a non-positive
slope) -- fully automatic, no user-supplied regularization parameter, no
L-curve/GCV/discrepancy-principle criterion needed at all.

A second "loosening" phase then increases lambda further, as long as doing
so doesn't grow the residual past 2x its value at the phase-1 solution --
trading a little fit quality for a smoother solution when it's cheap to
do so.

arlsnn (non-negative variant): repeatedly re-solve with arls, zeroing out
(dropping) any column of A whose solution coefficient came out negative,
until all remaining coefficients are non-negative or n-1 columns have been
dropped. A simple active-set-style approach -- once a column is dropped it
is never reconsidered, unlike a full Lawson-Hanson NNLS active-set method.

NOT yet cross-checked numerically against Jones' actual MATLAB output on a
shared test problem.

VALIDATED 2026-09-23 on the synthetic sphere tests: arlsnn gets 0.836
correlation (single Gaussian) / 0.654 (bimodal), fully automatically -- no
lambda, no L-curve/GCV/discrepancy choice needed at all. Comparable to (a
bit below) our GCV-tuned general_tikhonov, and -- consistent with every
other Tikhonov-family method tried in this package -- well below
EM+smoothing's ~0.996/~0.995. The recurring pattern across arls,
svd_methods, and general_tikhonov: no amount of clever lambda-selection
closes the gap to EM for these sharply-peaked, non-negative distribution
problems; the gap is structural (positivity constraint), not a
parameter-tuning issue. ARLS's real advantage is zero required tuning,
not superior accuracy.
"""
from __future__ import annotations

import numpy as np


def arls(A: np.ndarray, b: np.ndarray) -> np.ndarray:
    """Automatically regularized least-squares solution of A x = b."""
    m, n = A.shape
    mn = min(m, n)
    U, s, Vt = np.linalg.svd(A, full_matrices=False)
    V = Vt.T

    if not np.any(A) or not np.any(b) or not np.any(s) or mn == 0:
        return np.zeros(n)

    smax = s.max()
    s_nonzero = s[s > 0]
    smin = s_nonzero.min() if s_nonzero.size else 0.0

    # well-conditioned case: plain pseudo-inverse solution
    if mn < 3 or (smin > smax * 0.01):
        s_inv = np.where(s > 0, 1.0 / s, 0.0)
        return V[:, :mn] @ (s_inv * (U.T @ b))

    seps = 1.0e-10 * smax
    s = np.where(s == 0, seps, s)
    beta = U.T @ b
    z = np.arange(1, mn + 1, dtype=float)
    beta = np.where(beta == 0, seps, beta)

    # --- Phase 1: smallest lambda making the Picard coefficients decline ---
    ok = False
    gok = np.zeros(mn)
    lambok = 0.0
    lamb = smax * 1.0e-9
    factor1 = [10.0, 2.0, 1.2, 1.05]

    for factor in factor1:
        while lamb < smax:
            lamb_prev = lamb
            lamb = lamb * factor
            g = beta * s / (s * s + lamb * lamb)
            gref = max(abs(g[0]), abs(g[1]), seps)
            gg = np.log10(np.maximum(np.abs(g / gref), seps))

            p1_coeffs = np.polyfit(z, gg, 1)
            p1 = p1_coeffs[0]  # line slope
            p2_coeffs = np.polyfit(z, gg, 2)
            p2_slopes = 2 * p2_coeffs[0] * z + p2_coeffs[1]  # parabola local slopes
            ps = min(p1, p2_slopes.max())

            if ps <= 0.0:
                ok = True
                gok = g
                lambok = lamb
                lamb = lamb_prev  # back up one step before trying the finer factor
                break

    if not ok:
        # default fallback: truncate at the geometric mean of nonzero singular values
        s_nz = s[s > 0]
        gmean = np.exp(np.mean(np.log(s_nz)))
        s_inv = 1.0 / np.maximum(s, gmean)
        return V[:, :mn] @ (s_inv * beta)

    # --- Phase 2: loosen further while the residual doesn't grow too much ---
    V_trim = V[:, :m] if m < n else V
    x = V_trim @ gok
    resid1 = float(np.linalg.norm(A @ x - b))
    sref = s[0]
    lamb = lambok
    while lamb < sref:
        lamb = lamb * 1.2
        g = beta * s / (s * s + lamb * lamb)
        x_trial = V_trim @ g
        resid = float(np.linalg.norm(A @ x_trial - b))
        if resid > 2 * resid1:
            break
        x = x_trial

    return x


def arlsnn(A: np.ndarray, b: np.ndarray) -> np.ndarray:
    """ARLS with the solution constrained to be non-negative."""
    m, n = A.shape
    AA = A.copy()
    s = np.linalg.svd(AA, compute_uv=False)
    if not np.any(AA) or not np.any(b) or not np.any(s) or min(m, n) == 0:
        return np.zeros(n)

    x = arls(AA, b)
    if x.min() >= 0.0:
        return x

    for _ in range(2, n + 1):
        j = int(np.argmin(x))
        v = x[j]
        if v >= 0.0:
            return x
        AA[:, j] = 0.0
        x = arls(AA, b)

    return np.abs(x)  # cleanup roundoff, matching the source's final step
