# -*- coding: utf-8 -*-
"""
DRAFT / not yet fully validated. General N-species, single-Yukawa-term (M=1)
MSA solver following polydisperse_yukawa_spec.md sections 2-6 (equations
cross-checked against Blum & Arias 2006 and Blum & Hernando 2002 page images).

This is scaffolding for the M=1 validation task, per this repo's AI_USAGE.md --
not to be treated as an authoritative derivation until the checks in the spec's
"Open items" section are resolved.
"""
import math

import numpy as np
from scipy.optimize import fsolve


# --- BH02 eq.44-46 auxiliary functions -------------------------------------
# phi0(x) = (1-e^-x)/x ;  phi1(x) = (1-x-e^-x)/x^2 ;
# psi1(x) = (1-x/2-(1+x/2)e^-x)/x^3
# All three are entire (removable singularities at x=0), so each is evaluated
# by its Taylor series for small |x| and by the closed form otherwise.
#
# NOTE (bug fix): earlier versions of this file carried series branches for
# phi1 and psi1 that disagreed with their own closed forms -- phi1's had the
# wrong SIGN (+1/2 instead of -1/2 at x=0) and psi1's was unrelated to the
# correct -1/12. They were dormant, since their thresholds (1e-6, 1e-5) are
# never reached for physical z*sigma, so no previously reported result is
# affected. They are corrected here because the S(Q) assembly evaluates these
# same functions at s = -iQ, where small |x| IS reached as Q -> 0.
#
# The closed forms also lose precision by cancellation for small |x| (psi1's
# numerator is O(x^3/12), so it is worthless below |x| ~ 1e-4), which is why
# the switch-over is at |x| = 0.5 with enough series terms to be exact there
# to well below machine precision. Complex-safe: the S(Q) assembly needs
# imaginary arguments.
_NTERM = 16


def _phi0(x):
    x = np.asarray(x)
    cx = np.asarray(x, dtype=complex)
    small = np.abs(cx) < 0.5
    xs = np.where(small, 1.0, cx)
    ser = np.zeros_like(cx)
    for n in range(_NTERM, -1, -1):                      # sum (-x)^n/(n+1)!
        ser = ser + (-cx)**n/math.factorial(n + 1)
    out = np.where(small, ser, (1 - np.exp(-xs))/xs)
    return out if np.iscomplexobj(x) else out.real


def _phi1(x):
    x = np.asarray(x)
    cx = np.asarray(x, dtype=complex)
    small = np.abs(cx) < 0.5
    xs = np.where(small, 1.0, cx)
    ser = np.zeros_like(cx)
    for n in range(_NTERM, -1, -1):                      # -sum (-x)^n/(n+2)!
        ser = ser - (-cx)**n/math.factorial(n + 2)
    out = np.where(small, ser, (1 - cx - np.exp(-xs))/xs**2)
    return out if np.iscomplexobj(x) else out.real


def _psi1(x):
    x = np.asarray(x)
    cx = np.asarray(x, dtype=complex)
    small = np.abs(cx) < 0.5
    xs = np.where(small, 1.0, cx)
    ser = np.zeros_like(cx)
    for n in range(_NTERM, -1, -1):        # -sum (-x)^n (n+1)/(2 (n+3)!)
        ser = ser - (-cx)**n*(n + 1)/(2*math.factorial(n + 3))
    out = np.where(small, ser,
                   (1 - cx/2 - (1 + cx/2)*np.exp(-xs))/xs**3)
    return out if np.iscomplexobj(x) else out.real


class PolydisperseOneYukawaMSA:
    """N species, one Yukawa term (M=1). sigma, rho: length-N arrays.
    z, K: scalars (Yukawa screening length and coupling constant).
    delta: length-N per-species amplitude (charge/coupling polydispersity);
    defaults to all-ones (pure size polydispersity, common coupling)."""

    def __init__(self, sigma, rho, z, K, delta=None):
        self.sigma = np.asarray(sigma, dtype=float)
        self.rho = np.asarray(rho, dtype=float)
        self.z = float(z)
        self.K = float(K)
        N = len(self.sigma)
        self.N = N
        self.delta = np.ones(N) if delta is None else np.asarray(delta, dtype=float)

        # --- geometric knowns: spec section 2 ---
        self.zeta2 = np.sum(self.rho * self.sigma**2)
        self.zeta3 = np.sum(self.rho * self.sigma**3)
        self.Delta_geo = 1 - np.pi*self.zeta3/6
        self.A0 = (2*np.pi/self.Delta_geo) * (1 + 0.5*self.zeta2*(np.pi/self.Delta_geo)*self.sigma)
        self.beta0 = (np.pi/self.Delta_geo) * self.sigma

        # --- geometric auxiliary matrices: spec section 4 ---
        z, sig, rho = self.z, self.sigma, self.rho
        phi0_z = _phi0(z*sig)
        psi1_z = _psi1(z*sig)

        # BH02 eq.35/38: A^0, beta^0 carry the SUMMED index (l, the column --
        # same as rho_l), while sigma/phi0/psi1 carry the FIXED row index j.
        # (An earlier version of this matrix had these two roles swapped --
        # invisible for N=1, since j==l always there, but wrong for N>1.)
        Ihat = np.zeros((N, N))
        Jhat = np.zeros((N, N))
        for j in range(N):
            for l in range(N):
                kron = 1.0 if j == l else 0.0
                Ihat[j, l] = kron + rho[l]*(self.beta0[l]*(sig[j]**2/2)*phi0_z[j]
                                             - (self.A0[l] + z*self.beta0[l])*sig[j]**3*psi1_z[j])
                # eq.38 EXACTLY AS PRINTED (verified at high zoom on the page
                # image): beta^0 carries the summed index l, and sigma^3/psi1
                # carry the row index j. This as-printed form is superseded
                # below -- it disagrees with eq.30/32/33 at N>1; see the note
                # there. Kept only as self.Jhat_printed for comparison.
                Jhat[j, l] = (sig[j]*phi0_z[j] if j == l else 0.0) - 2*rho[l]*self.beta0[l]*sig[j]**3*psi1_z[j]
        self.Ihat, self.Jhat = Ihat, Jhat

        xi_hat = np.zeros(N)
        gamma_hat = np.zeros(N)
        for j in range(N):
            xi_hat[j] = -(1.0/z**2) * np.sum(rho*self.delta*(z*self.beta0[j] + self.A0[j]*(1 + z*sig/2)))
            gamma_hat[j] = self.delta[j] - (2*self.beta0[j]/z**2) * np.sum(rho*self.delta*(1 + z*sig/2))
        self.xi_hat, self.gamma_hat = xi_hat, gamma_hat

        # --- Ihat/Jhat rebuilt from BH02 eq.30/32/33 (spec section 7k) ------
        # eq.35 and eq.38 as PRINTED both disagree with the eq.30+32+33 route
        # at N>1 (by ~2e-2 and ~1e-2 respectively) while agreeing at N=1. The
        # eq.30/32/33 route is the trustworthy one: substituting eq.30 into
        # eq.33 and matching eq.37 reproduces eq.39's gamma_hat EXACTLY as
        # printed, and eq.36's xi_hat likewise comes out exact -- so the two
        # VECTORS are right and only the two MATRICES carry a j <-> l
        # interchange. (Independently, eq.27 and eq.28 agree exactly on P^(n)
        # at all N, while eq.31 does not.) For Jhat the interchange is
        # explicit: the derivation gives
        #     Jhat_jl = d_jl sigma_j phi0(z sigma_j)
        #               - 2 rho_l beta0_j sigma_l^3 psi1(z sigma_l)
        # whereas eq.38 prints beta0_l ... sigma_j^3 psi1(z sigma_j).
        # Rather than hand-transcribe the corrected matrices, they are obtained
        # here by exactly probing the affine map Bhat -> (Pi, X) of eq.30/32/33,
        # which is transcription-free. self.Ihat_printed / self.Jhat_printed
        # keep the as-printed versions for comparison.
        self.Ihat_printed, self.Jhat_printed = Ihat, Jhat
        self.xi_hat_printed, self.gamma_hat_printed = xi_hat, gamma_hat
        xi_hat, Ihat, gamma_hat, Jhat = self._PiX_affine_from_eq30_32_33()
        self.Ihat, self.Jhat = Ihat, Jhat
        self.xi_hat, self.gamma_hat = xi_hat, gamma_hat

        # rescaled/transposed I, J used in the primary closure (BH02 eq 56)
        I = np.zeros((N, N))
        J = np.zeros((N, N))
        for j in range(N):
            for l in range(N):
                I[j, l] = Ihat[l, j] * rho[l]/rho[j]
                J[j, l] = Jhat[l, j] * rho[l]/rho[j]
        self.I, self.J = I, J

    def _PiX_eq30_32_33(self, Bhat):
        """(Pi, X, Delta^(n)) directly from [BH02] eq.30, eq.33, eq.32.
        Explicit in Bhat; touches neither Ihat nor Jhat."""
        rho, sig, z, d, Dg = self.rho, self.sigma, self.z, self.delta, self.Delta_geo
        Dn = (-(2*np.pi/(z**2*Dg))*np.sum(rho*(1 + z*sig/2)*d)          # eq.30
              - (2*np.pi/Dg)*np.sum(rho*sig**3*_psi1(z*sig)*Bhat))
        X = d + sig*Bhat*_phi0(z*sig) + sig*Dn                          # eq.33
        Pi = Bhat + (1 + sig*z/2)*Dn + 0.5*sig*np.sum(rho*self.beta0*X)  # eq.32
        return Pi, X, Dn

    def _PiX_affine_from_eq30_32_33(self):
        """Exact affine decomposition Pi = xi + I@Bhat, X = gamma + J@Bhat
        of eq.30/32/33, recovered by probing with the standard basis."""
        N = self.N
        xi, gam, _ = self._PiX_eq30_32_33(np.zeros(N))
        I = np.zeros((N, N))
        J = np.zeros((N, N))
        for k in range(N):
            ek = np.zeros(N)
            ek[k] = 1.0
            Pk, Xk, _ = self._PiX_eq30_32_33(ek)
            I[:, k] = Pk - xi
            J[:, k] = Xk - gam
        return xi, I, gam, J

    def _Pi_X(self, Bhat):
        Pi = self.xi_hat + self.Ihat @ Bhat
        X = self.gamma_hat + self.Jhat @ Bhat
        return Pi, X

    def _P_Deltan(self, Pi, X):
        rho, beta0, A0, z, Dg = self.rho, self.beta0, self.A0, self.z, self.Delta_geo
        P = (Dg/(np.pi*z)) * np.sum(rho*(A0*X + 2*beta0*(z*X - Pi)))
        Delta_n = -(1.0/z**2) * np.sum(rho*(X*A0 + beta0*(z*X - 2*Pi)))
        return P, Delta_n

    def _rhs_of_Bhat(self, Bhat, a):
        N, rho, sig, z, Dg, A0, beta0 = self.N, self.rho, self.sigma, self.z, self.Delta_geo, self.A0, self.beta0
        Pi, X = self._Pi_X(Bhat)
        P, Delta_n = self._P_Deltan(Pi, X)
        Delta_tilde = np.zeros(N)
        for j in range(N):
            term1 = -np.sum(rho*a*((1.0/z)*A0*(1 + z*sig[j]/2) + beta0))
            term2 = -np.sum(rho*a**2*((np.pi/(z*Dg))*P + 0.5*(Delta_n + (sig[j]*np.pi/(2*Dg))*P)))
            Delta_tilde[j] = term1 + term2
        S2 = np.sum(rho*a**2)
        return 0.5*S2*(-Pi + z*X) + Delta_tilde

    def _solve_Bhat_given_a(self, a):
        """rhs_of_Bhat(Bhat) is affine in Bhat; recover the linear operator by
        probing with the standard basis (exact, not an approximation), then
        solve (2*pi*K*I - M) Bhat = c0."""
        N = self.N
        c0 = self._rhs_of_Bhat(np.zeros(N), a)
        M = np.zeros((N, N))
        for k in range(N):
            ek = np.zeros(N)
            ek[k] = 1.0
            M[:, k] = self._rhs_of_Bhat(ek, a) - c0
        lhs_mat = 2*np.pi*self.K*np.eye(N) - M
        return np.linalg.solve(lhs_mat, c0)

    def _residual_a(self, a, Bhat):
        Pi, X = self._Pi_X(Bhat)
        z, K, delta, I, J = self.z, self.K, self.delta, self.I, self.J
        S2 = np.sum(self.rho*a**2)
        return 2*np.pi*K*delta/z + I@a - (S2/(2*z))*(J@(Pi - z*X) - I@X)

    def solve(self, a0=None, tol=1e-13, maxiter=300):
        N = self.N
        a = np.zeros(N) if a0 is None else np.array(a0, dtype=float)
        Bhat = np.zeros(N)
        for it in range(maxiter):
            Bhat_new = self._solve_Bhat_given_a(a)
            a_new = fsolve(lambda x: self._residual_a(x, Bhat_new), a, xtol=1e-13)
            da = np.max(np.abs(a_new - a))
            dB = np.max(np.abs(Bhat_new - Bhat))
            a, Bhat = a_new, Bhat_new
            if da < tol and dB < tol:
                break
        Pi, X = self._Pi_X(Bhat)
        P, Delta_n = self._P_Deltan(Pi, X)
        A = self.A0 + (np.pi/self.Delta_geo)*a*P
        beta = self.beta0 + a*Delta_n
        res_a = self._residual_a(a, Bhat)
        return dict(a=a, Bhat=Bhat, A=A, beta=beta, P=P, Delta_n=Delta_n,
                    iterations=it+1, residual_a=res_a)

    def contact_value(self, sol):
        """g_ij(sigma_ij+), N species, M=1. Returns the N x N contact matrix.

        2*pi*sigma_ij*g_ij(sigma_ij) = A_j*sigma_i/2 + beta_j
                                       - sum_m z_m*C_ij^(m)*e^{-z_m*sigma_ij}

        i.e. the derivative of the CORE-REGION factor function q^0 (eq.18
        [BH02]) at contact -- note C only, NOT (C+D). eq.20 [BH02] is a
        different quantity: q'_ij(sigma_ji) for the FULL q = q^0 + sum_n D
        e^{-z_n r} (eq.17), whose extra -sum_m z_m D e^{-z_m sigma_ij} term
        does not belong in the contact value. eq.26 [BA] is a third form
        again and matches neither; using it was the cause of the long-standing
        K!=0 discrepancy against liboneyukawa/ozLib (spec sections 7b-7g).

        C_ij^(m) is eliminated via eq.21 + eq.22. For delta_i = 1 (common
        coupling, arbitrary size polydispersity), gtilde's symmetry gives
        sum_k rho_k gtilde_ik e^{z sigma_kj} = e^{z sigma_j/2} Bhat_i
        e^{-z sigma_i/2}/(2*pi), whence
            z*C_ij*e^{-z*sigma_ij} = z*a_j - a_j*Bhat_i*e^{-z*sigma_i}
        and therefore
            2*pi*sigma_ij*g_ij = A_j*sigma_i/2 + beta_j
                                 - z*a_j + a_j*Bhat_i*e^{-z*sigma_i}

        Verified to 1e-14 relative against liboneyukawa's own exact contact
        value (a+b)-c*Z*exp(-Z) over Z in [0.5,10], K in [0.1,3],
        phi in [0.05,0.35]; reduces to the exact PY/Lebowitz hard-sphere
        values as K->0 for N=1 and N=2; and comes out symmetric
        (g_ij = g_ji, eq.77 [BH02]) to ~1e-12 without that being imposed.

        GENERAL delta_i (spec 7m). This works for ANY delta once eq.22 is
        corrected to D_ij = -delta_I a_j e^{z sigma_ij} (delta carries the ROW
        index i, not the column index j as printed). With that correction
        eq.21 contracts gtilde with a delta weight -- which is exactly what
        Bhat supplies -- so C_ij closes in CLOSED FORM for any delta:
            C_ij = delta_i a_j e^{z sigma_ij} - (a_j/z) e^{z sigma_j/2} Bhat_i e^{-z sigma_i/2}
            z C_ij e^{-z sigma_ij} = z delta_i a_j - a_j Bhat_i e^{-z sigma_i}
        giving
            2 pi sigma_ij g_ij = A_j sigma_i/2 + beta_j
                                 - z delta_i a_j + a_j Bhat_i e^{-z sigma_i}
        which reduces to the delta=1 form above. Evidence for the correction:
        with eq.22 as printed the contact matrix comes out ASYMMETRIC at
        delta != 1 (|g-g^T| ~ 0.26-0.35, against a physical requirement), and
        disagrees with the independent numerical OZ solver by 15-21%; with
        delta on the row index, |g-g^T| drops to ~5e-6 and the agreement
        reaches the numerical method's own floor (~2.6e-3), the same as at
        delta=1. Invisible at delta=1, like the eq.35/38 interchange.
        """
        a, A, beta, Bhat = sol['a'], sol['A'], sol['beta'], sol['Bhat']
        z, sig, N, delta = self.z, self.sigma, self.N, self.delta
        g = np.zeros((N, N))
        for i in range(N):
            for j in range(N):
                sij = 0.5*(sig[i] + sig[j])
                val = (A[j]*sig[i]/2 + beta[j]
                       - z*delta[i]*a[j] + a[j]*Bhat[i]*np.exp(-z*sig[i]))
                g[i, j] = val/(2*np.pi*sij)
        return g

    # ------------------------------------------------------------------
    # Gamma-centric solve (spec section 7k).
    #
    # The solve() above treats the N amplitudes a_j as independent unknowns
    # and imposes eq.55 componentwise (N equations, N unknowns). That is
    # over-determined once [BH02] eq.72 is taken into account, and the N>1
    # solutions it returns violate eq.72 -- see spec 7i/7j.
    #
    # [BH02]'s abstract instead says Gamma is obtained from "M equations
    # together with M(M-1) symmetry conditions". At M=1 that is ONE equation,
    # not N. So the unknown is the SCALAR Gamma plus the N values Bhat_i:
    #
    #   eq.72  Pi_i = -Gamma * X_i           (N equations, LINEAR in Bhat:
    #          (Ihat + Gamma*Jhat) Bhat = -(xi_hat + Gamma*gamma_hat) )
    #   eq.100b  2*pi*K = -2*Gamma*(z+Gamma)/D_2,  D_2 = sum_k rho_k X_k^2
    #                                        (the one remaining scalar equation)
    #   eq.100/75  a_j = 2*Gamma*X_j/D_2     (a_j is DERIVED, never free)
    #
    # N+1 equations, N+1 unknowns; Gamma is species-independent by
    # construction, and the whole thing collapses to a 1-D root find in Gamma.
    # At N=1 this coincides with solve() identically (one equation either way),
    # which is why solve() came out exact there and this defect stayed hidden.
    # ------------------------------------------------------------------

    def _Bhat_X_given_gamma(self, gamma):
        """eq.72 solved for Bhat at fixed Gamma (linear), then Pi, X, D_2."""
        Amat = self.Ihat + gamma*self.Jhat
        rhs = -(self.xi_hat + gamma*self.gamma_hat)
        Bhat = np.linalg.solve(Amat, rhs)
        Pi, X = self._Pi_X(Bhat)
        D2 = np.sum(self.rho*X**2)
        return Bhat, Pi, X, D2

    def _gamma_residual(self, gamma):
        """eq.100b residual: 2*pi*K + 2*Gamma*(z+Gamma)/D_2."""
        try:
            _, _, _, D2 = self._Bhat_X_given_gamma(gamma)
        except np.linalg.LinAlgError:
            return np.nan
        if not np.isfinite(D2) or abs(D2) < 1e-300:
            return np.nan
        return 2*np.pi*self.K + 2*gamma*(self.z + gamma)/D2

    def _assemble(self, gamma):
        Bhat, Pi, X, D2 = self._Bhat_X_given_gamma(gamma)
        a = (2*gamma/D2)*X if abs(D2) > 0 else np.zeros(self.N)
        P, Delta_n = self._P_Deltan(Pi, X)
        A = self.A0 + (np.pi/self.Delta_geo)*a*P
        beta = self.beta0 + a*Delta_n
        return dict(a=a, Bhat=Bhat, A=A, beta=beta, P=P, Delta_n=Delta_n,
                    gamma=gamma, Pi=Pi, X=X, D2=D2,
                    eq72_residual=Pi + gamma*X,
                    eq100b_residual=self._gamma_residual(gamma),
                    residual_a=self._residual_a(a, Bhat))

    def _gamma_grid(self, span=6.0, decades=10, per_decade=400):
        """Search grid for Gamma. Gamma has dimensions of inverse length (it
        appears only as z+Gamma), so the grid is scaled by z rather than being
        absolute -- a fixed [-40,40] grid silently misses the root when lengths
        are in physical units (e.g. Gamma ~ -1e-5 for sigma ~ 100 A). Spacing
        is geometric so that roots many decades below z are still resolved."""
        z = self.z
        mag = np.geomspace(span*z*10.0**(-decades), span*z, decades*per_decade)
        return np.concatenate((-mag[::-1], [0.0], mag))

    def solve_gamma(self, grid=None, all_roots=False):
        """Gamma-centric solve. Returns the physical branch (or all roots)."""
        from scipy.optimize import brentq
        if self.K == 0.0:
            return self._assemble(0.0)          # Gamma -> 0 exactly at K=0
        grid = self._gamma_grid() if grid is None else np.asarray(grid, float)
        res = np.array([self._gamma_residual(g) for g in grid])
        roots = []
        for i in range(len(grid) - 1):
            r0, r1 = res[i], res[i+1]
            if not (np.isfinite(r0) and np.isfinite(r1)) or r0 == 0.0:
                continue
            if np.sign(r0) != np.sign(r1):
                try:
                    g = brentq(self._gamma_residual, grid[i], grid[i+1],
                               xtol=1e-15, rtol=1e-15, maxiter=200)
                except (ValueError, RuntimeError):
                    continue
                if all(abs(g - gg) > 1e-9 for gg in roots):
                    roots.append(g)
        if not roots:
            raise RuntimeError("solve_gamma: no root for Gamma found")
        sols = [self._assemble(g) for g in roots]
        if all_roots:
            return sols
        # physical branch: continuity with Gamma -> 0 as K -> 0, and a
        # positive-definite contact matrix. Prefer the smallest |Gamma| that
        # gives sane (positive) contact values.
        def ok(s):
            try:
                return np.all(self.contact_value(s) > 0)
            except Exception:
                return False
        good = [s for s in sols if ok(s)]
        pool = good if good else sols
        return min(pool, key=lambda s: abs(s['gamma']))
