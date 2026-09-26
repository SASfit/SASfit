"""
Maximum entropy regularization via nonlinear conjugate gradient, per
Hansen & Elfving's maxent.m in regu.zip (the genuine Regularization Tools
package, found 2026-09-23). This is a SECOND, independently-formulated
MaxEnt solver, deliberately kept separate from maxent.py's EM-based
constant/adaptive-prior implementation (which matches SASfit's
DR_EM_ME_const/adaptive, JAC 2022 eq. 47-55) -- they are NOT the same
algorithm:

  - maxent.py (SASfit/JAC-2022 style): minimizes via the EM/Lucy-Richardson
    multiplicative fixed-point update, with an *additive* entropy
    correction term each iteration, entropy defined relative to an
    explicit prior m: S = sum[-x ln(x/m) + x - m].
  - This module (Hansen/Elfving style): minimizes
        ||A x - b||^2 + lambda^2 * x^T log(w .* x)
    directly via nonlinear conjugate-gradient descent with a step-length
    control that keeps x positive at every iteration (no separate EM step
    at all) -- entropy here is x^T log(w.*x) (relative to a fixed weight
    vector w, not an evolving prior).

Transcribed as closely as practical from the actual maxent.m source
(nonlinear CG with "soft" line search via a secant root-finder on
phi(alpha) = p^T g(x + alpha*p), and a positivity-preserving step-length
bound alpha_right = min(-x_i/p_i) over i where p_i<0). Some MATLAB-specific
bookkeeping (the `data`/`X` iteration-history outputs) is dropped since
Python callers can just inspect SolverResult's histories instead.

NOT yet cross-checked numerically against MATLAB output -- transcribed
from source, not validated against a reference run.

VERIFIED (2026-09-23): the analytical gradient matches a finite-difference
gradient of the true objective to 1.5e-7 relative error -- the gradient/
objective transcription itself is correct.

PERFORMANCE FINDING, not a (confirmed) bug: on our SAS kernel (condition
number ~1e16), this unpreconditioned nonlinear-CG algorithm converges
prematurely to a poor point regardless of lambda -- chi2_r stuck around
2e5 and best-case correlation only ~0.65 on the synthetic sphere test,
worse than every other solver in this package. Hansen's own test problems
(shaw, phillips, baart, gravity, heat, in this same regu.zip) are
deliberately only moderately ill-conditioned; this plain nonlinear CG with
no preconditioning may simply not be robust enough for a kernel this
poorly conditioned, unlike EM's multiplicative update (which has better
implicit scale-handling) or the SVD/filter-factor methods (which handle
arbitrary conditioning by construction). I have not ruled out a subtler
bug in the line-search bookkeeping, but the gradient check plus the
lambda-insensitivity of the failure (same behavior across two orders of
magnitude in lambda) points at a robustness limit rather than a
correctness bug in the parts I've verified.
"""
from __future__ import annotations

import numpy as np

from .base import SolverResult, chi2_r


def _phi_and_z(x: np.ndarray, p: np.ndarray, alpha: float, g: np.ndarray,
               gamma: float, v: np.ndarray, lam2: float, phi0: float):
    """
    phi(alpha) = p^T g(x + alpha p), computed via the CG recurrence used in
    maxent.m rather than by re-evaluating the full gradient (matches the
    source's `z = log(1+alpha*p./x)` / `phi = phi0 + 2*alpha*gamma + lam2*p'*z`).
    """
    z = np.log(1 + alpha * p / x)
    phi = phi0 + 2 * alpha * gamma + lam2 * (p @ z)
    return phi, z


def solve(
    A: np.ndarray,
    b: np.ndarray,
    db: np.ndarray,
    lam: float,
    w: np.ndarray | None = None,
    x0: np.ndarray | None = None,
    max_iterations: int = 150,
    min_step: float = 1e-12,
    flat_tol: float = 1e-3,
    flat_range: int = 10,
    sigma: float = 0.5,
    tau0: float = 1e-3,
) -> SolverResult:
    """
    Hansen/Elfving MaxEnt: minimize ||Ax-b||^2 + lambda^2 * x^T log(w.*x)
    via nonlinear CG with a positivity-preserving soft line search.

    Parameters mirror maxent.m's defaults (flat=1e-3, flatrange=10,
    maxit=150, minstep=1e-12, sigma=0.5, tau0=1e-3).
    """
    m, n = A.shape
    if w is None:
        w = np.ones(n)
    if lam <= 0:
        raise ValueError("lambda must be positive")
    if x0 is None:
        x0 = np.ones(n)  # matches the actual code default (nargin<5 -> ones(n,1)),
                          # NOT the docstring comment's norm(b)/norm(A,1) formula --
                          # a real discrepancy in Hansen's own file; the code wins.

    l2 = lam**2
    x = x0.copy()
    Ax = A @ x
    g = 2 * A.T @ (Ax - b) + l2 * (1 + np.log(w * x))
    p = -g
    phi0 = float(p @ g)

    chi2_history = []
    F_history = []
    it = 0
    dF = 1.0
    delta_x_norm = np.inf  # ensures we enter the loop at least once

    while delta_x_norm > min_step * np.linalg.norm(x) and dF > flat_tol and it < max_iterations and phi0 < 0:
        it += 1

        Ap = A @ p
        gamma = float(Ap @ Ap)
        v = A.T @ Ap

        # --- soft line search: bracket then refine root of phi(alpha)=0 ---
        alpha_left, phi_left = 0.0, phi0
        if np.min(p) >= 0:
            alpha_right = -phi0 / (2 * gamma)
        else:
            neg = p < 0
            alpha_right = np.min(-x[neg] / p[neg])
            h = 1 + alpha_right * p / x
            delta = np.finfo(float).eps
            while np.min(h) <= 0:
                alpha_right *= 1 - delta
                h = 1 + alpha_right * p / x
                delta *= 2

        phi_right, z = _phi_and_z(x, p, alpha_right, g, gamma, v, l2, phi0)
        alpha, phi = alpha_right, phi_right

        if phi_right <= 0:
            z = np.log(1 + alpha * p / x)
            g_new = g + l2 * z + 2 * alpha * v
            t = float(g_new @ g_new)
            beta = (t - g @ g_new) / (phi - phi0)
        else:
            tau = tau0
            u, t = 1.0, 1.0
            while u > -sigma * t:
                while abs(phi / phi0) > tau:
                    alpha = (alpha_left * phi_right - alpha_right * phi_left) / (phi_right - phi_left)
                    phi, z = _phi_and_z(x, p, alpha, g, gamma, v, l2, phi0)
                    if phi > 0:
                        alpha_right, phi_right = alpha, phi
                    else:
                        alpha_left, phi_left = alpha, phi
                g_new = g + l2 * z + 2 * alpha * v
                t = float(g_new @ g_new)
                beta = (t - g @ g_new) / (phi - phi0)
                u = -t + beta * phi
                tau /= 10

        g = g_new
        delta_x = alpha * p
        x = x + delta_x
        p = -g + beta * p
        phi0 = float(p @ g)
        delta_x_norm = float(np.linalg.norm(delta_x))

        fitted_b = A @ x
        chi2_history.append(chi2_r(b, fitted_b, db))
        entropy_term = float(x @ np.log(w * x))
        residual_norm2 = float(np.sum((fitted_b - b) ** 2))
        F_history.append(residual_norm2 + l2 * entropy_term)

        if it <= flat_range:
            dF = 1.0
        else:
            dF = abs(F_history[it - 1] - F_history[it - 1 - flat_range]) / abs(F_history[it - 1])

    fitted_b = A @ x
    return SolverResult(
        x=x,
        fitted_b=fitted_b,
        n_iterations=it,
        chi2_r_history=chi2_history,
        roughness_history=[],  # not tracked by this algorithm's own logic
        converged=it < max_iterations,
        diagnostics={"lambda": lam, "F_history": F_history, "method": "Hansen CG MaxEnt (maxent.m)"},
    )
