# -*- coding: utf-8 -*-
'''
Thin ctypes wrapper around SASfit's own one-Yukawa MSA structure
factor C library (src/plugins/twoyukawa/{2Y_OneYukawa.c,2Y_cpoly.c,
2Y_utility.c,2Y_PairCorrelation.c} in SASfitGit), compiled here as a
standalone shared library with no SASfit-specific dependencies --
reuses SASfit's own algorithm directly via ctypes, same reasoning as
rmsaWrapper.py: the physical-root selection here needs a full g(r)
(via a 16384-point FFT) computed for every one of up to 4 quartic-root
candidates, and reimplementing that faithfully in Python would risk
subtly diverging from SASfit's own already-working selection logic.

Requires liboneyukawa.dll (Windows) or liboneyukawa.so (Linux/WSL) to
be built first from the sources in ./oneyukawa_c_source/ next to this
file -- see that directory's own build_windows.bat, or directly:
        cd oneyukawa_c_source
        gcc -shared -fPIC -O2 -o ../liboneyukawa.so \
            2Y_OneYukawa.c 2Y_cpoly.c 2Y_utility.c 2Y_PairCorrelation.c -I. -lm

NOTE ON LICENSING: 2Y_cpoly.c (the Jenkins-Traub polynomial root
finder this library depends on) carries its own copyright notice
("Henrik Vestermark Software License Agreement"), separate from the
GPLv3 that covers the rest of SASfit -- flagged here for visibility,
not because anything about compiling/using your own project's own
existing file differently is being done here.

Potential (Hayter & Penfold-adjacent one-Yukawa MSA convention, per
2Y_OneYukawa.c's own header comment):
    V(r) = -kB*T*K*exp[-Z*(r-1)]/r   for r > 1  (r in units of sigma)
    V(r) = infinity                   for r <= 1
i.e. Z is a dimensionless inverse screening length (in 1/sigma units,
NOT the same "Z" as macroion charge elsewhere in this project) and K
is the contact potential depth in kT. q here is dimensionless (q*sigma).

Cross-validated against this tool's own numerical HS3Yukawa+MSA solve
(see the conversation this was built in): mapping K1=-K, lambda1=1/Z
at sigma=1 reproduces this library's S(Q) to within ~2% mean / ~5% max
across q, concentrated at low q and shrinking rapidly at higher q --
the same order of residual found for the classic PY hard-sphere case
against its own exact analytical solution (a numerical-grid-vs-exact
discrepancy, not a bug in either implementation).
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
    candidates = [os.path.join(here, "liboneyukawa.dll"), os.path.join(here, "liboneyukawa.so"),
                  os.path.join(here, "liboneyukawa.dylib")]
    for path in candidates:
        if os.path.exists(path):
            try:
                _lib = ctypes.CDLL(path)
                break
            except OSError as e:
                _lib_error = str(e)
    if _lib is None and _lib_error is None:
        _lib_error = ("liboneyukawa.dll/.so/.dylib not found next to oneYukawaWrapper.py -- "
                       "build it first from oneyukawa_c_source/, see this file's own docstring.")
    if _lib is not None:
        _lib.Y_SolveEquations.argtypes = [ctypes.c_double, ctypes.c_double, ctypes.c_double,
                                           ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double),
                                           ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double),
                                           ctypes.c_int]
        _lib.Y_SolveEquations.restype = ctypes.c_int
        _lib.SqOneYukawa.argtypes = [ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double,
                                      ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double]
        _lib.SqOneYukawa.restype = ctypes.c_double
        _lib.Y_CheckSolution.argtypes = [ctypes.c_double, ctypes.c_double, ctypes.c_double,
                                          ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double]
        _lib.Y_CheckSolution.restype = ctypes.c_int


def one_yukawa_compute(Z, K, phi, q):
    '''
    Z: dimensionless inverse screening length (1/sigma units)
    K: contact potential depth (kT units)
    phi: volume fraction
    q: 1D array-like of dimensionless scattering vectors (q*sigma)

    Returns (structureFactor: np.ndarray same shape as q, ok: bool
    (whether Y_SolveEquations found a solution), coeffs: (a,b,c,d)
    tuple of the solved MSA closure coefficients, for inspection).

    This is a single (expensive, ~1-2s: solves a quartic via Jenkins-
    Traub, then for each real root does a 16384-point FFT to pick the
    physical one) solve per (Z, K, phi) -- unlike rmsaWrapper.py, this
    C library has no built-in cache, so if you need many q arrays at
    the same (Z, K, phi), call this once and reuse `coeffs` yourself
    via a from-scratch reimplementation of SqOneYukawa's own simple
    algebraic formula if repeated re-solving becomes a bottleneck.
    '''
    _load_lib()
    if _lib is None:
        raise RuntimeError(_lib_error)

    a = ctypes.c_double(0.0)
    b = ctypes.c_double(0.0)
    c = ctypes.c_double(0.0)
    d = ctypes.c_double(0.0)
    ok = _lib.Y_SolveEquations(ctypes.c_double(Z), ctypes.c_double(K), ctypes.c_double(phi),
                                ctypes.byref(a), ctypes.byref(b), ctypes.byref(c), ctypes.byref(d),
                                ctypes.c_int(0))
    if not ok:
        return None, False, (None, None, None, None)

    q = np.ascontiguousarray(q, dtype=np.float64)
    out = np.empty_like(q)
    for i, qi in enumerate(q):
        out[i] = _lib.SqOneYukawa(ctypes.c_double(qi), ctypes.c_double(Z), ctypes.c_double(K),
                                   ctypes.c_double(phi), a, b, c, d)
    return out, True, (a.value, b.value, c.value, d.value)


def one_yukawa_check_solution(Z, K, phi, a, b, c, d):
    _load_lib()
    if _lib is None:
        raise RuntimeError(_lib_error)
    return bool(_lib.Y_CheckSolution(ctypes.c_double(Z), ctypes.c_double(K), ctypes.c_double(phi),
                                      ctypes.c_double(a), ctypes.c_double(b),
                                      ctypes.c_double(c), ctypes.c_double(d)))
