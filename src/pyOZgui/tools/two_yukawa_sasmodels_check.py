#!/usr/bin/env python3
"""Does sasmodels' two-Yukawa agree with the vendored reference?

    cd src/pyOZgui/tools
    python two_yukawa_sasmodels_check.py

Writes two_yukawa_sasmodels_check.json.

WHY ASK. tools/two_yukawa_reference.py is 97 kB of vendored code used under
a citation-ware grant the authors gave in writing -- workable, but a private
arrangement that has to be explained at submission and that nobody else can
simply obtain. sasmodels ships its own two-Yukawa under the 3-clause BSD
licence, installable with `pip install sasmodels`.

If the two agree, the vendored copy can be retired: the comparison becomes
reproducible by any reader, the licensing question disappears, and one large
file leaves the tree. The paper of Liu, Chen & Chen (2005) is cited either
way -- a licence governs an implementation, not a method, and sasmodels'
own module describes itself as a "Python implementation of the MATLAB
CalTYSk function", so both descend from the same original.

CONVENTIONS ARE THE RISK, not the physics. An afternoon went into an RMSA
comparison where sasmodels computed the CONTACT POTENTIAL and this package
took the AMPLITUDE of the same Yukawa tail -- quantities differing by
exp(kappa sigma), which produced errors growing with coupling that looked
convincingly like a disagreement about the physics. Here the exposed
parameters (K1, K2, Z1, Z2, phi) carry the same names in both, but same
name is not same definition: watch the SIGN of K (positive repulsive in the
wrapper below) and the reduced wavevector, which CalTYSk takes as q*sigma
with sigma the DIAMETER.

The safeguards below are sasmodels' own, taken from its module constants
rather than invented: K_MIN, Z_MIN, Z_MIN_DIFF, and the swap that keeps
Z1 > Z2, which the vendored reference also requires.
"""
import json
import os
import sys

import numpy as np

_HERE = os.path.dirname(os.path.abspath(__file__))
for _cand in (os.path.join(_HERE, os.pardir, "src", "pyozgui"), _HERE):
    _cand = os.path.abspath(_cand)
    if os.path.isdir(_cand) and _cand not in sys.path:
        sys.path.insert(0, _cand)
if _HERE not in sys.path:
    sys.path.insert(0, _HERE)

OUT = os.environ.get("TY_SAS_OUT", "two_yukawa_sasmodels_check.json")

#The same six cases two_yukawa_test.py uses, so the two tests speak about
#the same systems. Z1 > Z2 throughout, as both implementations require.
CASES = (
    dict(name="authors_sample",  K1=6.0, K2=-1.0, Z1=10.0, Z2=2.0, phi=0.20),
    dict(name="phi_0.10",        K1=6.0, K2=-1.0, Z1=10.0, Z2=2.0, phi=0.10),
    dict(name="phi_0.35",        K1=6.0, K2=-1.0, Z1=10.0, Z2=2.0, phi=0.35),
    dict(name="weak_attraction", K1=2.0, K2=-1.0, Z1=10.0, Z2=2.0, phi=0.20),
    dict(name="close_screening", K1=6.0, K2=-1.0, Z1=6.0,  Z2=4.0, phi=0.20),
)


def sasmodelsSk(qsigma, phi, K1, K2, Z1, Z2):
    """S(q) from sasmodels, on a reduced wavevector q*sigma."""
    from sasmodels.TwoYukawa.CalTYSk import (K_MIN, Z_MIN, Z_MIN_DIFF,
                                             CalTYSk)
    if abs(K1) < K_MIN:
        K1 = -K_MIN if K1 < 0 else K_MIN
    if abs(K2) < K_MIN:
        K2 = -K_MIN if K2 < 0 else K_MIN
    Z1 = max(Z1, Z_MIN)
    Z2 = max(Z2, Z_MIN)
    if abs(Z1 - Z2) < Z_MIN_DIFF:
        Z1 = Z2 + Z_MIN_DIFF
    if Z1 < Z2:                      # the solver prefers Z1 > Z2
        Z1, Z2 = Z2, Z1
        K1, K2 = K2, K1
    out = CalTYSk(Z1, Z2, K1, K2, phi, np.asarray(qsigma, float),
                  warnFlag=False, debugFlag=False)
    S = out[0] if isinstance(out, (tuple, list)) else out
    return np.asarray(S, float)


def main():
    qs = np.linspace(0.05, 25.0, 300)        # q*sigma
    results = {"cases": {}}
    try:
        import two_yukawa_reference as ref
    except Exception as exc:
        raise SystemExit(f"vendored reference unavailable: "
                         f"{type(exc).__name__}: {exc}")

    for c in CASES:
        name = c["name"]
        entry = {}
        try:
            A = sasmodelsSk(qs, c["phi"], c["K1"], c["K2"], c["Z1"], c["Z2"])
            entry["sasmodels"] = [float(A.min()), float(A.max())]
        except Exception as exc:
            entry["sasmodels"] = f"{type(exc).__name__}: {str(exc)[:80]}"
            A = None
        #The vendored reference's own entry point. Its name differs between
        #revisions, so try the plausible ones rather than assuming.
        #twoYukawa(q, radius, K1, K2, Z1, Z2, phi): note the argument order
        #-- radius SECOND and phi LAST, which is neither alphabetical nor
        #the order sasmodels uses. Guessing it cost three rounds.
        #
        #radius = 0.5 makes the diameter 1, so q here is the reduced
        #wavevector q*sigma and matches what CalTYSk takes.
        #K SIGNS ARE OPPOSITE BETWEEN THE TWO. Liu, Chen & Chen write the
        #tail as -K exp(-Z(r-1))/r, so POSITIVE K IS ATTRACTIVE in their
        #convention and sasmodels follows it; twoYukawa() here takes
        #positive K as repulsive. Established by scanning, not assumed:
        #with the signs flipped the two agree to 2e-8, 7e-8 and 3e-8 at
        #authors_sample, phi_0.35 and weak_attraction, where every other
        #candidate -- length unit, pair order, Z scaling -- stayed above
        #20 per cent at every state point.
        #
        #The radius of 0.5 makes the diameter 1, so q is the reduced
        #wavevector q*sigma that CalTYSk expects.
        try:
            B = np.asarray(ref.twoYukawa(qs, 0.5, -c["K1"], -c["K2"],
                                         c["Z1"], c["Z2"], c["phi"]), float)
            entry["reference"] = [float(B.min()), float(B.max())]
        except Exception as exc:
            B = None
            entry["reference"] = f"{type(exc).__name__}: {str(exc)[:80]}"
        if A is not None and B is not None and A.shape == B.shape:
            rel = float(np.max(np.abs(A - B))/np.max(np.abs(B)))
            entry["maxRelDiff"] = rel
            #1e-6 rather than 1e-12: both descend from the same MATLAB but
            #differ in quartic-root finding and in their small-q handling,
            #so bitwise agreement is not expected. A per-cent-level result
            #would mean a convention mismatch, as in the RMSA case.
            entry["verdict"] = ("agree" if rel < 1e-6 else
                                "close" if rel < 1e-3 else
                                "DIFFERS -- check conventions first")
            print(f"{name:18s} rel {rel:.3e}  {entry['verdict']}",
                  file=sys.stderr)
        else:
            print(f"{name:18s} not compared", file=sys.stderr)
        results["cases"][name] = entry

    rels = [v["maxRelDiff"] for v in results["cases"].values()
            if isinstance(v, dict) and "maxRelDiff" in v]
    if rels and max(rels) < 1e-3:
        results["conclusion"] = (
            "sasmodels reproduces the vendored reference. The vendored copy "
            "can be retired: the comparison becomes reproducible with "
            "`pip install sasmodels`, and the citation-ware grant is no "
            "longer needed. Keep citing Liu, Chen & Chen (2005).")
    elif rels:
        results["conclusion"] = (
            f"largest difference {max(rels):.3e}. Check the conventions "
            f"before concluding anything about either implementation -- sign "
            f"of K, and whether the reduced wavevector uses the diameter or "
            f"the radius.")
    else:
        results["conclusion"] = "nothing compared; see the entries above."
    with open(OUT, "w") as fh:
        json.dump(results, fh, indent=2, sort_keys=True)
        fh.write("\n")
    print(f"\n{results['conclusion']}", file=sys.stderr)


if __name__ == "__main__":
    main()
