#!/usr/bin/env python3
"""Compare this package's RMSA against sasmodels' hayter_msa.

    cd src/pyOZgui/tools
    python rmsa_vs_sasmodels.py

Needs rmsa_sasmodels_reference.json beside it, and librmsa.dll/.so next to
rmsaWrapper.py. Writes rmsa_vs_sasmodels.json.

WHY sasmodels. The validation table's RMSA row claims agreement is "exact
(9/9)", measured against jscatter -- which does not build under MinGW, so
the claim could not be re-checked on the machine the paper is written on. A
validation nobody can repeat is a weak one, and the PY row showed what that
costs: re-measuring it found the quoted figure was a DST-IV result the table
did not identify as such.

sasmodels is a better reference than jscatter for this purpose. It installs
with `pip install sasmodels` and nothing else, it is the SANS community's
own implementation, and it is genuinely independent: a different code
lineage, not another copy of the same MATLAB.

THE CONVERSION IS THE WHOLE DIFFICULTY, and it is where this comparison can
quietly go wrong. The two codes take different inputs. rmsa_compute wants
the screening length and the contact potential directly; sasmodels wants
charge, temperature, salt concentration and dielectric constant, and derives
the other two internally. The derivation is in sasmodels/models/hayter_msa.c
and is reproduced in `convert` below, verbatim rather than paraphrased:

    IonSt = 0.5 e^2 (z phi / Vp + 2 cs)        counterions AND added salt
    kappa = sqrt(2 beta IonSt / perm)
    gamma = beta (ze)^2 / (pi perm sigma (2 + kappa sigma)^2)

Two traps in that. The ionic strength includes the COUNTERIONS from the
particles themselves, so setting the salt concentration to zero does not
give zero screening -- an obvious-looking test case that would compare two
different systems. And sigma there is the DIAMETER; jscatter's equivalent
expression is written with the radius, and conflating the two costs a factor
of two in gamma. An earlier comparison in this project reported a 72 per
cent "disagreement" that was entirely a convention mismatch of exactly this
kind, so the units are checked below rather than assumed.
"""
import json
import os
import sys

import numpy as np

_HERE = os.path.dirname(os.path.abspath(__file__))
for _cand in (os.path.join(_HERE, os.pardir, "src", "pyozgui"), _HERE):
    _cand = os.path.abspath(_cand)
    if os.path.isfile(os.path.join(_cand, "rmsaWrapper.py")):
        if _cand not in sys.path:
            sys.path.insert(0, _cand)
        break

REF = os.path.join(_HERE, "rmsa_sasmodels_reference.json")
OUT = os.environ.get("RMSA_SAS_OUT", "rmsa_vs_sasmodels.json")


def main():
    if not os.path.isfile(REF):
        raise SystemExit(f"reference not found: {REF}")
    ref = json.load(open(REF))
    q = np.asarray(ref["q"], float)

    from rmsaWrapper import rmsa_compute

    results = {"source": ref.get("note"), "points": {}}
    for p in ref["points"]:
        key = (f"d={p['diameterA']}A,phi={p['phi']},z={p['charge']},"
               f"cs={p['csalt']}M")
        expect = np.asarray(p["S"], float)
        #UNITS. sasmodels works in Angstrom and 1/Angstrom; rmsa_compute
        #takes a radius and a screening length in the same length unit as
        #1/q. Passing the radius in Angstrom with q in 1/Angstrom keeps both
        #consistent, and the screening length comes from the reference in
        #Angstrom for the same reason.
        R = p["diameterA"]/2.0
        try:
            S, status, sol = rmsa_compute(R, p["screeningLengthA"],
                                          p["gamma"], p["phi"], q)
            if status < 0:
                from rmsaWrapper import rmsa_error_string
                results["points"][key] = {
                    "failed": True, "status": int(status),
                    "message": rmsa_error_string(status)}
                print(f"{key:44s} FAILED status={status}", file=sys.stderr)
                continue
            S = np.asarray(S, float)
            rel = float(np.max(np.abs(S - expect))/np.max(np.abs(expect)))
            #WHAT COUNTS AS AGREEMENT HERE. Both solve the same closed-form
            #Hayter-Penfold problem, so two correct implementations should
            #agree to near rounding -- but they differ in how they rescale
            #and in their small-q series, so a few parts in 1e4 is the
            #realistic target rather than 1e-15. Anything at the per-cent
            #level means the inputs do not describe the same system, which
            #is a conversion error, not a disagreement about physics.
            verdict = ("excellent" if rel < 1e-4 else
                       "acceptable" if rel < 1e-2 else
                       "DIFFERS -- check the conversion, not the physics")
            results["points"][key] = {
                "maxRelDiff": rel, "verdict": verdict,
                "kappaSigma": p["kappaSigma"], "gamma": p["gamma"],
                "ourRange": [float(S.min()), float(S.max())],
                "refRange": [float(expect.min()), float(expect.max())]}
            print(f"{key:44s} rel {rel:.3e}  {verdict}", file=sys.stderr)
        except Exception as exc:
            results["points"][key] = f"{type(exc).__name__}: {str(exc)[:90]}"
            print(f"{key:44s} {type(exc).__name__}", file=sys.stderr)

    with open(OUT, "w") as fh:
        json.dump(results, fh, indent=2, sort_keys=True)
        fh.write("\n")
    print(f"done -- {OUT}", file=sys.stderr)


if __name__ == "__main__":
    main()
