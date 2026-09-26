#!/usr/bin/env python3
"""Is every claimed solution actually a fixed point?

    cd src/pyOZgui/tools
    python residual_check.py

Writes residual_check.json; progress to stderr, no redirect needed.

WHY THIS EXISTS. A solver's convergence flag is a claim; the residual
||T(x) - x|| is evidence. This package has now produced two cases where the
difference mattered and only the residual revealed it:

  BPGG alpha = 0.5, phi = 0.4    Picard reported g_max = 3.0039 as converged.
                                 |F| there was 19.4 -- not a fixed point at
                                 all. The true root is 5.2528. Cause: the
                                 convergence test compared successive NORMS
                                 of x rather than the change in x, so two
                                 different vectors of similar norm passed.
                                 Now fixed, and `converged` is exposed.

  LennardJones eps = 0.8,        scipy Anderson returns g_max = 1.4307 with
  phi = 0.3, HNC                 min S(Q) = -46.5. That one IS a genuine
                                 fixed point, |F| = 5.7e-11 -- a real
                                 solution on a negative-compressibility
                                 branch, exactly as the bifurcation analysis
                                 of Beardmore, Peplow & Bresme describes.

Same symptom, opposite causes, and only ||T(x) - x|| distinguishes them.

THE OPEN QUESTION THIS ANSWERS. The manuscript currently claims that branch
selection tracks ALGORITHM FAMILY: five fixed-point methods find the physical
root while four Newton-Krylov methods find negative-compressibility branches.
That rests on SUNDIALS results measured on a machine where SUNDIALS is
installed, and NOBODY HAS CHECKED THE RESIDUAL ON THEM. If the four
Newton-Krylov results have |F| ~ 1e-11 they are genuine roots and the claim
stands. If they are ~1e+1 they are non-convergences dressed as answers, and
that paragraph must come out of the paper.

Run this where SUNDIALS is available.
"""
import json
import os
import sys

import numpy as np

_HERE = os.path.dirname(os.path.abspath(__file__))
for _cand in (_HERE,
              os.path.join(_HERE, os.pardir, "src", "pyozgui"),
              os.path.join(_HERE, os.pardir, "src", "pyOZgui"),
              os.path.join(_HERE, os.pardir, "pyozgui"),
              os.path.join(_HERE, os.pardir)):
    _cand = os.path.abspath(_cand)
    if os.path.isfile(os.path.join(_cand, "ozLib.py")):
        if _cand not in sys.path:
            sys.path.insert(0, _cand)
        break
else:
    raise ImportError(f"cannot locate ozLib.py relative to {_HERE!r}")

OUT = os.environ.get("RESIDUAL_OUT", "residual_check.json")
N, PPS = 4095, 100

#(label, potential, args, closure-setter, closure parameter, phi)
CASES = [
    ("LJ_eps0.8_phi0.3_HNC", "LennardJones", (0.8,), "doHNCclosure", None, 0.3),
    ("BPGG_a0.5_phi0.4", "HardSphere", (), "doBPGGclosure", 0.5, 0.4),
    ("HS_PY_phi0.3", "HardSphere", (), "doPYclosure", None, 0.3),
]


def _say(m):
    print(m, file=sys.stderr, flush=True)


def _save(d):
    with open(OUT, "w") as fh:
        json.dump(d, fh, indent=2)
        fh.write("\n")


def makeSolver(cls, potential, args, closure, param, phi):
    s = cls(port=0, numberOfRadialSamplingPoints=N,
            hardSphereDiameterInPoints=PPS)
    s.setNumberOfIterations(8000)
    s.setVolumeDensity(phi)
    s.setPotentialByName(potential, *args)
    if param is not None:
        getattr(s, closure)(param)
    else:
        getattr(s, closure)()
    return s


def main():
    import ozLib
    import picardOZsolver

    out = {"grid": dict(N=N, pointsPerSigma=PPS), "cases": {}}

    for label, pot, args, clo, par, phi in CASES:
        _say(label)
        out["cases"][label] = {}

        #One reference operator per case, used to evaluate the residual of
        #whatever any solver claims. Deliberately a DIFFERENT object from the
        #solver under test, so a solver cannot mark its own homework.
        ref = makeSolver(picardOZsolver.PicardOZsolver, pot, args, clo, par, phi)

        def residualNorm(x):
            y = np.asarray(ref.fixPointOperatorForGamma(x), float)
            #NOTE the [0]: the operator returns a pair, and taking the whole
            #thing yields a (2, N) array that silently breaks the norm.
            return float(np.linalg.norm((y[0] if y.ndim > 1 else y) - x))

        for name in ozLib.SOLVER_CLASSES:
            _say(f"   {name}")
            rec = {}
            try:
                cls, _linear = ozLib.SOLVER_CLASSES[name]
                s = makeSolver(cls, pot, args, clo, par, phi)

                store = {}
                original = s.derivePhysicalQuantitiesFromFixpoint

                def capture(x, *a, **k):
                    store["x"] = np.asarray(x, float).copy()
                    return original(x, *a, **k)

                s.derivePhysicalQuantitiesFromFixpoint = capture
                s.solve()

                g = np.asarray(s.getRDF(), float)
                S = np.real(np.asarray(s.getSq(), float))
                rec["g_max"] = round(float(np.nanmax(g)), 6)
                rec["min_S"] = round(float(np.nanmin(S)), 4)
                rec["finite"] = bool(np.all(np.isfinite(g)))
                rec["converged_flag"] = bool(getattr(s, "converged", None)) \
                    if hasattr(s, "converged") else None
                if "x" in store:
                    r = residualNorm(store["x"])
                    rec["residual"] = r
                    #The verdict. A converged flag is a claim; this is
                    #evidence. 1e-6 is generous -- genuine roots here come
                    #in at 1e-11.
                    rec["is_a_fixed_point"] = bool(r < 1e-6)
                else:
                    rec["residual"] = None
                    rec["is_a_fixed_point"] = None
            except Exception as exc:
                rec["error"] = f"{type(exc).__name__}: {str(exc)[:80]}"
            out["cases"][label][name] = rec
            _save(out)

    _save(out)
    _say(f"done -- {OUT}")


if __name__ == "__main__":
    main()
