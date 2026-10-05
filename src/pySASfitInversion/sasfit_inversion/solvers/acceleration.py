"""
Generic fixed-point acceleration, extracted from Joachim's own
pyOZgui/src/pyozgui/biggsAndrewsOZsolver.py (Biggs & Andrews 1997 vector
extrapolation, ported there directly from sasfit_oz_solver.c's own
"case BIGGS_ANDREWS:" block). That file's own algorithm is completely
problem-agnostic -- it only ever calls a generic fixed_point_operator(x),
with no OZ-specific assumption anywhere -- so it is reproduced here as a
standalone function usable by any fixed-point iteration in this package
(em.py, em_general.py, maxent.py), not just OZ solving.

Already validated in the source file's own docstring/history (not
re-validated here beyond this module's own tests):
  - KINSetMAA=0 matches plain Picard iteration almost exactly (354 vs 353
    steps on a reference case), confirming the order-0 branch correctly
    degenerates to unaccelerated fixed-point iteration.
  - KINSetMAA=1 and -2 both roughly halve the iteration count versus plain
    Picard on that same case.
  - Every configuration tested (0, 1, 2, -2) agrees with an independently-
    validated GMRES-based solver to within ~1e-12.

ADAPTATION for this package's use (not present in the OZ version, which
has no sign constraint on its Gamma function): EM-family fixed-point maps
require a non-negative input. Biggs-Andrews' extrapolation step can
overshoot into negative territory, which would corrupt (not just slow)
the EM multiplicative update if handed a negative x0 -- see em.em_step's
own assumption that its `x` argument is >= 0. `clip_nonnegative=True`
(default) clips the extrapolated trial point to a small positive floor
before evaluating the fixed-point map there, whenever the map is meant to
operate on a non-negative vector (as all the EM-family ones are).
"""
from __future__ import annotations

from dataclasses import dataclass, field
from typing import Callable

import numpy as np


@dataclass
class AccelerationResult:
    x: np.ndarray
    converged: bool
    n_iterations: int
    residual_norm_history: list = field(default_factory=list)


def biggs_andrews(
    fixed_point_operator: Callable[[np.ndarray], np.ndarray],
    x0: np.ndarray,
    max_iterations: int = 1000,
    tol: float = 1e-8,
    kin_set_maa: int = 2,
    clip_nonnegative: bool = True,
    clip_floor: float = 0.0,
    on_iterate: Callable[[np.ndarray], None] | None = None,
) -> AccelerationResult:
    """
    Biggs-Andrews-accelerated fixed-point iteration for x = fixed_point_operator(x).

    Parameters
    ----------
    fixed_point_operator : callable(x) -> x_next
        The map being accelerated -- e.g. em.em_step (plus smoothing)
        wrapped in a small closure by the caller.
    x0 : initial guess.
    max_iterations : hard cap (this function's own "numberOfIterations").
    tol : convergence tolerance on relative change in ||x|| between
        successive (accepted) iterates -- same criterion
        biggsAndrewsOZsolver.py itself uses (not the em.py-family's
        absolute ||x_new-x_old|| gNorm test, since this is a
        general-purpose accelerator, not specific to this package's own
        convention).
    kin_set_maa : extrapolation order (|value|) and step-size formula
        choice (sign) -- see module/source docstring. 0 disables
        extrapolation (reduces to plain Picard iteration); 2 is a
        reasonable default matching the source file's own default.
    clip_nonnegative : if True, clip the extrapolated trial point to
        >= clip_floor before evaluating fixed_point_operator there (see
        module docstring's ADAPTATION note). Set False to reproduce the
        original OZ behaviour exactly (no clipping) for a sign-
        unconstrained fixed-point map.
    clip_floor : the non-negativity floor used when clip_nonnegative=True.
    on_iterate : optional callback(x_accepted) invoked once per accepted
        iterate (including the two warm-up steps), so a caller can record
        its own per-iteration diagnostics (e.g. em.py's chi2_r_history)
        without this generic module needing to know anything about them.
    """
    def clip(v: np.ndarray) -> np.ndarray:
        return np.maximum(v, clip_floor) if clip_nonnegative else v

    xn = np.asarray(x0, dtype=float).copy()

    # Two unaccelerated warm-up fixed-point steps -- needed to have two
    # residual-history vectors (gn1, gn2) available before any
    # extrapolation can be attempted (matches the source's own two
    # hand-unrolled steps before its main loop).
    xp1 = fixed_point_operator(clip(xn))
    gn1 = xp1 - xn
    xn1 = xn.copy()
    xn = xp1.copy()
    if on_iterate is not None:
        on_iterate(xn)

    xp1 = fixed_point_operator(clip(xn))
    gn2 = gn1.copy()
    gn1 = xp1 - xn
    xn2 = xn1.copy()
    xn1 = xn.copy()
    xn = xp1.copy()
    if on_iterate is not None:
        on_iterate(xn)

    n = 2
    previous_norm = float(np.linalg.norm(xn))
    residual_history = [previous_norm]
    relative_progress = np.inf
    converged = False

    while n < max_iterations and relative_progress >= tol:
        n += 1
        beta = float(np.dot(gn1, gn2))
        gamma = float(np.dot(gn2, gn2))
        if gamma == 0.0:
            alpha = 1.0
        else:
            if kin_set_maa > 0:
                alpha = beta / gamma
            else:
                alpha = np.sign(beta / gamma) * np.sqrt(abs(beta / gamma))
            bound = (n - 1.0) / (n + 2.0)
            alpha = max(min(alpha, bound), -bound)

        order = abs(kin_set_maa) * (alpha != 0.0)
        if order == 0:
            yn = xn.copy()
        elif order == 1:
            yn = xn + alpha * (xn - xn1)
        else:
            yn = xn + alpha * (xn - xn1) + 0.5 * alpha * alpha * (xn - 2.0 * xn1 + xn2)

        xp1 = fixed_point_operator(clip(yn))
        gn2 = gn1.copy()
        gn1 = xp1 - yn
        xn2 = xn1.copy()
        xn1 = xn.copy()
        xn = xp1.copy()

        norm = float(np.linalg.norm(xn))
        relative_progress = abs(previous_norm - norm) / (norm + np.finfo(float).eps)
        previous_norm = norm
        residual_history.append(norm)
        if on_iterate is not None:
            on_iterate(xn)

    if n < max_iterations:
        converged = True

    return AccelerationResult(x=clip(xn), converged=converged, n_iterations=n,
                               residual_norm_history=residual_history)
