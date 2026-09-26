#!/usr/bin/env python3
"""Deflation, done with controls: deflate the root the solver ACTUALLY finds.

    cd src/pyOZgui/tools
    python deflation_sundials.py

Writes deflation_sundials.json; progress to stderr, no redirect needed.

WHAT WENT WRONG LAST TIME. The previous version deflated the root found by
ANDERSON (g_max = 2.1636) and then ran Newton-Krylov. But Newton-Krylov never
goes there: undeflated it converges to g_max = 1.2480, and deflated it
converges to g_max = 1.2480 as well. Deflating root 1 fenced off somewhere
the solver was not going, so the test demonstrated nothing.

That also weakened an earlier conclusion. At BPGG alpha = 0.5, phi = 0.4 the
deflated run returned NoConvergence and this was read as "no second root
exists". If the solver wanders regardless of deflation, NoConvergence means
"this solver did not find one", not "there is none".

THE RULE THIS ENFORCES: establish what the solver does UNDEFLATED, deflate
THAT, and only then interpret the result.

WHAT THIS SCRIPT DOES.
  1. Reports the signature of `rootOperator`, which every sundials4py
     wrapper exposes alongside `fixPointOperator`. If it accepts an external
     residual, KINSOL's line-searched Newton becomes available as a deflation
     driver -- the method Farrell et al. use, and 2.5x faster than scipy's
     (0.0136 s against 0.0348 s).
  2. Runs the corrected experiment on two states:
       LJ eps = 0.8, phi = 0.3, HNC   -- known to have at least three roots
       BPGG alpha = 0.5, phi = 0.4    -- where the "only one root" claim
                                         needs redoing
     For each: find the Anderson root and the Newton-Krylov root separately,
     then deflate the NK root and re-run NK, then deflate both and re-run.
     Every claimed solution is reported with its residual, because a
     convergence flag is a claim and the residual is evidence.
"""
import inspect
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

OUT = os.environ.get("DEFLATION_SUNDIALS_OUT", "deflation_sundials.json")
N = 1024

#(label, potential, args, closure setter, closure parameter, phi)
STATES = [
    ("LJ_eps0.8_phi0.3_HNC", "LennardJones", (0.8,), "doHNCclosure", None, 0.3),
    ("BPGG_a0.5_phi0.4", "HardSphere", (), "doBPGGclosure", 0.5, 0.4),
]


def _say(m):
    print(m, file=sys.stderr, flush=True)


def _save(d):
    with open(OUT, "w") as fh:
        json.dump(d, fh, indent=2)
        fh.write("\n")


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
    """Run a solver and return the fixpoint vector it actually used."""
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
    from deflation import deflationFactor
    from scipy.optimize import newton_krylov

    out = {"probe": {}, "states": {}}

    # ---- 1. can KINSOL take an external residual? ----------------------
    _say("1/3 probing rootOperator")
    for name in ozLib.SOLVER_CLASSES:
        if "sundials" not in name.lower():
            continue
        cls, _linear = ozLib.SOLVER_CLASSES[name]
        fn = getattr(cls, "rootOperator", None)
        if fn is None:
            out["probe"][name] = "no rootOperator"
            continue
        try:
            out["probe"][name] = {
                "signature": str(inspect.signature(fn)),
                "doc": (inspect.getdoc(fn) or "")[:300]}
        except Exception as exc:
            out["probe"][name] = f"{type(exc).__name__}: {exc}"
    _save(out)

    # ---- 2 & 3. the corrected deflation experiment ---------------------
    for label, pot, args, clo, par, phi in STATES:
        _say(label)
        rec = {}
        ref = build(picardOZsolver.PicardOZsolver, pot, args, clo, par, phi)

        def F(x):
            y = np.asarray(ref.fixPointOperatorForGamma(x), float)
            #NOTE the [0]: the operator returns a pair.
            return (y[0] if y.ndim > 1 else y) - x

        def describe(x):
            if x is None:
                return None
            ref.derivePhysicalQuantitiesFromFixpoint(x)
            S = np.real(np.asarray(ref.getSq(), float))
            g = np.asarray(ref.getRDF(), float)
            r = float(np.linalg.norm(F(x)))
            return dict(g_max=round(float(np.nanmax(g)), 6),
                        min_S=round(float(np.nanmin(S)), 4),
                        residual=r,
                        is_a_fixed_point=bool(r < 1e-6))

        #(a) what Anderson finds
        try:
            rA = solveAndCapture(build(andersonOZsolver.AndersonOZsolver,
                                       pot, args, clo, par, phi))
            rec["anderson_root"] = describe(rA)
        except Exception as exc:
            rA, rec["anderson_root"] = None, f"{type(exc).__name__}"

        #(b) what Newton-Krylov finds UNDEFLATED -- the control
        try:
            rK = newton_krylov(F, np.zeros(N), f_tol=1e-8, maxiter=400,
                               line_search="armijo")
            rec["nk_undeflated_CONTROL"] = describe(rK)
        except Exception as exc:
            rK, rec["nk_undeflated_CONTROL"] = None, f"{type(exc).__name__}"
        _save(out | {"states": {**out["states"], label: rec}})

        #(c) deflate what NK ACTUALLY found, and look again
        if rK is not None:
            def G1(x):
                return deflationFactor(x, [rK], 2.0, 1.0)*F(x)
            try:
                r2 = newton_krylov(G1, np.zeros(N), f_tol=1e-8, maxiter=400,
                                   line_search="armijo")
                rec["nk_deflating_its_own_root"] = describe(r2)
            except Exception as exc:
                r2 = None
                rec["nk_deflating_its_own_root"] = f"{type(exc).__name__}"

            #(d) deflate both known roots
            known = [r for r in (rK, rA, r2) if r is not None]
            def G2(x):
                return deflationFactor(x, known, 2.0, 1.0)*F(x)
            try:
                r3 = newton_krylov(G2, np.zeros(N), f_tol=1e-8, maxiter=400,
                                   line_search="armijo")
                rec["nk_deflating_all_known"] = describe(r3)
            except Exception as exc:
                rec["nk_deflating_all_known"] = f"{type(exc).__name__}"

        out["states"][label] = rec
        _save(out)

    _save(out)
    _say(f"done -- {OUT}")


if __name__ == "__main__":
    main()
