#!/usr/bin/env python3
"""Does our RMSA agree with sasmodels where the rescaling does NOT engage?

    cd src/pyOZgui/tools
    python rmsa_weakcoupling_test.py

Needs rmsa_weakcoupling_reference.json beside it and librmsa next to
rmsaWrapper.py. Writes rmsa_weakcoupling_test.json.

THE QUESTION THIS SETTLES. Compared against sasmodels' hayter_msa at
strongly coupled state points, our RMSA disagrees by 6 to 29 per cent --
and the disagreement GROWS with coupling rather than being a constant
factor, which rules out a units or convention error. Separately,
rmsa_compute reports rescaleIterations = 0 for gamma <= 5 and 6 at
gamma = 15. Both observations point at the Hayter-Penfold RESCALING rather
than at the mean-spherical solution underneath it.

This test makes that precise instead of inferred. The four reference points
were constructed by inverting sasmodels' own conversion to land on chosen
(gamma, kappa*sigma) pairs in the regime where neither code rescales. If the
two agree here and not at strong coupling, the split is established:

    exact where the rescaling does not engage;
    implementation-dependent where it does.

That is a statement about the METHOD, not about this code, and a more useful
one for a reader than the "exact (9/9)" the validation table currently
carries -- which rests on a jscatter comparison that cannot be repeated on
the machine this is developed on.

WHY IT WOULD MATTER. Hayter-Penfold rescaling is widely used and its
procedure has real implementation freedom: when to trigger, how to search
for the rescaled volume fraction, when to stop. If implementations differ by
tens of per cent at strong coupling -- the regime the rescaling exists for
-- that is worth reporting, and it is not obvious from the literature.

A CAUTION ON READING THE RESULT. Agreement here does not validate the
rescaling; it validates everything EXCEPT the rescaling. The strongly
coupled cases remain unresolved either way, and should be described as such
rather than quietly dropped.
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

REF = os.path.join(_HERE, "rmsa_weakcoupling_reference.json")
OUT = os.environ.get("RMSA_WEAK_OUT", "rmsa_weakcoupling_test.json")


def main():
    if not os.path.isfile(REF):
        raise SystemExit(f"reference not found: {REF}")
    ref = json.load(open(REF))
    q = np.asarray(ref["q"], float)

    from rmsaWrapper import rmsa_compute

    results = {"source": ref.get("note"), "points": {}}
    agreed = engaged = 0
    for p in ref["points"]:
        key = f"gamma={p['gamma']},kappaSigma={p['kappaSigma']},phi={p['phi']}"
        expect = np.asarray(p["S"], float)
        try:
            S, status, sol = rmsa_compute(p["diameterA"]/2.0,
                                          p["screeningLengthA"],
                                          p["gamma"], p["phi"], q)
            if status < 0:
                from rmsaWrapper import rmsa_error_string
                results["points"][key] = {"failed": True,
                                          "message": rmsa_error_string(status)}
                print(f"{key:42s} FAILED", file=sys.stderr)
                continue
            S = np.asarray(S, float)
            rel = float(np.max(np.abs(S - expect))/np.max(np.abs(expect)))
            iters = int(getattr(sol, "rescaleIterations", -1))
            if iters > 0:
                engaged += 1
            if rel < 1e-3:
                agreed += 1
            results["points"][key] = {
                "maxRelDiff": rel,
                "rescaleIterations": iters,
                #The flag that makes the result interpretable: a point where
                #the rescaling ran is NOT a weak-coupling point, whatever it
                #was meant to be, and its agreement or disagreement says
                #nothing about the question being asked.
                "rescalingEngaged": iters > 0,
                "ourRange": [float(S.min()), float(S.max())],
                "refRange": [float(expect.min()), float(expect.max())]}
            print(f"{key:42s} rel {rel:.3e}  rescaleIter={iters}",
                  file=sys.stderr)
        except Exception as exc:
            results["points"][key] = f"{type(exc).__name__}: {str(exc)[:80]}"
            print(f"{key:42s} {type(exc).__name__}", file=sys.stderr)

    n = len(ref["points"])
    results["summary"] = (f"{agreed}/{n} agree to 1e-3; rescaling engaged at "
                          f"{engaged} of {n}")
    #The conclusion depends on BOTH numbers, so state it rather than leaving
    #it to be inferred from two counts.
    if engaged == 0 and agreed == n:
        results["conclusion"] = (
            "Agreement where the rescaling does not engage. Combined with the "
            "6-29 per cent disagreement at strong coupling, this isolates the "
            "difference to the Hayter-Penfold rescaling, not the MSA solution.")
    elif engaged == 0:
        results["conclusion"] = (
            "Disagreement even WITHOUT rescaling, so the difference is in the "
            "mean-spherical solution itself and the rescaling is not the "
            "explanation. Check the conversion in the reference generator "
            "before concluding anything about either implementation.")
    else:
        results["conclusion"] = (
            f"The rescaling engaged at {engaged} of {n} points intended to be "
            f"weak-coupling, so those points do not test what they were meant "
            f"to. Lower gamma and regenerate the reference.")
    with open(OUT, "w") as fh:
        json.dump(results, fh, indent=2, sort_keys=True)
        fh.write("\n")
    print(f"\n{results['summary']}\n{results['conclusion']}", file=sys.stderr)


if __name__ == "__main__":
    main()
