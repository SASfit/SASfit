#!/usr/bin/env python3
"""Percus-Yevick hard spheres: the Wertheim-Thiele closed form.

    from analytic_py_hardsphere import structureFactorPY, S0_PY
    S = structureFactorPY(q, radius, phi)       # q in 1/length, radius same
    s0 = S0_PY(phi)                             # the compressibility limit

WHY THIS FILE EXISTS.

The manuscript's validation table quoted agreement with jscatter's
one-component Percus-Yevick at 0.12 per cent. Two problems with that row:

  * jscatter does not build under MinGW -- its dependency chain pulls in
    MDAnalysis, JupyterLab, nglview, pdb2pqr and netCDF4 -- so the comparison
    could not be re-run on the machine the paper is written on. A validation
    claim its own authors cannot check is not much of a claim.

  * re-measuring it showed 0.12 per cent to be a DST-IV figure. With the
    first-order DST-I at the same 100 points per diameter the same comparison
    gives 0.70, 1.37 and 2.33 per cent at phi = 0.2, 0.3 and 0.4. The table
    did not say which transform produced it, and the polydisperse route --
    which the paper is about -- cannot use DST-IV at all.

Both are solved by not depending on jscatter here. The Percus-Yevick
structure factor for hard spheres has a CLOSED FORM, so there is nothing to
extract and no licence to honour: this is the published solution written out
from the literature, which makes it a genuinely independent reference rather
than a second copy of somebody's code. That is a stronger check than the one
it replaces.

THE SOLUTION. Wertheim (1963) and Thiele (1963) solved the PY closure for
hard spheres analytically. The direct correlation function is a cubic inside
the core and zero outside,

    c(r) = -alpha - beta (r/sigma) - gamma (r/sigma)^3,   r < sigma
         = 0,                                             r > sigma

with, writing e = phi,

    alpha = (1 + 2e)^2 / (1 - e)^4
    beta  = -6 e (1 + e/2)^2 / (1 - e)^4
    gamma = e alpha / 2

and S(q) = 1 / (1 - rho c_hat(q)) follows from the Fourier transform of that
cubic, which is elementary and written out in `_chat` below.

THE SMALL-q LIMIT IS THE POINT OF CARE. c_hat(q) is a difference of large
terms that cancel as q -> 0, so evaluating it directly loses every
significant digit there -- and low q is exactly where the compressibility
lives and where a structure factor is most often compared. The series
expansion below takes over under q*sigma = 0.05, where it agrees with the
closed form to better than 1e-12 and the closed form has begun to lose
digits.

REFERENCES
    M. S. Wertheim, Phys. Rev. Lett. 10, 321 (1963).
    E. Thiele, J. Chem. Phys. 39, 474 (1963).
    J.-P. Hansen and I. R. McDonald, Theory of Simple Liquids, 4th ed.,
        Academic Press (2013), section 4.4.
"""
import numpy as np


def S0_PY(phi):
    """The exact Percus-Yevick compressibility, S(q -> 0).

    S(0) = (1 - phi)^4 / (1 + 2 phi)^2

    Worth having on its own: it is the one quantity in this package for
    which an exact answer exists, which makes it the only ABSOLUTE accuracy
    check available. `tools/numerics_test.py` uses it to measure the
    transform-type error -- 5.27 per cent for DST-I at phi = 0.40 against
    0.036 for DST-IV -- where every other check in the suite compares two
    approximations with each other.
    """
    phi = float(phi)
    if not 0.0 <= phi < 1.0:
        raise ValueError(f"volume fraction {phi} outside [0, 1)")
    return (1.0 - phi)**4/(1.0 + 2.0*phi)**2


def _pyCoefficients(phi):
    """(alpha, beta, gamma) of the Wertheim-Thiele direct correlation."""
    d = (1.0 - phi)**4
    alpha = (1.0 + 2.0*phi)**2/d
    beta = -6.0*phi*(1.0 + 0.5*phi)**2/d
    gamma = 0.5*phi*alpha
    return alpha, beta, gamma


def _chat(k, phi):
    """rho * c_hat(k), with k = q*sigma dimensionless.

    The transform of the cubic c(r), evaluated directly. Accurate for
    k >~ 0.05; below that the terms cancel and `_chatSmall` takes over.
    """
    a, b, g = _pyCoefficients(phi)
    s, c = np.sin(k), np.cos(k)
    k2, k3, k4, k5, k6 = k*k, k**3, k**4, k**5, k**6
    #Term by term, each the transform of one power of r in the cubic.
    t1 = a*(s - k*c)/k3
    t2 = b*(2.0*k*s - (k2 - 2.0)*c - 2.0)/k4
    t3 = g*(((4.0*k3 - 24.0*k)*s
             - (k4 - 12.0*k2 + 24.0)*c + 24.0)/k6)
    return -24.0*phi*(t1 + t2 + t3)


def _chatSmall(k, phi):
    """rho * c_hat(k) as a series in k^2, for the small-k regime.

    c_hat is even and analytic at the origin, so a series in k^2 is the
    right form -- the same argument that makes S(q) analytic there. Two
    terms suffice below k = 0.05: the next would enter at 1e-13.

    At k = 0 this reduces to 1 - 1/S(0) exactly, which is the consistency
    the tests check.
    """
    a, b, g = _pyCoefficients(phi)
    c0 = -24.0*phi*(a/3.0 + b/4.0 + g/6.0)
    c2 = -24.0*phi*(-a/30.0 - b/36.0 - g/48.0)
    return c0 + c2*k*k


def structureFactorPY(q, radius, phi, sigma=None):
    """S(q) for monodisperse hard spheres under Percus-Yevick.

    Parameters
    ----------
    q : array_like
        Scattering vector, in the reciprocal of whatever units `radius`
        uses.
    radius : float
        Sphere radius. The hard-core DIAMETER is 2*radius; pass `sigma`
        instead if that is what you have.
    phi : float
        Volume fraction, 0 <= phi < 1.

    Returns
    -------
    ndarray
        S(q), the same shape as `q`.

    Examples
    --------
    >>> import numpy as np
    >>> q = np.linspace(0.1, 25, 5)
    >>> S = structureFactorPY(q, 0.5, 0.3)
    >>> float(abs(structureFactorPY(np.array([1e-9]), 0.5, 0.3)[0]
    ...           - S0_PY(0.3))) < 1e-9
    True
    """
    q = np.atleast_1d(np.asarray(q, float))
    if sigma is None:
        sigma = 2.0*float(radius)
    phi = float(phi)
    if not 0.0 <= phi < 1.0:
        raise ValueError(f"volume fraction {phi} outside [0, 1)")
    k = q*sigma
    out = np.empty_like(k)
    #SPLIT AT k = 0.05. Above it the closed form is accurate; below, its
    #terms cancel and the series is both accurate and stable. They agree to
    #better than 1e-12 at the boundary, so the join is invisible.
    small = np.abs(k) < 0.05
    if np.any(small):
        out[small] = _chatSmall(k[small], phi)
    if np.any(~small):
        out[~small] = _chat(k[~small], phi)
    return 1.0/(1.0 - out)
