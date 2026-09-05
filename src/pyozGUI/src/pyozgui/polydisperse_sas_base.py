# -*- coding: utf-8 -*-
"""
Shared SAS machinery: exact I(Q) from partial structure factors, plus all six
approximate approaches SASfit implements.

Split out of polydisperse_yukawa_sas.py so that the hard-sphere-Yukawa MSA
model and the Robertus multicomponent sticky-hard-sphere model run through
BYTE-IDENTICAL approximation code. Otherwise a difference between the two
comparisons could just be a difference in how the approximations were coded.

A concrete model must provide:
    self.R        (n,)  radii of the size classes
    self.w        (n,)  number weights, sum to 1
    self.sigma    (n,)  interaction (hard-core) diameters
    self.n_tot    float total number density
    self.ff             form factor object with .F(Q, R) -> (nQ, n)
    self.S_partials(Q)  -> (nQ, n, n) Ashcroft-Landreth partials
    self._mono_S(Q, sigma_eff) -> (nQ,) monodisperse S(Q) at that diameter,
                                  same thermodynamic state; must raise
                                  RuntimeError where no solution exists.

Formulas transcribed from SASfit's own manual, doc/manual/SASfit_ch3.tex,
section "Methods to include structure factors". The GUI radiobuttons in
sasfit.vfs/lib/app-sasfit/tcl/sasfit_analytical.tcl expose options 0-4; the
manual documents a sixth (van der Waals one-fluid) that the GUI does not.

  0 monodisperse             I = n <F^2> S(Q)
  1 decoupling (Kotlarchyk & Chen 1983)
                             I = n{<F^2> + <F>^2 [S(Q)-1]}
  2 local monodisperse (Pedersen 1994)
                             I = n sum_i w_i F_i^2 S(Q; R_i)
  3 partial structure factors
                             I = n{<F^2> + sum_ij w_i w_j F_i F_j [S_ij-1]}
  4 scaling (Gazzillo et al. 1999)   as 3, each term x Vbar_ij/V_av
  5 van der Waals one-fluid          as 4, normalised by <Vbar>

In 3-5 the "partial" structure factor is NOT the true S_ij: it is the
MONODISPERSE S(Q) at the mean radius (R_i+R_j)/2 -- exactly the approximation
that having a real S_ij removes. Vbar_ij = (4pi/3)((R_i+R_j)/2)^3,
V_av = <V_i>, <Vbar> = sum_ij w_i w_j Vbar_ij.
"""
import numpy as np


class PolydisperseSASBase:
    """Mixin providing exact I(Q) and SASfit's six approximations."""

    # ---- intensity ----
    def I_exact(self, Q):
        """I(Q) = sum_ij sqrt(n_i n_j) F_i F_j S_ij^AL  -- no approximation."""
        Q = np.atleast_1d(np.asarray(Q, float))
        F = self.ff.F(Q, self.R)                     # (nQ, nbins)
        sn = np.sqrt(self.rho)
        G = F*sn[None, :]
        S = self.S_partials(Q)                       # (nQ, N, N)
        return np.einsum('qi,qij,qj->q', G, S, G)

    def _mono_S(self, Q, sigma_eff):
        """Monodisperse S(Q) at diameter sigma_eff, same thermodynamic state.
        MODEL-SPECIFIC: must be provided by the concrete subclass, and must
        raise RuntimeError where that fictitious monodisperse system has no
        physical solution."""
        raise NotImplementedError

    def I_dilute(self, Q):
        """phi -> 0 limit: n_tot <F^2>."""
        Q = np.atleast_1d(np.asarray(Q, float))
        F = self.ff.F(Q, self.R)
        return self.n_tot*np.sum(self.w[None, :]*F**2, axis=1)

    # ---- the approximations, for comparison ----
    def sigma_eff(self):
        """volume-equivalent effective diameter, <sigma^3>^(1/3)."""
        return float(np.sum(self.w*self.sigma**3)**(1/3))

    def I_decoupling(self, Q, sigma_eff=None):
        """Kotlarchyk-Chen: I = n <F^2> [1 + beta (S_mono - 1)]."""
        Q = np.atleast_1d(np.asarray(Q, float))
        F = self.ff.F(Q, self.R)
        F2 = np.sum(self.w[None, :]*F**2, axis=1)
        F1 = np.sum(self.w[None, :]*F, axis=1)
        beta = np.where(F2 > 0, F1**2/np.maximum(F2, 1e-300), 1.0)
        S = self._mono_S(Q, self.sigma_eff() if sigma_eff is None else sigma_eff)
        return self.n_tot*F2*(1 + beta*(S - 1))

    def I_lma(self, Q, max_skipped_weight=1e-3):
        """Pedersen local monodisperse: each size in its own monodisperse bath.

        CAVEAT (not an implementation detail -- a property of the
        approximation): LMA requires an auxiliary MONODISPERSE MSA solution at
        diameter sigma_i for every bin, and some of those fictitious systems
        have no physical MSA solution even when the real polydisperse system
        does. The small-sigma bins are the ones that fail: they correspond to
        weak screening (Z_i = z*sigma_i of order 1) at the full volume
        fraction, where the one-Yukawa MSA has no root. So LMA can be
        UNDEFINED on parameters where the exact calculation is perfectly
        well-posed.

        Bins with no solution are skipped and their weight reported in
        `self.lma_skipped_weight`; if that exceeds max_skipped_weight the
        result is meaningless and a ValueError is raised rather than returning
        a quietly-wrong curve.
        """
        Q = np.atleast_1d(np.asarray(Q, float))
        F = self.ff.F(Q, self.R)
        out = np.zeros(len(Q))
        skipped = 0.0
        self.lma_skipped_bins = []
        for i, s in enumerate(self.sigma):
            try:
                out += self.w[i]*F[:, i]**2*self._mono_S(Q, s)
            except RuntimeError:
                skipped += self.w[i]
                self.lma_skipped_bins.append((i, float(s), float(self.z*s), float(self.w[i])))
        self.lma_skipped_weight = skipped
        if skipped > max_skipped_weight:
            raise ValueError(
                f"LMA undefined: bins carrying {skipped:.3g} of the number "
                f"weight have no monodisperse MSA solution (weak screening at "
                f"this phi). Skipped bins (i, sigma, z*sigma, w): "
                f"{self.lma_skipped_bins}")
        if skipped > 0:
            out = out/(1.0 - skipped)     # renormalise over the solvable bins
        return self.n_tot*out

    # ---- SASfit's full set of approaches (doc/manual/SASfit_ch3.tex) ----
    def I_monodisperse(self, Q, sigma_eff=None):
        """SASfit option 0: I = n <F^2> S(Q), the size-averaged form factor
        times a single monodisperse S(Q). S is taken at the volume-equivalent
        effective diameter <sigma^3>^(1/3)."""
        Q = np.atleast_1d(np.asarray(Q, float))
        F = self.ff.F(Q, self.R)
        F2 = np.sum(self.w[None, :]*F**2, axis=1)
        S = self._mono_S(Q, self.sigma_eff() if sigma_eff is None else sigma_eff)
        return self.n_tot*F2*S

    def _pair_sum(self, Q, weight, max_skipped_weight=1e-3, tag="method"):
        """sum_ij w_i w_j F_i F_j * weight_ij * [S(Q; (R_i+R_j)/2) - 1],
        shared by SASfit options 3, 4 and 5. `weight` is a callable (i, j) ->
        float or None for unity."""
        Q = np.atleast_1d(np.asarray(Q, float))
        F = self.ff.F(Q, self.R)
        N = len(self.R)
        out = np.zeros(len(Q))
        skipped = 0.0
        self.pair_skipped_bins = []
        for i in range(N):
            for j in range(N):
                sij = 0.5*(self.sigma[i] + self.sigma[j])   # = 2*(R_i+R_j)/2
                try:
                    Sm = self._mono_S(Q, sij)
                except RuntimeError:
                    skipped += self.w[i]*self.w[j]
                    self.pair_skipped_bins.append((i, j, float(sij)))
                    continue
                wt = 1.0 if weight is None else weight(i, j)
                out += self.w[i]*self.w[j]*F[:, i]*F[:, j]*wt*(Sm - 1.0)
        self.pair_skipped_weight = skipped
        if skipped > max_skipped_weight:
            raise ValueError(
                f"{tag} undefined: pairs carrying {skipped:.3g} of the weight "
                f"have no monodisperse MSA solution at their mean radius "
                f"(weak screening at this phi).")
        F2 = np.sum(self.w[None, :]*F**2, axis=1)
        return self.n_tot*(F2 + out)

    def _Vbar(self, i, j):
        """(4pi/3)((R_i+R_j)/2)^3 -- SASfit's Vbar_ij."""
        return 4*np.pi*(0.5*(self.R[i] + self.R[j]))**3/3

    def I_partial_sf(self, Q, **kw):
        """SASfit option 3: partial structure factors, with the monodisperse
        S(Q) evaluated at the mean radius (R_i+R_j)/2 in place of the true
        S_ij. Compare against I_exact to isolate exactly that substitution."""
        return self._pair_sum(Q, None, tag="partial structure factors", **kw)

    def I_scaling(self, Q, **kw):
        """SASfit option 4, Gazzillo et al. (1999): option 3 with each pair
        term weighted by Vbar_ij/V_av, V_av = <V_i>."""
        V_av = float(np.sum(self.w*4*np.pi*self.R**3/3))
        return self._pair_sum(Q, lambda i, j: self._Vbar(i, j)/V_av,
                              tag="scaling approximation", **kw)

    def I_vdw1(self, Q, **kw):
        """SASfit's van der Waals one-fluid approximation (documented in
        SASfit_ch3.tex but not among the GUI's five radiobuttons): option 4
        normalised by <Vbar> = sum_ij w_i w_j Vbar_ij instead of V_av."""
        Vx = float(sum(self.w[i]*self.w[j]*self._Vbar(i, j)
                       for i in range(len(self.R)) for j in range(len(self.R))))
        return self._pair_sum(Q, lambda i, j: self._Vbar(i, j)/Vx,
                              tag="van der Waals one-fluid", **kw)
