# -*- coding: utf-8 -*-
r"""
Exact analytic MSA scattering intensity for polydisperse charged hard spheres.

Implements Appendix A of

    D. Gazzillo, A. Giacometti and F. Carsughi, "A corresponding states
    approach to Small Angle Scattering from polydisperse ionic colloidal
    fluids", arXiv:cond-mat/9909153,

which is itself the corrected form of

    D. Gazzillo, A. Giacometti and F. Carsughi, J. Chem. Phys. 107, 10141
    (1997).

The 1999 appendix explicitly fixes two misprints in the 1997 paper: a factor
pi omitted from P_z, and an error in the determinant. THIS MODULE FOLLOWS THE
1999 FORM. If you compare against the 1997 paper directly, expect it to
disagree.

VALIDATED
---------
Neutral limits, where exact answers are independently available:

  * monodisperse, z = 0, against the analytic Wertheim-Thiele PY structure
    factor: 3e-14 at eta = 0.1, 0.2, 0.3, 0.4;
  * polydisperse (3 classes), z = 0, against mixscatter's analytic Vrij
    mixture PY: 1e-13 at eta = 0.1, 0.2, 0.3.

Both at machine precision, so the hard-sphere skeleton of the formulas --
the determinant, the bracket sums, alpha and beta -- is right.

Charged case, reproducing the paper's own figures (eta = 0.3,
<sigma>2 = 100 A, z = -20, L_B = 7.189 A):

  Fig. 1, increasing polydispersity s = 0, 0.1, 0.2, 0.3
    S_max      1.7350 -> 1.6414 -> 1.4731 -> 1.3546   (lowered)
    peak q<s>2 5.841  -> 5.717  -> 5.405  -> 5.030    (shifted down)
    S(q->0)    0.0187 -> 0.0232 -> 0.0347 -> 0.0484   (increased)
  matching all three effects they describe, and reproducing their component
  counts exactly (79, 112, 149 macroion components for s = 0.1, 0.2, 0.3).

  Fig. 2, charge on versus off at s = 0.3
    charged  S_max 1.3546 at 5.030, S(0) 0.0484
    neutral  S_max 1.1942 at 5.717, S(0) 0.1340
  i.e. charges raise and left-shift the peak and suppress S(0), as stated.

Electroneutrality holds to ~1e-20 and 2*Gamma*<sigma>2 ~ 4.3-4.5, both sane.

WHY THIS EXISTS
---------------
It is an INDEPENDENT EXACT REFERENCE for the one path in this package that has
never had one: the charge-coupled polydisperse route. Every defect found late
in this project was exposed by an external reference and invisible to internal
consistency checks (see docs/NEXT_SESSION.md), and the charged route is the
last major piece without such a check.

It is exact within the mean spherical approximation for the PRIMITIVE MODEL:
charged hard spheres in a dielectric continuum,

    beta u_ab(r) = +inf                      r < sigma_ab = (sigma_a+sigma_b)/2
                 = z_a z_b L_B / r           r > sigma_ab

with L_B the Bjerrum length and the electroneutrality condition
sum_v x_v z_v = 0. Note this is NOT the same model as
setPolydisperseHardCoreYukawaPotential, which follows D'Aguanno and Klein in
eliminating the microions in favour of an effective screened-Coulomb
macroion-macroion potential. Gazzillo contrasts the two approaches explicitly.
A comparison between them is meaningful but is a comparison of MODELS, not
merely of implementations.

KNOWN TYPOGRAPHICAL ISSUE IN THE SOURCE
---------------------------------------
Appendix A of the 1999 preprint writes the first-order spherical Bessel
function as "j1(x) = (cos x - x cos x)/x^2", which is a typo: the correct
form, given earlier in the same paper (their Eq. 2), is
j1(x) = (sin x - x cos x)/x^2. This module uses the correct one.

CONVENTIONS
-----------
sigma are DIAMETERS. Lengths may be in any consistent unit (the paper uses
angstroms); q is then in the reciprocal of that unit. The returned R(q) is the
scattering intensity per unit volume, i.e. the differential cross section
dSigma/dOmega, in units of (contrast)^2 * length^3 -- multiply by whatever
contrast units you supplied.
"""
import numpy as np


# ----------------------------------------------------------------------
def _j0(x):
    """sin(x)/x, with the x -> 0 limit."""
    x = np.asarray(x, float)
    out = np.ones_like(x)
    nz = np.abs(x) > 1e-8
    out[nz] = np.sin(x[nz])/x[nz]
    return out


def _j1_over_x(x):
    """3*j1(x)/x with j1(x) = (sin x - x cos x)/x^2; the x -> 0 limit is 1.

    NOTE the paper's Appendix A misprints j1; see the module docstring.
    """
    x = np.asarray(x, float)
    out = np.ones_like(x)
    nz = np.abs(x) > 1e-8
    xn = x[nz]
    out[nz] = 3.0*(np.sin(xn) - xn*np.cos(xn))/xn**3
    return out


def solveGamma(sigma, z, rho, bjerrum, tol=1e-13, maxIter=500):
    r"""Screening parameter Gamma from the consistency relation (A3).

        (2 Gamma)^2 = 4 pi L_B sum_v rho_v [(z_v - P_z sigma_v^2/2)
                                            / (1 + Gamma sigma_v)]^2

    with P_z and Omega from (A4)-(A5). Solved by damped fixed-point iteration
    on Gamma, which is monotone and well behaved here; the Debye value
    kappa_D/2 is used as the starting guess and is a rigorous UPPER bound
    (the paper notes 2 Gamma <= kappa_D always).

    Returns (Gamma, P_z). Gamma is exactly zero for a neutral system.
    """
    sigma = np.asarray(sigma, float)
    z = np.asarray(z, float)
    rho = np.asarray(rho, float)
    if bjerrum <= 0.0 or np.allclose(z, 0.0):
        return 0.0, 0.0

    eta = (np.pi/6.0)*np.sum(rho*sigma**3)
    delta = 1.0 - eta

    kappaD = np.sqrt(4.0*np.pi*bjerrum*np.sum(rho*z**2))
    g = 0.5*kappaD                       # upper bound, per the paper
    for _ in range(maxIter):
        denom = 1.0 + g*sigma
        omega = delta + (np.pi/2.0)*np.sum(rho*sigma**3/denom)
        pz = (np.pi/omega)*np.sum(rho*sigma*z/denom)
        rhs = 4.0*np.pi*bjerrum*np.sum(rho*((z - pz*sigma**2/2.0)/denom)**2)
        gNew = 0.5*np.sqrt(max(rhs, 0.0))
        #Damping: the map is contractive here, but the first step from the
        #Debye bound can overshoot for strongly coupled systems.
        gNext = 0.5*(g + gNew)
        if abs(gNext - g) < tol*max(1.0, abs(g)):
            g = gNext
            break
        g = gNext

    denom = 1.0 + g*sigma
    omega = delta + (np.pi/2.0)*np.sum(rho*sigma**3/denom)
    pz = (np.pi/omega)*np.sum(rho*sigma*z/denom)
    return float(g), float(pz)


# ----------------------------------------------------------------------
def scatteringIntensity(q, sigma, z, rho, formFactor, bjerrum,
                        sigmaScatt=None, returnParts=False):
    r"""R(q), the exact MSA intensity per unit volume, Eqs. (A12)-(A16).

    q           : wavevectors
    sigma       : hard-core DIAMETERS of each species
    z           : valencies (electroneutrality: sum rho_v z_v = 0)
    rho         : number densities of each species
    formFactor  : F_v(q), shape (nq, p) -- or None to build spheres of
                  diameter sigmaScatt (default sigma) with unit contrast
    bjerrum     : Bjerrum length L_B in the same length unit as sigma
    sigmaScatt  : scattering-core diameters, if different from sigma

    Returns R(q); with returnParts=True also the (Gamma, P_z) pair, which is
    worth inspecting because Gamma is where a bad parameter set shows up
    first.
    """
    q = np.atleast_1d(np.asarray(q, float))
    sigma = np.asarray(sigma, float)
    z = np.asarray(z, float)
    rho = np.asarray(rho, float)
    p = sigma.size

    eta = (np.pi/6.0)*np.sum(rho*sigma**3)
    delta = 1.0 - eta
    if delta <= 0.0:
        raise ValueError(f"packing fraction {eta:.4g} >= 1")

    gam, pz = solveGamma(sigma, z, rho, bjerrum)

    #A_v, Eq. (A6). For a neutral system Gamma = 0 and the numerator vanishes
    #identically, so A = 0 is the correct limit rather than 0/0.
    if gam > 0.0:
        A = (bjerrum/gam)*(z - pz*sigma**2/2.0)/(1.0 + gam*sigma)
    else:
        A = np.zeros(p)

    X = 0.5*q[:, None]*sigma[None, :]           # (nq, p)
    psi = _j0(X)                                # j0(X_v)
    phi = _j1_over_x(X)                         # 3 j1(X_v)/X_v

    if formFactor is None:
        ss = sigma if sigmaScatt is None else np.asarray(sigmaScatt, float)
        Xs = 0.5*q[:, None]*ss[None, :]
        V = (np.pi/6.0)*ss**3
        F = V[None, :]*_j1_over_x(Xs)
    else:
        F = np.asarray(formFactor, float)
        if F.shape != (q.size, p):
            raise ValueError(f"formFactor must have shape {(q.size, p)}")

    #Eqs. (A7)-(A11).
    alpha = np.pi*sigma[None, :]**3*phi/(6.0*delta)
    beta = np.pi*sigma[None, :]**2*psi/(2.0*delta)
    with np.errstate(divide="ignore", invalid="ignore"):
        pref = 2.0*np.pi*1j/q[:, None]
        g0 = pref*(gam**2/bjerrum)*A[None, :]*sigma[None, :]*psi if bjerrum > 0 \
            else np.zeros_like(X, dtype=complex)
        g1 = pref*(gam/bjerrum)*A[None, :]*np.exp(-1j*X) if bjerrum > 0 \
            else np.zeros_like(X, dtype=complex)
    gamma0 = g0
    gamma = g0 + g1

    #Bracket notations, Eqs. (A1)-(A2).
    def br0(Y):
        """{Y}_0 = sum_v rho_v Y_v; Y may be (p,) or (nq, p)."""
        return np.sum(rho[None, :]*np.atleast_2d(Y), axis=1)

    e = np.exp(1j*X)

    def br(Y):
        """{Y} = sum_v rho_v Y_v exp(i X_v)."""
        return np.sum(rho[None, :]*np.atleast_2d(Y)*e, axis=1)

    xi2 = (np.pi/6.0)*np.sum(rho*sigma**2)

    #The 4x4 determinant (A16). Its first row is species-resolved, so the
    #quantities actually needed are the cofactors t_m of the (1,m) elements,
    #which come from the LOWER 3x3 block and do not depend on the species
    #index.
    qg0 = q[:, None]*gamma0
    qg = q[:, None]*gamma

    row2 = np.stack([br(F),
                     1.0 + br(alpha),
                     br(beta) - 3.0*xi2/delta + 1j*q/2.0,
                     br(qg0) - 2j*gam*pz*delta], axis=1)
    row3 = np.stack([br(sigma[None, :]*F),
                     br(sigma[None, :]*alpha),
                     1.0 + br(sigma[None, :]*beta),
                     br(sigma[None, :]*qg0)], axis=1)
    row4 = np.stack([br(A[None, :]*F),
                     br(A[None, :]*alpha),
                     br(A[None, :]*beta),
                     q + br(A[None, :]*qg0) + 2j*gam], axis=1)

    M = np.stack([row2, row3, row4], axis=1)         # (nq, 3, 4)
    t = np.empty((q.size, 4), dtype=complex)
    for m in range(4):
        minor = np.delete(M, m, axis=2)              # (nq, 3, 3)
        t[:, m] = ((-1)**m)*np.linalg.det(minor)

    c1 = t[:, 1]/t[:, 0]
    c2 = t[:, 2]/t[:, 0]
    c3 = t[:, 3]/t[:, 0]

    #Eqs. (A13)-(A14).
    R1 = (br0(F**2) + br0(alpha**2)*np.abs(c1)**2 + br0(beta**2)*np.abs(c2)**2
          + 2.0*np.real(br0(F*alpha)*c1 + br0(F*beta)*c2
                        + br0(alpha*beta)*c1*np.conj(c2)))
    R2 = (br0(np.abs(qg)**2)*np.abs(c3)**2
          + 2.0*np.real(br0(qg*F)*c3 + br0(qg*alpha)*c3*np.conj(c1)
                        + br0(qg*beta)*c3*np.conj(c2)))
    R = np.real(R1 + R2)
    return (R, (gam, pz)) if returnParts else R


def measurableStructureFactor(q, sigma, z, rho, formFactor, bjerrum,
                              sigmaScatt=None):
    r"""S_M(q) = R(q) / (rho <F^2(q)>), their Eq. (6).

    This is the quantity plotted in the paper's figures, and the one to
    compare against `GenericPolydisperseSAS.S_number` and friends.
    """
    q = np.atleast_1d(np.asarray(q, float))
    sigma = np.asarray(sigma, float)
    rho = np.asarray(rho, float)
    rhoTot = float(np.sum(rho))
    x = rho/rhoTot

    if formFactor is None:
        ss = sigma if sigmaScatt is None else np.asarray(sigmaScatt, float)
        Xs = 0.5*q[:, None]*ss[None, :]
        V = (np.pi/6.0)*ss**3
        F = V[None, :]*_j1_over_x(Xs)
    else:
        F = np.asarray(formFactor, float)

    R = scatteringIntensity(q, sigma, z, rho, F, bjerrum, sigmaScatt)
    F2 = np.sum(x[None, :]*F**2, axis=1)
    return R/(rhoTot*F2)


# ----------------------------------------------------------------------
def schulzClasses(meanSigma, s, dsigmaOverMean=0.02, nSigmaCut=None):
    """Discretised Schulz distribution, as the paper does it.

    They use a grid dsigma/<sigma> = 0.02 truncated at 1.56, 2.22 and 2.96
    times <sigma> for s = 0.1, 0.2 and 0.3 respectively, giving 79, 112 and
    149 macroion components. Those truncations are reproduced here when
    nSigmaCut is not given, and the component counts have been verified to
    match.

    Returns (sigma, weights) with weights normalised to one.
    """
    from scipy.special import gammaln
    if s <= 0:
        return np.array([meanSigma]), np.array([1.0])
    cuts = {0.1: 1.56, 0.2: 2.22, 0.3: 2.96}
    hi = (nSigmaCut if nSigmaCut is not None
          else cuts.get(round(s, 3), 1.0 + 6.0*s))*meanSigma
    step = dsigmaOverMean*meanSigma
    sig = np.arange(step, hi + 0.5*step, step)
    a = 1.0/s**2
    b = a/meanSigma
    logf = a*np.log(b) - gammaln(a) + (a - 1.0)*np.log(sig) - b*sig
    w = np.exp(logf)
    return sig, w/w.sum()


def primitiveModelSystem(meanSigma2, zMean2, s, eta,
                         sigma1=5.0, z1=1.0, bjerrum=7.189,
                         chargeExponent=2.0):
    """Build the paper's primitive-model system: microions plus Schulz macroions.

    Charge is coupled to size as z(sigma) = z_<sigma> (sigma/<sigma>)^n with
    n = chargeExponent; the paper uses n = 2 (constant surface charge density,
    their Eq. 10). Microion fraction follows from electroneutrality, their
    Eq. (15).

    Defaults are the paper's own system: sigma1 = 5 A solvated counterions,
    z1 = +1, L_B = 7.189 A (water at 298 K).

    Returns (sigma, z, rho) ready for scatteringIntensity.

    NOTE mixture_validation_tab imports this; without it the charged path of
    that tab fails with AttributeError.
    """
    sig2, f2 = schulzClasses(meanSigma2, s)
    z2 = zMean2*(sig2/meanSigma2)**chargeExponent
    zBar2 = float(np.sum(f2*z2))
    x1 = 1.0/(1.0 - z1/zBar2)
    x2tot = 1.0 - x1

    sigma = np.concatenate([[sigma1], sig2])
    z = np.concatenate([[z1], z2])
    x = np.concatenate([[x1], x2tot*f2])

    rhoTot = eta/((np.pi/6.0)*float(np.sum(x*sigma**3)))
    return sigma, z, rhoTot*x
