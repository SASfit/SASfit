#!/usr/bin/env python3
"""Drive deflation through SUNDIALS KINSOL by SUBCLASSING, not patching.

    cd src/pyOZgui/tools
    python deflation_kinsol.py

Writes deflation_kinsol.json; progress to stderr, no redirect needed.

THE IDEA. `rootOperator(self, x)` is a METHOD on each sundials4py wrapper
class, returning the residual F(x). If the solver calls it through `self`,
then overriding it in a subclass changes what KINSOL sees -- no patch to
sundials4py required:

    class Deflated(BaseSolverClass):
        def rootOperator(self, x):
            return eta(x) * super().rootOperator(x)

That is the whole mechanism. KINSOL then solves G(x) = eta(x) F(x) = 0, where
eta accumulates one factor 1/||x - r||^p + alpha per known root, so known
roots become unreachable and others are left untouched.

WHY IT IS WORTH DOING. scipy's newton_krylov is currently the only working
deflation driver, and it is the slowest solver available (0.0348 s against
0.0136 s for KINSOL GMRES). KINSOL Newton with line search is also the
method Farrell et al. actually use. And scipy's NK could not converge AT ALL
on the BPGG state, which is why the question of whether that state has one
root or two is still open -- a stronger driver might settle it.

WHAT COULD GO WRONG, and the script checks for each:
  * the solver may capture `rootOperator` at construction rather than calling
    it through `self`, in which case the override never fires. TEST: the
    deflated run would return exactly the undeflated root.
  * KINSOL may be configured without a line search, in which case the
    deflated residual's blow-up near a known root is unnavigable -- the same
    failure Anderson showed.
  * the residual may be computed inside C from a registered callback, in
    which case Python-level overriding cannot reach it.

CONTROLS FIRST. Every experiment here reports the UNDEFLATED result from the
same starting point before the deflated one. An earlier version of this
investigation deflated a root the solver was not going to anyway and
concluded nothing while appearing to conclude something.
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

OUT = os.environ.get("DEFLATION_KINSOL_OUT", "deflation_kinsol.json")
N = 1024

STATES = [
    ("LJ_eps0.8_phi0.3_HNC", "LennardJones", (0.8,), "doHNCclosure", None, 0.3),
    ("BPGG_a0.5_phi0.4", "HardSphere", (), "doBPGGclosure", 0.5, 0.4),
]

#Keep the deflated solves short. A deflated residual can send the iteration
#into regions where the closure overflows, and without a cap on iterations
#the solve simply hangs -- observed on the first attempt.
DEFLATED_ITERATIONS = 400

CANDIDATES = ["sundials4py: Newton-Krylov (GMRES)",
              "sundials4py: Newton-Krylov (FGMRES)",
              "sundials4py: Newton-Krylov (TFQMR)"]


def _say(m):
    print(m, file=sys.stderr, flush=True)


def _save(d):
    with open(OUT, "w") as fh:
        json.dump(d, fh, indent=2)
        fh.write("\n")


#makeDeflatedClass NOW LIVES IN THE PACKAGE, not here.
#
#This script carried its own copy, which was the authoritative one until
#ozLib.solve() needed deflation and could not import from a standalone
#script. The copy here then went stale in a way that was invisible: it
#wrapped `rootOperator`, which the MULTICOMPONENT path never calls, so
#deflating a mixture solve did precisely nothing -- same iteration count,
#identical result, zero difference after excluding the known root. A
#feature that appears to work while doing nothing is the worst way for this
#to fail, and two copies of one function is how it happened.
#
#The package version wraps `fixPointOperator` instead, which both paths go
#through, and scales the STEP rather than the operator so the deflated map
#keeps the same fixed points except at the roots excluded.
from deflation import makeDeflatedClass        # noqa: F401  (re-export)


def build(cls, pot, args, clo, par, phi):
    s = cls(port=0, numberOfRadialSamplingPoints=N,
            hardSphereDiameterInPoints=100)
    s.setNumberOfIterations(8000)
    s.setVolumeDensity(phi)
    s.setPotentialByName(pot, *args)
    if par is not None:
        getattr(s, clo)(par)
    else:
        getattr(s, clo)()
    return s


def solveAndCapture(s):
    store = {}
    original = s.derivePhysicalQuantitiesFromFixpoint

    def capture(x, *a, **k):
        store["x"] = np.asarray(x, float).copy()
        return original(x, *a, **k)

    s.derivePhysicalQuantitiesFromFixpoint = capture
    s.solve()
    return store.get("x")


def main():
    import ozLib
    import picardOZsolver
    import andersonOZsolver

    out = {"states": {}}

    for label, pot, args, clo, par, phi in STATES:
        _say(label)
        rec = {}
        ref = build(picardOZsolver.PicardOZsolver, pot, args, clo, par, phi)

        def F(x):
            y = np.asarray(ref.fixPointOperatorForGamma(x), float)
            return (y[0] if y.ndim > 1 else y) - x

        def describe(x):
            if x is None:
                return None
            ref.derivePhysicalQuantitiesFromFixpoint(x)
            S = np.real(np.asarray(ref.getSq(), float))
            r = float(np.linalg.norm(F(x)))
            return dict(g_max=round(float(np.nanmax(
                            np.asarray(ref.getRDF(), float))), 6),
                        min_S=round(float(np.nanmin(S)), 4),
                        residual=r, is_a_fixed_point=bool(r < 1e-6))

        known = []
        try:
            rA = solveAndCapture(build(andersonOZsolver.AndersonOZsolver,
                                       pot, args, clo, par, phi))
            rec["anderson_root"] = describe(rA)
            if rA is not None:
                known.append(rA)
        except Exception as exc:
            rec["anderson_root"] = f"{type(exc).__name__}"

        for name in CANDIDATES:
            if name not in ozLib.SOLVER_CLASSES:
                continue
            base, _linear = ozLib.SOLVER_CLASSES[name]
            short = name.split("(")[-1].rstrip(")")

            #CONTROL: undeflated, same class, same start.
            try:
                r0 = solveAndCapture(build(base, pot, args, clo, par, phi))
                rec[f"{short}_CONTROL"] = describe(r0)
            except Exception as exc:
                r0 = None
                rec[f"{short}_CONTROL"] = f"{type(exc).__name__}"
            #Save NOW. The deflated run below can hang or take a very long
            #time, and an interrupt there previously discarded this control
            #result too -- saving only after the pair meant losing both.
            out["states"][label] = rec
            _save(out)

            #DEFLATED: everything known so far, including what this solver
            #just found -- deflating a root the solver was never going to
            #reach proves nothing.
            roots = list(known) + ([r0] if r0 is not None else [])
            if not roots:
                rec[f"{short}_DEFLATED"] = "no root to deflate"
                continue
            try:
                cls = makeDeflatedClass(base, roots)
                s = build(cls, pot, args, clo, par, phi)
                s.setNumberOfIterations(DEFLATED_ITERATIONS)
                r1 = solveAndCapture(s)
                d = describe(r1)
                if d is not None and r0 is not None:
                    #Did the override actually fire? If the deflated run
                    #returns the undeflated root, it did not.
                    same = float(np.linalg.norm(r1 - r0)
                                 / max(np.linalg.norm(r0), 1e-30))
                    d["distance_from_control"] = same
                    d["override_took_effect"] = bool(same > 1e-6)
                rec[f"{short}_DEFLATED"] = d
                if d and d.get("is_a_fixed_point") and d.get(
                        "override_took_effect"):
                    known.append(r1)
            except Exception as exc:
                rec[f"{short}_DEFLATED"] = f"{type(exc).__name__}: {str(exc)[:60]}"
            _save(out | {"states": {**out["states"], label: rec}})

        out["states"][label] = rec
        _save(out)

    _save(out)
    _say(f"done -- {OUT}")


if __name__ == "__main__":
    main()
