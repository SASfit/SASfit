r"""Scattering amplitudes of symmetric density profiles (Hansen 2016, eqs. 1-5).

The radial density (scattering-length-density contrast) profile rho(r),
0 <= r <= r_max, is represented by N piecewise-constant shells with edges
r_k = k * r_max / N.  For each geometry the orientationally averaged
intensity of a centro-symmetric scatterer is the square of a *real*
amplitude that is linear in rho:

    F(q) = sum_k a_k(q) rho_k ,          I(q) = c_dim(q) F(q)^2

=============  ====================================================  ============
geometry        a_k(q) = integral over shell k                        c_dim(q)
=============  ====================================================  ============
``sphere``      4 pi \int r^2 sin(qr)/(qr) dr                         1
``cylinder``    2 pi \int r J0(qr) dr          (infinitely long)      pi / q
``lamella``     2 \int cos(qx) dx              (infinitely extended)  2 pi / q^2
=============  ====================================================  ============

``cylinder`` is per unit length, ``lamella`` per unit area.

Hansen (2016) writes the same model as I_i = rho^T C_i rho (eq. 11) with
C_ikl = sum_j A_ij V_jkl, i.e. via the distance distribution p(r) and the
overlap integrals V of Glatter (1981).  For a centro-symmetric scatterer the
q-dependent part of C_i is exactly the rank-one matrix c_dim a(q_i) a(q_i)^T,
which is what is used here (exact shell integrals, no p(r) discretisation
error).  Instrumental smearing turns C_i into a sum of such rank-one terms
(see :mod:`pysldprofile.resolution`).

The distance distribution p(r) = r^(dim-1) gamma(r) (eq. 4, 5) is still
available from :func:`pddf` for output and comparison with ordinary IFT.
"""

from __future__ import annotations

import numpy as np
from scipy import special

__all__ = ["GEOMETRIES", "DIM", "BASES", "exponent", "coef_to_rho", "coef_to_mass", "shell_edges", "uniform_grid", "power_grid", "amplitude_matrix", "prefactor", "pddf", "radius_of_gyration", "volume_weights"]

GEOMETRIES = ("sphere", "cylinder", "lamella")
DIM = {"lamella": 1, "cylinder": 2, "sphere": 3}


def _check(geometry):
    if geometry not in GEOMETRIES:
        raise ValueError(f"geometry must be one of {GEOMETRIES}, got {geometry!r}")


def shell_edges(r_max: float, n: int, grid=None) -> np.ndarray:
    """Shell edges. ``grid`` (optional) are normalised edges 0 = t_0 < ... < t_n = 1
    (non-uniform grids); default uniform."""
    if grid is None:
        return np.linspace(0.0, r_max, n + 1)
    t = np.asarray(grid, float)
    if t.size != n + 1:
        raise ValueError(f"grid has {t.size} edges, expected {n + 1}")
    return r_max * t


def uniform_grid(n: int) -> np.ndarray:
    return np.linspace(0.0, 1.0, n + 1)


def power_grid(n: int, gamma: float) -> np.ndarray:
    """t_k = (k/n)^gamma: gamma > 1 refines towards the centre, gamma < 1 towards r_max."""
    return (np.arange(n + 1) / n) ** float(gamma)


def _primitive(geometry, q, r, basis="rho"):
    """Antiderivative G(q, r) of the shell integrand, vectorised (q[:,None], r[None,:])."""
    q = np.asarray(q, float)[:, None]
    r = np.asarray(r, float)[None, :]
    x = q * r
    if basis == "mass" and geometry != "lamella":
        # coefficient g = r^(dim-1) rho is constant within a shell (rho ~ g / r^(dim-1))
        small = np.abs(x) < 1e-3
        xs = np.where(small, 1.0, x)
        if geometry == "sphere":
            # 4 pi int sin(qr)/(qr) dr = 4 pi Si(qr)/q = 4 pi r Si(x)/x
            f = np.where(small, 1.0 - x**2 / 18.0, special.sici(xs)[0] / xs)
            return 4.0 * np.pi * r * f
        # cylinder: 2 pi int J0(qr) dr = 2 pi r (int_0^x J0)/x
        f = np.where(small, 1.0 - x**2 / 12.0, special.itj0y0(xs)[0] / xs)
        return 2.0 * np.pi * r * f
    if geometry == "sphere":
        # 4 pi (sin x - x cos x)/q^3 = 4 pi r^3 * (sin x - x cos x)/x^3
        small = np.abs(x) < 1e-2
        xs = np.where(small, 1.0, x)
        f = np.where(small, 1.0 / 3.0 - x**2 / 30.0 + x**4 / 840.0, (np.sin(xs) - xs * np.cos(xs)) / xs**3)
        return 4.0 * np.pi * r**3 * f
    if geometry == "cylinder":
        # 2 pi r J1(qr)/q = 2 pi r^2 J1(x)/x
        small = np.abs(x) < 1e-4
        xs = np.where(small, 1.0, x)
        f = np.where(small, 0.5 - x**2 / 16.0, special.j1(xs) / xs)
        return 2.0 * np.pi * r**2 * f
    # lamella: 2 sin(qx)/q = 2 r sinc
    return 2.0 * r * np.sinc(x / np.pi)


BASES = ("rho", "mass")


def exponent(geometry: str, basis) -> float:
    """Exponent p of the represented quantity r^p rho(r).

    ``'rho'`` -> 0, ``'mass'`` -> dim-1, a number -> that number.
    """
    if isinstance(basis, str):
        if basis == "rho":
            return 0.0
        if basis == "mass":
            return float(DIM[geometry] - 1)
        raise ValueError(f"basis must be 'rho', 'mass' or a number, got {basis!r}")
    return float(basis)


def _check_basis(basis, geometry="sphere"):
    p = exponent(geometry, basis)
    if not p < DIM[geometry]:
        raise ValueError(f"exponent p = {p} must be < dim = {DIM[geometry]} (otherwise the scattering length diverges at r = 0)")


_ANG = {"sphere": 4.0 * np.pi, "cylinder": 2.0 * np.pi, "lamella": 2.0}


def _kernel(geometry, x):
    if geometry == "sphere":
        return np.sinc(x / np.pi)
    if geometry == "cylinder":
        return special.j0(x)
    return np.cos(x)


def _amplitude_numeric(q, r_max, n, geometry, p, n_gl=16, grid=None):
    """Shell integrals  A_d int_shell r^(d-1-p) K(qr) dr  by Gauss quadrature.

    The first shell uses Gauss-Jacobi with the weight r^(d-1-p) (integrable
    singularity for p > d-1), the others Gauss-Legendre.
    """
    q = np.asarray(q, float)
    e = shell_edges(r_max, n, grid)
    beta = DIM[geometry] - 1 - p
    out = np.empty((q.size, n))
    # first shell [0, e1]: r = e1 (1+x)/2, int r^beta f dr = (e1/2)^(beta+1) sum w f
    xj, wj = special.roots_jacobi(n_gl, 0.0, beta)
    rj = e[1] * (1 + xj) / 2
    out[:, 0] = (e[1] / 2) ** (beta + 1) * (_kernel(geometry, np.outer(q, rj)) @ wj)
    xl, wl = np.polynomial.legendre.leggauss(n_gl)
    a, b = e[1:-1], e[2:]
    rr = 0.5 * (b - a)[:, None] * xl[None, :] + 0.5 * (b + a)[:, None]  # (n-1, n_gl)
    ww = 0.5 * (b - a)[:, None] * wl[None, :] * rr**beta
    K = _kernel(geometry, q[:, None, None] * rr[None])  # (nq, n-1, n_gl)
    out[:, 1:] = np.einsum("qkg,kg->qk", K, ww)
    return _ANG[geometry] * out


def amplitude_matrix(q, r_max: float, n: int, geometry: str = "sphere", basis: str = "rho", grid=None) -> np.ndarray:
    """Matrix a (len(q) x n) with F(q) = a @ c for piecewise-constant shell coefficients c.

    ``basis='rho'``: c_k = rho on shell k (Hansen 2016).
    ``basis='mass'``: c_k = r^(dim-1) rho(r) on shell k ("radial mass", scattering length
    per unit radius up to the angular factor). Inside a shell rho ~ 1/r^(dim-1), so a
    centre cusp rho ~ 1/r^2 (sphere), as needed for coil-like I ~ q^-2 without form-factor
    zeros, is represented exactly.
    ``basis=p`` (a number < dim): c_k = r^p rho(r); e.g. p = 4/3 for a Daoud-Cotton star
    profile rho ~ r^(-4/3). p = 0 and p = dim-1 use the closed forms above, other p
    Gauss(-Jacobi) quadrature.
    """
    _check(geometry)
    _check_basis(basis, geometry)
    p = exponent(geometry, basis)
    d = DIM[geometry]
    if p == 0.0:
        return np.diff(_primitive(geometry, q, shell_edges(r_max, n, grid), "rho"), axis=1)
    if p == d - 1:
        return np.diff(_primitive(geometry, q, shell_edges(r_max, n, grid), "mass"), axis=1)
    # general exponent: c_k = r^p rho constant in shell k  ->  rho = c_k / r^p
    return _amplitude_numeric(q, r_max, n, geometry, p, grid=grid)


def prefactor(q, geometry: str = "sphere") -> np.ndarray:
    """c_dim(q) with I = c_dim F^2."""
    _check(geometry)
    q = np.asarray(q, float)
    if geometry == "sphere":
        return np.ones_like(q)
    if geometry == "cylinder":
        return np.pi / q
    return 2.0 * np.pi / q**2


def volume_weights(r_max: float, n: int, geometry: str = "sphere", basis: str = "rho", grid=None) -> np.ndarray:
    """Shell weights (the q -> 0 limit of a_k): F(0) = weights @ c."""
    return amplitude_matrix(np.array([0.0]), r_max, n, geometry, basis, grid)[0]


def coef_to_rho(c, r, r_max: float, geometry: str = "sphere", basis: str = "rho", grid=None):
    """rho(r) of the step representation (0 beyond r_max)."""
    c = np.asarray(c, float)
    r = np.asarray(r, float)
    n = c.size
    if grid is None:
        k = np.floor(r / r_max * n).astype(int)
    else:
        k = np.searchsorted(shell_edges(r_max, n, grid), r, side="right") - 1
    out = np.zeros_like(r)
    m = (k >= 0) & (r <= r_max)
    out[m] = c[np.clip(k[m], 0, n - 1)]
    p = exponent(geometry, basis)
    if p != 0.0:
        with np.errstate(divide="ignore", invalid="ignore"):
            rp = np.where(r > 0, r, np.nan) ** p
            out = np.where(r > 0, out / rp, np.where((out != 0) & (p > 0), np.inf * np.sign(out), out))
    return out


def coef_to_mass(c, r, r_max: float, geometry: str = "sphere", basis="rho", grid=None):
    """r^(dim-1) rho(r) of the step representation."""
    r = np.asarray(r, float)
    d = DIM[geometry]
    p = exponent(geometry, basis)
    with np.errstate(divide="ignore", invalid="ignore"):
        fac = np.where(r > 0, np.where(r > 0, r, 1.0) ** (d - 1 - p), 0.0 if d - 1 - p > 0 else 1.0)
    return coef_to_rho(c, r, r_max, geometry, "rho", grid) * fac


def radius_of_gyration(rho, r_max: float, geometry: str = "sphere", basis="rho", grid=None) -> float:
    """Rg of the 3-D object (sphere), of the cross-section (cylinder) or of the thickness (lamella)."""
    rho = np.asarray(rho, float)
    e = shell_edges(r_max, rho.size, grid)
    d = DIM[geometry]
    # integral of r^2 * r^(d-1) and r^(d-1) over each shell (common angular factors cancel)
    p = exponent(geometry, basis)
    # rho = c / r^p:  int r^(d-1-p) dr and int r^(d+1-p) dr over each shell
    m0 = (e[1:] ** (d - p) - e[:-1] ** (d - p)) / (d - p)
    m2 = (e[1:] ** (d + 2 - p) - e[:-1] ** (d + 2 - p)) / (d + 2 - p)
    num = np.sum(rho * m2)
    den = np.sum(rho * m0)
    return float(np.sqrt(num / den)) if den != 0 and num / den > 0 else float("nan")


def pddf(rho, r_max: float, geometry: str = "sphere", r=None, n_sub: int = 8, basis: str = "rho", grid=None):
    """Distance distribution p(r) = r^(dim-1) gamma(r)  (Hansen 2016, eqs. 4-5).

    ``gamma`` is the (dim-dimensional) autocorrelation of the density.  The
    normalisation is such that, for the sphere, I(q) = 4 pi int p(r) sin(qr)/(qr) dr.
    Returns (r, p).
    """
    _check(geometry)
    rho = np.asarray(rho, float)
    n = rho.size
    D = 2.0 * r_max
    if r is None:
        r = np.linspace(0, D, 2 * n + 1)
    r = np.asarray(r, float)
    # sub-sample shells into thin sub-shells
    m = n * n_sub
    e = shell_edges(r_max, m)
    rc = 0.5 * (e[1:] + e[:-1])
    # rho on uniform thin sub-shells (handles any representation and non-uniform grids)
    rr = coef_to_rho(rho, rc, r_max, geometry, basis, grid)
    if geometry == "sphere":
        # two thin shells s,t:  F_s F_t = (4 pi s^2 ds)(4 pi t^2 dt) sinc(qs) sinc(qt),
        # sinc(qs) sinc(qt) = 1/(2st) int_{|s-t|}^{s+t} x sinc(qx) dx
        # => 4 pi p(x) = sum rho_s rho_t (4 pi s^2 ds)(4 pi t^2 dt) x/(2 s t) on |s-t|<x<s+t
        ds = e[1] - e[0]
        ws = rr * 4 * np.pi * rc**2 * ds
        S, T = np.meshgrid(rc, rc, indexing="ij")
        W = np.outer(ws, ws) / (2 * S * T)
        lo, hi = np.abs(S - T).ravel(), (S + T).ravel()
        W = W.ravel()
        # accumulate W over [lo, hi] using a difference array on the r grid, then multiply by r
        rg = np.linspace(0, D, 4 * m + 1)
        acc = np.zeros(rg.size + 1)
        il = np.searchsorted(rg, lo)
        ih = np.searchsorted(rg, hi)
        np.add.at(acc, il, W)
        np.add.at(acc, ih, -W)
        cum = np.cumsum(acc)[:-1]
        p = rg * cum / (4 * np.pi)
        return r, np.interp(r, rg, p)
    if geometry == "lamella":
        # symmetric 1-D profile on [-r_max, r_max]; p(r) = gamma(r) = int rho(x) rho(x+r) dx
        dx = e[1] - e[0]
        full = np.r_[rr[::-1], rr]
        g = np.correlate(full, full, mode="full")[full.size - 1 :] * dx
        rg = np.arange(g.size) * dx
        return r, np.interp(r, rg, g, right=0.0)
    # cylinder: 2-D autocorrelation by FFT on a square raster, radially averaged
    npx = 256
    L = 2 * r_max
    xs = (np.arange(npx) - npx / 2 + 0.5) * (L / npx)
    X, Y = np.meshgrid(xs, xs)
    R = np.hypot(X, Y)
    img = np.interp(R, rc, rr, right=0.0) * (R <= r_max)
    pad = np.zeros((2 * npx, 2 * npx))
    pad[:npx, :npx] = img
    Fk = np.fft.rfft2(pad)
    ac = np.fft.irfft2(np.abs(Fk) ** 2, s=pad.shape) * (L / npx) ** 2
    ac = np.fft.fftshift(ac)
    c = npx
    yy, xx = np.indices(ac.shape)
    dist = np.hypot(xx - c, yy - c) * (L / npx)
    bins = np.linspace(0, D, 2 * n + 2)
    idx = np.digitize(dist.ravel(), bins) - 1
    ok = (idx >= 0) & (idx < bins.size - 1)
    sums = np.bincount(idx[ok], ac.ravel()[ok], minlength=bins.size - 1)
    cnt = np.bincount(idx[ok], minlength=bins.size - 1)
    gam = np.where(cnt > 0, sums / np.maximum(cnt, 1), 0.0)
    rb = 0.5 * (bins[1:] + bins[:-1])
    return r, np.interp(r, rb, rb * gam, right=0.0)
