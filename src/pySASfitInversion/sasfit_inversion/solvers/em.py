"""
Expectation-Maximization (Lucy-Richardson) inversion, eq. 36-46 of
Kohlbrecher & Bressler (2022).

Implements:
  - plain EM fixed-point iteration (eq. 36-37)
  - optional single or double smoothing (eq. 41-46), operator S^(h)

STOPPING CRITERION -- confirmed directly from FP_solver/FP_step in
src/sasfit_fixed_point_acc/sasfit_fixed_point_acc.c (not guessed): the
default Picard_iteration loop is

    while (it < maxsteps && gNorm > relerror) { gNorm = FP_step(...); }

where FP_step computes gNorm = sqrt(sum((x_new[j] - x_old[j])**2)) -- the
plain Euclidean norm of the change between successive iterates, compared
against a user-set absolute tolerance `relerror` (the name is the C
variable's; despite the name it's used as an absolute, not relative,
tolerance in this loop). Implemented below as `tol` / `max_steps`.

NOT yet implemented:
  - Anderson / Biggs-Andrews acceleration. The C source's BIGGS_ANDREWS
    case gives the exact pseudocode (also in the JAC 2022 paper, fig. 14) --
    worth adding since the paper calls it the preferred scheme and plain
    Picard iteration converges slowly, but not done here yet.
  - The signed/general-kernel (Chae et al. 2018) variant -- see
    kernels.py docstring.
"""
from __future__ import annotations

import numpy as np

from .base import SolverResult, chi2_r, g_test


def _smoothing_matrix(n: int, h: float) -> np.ndarray:
    """
    Tridiagonal smoothing operator S^(h), eq. 43. 0 < h <~ 0.3.
    Endpoints use (1-h) rather than (1-2h) so each row still sums to 1.
    """
    S = np.zeros((n, n))
    for i in range(n):
        if i > 0:
            S[i, i - 1] = h
        if i < n - 1:
            S[i, i + 1] = h
        S[i, i] = 1 - h if (i == 0 or i == n - 1) else 1 - 2 * h
    return S


def em_step(x: np.ndarray, A: np.ndarray, b: np.ndarray) -> np.ndarray:
    """One plain EM/Lucy-Richardson update, eq. 36-37."""
    col_sums = A.sum(axis=0)  # sum_m A_mj
    Ax = A @ x                # sum_n A_in x_n
    Ax = np.where(Ax == 0, np.finfo(float).eps, Ax)  # guard div-by-zero
    correction = (A.T @ (b / Ax)) / col_sums
    return x * correction


def solve(
    A: np.ndarray,
    b: np.ndarray,
    db: np.ndarray,
    x0: np.ndarray | None = None,
    max_iterations: int = 10_000,
    smoothing_h: float | None = None,
    double_smoothing: bool = False,
    tol: float = 1e-8,
) -> SolverResult:
    """
    Parameters
    ----------
    A : (M, N) kernel matrix (background-subtracted convention: does not
        include the flat background term).
    b : (M,) background-subtracted measured intensity.
    db : (M,) uncertainty on b, for chi2_r tracking.
    x0 : initial guess. Defaults to a small positive constant -- the paper
         notes the EM fixed point is independent of the seed, though the
         *path* to it (and apparent smoothness along the way) is not.
    smoothing_h : if given, apply the single (or double, see
         `double_smoothing`) smoothing operator each iteration (eq. 41-46).
    max_iterations : hard cap on iterations (C code's `maxsteps`).
    tol : stop when ||x_new - x_old|| (Euclidean norm, matching the C
          code's `gNorm`) drops below this (C code's `relerror`).
    """
    n = A.shape[1]
    x = x0.copy() if x0 is not None else np.full(n, 1e-6)

    S = _smoothing_matrix(n, smoothing_h) if smoothing_h is not None else None

    chi2_history = []
    roughness_history = []
    g_norm_history = []
    converged = False
    n_iter = 0

    for n_iter in range(1, max_iterations + 1):
        x_old = x

        if S is not None and double_smoothing:
            # eq. 44: nonlinear smoothing in log-space before the EM step
            x_safe = np.where(x <= 0, np.finfo(float).eps, x)
            x = np.exp(S @ np.log(x_safe))

        x = em_step(x, A, b)

        if S is not None:
            x = S @ x  # eq. 42 / 46

        g_norm = float(np.sqrt(np.sum((x - x_old) ** 2)))  # C code's gNorm
        g_norm_history.append(g_norm)

        fitted_b = A @ x
        chi2_history.append(chi2_r(b, fitted_b, db))
        roughness_history.append(float(np.sum(np.diff(x) ** 2)))

        if g_norm <= tol:
            converged = True
            break

    fitted_b = A @ x
    return SolverResult(
        x=x,
        fitted_b=fitted_b,
        n_iterations=n_iter,
        chi2_r_history=chi2_history,
        roughness_history=roughness_history,
        converged=converged,
        diagnostics={"g_test_final": g_test(b, fitted_b), "g_norm_history": g_norm_history},
    )
