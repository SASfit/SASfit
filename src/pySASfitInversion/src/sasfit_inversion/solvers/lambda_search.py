"""
Outer search over a regularization/smoothing parameter, wrapping an inner
solver that takes that parameter and returns a SolverResult.

This reproduces the *structure* of Sasfit_DR_EM_smoothing_Cmd confirmed by
reading the C source directly:

  - `Optimum_smooth4DR_EM(param, FPd)` sets up the smoothing matrix for
    `param`, runs the inner fixed-point solver to convergence, and returns
    `chi2_r_final - target_chi2_r` (target is usually 1.0).
  - For `redchi2` (discrepancy principle): root-find that residual function
    with Brent's method (GSL's gsl_root_fsolver_brent) after bracketing the
    root by walking `param` down geometrically until the residual changes
    sign.
  - For `Lcorner_o` / `Lcorner_l` / `Lcorner_w` / `Lcorner_wo` / `Lcorner2`:
    walk `param` down geometrically, tracking (G-test, penalty-measure)
    pairs, and pick the corner via Menger radius (same as
    lambda_selection.find_corner here, reimplemented against Hansen &
    O'Leary 1993 rather than transliterated).
  - `manual`: just run the inner solver once at a user-given param.

NOT verified against the C code's actual bracketing constants (XMAX=0.35,
XHI=0.325, XLO=0.3, REDFACTOR) or its exact inner-loop stopping tolerance
-- those are C-code implementation details for finding a good starting
bracket efficiently, not part of the mathematical method itself. This uses
a more generic geometric bracket search instead; it should converge to the
same regularization parameter, just not necessarily in the same number of
function evaluations as the C version.
"""
from __future__ import annotations

from dataclasses import dataclass
from typing import Callable

import numpy as np
from scipy.optimize import brentq

from .base import SolverResult
from ..lambda_selection import LCurvePoint, find_corner


@dataclass
class ParameterSearchResult:
    param: float
    result: SolverResult
    trace: list[tuple[float, SolverResult]]
    # False when the target could not be bracketed (e.g. chi2_r saturates
    # above/below the target over the whole parameter range): `param` and
    # `result` are then the closest achievable point, not a root.
    reachable: bool = True


def discrepancy_principle_search(
    solve_fn: Callable[[float], SolverResult],
    target_chi2_r: float = 1.0,
    param_start: float = 0.3,
    reduction_factor: float = 2.0,
    param_floor: float = 1e-8,
) -> ParameterSearchResult:
    """
    Find the parameter value such that the inner solver's final chi2_r
    equals `target_chi2_r` (discrepancy principle), matching the `redchi2`
    branch of Sasfit_DR_EM_smoothing_Cmd.

    Brackets the root by walking `param` down geometrically from
    `param_start` until chi2_r crosses the target, then refines with
    Brent's method.
    """
    trace: list[tuple[float, SolverResult]] = []

    def objective(param: float) -> float:
        result = solve_fn(param)
        trace.append((param, result))
        return result.chi2_r_history[-1] - target_chi2_r

    lo = param_start
    f_lo = objective(lo)
    hi = lo
    f_hi = f_lo

    # walk down geometrically until the objective changes sign
    while f_lo * f_hi > 0 and hi > param_floor:
        lo = hi
        f_lo = f_hi
        hi = lo / reduction_factor
        f_hi = objective(hi)

    if f_lo * f_hi > 0:
        raise RuntimeError(
            "Could not bracket a root of chi2_r - target within "
            f"[{param_floor}, {param_start}]. Either target_chi2_r is "
            "unreachable in this range, or param_start needs to be larger."
        )

    param_opt = brentq(objective, hi, lo, xtol=1e-6)
    result_opt = solve_fn(param_opt)
    trace.append((param_opt, result_opt))

    return ParameterSearchResult(param=param_opt, result=result_opt, trace=trace)


def discrepancy_principle_search_bidirectional(
    solve_fn: Callable[[float], SolverResult],
    target_chi2_r: float = 1.0,
    param_start: float = 1.0,
    factor: float = 2.0,
    param_floor: float = 1e-12,
    param_ceil: float = 1e12,
) -> ParameterSearchResult:
    """
    Like discrepancy_principle_search, but does not assume which direction
    (increasing or decreasing `param`) raises chi2_r toward the target --
    instead walks whichever direction the sign of the objective at
    param_start indicates, geometrically, until the root is bracketed, then
    refines with Brent's method.

    Needed for the Bayesian-evidence solvers' lambda: discrepancy_principle_
    search above always starts over-smoothed (EM's smoothing_h at
    param_start) and walks DOWN to find where chi2_r first drops to the
    target. The Bayesian-evidence solvers' own evidence-maximizing lambda
    is not constructed to hit any particular chi2_r at all -- on a given
    problem it can land on EITHER side of chi2_r=target (this package's own
    real-data test.dat case lands well under-regularized, chi2_r~0.38,
    needing lambda increased to reach chi2_r=1; a different dataset could
    just as easily land over-regularized instead), so the search direction
    has to be decided from the sign of the objective rather than assumed.

    This is deliberately opt-in (see solver_registry.py's
    target_chi2_r=None default for the Bayesian-evidence solvers) rather
    than replacing evidence-maximization outright: maximizing the evidence
    and hitting chi2_r=1 exactly are two different, individually
    well-motivated criteria (see bayesian_evidence.py's own docstring), and
    which one is more trustworthy on a given real dataset is a judgment
    call the evidence-based lambda's own chi2_r value (reported regardless)
    lets you make, not something to silently decide for you.
    """
    trace: list[tuple[float, SolverResult]] = []

    def objective(param: float) -> float:
        result = solve_fn(param)
        trace.append((param, result))
        return result.chi2_r_history[-1] - target_chi2_r

    p0 = param_start
    f0 = objective(p0)
    if f0 == 0:
        return ParameterSearchResult(param=p0, result=trace[-1][1], trace=trace)

    # f0 < 0 means chi2_r < target (under-regularized/overfitting) -> more
    # regularization is needed -> walk param UP. f0 > 0 means over-
    # regularized -> walk DOWN.
    grow = f0 < 0
    p1, f1 = p0, f0
    n_flat = 0
    while (f0 * f1 > 0) and (param_floor < p1 < param_ceil):
        p0, f0 = p1, f1
        p1 = p1 * factor if grow else p1 / factor
        f1 = objective(p1)
        # chi2_r plateau: further walking cannot reach the target (e.g. a
        # coarse spline basis whose unregularized fit still has chi2_r >
        # target). Stop after a few flat steps instead of walking 24 decades.
        if abs(f1 - f0) <= 1e-6 * (abs(f0) + target_chi2_r):
            n_flat += 1
            if n_flat >= 4:
                break
        else:
            n_flat = 0

    if f0 * f1 > 0:
        # Target not reachable: return the closest achievable point instead
        # of failing, flagged reachable=False so callers can report it.
        p_best, r_best = min(
            trace, key=lambda pr: abs(pr[1].chi2_r_history[-1] - target_chi2_r)
        )
        return ParameterSearchResult(
            param=p_best, result=r_best, trace=trace, reachable=False
        )

    lo, hi = (p0, p1) if p0 < p1 else (p1, p0)
    param_opt = brentq(objective, lo, hi, xtol=1e-6)
    result_opt = solve_fn(param_opt)
    trace.append((param_opt, result_opt))

    return ParameterSearchResult(param=param_opt, result=result_opt, trace=trace)


def l_curve_search(
    solve_fn: Callable[[float], SolverResult],
    param_start: float = 0.3,
    reduction_factor: float = 1.2,
    n_points: int = 30,
    param_floor: float = 1e-8,
) -> ParameterSearchResult:
    """
    Find the parameter value at the L-curve corner, matching the
    `Lcorner_o` branch of Sasfit_DR_EM_smoothing_Cmd: walk `param` down
    geometrically, track (G-test-like residual measure, roughness) pairs,
    and pick the point of maximum Menger curvature in log-log space.

    Uses chi2_r history's final value as the "residual" axis and the
    roughness history's final value as the "solution norm" axis -- these
    play the same qualitative role as the G-test / sum-of-derivatives pairs
    in the C code's L-curve, though not numerically identical to them.
    """
    params = [param_start / (reduction_factor**i) for i in range(n_points)]
    params = [p for p in params if p > param_floor]

    trace: list[tuple[float, SolverResult]] = []
    points: list[LCurvePoint] = []

    for p in params:
        result = solve_fn(p)
        trace.append((p, result))
        points.append(
            LCurvePoint(
                lam=p,
                residual_norm=max(result.chi2_r_history[-1], 1e-12),
                solution_norm=max(result.roughness_history[-1], 1e-12),
            )
        )

    corner_idx = find_corner(points)
    param_opt = points[corner_idx].lam
    result_opt = trace[corner_idx][1]

    return ParameterSearchResult(param=param_opt, result=result_opt, trace=trace)
