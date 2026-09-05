# -*- coding: utf-8 -*-
"""
Moment-matched size classes for polydisperse models.

Given a distribution of diameters with mean <sigma> and relative width s,
return p classes (sigma_i, x_i) whose first 2p-1 moments reproduce the
continuum exactly. That is what lets p = 3 classes stand in for a continuous
distribution where an equally spaced histogram needs 7-9 (D'Aguanno & Klein,
Phys. Rev. A 46, 7652).

WHY THE RULE DIFFERS PER DISTRIBUTION
-------------------------------------
Every distribution here has ANALYTIC moments, so computing <sigma^n> is never
the problem. The problem is the map from moments to quadrature nodes: solving
the Hankel eigenproblem (Golub-Welsch) is classically ill-conditioned even
with exact input. Measured condition numbers for the log-normal Hankel matrix,
built from exact moments:

        s = 0.3      s = 0.4      s = 0.5
  N=6   1.6e7        1.7e7        1.2e8
  N=8   2.1e10       1.9e11       4.7e13
  N=10  7.0e13       1.7e16       5.4e19
  N=12  6.4e17       1.2e21       2.3e27

float64 carries about 1e16, so beyond roughly N=8-10 for a broad weight the
node positions are numerically meaningless -- and Cholesky does NOT raise,
because the matrix stays formally positive definite; it silently returns
nonsense, which is worse than failing.

So a closed-form classical rule is used wherever one exists, bypassing the
Hankel step entirely:

    Schulz / gamma  -> generalised Gauss-Laguerre   exact, stable, float64
    Gaussian        -> Gauss-Hermite                exact, stable, float64
    log-normal      -> Golub-Welsch + mpmath        no classical rule
    Weibull         -> Golub-Welsch + mpmath        no classical rule

All four are moment-exact to ~1e-16 (verified with momentError() below).

A PHYSICAL CAVEAT ON THE GAUSSIAN
---------------------------------
Gauss-Hermite is exact but places its outermost node at u ~ +/-2.86 for p=5,
so sigma = <sigma>(1 + s*u) becomes zero at s ~ 0.35 and negative beyond. For
a FORM factor a stray tiny bin is just a vanishing weight; for a STRUCTURE
factor it is fatal, because a hard core has to be placed at every
sigma_ij = (sigma_i+sigma_j)/2 and a non-positive diameter poisons the whole
pair matrix. Such nodes are therefore dropped and the caller falls back to a
truncated rule, which sacrifices exactness rather than physicality.

NOTE the test is RELATIVE (sigma <= 1e-3*<sigma>), not sigma <= 0: at s = 0.35
the outermost node sits at 1e-4, positive but still a zero-diameter particle.
"""
import numpy as np
from scipy.special import roots_genlaguerre, roots_hermitenorm
from scipy.special import gamma as gammafn


DISTRIBUTIONS = ("Schulz", "Gaussian", "LogNormal", "Weibull")


def _weibull_shape_from_cv(srel):
    """Shape k with coefficient of variation equal to srel (bisection)."""
    def cv(k):
        return np.sqrt(gammafn(1.0 + 2.0/k)/gammafn(1.0 + 1.0/k)**2 - 1.0)
    lo, hi = 0.05, 200.0
    for _ in range(200):
        mid = 0.5*(lo + hi)
        if cv(mid) > srel:
            lo = mid
        else:
            hi = mid
    return 0.5*(lo + hi)


def analyticMoments(distribution, srel, nMoments, meanSigma=1.0):
    """<sigma^n> for n = 0 .. nMoments-1. Closed form for every distribution."""
    n = np.arange(nMoments)
    if distribution == "Schulz":
        t = 1.0/srel**2 - 1.0
        m = np.ones(nMoments)
        for k in range(1, nMoments):
            m[k] = m[k-1]*(t + k)/(t + 1.0)
        return m*meanSigma**n
    if distribution == "LogNormal":
        s2 = np.log(1.0 + srel*srel)
        mu = -0.5*s2                       # so that <sigma> = 1 before scaling
        return np.exp(n*mu + n*n*s2/2.0)*meanSigma**n
    if distribution == "Weibull":
        k = _weibull_shape_from_cv(srel)
        lam = 1.0/gammafn(1.0 + 1.0/k)
        return np.array([lam**j*gammafn(1.0 + j/k) for j in n])*meanSigma**n
    if distribution == "Gaussian":
        # central moments of N(0,1): 0 for odd, (j-1)!! for even
        out = np.zeros(nMoments)
        for j in range(nMoments):
            tot = 0.0
            for i in range(j + 1):
                if (j - i) % 2:
                    continue
                dbl = 1.0
                for q in range(j - i - 1, 0, -2):
                    dbl *= q
                from math import comb
                tot += comb(j, i)*srel**(j - i)*dbl
            out[j] = tot
        return out*meanSigma**n
    raise ValueError("unknown distribution %r" % (distribution,))


def _golubWelsch(moments, p, dps=200):
    """p-point Gaussian rule exact for moments[0..2p-1], in high precision.

    High precision is not optional here: see the condition-number table in the
    module docstring.
    """
    try:
        import mpmath as mp
    except ImportError as exc:
        raise ImportError(
            "log-normal and Weibull need the moment-based rule, which requires "
            "mpmath (pip install mpmath) because the moments-to-nodes map is "
            "ill-conditioned in float64. Use Schulz or Gaussian, which have "
            "closed-form rules, or reduce the number of classes.") from exc
    mp.mp.dps = dps
    m = [mp.mpf(float(v)) for v in moments]
    H = mp.matrix(p, p)
    Hs = mp.matrix(p, p)
    for i in range(p):
        for j in range(p):
            H[i, j] = m[i + j]
            Hs[i, j] = m[i + j + 1]
    R = mp.cholesky(H)
    Rinv = mp.inverse(R)
    J = Rinv*Hs*Rinv.T
    nodes, vecs = mp.eigsy(J)
    sigma = np.array([float(nodes[i]) for i in range(p)])
    w = np.array([float(vecs[0, i]**2) for i in range(p)])
    order = np.argsort(sigma)
    return sigma[order], w[order]/w.sum()


def sizeClasses(distribution, srel, p, meanSigma=1.0, allowTruncated=True):
    """(sigma, x) for p moment-matched classes.

    Returns diameters and normalised number fractions. Raises ValueError if a
    physically valid set cannot be produced.
    """
    p = int(p)
    srel = float(srel)
    if p < 1:
        raise ValueError("need at least one class")
    if srel <= 0.0 or p == 1:
        return np.array([meanSigma]), np.array([1.0])

    if distribution == "Schulz":
        # Closed-form generalised Gauss-Laguerre for the gamma weight.
        t = 1.0/srel**2 - 1.0
        nodes, w = roots_genlaguerre(p, t)
        sigma = nodes*meanSigma/(t + 1.0)
        x = w/w.sum()

    elif distribution == "Gaussian":
        # Closed-form Gauss-Hermite; exact, but can place non-positive nodes.
        u, w = roots_hermitenorm(p)
        sigma = meanSigma*(1.0 + srel*u)
        x = w/w.sum()
        #A relative floor, not just > 0: at s = 0.35 the outermost
        #Gauss-Hermite node lands at sigma = 1e-4, which is positive but
        #physically still a zero-diameter particle. It contributes a hard core
        #at sigma_ij ~ sigma_j/2 for every partner j, distorting the whole
        #pair matrix while carrying almost no weight.
        tooSmall = sigma <= 1e-3*meanSigma
        if np.any(tooSmall):
            if not allowTruncated:
                raise ValueError(
                    f"Gaussian with s = {srel:g} and {p} classes puts a node "
                    f"at sigma = {sigma.min():.3g}, below 1e-3 of the mean. "
                    "A structure factor needs a hard core at every sigma_ij, "
                    "so a vanishing diameter is not usable. Reduce s or the "
                    "number of classes, or allow the truncated rule.")
            # Truncated fallback: keep only usable nodes and renormalise.
            # This sacrifices moment-exactness, which is the right trade --
            # an exact rule over unphysical diameters is worthless here.
            keep = ~tooSmall
            if not np.any(keep):
                raise ValueError("no positive diameters remain")
            sigma, x = sigma[keep], w[keep]/w[keep].sum()

    elif distribution in ("LogNormal", "Weibull"):
        sigma, x = _golubWelsch(analyticMoments(distribution, srel, 2*p + 1,
                                                meanSigma), p)
        if np.any(sigma <= 0.0):
            raise ValueError(
                f"{distribution} with s = {srel:g} and {p} classes produced a "
                f"non-positive node ({sigma.min():.3g}); reduce the number of "
                "classes.")
    else:
        raise ValueError("unknown distribution %r" % (distribution,))

    return np.asarray(sigma, float), np.asarray(x, float)


def momentError(distribution, srel, p, meanSigma=1.0):
    """max relative error of the discrete moments 0..2p-1. Diagnostic."""
    sigma, x = sizeClasses(distribution, srel, p, meanSigma)
    exact = analyticMoments(distribution, srel, 2*p, meanSigma)
    got = np.array([np.sum(x*sigma**k) for k in range(2*p)])
    return float(np.max(np.abs(got - exact)/np.abs(exact)))
