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


OPERATORS = {
    "identity": identity_operator,
    "first_derivative": first_derivative_operator,
    "second_derivative": second_derivative_operator,
    "hansen_smoothness": hansen_smoothness_cholesky,
}
