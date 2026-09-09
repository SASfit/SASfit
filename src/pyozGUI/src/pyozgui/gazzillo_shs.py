# -*- coding: utf-8 -*-
r"""
Exact analytic MSA scattering functions for polydisperse STICKY hard spheres.

Implements Gazzillo & Giacometti, "Structure factors for the simplest solvable
model of polydisperse colloidal fluids with surface adhesion",
arXiv:cond-mat/0011330.

The model is hard core plus Yukawa attraction in the Baxter sticky limit
(amplitude -> infinity, range -> 0, product fixed), solved in the MSA, with
FACTORISABLE coupling parameters K_ij = Y_i Y_j (their Eq. 29). That
factorisation is what makes qhat_ij(k) 3-dyadic instead of Tutschka & Kahl's
5-dyadic, and it is why the whole p x p matrix inversion collapses to a 4 x 4
determinant regardless of the number of components.

WHY THIS EXISTS
---------------
Two reasons, both about validation.

1. The polydisperse STICKY hard-sphere path in this package has no exact
   reference. It is also the section 5.3 row that turned out to be computed
   on a two-grid-point well and had to be recomputed at 1600 points per
   diameter; an analytic check would have caught that immediately.
2. It is the same family as gazzillo_msa.py (polydisperse CHARGED hard
   spheres) and shares its structure: Vrij summation over a dyadic Baxter
   factor matrix, giving a determinant of order n+1 with n = 3.

Together with mixscatter (polydisperse HS, Vrij PY) and gazzillo_msa
(polydisperse CHS, MSA) this completes analytic coverage of the three
classical solvable polydisperse models.

CONVENTIONS AND A WARNING ABOUT tau
-----------------------------------
The stickiness here is the parameter Y_i, which has DIMENSIONS OF LENGTH, via

    K_ij = Y_i Y_j,     Y_i = gamma0 * sigma_i^alpha / <sigma>^(alpha-1)

with gamma0^2 = eps0/(kB T) = 1/T*. This is NOT Baxter's tau, and the two are
not trivially related: Baxter's PY solution for mixtures needs p(p+1)/2
coupled quadratic equations for lambda_ij and is generally NOT dyadic, which
is precisely why this MSA model exists. Do not feed a tau from
setStickyHardSpherePotential into this module and expect agreement -- they
are different models (MSA versus PY) with different parameters. What CAN be
compared is qualitative behaviour, and the neutral limit gamma0 = 0, where
both reduce to hard spheres.

The paper studies two versions:
    Model I  (alpha = 0): size polydisperse, single stickiness for all sizes
    Model II (alpha = 1): Y_i = gamma0 sigma_i, stickiness proportional to
                          diameter. Only here does qhat become 2-dyadic.

VALIDATED
---------
* gamma0 = 0, MONODISPERSE, against analytic Wertheim-Thiele PY:
  4e-14 at eta = 0.1, 0.2, 0.3, 0.4.
* gamma0 = 0, POLYDISPERSE (3 classes), against mixscatter's analytic Vrij
  mixture PY: 1e-13 at eta = 0.1, 0.2, 0.3.
* MONODISPERSE STICKY against the paper's closed form, their Eq. (48):
  agreement 3e-08 at k = 1e-4, across eta = 0.1-0.3 and gamma0 = 0-0.7.
* CRITICAL POINT: at eta_c = 0.36603 and gamma0c^2 = 0.62201, S(0) runs
  1.49 -> 136 -> 3.3e30 as gamma0 -> gamma0c, so the spinodal divergence sits
  exactly where the paper places it.
* GENERALISED BOYLE TEMPERATURE, their Eq. (53), reproduced to the printed
  digits: Model I 2.787 / 1.677 and Model II 2.986 / 2.897 at s = 0.1 / 0.3
  (paper: 2.79 / 1.68 and 2.99 / 2.90), both tending to 3 as s -> 0.
* Qualitative polydisperse behaviour as described in the paper: at eta = 0.3,
  adhesion lowers the first peak (1.5152 -> 1.3726 at gamma0 = 0.5) and size
  polydispersity lowers it further (-> 1.0614 at s = 0.3).

NUMERICAL NOTE ON S(0). Evaluate the k -> 0 limit at k ~ 1e-4, NOT smaller.
The relative error against Eq. (48) is 3e-08 at k = 1e-4 but rises to 3.9e-06,
8.1e-04 and 2.9e-02 at k = 1e-5, 1e-6 and 1e-7: the determinant terms cancel
and round-off takes over. That non-monotonic behaviour is round-off, not a
formula error -- the giveaway is that the error at fixed k is IDENTICAL for
every gamma0 and scales only with eta.
"""
import numpy as np


def _j0(x):
    x = np.asarray(x, float)
    out = np.ones_like(x)
    nz = np.abs(x) > 1e-8
    out[nz] = np.sin(x[nz])/x[nz]
    return out


def _j1_over_x(x):
    """3 j1(x)/x, limit 1 at x -> 0."""
    x = np.asarray(x, float)
    out = np.ones_like(x)
    nz = np.abs(x) > 1e-8
    xn = x[nz]
    out[nz] = 3.0*(np.sin(xn) - xn*np.cos(xn))/xn**3
    return out


def stickinessParameters(sigma, gamma0, meanSigma=None, alpha=1.0):
    """Y_i = gamma0 sigma_i^alpha / <sigma>^(alpha-1), their Eq. (44).

    alpha = 0 is Model I (one stickiness for every size), alpha = 1 is
    Model II (stickiness proportional to diameter, the 2-dyadic case).
    """
    sigma = np.asarray(sigma, float)
    ms = float(np.mean(sigma)) if meanSigma is None else float(meanSigma)
    return gamma0*sigma**alpha/ms**(alpha - 1.0)


def scatteringIntensity(k, sigma, rho, Y, formFactor=None, sigmaScatt=None,
                        returnParts=False):
    r"""R(k)/rho, their Eq. (37): the exact MSA intensity per particle.

    k           : wavevectors
    sigma       : hard-core DIAMETERS
    rho         : number densities per species
    Y           : stickiness lengths, K_ij = Y_i Y_j (see stickinessParameters)
    formFactor  : F_v(k), shape (nk, p); None builds homogeneous spheres of
                  diameter sigmaScatt (default sigma) with unit contrast
    """
    k = np.atleast_1d(np.asarray(k, float))
    sigma = np.asarray(sigma, float)
    rho = np.asarray(rho, float)
    Y = np.asarray(Y, float)
    p = sigma.size
    rhoTot = float(np.sum(rho))
    x = rho/rhoTot

    #Eq. (26).
    xi2 = (np.pi/6.0)*np.sum(rho*sigma**2)
    xi3 = (np.pi/6.0)*np.sum(rho*sigma**3)
    delta = 1.0 - xi3
    if delta <= 0.0:
        raise ValueError(f"packing fraction {xi3:.4g} >= 1")
    xi2Y = (np.pi/6.0)*np.sum(rho*sigma*Y)

    X = 0.5*k[:, None]*sigma[None, :]
    j0X = _j0(X)
    phi = _j1_over_x(X)

    if formFactor is None:
        ss = sigma if sigmaScatt is None else np.asarray(sigmaScatt, float)
        Xs = 0.5*k[:, None]*ss[None, :]
        V = (np.pi/6.0)*ss**3
        F = V[None, :]*_j1_over_x(Xs)
    else:
        F = np.asarray(formFactor, float)
        if F.shape != (k.size, p):
            raise ValueError(f"formFactor must have shape {(k.size, p)}")

    #Eq. (31). NOTE their alpha_m uses j1(X)/X while our helper returns
    #3 j1(X)/X, hence the factor 1/3 -- an easy place to be off by three.
    alph = (np.pi/(2.0*delta))*sigma[None, :]**3*phi/3.0
    bet0 = (np.pi/(2.0*delta))*sigma[None, :]**2*j0X
    del0 = -2.0*np.pi*sigma[None, :]*Y[None, :]*j0X

    e = np.exp(1j*X)

    def br(*fs):
        """{f g ...} = sum_m rho_m exp(i X_m) f_m g_m ..., their Eq. (35)."""
        out = rho[None, :]*e
        for f in fs:
            out = out*np.atleast_2d(f)
        return np.sum(out, axis=1)

    def av(*fs):
        """<f g ...> = sum_m x_m f_m g_m ..., a plain compositional average.

        NOTE the distinction from br(): the curly brackets carry exp(i X_m),
        the angular brackets do not. Mixing them up is the single easiest way
        to get plausible but wrong numbers here.
        """
        out = x[None, :]*np.ones((k.size, 1))
        for f in fs:
            out = out*np.atleast_2d(f)
        return np.sum(out, axis=1)

    sig2 = sigma[None, :]
    #Rows 2-4 of the determinant in Eq. (36).
    row2 = np.stack([br(F), 1.0 + br(alph),
                     br(bet0) - 3.0*xi2/delta + 1j*k/2.0,
                     br(del0) + 12.0*xi2Y], axis=1)
    row3 = np.stack([br(sig2, F), br(sig2, alph),
                     1.0 + br(sig2, bet0), br(sig2, del0)], axis=1)
    row4 = np.stack([br(Y[None, :], F), br(Y[None, :], alph),
                     br(Y[None, :], bet0), 1.0 + br(Y[None, :], del0)], axis=1)

    M = np.stack([row2, row3, row4], axis=1)          # (nk, 3, 4)
    T = np.empty((k.size, 4), dtype=complex)
    for m in range(4):
        T[:, m] = ((-1)**m)*np.linalg.det(np.delete(M, m, axis=2))
    DQ = T[:, 0]
    C1, C2, C3 = T[:, 1]/DQ, T[:, 2]/DQ, T[:, 3]/DQ

    #Eq. (37).
    R = (av(F, F)
         + av(alph, alph)*np.abs(C1)**2
         + av(bet0, bet0)*np.abs(C2)**2
         + av(del0, del0)*np.abs(C3)**2
         + 2.0*np.real(av(F, alph)*C1 + av(F, bet0)*C2 + av(F, del0)*C3
                       + av(alph, bet0)*C1*np.conj(C2)
                       + av(alph, del0)*C1*np.conj(C3)
                       + av(bet0, del0)*C2*np.conj(C3)))
    R = np.real(R)
    return (R, (C1, C2, C3)) if returnParts else R


def measurableStructureFactor(k, sigma, rho, Y, formFactor=None,
                              sigmaScatt=None):
    """S_M(k) = R(k)/(rho <F^2>), their Eq. (3)."""
    k = np.atleast_1d(np.asarray(k, float))
    sigma = np.asarray(sigma, float)
    rho = np.asarray(rho, float)
    x = rho/float(np.sum(rho))
    if formFactor is None:
        ss = sigma if sigmaScatt is None else np.asarray(sigmaScatt, float)
        Xs = 0.5*k[:, None]*ss[None, :]
        V = (np.pi/6.0)*ss**3
        F = V[None, :]*_j1_over_x(Xs)
    else:
        F = np.asarray(formFactor, float)
    R = scatteringIntensity(k, sigma, rho, Y, F, sigmaScatt)
    return R/np.sum(x[None, :]*F**2, axis=1)


def numberNumberStructureFactor(k, sigma, rho, Y):
    """Bhatia-Thornton S_NN(k), their Eq. (38): set every form factor to 1."""
    k = np.atleast_1d(np.asarray(k, float))
    F = np.ones((k.size, np.asarray(sigma).size))
    return scatteringIntensity(k, sigma, rho, Y, F)


def monodisperseS0(eta, gamma0):
    """Their Eq. (48), the exact monodisperse sticky S(0).

        S(0) = (1-eta)^4 / [1 + 2eta - 12 gamma0^2 eta (1-eta)]^2

    Its divergence is the spinodal; the critical point they quote is
    eta_c = (sqrt3 - 1)/2 ~ 0.366 and gamma0c^2 = (sqrt3 + 2)/6 ~ 0.622,
    both confirmed numerically against this module.
    """
    return (1.0 - eta)**4/(1.0 + 2.0*eta - 12.0*gamma0**2*eta*(1.0 - eta))**2


def generalisedBoyleTemperature(sSigma, model=2):
    """T*_{B,F}, their Eq. (53), from the Schulz moment ratios M_j = 1 + j s^2.

    Above it the structure factor behaves like hard spheres; below it (but
    above T_c) a "strong-attraction regime" sets in. Model I falls rapidly
    with polydispersity, Model II barely moves, approaching 18/7 ~ 2.571.

    Reproduces the paper's printed values: Model I 2.787 and 1.677, Model II
    2.986 and 2.897, at s = 0.1 and 0.3.
    """
    M = {j: 1.0 + j*sSigma**2 for j in (3, 4, 5)}
    if model == 1:
        return 12.0/(M[4]*(M[5] + 3.0*M[3]))
    return 12.0*M[3]/(M[5] + 3.0*M[3])


def schulzClasses(meanSigma, s, dsigmaOverMean=0.02, cutoff=1e-8):
    """Discretised Schulz distribution, as the paper does it.

    They use a grid dsigma/<sigma> = 0.02 and truncate where
    f(sigma) dsigma ~ 1e-8, giving p = 85 and 175 components for s = 0.1 and
    0.3 respectively.

    Returns (sigma, weights) with weights normalised to one.
    """
    from scipy.special import gammaln
    if s <= 0:
        return np.array([meanSigma]), np.array([1.0])
    step = dsigmaOverMean*meanSigma
    hi = (1.0 + 8.0*s)*meanSigma
    sig = np.arange(step, hi + 0.5*step, step)
    a = 1.0/s**2
    b = a/meanSigma
    logf = a*np.log(b) - gammaln(a) + (a - 1.0)*np.log(sig) - b*sig
    w = np.exp(logf)
    keep = w*step > cutoff
    #Truncate at the UPPER end only. The paper says the distribution is
    #"truncated where f(sigma) dsigma ~ 1e-8, i.e. at sigma_cut/<sigma> =
    #1.68", a single upper cutoff -- the grid still starts at dsigma. Masking
    #both tails instead drops the small-sigma points as well, which for a
    #narrow distribution is most of them: it gave p = 57 against their 85 at
    #s = 0.1, while the upper cutoff itself was right (1.66 versus 1.68).
    if np.any(keep):
        last = int(np.nonzero(keep)[0][-1])
        sig, w = sig[:last + 1], w[:last + 1]
    return sig, w/w.sum()
