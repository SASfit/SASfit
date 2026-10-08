"""
Regularization-parameter (lambda) selection.

Implements the L-curve method with corner detection via Menger curvature
(radius of the circle through three consecutive points on the log-log
(residual-norm, solution-norm) curve) -- the same principle used by
sasfit_gsl_multifit_linear_lcurve / menger_C / menger_r in the C code,
reimplemented directly against the standard reference (Hansen & O'Leary,
1993) rather than transliterated line-by-line from the C, since the
underlying algorithm is well established.

The C code has several corner-detection variants (global minimum radius,
local-minima search, G-test-weighted). This starts with the global-minimum
variant (Lcorner_o); the others can be added once we have a shared test
curve to compare against the C output on.
"""
from __future__ import annotations

from dataclasses import dataclass

import numpy as np


@dataclass
class LCurvePoint:
    lam: float
    residual_norm: float  # ||b - A x||
    solution_norm: float  # ||L x||


def menger_curvature(p1: LCurvePoint, p2: LCurvePoint, p3: LCurvePoint) -> float:
    """
    Menger curvature of three points in the (log residual_norm,
    log solution_norm) plane. Larger curvature = sharper corner.
    """
    pts = np.array(
        [
            [np.log(p.residual_norm), np.log(p.solution_norm)]
            for p in (p1, p2, p3)
        ]
    )
    x1, y1 = pts[0]
    x2, y2 = pts[1]
    x3, y3 = pts[2]

    area2 = (x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1)  # 2x signed triangle area
    d12 = np.hypot(x2 - x1, y2 - y1)
    d23 = np.hypot(x3 - x2, y3 - y2)
    d13 = np.hypot(x3 - x1, y3 - y1)
    denom = d12 * d23 * d13
    if denom == 0:
        return 0.0
    return abs(2 * area2) / denom


def find_corner(points: list[LCurvePoint]) -> int:
    """
    Return the index into `points` of the L-curve corner: the point with
    maximum Menger curvature among consecutive triples. `points` must be
    ordered by increasing lambda.
    """
    if len(points) < 3:
        raise ValueError("Need at least 3 points on the L-curve to find a corner")

    curvatures = [
        menger_curvature(points[i - 1], points[i], points[i + 1])
        for i in range(1, len(points) - 1)
    ]
    best = int(np.argmax(curvatures)) + 1  # +1 to shift back into `points` indexing
    return best
