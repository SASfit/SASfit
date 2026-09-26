#!/usr/bin/env python3
"""SHELVED. Ginoza's closed-form MSA for a polydisperse hard-sphere Yukawa
fluid -- superseded in practice by gazzillo_msa.py.

DO NOT FINISH THIS WITHOUT READING THE NEXT PARAGRAPH FIRST.

This module solves for the Blum-Hoye scaling parameter Gamma and the factor
function coefficients, following Ginoza, J. Phys. Soc. Jpn. 55, 95 (1986).
It does NOT compute S(q), and it should not be completed unless the
capability difference below actually matters, because the package already
has a working, validated implementation of the same physics.

WHAT ALREADY EXISTS. `gazzillo_msa.py` implements the analytic MSA
scattering intensity for polydisperse charged hard spheres, from Appendix A
of Gazzillo, Giacometti & Carsughi (arXiv:cond-mat/9909153, the corrected
form of J. Chem. Phys. 107, 10141). It is validated to 3e-14 against the
Wertheim-Thiele PY structure factor and 1e-13 against mixscatter's Vrij
mixture -- machine precision, on a published correction of two misprints in
the earlier paper. Reaching the same S(q) by transcribing Ginoza's equations
out of a scanned 1986 PDF would produce a second implementation of something
already right, by a worse route.

THE ONE REAL DIFFERENCE, and the only reason to revive this. Gazzillo's
corresponding-states approach assumes the valence is PROPORTIONAL TO SURFACE
AREA, so charge polydispersity is fully correlated with size polydispersity
-- one distribution, not two. Ginoza and Yasutomi (Phys. Rev. E 58, 3329
(1998), "Analytical structure factors for colloidal fluids with size and
interaction polydispersities") treat size and interaction polydispersity as
INDEPENDENT. If a system needs charge decoupled from size -- a sample whose
surface chemistry does not scale with area, say -- Gazzillo's route cannot
express it and this one can.

If that day comes, use the 1998 Phys. Rev. E paper rather than the 1986 one:
it gives the structure factors directly, in Eqs. (3.4) onward, and it is
machine-readable rather than a scan.

WHAT IS ESTABLISHED HERE. Gamma solves at every state point tried,
monodisperse and five-class, with Gamma -> 0 as K -> 0 and growing smoothly
with coupling. Ginoza gives the closing condition in two forms, Eq. (38a)
per component and Eq. (38b) summed; both are implemented and they agree to
1e-12, which checks the transcription against itself. It does NOT check it
against Ginoza -- that would need the K -> 0 limit against
tools/analytic_py_hardsphere.py, which cannot run until S(q) exists.

WHY THIS IS WORTH HAVING. The multicomponent Yukawa MSA of Blum and Hoye is
a set of coupled nonlinear algebraic equations whose size grows with the
number of components -- unusable as a fitting reference for a size
distribution. Ginoza shows that for FACTORIZABLE coefficients,

    K_ij = K d_i d_j,                                          Eq. (2)

and a single Yukawa term, the whole solution collapses to rational functions
of ONE scalar parameter Gamma, fixed by a single algebraic equation. Every
component enters only through (sigma_i, rho_i, d_i), so a p-class system
costs O(p) per Gamma evaluation rather than a p-dimensional solve.

That is what makes it usable here: `polydisperse_yukawa_msa.py` is still
marked DRAFT and this package has no validated analytic reference for the
polydisperse Yukawa case.

THE RESTRICTION IS REAL AND MUST BE CHECKED, not assumed. K_ij = K d_i d_j
is an assumption about the system, not an approximation that gets better
with effort. For charged colloids where the coupling scales with surface
charge it is the usual form; where it does not hold, this solution does not
apply at all and the general Blum-Hoye equations are needed.

TRANSCRIPTION. The source is a scanned PDF, so these equations were read by
eye rather than extracted. Two independent checks are built in and run by
`selfTest()`: Ginoza gives the closing condition in two forms, Eq. (38a) per
component and Eq. (38b) summed, the latter obtained from the former by
multiplying by rho_j sigma_j lambda_j and summing over j. They must have the
same root. Computing both is free and catches exactly the kind of sign error
that transcription from a scan produces -- one that yields a plausible wrong
Gamma rather than an obvious failure.
"""
import numpy as np


def _phi0(x):
    """(1 - exp(-x))/x, Ginoza after Eq. (19). Series for small x."""
    x = np.asarray(x, float)
    small = np.abs(x) < 1e-6
    xs = np.where(small, 1.0, x)
    return np.where(small, 1.0 - x/2.0 + x*x/6.0, (1.0 - np.exp(-xs))/xs)


def _phi1(x):
    """(1 - x - exp(-x))/x^2, Ginoza Eq. (15)."""
    x = np.asarray(x, float)
    small = np.abs(x) < 1e-4
    xs = np.where(small, 1.0, x)
    return np.where(small, -0.5 + x/6.0 - x*x/24.0,
                    (1.0 - xs - np.exp(-xs))/(xs*xs))


def _psi1(x):
    """[1 - x/2 - (1 + x/2) exp(-x)]/x^3, Ginoza Eq. (14).

    The small-x series matters: at x = 1e-3 the bracket is a difference of
    numbers agreeing to nine digits, so the direct form loses everything.
    """
    x = np.asarray(x, float)
    small = np.abs(x) < 1e-3
    xs = np.where(small, 1.0, x)
    return np.where(small, 1.0/6.0 - x/8.0 + x*x/20.0,
                    (1.0 - xs/2.0 - (1.0 + xs/2.0)*np.exp(-xs))/(xs**3))


class GinozaOneYukawa:
    """Single-Yukawa MSA for a hard-sphere mixture, factorizable coupling.

    Parameters
    ----------
    sigma : (p,) array
        Hard-sphere diameters.
    rho : (p,) array
        Number densities, same length.
    d : (p,) array
        Yukawa coupling amplitudes, entering only as K_ij = K d_i d_j.
    K, z : float
        Yukawa strength and inverse screening length, in the potential
        beta u_ij(r) = -K d_i d_j exp(-z r)/r for r > sigma_ij.
    """

    def __init__(self, sigma, rho, d, K, z):
        self.sigma = np.atleast_1d(np.asarray(sigma, float))
        self.rho = np.atleast_1d(np.asarray(rho, float))
        self.d = np.atleast_1d(np.asarray(d, float))
        self.K = float(K)
        self.z = float(z)
        if not (self.sigma.shape == self.rho.shape == self.d.shape):
            raise ValueError("sigma, rho and d must have the same length")
        if self.z <= 0:
            raise ValueError(f"screening z must be positive, got {self.z}")
        #Ginoza Eq. (9): zeta_m = sum_l rho_l sigma_l^m, Delta = 1 - pi zeta_3/6.
        self.zeta2 = float(np.sum(self.rho*self.sigma**2))
        self.zeta3 = float(np.sum(self.rho*self.sigma**3))
        self.Delta = 1.0 - np.pi*self.zeta3/6.0
        if self.Delta <= 0:
            raise ValueError(
                f"packing fraction {np.pi*self.zeta3/6:.4f} >= 1: no fluid")
        self._Gamma = None

    # -- the rational functions of Gamma, Ginoza section 4 -----------------
    def _coefficients(self, Gamma):
        """(xi_i, eta_i, lambda_i) of Eq. (33), all rational in Gamma."""
        s, z, D = self.sigma, self.z, self.Delta
        p0 = _phi0(z*s)
        #The common denominator. Its vanishing is the natural domain
        #boundary of the solution and is checked by the caller.
        den = 1.0 + p0*s*Gamma
        xi = (np.pi/(2.0*D))*s**2*p0/den
        eta = (z*s)**2*_psi1(z*s)*s/den
        lam = self.d*np.exp(-z*s/2.0)/den
        return xi, eta, lam, den

    def _solve_X(self, Gamma):
        """X_i, Delta_N from Eqs. (34) and (35)."""
        s, rho, z, D = self.sigma, self.rho, self.z, self.Delta
        xi, eta, lam, den = self._coefficients(Gamma)
        if np.any(den <= 0):
            raise RuntimeError(
                f"Ginoza denominator 1 + phi0(z sigma) sigma Gamma vanished "
                f"at Gamma = {Gamma:.6g} (min {den.min():.3e}): outside the "
                f"domain of the solution, not a numerical failure")
        #Eq. (34): Y_i and Z_i, each a sum over components.
        sum_xi = np.sum(rho*s*xi)
        Y = eta - xi*np.sum(rho*s*eta)/(1.0 + sum_xi)
        Z = lam - xi*np.sum(rho*s*lam)/(1.0 + sum_xi)
        #Eq. (35): Delta_N as a ratio of two sums.
        p0, p1 = _phi0(z*s), _psi1(z*s)
        w = p1/_phi0(z*s)
        num = -(2.0*np.pi/D)*np.sum(
            rho*s**2*((Z - self.d*np.exp(-z*s/2.0))*w
                      + self.d*np.exp(-z*s/2.0)*(1.0 + z*s/2.0)/((z*s)**2)))
        dnm = 1.0 - (2.0*np.pi/D)*np.sum(rho*s**2*w*(Y + s))
        if abs(dnm) < 1e-300:
            raise RuntimeError(f"Eq. (35) denominator vanished at "
                               f"Gamma = {Gamma:.6g}")
        Delta_N = num/dnm
        X = Z - Delta_N*Y
        return X, Delta_N, xi, eta, lam

    def _closure38b(self, Gamma):
        """Ginoza Eq. (38b), the summed closing condition on Gamma."""
        s, rho, z, D = self.sigma, self.rho, self.z, self.Delta
        X, Delta_N, xi, eta, lam = self._solve_X(Gamma)
        Dsum = float(np.sum(rho*X**2))            # Eq. (36)
        if abs(Dsum) < 1e-300:
            raise RuntimeError(f"D = sum rho X^2 vanished at {Gamma:.6g}")
        sum_xi = np.sum(rho*s*xi)
        Z0 = np.sum(rho*s*lam)/(1.0 + sum_xi)
        Y0 = -(2.0*np.pi/(D*z*z))*np.sum(rho*s*eta)/(1.0 + sum_xi)
        sX = float(np.sum(rho*X))
        ssX = float(np.sum(rho*s*X))
        if abs(Z0) < 1e-300:
            raise RuntimeError(f"Z0 vanished at Gamma = {Gamma:.6g}")
        return (Gamma**2/Dsum + np.pi*self.K
                + (z*Gamma/(Dsum*Z0))
                * (Y0*sX + (1.0 + Y0*(Gamma + np.pi*self.zeta2/(2.0*D)
                                      + z/2.0))*ssX))

    def _closure38a(self, Gamma):
        """Ginoza Eq. (38a), per component. Returns the vector.

        Kept because 38b follows from it by multiplying rho_j sigma_j
        lambda_j and summing, so the two must share a root. Any transcription
        error in one is very unlikely to appear identically in the other,
        which makes agreement between them a real check rather than a
        restatement.
        """
        s, rho, z, D = self.sigma, self.rho, self.z, self.Delta
        X, Delta_N, xi, eta, lam = self._solve_X(Gamma)
        Dsum = float(np.sum(rho*X**2))
        p0 = _phi0(z*s)
        p1 = _psi1(z*s)
        sX = float(np.sum(rho*X))
        ssX = float(np.sum(rho*s*X))
        bracket = (s**3*p1*sX
                   + s**3*p1*(Gamma + np.pi*self.zeta2/(2.0*D) + z/2.0)*ssX
                   - 0.25*s**2*p0*ssX)
        return (Gamma**2/Dsum + np.pi*self.K
                + (z*Gamma*np.exp(z*s/2.0)/(Dsum*self.d))
                * ((1.0 + p0*s*Gamma)*X - (2.0*np.pi/D)*bracket))

    def solveGamma(self, bracket=None, tol=1e-12):
        """Find Gamma from Eq. (38b) by bisection on a bracketed sign change.

        Bisection rather than Newton: the closing condition is a ratio of
        sums that can be steep, and a bracketed method cannot leave the
        interval where the coefficients are finite. The bracket is scanned
        rather than guessed, since the admissible range depends on the
        denominators in `_coefficients`.
        """
        if bracket is None:
            #SCAN BOTH SIGNS. An earlier version searched only Gamma > 0 on
            #the reasoning that an attractive Yukawa gives a positive root;
            #that was wrong. For beta u = -K d_i d_j exp(-zr)/r with K > 0
            #the physical root is NEGATIVE -- scanning a monodisperse hard
            #sphere at phi = 0.3, z = 2, K = 1 puts sign changes between
            #-2 and -1 and again between -0.1 and 0. Searching upward from
            #zero finds neither and reports "no physical Gamma", which looks
            #like a domain failure and is not.
            #
            #Two roots is the usual MSA situation, so the one nearest zero is
            #taken: it is the branch continuously connected to the
            #no-coupling limit Gamma -> 0 as K -> 0, which is the physical
            #one. The other is the spurious branch.
            grid = np.concatenate([
                -np.logspace(np.log10(50.0), np.log10(1e-6), 400),
                np.logspace(np.log10(1e-6), np.log10(50.0), 400)])
            vals = []
            for gval in grid:
                try:
                    vals.append((gval, self._closure38b(gval)))
                except RuntimeError:
                    vals.append((gval, None))
            brackets = []
            for (g1, f1), (g2, f2) in zip(vals, vals[1:]):
                if f1 is None or f2 is None:
                    continue
                if np.sign(f1) != np.sign(f2):
                    brackets.append((g1, g2))
            if not brackets:
                raise RuntimeError(
                    "no sign change in Eq. (38b) over |Gamma| <= 50: no "
                    "physical solution at this state point, which for MSA "
                    "usually means the coupling is too strong for the "
                    "density")
            #Nearest to zero, by the midpoint of the bracket.
            bracket = min(brackets, key=lambda b: abs(0.5*(b[0]+b[1])))
        a, b = bracket
        fa = self._closure38b(a)
        for _ in range(400):
            m = 0.5*(a + b)
            fm = self._closure38b(m)
            if abs(b - a) < tol*max(1.0, abs(m)):
                break
            if np.sign(fm) == np.sign(fa):
                a, fa = m, fm
            else:
                b = m
        self._Gamma = 0.5*(a + b)
        return self._Gamma

    def selfTest(self, Gamma=None):
        """Check Eq. (38a) and Eq. (38b) agree, as they must.

        38b is 38a multiplied by rho_j sigma_j lambda_j and summed, so at the
        root of one the weighted sum of the other must vanish. This is the
        transcription check described in the module docstring.
        """
        if Gamma is None:
            Gamma = self._Gamma if self._Gamma is not None else self.solveGamma()
        X, Delta_N, xi, eta, lam = self._solve_X(Gamma)
        v38a = np.atleast_1d(self._closure38a(Gamma))
        weighted = float(np.sum(self.rho*self.sigma*lam*v38a))
        #SCALE AGAINST THE TERMS, not against the residual. An earlier
        #version divided by a sum built from the same near-zero values, so
        #the comparison was 1e-12 against 1e-12 and always failed. The right
        #reference is the size of the individual contributions BEFORE they
        #cancel -- here pi*K, the term of Eq. (38) that does not vanish at
        #the root.
        scale = max(abs(np.pi*self.K), 1.0)
        return {"Gamma": float(Gamma),
                "eq38b": float(self._closure38b(Gamma)),
                "eq38a_weightedSum": weighted,
                "scale": scale,
                "agree": bool(abs(weighted) <= 1e-8*scale)}
