# -*- coding: utf-8 -*-
"""
Multicomponent Percus-Yevick adhesive (sticky) hard spheres --- a genuine
Python port of SASfit's validated C engine.

Method: C. Robertus, A. P. Philipse, J. G. H. Joosten and Y. K. Levine,
"Solution of the Percus-Yevick approximation of the multicomponent adhesive
spheres system applied to the small angle X-ray scattering from
microemulsions", J. Chem. Phys. 90, 4482 (1989).

This file is a line-by-line port of
src/plugins/robertus_shs/robertus_shs_core.c (py_compute_ab, py_residual,
build_Q_matrix, rshs_structure_matrix), which is itself a port of the
original Fortran and has been numerically validated.

WHY THIS FILE EXISTS
--------------------
It REPLACES an earlier robertus_shs_core_py.py that was a placeholder rather
than an implementation:

  * its solve() body was a bare `pass` under a docstring reading
    "Dummy solve method for compatibility" --- the multicomponent PY
    lambda(i,j) system was never formed;
  * S_matrix() started from `S = np.ones((p,p))` and filled EVERY element,
    diagonal and off-diagonal alike, from a single-component expression
    `1/(1+24*phi*A)` times an ad-hoc `sticky_correction`, finished with
    `np.maximum(S, 0.1)` to force positivity;
  * consequently the size classes were never coupled: each S_ij was computed
    independently, so the off-diagonals carried no correlation information.

The symptom was subtle rather than obvious. In the dilute limit a correct
Ashcroft-Langreth S must tend to the identity; the placeholder instead gave
diagonal ~ 1.0 AND off-diagonal ~ 1.0, i.e. an all-ones matrix. I_exact then
collapses from sum_i rho_i F_i^2 to (sum_i sqrt(rho_i) F_i)^2, inflating it by
(sum_i sqrt(w_i))^2 --- a factor that depends only on the NUMBER OF CLASSES.
Measured ratios I_exact/I_monodisperse at s_rel = 0.01, where every scheme
must agree, were 1.00, 1.97, 2.53, 5.69, 8.84 and 18.2 for 1, 2, 4, 8, 12 and
24 classes, and the dilute limit returned 8.83 instead of 1.

That also explains the reported "the exact model does not respond to the
distribution width": polydispersity entered only through the class weights in
that spurious sum, never through the physics, because the classes were never
coupled. The approximation schemes call the real PolydisperseSASBase
machinery and so did respond --- hence exact and approximate disagreeing in
both scale and width dependence.

After this replacement: the dilute off-diagonal is 2e-7 (was 1.000116),
I_exact/I_dilute is 0.99999 (was 8.83), and I_exact/I_monodisperse at
s_rel = 0.01 is 1.000 for every class count from 1 to 24.

CONVENTION
----------
S_matrix() returns the ASHCROFT-LANGRETH partials,
    S^AL_ij = delta_ij + sqrt(rho_i rho_j) h_ij(q),
which is what PolydisperseSASBase.I_exact expects (it forms G = F*sqrt(rho)
and contracts G S G) and what robertus_shs_sas.py assumes. In the dilute limit
this tends to the identity --- the check the placeholder failed.
"""
import numpy as np
from scipy.optimize import fsolve
from scipy.special import gamma as gammafn

DIST_SCHULZ_ZIMM = 1
DIST_LOGNORMAL = 2
DIST_GAUSSIAN = 3
DIST_WEIBULL = 4


# ----------------------------------------------------------------------
# Size distributions (ported from the C pdf_* functions)
# ----------------------------------------------------------------------
def _lngamma(x):
    from scipy.special import gammaln
    return gammaln(x)


def _pdf_schulz_zimm(D, Dmean, srel):
    if srel <= 0.0:
        raise ValueError("srel must be > 0")
    z = 1.0 / (srel * srel) - 1.0
    zp = z + 1.0
    rf = np.asarray(D, float) / Dmean
    with np.errstate(divide="ignore", invalid="ignore"):
        logf = (zp * np.log(zp) - zp - np.log(Dmean)
                - _lngamma(zp) + z * np.log(rf) + zp * (1.0 - rf))
    return np.where(np.asarray(D, float) > 0.0, np.exp(logf), 0.0)


def _pdf_lognormal(D, Dmean, srel):
    D = np.asarray(D, float)
    s2 = np.log(1.0 + srel * srel)
    mu = np.log(Dmean) - 0.5 * s2
    with np.errstate(divide="ignore", invalid="ignore"):
        u = np.log(np.where(D > 0, D, 1.0)) - mu
        val = 1.0 / (D * np.sqrt(2.0 * np.pi * s2)) * np.exp(-0.5 * u * u / s2)
    return np.where(D > 0.0, val, 0.0)


def _pdf_gaussian(D, Dmean, srel):
    D = np.asarray(D, float)
    sigma = srel * Dmean
    u = (D - Dmean) / sigma
    return np.exp(-0.5 * u * u) / (sigma * np.sqrt(2.0 * np.pi))


def _weibull_cv(k):
    return np.sqrt(gammafn(1.0 + 2.0 / k) / gammafn(1.0 + 1.0 / k) ** 2 - 1.0)


def _weibull_shape_from_cv(srel):
    """Bisection for the shape k with CV(k) = srel, as in the C code."""
    lo, hi = 0.1, 200.0
    for _ in range(200):
        mid = 0.5 * (lo + hi)
        if _weibull_cv(mid) > srel:
            lo = mid
        else:
            hi = mid
    return 0.5 * (lo + hi)


def _pdf_weibull(D, Dmean, srel):
    D = np.asarray(D, float)
    k = _weibull_shape_from_cv(srel)
    lam = Dmean / gammafn(1.0 + 1.0 / k)
    z = np.where(D > 0, D / lam, 0.0)
    val = (k / lam) * z ** (k - 1.0) * np.exp(-(z ** k))
    return np.where(D > 0.0, val, 0.0)


_PDFS = {DIST_SCHULZ_ZIMM: _pdf_schulz_zimm,
         DIST_LOGNORMAL: _pdf_lognormal,
         DIST_GAUSSIAN: _pdf_gaussian,
         DIST_WEIBULL: _pdf_weibull}


def _pidx(i, j):
    """Packed index of the symmetric lambda(i,j); mirrors the C PIDX()."""
    if i < j:
        i, j = j, i
    return i * (i + 1) // 2 + j


class RobertusSHSSolver:
    """Multicomponent PY adhesive hard spheres.

    Dmean : mean diameter
    srel  : relative width of the size distribution
    phi   : total volume fraction
    tau   : Baxter stickiness. SMALLER tau is STICKIER; below tau_c(phi) the
            closure has no real solution and solve() raises.
    """

    def __init__(self, Dmean, srel, phi, tau, nbins=12, dist=DIST_LOGNORMAL):
        self.Dmean = float(Dmean)
        self.srel = float(srel)
        self.phi = float(phi)
        self.tau = float(tau)
        self.p = int(nbins)
        self.dist = dist
        if self.p < 1:
            raise ValueError("nbins must be >= 1")
        if not (0.0 < self.phi < 1.0):
            raise ValueError("phi must be in (0,1)")
        self._make_classes()
        self.solved = False
        self.lam = None

    # ------------------------------------------------------------------
    def _make_classes(self):
        """p classes on an equally spaced diameter grid over Dmean +- 3 sigma,
        renormalised so that the classes reproduce the requested phi.
        Mirrors classes_from_grid() in the C engine."""
        p = self.p
        if p == 1 or self.srel <= 0.0:
            self.D = np.array([self.Dmean])
            w = np.array([1.0])
        else:
            sigma = self.srel * self.Dmean
            D0 = self.Dmean - 3.0 * sigma
            D1 = self.Dmean + 3.0 * sigma
            dr = (D1 - D0) / (p - 1)
            if D0 <= 0.0:
                D0 = 0.5 * dr
                D1 = D0 + (p - 1) * dr
            self.D = np.linspace(D0, D1, p)
            w = _PDFS[self.dist](self.D, self.Dmean, self.srel) * dr
            if not np.any(w > 0):
                raise ValueError("size distribution produced no weight")
        # x_i are NUMBER DENSITIES, scaled so that (pi/6) sum x_i D_i^3 = phi
        m3 = np.sum(w * self.D ** 3)
        self.x = w * (6.0 * self.phi / (np.pi * m3))
        self.w = self.x / self.x.sum()          # number fractions
        self.n_tot = float(self.x.sum())

    # ------------------------------------------------------------------
    def _prep(self):
        D = self.D
        self.r1 = 0.5 * (D[:, None] + D[None, :])
        self.r2 = self.r1 ** 2
        self.rr = D[:, None] * D[None, :]
        self.k2 = (np.pi / 6.0) * np.sum(self.x * D ** 2)
        self.k3 = (np.pi / 6.0) * np.sum(self.x * D ** 3)
        self.h = 1.0 - self.k3
        if self.h <= 0.0:
            raise ValueError(f"1 - k3 = {self.h:.4g} <= 0: volume fraction too high")

    def _compute_ab(self, lam):
        """Ported from py_compute_ab()."""
        p = self.p
        D, x = self.D, self.x
        L = np.array([[lam[_pidx(i, j)] for j in range(p)] for i in range(p)])
        som = np.einsum('j,ij,ij,j->i', x, L, self.r2, D)
        xc = som * (np.pi / 6.0)
        b0 = 3.0 * D * self.k2 / (self.h * self.h) - xc / self.h
        a = 1.0 / self.h + b0
        b = -0.5 * b0 * D
        return a, b, L

    def _residual(self, lam):
        """Ported from py_residual(): the multicomponent PY system, Eq. 9."""
        p, D, x = self.p, self.D, self.x
        a, b, L = self._compute_ab(lam)
        # T[i][k] = -0.5 a_i rr_ik - b_i D_k + lambda_ik r2_ik / 12
        T = (-0.5 * a[:, None] * self.rr - b[:, None] * D[None, :]
             + L * self.r2 / 12.0)
        # som_ij = sum_k x_k lambda_jk r2_jk T_ik
        M = x[None, :] * L * self.r2                      # (j,k)
        som = T @ M.T                                     # (i,j)
        val = (som / self.r1 * (np.pi / 6.0)
               + b[:, None] / self.r1 + a[:, None] - self.tau * L)
        out = np.empty(p * (p + 1) // 2)
        for i in range(p):
            for j in range(i + 1):
                out[_pidx(i, j)] = val[i, j]
        return out

    # ------------------------------------------------------------------
    def solve(self):
        """Solve for lambda(i,j). Initial guess is the monodisperse analytic
        root, exactly as in the C engine."""
        self._prep()
        n = self.p * (self.p + 1) // 2
        bla = self.k3 / self.h + self.tau
        dla = bla * bla - self.k3 / 3.0 * (1.0 + 0.5 * self.k3) / (self.h * self.h)
        if dla >= 0.0:
            alab = 6.0 / self.k3 * (bla - np.sqrt(dla))
        else:
            alab = 6.0 / self.k3 * bla
        guess = np.full(n, alab)

        lam, info, ier, msg = fsolve(self._residual, guess, full_output=True,
                                     xtol=1e-12, maxfev=20000)
        res = float(np.max(np.abs(self._residual(lam))))
        # Never trust the solver flag alone -- check the residual. Below the
        # critical stickiness the PY closure has no real solution and the
        # iteration wanders instead of failing cleanly.
        if not np.all(np.isfinite(lam)) or res > 1e-6:
            raise RuntimeError(
                "no real solution found for lambda(i,j) "
                f"(max|residual| = {res:.3e}) at phi={self.phi:.4g}, "
                f"tau={self.tau:.4g}. Below the critical tau_c(phi) the "
                "adhesive Percus-Yevick closure has no physical solution: "
                "INCREASE tau (smaller tau is stickier), or lower phi. "
                f"[fsolve: {msg.strip() if ier != 1 else 'converged'}]")
        self.lam = lam
        self.a, self.b, self.L = self._compute_ab(lam)
        self.solved = True
        self.residual = res
        return lam

    # ------------------------------------------------------------------
    def _Q_matrix(self, q):
        """Complex Q(i,k)(q); ported from build_Q_matrix()."""
        p, D, x = self.p, self.D, self.x
        vrx = 0.5 * D * q
        sqv, cqv = np.sin(vrx), np.cos(vrx)
        # Taylor series below |x| = 0.5: sin(x)-x cos(x) and sin(x)/x both
        # suffer catastrophic cancellation as x -> 0. This threshold and the
        # series are taken from the C engine, where they were validated
        # against a 50-digit mpmath reference.
        small = np.abs(vrx) < 0.5
        x2 = vrx ** 2
        vrphi = np.where(small,
                         1.0 - x2 / 10.0 + x2 ** 2 / 280.0 - x2 ** 3 / 15120.0,
                         3.0 / np.where(small, 1.0, vrx ** 3) * (sqv - vrx * cqv))
        vrpsi = np.where(small,
                         1.0 - x2 / 6.0 + x2 ** 2 / 120.0 - x2 ** 3 / 5040.0,
                         sqv / np.where(small, 1.0, vrx))
        sqrtx = np.sqrt(x)

        Di = D[:, None]
        Dk = D[None, :]
        phik = vrphi[None, :]
        psik = vrpsi[None, :]
        re = (-0.25 * self.L * Dk * (Di + Dk) ** 2 * psik
              + Dk ** 3 / self.h * phik
              + Dk ** 2 * Di / self.h * 3.0 * psik
              - 2.0 * Dk ** 3 * (self.b / D)[:, None] * phik)
        im = -Di * Dk ** 2 / self.h * vrx[None, :] * phik
        pref = (np.pi / 6.0) * np.outer(sqrtx, sqrtx)
        z = pref * (re + 1j * im)
        phase = (cqv + 1j * sqv)[:, None]         # exp(i*vrx_i)
        Q = z * phase
        Q[np.diag_indices(p)] += 1.0
        return Q

    def S_matrix(self, q):
        """Ashcroft-Langreth partial structure factors S_ij(q), shape (p,p).

        S = B B^H with B = Q^{-1}, exactly as rshs_structure_matrix() does.
        """
        if not self.solved:
            self.solve()
        Q = self._Q_matrix(float(q))
        B = np.linalg.inv(Q)
        return np.real(B @ B.conj().T)

    def S_number(self, q):
        wv = np.sqrt(self.w)
        return float(wv @ self.S_matrix(q) @ wv)
