r"""Direct Bayesian estimation of the radial density distribution.

Implementation of

    S. Hansen, "Estimation of the density distribution from small-angle
    scattering data", J. Appl. Cryst. 49 (2016) 856-865.

The radial scattering-length-density (contrast) profile rho(r) of a sphere,
an infinitely long cylinder or an infinite lamella is estimated *directly*
from I(q) (no intermediate p(r)), by minimising (eq. 12)

    Phi(rho) = alpha S(rho) + chi^2(rho)/2,     I_i = rho^T C_i rho  (+ b)

with the smoothness (eq. 14) or maximum-entropy (eq. 15) functional S.
Because the model is quadratic in rho, Phi is minimised with a damped
Gauss-Newton (Levenberg-Marquardt) iteration.

The Lagrange multiplier alpha (overall noise level) and the maximum radius
r_max (D = 2 r_max) are hyper-parameters estimated from their posterior
probability (Hansen 2000, 2016), using the Gaussian approximation around the
optimum::

    log P(alpha, r_max) = -alpha S - chi^2/2 - 1/2 log det(alpha A + B)
                          + 1/2 log det(alpha A)

with A = grad grad S and B = J^T J (Gauss-Newton Hessian of chi^2/2).

To avoid the local minima mentioned by Hansen, alpha is decreased from a
large value (where the solution is close to a homogeneous profile / the
prior) towards small values along a continuation path, each solution being
warm-started from the previous one.
"""

from __future__ import annotations

import warnings
from dataclasses import dataclass, field

import numpy as np
from scipy import linalg, optimize

from . import geometry as geo
from .background import Background, Constant
from .regularization import MaxEnt, Smoothness
from .resolution import NoResolution

__all__ = ["DensityProfileFit", "ProfileSolution", "ProfileResult"]


@dataclass
class ProfileSolution:
    """Solution for one fixed (alpha, r_max)."""

    alpha: float
    r_max: float
    geometry: str
    rho: np.ndarray  # shell values, shell k = [k, k+1] * r_max / N
    background: float  # sum of the constant terms of the background
    cov: np.ndarray  # posterior covariance of [rho, z_background]
    chi2: float
    S: float
    log_evidence: float
    I_fit: np.ndarray
    n_good: float
    converged: bool
    basis: object = "rho"  # 'rho', 'mass' (= r^(dim-1) rho) or exponent p (r^p rho)
    bg_z: np.ndarray = None  # packed free background parameters
    bg_params: dict = None  # name -> (value, free)
    bg_curve: np.ndarray = None  # background at the data q
    grid: np.ndarray = None  # normalised shell edges (None = uniform)

    @property
    def x(self):
        """Full parameter vector [rho, z_background]."""
        return np.r_[self.rho, self.bg_z if self.bg_z is not None else []]

    @property
    def n(self):
        return self.rho.size

    @property
    def edges(self):
        return geo.shell_edges(self.r_max, self.n, self.grid)

    @property
    def r_centres(self):
        e = self.edges
        return 0.5 * (e[1:] + e[:-1])

    @property
    def rho_err(self):
        return np.sqrt(np.clip(np.diag(self.cov)[: self.n], 0, None))

    def rho_on(self, r, rho=None):
        """Profile rho(r) of the step representation (zero = solvent beyond r_max).

        ``rho`` (optional) are coefficients in this solution's basis."""
        return geo.coef_to_rho(self.rho if rho is None else rho, r, self.r_max, self.geometry, self.basis, self.grid)

    def mass_on(self, r, rho=None):
        """Radial mass r^(dim-1) rho(r)."""
        return geo.coef_to_mass(self.rho if rho is None else rho, r, self.r_max, self.geometry, self.basis, self.grid)

    def Rg(self, rho=None):
        return geo.radius_of_gyration(self.rho if rho is None else rho, self.r_max, self.geometry, self.basis, self.grid)

    def forward_amplitude0(self, rho=None):
        """F(0) = integral of rho over the particle (excess scattering length)."""
        rho = self.rho if rho is None else rho
        return float(geo.volume_weights(self.r_max, self.n, self.geometry, self.basis, self.grid) @ rho)

    def pddf(self, r=None):
        return geo.pddf(self.rho, self.r_max, self.geometry, r=r, basis=self.basis, grid=self.grid)

    def intensity(self, q, rho=None, resolution=None):
        """Model intensity (without background) at arbitrary q."""
        rho = self.rho if rho is None else rho
        res = NoResolution() if resolution is None else resolution
        qf, R = res.setup(np.asarray(q, float))
        a = geo.amplitude_matrix(qf, self.r_max, self.n, self.geometry, self.basis, self.grid)
        If = geo.prefactor(qf, self.geometry) * (a @ rho) ** 2
        return If if R is None else R @ If


@dataclass
class ProfileResult:
    """Evidence-averaged result of :meth:`DensityProfileFit.fit`."""

    best: ProfileSolution
    r: np.ndarray
    rho: np.ndarray
    rho_err: np.ndarray
    mass: np.ndarray  # r^(dim-1) rho(r)
    mass_err: np.ndarray
    r_max: tuple  # (mean, std) from the posterior
    log_alpha: tuple  # (mean, std)
    Rg: tuple
    background: tuple
    r_max_grid: np.ndarray
    r_max_posterior: np.ndarray
    q: np.ndarray
    I_fit: np.ndarray
    chi2_red: float
    regularization: str
    geometry: str
    solutions: list = field(default_factory=list, repr=False)
    weights: np.ndarray = field(default=None, repr=False)
    bg_params: dict = field(default_factory=dict)  # name -> (mean, std, free)
    bg_curve: np.ndarray = None

    def summary(self) -> str:
        b = self.best
        lines = [
            f"Density profile (Hansen 2016)  geometry={self.geometry}  regularisation={self.regularization}  "
            f"represented: r^{geo.exponent(self.geometry, self.best.basis):.4g}*rho",
            "-" * 60,
            f"  r_max      = {self.r_max[0]:.5g} +/- {self.r_max[1]:.2g}   (best {b.r_max:.5g})",
            f"  log alpha  = {self.log_alpha[0]:.4g} +/- {self.log_alpha[1]:.2g}",
            f"  Rg         = {self.Rg[0]:.5g} +/- {self.Rg[1]:.2g}",
        ]
        if self.bg_params:
            for nm, (v, e, fr) in self.bg_params.items():
                lines.append(f"  bg {nm:<20s} = {v:.5g}" + (f" +/- {e:.2g}" if fr else "   (fixed)"))
        lines += [
            f"  chi2/M     = {self.chi2_red:.4g}",
            f"  N_good     = {b.n_good:.3g}",
            f"  log Ev     = {b.log_evidence:.6g}",
        ]
        return "\n".join(lines)

    def save(self, prefix: str) -> None:
        """Write ``<prefix>_rho.dat``, ``_fit.dat``, ``_pr.dat``, ``_rmax.dat`` and ``_params.txt``."""
        np.savetxt(f"{prefix}_rho.dat", np.c_[self.r, self.rho, self.rho_err, self.mass, self.mass_err],
                   header="r rho drho r^(dim-1)*rho d(r^(dim-1)*rho)")
        bgc = self.bg_curve if self.bg_curve is not None else np.zeros_like(self.q)
        np.savetxt(f"{prefix}_fit.dat", np.c_[self.q, self.I_fit, bgc], header="q I_fit(best) background(best)")
        rp, p = self.best.pddf()
        np.savetxt(f"{prefix}_pr.dat", np.c_[rp, p], header="r p(r) of best solution")
        np.savetxt(f"{prefix}_rmax.dat", np.c_[self.r_max_grid, self.r_max_posterior], header="r_max posterior")
        with open(f"{prefix}_params.txt", "w") as fh:
            fh.write(self.summary() + "\n")

    def plot(self, data=None, rho_true=None, fig=None):  # pragma: no cover - convenience
        import matplotlib.pyplot as plt

        fig = fig or plt.figure(figsize=(13, 4))
        ax1, ax2, ax3 = fig.subplots(1, 3)
        if data is not None:
            q, I, s = data
            ax1.errorbar(q, I, s, fmt=".", ms=3, color="0.4", label="data", zorder=1)
        ax1.plot(self.q, self.I_fit, label="fit")
        ax1.set(xscale="log", yscale="log", xlabel="q", ylabel="I(q)")
        ax1.legend()
        ax2.plot(self.r, self.rho, label="estimate")
        ax2.fill_between(self.r, self.rho - self.rho_err, self.rho + self.rho_err, alpha=0.3)
        if rho_true is not None:
            ax2.plot(*rho_true, ":", label="true")
        ax2.axhline(0, color="k", lw=0.5)
        ax2.set(xlabel="r", ylabel=r"$\rho(r)$")
        ax2.legend()
        ax3.plot(self.r_max_grid, self.r_max_posterior)
        ax3.set(xlabel=r"$r_{max}$", ylabel="P($r_{max}$)")
        fig.tight_layout()
        return fig


class DensityProfileFit:
    """Bayesian estimation of a radial density profile (Hansen 2016).

    Parameters
    ----------
    q, I, sigma : data (sigma > 0)
    geometry : ``'sphere'``, ``'cylinder'`` (infinitely long, I per length)
        or ``'lamella'`` (infinite, symmetric about the mid-plane)
    n : number of density points / shells (paper: 80)
    regularization : ``'smooth'`` (eq. 14) or ``'maxent'`` (eq. 15)
    prior : max-ent prior shape; ``None`` = uniform, a callable f(x) with
        x = r/r_max in [0, 1], or an array of length ``n`` (on x). Its
        amplitude is scaled to the data automatically.
    outer : ``'free'`` (free end point at r_max, as in most of the paper's
        examples) or ``'zero'`` (rho(r_max) pulled to 0)
    background : ``True`` fits a flat background (Hansen's free p(0) term);
        a :class:`pysldprofile.background.Background` gives a general,
        possibly nonlinear background model (constant, power law,
        Ornstein-Zernike, user functions)
    resolution : None, or a resolution object from :mod:`pysldprofile.resolution`
    basis : quantity represented by the shell values and regularised:
        ``'rho'`` (rho, as in the paper), ``'mass'`` (radial mass
        r^(dim-1) rho) or a number p < dim (r^p rho, e.g. p = 4/3 for
        Daoud-Cotton star profiles). p > 0 allows a centre cusp rho ~ 1/r^p,
        needed for coil- or star-like profiles whose I(q) decays as a power law
        without form-factor minima.
    grid : shell grid. ``None``/'uniform' (default), ``('power', gamma)``
        (t_k = (k/N)^gamma), or an array of N+1 normalised edges 0 = t_0 < ... < t_N = 1,
        e.g. from :meth:`adaptive_grid`. The grid scales with r_max. The smoothness
        prior is evaluated on the actual node positions (smoothness in r).
    background_value : fixed flat background added to the model when
        ``background`` is False (e.g. a value determined elsewhere).
    """

    def __init__(
        self,
        q,
        I,
        sigma,
        geometry: str = "sphere",
        n: int = 80,
        regularization: str = "smooth",
        prior=None,
        outer: str = "free",
        background: bool = False,
        resolution=None,
        basis="rho",
        background_value: float = 0.0,
        grid=None,
    ):
        self.q = np.asarray(q, float)
        self.I = np.asarray(I, float)
        self.sigma = np.asarray(sigma, float)
        if not (self.q.shape == self.I.shape == self.sigma.shape):
            raise ValueError("q, I and sigma must have the same shape")
        if np.any(self.sigma <= 0):
            raise ValueError("sigma must be positive")
        geo._check(geometry)
        if regularization not in ("smooth", "maxent"):
            raise ValueError("regularization must be 'smooth' or 'maxent'")
        geo._check_basis(basis, geometry)
        self.geometry = geometry
        self.basis = basis
        self.n = int(n)
        self.regularization = regularization
        self.prior = prior
        self.outer = outer
        if isinstance(background, Background):
            self.bg = background
        elif background:
            self.bg = Background([Constant()])
        elif background_value:
            self.bg = Background([Constant(float(background_value), fit=False)])
        else:
            self.bg = Background([])
        self.bg.setup(self.q, self.I)
        self.nb = self.bg.n_free
        self.fit_background = self.nb > 0
        self.resolution = NoResolution() if resolution is None else resolution
        self.qf, self.R = self.resolution.setup(self.q)
        self.pref = geo.prefactor(self.qf, geometry)
        self.sw = 1.0 / self.sigma
        self.grid = self._make_grid(grid)
        self._smooth = Smoothness(self.n, outer, grid=self.grid) if regularization == "smooth" else None
        self._cache = {}
        #: optional progress hook, called as progress(message_or_None). It is
        #: called once per alpha step (message None) and once per r_max (text);
        #: raising an exception from it aborts the fit (used by the GUI's Stop button).
        self.progress = None

    def _make_grid(self, grid):
        if grid is None or (isinstance(grid, str) and grid == "uniform"):
            return None
        if isinstance(grid, tuple) and grid and grid[0] == "power":
            g = float(grid[1])
            return None if g == 1.0 else geo.power_grid(self.n, g)
        t = np.asarray(grid, float)
        if t.size != self.n + 1 or abs(t[0]) > 1e-12 or abs(t[-1] - 1) > 1e-12 or np.any(np.diff(t) <= 0):
            raise ValueError("grid must be N+1 increasing normalised edges from 0 to 1")
        return t

    def adaptive_grid(self, r_max, beta=3.0, max_ratio=8.0, smoothing=2.0, solution=None, verbose=False,
                      min_width="auto"):
        """Normalised shell edges equidistributing the variation of the represented profile.

        A first, deliberately over-smoothed solution c(r) (alpha raised by
        ``smoothing`` e-folds above the evidence optimum, unless ``solution`` is
        given) defines the monitor

            M(r) = 1 + beta |c'(r)| / mean |c'|

        (de Boor: for a piecewise-constant representation the error per shell is
        ~ dr |c'|, so equal M-mass per shell equalises it). M is limited to
        ``max_ratio`` x its minimum, so shells are at most ``max_ratio`` times finer
        than the coarsest. ``min_width`` is the smallest allowed shell width in r units;
        ``'auto'`` = min(pi/q_max, r_max/N): structure finer than the data resolution
        pi/q_max is not determined by I(q), and finer shells only add freedom that the
        prior has to suppress (lower evidence). Returns the grid; pass it to a new
        DensityProfileFit (``grid=...``) with otherwise identical settings.
        """
        if solution is None:
            sol, _ = self.best_alpha(r_max)
            if smoothing:
                x0 = sol.x
                sol = self.solve(sol.alpha * np.exp(smoothing), r_max, x0=x0)
        else:
            sol = solution
        e = geo.shell_edges(r_max, self.n, self.grid)
        xc = 0.5 * (e[1:] + e[:-1])
        c = sol.rho
        # slope on the node positions, lightly smoothed
        dc = np.gradient(c, xc)
        k = np.array([0.25, 0.5, 0.25])
        ad = np.abs(dc)
        dc = np.convolve(np.r_[ad[0], ad, ad[-1]], k, mode="valid")
        m = np.mean(dc) if np.mean(dc) > 0 else 1.0
        M = 1.0 + beta * dc / m
        M = np.clip(M, None, max_ratio * M.min())
        de = np.diff(e)
        if min_width == "auto":
            min_width = min(np.pi / self.q.max(), r_max / self.n)
        # a shell at monitor value M gets width ~ (int M dr)/(N M): cap M so width >= min_width
        for _ in range(20):
            cap = np.sum(M * de) / (self.n * float(min_width))
            if M.max() <= cap * (1 + 1e-9):
                break
            M = np.minimum(M, cap)
        # cumulative M on the old shells -> invert at equal fractions
        cum = np.r_[0.0, np.cumsum(M * de)]
        cum /= cum[-1]
        t = np.interp(np.linspace(0, 1, self.n + 1), cum, e / r_max)
        t[0], t[-1] = 0.0, 1.0
        self._report(
            f"adaptive grid at r_max={r_max:.5g}: shell widths {np.diff(t).min() * r_max:.3g} .. "
            f"{np.diff(t).max() * r_max:.3g} (uniform {r_max / self.n:.3g})", verbose)
        return t

    def _report(self, msg=None, verbose=False):
        if verbose and msg:
            print(msg)
        if self.progress is not None:
            self.progress(msg)

    # ------------------------------------------------------------------ #
    # forward model
    # ------------------------------------------------------------------ #
    def _amp(self, r_max):
        key = float(r_max)
        if key not in self._cache:
            if len(self._cache) > 64:
                self._cache.clear()
            self._cache[key] = geo.amplitude_matrix(self.qf, r_max, self.n, self.geometry, self.basis, self.grid)
        return self._cache[key]

    def _smear(self, If):
        return If if self.R is None else self.R @ If

    def model(self, rho, r_max, z=None, with_background=True):
        """Model intensity; ``z`` are the packed background parameters (default: start values)."""
        F = self._amp(r_max) @ rho
        I = self._smear(self.pref * F**2)
        return I + self.bg.value(self.q, z) if with_background else I

    def _model_jac(self, x, r_max):
        n = self.n
        rho = x[:n]
        z = x[n:]
        a = self._amp(r_max)
        F = a @ rho
        Im = self._smear(self.pref * F**2) + self.bg.value(self.q, z)
        Jr = self._smear((2.0 * self.pref * F)[:, None] * a)
        if self.nb:
            J = np.hstack([Jr, self.bg.jac(self.q, z)])
        else:
            J = Jr
        return Im, J

    def _homogeneous_coef(self, r_max, k=None):
        """Coefficients of rho = 1 for r < r_k (k shells), 0 beyond, in the chosen basis."""
        k = self.n if k is None else k
        c = np.zeros(self.n)
        p = geo.exponent(self.geometry, self.basis)
        if p != 0.0:
            e = geo.shell_edges(r_max, self.n, self.grid)
            d = geo.DIM[self.geometry]
            # same scattering length per shell as rho = 1:  c_k = int r^(d-1) dr / int r^(d-1-p) dr
            num = (e[1:] ** d - e[:-1] ** d) / d
            den = (e[1:] ** (d - p) - e[:-1] ** (d - p)) / (d - p)
            c[:k] = (num / den)[:k]
            c /= np.abs(c[:k]).max() if k else 1.0
        else:
            c[:k] = 1.0
        return c

    def homogeneous_scale(self, r_max, shape=None):
        """Least-squares amplitude c for coefficients = c * shape (default: homogeneous rho)."""
        shape = self._homogeneous_coef(r_max) if shape is None else np.asarray(shape, float)
        m = self.model(shape, r_max, with_background=False)
        I = self.I - self.bg.value(self.q)
        w = self.sw**2
        c2 = np.sum(w * I * m) / np.sum(w * m * m)
        return float(np.sqrt(max(c2, 1e-300)))

    def _prior_shape(self):
        t = geo.uniform_grid(self.n) if self.grid is None else self.grid
        x = 0.5 * (t[1:] + t[:-1])
        if self.prior is None:
            return np.ones(self.n)
        if callable(self.prior):
            return np.asarray(self.prior(x), float)
        p = np.asarray(self.prior, float)
        if p.size != self.n:
            p = np.interp(x, (np.arange(p.size) + 0.5) / p.size, p)
        return p

    def regularizer(self, r_max):
        if self.regularization == "smooth":
            return self._smooth
        shape = self._prior_shape()
        return MaxEnt(self.homogeneous_scale(r_max, shape) * shape)

    def start(self, r_max):
        """Default starting point: homogeneous profile (smooth) or the scaled prior (maxent)."""
        if self.regularization == "maxent":
            rho = self.regularizer(r_max).m.copy()
        else:
            rho = self.homogeneous_scale(r_max) * self._homogeneous_coef(r_max)
        return np.r_[rho, self.bg.z0()]

    def truncated_start(self, r_max):
        """Best homogeneous step profile rho = c for r < R <= r_max (R scanned on the shell grid).

        Used as an additional starting point: with a too large r_max the
        homogeneous start converges to a wrong local minimum (cf. Hansen 2016,
        Discussion).
        """
        w = self.sw**2
        best = (np.inf, None)
        for k in range(max(self.n // 4, 3), self.n + 1):
            shape = self._homogeneous_coef(r_max, k)
            m = self.model(shape, r_max, with_background=False)
            I = self.I - self.bg.value(self.q)
            c2 = np.sum(w * I * m) / np.sum(w * m * m)
            if c2 <= 0:
                continue
            chi2 = np.sum(w * (c2 * m - I) ** 2)
            if chi2 < best[0]:
                best = (chi2, np.sqrt(c2) * shape)
        rho = best[1] if best[1] is not None else self.start(r_max)[: self.n]
        if self.regularization == "maxent":
            rho = np.maximum(rho, 1e-3 * rho.max())
        return np.r_[rho, self.bg.z0()]

    # ------------------------------------------------------------------ #
    # one solution
    # ------------------------------------------------------------------ #
    def solve(self, alpha, r_max, x0=None, reg=None, max_iter=300, tol=1e-10) -> ProfileSolution:
        """Minimise alpha S + chi^2/2 for fixed (alpha, r_max)."""
        n = self.n
        reg = self.regularizer(r_max) if reg is None else reg
        x = self.start(r_max) if x0 is None else np.array(x0, float)
        if not reg.feasible(x[:n]):
            x[:n] = np.maximum(x[:n], 1e-6 * np.abs(x[:n]).max())

        def parts(x):
            Im, J = self._model_jac(x, r_max)
            res = (Im - self.I) * self.sw
            Jw = J * self.sw[:, None]
            return res, Jw

        def phi(x):
            if not reg.feasible(x[:n]):
                return np.inf
            res = (self.model(x[:n], r_max, x[n:]) - self.I) * self.sw
            return alpha * reg.value(x[:n]) + 0.5 * float(res @ res)

        f = phi(x)
        lam = 1e-3
        converged = False
        nx = x.size
        logvar = reg.name == "maxent"  # max-ent: iterate in z = ln(rho) (keeps rho > 0)
        for it in range(max_iter):
            res, Jw = parts(x)
            if logvar:
                # d/dz with rho = exp(z): J_z = J diag(rho); S-Hessian (metric) alpha diag(rho)
                t = np.ones(nx)
                t[:n] = x[:n]
                Jz = Jw * t[None, :]
                g = Jz.T @ res
                H = Jz.T @ Jz
                g[:n] += alpha * x[:n] * reg.grad(x[:n])
                H[:n, :n] += alpha * np.diag(x[:n])
            else:
                g = Jw.T @ res
                H = Jw.T @ Jw
                g[:n] += alpha * reg.grad(x[:n])
                H[:n, :n] += alpha * reg.hess(x[:n])
            dH = np.sqrt(np.clip(np.diag(H), 1e-300, None))
            Hs = H / dH[:, None] / dH[None, :]
            gs = g / dH
            improved = False
            for _ in range(40):
                try:
                    with warnings.catch_warnings():
                        warnings.simplefilter("ignore")
                        dx = -linalg.solve(Hs + lam * np.eye(nx), gs, assume_a="pos") / dH
                except linalg.LinAlgError:
                    lam *= 10
                    continue
                if logvar:
                    dz = np.clip(dx[:n], -2.0, 2.0)
                    xt = x.copy()
                    xt[:n] = np.maximum(x[:n] * np.exp(dz), 1e-12 * np.max(x[:n]))
                    xt[n:] = x[n:] + dx[n:]
                else:
                    xt = x + dx
                ft = phi(xt)
                if ft < f:
                    improved = True
                    break
                lam *= 4.0
            if not improved:
                converged = True  # no further descent possible at machine precision
                break
            df = f - ft
            x, f = xt, ft
            lam = max(lam / 3.0, 1e-12)
            if df <= tol * max(abs(f), 1.0):
                converged = True
                break

        # sign convention: I = F^2 cannot distinguish rho from -rho; choose F(0) = int rho dV > 0
        if float(geo.volume_weights(r_max, n, self.geometry, self.basis, self.grid) @ x[:n]) < 0:
            x = x.copy()
            x[:n] = -x[:n]
        res, Jw = parts(x)
        B = Jw.T @ Jw
        H = B.copy()
        H[:n, :n] += alpha * reg.hess(x[:n])
        dH = np.sqrt(np.clip(np.diag(H), 1e-300, None))
        Hs = H / dH[:, None] / dH[None, :]
        try:
            cH = linalg.cho_factor(Hs)
            logdetH = 2 * np.sum(np.log(np.diag(cH[0]))) + 2 * np.sum(np.log(dH))
            cov = linalg.cho_solve(cH, np.eye(nx)) / dH[:, None] / dH[None, :]
        except linalg.LinAlgError:
            sgn, ld = np.linalg.slogdet(Hs)
            logdetH = ld + 2 * np.sum(np.log(dH))
            cov = np.linalg.pinv(Hs) / dH[:, None] / dH[None, :]
        S = reg.value(x[:n])
        chi2 = float(res @ res)
        logdetA = n * np.log(alpha) + reg.logdet_hess(x[:n])
        log_ev = -alpha * S - 0.5 * chi2 - 0.5 * logdetH + 0.5 * logdetA
        Ah = alpha * reg.hess(x[:n])
        n_good = float(n - np.trace(Ah @ cov[:n, :n]))
        zb = x[n:].copy()
        b = float(self.bg.constant(zb))
        return ProfileSolution(
            alpha=float(alpha), r_max=float(r_max), geometry=self.geometry, rho=x[:n].copy(), background=b,
            cov=cov, chi2=chi2, S=float(S), log_evidence=float(log_ev),
            I_fit=self.model(x[:n], r_max, zb), n_good=n_good, converged=converged, basis=self.basis,
            bg_z=zb, bg_params=self.bg.describe(zb), bg_curve=self.bg.value(self.q, zb), grid=self.grid,
        )

    def alpha_scale(self, r_max) -> float:
        """tr(B)/tr(A) at the starting point: the natural scale of alpha."""
        x = self.start(r_max)
        _, J = self._model_jac(x, r_max)
        Jw = J[:, : self.n] * self.sw[:, None]
        reg = self.regularizer(r_max)
        return float(np.sum(Jw**2) / np.trace(reg.hess(x[: self.n])))

    # ------------------------------------------------------------------ #
    # hyper-parameters
    # ------------------------------------------------------------------ #
    def alpha_path(self, r_max, log_alpha_range=(6.0, -16.0), step=0.5, stop_drop=25.0):
        """Continuation in alpha (large -> small) at fixed r_max.

        Returns the list of solutions along the path (each warm-started from
        the previous one). The path stops once the evidence has fallen by
        ``stop_drop`` below its running maximum.
        """
        la0 = np.log(self.alpha_scale(r_max))
        reg = self.regularizer(r_max)
        las = la0 + np.arange(log_alpha_range[0], log_alpha_range[1] - 1e-9, -abs(step))
        cands = [self.start(r_max), self.truncated_start(r_max)]
        if geo.exponent(self.geometry, self.basis) != 0.0 and self.regularization == "smooth":
            # flat coefficients (rho ~ 1/r^p): coil/star-like start
            c = np.ones(self.n)
            c = self.homogeneous_scale(r_max, c) * c
            cands.append(np.r_[c, self.bg.z0()])
        x = None
        sols = []
        best = -np.inf
        M = self.q.size
        for il, la in enumerate(las):
            self._report()
            starts = cands if x is None else [x]
            trial = [self.solve(np.exp(la), r_max, x0=x0, reg=reg) for x0 in starts]
            s = min(trial, key=lambda t: t.alpha * t.S + 0.5 * t.chi2)
            if x is not None and s.chi2 > 2.0 * M and il % 4 == 0:
                # possibly trapped in a wrong branch: try the other starting points as well
                for x0 in cands:
                    t = self.solve(np.exp(la), r_max, x0=x0, reg=reg)
                    if t.alpha * t.S + 0.5 * t.chi2 < s.alpha * s.S + 0.5 * s.chi2:
                        s = t
            sols.append(s)
            x = s.x
            best = max(best, s.log_evidence)
            # stop only once the data are fitted (chi2 ~ M) and the evidence has clearly dropped
            if s.chi2 < 1.5 * M and s.log_evidence < best - stop_drop:
                break
        return sols

    def best_alpha(self, r_max, path=None):
        """Maximum-evidence alpha at fixed r_max (path + bounded refinement)."""
        path = self.alpha_path(r_max) if path is None else path
        le = np.array([s.log_evidence for s in path])
        i = int(np.argmax(le))
        if 0 < i < len(path) - 1:
            reg = self.regularizer(r_max)
            x0 = path[i].x
            cache = {}

            def f(la):
                s = self.solve(np.exp(la), r_max, x0=x0, reg=reg)
                cache[la] = s
                return -s.log_evidence

            lo, hi = np.log(path[i + 1].alpha), np.log(path[i - 1].alpha)
            r = optimize.minimize_scalar(f, bounds=(lo, hi), method="bounded", options={"xatol": 1e-3})
            s = cache.get(r.x) or self.solve(np.exp(r.x), r_max, x0=x0, reg=reg)
            if s.log_evidence > path[i].log_evidence:
                return s, path
        return path[i], path

    def guess_r_max(self) -> float:
        """r_max from a Guinier fit (sphere: sqrt(5/3) Rg; cylinder: sqrt(2) Rc; lamella: sqrt(3) Rt)."""
        q, I, s = self.q, self.I, self.sigma
        m = I > 0
        q, I, s = q[m], I[m], s[m]
        k = {"sphere": 0, "cylinder": 1, "lamella": 2}[self.geometry]
        y = np.log(I * q**k)
        fac = {"sphere": 3.0, "cylinder": 2.0, "lamella": 1.0}[self.geometry]
        rg = 1.0 / q.min()
        for _ in range(30):
            sel = q * rg < 1.3
            if sel.sum() < 4:
                sel = np.arange(q.size) < 4
            c = np.polyfit(q[sel] ** 2, y[sel], 1, w=(I[sel] / s[sel]))
            rg_new = np.sqrt(max(-fac * c[0], 1e-20))
            if abs(rg_new - rg) < 1e-5 * rg:
                break
            rg = rg_new
        return rg * {"sphere": np.sqrt(5 / 3), "cylinder": np.sqrt(2.0), "lamella": np.sqrt(3.0)}[self.geometry]

    def scan_r_max(self, r_max_values, verbose=False):
        """Evidence over a grid of r_max (alpha marginalised along each path).

        Returns (r_max_values, posterior, best_solutions, all_solutions).
        """
        r_max_values = np.asarray(r_max_values, float)
        logZ, bests, alls = [], [], []
        for rm in r_max_values:
            sbest, path = self.best_alpha(rm)
            le = np.array([s.log_evidence for s in path])
            # marginalise over log alpha (trapezoid on the uniform path grid)
            dla = abs(np.log(path[0].alpha) - np.log(path[1].alpha)) if len(path) > 1 else 1.0
            mx = le.max()
            logZ.append(mx + np.log(np.sum(np.exp(le - mx)) * dla))
            bests.append(sbest)
            alls.extend(path + [sbest])
            self._report(
                f"r_max={rm:.5g}  log alpha={np.log(sbest.alpha):.3f}  logEv={sbest.log_evidence:.6g}  "
                f"chi2/M={sbest.chi2 / self.q.size:.3f}  conv={sbest.converged}",
                verbose,
            )
        logZ = np.array(logZ)
        post = np.exp(logZ - logZ.max())
        if r_max_values.size > 1:
            post /= np.trapezoid(post, r_max_values)
        return r_max_values, post, bests, alls

    def scan_exponent(self, p_values, r_max, verbose=False):
        """Evidence for different representations r^p rho at fixed r_max.

        The exponent changes the prior (what is assumed smooth), so the
        Bayesian evidence can be used to compare them. Returns
        (p_values, log_evidence, best_solutions). The fitter's own ``basis``
        is restored afterwards.
        """
        old, out_le, out_s = self.basis, [], []
        try:
            for p in p_values:
                geo._check_basis(float(p), self.geometry)
                self.basis = float(p)
                self._cache.clear()
                sol, _ = self.best_alpha(r_max)
                out_le.append(sol.log_evidence)
                out_s.append(sol)
                self._report(f"p={p:.4g}  logEv={sol.log_evidence:.6g}  chi2/M={sol.chi2 / self.q.size:.3f}  Rg={sol.Rg():.5g}", verbose)
        finally:
            self.basis = old
            self._cache.clear()
        return np.asarray(p_values, float), np.array(out_le), out_s

    def _posterior_from(self, rg, bests, alls):
        """Posterior over r_max from stored alpha paths (alpha marginalised)."""
        logZ = []
        for rm in rg:
            path = [s for s in alls if s.r_max == rm]
            le = np.array([s.log_evidence for s in path])
            al = np.log([s.alpha for s in path])
            o = np.argsort(al)
            le, al = le[o], al[o]
            mx = le.max()
            dla = np.median(np.diff(al)) if al.size > 1 else 1.0
            logZ.append(mx + np.log(np.sum(np.exp(le - mx)) * dla))
        logZ = np.array(logZ)
        post = np.exp(logZ - logZ.max())
        if rg.size > 1:
            post /= np.trapezoid(post, rg)
        return post

    def fit(self, r_max=None, r_max_range=None, n_coarse=15, n_fine=15, fix_r_max=False, r_out=None, n_draws=40, seed=0, verbose=False, max_extend=6) -> ProfileResult:
        """Full Bayesian estimate.

        r_max : starting value / fixed value (``fix_r_max=True``); default from a Guinier fit
        r_max_range : (lo, hi) of the coarse r_max grid; default (0.7, 1.5) * r_max
        max_extend : how often the coarse grid may be extended upwards when the
            evidence maximum lies at its upper end (e.g. coil-like profiles, which
            extend to 3-4 Rg while the default start assumes a homogeneous sphere)
        """
        rm0 = self.guess_r_max() if r_max is None else float(r_max)
        if fix_r_max:
            grid = np.array([rm0])
            rg, post, bests, alls = self.scan_r_max(grid, verbose)
        else:
            lo, hi = (0.7 * rm0, 1.5 * rm0) if r_max_range is None else r_max_range
            grid = np.linspace(lo, hi, n_coarse)
            rg, post, bests, alls = self.scan_r_max(grid, verbose)
            # extend the scan while the evidence maximum sits at the upper edge
            for _ in range(max_extend):
                if int(np.argmax(post)) < rg.size - 2:
                    break
                step = rg[1] - rg[0]
                ext = rg[-1] + step * np.arange(1, max(n_coarse // 2, 3) + 1)
                self._report(f"evidence maximum at upper end of the r_max scan -> extending to {ext[-1]:.5g}", verbose)
                rg2, _, bests2, alls2 = self.scan_r_max(ext, verbose)
                # recompute the posterior on the union (log Z is not returned; rescan is cheap enough)
                rg = np.r_[rg, rg2]
                bests = bests + bests2
                alls = alls + alls2
                post = self._posterior_from(rg, bests, alls)
            # fine grid around the maximum
            i = int(np.argmax(post))
            mean = np.trapezoid(post * rg, rg)
            std = np.sqrt(max(np.trapezoid(post * (rg - mean) ** 2, rg), 0))
            step = rg[1] - rg[0]
            half = max(4 * std, 1.5 * step)
            fine = np.linspace(max(rg[i] - half, 1e-6), rg[i] + half, n_fine)
            self._report(f"coarse r_max = {mean:.5g} +/- {std:.2g}; refining on [{fine[0]:.5g}, {fine[-1]:.5g}]", verbose)
            rg, post, bests, alls = self.scan_r_max(fine, verbose)

        sols = alls
        le = np.array([s.log_evidence for s in sols])
        w = np.exp(le - le.max())
        keep = w > 1e-6
        sols = [s for s, k in zip(sols, keep) if k]
        w = w[keep] / w[keep].sum()
        best = max(bests, key=lambda s: s.log_evidence)

        if rg.size > 1:
            m = np.trapezoid(post * rg, rg)
            sd = np.sqrt(max(np.trapezoid(post * (rg - m) ** 2, rg), 0))
        else:
            m, sd = rg[0], 0.0
        r_hi = max(s.r_max for s in sols)
        if r_out is None:
            # cell centres (avoids r = 0, where rho diverges for basis='mass')
            e = np.linspace(0, 1.05 * r_hi, 301)
            r = 0.5 * (e[1:] + e[:-1])
        else:
            r = np.asarray(r_out, float)

        rng = np.random.default_rng(seed)
        prof, mass, rgs, bgs, las, ws, bth = [], [], [], [], [], [], []
        for s, wi in zip(sols, w):
            nn = s.n
            xv = s.x
            nt = xv.size
            try:
                C = s.cov[:nt, :nt]
                Lc = linalg.cholesky(C + 1e-15 * np.trace(C) / nt * np.eye(nt), lower=True)
                draws = xv[None, :] + rng.standard_normal((n_draws, nt)) @ Lc.T
            except linalg.LinAlgError:
                draws = np.repeat(xv[None, :], n_draws, axis=0)
            draws[0] = xv
            for d_ in draws:
                prof.append(s.rho_on(r, d_[:nn]))
                mass.append(s.mass_on(r, d_[:nn]))
                rgs.append(s.Rg(d_[:nn]))
                bgs.append(self.bg.constant(d_[nn:]))
                bth.append(self.bg.theta(d_[nn:]))
            las.extend([np.log(s.alpha)] * n_draws)
            ws.extend([wi / n_draws] * n_draws)
        ws = np.array(ws)

        def ms(a):
            a = np.asarray(a, float)
            ok = np.isfinite(a) if a.ndim == 1 else np.ones(a.shape[0], bool)
            ww = ws[ok] / ws[ok].sum()
            mu = np.einsum("i,i...->...", ww, a[ok])
            var = np.einsum("i,i...->...", ww, (a[ok] - mu) ** 2)
            return mu, np.sqrt(var)

        rho_m, rho_s = ms(np.array(prof))
        mass_m, mass_s = ms(np.array(mass))
        return ProfileResult(
            best=best, r=r, rho=rho_m, rho_err=rho_s, r_max=(float(m), float(sd)),
            log_alpha=tuple(float(v) for v in ms(las)), Rg=tuple(float(v) for v in ms(rgs)),
            background=tuple(float(v) for v in ms(bgs)), r_max_grid=rg, r_max_posterior=post,
            q=self.q, I_fit=best.I_fit, chi2_red=best.chi2 / self.q.size,
            regularization=self.regularization, geometry=self.geometry, solutions=sols, weights=w,
            mass=mass_m, mass_err=mass_s,
            bg_params={nm: (float(mu_), float(sd_), fr) for nm, mu_, sd_, fr in zip(
                self.bg.names, *ms(np.array(bth)) if bth and len(self.bg.names) else ([], []),
                [v[1] for v in self.bg.describe().values()])},
            bg_curve=best.bg_curve,
        )
