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

ACCELERATION (2026-10-03, per Joachim's request that every EM-family
technique use it, not just plain em.py): both solvers now wrap their
per-iteration update (EM step + entropy correction term) with
Biggs-Andrews acceleration (acceleration.py), default ON
(accelerate=True), same as em.py. Also added a proper gNorm-based
stopping criterion (matching em.py's convention) -- previously these
loops always ran the full max_iterations with no early stopping at all
(converged was hard-coded False).

IMPORTANT FINDING from adding acceleration: for this entropy-penalized
EM, the ITERATION COUNT itself can act as an implicit secondary
regularizer on top of lambda (the classic EM/Richardson-Lucy
"semiconvergence" phenomenon: an intermediate, not-yet-converged iterate
can fit held-out/true structure better than the eventual fixed point,
because later iterations start fitting noise). Confirmed directly: on
this package's bimodal synthetic test at lam=0.01, sigma2=1.0, a plain
(unaccelerated) run stopped at 2000 iterations gave chi2_r=0.963,
corr=0.949 -- but running plain Picard all the way to 200,000 iterations
converges to chi2_r=0.710, corr=0.789, and accelerated reaches that SAME
fixed point in only 3833 iterations. The earlier "0.963/0.949" was a
semiconvergence snapshot, not a better answer -- acceleration didn't
break anything, it just removes the free (and uncontrolled) extra
regularization that running too few plain-Picard iterations was
accidentally providing. Practical consequence: any lambda chosen by
comparing chi2_r at a fixed, small iteration count (as this module's
solver_registry.py wrappers' grid searches used to do) was implicitly
tuned against that iteration count too, not against lambda alone -- see
solver_registry.py's _run_maxent_constant_prior/_run_maxent_adaptive_prior,
which were updated accordingly once acceleration landed here.

NOT yet implemented / verified:
  - Cross-checked numerically against SASfit's own DR_EM_ME_const_Cmd /
    DR_EM_ME_adaptive_Cmd output on a shared test curve -- the equations
    below are transcribed from the published paper, not validated against
    the C implementation's actual numbers yet.
"""
from __future__ import annotations

import numpy as np

from .base import SolverResult, chi2_r, g_test
from .em import em_step
from . import acceleration


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


def _run_loop(one_step, x0, max_iterations, tol, accelerate, kin_set_maa,
              A, b, db, diagnostics_extra_fn):
    """Shared driver for both solve_* functions below: plain Picard or
    Biggs-Andrews-accelerated, with gNorm-based stopping either way
    (matching em.py's convention) and the same chi2_r/roughness/entropy
    history bookkeeping regardless of which path is taken."""
    chi2_history, roughness_history, entropy_history, g_norm_history = [], [], [], []

    def record(x_new, x_old):
        g_norm_history.append(float(np.linalg.norm(x_new - x_old)))
        fitted_b = A @ x_new
        chi2_history.append(chi2_r(b, fitted_b, db))
        roughness_history.append(float(np.sum(np.diff(x_new) ** 2)))
        entropy_history.append(diagnostics_extra_fn(x_new))

    if accelerate:
        x_prev_box = [x0.copy()]

        def on_iterate(x_new):
            record(x_new, x_prev_box[0])
            x_prev_box[0] = x_new.copy()

        result = acceleration.biggs_andrews(
            one_step, x0, max_iterations=max_iterations, tol=tol,
            kin_set_maa=kin_set_maa, clip_nonnegative=True,
            clip_floor=np.finfo(float).eps, on_iterate=on_iterate,
        )
        x, n_iter, converged = result.x, result.n_iterations, result.converged
    else:
        x = x0.copy()
        converged = False
        n_iter = 0
        for n_iter in range(1, max_iterations + 1):
            x_old = x
            x = one_step(x)
            record(x, x_old)
            if g_norm_history[-1] <= tol:
                converged = True
                break

    return x, n_iter, converged, chi2_history, roughness_history, entropy_history, g_norm_history


def solve_constant_prior(
    A: np.ndarray,
    b: np.ndarray,
    db: np.ndarray,
    prior: np.ndarray,
    lam: float,
    x0: np.ndarray | None = None,
    max_iterations: int = 10_000,
    tol: float = 1e-8,
    accelerate: bool = True,
    kin_set_maa: int = 2,
) -> SolverResult:
    """
    EM + MaxEnt with a fixed, known prior `m` (eq. 48-50).

    x^(k+1)_j = x^(k)_j + dx^(k)_j + ds^(k)_j

    where dx is the plain EM correction (em_step) and

        ds^(k)_j = lambda * x^(k)_j * [ -ln(x_j/m_j)
                       + (1/sum_j x^(k)_j) * sum_j x^(k)_j * ln(x^(k)_j/m_j) ]

    accelerate (default True) wraps this update in Biggs-Andrews
    acceleration (acceleration.py) instead of running it as plain Picard
    iteration -- same final fixed point, typically far fewer iterations
    (see module docstring's semiconvergence finding for why "fewer
    iterations at the same lambda" is not the same as "a worse answer").
    """
    n = A.shape[1]
    x0 = x0.copy() if x0 is not None else np.full(n, 1e-6)
    m = prior

    def one_step(x):
        x_em = em_step(x, A, b)
        dx = x_em - x
        x_safe = np.where(x <= 0, np.finfo(float).eps, x)
        m_safe = np.where(m <= 0, np.finfo(float).eps, m)
        log_ratio = np.log(x_safe / m_safe)
        weighted_mean_log_ratio = np.sum(x_safe * log_ratio) / np.sum(x_safe)
        ds = lam * x_safe * (-log_ratio + weighted_mean_log_ratio)
        x_new = x + dx + ds
        return np.where(x_new < 0, np.finfo(float).eps, x_new)

    x, n_iter, converged, chi2_history, roughness_history, entropy_history, g_norm_history = _run_loop(
        one_step, x0, max_iterations, tol, accelerate, kin_set_maa,
        A, b, db, diagnostics_extra_fn=lambda xv: entropy(xv, m),
    )

    fitted_b = A @ x
    return SolverResult(
        x=x,
        fitted_b=fitted_b,
        n_iterations=n_iter,
        chi2_r_history=chi2_history,
        roughness_history=roughness_history,
        converged=converged,
        diagnostics={
            "g_test_final": g_test(b, fitted_b),
            "entropy_history": entropy_history,
            "g_norm_history": g_norm_history,
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
    tol: float = 1e-8,
    accelerate: bool = True,
    kin_set_maa: int = 2,
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

    accelerate (default True): see solve_constant_prior.
    """
    n = A.shape[1]
    x0 = x0.copy() if x0 is not None else np.full(n, 1e-6)
    Pi = _prior_smoothing_matrix(n, sigma2)

    def one_step(x):
        m = Pi @ x
        m_safe = np.where(m <= 0, np.finfo(float).eps, m)
        x_em = em_step(x, A, b)
        dx = x_em - x
        x_safe = np.where(x <= 0, np.finfo(float).eps, x)
        log_ratio = np.log(x_safe / m_safe)
        weighted_mean_log_ratio = np.sum(x_safe * log_ratio) / np.sum(x_safe)
        pi_weighted_term = Pi.T @ (x_safe / m_safe)
        ds = lam * x_safe * (weighted_mean_log_ratio - log_ratio - 1 + pi_weighted_term)
        x_new = x + dx + ds
        return np.where(x_new < 0, np.finfo(float).eps, x_new)

    def current_entropy(xv):
        return entropy(xv, Pi @ xv)

    x, n_iter, converged, chi2_history, roughness_history, entropy_history, g_norm_history = _run_loop(
        one_step, x0, max_iterations, tol, accelerate, kin_set_maa,
        A, b, db, diagnostics_extra_fn=current_entropy,
    )

    fitted_b = A @ x
    return SolverResult(
        x=x,
        fitted_b=fitted_b,
        n_iterations=n_iter,
        chi2_r_history=chi2_history,
        roughness_history=roughness_history,
        converged=converged,
        diagnostics={
            "g_test_final": g_test(b, fitted_b),
            "entropy_history": entropy_history,
            "g_norm_history": g_norm_history,
            "lambda": lam,
            "sigma2": sigma2,
        },
    )
