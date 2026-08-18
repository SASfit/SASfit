# -*- coding: utf-8 -*-
'''
Thin ctypes wrapper around SASfit's own Rational-Function Approximation
(RFA) C library (src/plugins/RFA/rfa_piecewise.c in SASfitGit),
compiled here as a standalone shared library with no SASfit-specific
dependencies.

Theory: A. Santos, S.B. Yuste and M. Lopez de Haro, "Rational-function
approximation for fluids interacting via piece-wise constant
potentials," Condens. Matter Phys. 15, 23602 (2012), and its n=2
illustration in the same authors + M. Barcenas, P. Orea, J. Chem. Phys.
139, 074505 (2013). Handles an ARBITRARY number of piecewise-constant
steps (square wells and/or shoulders) on top of a hard core, so this
one library validates this tool's SquareWell (n=1) AND
PiecewiseConstantHS (n=3) potentials both -- reused directly via
ctypes rather than reimplemented, same reasoning as rmsaWrapper.py/
oneYukawaWrapper.py: the numerically-delicate q->0 series expansion
(to avoid catastrophic cancellation) and the aliasing-safe warm-start
logic are exactly the kind of already-debugged detail worth reusing,
not risking a subtly different reimplementation of.

Two flavours (only RFA1 is wrapped here, RFA2 is available in the C
library itself if needed later): RFA1 fixes the A_j coefficients at
their exact zero-density values and solves only for B_j (n unknowns,
continuity of the cavity function y(r)); RFA2 additionally solves for
A_j too (2n unknowns, continuity of y'(r) as well) -- more accurate
but a larger nonlinear system.

Requires librfa.dll (Windows) or librfa.so (Linux/WSL) to be built
first from the sources in ./rfa_c_source/ next to this file:
        cd rfa_c_source
        gcc -shared -fPIC -O2 -o ../librfa.so rfa_piecewise.c -I. -lgsl -lgslcblas -lm

Units: lengths in units of the hard-core diameter sigma=1 (so RADIUS=
0.5*sigma in this convention); q is dimensionless (q*sigma); eta is
the packing fraction (same as this tool's volumeDensity/phi).

Cross-validated against this tool's own numerical solves (see the
conversation this was built in): on the n=1 strong square-well case
(phi=0.15, epsilon=-0.5, delta=0.3, where Sharma-Sharma showed max diff
0.33 and ORPA showed 0.057), RFA1 gave max diff 0.025 -- the most
accurate of the three square-well benchmarks in this tool. On a genuine
n=3 shoulder-well-shoulder PiecewiseConstantHS potential (unique to
RFA among these benchmarks, since neither Sharma-Sharma nor ORPA
generalize past n=1), RFA1 gave max diff 0.0056/mean 0.0028 -- a
strong, independent cross-check of both the numerical potential and
this port.
'''
import ctypes
import os
import numpy as np


class PWPotential(ctypes.Structure):
    _fields_ = [
        ("n", ctypes.c_int),
        ("eta", ctypes.c_double),
        ("lambda_", ctypes.POINTER(ctypes.c_double)),
        ("beps", ctypes.POINTER(ctypes.c_double)),
    ]


class _complex_t(ctypes.Structure):
    # ABI-compatible with C99 `double complex` on all platforms this
    # project targets (two adjacent doubles, real then imaginary).
    _fields_ = [("re", ctypes.c_double), ("im", ctypes.c_double)]


class RFADerived(ctypes.Structure):
    _fields_ = [
        ("s", _complex_t * 3),
        ("Sp", _complex_t * 3),
        ("A0", ctypes.c_double), ("B0", ctypes.c_double),
        ("S1", ctypes.c_double), ("S2", ctypes.c_double), ("S3", ctypes.c_double),
    ]


class RFASolution(ctypes.Structure):
    _fields_ = [
        ("n", ctypes.c_int),
        ("eta", ctypes.c_double),
        ("lambda_", ctypes.POINTER(ctypes.c_double)),
        ("beps", ctypes.POINTER(ctypes.c_double)),
        ("A", ctypes.POINTER(ctypes.c_double)),
        ("B", ctypes.POINTER(ctypes.c_double)),
        ("d", RFADerived),
        ("converged", ctypes.c_int),
        ("newton_iters", ctypes.c_int),
    ]


_lib = None
_lib_error = None


def _load_lib():
    global _lib, _lib_error
    if _lib is not None or _lib_error is not None:
        return
    here = os.path.dirname(os.path.abspath(__file__))
    candidates = [os.path.join(here, "librfa.dll"), os.path.join(here, "librfa.so"),
                  os.path.join(here, "librfa.dylib")]
    for path in candidates:
        if os.path.exists(path):
            try:
                _lib = ctypes.CDLL(path)
                break
            except OSError as e:
                _lib_error = str(e)
    if _lib is None and _lib_error is None:
        _lib_error = ("librfa.dll/.so/.dylib not found next to rfaWrapper.py -- "
                       "build it first from rfa_c_source/, see this file's own docstring.")
    if _lib is not None:
        _lib.rfa_potential_init.argtypes = [ctypes.POINTER(PWPotential), ctypes.c_int, ctypes.c_double,
                                             ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double)]
        _lib.rfa_potential_init.restype = ctypes.c_int
        _lib.rfa_potential_free.argtypes = [ctypes.POINTER(PWPotential)]
        _lib.rfa1_solve.argtypes = [ctypes.POINTER(PWPotential), ctypes.POINTER(RFASolution)]
        _lib.rfa1_solve.restype = ctypes.c_int
        _lib.rfa_solution_free.argtypes = [ctypes.POINTER(RFASolution)]
        _lib.rfa_structure_factor.argtypes = [ctypes.POINTER(RFASolution), ctypes.c_double]
        _lib.rfa_structure_factor.restype = ctypes.c_double


def rfa1_compute(eta, lambda_steps, beps_steps, q):
    '''
    eta: packing fraction (volume fraction)
    lambda_steps: list of n increasing step outer-radii (>1, in units
        of sigma) -- e.g. [1.3] for a single square well matching this
        tool's own setSquareWellPotential(epsilon, delta)'s
        lambda=1+delta/sigma, or 3 values for PiecewiseConstantHS.
    beps_steps: list of n step energies in kT units (>0 shoulder, <0
        well). Confirmed by direct testing (see the conversation this
        was built in) to use the SAME sign convention as this tool's
        own setSquareWellPotential()/setPiecewiseConstantHSPotential()
        already do -- pass epsilonInkTUnits values directly, no sign
        flip needed.
    q: 1D array-like of dimensionless scattering vectors (q*sigma)

    Returns (structureFactor: np.ndarray same shape as q, ok: bool
    (whether rfa1_solve converged)). A converged=0 result (per the
    library's own header) can still occur in physically extreme
    parameter regions (high density, deep/wide wells) where no
    solution of the RFA equations exists -- this is documented,
    expected behaviour of the underlying theory, not a bug.
    '''
    _load_lib()
    if _lib is None:
        raise RuntimeError(_lib_error)

    n = len(lambda_steps)
    lam_arr = (ctypes.c_double * n)(*lambda_steps)
    beps_arr = (ctypes.c_double * n)(*beps_steps)

    pot = PWPotential()
    if not _lib.rfa_potential_init(ctypes.byref(pot), ctypes.c_int(n), ctypes.c_double(eta),
                                    lam_arr, beps_arr):
        raise ValueError("rfa_potential_init: lambda_steps must be strictly increasing from 1")

    sol = RFASolution()
    try:
        ok = _lib.rfa1_solve(ctypes.byref(pot), ctypes.byref(sol))
        q = np.ascontiguousarray(q, dtype=np.float64)
        out = np.array([_lib.rfa_structure_factor(ctypes.byref(sol), float(qi)) for qi in q])
        return out, bool(ok)
    finally:
        _lib.rfa_solution_free(ctypes.byref(sol))
        _lib.rfa_potential_free(ctypes.byref(pot))
