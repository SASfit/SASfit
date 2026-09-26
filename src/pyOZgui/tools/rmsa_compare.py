#!/usr/bin/env python3
"""Compare this package's RMSA against stored jscatter reference values.

    cd src/pyOZgui/tools
    python rmsa_compare.py

Needs `rmsa_jscatter_reference.json` in this directory, and librmsa.dll/.so
beside rmsaWrapper.py. Writes rmsa_compare.json.

WHY A STORED REFERENCE RATHER THAN A LIVE ONE. The manuscript's validation
table claims RMSA agreement is 'exact (9/9)'. That was measured against
jscatter's Hayter-Penfold implementation -- but jscatter does not build under
MinGW, its dependency chain pulling in MDAnalysis, JupyterLab, nglview,
pdb2pqr and netCDF4, so the comparison could not be repeated on the machine
the paper is written on.

The values were therefore generated once, where jscatter does run, and
committed. That makes the row re-checkable by anyone with this repository and
numpy, which is the property the table's other jscatter rows still lack. It
also means the reference cannot drift: if a future jscatter changes its
algorithm the stored numbers stay as the claim was made.

WHAT 'EXACT' SHOULD MEAN HERE, and why it is a strong claim worth testing.
Both implementations solve the same Hayter-Penfold rescaled MSA, which has a
closed-form solution -- there is no iteration, no grid and no discretisation
error. Two correct implementations should therefore agree to something near
machine precision, not merely closely. Agreement at 1e-3 would NOT be a
small discrepancy to be explained away; it would mean one of them has a bug,
or that the two are solving different equations behind the same name.

That is the opposite of the situation for the numerical routes, where a few
tenths of a per cent is the expected discretisation error. It is worth being
explicit about which kind of comparison is being made, because the same
number means different things in the two cases.

THE PARAMETER CONVENTION is recorded in the reference file and matters:
jscatter takes R as the RADIUS, scl as the Debye screening length 1/kappa,
gamma as the contact potential in kT, and eta as the volume fraction. A
mismatch here produces a plausible curve that disagrees by tens of per cent,
which is how an earlier attempt at the adhesive-sphere row went wrong: delta
was passed as 1+delta, giving a well fifty times too wide and a 72 per cent
'error' that was entirely the test's fault.
"""
import json
import os
import sys

import numpy as np

_HERE = os.path.dirname(os.path.abspath(__file__))
for _cand in (os.path.join(_HERE, os.pardir, "src", "pyozgui"),
              os.path.join(_HERE, os.pardir), _HERE):
    _cand = os.path.abspath(_cand)
    if os.path.isfile(os.path.join(_cand, "rmsaWrapper.py")):
        if _cand not in sys.path:
            sys.path.insert(0, _cand)
        break
else:
    raise ImportError("cannot locate rmsaWrapper.py")

REF = os.environ.get(
    "RMSA_REFERENCE",
    os.path.join(_HERE, "rmsa_jscatter_reference.json"))
OUT = os.environ.get("RMSA_COMPARE_OUT", "rmsa_compare.json")

#TWO REFERENCE SETS, and the second is the one that matters.
#
#  rmsa_jscatter_reference.json           gentle conditions
#  rmsa_jscatter_reference_rescaled.json  rescaling engaged
#
#The first was written before anyone checked whether the RESCALING actually
#triggered at those state points. It does not: rescaleIterations is 0 at all
#nine, so all nine agreements are for the unrescaled MSA and the "R" in RMSA
#-- the Hayter-Penfold rescaling, which exists precisely because MSA fails at
#strong coupling -- went untested. Agreement to 1e-16 on nine points that
#never exercise the feature under test is a weaker result than it looks.
#
#The second set is dilute, strongly charged and long-screened: charged
#colloids at low ionic strength, which is the case the method was written
#for. The effective volume fraction is lifted by factors of 2.3 to 34, and
#S(Q) peaks reach 3.9. Run both:
#
#    python rmsa_compare.py
#    RMSA_REFERENCE=rmsa_jscatter_reference_rescaled.json \
#        RMSA_COMPARE_OUT=rmsa_compare_rescaled.json python rmsa_compare.py


def main():
    if not os.path.isfile(REF):
        raise SystemExit(f"reference not found: {REF}")
    ref = json.load(open(REF))
    q = np.asarray(ref["q"], float)
    R = float(ref["R"])

    from rmsaWrapper import rmsa_compute

    results = {"reference": {k: ref[k] for k in
                             ("description", "jscatter_version",
                              "convention")},
               "points": {}}
    exact = 0
    for p in ref["points"]:
        key = f"phi={p['phi']},gamma={p['gamma']},scl={p['scl']}"
        expect = np.asarray(p["S"], float)
        try:
            S, status, sol = rmsa_compute(R, p["scl"], p["gamma"], p["phi"], q)
            S = np.asarray(S, float)
            #READ THE STATUS BEFORE THE ARRAY. status is the rescale
            #iteration count when >= 0 and an ERROR CODE when negative, and
            #the array returned alongside a failure is the unrescaled result
            #-- meaningful to look at, but not an answer.
            #
            #An earlier version compared it regardless and reported a 168 per
            #cent "discrepancy" at one state point, which was then nearly
            #written up as a silent fallback to MSA. The library had reported
            #the failure perfectly clearly; the test threw the report away. A
            #check that ignores the error code it is handed does not find
            #bugs, it invents them.
            if status < 0:
                try:
                    from rmsaWrapper import rmsa_error_string
                    why = rmsa_error_string(status)
                except Exception:
                    why = "(no message available)"
                out[key] = {"status": int(status),
                            "failed": True,
                            "message": why,
                            "note": "the solver reported failure; no "
                                    "comparison made"}
                print(f"{key:34s} FAILED status={status}: {why}",
                      file=sys.stderr)
                continue
            absdiff = float(np.max(np.abs(S - expect)))
            reldiff = absdiff/float(np.max(np.abs(expect)))
            #"EXACT" at 1e-10: both are closed-form solutions of the same
            #equations, so anything above rounding is a real difference.
            verdict = ("exact" if reldiff < 1e-10 else
                       "close" if reldiff < 1e-4 else
                       "DIFFERS")
            if reldiff < 1e-10:
                exact += 1
            results["points"][key] = {
                "maxAbsDiff": absdiff,
                "maxRelDiff": reldiff,
                "verdict": verdict,
                "status": int(status),
                "rescaleIterations": int(getattr(sol, "rescaleIterations", -1)),
                "ourRescaledEta": float(
                    getattr(sol, "rescaledVolumeFraction", float("nan"))),
                "jscatterRescaledEta": p.get("jscatterRescaledEta"),
                "ourRange": [float(S.min()), float(S.max())],
                "refRange": [p["minS"], p["maxS"]]}
            print(f"{key:34s} rel {reldiff:.3e}  {verdict}", file=sys.stderr)
        except Exception as exc:
            results["points"][key] = f"{type(exc).__name__}: {str(exc)[:90]}"
            print(f"{key:34s} {type(exc).__name__}", file=sys.stderr)

    n = len(ref["points"])
    #Points the solver declined are counted separately from points it got
    #wrong. Folding them together would let a failure masquerade as a
    #disagreement, which is what the first version of this script did.
    failed = sum(1 for v in results["points"].values()
                 if isinstance(v, dict) and v.get("failed"))
    compared = n - failed
    results["summary"] = (f"{exact}/{compared} exact to 1e-10 "
                          f"({failed} not solved, excluded)")
    results["matchesPublishedClaim"] = (exact == n)
    with open(OUT, "w") as fh:
        json.dump(results, fh, indent=2, sort_keys=True)
        fh.write("\n")
    print(f"done -- {OUT}; {results['summary']}", file=sys.stderr)


if __name__ == "__main__":
    main()
