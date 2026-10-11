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
   Chae, Martin & Walker 2019) -- this is what `pysasem` implemented for
   nano-disc / GluA2 / May & Nowotny-style p(r) recovery, and is NOT in the
   current C code. NOT YET IMPLEMENTED here: the pysasem notebook's
   "Known problems and To-Dos" section documents non-obvious handling
   (no normalization of the extended kernel, discarding half the result
   after each update, careful sign handling) that needs to be taken
   directly from that notebook or from Chae et al. (2019) rather than
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


# --------------------------------------------------------------------------
# Non-uniform r grids and fine-grid integration (2026-10-10)

R_GRID_SPACINGS = ("uniform", "quadratic", "log")


def make_r_grid(r_max: float, n: int, spacing: str = "uniform") -> np.ndarray:
    """n grid points on [0, r_max], always including r=0 and r=r_max.

    uniform   -- equidistant
    quadratic -- r = r_max * u^2, u equidistant (denser at small r)
    log       -- r=0 plus n-1 geometric points from r_max/(2n) to r_max
    Same placement rules as the B-spline knots (spline_basis.interior_knots).
    """
    if n < 3:
        raise ValueError("need at least 3 r grid points")
    if spacing == "uniform":
        t = np.linspace(0.0, 1.0, n)
    elif spacing == "quadratic":
        t = np.linspace(0.0, 1.0, n) ** 2
    elif spacing == "log":
        t = np.concatenate([[0.0], np.geomspace(1.0 / (2 * n), 1.0, n - 1)])
    else:
        raise ValueError(f"unknown r grid spacing {spacing!r}; use one of {R_GRID_SPACINGS}")
    return r_max * t


def quadrature_points(r_max: float, q_max: float, n_nodes: int) -> int:
    """Fine integration grid size: at least 4 points per Shannon interval
    pi/q_max and at least 4x the number of nodes."""
    return max(4 * (n_nodes - 1) + 1, int(np.ceil(4 * r_max * q_max / np.pi)) + 1)


def hat_basis(r_nodes: np.ndarray, r_fine: np.ndarray) -> np.ndarray:
    """(len(r_fine), len(r_nodes)) matrix of piecewise-linear 'hat'
    functions: x(r_fine) = H @ x(r_nodes) is linear interpolation between
    the (possibly non-uniform) nodes."""
    eye = np.eye(len(r_nodes))
    return np.stack([np.interp(r_fine, r_nodes, eye[j]) for j in range(len(r_nodes))], axis=1)


NODE_INTERPOLATIONS = ("linear", "cubic")


def cubic_basis(r_nodes: np.ndarray, r_fine: np.ndarray) -> np.ndarray:
    """(len(r_fine), len(r_nodes)) matrix C of cardinal cubic interpolating
    splines with not-a-knot end conditions: x(r_fine) = C @ x(r_nodes) is the
    C2 cubic spline through the node values (2026-10-10).

    Not-a-knot instead of "natural" ends: natural forces x'' = 0 at r=0 and
    r=Dmax, but a PDDF grows like r^2 near 0 (test.dat, j0, uniform 45
    nodes: chi2_r natural 2.42 vs not-a-knot 0.65).
    The cardinal functions have negative lobes (down to about -3 on a log
    grid), so non-negative node values do NOT imply x(r) >= 0 in between
    and a non-negative kernel becomes slightly signed -> not for EM /
    positivity-constrained solvers (use linear, or the B-spline option,
    whose basis functions are >= 0)."""
    from scipy.interpolate import CubicSpline
    r_nodes = np.asarray(r_nodes, dtype=float)
    if len(r_nodes) < 4:
        raise ValueError("cubic interpolation needs at least 4 r grid points")
    return CubicSpline(r_nodes, np.eye(len(r_nodes)), bc_type="not-a-knot", axis=0)(r_fine)


def node_basis(r_nodes: np.ndarray, r_fine: np.ndarray, interpolation: str = "linear") -> np.ndarray:
    if interpolation == "linear":
        return hat_basis(r_nodes, r_fine)
    if interpolation == "cubic":
        return cubic_basis(r_nodes, r_fine)
    raise ValueError(f"unknown interpolation {interpolation!r}; use one of {NODE_INTERPOLATIONS}")


def build_kernel_on_nodes(
    q: np.ndarray, r_nodes: np.ndarray, form_factor_squared, alpha: float = 0.0,
    n_quad: int | None = None, interpolation: str = "linear",
) -> np.ndarray:
    """Kernel for unknowns x(r_nodes) with x(r) piecewise linear between the
    nodes, integrated on a fine equidistant grid over [0, r_nodes[-1]].

    Needed for non-uniform grids (quadratic/log), whose wide steps at
    large r undersample the kernel, and for coarse uniform grids with step
    > pi/q_max. test.dat, j0 kernel, Bayesian evidence: quadratic grid with
    60 nodes chi2_r 20.4 (direct) -> 0.53 (this); uniform 30 nodes 580 ->
    282 (still too few nodes); quadratic 30 nodes 516 -> 1.41.

    interpolation="cubic" (2026-10-10): cubic not-a-knot spline between the
    nodes (cubic_basis). Same test, Bayesian evidence chi2_r linear -> cubic:
    uniform 30 nodes 282 -> 1.18, uniform 45 13.9 -> 0.65, log 30 2.29 ->
    0.81, quadratic 30 1.41 -> 0.79; no change on fine grids (uniform 95).
    """
    r_nodes = np.asarray(r_nodes, dtype=float)
    r_max = float(r_nodes[-1])
    if n_quad is None:
        n_quad = quadrature_points(r_max, float(np.max(q)), len(r_nodes))
    r_fine = np.linspace(0.0, r_max, n_quad)
    A_fine = build_size_distribution_kernel(q, r_fine, form_factor_squared, alpha=alpha)
    return A_fine @ node_basis(r_nodes, r_fine, interpolation)
