# -*- coding: utf-8 -*-
"""
Robertus multicomponent sticky-hard-sphere model plugged into the same SAS
machinery as the Yukawa one, so SASfit's six structure-factor approximations
can be scored against an EXACT polydisperse S_ij for a completely different
interaction.

Exact S_ij here comes from SASfit's own C engine
(src/plugins/robertus_shs/robertus_shs_core.c, wrapped by robertusWrapper.py),
after C. Robertus, W.H. Philipse, J.G.H. Joosten, Y.K. Levine, J. Chem. Phys.
90, 4482 (1989). Nothing in the Blum-Hernando machinery is involved, and the
approximation code is byte-identical (PolydisperseSASBase), so this is a fully
independent second test of the same question.

Conventions inherited from the engine: stickiness tau is size-INDEPENDENT
(tau_nm = tau, as the paper assumes), and smaller tau means stickier. The
engine does its own size-class discretisation and normalises so that
(pi/6) sum_i x_i D_i^3 == phi exactly.
"""
import numpy as np

from polydisperse_sas_base import PolydisperseSASBase
from polydisperse_yukawa_sas import Sphere, CoreShell
from robertusWrapper import (RobertusSHS, DIST_LOGNORMAL, DIST_SCHULZ_ZIMM,
                             DIST_GAUSSIAN, DIST_WEIBULL)


class RobertusSHSSAS(PolydisperseSASBase):
    """Exact I(Q) for polydisperse adhesive (sticky) hard spheres.

    Dmean : mean hard-sphere diameter
    srel  : relative width sigma/D of the size distribution
    phi   : hard-sphere volume fraction
    tau   : Baxter stickiness (smaller = stickier)
    """

    def __init__(self, Dmean, srel, phi, tau, formfactor=None, nbins=12,
                 dist=DIST_LOGNORMAL):
        self.Dmean, self.srel, self.phi, self.tau = (float(Dmean), float(srel),
                                                     float(phi), float(tau))
        self.dist = dist
        self.ff = formfactor if formfactor is not None else Sphere()
        self._eng = RobertusSHS(Dmean, srel, phi, tau, p=nbins, dist=dist)
        self.sigma = self._eng.D.copy()                 # interaction diameters
        self.R = self.ff.outer_radius(self.sigma/2.0)
        self.n_tot = float(np.sum(self._eng.x))
        self.rho = self._eng.x.copy()                   # per-class number density
        self.w = self.rho/self.n_tot
        self._mono_cache = {}

    def S_partials(self, Q):
        Q = np.atleast_1d(np.asarray(Q, float))
        return np.array([self._eng.S_matrix(q) for q in Q])

    def S_number(self, Q):
        wv = np.sqrt(self.w)
        return np.array([float(wv @ S @ wv) for S in self.S_partials(Q)])

    def _mono_S(self, Q, sigma_eff):
        """Monodisperse sticky-hard-sphere S(Q) at diameter sigma_eff, same phi
        and tau -- taken from the SAME engine with a single size class, so the
        approximations are built from exactly the physics they assume."""
        key = round(float(sigma_eff), 10)
        if key not in self._mono_cache:
            try:
                self._mono_cache[key] = RobertusSHS(sigma_eff, 1e-6, self.phi,
                                                    self.tau, p=1,
                                                    dist=DIST_SCHULZ_ZIMM)
            except Exception as e:
                raise RuntimeError(str(e))
        eng = self._mono_cache[key]
        return np.array([eng.S_matrix(q)[0, 0] for q in np.atleast_1d(Q)])

    def sigma_eff(self):
        return float(np.sum(self.w*self.sigma**3)**(1/3))
