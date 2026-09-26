#!/usr/bin/env python3
"""Does a candidate verifier actually CATCH the wrong root?

    cd src/pyOZgui/tools
    python verifier_test.py

Writes verifier_test.json incrementally; progress goes to stderr, so NO
SHELL REDIRECT is needed. (Redirecting stdout is what filled benchmark.json
with the solvers' own "Picard converged after N steps" chatter.)

WHY. ozLib.solve(verify=True) re-solves with an independent solver and raises
if the two disagree. The default chain is ("Picard iteration",
"Biggs-Andrews"), chosen for INDEPENDENCE: Picard has no acceleration and no
history vectors, so it cannot share an acceleration failure mode with the
primary solver.

Benchmarking on a machine WITH SUNDIALS then showed the cost is a factor of
ten, not the 2.8x measured without it: the primary solve is 0.0075 s and
Picard is 0.0886 s, so the verifier is twelve times the thing it verifies.
`sundials4py: Newton-Krylov (GMRES)` at 0.0136 s would bring that to ~3x and
is a genuinely different algorithm family.

BUT SPEED IS NOT THE CRITERION. The verifier exists to catch a converged
WRONG answer. The test case is Lennard-Jones at epsilon = 0.8, phi = 0.3
under HNC -- a subcritical state near a documented fold bifurcation
(Beardmore, Peplow & Bresme, SIAM J. Sci. Comput. 29, 2442, 2007), where the
equation genuinely has several solution branches:

    correct (positive compressibility)   g_max = 2.163595
    scipy Anderson                       g_max = 1.431, min S(Q) = -46.5
    scipy Newton-Krylov                  raised NoConvergence

A verifier that REFUSES rather than disagreeing is safe but useless: it turns
every solve at a hard state point into an error. So for each candidate this
records whether it solves the easy case, whether it finds the CORRECT root on
the hard one, and whether pairing it with the known-bad solver produces a
caught disagreement rather than a crash or a false agreement.
"""
import json
import os
import sys
import time

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

OUT = os.environ.get("VERIFIER_OUT", "verifier_test.json")

BAD = "scipy Anderson"          # known to find the wrong branch on the LJ case
TRUE_GMAX = 2.163595            # correct answer for that case


def _say(m):
    print(m, file=sys.stderr, flush=True)


def _save(d):
    with open(OUT, "w") as fh:
        json.dump(d, fh, indent=2)
        fh.write("\n")


def solveOne(solver, potential, args, closure, phi):
    """(g_max, min S, seconds) for one solve, or an error string."""
    import ozLib
    t0 = time.perf_counter()
    try:
        r = ozLib.solve(potential, phi=phi, potentialArgs=args,
                        closure=closure, solver=solver, verify=False,
                        maxIterations=8000)
        g = np.asarray(r.curves["gr"], float)
        S = np.real(np.asarray(r.curves["Sq"], float))
        if not np.all(np.isfinite(g)):
            return f"non-finite after {time.perf_counter() - t0:.2f}s"
        return dict(gmax=round(float(np.nanmax(g)), 6),
                    minS=round(float(np.nanmin(S)), 4),
                    seconds=round(time.perf_counter() - t0, 4))
    except Exception as exc:
        return (f"{type(exc).__name__}: {str(exc)[:60]} "
                f"after {time.perf_counter() - t0:.2f}s")


def main():
    import ozLib
    out = {"note": ("correct g_max for the LJ case is 2.163595; "
                    f"{BAD} returns 1.431 with min S(Q) = -46.5, a converged "
                    "solution on the negative-compressibility branch"),
           "candidates": {}}

    cands = [k for k in ozLib.SOLVER_CLASSES if k != BAD]
    _say(f"testing {len(cands)} candidate verifiers against {BAD}")

    for name in cands:
        _say(f"  {name}")
        rec = {"easy_HS_PY": solveOne(name, "HardSphere", (),
                                      "Percus-Yevick", 0.3),
               "hard_LJ_HNC": solveOne(name, "LennardJones", (0.8,),
                                       "Hypernetted-Chain", 0.3)}
        h = rec["hard_LJ_HNC"]
        rec["finds_correct_root"] = (
            isinstance(h, dict) and abs(h["gmax"] - TRUE_GMAX) < 1e-3)

        # The decisive test: paired with the bad solver, does it raise?
        try:
            ozLib.solve("LennardJones", phi=0.3, potentialArgs=(0.8,),
                        closure="Hypernetted-Chain", solver=BAD,
                        verify=True, verifyWith=(name,), maxIterations=8000)
            rec["catches_wrong_root"] = "NO -- accepted the wrong answer"
        except ValueError as exc:
            msg = str(exc)
            rec["catches_wrong_root"] = (
                "YES -- disagreement" if "disagree" in msg
                else f"refused: {msg[:70]}")
        except Exception as exc:
            rec["catches_wrong_root"] = (
                f"{type(exc).__name__}: {str(exc)[:60]}")

        out["candidates"][name] = rec
        _save(out)

    _save(out)
    _say(f"done -- {OUT}")


if __name__ == "__main__":
    main()
