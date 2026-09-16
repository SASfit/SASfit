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

An ALTERNATIVE rule, quantileClasses(), is provided at the foot of this
module: the probability integral transform turns the integral over
(0, infinity) into one over [0, 1] needing only a quantile function. It is
not the default -- moment matching wins outright at the three to seven
classes an OZ solve uses -- but it is an independent cross-check on the
rules above and a cheap route for any new distribution. See the comment
block there for the measurements.

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
    try:
        R = mp.cholesky(H)
    except (ValueError, ZeroDivisionError) as exc:
        #mpmath raises a bare "matrix is not positive-definite", which tells
        #the caller nothing about the cause or the remedy. The Hankel matrix
        #of moments is ill-conditioned by construction -- its condition
        #number exceeds double precision by p ~ 10 for a broad distribution
        #-- so this is the expected outcome at large p, not a bug. Measured:
        #LogNormal fails at p >= 32, Weibull at p >= 16, both at srel = 0.3.
        raise ValueError(
            f"Golub-Welsch failed at p = {p} with dps = {dps}: the Hankel "
            f"moment matrix is not positive-definite ({exc}). This is the "
            f"expected behaviour at large p, not a defect -- the moments "
            f"span too many decades for the factorisation to survive, and "
            f"the spread here is {float(max(moments))/max(float(min(moments)), 1e-300):.2e}. "
            f"Either use fewer classes (the OZ solve needs only 3-7, where "
            f"this rule is exact to 1e-16), raise dps, or use "
            f"quantileClasses(), which needs no moments and is stable at any "
            f"node count.") from exc
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
        #The RAW Laguerre weights carry a factor Gamma(alpha+p+1) and overflow
        #for a narrow distribution: 9e155 at alpha = 99 (s = 0.10), 1.7e274 at
        #alpha = 155 (s = 0.08), and +inf at alpha = 203 (s = 0.07). The NODES
        #stay finite throughout, so the failure is invisible until w.sum()
        #turns the normalised weights into NaN -- whereupon the solver reports
        #"Percus-Yevick solve diverged" and the real cause is hidden.
        #
        #s < 0.08 is not exotic: monodisperse-looking colloids sit there. Fall
        #back to the high-precision Golub-Welsch path already used for the
        #log-normal and Weibull weights, which has no such overflow because
        #mpmath carries the large factor symbolically.
        w = np.asarray(w, float)
        if not np.all(np.isfinite(w)) or not np.isfinite(w.sum()) or w.sum() <= 0.0:
            sigma, x = _golubWelsch(
                analyticMoments(distribution, srel, 2*p + 1, meanSigma), p)
        else:
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


# ----------------------------------------------------------------------
# Quantile-transform quadrature: an ALTERNATIVE rule and a cross-check.
#
# The probability integral transform turns an integral over (0, infinity)
# with a weight into an unweighted integral over the unit interval:
#
#     int_0^inf f(sigma) p(sigma) dsigma  =  int_0^1 f(Q(u)) du,
#
# with u = F(sigma) and Q = F^-1 the quantile function. The weight vanishes,
# the interval becomes finite, and the tail of a heavy-tailed distribution is
# compressed into u -> 1 automatically. It needs only a quantile function --
# no moments, no orthogonal polynomials, no high-precision arithmetic.
#
# WHY THIS IS AN ALTERNATIVE AND NOT THE DEFAULT. Measured against the exact
# log-normal moments at the class counts the Ornstein-Zernike solve actually
# uses:
#
#     s = 0.6, p = 3    Golub-Welsch <R^6> err 2e-16   quantile/log rules 8e-1
#     s = 0.6, p = 7    Golub-Welsch <R^6> err 0e+00   quantile/log rules 7e-2
#
# Moment matching wins outright for p = 3..7, which is the whole point of
# using it: the thermodynamics depends on the low-order moments and a p-point
# Gaussian rule reproduces 2p-1 of them exactly. The quantile rules only
# overtake at N >~ 16, where Golub-Welsch has begun to suffer from its
# ill-conditioning (it returned NaN at N >= 32 in testing) but where nobody
# runs an OZ solve.
#
# Nor does it help the FORM-FACTOR average, the other place a size quadrature
# appears. There the integrand oscillates in sigma and no moment-based or
# spectral rule converges quickly: log-space Gauss-Hermite was still 34 %
# wrong on <|F|^2>(Q) at N = 128. That average needs dense sampling, which is
# what nFF already does.
#
# So the quantile rule earns its place in two other ways:
#
#   1. as an INDEPENDENT CHECK on sizeClasses(), which is the kind of
#      cross-validation that has found every serious defect in this project;
#   2. as a cheap route for a NEW distribution -- anything with a ppf gets a
#      working rule with no moment derivation and no mpmath.


def quantileFunction(distribution, srel, meanSigma=1.0):
    """Q(u) = F^-1(u) for each supported distribution, as a callable.

    Conventions match analyticMoments(): mean diameter meanSigma, relative
    standard deviation srel. Verified against the closed-form moments to
    between 1e-16 and 1e-8 for Schulz, LogNormal and Weibull.
    """
    from scipy.stats import norm, gamma as gammadist
    if distribution == "LogNormal":
        s2 = np.log(1.0 + srel*srel)
        mu = np.log(meanSigma) - 0.5*s2
        return lambda u: np.exp(mu + np.sqrt(s2)*norm.ppf(u))
    if distribution == "Gaussian":
        return lambda u: meanSigma*(1.0 + srel*norm.ppf(u))
    if distribution == "Schulz":
        #Schulz/gamma with shape t+1 and unit mean before scaling.
        t = 1.0/srel**2 - 1.0
        return lambda u: meanSigma*gammadist.ppf(u, t + 1.0)/(t + 1.0)
    if distribution == "Weibull":
        k = _weibull_shape_from_cv(srel)
        lam = meanSigma/gammafn(1.0 + 1.0/k)
        return lambda u: lam*(-np.log1p(-u))**(1.0/k)
    raise ValueError(f"unknown distribution {distribution!r}")


def quantileClasses(distribution, srel, p, meanSigma=1.0, level=6):
    """(sigma, x) from the quantile transform with a tanh-sinh rule.

    tanh-sinh (double exponential) rather than Gauss-Legendre, because the
    transform moves the difficulty rather than removing it: Q(u)^k diverges
    as u -> 1 for a heavy tail, so the integrand has an ENDPOINT SINGULARITY
    on [0, 1]. Gauss-Legendre converges only like 1/N there -- measured 8.8 %
    error on the log-normal <R^6> at N = 128, s = 0.5 -- while tanh-sinh,
    which is designed for exactly this, reached 1e-7 at N = 16.

    `p` here is the half-count: the rule returns up to 2p+1 nodes, minus any
    that underflow to u = 0 or 1. Unlike sizeClasses() this is NOT
    moment-exact; it converges instead, so ask for more nodes than you would
    classes.

    Scale-adaptive by construction: a very narrow distribution needs no
    special handling, because Q maps [0, 1] onto whatever range the
    distribution occupies. There is no narrow/wide crossover to tune.

    THE GAUSSIAN IS TRUNCATED, deliberately. A Gaussian has support below
    sigma = 0, which is unphysical for a diameter, so those nodes are dropped
    and the weights renormalised. The resulting moments therefore differ from
    the UNtruncated closed-form values of analyticMoments() by roughly the
    mass below zero -- measured 4.0e-4 at s = 0.3 and 2.7e-2 at s = 0.5,
    against P(sigma < 0) = 4.3e-4 and 2.3e-2. That is agreement, not error:
    the same physical truncation the Gauss-Hermite small-node guard applies
    in sizeClasses(). For the other three distributions, which have support
    only on sigma > 0, this rule agrees with the closed-form moments to
    between 1e-16 and 1e-8.
    """
    if srel <= 0:
        return np.array([float(meanSigma)]), np.array([1.0])
    Q = quantileFunction(distribution, srel, meanSigma)
    h = float(level)/max(int(p), 1)
    t = np.arange(-p, p + 1)*h
    g = 0.5*np.pi*np.sinh(t)
    u = 0.5*(1.0 + np.tanh(g))
    w = 0.5*np.pi*np.cosh(t)/(2.0*np.cosh(g)**2)*h
    ok = (u > 0.0) & (u < 1.0) & np.isfinite(w) & (w > 0.0)
    sigma = np.asarray(Q(u[ok]), float)
    w = w[ok]
    good = np.isfinite(sigma) & (sigma > 0.0)
    sigma, w = sigma[good], w[good]
    if sigma.size == 0:
        raise ValueError(f"quantileClasses produced no usable nodes for "
                         f"{distribution} at srel={srel}")
    order = np.argsort(sigma)
    return sigma[order], (w/w.sum())[order]


def crossCheckMoments(distribution, srel, p, meanSigma=1.0, nQuantile=64):
    """Relative moment error of sizeClasses() against the quantile rule.

    An INDEPENDENT check: the two routes share no machinery -- one builds
    nodes from moments via a Hankel eigenproblem, the other evaluates a
    quantile function on a tanh-sinh grid -- so agreement is meaningful.

    Returns, for <sigma^3> and <sigma^6> (the moments governing I(Q -> 0)):
        analytic            sizeClasses vs the closed-form moments
        quantile            sizeClasses vs the quantile rule
        quantileVsAnalytic  the quantile rule vs the closed form, i.e. how
                            much the reference itself can be trusted

    Behaves as intended in testing: at p = 5 it confirms sizeClasses to
    1e-16, and at p = 3 it reports the genuine truncation error (5.6e-3 for
    Schulz, 1.3e-2 for LogNormal at s = 0.4) with the quantile and analytic
    columns agreeing.
    """
    sig, x = sizeClasses(distribution, srel, p, meanSigma)
    sq, wq = quantileClasses(distribution, srel, nQuantile, meanSigma)
    exact = analyticMoments(distribution, srel, 7, meanSigma)
    out = {}
    for k in (3, 6):
        got = float(np.sum(x*sig**k))
        ref = float(np.sum(wq*sq**k))
        out[f"m{k}"] = dict(
            analytic=abs(got/exact[k] - 1.0) if exact[k] else float("nan"),
            quantile=abs(got/ref - 1.0) if ref else float("nan"),
            quantileVsAnalytic=abs(ref/exact[k] - 1.0) if exact[k]
            else float("nan"))
    return out
