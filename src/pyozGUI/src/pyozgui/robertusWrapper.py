# -*- coding: utf-8 -*-
"""
Pure Python wrapper for the Robertus multicomponent sticky-hard-sphere solver.
Uses the native Python implementation from robertus_shs_core_py.py.

  C. Robertus, W.H. Philipse, J.G.H. Joosten, Y.K. Levine, J. Chem. Phys. 90,
  4482 (1989) -- PY solution of the multicomponent adhesive-sphere system.

This gives polydisperse S_ij(q) for the adhesive/sticky sphere interaction,
so the same six SASfit approximations can be scored against it for comparison.

The stickiness tau is taken size-INDEPENDENT (tau_nm = tau for all pairs),
as in the paper; classes are sampled at p equally spaced diameters over
Dmean +/- 3 sigma and renormalised so that (pi/6) sum_i x_i D_i^3 == phi.
"""
import numpy as np

# Import the pure Python implementation
try:
    from robertus_shs_core_py import (
        RobertusSHSSolver,
        DIST_SCHULZ_ZIMM,
        DIST_LOGNORMAL,
        DIST_GAUSSIAN,
        DIST_WEIBULL
    )
except ImportError as e:
    raise ImportError(
        "Cannot import robertus_shs_core_py. Please ensure it is in the same directory. "
        f"Error: {e}"
    )

_MAXP = 300

DIST_SCHULZ_ZIMM = DIST_SCHULZ_ZIMM
DIST_LOGNORMAL = DIST_LOGNORMAL
DIST_GAUSSIAN = DIST_GAUSSIAN
DIST_WEIBULL = DIST_WEIBULL
#


class RobertusSHS:
    """Multicomponent sticky-hard-sphere model using Percus-Yevick closure.

    Dmean : mean hard-sphere DIAMETER
    srel  : relative width sigma/D of the size distribution
    phi   : hard-sphere volume fraction
    tau   : Baxter stickiness (SMALLER tau = stickier/more attractive)
    p     : number of size classes (default 12)
    dist  : size distribution type (LOGNORMAL, SCHULZ_ZIMM, GAUSSIAN, WEIBULL)
    """

    def __init__(self, Dmean, srel, phi, tau, p=12, dist=DIST_LOGNORMAL):
        """Initialize the solver using pure Python implementation."""
        self.solver = RobertusSHSSolver(
            Dmean=Dmean,
            srel=srel,
            phi=phi,
            tau=tau,
            nbins=p,
            dist=dist
        )
        self.p = self.solver.p
        self.D = self.solver.D.copy()
        self.x = self.solver.x.copy()
        self.phi = phi
        self.tau = tau

    def S_matrix(self, q):
        """Calculate partial structure factor matrix S_ij(q), returns (p,p) array."""
        return self.solver.S_matrix(q)

