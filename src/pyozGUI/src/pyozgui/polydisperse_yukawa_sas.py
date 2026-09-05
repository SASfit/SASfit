# -*- coding: utf-8 -*-
"""
SAS layer on top of the polydisperse hard-sphere-Yukawa MSA solution:
size distribution -> N-bin discretization -> S_ij(Q) -> form factors -> I(Q).

Exact scattered intensity for a polydisperse interacting system (no decoupling
or local-monodisperse approximation):

    I(Q) = sum_ij sqrt(n_i n_j) F_i(Q) F_j(Q) S_ij^AL(Q)

with S_ij^AL from polydisperse_yukawa_sq.py (spec 7i) and n_i the per-bin
number density. For a single component this reduces to I = n |F|^2 S(Q).

Also provides ALL the approximate approaches SASfit implements, so their error
can be quantified against the exact result. Transcribed from SASfit's own
manual, doc/manual/SASfit_ch3.tex section "Methods to include structure
factors" (the GUI radiobuttons in sasfit_analytical.tcl expose values 0-4; the
manual documents a sixth, the van der Waals one-fluid approximation):

  0 monodisperse approach       I = n <F^2> S(Q)
  1 decoupling (Kotlarchyk & Chen 1983)
                                I = n{<F^2> + <F>^2 [S(Q)-1]}
  2 local monodisperse (Pedersen 1994)
                                I = n sum_i w_i F_i^2 S(Q; R_i)
  3 partial structure factors   I = n{<F^2> + sum_ij w_i w_j F_i F_j [S_ij-1]}
  4 scaling approximation (Gazzillo et al. 1999)
                                as 3, with each term weighted by Vbar_ij/V_av
  5 van der Waals one-fluid     as 4, but normalised by <Vbar> instead of V_av

In 3-5 the "partial" structure factor is NOT the true S_ij: it is the
MONODISPERSE S(Q) evaluated at the mean radius (R_i+R_j)/2, which is exactly
the approximation the exact solution here removes. Vbar_ij = (4pi/3)
((R_i+R_j)/2)^3, V_av = <V_i>, <Vbar> = sum_ij w_i w_j Vbar_ij.

All of 0 and 2-5 need auxiliary MONODISPERSE MSA solutions at diameters set by
the size grid, and some of those fictitious systems have no physical MSA
solution even where the real polydisperse system does (see I_lma's docstring).
Every such method reports the affected weight and refuses rather than
returning a quietly-wrong curve.

Restricted to delta_i = 1 (common Yukawa coupling, arbitrary size
polydispersity) -- see the spec's open items.

CHOOSING nbins AND nsig
-----------------------
`nsig` truncates the size quadrature at +/- nsig standard deviations. It trades
truncation bias against numerical dynamic range, and the binding quantity is
always the highest moment used by I(Q), <R^6>. Measured relative error of
<R^6>, and the resulting z*sigma_max (the S_ij assembly carries exp(z*sigma_ij),
so this is the precision-limiting number; a guard warns above 45):

    relsd   nsig=3    nsig=4    nsig=5    nsig=6     z*sigma_max (nsig=5)
    0.15    1.5e-2    9.0e-4    2.0e-5    7.7e-6            12
    0.31    1.1e-1    1.4e-2    6.9e-4    9.1e-6            23
    0.53    5.0e-1    1.6e-1    2.3e-2    1.4e-3            49  (guard fires)

Default nsig=5. For relsd above ~0.4 there is no good choice: raising nsig to
fix the moments pushes z*sigma_max past the point where the S_ij assembly holds
precision. Use `moment_report` to check any case you care about.

`nbins`: S(Q) and I(Q) at low/mid Q converge to 5-6 digits by nbins ~ 12.
The deep form-factor minima (where I(Q) has fallen by ~300x and is a
near-cancellation of oscillating F_i) need nbins ~ 20. Judge convergence
per-Q, not by a max-relative-change across decades of I(Q).
"""
import numpy as np
from scipy.special import gammaln, betaln, roots_genlaguerre, roots_jacobi

from polydisperse_sas_base import PolydisperseSASBase
from polydisperse_yukawa_msa import PolydisperseOneYukawaMSA
from polydisperse_yukawa_sq import PolydisperseOneYukawaSq


# --------------------------------------------------------------------------
# size distributions (number-weighted, in RADIUS)
# --------------------------------------------------------------------------
def _moment_rule(moments, N, dps=200):
    """N-point Gaussian rule exact for `moments[0..2N-1]`, by Golub-Welsch.

    Given the first 2N moments m_k of a positive weight, the N-point rule that
    integrates every polynomial of degree < 2N exactly is obtained from the
    generalised eigenproblem for the Hankel matrices H_ij = m_{i+j} and
    H*_ij = m_{i+j+1}: the nodes are the eigenvalues and the weights come from
    the first components of the eigenvectors.

    This is done in high precision because the Hankel matrix of a broad
    positive weight -- the log-normal above all -- is savagely ill-conditioned;
    in float64 the Cholesky factorisation fails well before N = 8. Requires
    mpmath, which is why the caller treats it as optional.

    NOTE for the log-normal specifically: its moment problem is indeterminate
    (many distributions share its moments), so a rule built from the moments
    is exact for THOSE moments and says nothing beyond them. That is precisely
    what is wanted here -- I(Q -> 0) is controlled by <R^3>^2 and <R^6> -- but
    it is not a general statement that the rule represents the distribution
    better in every respect.
    """
    try:
        import mpmath as mp
    except ImportError as exc:                                # pragma: no cover
        raise ImportError(
            "the moment-exact quadrature needs mpmath (pip install mpmath); "
            "use rule='legendre' for the float64 rule instead") from exc
    mp.mp.dps = dps
    m = [mp.mpf(x) if not isinstance(x, mp.mpf) else x for x in moments]
    H = mp.matrix(N, N)
    Hs = mp.matrix(N, N)
    for i in range(N):
        for j in range(N):
            H[i, j] = m[i + j]
            Hs[i, j] = m[i + j + 1]
    L = mp.cholesky(H)
    Li = mp.inverse(L)
    A = Li*Hs*Li.T
    A = (A + A.T)/2                       # symmetrise against rounding
    E, V = mp.eigsy(A)
    nodes = np.array([float(E[i]) for i in range(N)], float)
    w = np.array([float(V[0, i]**2) for i in range(N)], float)
    w = w/np.sum(w)
    order = np.argsort(nodes)
    return nodes[order], w[order]


class LogNormal:
    """f(R) = exp(-ln^2(R/R0)/(2 s^2)) / (R s sqrt(2 pi));  <R^n> = R0^n e^{n^2 s^2/2}.
    `sigma_ln` is s (the log-space width). The number-weighted relative
    standard deviation of R is sqrt(e^{s^2}-1)."""

    def __init__(self, R0, sigma_ln, rule="legendre"):
        self.R0, self.s = float(R0), float(sigma_ln)
        if rule not in ("legendre", "moment"):
            raise ValueError("rule must be 'legendre' or 'moment'")
        self.rule = rule
        self._cache = {}

    def pdf(self, R):
        R = np.asarray(R, float)
        return np.exp(-np.log(R/self.R0)**2/(2*self.s**2))/(R*self.s*np.sqrt(2*np.pi))

    def moment(self, n):
        return self.R0**n*np.exp(n**2*self.s**2/2)

    def support(self, nsig=6.0):
        return (self.R0*np.exp(-nsig*self.s), self.R0*np.exp(nsig*self.s))

    def nodes_weights(self, nbins, nsig=5.0):
        """Gauss-Legendre in log space over u in [-nsig, nsig], with the
        Gaussian weight in the integrand (R = R0 exp(s u), u ~ N(0,1)).

        With rule="moment" a Golub-Welsch rule built from the analytic
        moments is used instead, which IS exact for moments 0..2N-1 (the
        Legendre rule below is not: it reaches ~1e-5 on <R^6> at N=12, against
        ~1e-16 for the moment rule, and <R^3>^2 and <R^6> are what set I(Q) as
        Q -> 0). It needs mpmath and places its nodes differently -- wider at
        the top -- so it is opt-in rather than the default.

        Untruncated Gauss-Hermite would be exact for the moments, but it places
        nodes out to |u| ~ 6 and the node range then grows without bound with
        nbins: at s=0.3, nbins=24 spans R by a factor 165, with weights down to
        1e-17 and exp(z*sigma_max) ~ 1e33. Those bins hold no particles and
        wreck the S_ij(Q) assembly by cancellation. Truncating keeps the
        dynamic range bounded and makes nsig an explicit, physical knob."""
        if self.rule == "moment":
            key = ("moment", nbins)
            if key not in self._cache:
                try:
                    import mpmath as mp
                    mp.mp.dps = 200
                    mom = [mp.mpf(self.R0)**k*mp.e**(mp.mpf(k)**2*mp.mpf(self.s)**2/2)
                           for k in range(2*nbins)]
                except ImportError:
                    mom = None
                if mom is None:
                    raise ImportError(
                        "LogNormal(rule='moment') needs mpmath "
                        "(pip install mpmath)")
                self._cache[key] = _moment_rule(mom, nbins)
            return self._cache[key]
        x, wgl = np.polynomial.legendre.leggauss(nbins)
        u = nsig*x
        w = wgl*np.exp(-u**2/2)
        return self.R0*np.exp(self.s*u), w/np.sum(w)

    def __repr__(self):
        return (f"LogNormal(R0={self.R0:g}, sigma_ln={self.s:g}, "
                f"relsd={np.sqrt(np.exp(self.s**2)-1):.3f}, rule={self.rule})")


class SchulzZimm:
    """Gamma / Schulz-Zimm: shape z+1, mean <R> = Rmean.
    f(R) = ((z+1)/<R>)^{z+1} R^z e^{-(z+1)R/<R>} / Gamma(z+1)
    <R^n> = (<R>/(z+1))^n Gamma(z+1+n)/Gamma(z+1);  relative sd = 1/sqrt(z+1)."""

    def __init__(self, Rmean, z=None, relsd=None):
        if (z is None) == (relsd is None):
            raise ValueError("give exactly one of z, relsd")
        self.Rmean = float(Rmean)
        self.z = float(z) if z is not None else 1.0/float(relsd)**2 - 1.0

    def pdf(self, R):
        R = np.asarray(R, float)
        z, Rm = self.z, self.Rmean
        lg = ((z + 1)*np.log((z + 1)/Rm) + z*np.log(R)
              - (z + 1)*R/Rm - gammaln(z + 1))
        return np.exp(lg)

    def moment(self, n):
        z, Rm = self.z, self.Rmean
        return (Rm/(z + 1))**n*np.exp(gammaln(z + 1 + n) - gammaln(z + 1))

    def support(self, nsig=6.0):
        sd = self.Rmean/np.sqrt(self.z + 1)
        return (max(1e-9*self.Rmean, self.Rmean - nsig*sd), self.Rmean + nsig*sd)

    def nodes_weights(self, nbins, nsig=5.0):
        """Generalised Gauss-Laguerre with alpha = z -- exact for every
        polynomial moment of a Gamma distribution, so <R^n> is reproduced to
        machine precision for n up to 2*nbins-1.

        roots_genlaguerre breaks down for very large alpha (very narrow
        distributions, relsd = 1/sqrt(z+1) below a few percent), returning
        non-finite nodes. A Gamma distribution with large z is asymptotically
        Gaussian, so fall back to Gauss-Hermite about the mean there."""
        if self.z < 1e3:
            # roots_genlaguerre can return non-finite nodes/weights here; that
            # is exactly what the isfinite guard below is for. Silence the
            # warning the arithmetic emits on the way to being caught, so a
            # handled fallback does not look like an error to the caller (it
            # is visible in the GUI's log otherwise).
            with np.errstate(invalid="ignore", divide="ignore"):
                t, W = roots_genlaguerre(nbins, self.z)
                R, w = self.Rmean*t/(self.z + 1), W/np.sum(W)
            if np.all(np.isfinite(R)) and np.all(np.isfinite(w)) and np.all(R > 0):
                return R, w
        sd = self.Rmean/np.sqrt(self.z + 1)
        x, w = np.polynomial.hermite.hermgauss(nbins)
        return self.Rmean + np.sqrt(2.0)*sd*x, w/np.sqrt(np.pi)

    def __repr__(self):
        return f"SchulzZimm(Rmean={self.Rmean:g}, z={self.z:g}, relsd={1/np.sqrt(self.z+1):.3f})"


class Gamma:
    """Gamma distribution in radius, in the standard (shape k, scale theta)
    parameterisation:

        f(R) = R^{k-1} e^{-R/theta} / (Gamma(k) theta^k),   R > 0
        <R^n> = theta^n Gamma(k+n)/Gamma(k)
        mean  = k theta,   relative sd = 1/sqrt(k)

    This is the SAME family as SchulzZimm above -- k = z+1, theta = <R>/(z+1)
    -- and is provided because (k, theta) is the usual parameterisation
    outside the polymer literature, where the Schulz-Zimm (Rmean, z) form is
    the convention. It delegates to SchulzZimm so that both share one
    implementation and one exact quadrature; there is no second numerical
    route here to get out of step.
    """

    def __init__(self, k, theta):
        self.k, self.theta = float(k), float(theta)
        if self.k <= 0 or self.theta <= 0:
            raise ValueError("Gamma: shape k and scale theta must be positive")
        self._sz = SchulzZimm(self.k*self.theta, z=self.k - 1.0)

    def pdf(self, R):
        return self._sz.pdf(R)

    def moment(self, n):
        return self._sz.moment(n)

    def support(self, nsig=6.0):
        return self._sz.support(nsig)

    def nodes_weights(self, nbins, nsig=5.0):
        return self._sz.nodes_weights(nbins, nsig)

    def __repr__(self):
        return (f"Gamma(k={self.k:g}, theta={self.theta:g}, "
                f"mean={self.k*self.theta:g}, relsd={1/np.sqrt(self.k):.3f})")


class Beta:
    """Four-parameter Beta distribution on a BOUNDED, STRICTLY POSITIVE
    radius interval [Rmin, Rmax], 0 < Rmin < Rmax:

        x = (R - Rmin)/(Rmax - Rmin) ~ Beta(a, b)
        f(R) = x^{a-1} (1-x)^{b-1} / (B(a,b) (Rmax - Rmin))

    Unlike the log-normal and the Gamma, this has compact support, so there
    is no tail to truncate and `nsig` is accepted but ignored -- the support
    IS [Rmin, Rmax]. That is what makes it useful here: a hard upper bound on
    the radius bounds z*sigma_max directly, which is the quantity that limits
    the S_ij(Q) assembly's dynamic range.

    Moments are exact and analytic. With m_j = <x^j> = prod_{i<j} (a+i)/(a+b+i),

        <R^n> = sum_{j=0..n} C(n,j) Rmin^{n-j} (Rmax-Rmin)^j m_j .

    Quadrature is Gauss-Jacobi, which is the natural exact rule for the Beta
    weight (the analogue of generalised Gauss-Laguerre for the Gamma): with
    the map x = 2t-1, the weight t^{a-1}(1-t)^{b-1} becomes the Jacobi weight
    (1-x)^{b-1}(1+x)^{a-1}, so roots_jacobi(nbins, b-1, a-1) integrates every
    polynomial up to degree 2*nbins-1 exactly. All moments are therefore
    reproduced to machine precision.
    """

    def __init__(self, Rmin, Rmax, a, b):
        self.Rmin, self.Rmax = float(Rmin), float(Rmax)
        self.a, self.b = float(a), float(b)
        # The bounds are required to be positive, not merely ordered: a radius
        # interval reaching 0 or below is not a physical size distribution,
        # and sigma -> 0 would in any case break the Yukawa assembly.
        if self.Rmin <= 0:
            raise ValueError("Beta: Rmin must be > 0")
        if self.Rmax <= self.Rmin:
            raise ValueError("Beta: Rmax must be > Rmin")
        if self.a <= 0 or self.b <= 0:
            raise ValueError("Beta: shape parameters a and b must be positive")

    @property
    def width(self):
        return self.Rmax - self.Rmin

    def pdf(self, R):
        R = np.asarray(R, float)
        x = (R - self.Rmin)/self.width
        out = np.zeros_like(x)
        inside = (x > 0) & (x < 1)
        xin = x[inside]
        out[inside] = np.exp((self.a - 1)*np.log(xin)
                             + (self.b - 1)*np.log1p(-xin)
                             - betaln(self.a, self.b))/self.width
        return out

    def moment(self, n):
        n = int(n)
        a, b, R0, W = self.a, self.b, self.Rmin, self.width
        # m_j = <x^j> for x ~ Beta(a, b)
        m = np.ones(n + 1)
        for j in range(1, n + 1):
            m[j] = m[j - 1]*(a + j - 1)/(a + b + j - 1)
        from math import comb
        return float(sum(comb(n, j)*R0**(n - j)*W**j*m[j] for j in range(n + 1)))

    def support(self, nsig=6.0):
        return (self.Rmin, self.Rmax)

    def nodes_weights(self, nbins, nsig=5.0):
        """Gauss-Jacobi; `nsig` is ignored (compact support -- see the class
        docstring)."""
        x, W = roots_jacobi(nbins, self.b - 1.0, self.a - 1.0)
        R = self.Rmin + self.width*(1.0 + x)/2.0
        return R, W/np.sum(W)

    def __repr__(self):
        mean = self.moment(1)
        sd = np.sqrt(max(self.moment(2) - mean**2, 0.0))
        return (f"Beta(Rmin={self.Rmin:g}, Rmax={self.Rmax:g}, "
                f"a={self.a:g}, b={self.b:g}, mean={mean:g}, "
                f"relsd={sd/mean:.3f})")


def discretize(dist, nbins=12, nsig=5.0):
    """Discretize a size distribution into nbins (R_i, w_i) with sum w_i = 1.

    Uses the distribution's own natural quadrature when it has one
    (`nodes_weights`): Gauss-Legendre in log space for log-normal,
    generalised Gauss-Laguerre for Schulz-Zimm/Gamma, Gauss-Jacobi for Beta.
    All are essentially exact
    for the moments, which matters because I(Q) at low Q is sensitive to
    <R^3>^2 and <R^6>. Falls back to Gauss-Legendre against the pdf for any
    other distribution. Verify with `moment_report`."""
    if hasattr(dist, "nodes_weights"):
        R, w = dist.nodes_weights(nbins, nsig)
    else:
        lo, hi = dist.support(nsig)
        x, wgl = np.polynomial.legendre.leggauss(nbins)
        R = 0.5*(hi - lo)*x + 0.5*(hi + lo)
        w = wgl*dist.pdf(R)
    w = w/np.sum(w)
    order = np.argsort(R)
    return R[order], w[order]


def moment_report(dist, R, w, orders=(1, 2, 3, 6)):
    """relative error of the discretized moments against the analytic ones."""
    out = {}
    for n in orders:
        num = np.sum(w*R**n)
        ex = dist.moment(n)
        out[n] = (num, ex, abs(num - ex)/abs(ex))
    return out


# --------------------------------------------------------------------------
# form factors  (amplitudes, units of scattering-length density x volume)
# --------------------------------------------------------------------------
def _j1x3(x):
    """3 j1(x)/x = 3 (sin x - x cos x)/x^3, regular at x=0 (-> 1)."""
    x = np.asarray(x, float)
    small = np.abs(x) < 1e-3
    xs = np.where(small, 1.0, x)
    ser = 1.0 - x**2/10.0 + x**4/280.0 - x**6/15120.0
    return np.where(small, ser, 3*(np.sin(xs) - xs*np.cos(xs))/xs**3)


class Sphere:
    """Homogeneous sphere. F(Q,R) = drho * (4/3 pi R^3) * 3j1(QR)/(QR)."""

    def __init__(self, drho=1.0):
        self.drho = float(drho)

    def F(self, Q, R):
        Q = np.atleast_1d(np.asarray(Q, float))
        R = np.atleast_1d(np.asarray(R, float))
        V = 4*np.pi*R**3/3
        return self.drho*V[None, :]*_j1x3(Q[:, None]*R[None, :])

    def outer_radius(self, R):
        return np.asarray(R, float)


class CoreShell:
    """Core-shell sphere, polydisperse in the OUTER radius R.

    mode='thickness': core radius R_c = R - t (clipped at 0)
    mode='ratio'    : core radius R_c = ratio * R

    F(Q) = (rho_c - rho_s) V_c 3j1(Q R_c)/(Q R_c)
           + (rho_s - rho_solv) V   3j1(Q R)/(Q R)

    The hard core / Yukawa interaction uses the OUTER diameter 2R.
    """

    def __init__(self, rho_core=2.0, rho_shell=1.0, rho_solvent=0.0,
                 thickness=None, ratio=None):
        if (thickness is None) == (ratio is None):
            raise ValueError("give exactly one of thickness, ratio")
        self.dc = float(rho_core) - float(rho_shell)
        self.ds = float(rho_shell) - float(rho_solvent)
        self.t, self.ratio = thickness, ratio

    def _Rc(self, R):
        R = np.asarray(R, float)
        return np.clip(R - self.t, 0.0, None) if self.t is not None else self.ratio*R

    def F(self, Q, R):
        Q = np.atleast_1d(np.asarray(Q, float))
        R = np.atleast_1d(np.asarray(R, float))
        Rc = self._Rc(R)
        Vc, V = 4*np.pi*Rc**3/3, 4*np.pi*R**3/3
        return (self.dc*Vc[None, :]*_j1x3(Q[:, None]*Rc[None, :])
                + self.ds*V[None, :]*_j1x3(Q[:, None]*R[None, :]))

    def outer_radius(self, R):
        return np.asarray(R, float)


# --------------------------------------------------------------------------
# the model
# --------------------------------------------------------------------------
class PolydisperseYukawaSAS(PolydisperseSASBase):
    """Exact I(Q) for a polydisperse hard-sphere-Yukawa system.

    dist      : LogNormal or SchulzZimm (in radius)
    phi       : total hard-core volume fraction (of the OUTER spheres)
    z, K      : Yukawa screening (in units of 1/sigma_ref) and contact strength
    formfactor: Sphere or CoreShell
    nbins     : number of size bins

    NOTE on z: the solver works in units where lengths are as given, so z here
    is an inverse length in the SAME units as R. (For a monodisperse reference
    of diameter sigma_ref, the dimensionless Z of liboneyukawa is z*sigma_ref.)
    """

    def __init__(self, dist, phi, z, K, formfactor=None, nbins=12, nsig=5.0):
        self.dist, self.phi, self.z, self.K = dist, float(phi), float(z), float(K)
        self.ff = formfactor if formfactor is not None else Sphere()
        self.R, self.w = discretize(dist, nbins, nsig)
        self.sigma = 2*self.ff.outer_radius(self.R)
        V = np.pi*self.sigma**3/6
        self.n_tot = self.phi/np.sum(self.w*V)          # phi = n_tot <V>
        self.rho = self.n_tot*self.w

        # Solve in REDUCED units (lengths in units of sigma_ref). The closure
        # and S(Q) machinery was validated at sigma ~ 1; feeding it physical
        # lengths (sigma ~ 100 A) leaves the physics unchanged but pushes a,
        # Bhat and Gamma over many orders of magnitude, which is how scale bugs
        # get in. Reducing here keeps every solver input O(1).
        self.sigma_ref = float(np.sum(self.w*self.sigma**3)**(1/3))
        sr = self.sigma_ref
        self.solver = PolydisperseOneYukawaMSA(self.sigma/sr, self.rho*sr**3,
                                              z=self.z*sr, K=self.K)
        self.sol = self.solver.solve_gamma()
        self.sq = PolydisperseOneYukawaSq(self.solver, self.sol)

        # Dynamic-range guard. The S_ij(Q) assembly carries exp(z*sigma_ij)
        # factors, so an over-wide size grid loses precision by cancellation
        # long before it overflows. This is about the QUADRATURE range, not the
        # physics: the closure itself stays healthy (Gamma is unchanged to 7
        # digits as nbins grows). Reduce nsig or nbins if this fires.
        # Dynamic-range guard, on the EFFECTIVE span.
        #
        # An earlier version tested z*sigma over ALL bins and warned above 45.
        # That is the wrong quantity, and it fires on healthy calculations:
        # a Gauss-Laguerre grid puts its outermost nodes at enormous radii
        # carrying utterly negligible weight (measured: at relsd = 0.5 and
        # nbins = 96 the largest node sits at z*sigma = 548 with w = 5e-151),
        # so those species contain no particles and are numerically inert.
        # Measured across nbins = 12..200 at z*sigma_max up to 548, the
        # contact-matrix symmetry, the eq.72 residual and the S_ij sum rule
        # all stay at 1e-15; nothing degrades. What matters is the span of the
        # bins that actually hold particles, and that barely grows with nbins.
        self.zsigma_max = float(self.z*self.sigma.max())
        order = np.argsort(self.sigma)
        keep = order[np.cumsum(self.w[order]) <= 1.0 - 1e-6]
        if len(keep) == 0:
            keep = order[:1]
        self.zsigma_effective = float(self.z*self.sigma[keep].max())
        if self.zsigma_effective > 45.0:
            import warnings as _w
            _w.warn(f"bins holding particles span z*sigma = "
                    f"{self.zsigma_effective:.1f} (exp = "
                    f"{np.exp(self.zsigma_effective):.1e}); S_ij(Q) may lose "
                    f"precision. Reduce nsig (currently {nsig}) or the width "
                    f"of the distribution.", RuntimeWarning)

    # ---- structure ----
    def _mono_sq(self, sigma_eff):
        """cached monodisperse solver at diameter sigma_eff, same phi, in
        reduced units (see __init__). Raises RuntimeError if that fictitious
        monodisperse system has no physical MSA solution."""
        key = round(float(sigma_eff), 10)
        cache = self.__dict__.setdefault("_mono_cache", {})
        if key not in cache:
            s = PolydisperseOneYukawaMSA(np.array([1.0]),
                                        np.array([6*self.phi/np.pi]),
                                        z=self.z*sigma_eff, K=self.K)
            cache[key] = PolydisperseOneYukawaSq(s, s.solve_gamma())
        return cache[key]

    def _mono_S(self, Q, sigma_eff):
        sq = self._mono_sq(sigma_eff)
        return np.array([sq.S_number(q*sigma_eff) for q in np.atleast_1d(Q)])

    def S_partials(self, Q):
        """S_ij^AL(Q). Q is in physical inverse-length units; the reduction to
        the solver's own units (Q*sigma_ref) happens here."""
        Q = np.atleast_1d(np.asarray(Q, float))
        return np.array([self.sq.S_AL(q*self.sigma_ref) for q in Q])

    def S_number(self, Q):
        Q = np.atleast_1d(np.asarray(Q, float))
        return np.array([self.sq.S_number(q*self.sigma_ref) for q in Q])
