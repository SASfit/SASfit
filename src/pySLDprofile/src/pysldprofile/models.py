"""Test profiles and simulated data in the style of Hansen (2016), sect. 3.1."""

from __future__ import annotations

import numpy as np

from . import geometry as geo
from .resolution import NoResolution

__all__ = ["intensity_from_profile", "homogeneous", "core_shell", "simulate"]


def intensity_from_profile(q, rho_func, r_max, geometry="sphere", n=4000, resolution=None):
    """Exact (fine-shell) intensity of an arbitrary radial profile rho_func(r), 0 <= r <= r_max."""
    res = NoResolution() if resolution is None else resolution
    qf, R = res.setup(np.asarray(q, float))
    e = geo.shell_edges(r_max, n)
    rho = rho_func(0.5 * (e[1:] + e[:-1]))
    a = geo.amplitude_matrix(qf, r_max, n, geometry)
    If = geo.prefactor(qf, geometry) * (a @ rho) ** 2
    return If if R is None else R @ If


def homogeneous(r):
    return np.ones_like(np.asarray(r, float))


def core_shell(r_core, rho_core, rho_shell):
    """Step profile: rho_core for r < r_core, rho_shell beyond."""

    def f(r):
        r = np.asarray(r, float)
        return np.where(r < r_core, rho_core, rho_shell)

    return f


def simulate(q, rho_func, r_max, geometry="sphere", rel_noise=0.01, abs_noise=1e-4, resolution=None, seed=0, normalise=True):
    """Simulated data I(q) with Gaussian noise sigma = rel_noise*I + abs_noise*I(0).

    Hansen (2016) uses 5 % relative noise (homogeneous sphere) or
    1 % + 1e-4 (inhomogeneous examples, I(0) = 1).
    Returns (q, I_noisy, sigma, I_true, scale) where I_true is normalised to
    I(0) = 1 (sphere) or I(q_min) = 1 (cylinder, lamella) if ``normalise``; ``scale`` is the applied factor.
    """
    q = np.asarray(q, float)
    It = intensity_from_profile(q, rho_func, r_max, geometry, resolution=resolution)
    if normalise:
        # sphere: I(0) = 1; cylinder / lamella (I diverges at q -> 0): I(q_min) = 1
        q_ref = 1e-6 if geometry == "sphere" else q.min()
        I0 = intensity_from_profile(np.array([q_ref]), rho_func, r_max, geometry)[0]
        scale = 1.0 / I0
    else:
        scale = 1.0
    It = It * scale
    sigma = rel_noise * It + abs_noise
    rng = np.random.default_rng(seed)
    return q, It + rng.normal(size=q.size) * sigma, sigma, It, scale
