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
