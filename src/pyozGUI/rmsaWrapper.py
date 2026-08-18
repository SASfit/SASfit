# -*- coding: utf-8 -*-
'''
Thin ctypes wrapper around SASfit's own Hayter-Penfold RMSA C library
(src/plugins/RMSA/{rmsa.c,polyroots.c,rmsa_physical.c} in SASfitGit),
compiled here as a standalone shared library (librmsa.so/.dll) with no
SASfit-specific dependencies -- reuses that exact, already-validated
implementation directly via ctypes, rather than reimplementing the
quartic-root-selection/rescaling algorithm in Python (a substantial
undertaking with real risk of reintroducing bugs that library's own
extensive comments describe having already found and fixed: wrong-root
selection, catastrophic small-K cancellation, ill-conditioned quartics).

Requires librmsa.dll (Windows) or librmsa.so (Linux/WSL) to be built
first from the sources in ./rmsa_c_source/ next to this file:
  - Windows (MSYS2 MinGW64 shell, same toolchain already used for the
    other C plugins in this project, e.g. robertus_shs):
        cd rmsa_c_source
        build_windows.bat
    (or run its one gcc command directly -- see that file)
  - Linux/WSL:
        cd rmsa_c_source
        gcc -shared -fPIC -O2 -o ../librmsa.so rmsa.c polyroots.c rmsa_physical.c \
            -I. -lgsl -lgslcblas -lm -lpthread

The .c/.h files themselves are a simplified copy of SASfit's own RMSA
plugin sources: the warm-start/caching optimization in
rmsa_compute_cached() was dropped for a plain re-solve on every call,
since that optimization only ever affects performance, never
correctness -- the original always falls back to a full solve whenever
there is any doubt anyway.

Validated (see the conversation this was built in) against two
independent checks: (1) the weak-coupling limit (gamma -> 0) correctly
reproduces the exact analytical PY hard-sphere contact value
g(sigma+) = (1+eta/2)/(1-eta)^2; (2) this tool's own numerical MSA
closure (oZfixpointOperator.py) independently reproduces that same
exact value in the Yukawa-free limit. Quantitative agreement at strong
Yukawa coupling was not fully nailed down against the numerical OZ
solver -- that regime is numerically hard for the finite Hankel-
transform grid (long-range tail, limited resolution), not a known flaw
in either implementation; treat this library, not the numerical OZ
route, as authoritative for RMSA specifically.
'''
import ctypes
import os
import numpy as np


# RMSASolution struct layout, matching include/rmsa.h exactly (field
# order and types) -- passed as a raw ctypes.Structure rather than
# void* so Python code can inspect the solved coefficients/diagnostics
# directly, same as a C caller would. Defined before _load_lib() below
# since that function's argtypes reference it directly.
class RMSASolution(ctypes.Structure):
    _fields_ = [
        ("rescaleIterations", ctypes.c_int),
        ("volumeFraction", ctypes.c_double),
        ("contactPotential", ctypes.c_double),
        ("screeningParam", ctypes.c_double),
        ("couplingConstant", ctypes.c_double),
        ("closureConstantCoeff", ctypes.c_double),
        ("closureLinearCoeff", ctypes.c_double),
        ("closureSinhCoeff", ctypes.c_double),
        ("closureCoshCoeff", ctypes.c_double),
        ("laplaceHAtScreening", ctypes.c_double),
        ("laplaceHAtZero", ctypes.c_double),
        ("rescaledVolumeFraction", ctypes.c_double),
        ("rescaledContactPotential", ctypes.c_double),
        ("rescaledScreeningParam", ctypes.c_double),
        ("rescalingFactor", ctypes.c_double),
        ("contactValue", ctypes.c_double),
        ("physicalRootResidual", ctypes.c_double),
        ("numCandidateRoots", ctypes.c_int),
        ("warnings", ctypes.c_uint),
    ]


_lib = None
_lib_error = None


def _load_lib():
    global _lib, _lib_error
    if _lib is not None or _lib_error is not None:
        return
    here = os.path.dirname(os.path.abspath(__file__))
    candidates = [os.path.join(here, "librmsa.dll"), os.path.join(here, "librmsa.so"),
                  os.path.join(here, "librmsa.dylib")]
    for path in candidates:
        if os.path.exists(path):
            try:
                _lib = ctypes.CDLL(path)
                break
            except OSError as e:
                _lib_error = str(e)
    if _lib is None and _lib_error is None:
        _lib_error = ("librmsa.dll/.so/.dylib not found next to rmsaWrapper.py -- "
                       "build it first from rmsa_c_source/, see this file's own docstring.")
    if _lib is not None:
        _lib.rmsa_compute.argtypes = [ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double,
                                       ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double),
                                       ctypes.c_int, ctypes.POINTER(RMSASolution)]
        _lib.rmsa_compute.restype = ctypes.c_int
        _lib.rmsa_error_string.argtypes = [ctypes.c_int]
        _lib.rmsa_error_string.restype = ctypes.c_char_p


def rmsa_compute(radius, screeningLength, chargeParameter, volumeFraction, q):
    '''
    q: 1D array-like of scattering vectors (same length unit as
    1/radius). Returns (structureFactor: np.ndarray same shape as q,
    status: int (>=0 success, rescaleIterations; <0 error, see
    rmsa_error_string(status)), solution: RMSASolution for inspection).
    '''
    _load_lib()
    if _lib is None:
        raise RuntimeError(_lib_error)

    q = np.ascontiguousarray(q, dtype=np.float64)
    out = np.empty_like(q)
    solution = RMSASolution()

    status = _lib.rmsa_compute(
        ctypes.c_double(radius), ctypes.c_double(screeningLength),
        ctypes.c_double(chargeParameter), ctypes.c_double(volumeFraction),
        q.ctypes.data_as(ctypes.POINTER(ctypes.c_double)),
        out.ctypes.data_as(ctypes.POINTER(ctypes.c_double)),
        ctypes.c_int(q.size), ctypes.byref(solution))

    return out, status, solution


def rmsa_error_string(status):
    _load_lib()
    if _lib is None:
        raise RuntimeError(_lib_error)
    return _lib.rmsa_error_string(ctypes.c_int(status)).decode("ascii")
