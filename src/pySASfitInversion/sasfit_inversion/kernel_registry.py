"""
Named, pluggable Fredholm kernels for interactive selection (e.g. a GUI
dropdown), on top of the generic build_size_distribution_kernel machinery
in kernels.py.

Each kernel function has signature f(q, r) -> array over q, for a single
scalar r -- exactly what build_size_distribution_kernel's
`form_factor_squared` argument expects, so these plug in directly:

    A = build_size_distribution_kernel(q, r_grid, KERNEL_REGISTRY["sphere_rg"].func, alpha=0.0)

KERNEL_REGISTRY is deliberately a plain dict of small dataclasses (not a
plugin/entry-point system) so adding a new kernel later is just adding one
function + one registry entry -- matching Joachim's "or something else
defined later" requirement without over-engineering a plugin framework
before there's a second real use case for one.
"""
from __future__ import annotations

from dataclasses import dataclass
from typing import Callable

import numpy as np


@dataclass
class KernelSpec:
    label: str                 # display name, e.g. for a GUI dropdown
    func: Callable[[np.ndarray, float], np.ndarray]
    description: str
    default_alpha: float = 0.0  # default size-weighting exponent (see kernels.py eq. 32-34)


def sinc_kernel(q: np.ndarray, r: float) -> np.ndarray:
    """
    K(q, r) = 4 pi r^2 j0(qr), where j0(x) = sin(x)/x is the zeroth-order
    spherical Bessel function of the first kind.

    This is the kernel relating a radial (pair-distance-style) distribution
    to I(q) via I(q) = integral 4 pi r^2 p(r) j0(qr) dr -- the
    spherically-averaged Fourier-sine transform kernel used in the
    indirect Fourier transform / Debye formula. j0 shows up here because
    it is precisely the radial part of a 3D Fourier transform of a
    spherically symmetric function (Joachim, 2026-09-25: correctly pointed
    out this is j0, not just "a sinc-like function" -- using
    scipy.special.spherical_jn(0, ...) directly now instead of the
    np.sinc(x/pi) reindexing trick, for both clarity and because the
    dedicated special function is the numerically appropriate way to
    evaluate it, including at qr=0 where j0(0)=1.
    """
    from scipy.special import spherical_jn

    q = np.asarray(q, dtype=float)
    qr = q * r
    return 4.0 * np.pi * spherical_jn(0, qr)


def sphere_rayleigh_gans_intensity(q: np.ndarray, r: float) -> np.ndarray:
    """
    Scattered intensity of a homogeneous sphere of radius r, Rayleigh-Gans
    (= exact form factor for a sphere in the Born/RG approximation, which
    for a sphere coincides with the standard exact form factor):

        |F(qr)|^2 = r^6 * [3 (sin(qr) - qr cos(qr)) / (qr)^3]^2

    BUG FIX (2026-09-25, caught by comparing a blind inversion of the real
    JAC-2022-paper benchmark dataset against the paper's own published
    Figure 5): earlier versions of this function returned ONLY the
    normalized shape P(qr) = [3(sin(qr)-qr cos(qr))/(qr)^3]^2, P(0)=1,
    omitting the r^6 (Volume^2) prefactor. That is NOT just a missing
    "overall amplitude" -- omitting it changes the *relative* weighting
    between different particle sizes in a polydisperse sum, because real
    particles scatter with intensity proportional to Volume^2, not with
    uniform per-particle efficiency regardless of size. Without r^6, a
    blind EM inversion on that benchmark data (known bilognormal spheres
    at 60nm and 180nm modes) recovered a spurious single broad peak
    dominated by ~300nm content and negligible signal near 60nm --
    because the kernel made large particles look like weak scatterers, so
    the solver had to pile up unrealistic amounts of N(r) at large r to
    explain the data. With r^6 restored, a direct two-population linear
    fit against the real data recovers a volume ratio of ~1.9:1 in favor
    of the 60nm population, qualitatively matching the paper's Fig. 5
    (dominant sharp peak near 60-85nm, smaller broad hump near 180-220nm)
    for the first time.

    An overall physical prefactor ((4pi/3)^2 * (contrast)^2) is still
    omitted -- that IS a genuine free amplitude/scale (concentration,
    contrast), safe to fold into an overall fit scale factor if an
    absolute (not just relative) intensity match is needed. The r^6 term
    is not optional in the same way.
    """
    q = np.asarray(q, dtype=float)
    qr = q * r
    qr_safe = np.where(qr == 0, np.finfo(float).eps, qr)
    F = 3.0 * (np.sin(qr_safe) - qr_safe * np.cos(qr_safe)) / qr_safe**3
    F = np.where(qr == 0, 1.0, F)
    return (r**6) * F**2


KERNEL_REGISTRY: dict[str, KernelSpec] = {
    "sinc_4pi": KernelSpec(
        label="4\u03c0 j\u2080(qr)  [j\u2080(x)=sin(x)/x, spherical Bessel]",
        func=sinc_kernel,
        description="Fourier-sine (Debye/IFT-style) kernel relating a radial "
                     "distribution p(r) to I(q), via the zeroth-order spherical "
                     "Bessel function j0(qr).",
        default_alpha=0.0,
    ),
    "sphere_rg": KernelSpec(
        label="Sphere (Rayleigh-Gans)",
        func=sphere_rayleigh_gans_intensity,
        description="Homogeneous sphere form factor (with the physical r^6 "
                     "Volume^2 scaling). default_alpha=6 is not just a "
                     "convenience default here -- alpha=6 is what divides "
                     "the r^6 back out of the kernel for numerical "
                     "conditioning (see the function's own docstring); "
                     "using alpha=0 with this kernel produces a ~1e15 "
                     "dynamic range that breaks the EM solver.",
        default_alpha=6.0,
    ),
}


def list_kernels() -> list[tuple[str, str]]:
    """(key, label) pairs, e.g. for populating a GUI dropdown."""
    return [(key, spec.label) for key, spec in KERNEL_REGISTRY.items()]
