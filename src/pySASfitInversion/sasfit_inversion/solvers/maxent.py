"""
EM + Maximum Entropy penalty, constant and adaptive prior variants,
eq. 47-55 of Kohlbrecher & Bressler (2022).

Both variants extend the plain EM update (eq. 36-37, see em.py's em_step)
with an additional entropy-penalty term each iteration. The Lagrange
multiplier lambda controls the penalty strength and is expected to be
chosen externally (e.g. via the L-curve corner detection in
lambda_selection.py) by running this solver at several lambda values --
this module does the inner iteration only, not the outer lambda search
(see lambda_search.py for that).

NOT yet implemented / verified:
  - The exact stopping criterion (same caveat as em.py).
  - Cross-checked numerically against SASfit's own DR_EM_ME_const_Cmd /
    DR_EM_ME_adaptive_Cmd output on a shared test curve -- the equations
    below are transcribed from the published paper, not validated against
    the C implementation's actual numbers yet.
"""
from __future__ import annotations

import numpy as np

from .base import SolverResult, chi2_r, g_test
from .em import em_step


def entropy(x: np.ndarray, m: np.ndarray) -> float:
    """Entropy S, eq. 47. m is the prior estimate of x."""
    x_safe = np.where(x <= 0, np.finfo(float).eps, x)
    m_safe = np.where(m <= 0, np.finfo(float).eps, m)
    return float(np.sum(-x_safe * np.log(x_safe / m_safe) + x_safe - m_safe))


def _prior_smoothing_matrix(n: int, sigma2: float) -> np.ndarray:
    """
    Smoothing operator Pi_ij for constructing an adaptive prior, eq. 55.

        Pi_ij = (1/c) * exp(-(i-j)^2 / (2*sigma2))

    with c a per-row normalization constant so each row of Pi sums to 1.
    sigma2 (paper: sigma^2) is typically between 0.5 and 2.
    """
    idx = np.arange(n)
    diff2 = (idx[:, None] - idx[None, :]) ** 2
    Pi = np.exp(-diff2 / (2 * sigma2))
    Pi /= Pi.sum(axis=1, keepdims=True)
    return Pi


def solve_constant_prior(
    A: np.ndarray,
    b: np.ndarray,
    db: np.ndarray,
    prior: np.ndarray,
    lam: float,
    x0: np.ndarray | None = None,
    max_iterations: int = 10_000,
) -> SolverResult:
    """
    EM + MaxEnt with a fixed, known prior `m` (eq. 48-50).

    x^(k+1)_j = x^(k)_j + dx^(k)_j + ds^(k)_j

    where dx is the plain EM correction (em_step) and

        ds^(k)_j = lambda * x^(k)_j * [ -ln(x_j/m_j)
                       + (1/sum_j x^(k)_j) * sum_j x^(k)_j * ln(x^(k)_j/m_j) ]
    """
    n = A.shape[1]
    x = x0.copy() if x0 is not None else np.full(n, 1e-6)
    m = prior

    chi2_history, roughness_history, entropy_history = [], [], []

    for _ in range(max_iterations):
        x_em = em_step(x, A, b)
        dx = x_em - x

        x_safe = np.where(x <= 0, np.finfo(float).eps, x)
        m_safe = np.where(m <= 0, np.finfo(float).eps, m)
        log_ratio = np.log(x_safe / m_safe)
        weighted_mean_log_ratio = np.sum(x_safe * log_ratio) / np.sum(x_safe)
        ds = lam * x_safe * (-log_ratio + weighted_mean_log_ratio)

        x = x + dx + ds
        x = np.where(x < 0, np.finfo(float).eps, x)  # guard: EM/MaxEnt assumes x >= 0

        fitted_b = A @ x
        chi2_history.append(chi2_r(b, fitted_b, db))
        roughness_history.append(float(np.sum(np.diff(x) ** 2)))
        entropy_history.append(entropy(x, m))

    fitted_b = A @ x
    return SolverResult(
        x=x,
        fitted_b=fitted_b,
        n_iterations=max_iterations,
        chi2_r_history=chi2_history,
        roughness_history=roughness_history,
        converged=False,
        diagnostics={
            "g_test_final": g_test(b, fitted_b),
            "entropy_history": entropy_history,
            "lambda": lam,
        },
    )


def solve_adaptive_prior(
    A: np.ndarray,
    b: np.ndarray,
    db: np.ndarray,
    lam: float,
    sigma2: float = 1.0,
    x0: np.ndarray | None = None,
    max_iterations: int = 10_000,
) -> SolverResult:
    """
    EM + MaxEnt with an adaptive prior constructed from the current
    solution via a Gaussian point-spread smoothing (Horne, 1985),
    eq. 51-55.

        m^(k)_i = sum_j Pi_ij x^(k)_j

    with the same overall update structure as the constant-prior case but
    using m^(k) (recomputed each iteration) instead of a fixed prior, and
    a slightly different ds formula (eq. 53) that includes a normalized
    Pi-weighted correction term.
    """
    n = A.shape[1]
    x = x0.copy() if x0 is not None else np.full(n, 1e-6)
    Pi = _prior_smoothing_matrix(n, sigma2)

    chi2_history, roughness_history, entropy_history = [], [], []

    for _ in range(max_iterations):
        m = Pi @ x
        m_safe = np.where(m <= 0, np.finfo(float).eps, m)

        x_em = em_step(x, A, b)
        dx = x_em - x

        x_safe = np.where(x <= 0, np.finfo(float).eps, x)
        log_ratio = np.log(x_safe / m_safe)
        weighted_mean_log_ratio = np.sum(x_safe * log_ratio) / np.sum(x_safe)
        pi_weighted_term = (Pi.T @ (x_safe / m_safe))  # sum_i Pi_ij * x_i/m_i, per eq. 53's last term
        ds = lam * x_safe * (
            weighted_mean_log_ratio - log_ratio - 1 + pi_weighted_term
        )

        x = x + dx + ds
        x = np.where(x < 0, np.finfo(float).eps, x)

        fitted_b = A @ x
        chi2_history.append(chi2_r(b, fitted_b, db))
        roughness_history.append(float(np.sum(np.diff(x) ** 2)))
        entropy_history.append(entropy(x, m))

    fitted_b = A @ x
    return SolverResult(
        x=x,
        fitted_b=fitted_b,
        n_iterations=max_iterations,
        chi2_r_history=chi2_history,
        roughness_history=roughness_history,
        converged=False,
        diagnostics={
            "g_test_final": g_test(b, fitted_b),
            "entropy_history": entropy_history,
            "lambda": lam,
            "sigma2": sigma2,
        },
    )
