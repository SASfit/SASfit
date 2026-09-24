"""
Bayesian evidence for choosing the regularization weight lambda, per
Vestergaard & Hansen (2006), "Application of Bayesian analysis to indirect
Fourier transformation in small-angle scattering", J. Appl. Cryst. 39,
797-804 -- read directly (via pdftotext) from the copy at
.../FormFactors4SASfit/In Progress/Expectation Maximation/wf5022.pdf on
2026-09-23, not from secondary description.

Their setup (their eq. 3-6, notation p for the solution vector, their S is
our L2 smoothness penalty p^T L^T L p matching their eq. 5, their A = grad-
grad S, B = grad-grad (chi2/2)):

    Q = lambda*S + chi2                                  (eq. 3)
    chi2 = sum_i [Im(qi) - I(qi)]^2 / sigma_i^2            (eq. 4)
    P(lambda, d) = [lambda^(N-1) (N+1)]^(1/2)
                   / [det(lambda*A + B)]^(1/2)
                   * exp(-lambda*S - chi2/2)               (eq. 6, evaluated
                                                             at p = p_MAP(lambda))

I initially mis-transcribed eq. 6's denominator as det(A + lambda*B) from a
garbled OCR pass; the paper's own later discussion of eq. 8 explicitly says
"eigenvalues of lambda*A + B", which fixes the ordering used here.

CAVEAT I have not resolved: eq. 3 minimizes (lambda*S + chi2) -- no factor
of 1/2 on chi2 -- while eq. 6's exponent is (-lambda*S - chi2/2). Taken
literally these correspond to different MAP points unless a factor of 2 is
being absorbed into lambda's definition between the two equations (a common
convention split between "Tikhonov-style" and "Bayesian-style" weightings
in this literature). I've implemented internally consistently with eq. 6
(MAP point minimizes lambda*S + chi2/2, since that's what makes eq. 6 a
normalized-looking posterior), but if you need this to match the paper's
lambda values exactly, this factor-of-2 needs checking against the actual
typeset paper, not the OCR'd text extraction used here.

Also implements eq. 8's "number of good parameters":

    Ng = sum_j  mu_j / (lambda + mu_j)

where mu_j are the eigenvalues of the generalized eigenvalue problem
B v = mu A v (their eq. 8 is the A=I special case, sum mu_j/(lambda+mu_j)
with mu_j = eigenvalues of B directly).
"""
from __future__ import annotations

from dataclasses import dataclass

import numpy as np
import scipy.linalg as sla


@dataclass
class EvidenceResult:
    lam: float
    log_evidence: float
    p_map: np.ndarray
    n_good_params: float


def _map_solution(K: np.ndarray, b: np.ndarray, sigma: np.ndarray, L: np.ndarray, lam: float) -> np.ndarray:
    """
    p that minimizes lambda*S + chi2/2 = lambda* p^T L^T L p + (1/2) sum_i
    [(Kp)_i - b_i]^2/sigma_i^2 -- a linear-Gaussian problem, solved directly
    via the normal equations (no need for the SVD machinery here since L
    need not be square).
    """
    W = 1.0 / sigma**2
    lhs = 2 * lam * (L.T @ L) + K.T @ (W[:, None] * K)
    rhs = K.T @ (W * b)
    return np.linalg.solve(lhs, rhs)


def log_evidence(K: np.ndarray, b: np.ndarray, sigma: np.ndarray, L: np.ndarray, lam: float) -> EvidenceResult:
    """
    log P(lambda) up to an additive constant that doesn't depend on lambda
    (the (N+1) factor in eq. 6 is lambda-independent and dropped here since
    it doesn't affect the argmax over lambda).
    """
    n = K.shape[1]
    W = 1.0 / sigma**2

    A_hess = 2 * (L.T @ L)          # Hessian of S = p^T L^T L p
    B_hess = K.T @ (W[:, None] * K)  # Hessian of chi2/2

    p_map = _map_solution(K, b, sigma, L, lam)
    S_val = float(p_map @ (L.T @ L) @ p_map)
    chi2_val = float(np.sum(((K @ p_map - b) / sigma) ** 2))

    hessian = lam * A_hess + B_hess
    sign, logdet = np.linalg.slogdet(hessian)
    if sign <= 0:
        logdet = np.inf  # degenerate/indefinite Hessian -- lambda not viable here

    log_p = 0.5 * (n - 1) * np.log(lam) - 0.5 * logdet - lam * S_val - chi2_val / 2

    eigvals_B = sla.eigh(B_hess, eigvals_only=True)
    eigvals_B = np.clip(eigvals_B, 0, None)  # guard tiny negative numerical noise
    n_good = float(np.sum(eigvals_B / (lam + eigvals_B)))

    return EvidenceResult(lam=lam, log_evidence=log_p, p_map=p_map, n_good_params=n_good)


def evidence_search(
    K: np.ndarray,
    b: np.ndarray,
    sigma: np.ndarray,
    L: np.ndarray,
    lam_grid: np.ndarray,
) -> tuple[EvidenceResult, list[EvidenceResult]]:
    """Evaluate log-evidence over a grid of lambda and return the maximizer."""
    results = [log_evidence(K, b, sigma, L, lam) for lam in lam_grid]
    best = max(results, key=lambda r: r.log_evidence)
    return best, results
