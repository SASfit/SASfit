#!/usr/bin/env python3
"""Check this package's numerical two-Yukawa MSA against the reference.

    cd src/pyOZgui/tools
    python two_yukawa_test.py

Writes two_yukawa_test.json; progress to stderr.

WHAT THIS ANSWERS.

The manuscript's validation table quotes agreement with jscatter's two-Yukawa
MSA at 0.15 to 0.28 per cent. Two problems with that row as it stands:

  * it was measured where jscatter was installed, and jscatter's dependency
    chain does not build under MinGW, so it cannot be re-run on the machine
    the paper is written on;
  * it reads as a comparison against an independent implementation, and it is
    not. jscatter's docstring says its Igor ancestor was "based in part on
    Matlab code supplied by Yun Liu"; the Matlab in FormFactors4SASfit/TYSQ12
    shares that ancestry. All of them are one lineage from the paper's own
    author.

`two_yukawa_reference.py` is that solver lifted out, needing only numpy, so
the row becomes reproducible. This script does the comparison and records it.

HOW TO READ THE RESULT. Because the reference is not independent, agreement
does not validate the PAPER's equations -- it validates our discretisation
against the author's own code. A disagreement of a few tenths of a per cent
therefore points at OUR grid rather than at theirs, and should shrink under
refinement. One that does NOT shrink is a defect, exactly as in the mixture
validation tab.

CONSTRAINT FROM THE ORIGINAL AUTHORS: Z1 > Z2 is required. With Z1 < Z2 the
reference returns a number (0) rather than an array, because the mathematics
treats the two screening lengths asymmetrically and the result would look
like a structure factor while being wrong. Every case below respects it, and
the last check confirms the refusal still works.
"""
import json
import os
import sys

import numpy as np

_HERE = os.path.dirname(os.path.abspath(__file__))
for _cand in (_HERE,
              os.path.join(_HERE, os.pardir, "src", "pyozgui"),
              os.path.join(_HERE, os.pardir)):
    _cand = os.path.abspath(_cand)
    if os.path.isfile(os.path.join(_cand, "ozLib.py")):
        if _cand not in sys.path:
            sys.path.insert(0, _cand)
        break
else:
    raise ImportError(f"cannot locate ozLib.py relative to {_HERE!r}")
if _HERE not in sys.path:
    sys.path.insert(0, _HERE)

OUT = os.environ.get("TWO_YUKAWA_OUT", "two_yukawa_test.json")


def _say(m):
    print(m, file=sys.stderr, flush=True)


def _save(d):
    with open(OUT, "w") as fh:
        json.dump(d, fh, indent=2, sort_keys=True)
        fh.write("\n")


#Parameter sets. The first is the one the authors' own sample script uses
#(TwoYukawaSample.m: Z1=10, Z2=2, K1=6 attractive, K2=-1 repulsive,
#phi=0.2), so it is the case their code was demonstrated on. The rest vary
#one thing at a time: the volume fraction, then the strengths, then the
#screening contrast. Z1 > Z2 throughout.
CASES = (
    dict(name="authors_sample", K1=6.0, K2=-1.0, Z1=10.0, Z2=2.0, phi=0.20),
    dict(name="phi_0.10", K1=6.0, K2=-1.0, Z1=10.0, Z2=2.0, phi=0.10),
    dict(name="phi_0.35", K1=6.0, K2=-1.0, Z1=10.0, Z2=2.0, phi=0.35),
    dict(name="weak_attraction", K1=2.0, K2=-1.0, Z1=10.0, Z2=2.0, phi=0.20),
    dict(name="no_repulsion", K1=6.0, K2=0.0, Z1=10.0, Z2=2.0, phi=0.20),
    dict(name="close_screening", K1=6.0, K2=-1.0, Z1=6.0, Z2=4.0, phi=0.20),
)

#Radial resolutions for the convergence check. A genuine discretisation
#error falls roughly fourfold for a fourfold refinement; one that does not
#shrink is something else.
#
#THREE resolutions, not two. With only 100 and 400 a ratio near 2 cannot be
#told apart from an error approaching a floor: both give one number. A third
#point distinguishes them -- 4x then 4x again should give the same ratio
#twice if it is really first order, and a falling ratio means the error is
#bottoming out. phi = 0.35 gave 2.13 against the authors_sample 4.44, and
#that is the question this answers.
RESOLUTIONS = (100, 400, 1600)

#q RANGES. The error is a maximum over q, so where it is taken matters as
#much as the grid. The manuscript quotes 0.15 to 0.28 per cent for this
#model, and the same comparison here gave 0.86 per cent at 400 points --
#so either the published number used a finer grid or a narrower window, and
#it is worth knowing which.
#
#"full" spans the structure-factor peak and well past it; "peak" keeps only
#the region a scattering measurement actually resolves. The high-q tail is
#where a first-order transform is worst, since S(Q) oscillates fastest
#there, so a narrow window flatters the agreement -- which is exactly why
#both are reported rather than one.
Q_RANGES = {
    "full": (0.2, 25.0),
    "peak": (1.0, 12.0),
}


def numericalSq(q, case, pps):
    """S(Q) from this package's solver for the same two-Yukawa potential.

    Reduced units throughout: the hard-core diameter is 1, so q here is
    Q*sigma and the screening lengths are in units of sigma -- the same
    convention the reference uses, which is why no rescaling appears.
    """
    import ozLib
    from oZfixpointOperator import bestGridSize
    #The potential is "HardSphereDoubleYukawa", taking (K1, z1, K2, z2) --
    #verified against PicardOZsolver's setter rather than guessed, after a
    #first attempt used a name that does not exist.
    #
    #Note also that ozLib.solve names its grid arguments
    #numberOfRadialSamplingPoints and hardSphereDiameterInPoints, NOT gridN
    #and pointsPerSigma: those are GenericPolydisperseSAS's names. The two
    #layers spell the same quantities differently, which is worth knowing
    #before writing a call from memory.
    #THE GRID MUST SCALE WITH THE RESOLUTION.
    #
    #r_max = N*sigma/pps, so raising pps at FIXED N shrinks the real-space
    #box by the same factor. At N = 4095 and pps = 1600 the box is 2.6 sigma
    #-- far too small to contain the correlations, which is why the error
    #GREW roughly tenfold at the finest resolution in every case and the
    #"reaching a floor" verdict was nonsense.
    #
    #This is the identical mistake found and fixed in the mixture validation
    #tab earlier in the same session. Scaling both keeps r_max constant, so
    #a finer grid is a genuine refinement; N stays of the form 2^k - 1 to
    #keep the DST-I's underlying FFT a power of two.
    nScaled = bestGridSize(4096*pps//RESOLUTIONS[0] - 1, 1)
    sol = ozLib.solve(
        "HardSphereDoubleYukawa",
        potentialArgs=(case["K1"], case["Z1"], case["K2"], case["Z2"]),
        phi=case["phi"], closure="MSA",
        numberOfRadialSamplingPoints=nScaled,
        hardSphereDiameterInPoints=pps)
    #ozLib.solve returns an OZResult, whose q and Sq are plain arrays --
    #NOT the solver, so getqArray()/getSq() do not exist here. Those belong
    #to the solver object, reachable as sol.solverInstance if ever needed.
    #Three different spellings of the same two quantities across the layers;
    #each was found by reading rather than by assuming, after guessing wrong
    #twice.
    qs = np.asarray(sol.q, float)
    Ss = np.real(np.asarray(sol.Sq, float))
    #Interpolate onto the requested q in log-log: S(Q) is smooth and the
    #solver's own q grid is fixed by the radial one.
    good = (qs > 0) & np.isfinite(Ss) & (Ss > 0)
    return np.exp(np.interp(np.log(q), np.log(qs[good]), np.log(Ss[good])))


def main():
    results = {"cases": {}}

    _say("importing the reference")
    try:
        from two_yukawa_reference import twoYukawa
        results["reference"] = "two_yukawa_reference.twoYukawa"
    except ImportError:
        #Optional, though no longer for licensing reasons: the authors
        #granted redistribution in writing, conditional only on citing
        #J. Chem. Phys. 122, 044507 (2005). Kept optional because a user may
        #simply not have copied it in, and skipping cleanly is better than a
        #traceback from a tool that has other cases to report.
        results["reference"] = (
            "NOT INSTALLED -- two_yukawa_reference.py is absent. It is "
            "optional: the two-Yukawa MSA solver of Liu, Chen & Chen, "
            "J. Chem. Phys. 122, 044507 (2005), obtainable from jscatter "
            "(jscatter.libs.Two_Yukawa) or from the authors. Place it in "
            "this directory to enable this comparison.")
        results["FAILURES"] = []
        _save(results)
        _say("reference not installed -- nothing to compare; see the JSON")
        return
    except Exception as exc:
        results["reference"] = f"UNUSABLE: {type(exc).__name__}: {exc}"
        _save(results)
        raise
    _save(results)

    #Reduced units: hard-core diameter 1, hence radius 0.5.
    for rangeName, (qlo, qhi) in Q_RANGES.items():
        q = np.logspace(np.log10(qlo), np.log10(qhi), 120)
        results.setdefault("ranges", {})[rangeName] = [qlo, qhi]
        for case in CASES:
            key = f"{case['name']}__{rangeName}"
            _say(f"case {key}")
            entry = dict(case)
            entry["qRange"] = [qlo, qhi]
            try:
                ref = np.asarray(
                    twoYukawa(q, 0.5, case["K1"], case["K2"],
                              case["Z1"], case["Z2"], case["phi"]), float)
                if ref.ndim == 0:
                    entry["reference"] = "no physical root (scalar returned)"
                    results["cases"][key] = entry
                    _save(results)
                    continue
                entry["referenceRange"] = [float(ref.min()), float(ref.max())]
            except Exception as exc:
                entry["reference"] = f"RAISED {type(exc).__name__}: {exc}"
                results["cases"][key] = entry
                _save(results)
                continue

            errs = {}
            for pps in RESOLUTIONS:
                try:
                    num = numericalSq(q, case, pps)
                    errs[pps] = float(np.max(np.abs(num - ref))
                                      / np.max(np.abs(ref)))
                except ValueError as exc:
                    msg = str(exc)
                    errs[pps] = (f"MULTIPLE ROOTS: {msg[:120]}"
                                 if "disagree" in msg
                                 else f"RAISED ValueError: {msg[:120]}")
                except Exception as exc:
                    errs[pps] = f"RAISED {type(exc).__name__}: {exc}"
            entry["maxRelError"] = {str(k): v for k, v in errs.items()}
            #Successive ratios, one per 4x step. Equal ratios near 4 is
            #first order. A ratio BELOW 1 means the error grew, which is not
            #a floor but a broken comparison -- that is what a fixed grid
            #with rising pps produced, by shrinking the real-space box.
            ratios = []
            vals = [errs.get(p) for p in RESOLUTIONS]
            for lo, hi in zip(vals, vals[1:]):
                ratios.append(lo/hi if (isinstance(lo, float)
                                        and isinstance(hi, float)
                                        and hi > 0) else None)
            entry["successiveRatios"] = ratios
            clean = [x for x in ratios if isinstance(x, float)]
            if clean:
                if any(x < 1.0 for x in clean):
                    entry["verdict"] = (
                        "ERROR GREW under refinement -- not convergence at "
                        "all; suspect the real-space range rather than the "
                        "model")
                elif all(x > 3.0 for x in clean):
                    entry["verdict"] = "first order throughout"
                elif len(clean) > 1 and clean[-1] < clean[0]:
                    entry["verdict"] = "reaching a floor"
                else:
                    entry["verdict"] = "below first order"
            results["cases"][key] = entry
            _save(results)

    #The authors' asymmetry constraint. Included because it is the one way to
    #use this reference and get a plausible wrong answer.
    _say("Z1 < Z2 refusal")
    try:
        bad = twoYukawa(q, 0.5, 6.0, -1.0, 2.0, 10.0, 0.20)
        results["z1LessThanZ2"] = (
            "correctly refused (scalar returned)" if np.ndim(bad) == 0
            else "RETURNED AN ARRAY -- the Z1 > Z2 guard is not working, and "
                 "the result would look like a structure factor while being "
                 "wrong")
    except Exception as exc:
        results["z1LessThanZ2"] = f"raised {type(exc).__name__}: {exc}"
    _save(results)

    #A case FAILS when the comparison could not be made at all. A large but
    #shrinking error is a result, not a failure -- and "no physical root" is
    #the reference declining to answer, which is also a result.
    bad = []
    for k, v in results["cases"].items():
        e = v.get("maxRelError")
        if not isinstance(e, dict):
            continue                      # reference declined; reported above
        if all(not isinstance(x, float) for x in e.values()):
            bad.append(f"{k} (no resolution succeeded)")
    results["FAILURES"] = bad
    _save(results)
    _say(f"done -- {OUT}; {len(bad)} failure(s): {bad}" if bad
         else f"done -- {OUT}; all cases compared")


if __name__ == "__main__":
    main()
