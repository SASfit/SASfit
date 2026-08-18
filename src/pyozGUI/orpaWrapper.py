# -*- coding: utf-8 -*-
'''
Thin ctypes wrapper around SASfit's own square-well ORPA (Optimized
Random Phase Approximation) C library
(src/plugins/hs_plus_shoulder_or_well/{sasfit_sq_squarewell_nonlinear_orpa.c,
squarewell_common.c} in SASfitGit), compiled here as a standalone
shared library with no SASfit-specific dependencies.

Theory: Pini, Parola & Reatto, "A simple approximation for fluids
with narrow attractive potentials," Molecular Physics 100, 1507
(2002); arXiv:cond-mat/0109311. Solves the paper's self-consistency
condition via a projected polynomial correction and GSL's derivative-
free multiroot solver, with continuation in the well depth for
robustness -- a genuinely different theory from RFA (below) or the
Sharma-Sharma first-order perturbative square-well formula already in
oZsolver.py, giving an independent third square-well benchmark.

Requires liborpa.dll (Windows) or liborpa.so (Linux/WSL) to be built
first from the sources in ./orpa_c_source/ next to this file:
        cd orpa_c_source
        gcc -shared -fPIC -O2 -o ../liborpa.so \
            sw_nonlinear_orpa_core.c squarewell_common.c -I. -lgsl -lgslcblas -lm

IMPORTANT (per the original header's own documented validation): a
converged solve does not guarantee a PHYSICAL one -- always check
S(Q)>=0 on the result. The paper's own abstract states no solution
exists inside the liquid-vapor coexistence region, and the original
header notes this solver's own breakdown into unphysical (negative)
S(Q) at low density lines up closely with the literature critical
temperature for lambda=1.5 -- i.e. that breakdown is a genuine
physical feature of the theory, not a bug to work around.

Cross-validated against this tool's own numerical SquareWell+PY solve
(see the conversation this was built in): at a "strong well" test case
(phi=0.15, epsilon=-0.5kT, delta=0.3*sigma) where the simpler Sharma-
Sharma perturbative formula (calculateAnalyticalStructureFactorForSquareWell
in oZsolver.py) showed max diff 0.33/mean 0.10 against the numerical
solve, ORPA gave max diff 0.057/mean 0.013 -- roughly 6x closer,
consistent with ORPA being a genuinely non-perturbative, self-
consistent theory rather than a first-order expansion.
'''
import ctypes
import os
import numpy as np

_lib = None
_lib_error = None


def _load_lib():
    global _lib, _lib_error
    if _lib is not None or _lib_error is not None:
        return
    here = os.path.dirname(os.path.abspath(__file__))
    candidates = [os.path.join(here, "liborpa.dll"), os.path.join(here, "liborpa.so"),
                  os.path.join(here, "liborpa.dylib")]
    for path in candidates:
        if os.path.exists(path):
            try:
                _lib = ctypes.CDLL(path)
                break
            except OSError as e:
                _lib_error = str(e)
    if _lib is None and _lib_error is None:
        _lib_error = ("liborpa.dll/.so/.dylib not found next to orpaWrapper.py -- "
                       "build it first from orpa_c_source/, see this file's own docstring.")
    if _lib is not None:
        _lib.sw_nonlinear_orpa_create.argtypes = [ctypes.c_double, ctypes.c_double,
                                                   ctypes.c_double, ctypes.c_int]
        _lib.sw_nonlinear_orpa_create.restype = ctypes.c_void_p
        _lib.sw_nonlinear_orpa_free.argtypes = [ctypes.c_void_p]
        _lib.sw_nonlinear_orpa_solve.argtypes = [ctypes.c_void_p, ctypes.c_double,
                                                  ctypes.c_int, ctypes.c_double]
        _lib.sw_nonlinear_orpa_solve.restype = ctypes.c_int
        _lib.sw_nonlinear_orpa_S.argtypes = [ctypes.c_void_p, ctypes.c_double]
        _lib.sw_nonlinear_orpa_S.restype = ctypes.c_double


def orpa_compute(phi, lam, eps_kT, q, k_max=60.0, n_k=6000,
                  n_continuation_steps=15, eps_kT_start=0.05):
    '''
    phi: volume fraction
    lam: well outer range in units of sigma (well width = (lam-1)*sigma)
    eps_kT: attractive well depth in kT units (eps_kT>0 = attractive,
        matching this library's own convention -- NOT the same sign
        convention as this tool's own setSquareWellPotential(), which
        uses epsilonInkTUnits<0 for attractive; negate when comparing)
    q: 1D array-like of dimensionless scattering vectors (q*sigma)
    k_max, n_k: integration grid range/density for the k-space
        self-consistency condition (defaults match the original
        sasfit_sq_square_well_nonlin_orpa()'s own choices)
    n_continuation_steps, eps_kT_start: well-depth continuation
        schedule for robustness (defaults match the original)

    Returns (structureFactor: np.ndarray same shape as q, ok: bool).
    Does NOT check S(Q)>=0 itself -- see this module's own docstring
    for why a converged-but-unphysical result is a real, expected
    possibility with this theory, not a bug; check the sign yourself.
    '''
    _load_lib()
    if _lib is None:
        raise RuntimeError(_lib_error)

    m = _lib.sw_nonlinear_orpa_create(ctypes.c_double(phi), ctypes.c_double(lam),
                                       ctypes.c_double(k_max), ctypes.c_int(n_k))
    try:
        ok = _lib.sw_nonlinear_orpa_solve(m, ctypes.c_double(eps_kT),
                                           ctypes.c_int(n_continuation_steps),
                                           ctypes.c_double(eps_kT_start))
        q = np.ascontiguousarray(q, dtype=np.float64)
        out = np.empty_like(q)
        for i, qi in enumerate(q):
            out[i] = _lib.sw_nonlinear_orpa_S(m, ctypes.c_double(qi))
        return out, bool(ok)
    finally:
        _lib.sw_nonlinear_orpa_free(m)
