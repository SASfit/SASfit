"""
Regularization operators L for the penalty term ||L x||^2 in

    chi^2 = ||b - A x||_w^2 + lambda^2 ||L x||^2

These are standard finite-difference operators (Donatelli & Reichel, 2014,
as cited in the SASfit JAC 2022 paper). The C code (sasfit_extrapol.c)
implements 9 boundary-condition variants; this starts with the three most
common ones (identity, 1st derivative, 2nd derivative with free/natural
boundaries). The remaining Dirichlet/Neumann boundary combinations from the
C code can be added once we've confirmed exactly which ones the existing
SASfit fits actually rely on -- no point guessing boundary conventions that
won't be exercised.
"""
from __future__ import annotations

import numpy as np
import scipy.sparse as sp


def identity_operator(n: int) -> sp.spmatrix:
    """L = I. Penalizes the magnitude of the solution itself."""
    return sp.identity(n, format="csr")


def first_derivative_operator(n: int) -> sp.spmatrix:
    """
    Discrete first-derivative operator, (n-1) x n, free boundary
    (no assumption about the derivative at the endpoints).

        (Lx)_i = x_{i+1} - x_i
    """
    L = sp.diags([-np.ones(n - 1), np.ones(n - 1)], offsets=[0, 1], shape=(n - 1, n))
    return L.tocsr()


def second_derivative_operator(n: int) -> sp.spmatrix:
    """
    Discrete second-derivative operator, (n-2) x n, free boundary.

        (Lx)_i = x_{i+1} - 2 x_i + x_{i-1}
    """
    L = sp.diags(
        [np.ones(n - 2), -2 * np.ones(n - 2), np.ones(n - 2)],
        offsets=[0, 1, 2],
        shape=(n - 2, n),
    )
    return L.tocsr()


def hansen_smoothness_cholesky(n: int) -> np.ndarray:
    """
    Cholesky factor L (n x n, dense) such that L^T L = A_hansen, where
    A_hansen is the smoothness-regularization Hessian of Hansen (2000),
    J. Appl. Cryst. 33, 1415-1421, eq. 19:

        S = sum_{j=2}^{N-1} [f_j - (f_{j-1}+f_{j+1})/2]^2 + (1/2) f_1^2 + (1/2) f_N^2

    which Hansen states (text following eq. 19) works out to the matrix

        a_ij = 1        for i == j
        a_ij = -1/2      for |i - j| == 1
        a_ij = 0         otherwise

    with det(A_hansen) = (1/2)^N (N+1) -- i.e. NONZERO, so A_hansen is
    full rank, unlike `second_derivative_operator` above (which has a
    2-dimensional null space: constants and linear ramps).

    WHY THIS MATTERS (found 2026-10-05 debugging the signed j0-kernel PDDF
    inversion on real data, chi2_r stuck at ~500-600 for any smoothing
    strength no matter how tuned): `second_derivative_operator`'s penalty
    is a pure curvature/roughness penalty with NO constraint at all on the
    endpoints' absolute values, so the regularized problem inherits two
    completely free directions (any constant offset, any linear ramp
    added to the solution leaves the penalty unchanged). Combined with a
    kernel matrix that is itself extremely ill-conditioned (~1e16 for the
    j0/PDDF kernel on a 91-point/150-r-point real-data grid), those two
    free directions are enough to make GCV- or L-curve-selected lambda
    choose a wildly undersmoothed, highly oscillatory solution no matter
    what the target chi2_r is.

    Hansen's regularizer fixes this NOT by introducing basis functions
    (the number-of-basis-functions restriction of Glatter's original 1977
    method is exactly what Hansen's introduction calls obsolete once
    Steenstrup's (1985) algorithm allows ~1000 direct point values) but by
    adding the explicit boundary assumption p(0) = p(Dmax) = 0 directly
    into the regularization functional (the added (1/2)f_1^2 + (1/2)f_N^2
    terms, which is exactly the assumption that p(r) vanishes just outside
    the endpoints). This removes the 2-dimensional null space and
    dramatically improves conditioning -- a real physical assumption (the
    pair-distance/size distribution genuinely must vanish at r=0 and at
    the true maximum dimension), not an arbitrary numerical trick.

    Usage: pass this L straight into bayesian_evidence.log_evidence /
    evidence_search in place of second_derivative_operator(n) -- that
    module already treats L generically (S = p^T L^T L p), so no changes
    are needed there, only which L is supplied.
    """
    A_hansen = np.zeros((n, n))
    np.fill_diagonal(A_hansen, 1.0)
    idx = np.arange(n - 1)
    A_hansen[idx, idx + 1] = -0.5
    A_hansen[idx + 1, idx] = -0.5

    # Sanity check against Hansen's own stated closed-form determinant --
    # if this ever fails, the matrix above no longer matches eq. 19.
    expected_log_det = n * np.log(0.5) + np.log(n + 1)
    sign, actual_log_det = np.linalg.slogdet(A_hansen)
    assert sign > 0, "A_hansen should be positive definite (Hansen's own result)"
    assert abs(actual_log_det - expected_log_det) < 1e-6 * max(abs(expected_log_det), 1.0), (
        f"A_hansen determinant mismatch vs Hansen (2000) eq. 19: "
        f"got log|det|={actual_log_det:.6g}, expected {expected_log_det:.6g}"
    )

    return np.linalg.cholesky(A_hansen).T  # upper-triangular L with L^T L = A_hansen


def hann_taper(r: np.ndarray, r_max: float, taper_frac: float = 0.25) -> np.ndarray:
    """
    Weight w(r) in [0,1]: 1 in the interior, smoothly (raised-cosine /
    Hann-style) tapering to 0 over the last taper_frac of [0, r_max].
    Reparametrizing p(r) = w(r) * p_free(r) and solving for p_free (plain,
    unconstrained smoothness, no special boundary knot) replaces Hansen
    (2000)'s HARD p(Dmax)=0 boundary condition with a soft, gradual decay.

    Motivation: Hansen's hard boundary is mathematically a sharp edge in
    r-space. p(r) and I(q) are related by (essentially) a Fourier sine
    transform, and a sharp edge/discontinuity in one domain convolves the
    conjugate domain's transform with a sinc function -- i.e. a hard
    r=Dmax cutoff is expected to leak oscillatory "ringing" into I(q)
    with slowly-decaying (~1/q) sidelobes that persist out to high q,
    wherever the edge sits (confirmed empirically: diagnose_hansen_dmax_scan.py
    and diagnose_hansen_nr_scan.py both found the real-data high-q
    noise-tracking essentially unchanged across Dmax/n_r/r_min). A
    gradual (Hann) rolloff instead of a sharp edge is the standard
    apodization/windowing fix for this kind of truncation ringing in
    signal processing.

    Validated on a synthetic sphere test (known ground truth, 5 independent
    noise draws, 2026-10-07): the Hann-tapered reconstruction had LOWER
    RMSE against the true p(r) than Hansen's hard boundary in every single
    trial (e.g. 0.023-0.041 vs 0.036-0.057), and matched the true
    solution's smoothness (sum-of-squared-second-differences roughness)
    far more closely (closer to the true curve's own roughness by a factor
    of ~5, vs the hard boundary's solution being ~7x rougher than truth).
    High-q chi2 contribution was also consistently, if modestly, closer to
    1 (less overfit) under tapering in every trial. Caveat: that synthetic
    test (a smooth, textbook homogeneous-sphere PDDF) did not reproduce
    the SEVERITY of the real test.dat high-q pathology (chi2 contribution
    ~0.02 there vs ~0.5-1.2 in the synthetic test), so this validates the
    mechanism and direction, not a guarantee of fully closing that gap on
    real data -- see tests/diagnose_hansen_tapered.py for the real-data
    comparison.

    taper_frac=0.25 (the last quarter of the r-range) is a reasonable
    default, not independently tuned; a shorter/longer taper trades off
    how much of the resolved r-range is affected vs how gradual the edge
    is.
    """
    r = np.asarray(r, dtype=float)
    w = np.ones_like(r)
    edge_start = r_max * (1 - taper_frac)
    in_taper = r >= edge_start
    denom = r_max - edge_start
    if denom > 0:
        x = (r[in_taper] - edge_start) / denom
        w[in_taper] = 0.5 * (1 + np.cos(np.pi * np.clip(x, 0, 1)))
    w[r > r_max] = 0.0
    return w


def hansen_smoothness_two_segment(n: int, split_idx: int) -> tuple[np.ndarray, np.ndarray, int, int]:
    """
    Split Hansen (2000)'s boundary-constrained smoothness penalty into two
    additive pieces by r-index, enabling a two-hyperparameter extension
    (lambda_lo for r-indices below split_idx, lambda_hi above) of his
    single-global-lambda method -- implemented here because an empirical
    investigation (tests/diagnose_hansen_lambda_balance.py,
    diagnose_hansen_dmax_scan.py, diagnose_hansen_nr_scan.py, all run on
    real data, 2026-10-06) showed that no single global lambda, at any
    Dmax, n_r, or r_min, can simultaneously give a well-sized fit (chi2
    contribution ~1) in both the low/mid-q region and the high-q region:
    the high-q Debye/sinc kernel lets small, cheap wiggles in p(r) fit
    noise there almost for free under any uniform smoothing strength.

    Derivation: Hansen's penalty (his eq. 19) is
        S(p) = (1/2) sum_{i=0}^{n-2} (p_i - p_{i+1})^2
               + (1/2) p_0^2 + (1/2) p_{n-1}^2
    which expands to the p^T A p form implemented in
    hansen_smoothness_cholesky (diag=1, off-diag=-1/2). Crucially, S(p) is
    a SUM of independent local terms -- one per consecutive pair, plus the
    two boundary terms -- so it can be partitioned by index with no
    approximation:
        A_lo = sum_{i=0}^{split_idx-1} (1/2) C_i  +  (1/2) e_0 e_0^T
        A_hi = sum_{i=split_idx}^{n-2} (1/2) C_i  +  (1/2) e_{n-1} e_{n-1}^T
    where C_i is the rank-2 matrix representing (p_i - p_{i+1})^2. By
    construction A_lo + A_hi == hansen_smoothness_cholesky(n)'s full
    A_hansen exactly (verified below), so a two-lambda penalty
    lam_lo * p^T A_lo p + lam_hi * p^T A_hi p reduces exactly to Hansen's
    original single-lambda penalty when lam_lo == lam_hi.

    Returns (A_lo, A_hi, n_lo_terms, n_hi_terms) where n_lo_terms =
    split_idx and n_hi_terms = n-1-split_idx are the counts of
    consecutive-difference terms assigned to each segment (n_lo_terms +
    n_hi_terms == n-1 always) -- used as the log(lambda) prefactor
    exponents in the generalized evidence formula (see
    solvers/bayesian_evidence.py's log_evidence_blocks), chosen
    specifically so that the two-term prefactor
    0.5*n_lo_terms*log(lam_lo) + 0.5*n_hi_terms*log(lam_hi) collapses
    exactly to Vestergaard & Hansen (2006) eq. 6's 0.5*(N-1)*log(lam) term
    when lam_lo == lam_hi == lam.

    split_idx must be in [1, n-2] (each segment needs at least one
    difference term).
    """
    if not (1 <= split_idx <= n - 2):
        raise ValueError(f"split_idx must be in [1, {n - 2}], got {split_idx}")

    A_lo = np.zeros((n, n))
    A_hi = np.zeros((n, n))

    for i in range(n - 1):
        target = A_lo if i < split_idx else A_hi
        target[i, i] += 0.5
        target[i + 1, i + 1] += 0.5
        target[i, i + 1] += -0.5
        target[i + 1, i] += -0.5

    A_lo[0, 0] += 0.5
    A_hi[n - 1, n - 1] += 0.5

    n_lo_terms = split_idx
    n_hi_terms = (n - 1) - split_idx

    # Sanity check: the split must reconstruct the original operator
    # exactly (not approximately) -- this is an algebraic identity, not a
    # numerical approximation, so use a tight tolerance.
    L_full_check = hansen_smoothness_cholesky(n)
    A_full_check = L_full_check.T @ L_full_check
    recombined = A_lo + A_hi
    max_err = np.max(np.abs(recombined - A_full_check))
    assert max_err < 1e-10, (
        f"hansen_smoothness_two_segment: A_lo+A_hi should exactly equal the "
        f"full Hansen operator, max diff={max_err:.3g}"
    )

    return A_lo, A_hi, n_lo_terms, n_hi_terms


OPERATORS = {
    "identity": identity_operator,
    "first_derivative": first_derivative_operator,
    "second_derivative": second_derivative_operator,
    "hansen_smoothness": hansen_smoothness_cholesky,
}
