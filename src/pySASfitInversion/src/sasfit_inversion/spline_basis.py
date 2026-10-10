"""
Glatter-style B-spline basis for p(r) (Glatter, 1977, Acta Cryst. A33,
83-90 -- the original Indirect Fourier Transform method, still used today
in GNOM; Svergun, 1992, J. Appl. Cryst. 25, 495-503). Instead of solving
for p(r) directly on a fine point grid of n values (this package's default
representation, and the one Hansen (2000) and the Bayesian-evidence
solvers here use), Glatter represents p(r) as a linear combination of a
SMALL number of smooth basis functions:

    p(r) = sum_j c_j * phi_j(r)

with ~15-30 cubic B-spline basis functions phi_j, rather than the
~100-200 point values this package's grid typically uses. This is
regularization by dimensionality reduction rather than by a roughness
penalty: a high-frequency oscillation simply isn't representable in a
15-30-dimensional spline basis, so near-Nyquist "ringing" of the kind
investigated in this package's own diagnose_hansen_* scripts (2026-10-06)
is structurally excluded, independent of any regularization-strength
tuning. This is a genuinely different mechanism from the single- or
multi-block lambda*||L p||^2 penalty used elsewhere in this package (see
regularization.py and bayesian_evidence.log_evidence_blocks) -- not a
competitor that needs to "win" against it, but a different, complementary
way of imposing smoothness, in the same spirit as the Glatter-vs-Hansen
split in the IFT literature this package's own docstrings already
reference.

NORMALIZED r: the basis is built on t = r/r_max in [0, 1] (originally on
integer indices 0..n-1, which is the same thing for an equidistant grid), exactly mirroring how
regularization.hann_taper is used in index space inside
solver_registry.py's _run_bayesian_evidence_hann_tapered -- the physical
r grid is always an affine function of index (r = r_max * i/(n-1)), so a
basis built in index space and a basis built in r space span the same
set of representable shapes; building in index space avoids having to
plumb r_max/r_grid into every solver function that currently only
receives (A, b, db).

ENDPOINT CONSTRAINT: Hansen (2000) imposes p(0)=p(Dmax)=0 explicitly as a
term in his regularization functional (see hansen_smoothness_cholesky's
docstring). The spline basis built here imposes the SAME physical
constraint structurally instead, via the standard clamped-B-spline trick:
build an open-uniform (clamped) B-spline basis with n_coeffs+2 control
points, then drop the first and last control points/basis functions. A
clamped B-spline's first basis function is the only one nonzero at the
left domain edge (where it equals 1, i.e. the curve there equals exactly
that control point), and symmetrically for the last basis function at the
right edge -- so every REMAINING (interior) basis function is
*identically* zero at both r=0 and r=r_max, for any choice of
coefficients. p(0)=p(Dmax)=0 therefore holds exactly, not as a penalized
soft preference, with no extra machinery needed in the solvers that use
this basis.
"""
from __future__ import annotations

import numpy as np
from scipy.interpolate import BSpline


SPACINGS = ("uniform", "quadratic", "log")


def interior_knots(n_coeffs: int, degree: int = 3, spacing: str = "uniform") -> np.ndarray:
    """Interior knot positions in normalized r, t = r/r_max in (0, 1).

    The clamped basis has n_coeffs + 2 control points (the two endpoint ones
    are dropped later), hence n_coeffs + 2 - degree - 1 interior knots.

    spacing:
      "uniform"   -- equidistant in r (Glatter 1977; uniform Shannon
                     resolution pi/q_max along r).
      "quadratic" -- t_k = u_k^2 with u uniform: knot density ~ 1/sqrt(t),
                     denser at small r.
      "log"       -- geometric from t_min = 1/(2*n_coeffs) to 1: denser at
                     small r, sparse in the tail.
    On test.dat (2026-10-10) quadratic/log gave chi2_r ~1.0 with balanced
    low/mid/high-q residuals at 20 coefficients, uniform needed ~25 (p(r)
    rises steeply to its peak and has a long smooth tail). Data-dependent:
    a p(r) with structure at large r may prefer uniform.
    """
    k = n_coeffs + 2 - degree - 1
    if k < 0:
        raise ValueError(f"n_coeffs={n_coeffs} too small for degree={degree}")
    if spacing == "uniform":
        return np.linspace(0.0, 1.0, k + 2)[1:-1]
    if spacing == "quadratic":
        return np.linspace(0.0, 1.0, k + 2)[1:-1] ** 2
    if spacing == "log":
        if k == 0:
            return np.empty(0)
        t_min = 1.0 / (2.0 * n_coeffs)
        return np.geomspace(t_min, 1.0, k + 1)[:-1]
    raise ValueError(f"unknown knot spacing {spacing!r}; use one of {SPACINGS}")


def bspline_basis_at(
    t: np.ndarray, n_coeffs: int, degree: int = 3, spacing: str = "uniform"
) -> np.ndarray:
    """(len(t), n_coeffs) basis evaluated at normalized positions t in
    [0, 1] (t = r/r_max). Every column vanishes at t=0 and t=1 (endpoint
    control points dropped, see module docstring). Evaluating the same
    basis on a fine quadrature grid and on the coarse display grid is what
    lets the spline solvers integrate the kernel accurately even when knot
    spans are shorter than the display-grid spacing (log/quadratic knots
    near r=0)."""
    if degree < 1:
        raise ValueError(f"degree must be >= 1, got {degree}")
    if n_coeffs < degree + 1:
        raise ValueError(
            f"n_coeffs={n_coeffs} too small for degree={degree}; need n_coeffs >= {degree + 1}"
        )
    t = np.asarray(t, dtype=float)
    n_ctrl = n_coeffs + 2
    knots = np.concatenate([
        np.zeros(degree + 1), interior_knots(n_coeffs, degree, spacing), np.ones(degree + 1)
    ])
    B_full = np.empty((t.size, n_ctrl))
    eye = np.eye(n_ctrl)
    for j in range(n_ctrl):
        vals = BSpline(knots, eye[j], degree, extrapolate=False)(t)
        B_full[:, j] = np.nan_to_num(vals, nan=0.0)
    # FITPACK returns 0 at the right clamped edge; the true value there is
    # 1 for the last basis function and 0 for all others. That column is
    # dropped below, so the interior columns are already correct.
    return B_full[:, 1:-1]


def build_bspline_basis(
    n: int, n_coeffs: int, degree: int = 3, spacing: str = "uniform"
) -> np.ndarray:
    """
    (n, n_coeffs) basis on n equidistant grid points spanning [0, r_max],
    with x[0] == x[n-1] == 0 for every coefficient vector (see module
    docstring). Thin wrapper around bspline_basis_at(linspace(0, 1, n)).

    n_coeffs: number of free coefficients (Glatter/GNOM typically ~15-30);
    the main regularization knob -- fewer = stiffer.
    spacing: knot placement in r, see interior_knots().

    Raises ValueError if n_coeffs is too small for the degree or larger
    than n - 2.
    """
    if n_coeffs + 2 > n:
        raise ValueError(
            f"n_coeffs={n_coeffs} too large for a grid of n={n} points "
            f"(need n_coeffs <= n - 2)"
        )
    B = bspline_basis_at(np.linspace(0.0, 1.0, n), n_coeffs, degree, spacing)
    assert B.shape == (n, n_coeffs)
    return B
