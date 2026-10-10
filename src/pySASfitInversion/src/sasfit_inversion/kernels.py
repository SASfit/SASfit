"""
Discretization of the Fredholm integral

    I(Q) = integral_0^inf  N(s) |F(Q, s)|^2  ds

into the linear system  A x = b  (Kohlbrecher & Bressler, 2022, eq. 25/32-34).

Two kernel families are supported (per Joachim's 2026-09-22 scoping
decision):

1. Size-distribution kernel (non-negative x, non-negative kernel) -- this
   is what all six DR_* solvers in sasfit_extrapol.c operate on. Verified
   directly against the published equations.

2. Pair-distance-distribution / general kernel (signed x, signed kernel,
   Chae, Martin & Walker 2018) -- this is what `pysasem` implemented for
   nano-disc / GluA2 / May & Nowotny-style p(r) recovery, and is NOT in the
   current C code. NOT YET IMPLEMENTED here: the pysasem notebook's
   "Known problems and To-Dos" section documents non-obvious handling
   (no normalization of the extended kernel, discarding half the result
   after each update, careful sign handling) that needs to be taken
   directly from that notebook or from Chae et al. (2018) rather than
   guessed at -- getting this wrong silently produces a plausible-looking
   but incorrect p(r). Flagging as a TODO rather than a guessed
   implementation.
"""
from __future__ import annotations

from dataclasses import dataclass

import numpy as np


@dataclass
class FredholmSystem:
    A: np.ndarray  # (M, N) kernel matrix
    b: np.ndarray  # (M,) measured, background-subtracted intensity
    s: np.ndarray  # (N,) size grid
    alpha: float  # weighting exponent used (0=number, 3=volume, 6=intensity)


def build_size_distribution_kernel(
    q: np.ndarray,
    s: np.ndarray,
    form_factor_squared,
    alpha: float = 0.0,
) -> np.ndarray:
    """
    Build the kernel matrix A for a size-distribution inversion, following
    eq. 32-34 of Kohlbrecher & Bressler (2022):

        x_j = N(s_j) * s_j^alpha
        A_ij = ds_j * s_j^-alpha * |F(Q_i, s_j)|^2

    Parameters
    ----------
    q : array (M,)
        Scattering vector magnitudes.
    s : array (N,)
        Size grid (e.g. radius). Does not need to be uniformly spaced.
    form_factor_squared : callable(q, s) -> |F(q, s)|^2
        Squared form factor, vectorized over q for a given s (or fully
        vectorized over both -- see usage below).
    alpha : float
        0 for number-weighted, 3 for volume-weighted, 6 for
        intensity-weighted size distribution.

    Returns
    -------
    A : array (M, N)
    """
    q = np.asarray(q)
    s = np.asarray(s)
    ds = np.gradient(s)  # trapezoid-like local spacing; replace with exact
    # bin widths if the grid is deliberately non-uniform and edges matter.

    A = np.empty((q.size, s.size))
    for j, (sj, dsj) in enumerate(zip(s, ds)):
        # s_j^-alpha blows up at s_j=0 for alpha>0 (e.g. the sphere_rg
        # kernel's default alpha=6 -> 0**-6). The substitution
        # x_j = N(s_j)*s_j^alpha is itself only meaningful for s_j>0 when
        # alpha!=0 -- there is no finite s^-alpha to assign at the exact
        # origin. Physically the ds-weighted contribution of an
        # infinitesimally thin bin right at s=0 is negligible anyway, so
        # this column is just set to 0 rather than left as inf/nan (which
        # would otherwise poison every solve that includes r=0 in the
        # grid, e.g. the GUI's r-grid always starting at r=0 for p(0)=0).
        if sj == 0 and alpha > 0:
            A[:, j] = 0.0
        else:
            A[:, j] = dsj * sj ** (-alpha) * form_factor_squared(q, sj)
    return A


def recover_N_from_x(x: np.ndarray, s: np.ndarray, alpha: float) -> np.ndarray:
    """Undo the x_j = N(s_j) * s_j^alpha substitution.

    At s=0 with alpha!=0 the substitution itself is singular (0**-alpha or
    0**alpha depending on sign); N(0) is reported as 0 there rather than
    inf/nan, matching build_size_distribution_kernel's treatment of that
    same point (see its comment).
    """
    s = np.asarray(s, dtype=float)
    x = np.asarray(x, dtype=float)
    if alpha == 0:
        return x.copy()
    with np.errstate(divide="ignore", invalid="ignore"):
        N = x / (s ** alpha)
    N = np.where(s == 0, 0.0, N)
    return N
