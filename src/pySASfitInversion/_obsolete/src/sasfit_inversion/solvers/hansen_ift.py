"""
Steen Hansen's indirect Fourier transform (IFT) approach for SAS, following
Hansen (2000), J. Appl. Cryst. 33, 1415-1421 ("Bayesian estimation of
hyperparameters for indirect Fourier transformation in small-angle
scattering") -- read directly from the paper, 2026-10-04 (Joachim pointed
out that Hansen does NOT reduce to a small basis-function set, correcting
an earlier wrong assumption in this module's design).

What's actually different from Glatter's classic method, per the paper:

  - Hansen does NOT restrict p(r) to a small number of basis functions
    (cubic B-splines etc., ~20-40 as in Glatter 1977). He explicitly calls
    that restriction obsolete given modern computing power and uses up to
    ~1000 points on a plain grid (his eq. 2) -- the same point-grid
    representation already used elsewhere in this package.

  - The regularizer (his eq. 19) is NOT the usual free-boundary
    2nd-derivative operator (regularization.py's second_derivative_operator,
    (n-2) x n, rank-deficient with a 2D null space of constants/ramps).
    He adds the endpoint terms (1/2)p_1^2 and (1/2)p_N^2 to the usual
    interior smoothness sum -- exactly the soft constraint p(0) = p(d) = 0
    (particles have finite extent, so the distance distribution must
    vanish at the domain boundaries) folded directly into the quadratic
    penalty. This makes the resulting n x n matrix

        S_ij =  1     i == j
             = -1/2   |i-j| == 1
             =  0     otherwise

    *full rank* -- the paper gives det(S) = (1/2)^N (N+1) > 0 explicitly --
    unlike the free-boundary operator, which needs a QR-based null-space
    transform (general_tikhonov.py's std_form) just to be usable at all.
    That transform is itself numerically delicate for a severely
    ill-conditioned kernel: diagnosed directly on this package's own
    j0(qr)/PDDF kernel for real SAS data (condition number ~1e16), where
    general_tikhonov_gcv's selected lambda gave chi2_r ~3.5e4 and even an
    exhaustive lambda scan down to 1e-8 only reached chi2_r~37-43, with
    chi2_r actually *increasing* again below that -- a signature of
    numerical noise amplification from the QR null-space separation
    compounding with the kernel's own ill-conditioning, not of the
    regularizer being appropriate but mistuned.

  - Because Hansen's S is invertible, there is no null space to separate:
    the general-form problem min ||Ap-b||^2 + lambda*p^T S p can be solved
    by an ordinary whitening change of variables p = L^-1 p~ with
    S = L^T L (L = Cholesky factor of S, computed once), giving the
    perfectly ordinary standard-form problem

        min || (A L^-1) p~ - b ||^2 + lambda^2 ||p~||^2

    solved with svd_methods' existing SVD-filter-factor Tikhonov -- no QR
    null-space machinery at all, and the filter-factor approach degrades
    gracefully (not catastrophically) as the kernel's singular values
    shrink toward zero, unlike forming normal equations (which squares the
    condition number -- see the note on bayesian_evidence.py below).

  - A second, independent bug fixed here (not present in the paper, a gap
    in this package's own general_tikhonov.py/bayesian_evidence.py): both
    of those accept `db` (per-point measurement uncertainty) as a
    parameter but never actually use it to weight the solve -- only
    afterward, to *report* chi2_r. Hansen's own chi2 (his eq. 6/8) is
    explicitly weighted by 1/sigma_i^2. This module weights A and b by
    1/db *before* the whitening transform, so what's being solved for is
    actually the same quantity being reported.

  - Lambda is selected either by GCV (reusing svd_methods' existing
    filter-factor GCV, just on the whitened+weighted problem) or by
    Bayesian evidence maximization (Hansen's eq. 15-17, essentially the
    same formula as Vestergaard & Hansen 2006's eq. 6 already implemented
    in bayesian_evidence.py -- but that module's _map_solution solves the
    *normal equations* (K^T W K + 2 lam L^T L) x = K^T W b directly, which
    squares the kernel's condition number (via K^T K) and is exactly the
    kind of operation that breaks down for a ~1e16-condition-number
    kernel. This module's evidence calculation instead reuses the same
    whitened-SVD MAP solution as hansen_tikhonov, which never forms K^T K.

SCOPE NOTE: the paper also uses this same evidence machinery to estimate
the maximum dimension d itself (evaluating the evidence over a (lambda, d)
grid) and to produce a posterior-weighted-average p(r) over all (lambda, d)
pairs rather than a single "best" solution (its section 2.3-3). Only the
lambda part is implemented here (d and the r-grid are chosen elsewhere, as
with this package's other solvers) -- extending to a full (lambda, d)
evidence surface is a natural follow-up, not done here.
"""
from __future__ import annotations

from dataclasses import dataclass

import numpy as np
import scipy.linalg as sla

from .base import SolverResult, chi2_r as pkg_chi2_r
from .svd_methods import compute_svd, tikhonov as _standard_tikhonov, tikhonov_filter_factors, SVDSystem


def hansen_smoothness_matrix(n: int) -> np.ndarray:
    """
    Hansen (2000) eq. 19: full-rank n x n smoothness matrix with the
    distribution's endpoints softly constrained toward zero (p(0)=p(d)=0).

        S_ij =  1     i == j
             = -1/2   |i-j| == 1
             =  0     otherwise
    """
    S = np.zeros((n, n))
    idx = np.arange(n)
    S[idx, idx] = 1.0
    if n > 1:
        S[idx[:-1], idx[:-1] + 1] = -0.5
        S[idx[:-1] + 1, idx[:-1]] = -0.5
    return S


@dataclass
class HansenTransform:
    A_tilde: np.ndarray  # (A/db) @ L^-1, the weighted+whitened kernel
    b_tilde: np.ndarray  # b/db, the weighted data
    L: np.ndarray        # Cholesky factor of S (S = L^T L, upper-triangular)


def _build_transform(A: np.ndarray, b: np.ndarray, db: np.ndarray, S: np.ndarray) -> HansenTransform:
    Aw = A / db[:, None]
    bw = b / db
    L = sla.cholesky(S, lower=False)  # S = L^T L
    # A_tilde = Aw @ L^-1, via a triangular solve (avoids forming L^-1
    # explicitly): solve L^T X^T = Aw^T  =>  X = (L^-T Aw^T)^T = Aw L^-1
    A_tilde = sla.solve_triangular(L, Aw.T, lower=False, trans=1).T
    return HansenTransform(A_tilde=A_tilde, b_tilde=bw, L=L)


def _recover_x(transform: HansenTransform, x_tilde: np.ndarray) -> np.ndarray:
    return sla.solve_triangular(transform.L, x_tilde, lower=False)


def hansen_tikhonov(
    A: np.ndarray, b: np.ndarray, db: np.ndarray, lam: float, S: np.ndarray | None = None
) -> np.ndarray:
    """
    Solve min ||(A/db)p - b/db||^2 + lambda^2 ||L p||^2 (L^T L = S, Hansen's
    full-rank endpoint-constrained smoothness matrix by default) via
    whitening + standard SVD-filter Tikhonov -- no QR null-space transform
    needed since S is invertible, and no normal equations are formed.
    """
    n = A.shape[1]
    if S is None:
        S = hansen_smoothness_matrix(n)
    transform = _build_transform(A, b, db, S)
    svd = compute_svd(transform.A_tilde)
    x_tilde = _standard_tikhonov(svd, transform.b_tilde, lam)
    return _recover_x(transform, x_tilde)


def hansen_tikhonov_gcv(
    A: np.ndarray, b: np.ndarray, db: np.ndarray, lam_grid: np.ndarray, S: np.ndarray | None = None
) -> tuple[float, np.ndarray]:
    """GCV-select lambda for the whitened+weighted problem."""
    n = A.shape[1]
    if S is None:
        S = hansen_smoothness_matrix(n)
    transform = _build_transform(A, b, db, S)
    svd = compute_svd(transform.A_tilde)

    def gcv(lam: float) -> float:
        f = tikhonov_filter_factors(svd.s, lam)
        x_tilde = _standard_tikhonov(svd, transform.b_tilde, lam)
        Ax = svd.U @ (svd.s * (svd.Vt @ x_tilde))
        residual_norm2 = np.sum((Ax - transform.b_tilde) ** 2)
        denom = (svd.m - np.sum(f)) ** 2
        return residual_norm2 / denom if denom > 0 else np.inf

    g = np.array([gcv(lam) for lam in lam_grid])
    return float(lam_grid[np.argmin(g)]), g


@dataclass
class HansenEvidenceResult:
    lam: float
    log_evidence: float
    x: np.ndarray
    n_good_params: float


def hansen_evidence(
    A: np.ndarray, b: np.ndarray, db: np.ndarray, lam: float, S: np.ndarray | None = None
) -> HansenEvidenceResult:
    """
    Bayesian evidence (Hansen 2000 eq. 15-17 / Vestergaard & Hansen 2006
    eq. 6) for this lambda, computed on the whitened+weighted problem so
    no normal equations (K^T K) are ever formed -- robust for the
    ill-conditioned kernels this module exists for.

    In the whitened+weighted coordinates p~ (where p = L^-1 p~), the
    penalty lambda^2||p~||^2 and the data term ||A_tilde p~ - b_tilde||^2
    are both already in "flat metric" form, so the Hessian of
    (lambda^2 * p~^T p~ + chi2) wrt p~ is simply
        2*lambda^2*I + 2*A_tilde^T A_tilde = 2*(lambda^2*I + sum s_i^2 ...)
    whose eigenvalues are directly 2*(lambda^2 + s_i^2) from the SVD of
    A_tilde -- no explicit matrix products or determinants needed, which
    is both exact and numerically trivial regardless of how ill-
    conditioned A_tilde is.
    """
    n = A.shape[1]
    if S is None:
        S = hansen_smoothness_matrix(n)
    transform = _build_transform(A, b, db, S)
    svd = compute_svd(transform.A_tilde)

    x_tilde = _standard_tikhonov(svd, transform.b_tilde, lam)
    x = _recover_x(transform, x_tilde)

    fitted_b = A @ x
    chi2 = float(np.sum(((fitted_b - b) / db) ** 2))
    pen = float(x_tilde @ x_tilde)  # ||p~||^2 == p^T S p in original coordinates

    s2 = svd.s ** 2
    m = svd.m
    # log det(lambda^2*I_n + A_tilde^T A_tilde): n - len(s) zero eigenvalues
    # contribute log(lambda^2) each (A_tilde has rank <= min(m,n)); the
    # remaining len(s) eigenvalues are lambda^2 + s_i^2.
    n_zero = n - len(s2)
    logdet = n_zero * np.log(lam ** 2) + np.sum(np.log(lam ** 2 + s2))

    # eq. 15-17 (dropping lambda-independent constants, as bayesian_evidence.py does)
    log_p = 0.5 * n * np.log(lam ** 2) - 0.5 * logdet - (lam ** 2) * pen / 2 - chi2 / 2
    n_good = float(np.sum(s2 / (lam ** 2 + s2)))

    return HansenEvidenceResult(lam=lam, log_evidence=log_p, x=x, n_good_params=n_good)


def hansen_evidence_search(
    A: np.ndarray, b: np.ndarray, db: np.ndarray, lam_grid: np.ndarray, S: np.ndarray | None = None
) -> tuple[HansenEvidenceResult, list[HansenEvidenceResult]]:
    results = [hansen_evidence(A, b, db, lam, S=S) for lam in lam_grid]
    best = max(results, key=lambda r: r.log_evidence)
    return best, results


def solve_gcv(A: np.ndarray, b: np.ndarray, db: np.ndarray, lam_grid: np.ndarray | None = None) -> SolverResult:
    """GCV-selected Hansen IFT, packaged as a SolverResult like this
    package's other solvers."""
    n = A.shape[1]
    S = hansen_smoothness_matrix(n)
    if lam_grid is None:
        svd_A = compute_svd(A / db[:, None])
        lam_grid = np.geomspace(max(svd_A.s.min() * 1e-4, 1e-12), svd_A.s.max() * 10, 80)
    lam_opt, _ = hansen_tikhonov_gcv(A, b, db, lam_grid, S=S)
    x = hansen_tikhonov(A, b, db, lam_opt, S=S)
    fitted_b = A @ x
    return SolverResult(
        x=x, fitted_b=fitted_b, n_iterations=0,
        chi2_r_history=[pkg_chi2_r(b, fitted_b, db)], roughness_history=[],
        converged=True,
        diagnostics={"lambda_selection": f"Hansen IFT, GCV, lambda={lam_opt:.4g}"},
    )


def solve_evidence(A: np.ndarray, b: np.ndarray, db: np.ndarray, lam_grid: np.ndarray | None = None) -> SolverResult:
    """Bayesian-evidence-selected Hansen IFT, packaged as a SolverResult."""
    n = A.shape[1]
    S = hansen_smoothness_matrix(n)
    if lam_grid is None:
        svd_A = compute_svd(A / db[:, None])
        lam_grid = np.geomspace(max(svd_A.s.min() * 1e-4, 1e-12), svd_A.s.max() * 10, 80)
    best, _ = hansen_evidence_search(A, b, db, lam_grid, S=S)
    fitted_b = A @ best.x
    return SolverResult(
        x=best.x, fitted_b=fitted_b, n_iterations=0,
        chi2_r_history=[pkg_chi2_r(b, fitted_b, db)], roughness_history=[],
        converged=True,
        diagnostics={
            "lambda_selection": f"Hansen IFT, Bayesian evidence, lambda={best.lam:.4g}, Ng={best.n_good_params:.1f}",
        },
    )
