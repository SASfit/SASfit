#!/usr/bin/env python3
"""Check the transformType wiring in GenericPolydisperseSAS.

    cd src/pyOZgui/tools
    python transform_test.py

Writes transform_test.json; progress to stderr, no redirect needed.

Three things must hold for transformType to be trustworthy:

  1. REGRESSION. transformType=1 must give bit-identical results to before
     the parameter existed. If this moves, the wiring broke something.
  2. ALIGNMENT ENFORCEMENT. A mixture whose sigma_ij land ON grid points
     must RAISE, not silently return first-order results. A misaligned
     type-4 grid was measured at three orders of magnitude worse.
  3. ACCURACY. With an aligned grid, type 4 must beat type 1 against the
     analytic Vrij mixture solution (mixscatter), and converge at second
     order (ratio 4 per halving, against 2 for first).

WHY "TRUSTWORTHY" AND NOT "A DEFAULT", which is what this said before. The
question of whether type 4 is good enough to default to has since been
answered, emphatically: measured against the exact Percus-Yevick
compressibility at phi = 0.40, type 1 gives S(0) low by 5.27 per cent and
type 4 by 0.036 -- a factor of 148 on the SAME grid, because the hard core
falls ON a type-1 node and BETWEEN type-4 nodes. See testCompressibility in
numerics_test.py.

So accuracy is not the obstacle. The obstacle is that type 4 cannot be used
for a MIXTURE at all: it needs every pair core between grid points, and the
sigma_ij = (sigma_i + sigma_j)/2 of a moment-matched quadrature are mutually
incommensurate, so no single offset places them all. Check 2 is what enforces
that, and it is load-bearing rather than a formality -- a misaligned type-4
grid returns a plausible answer that is three orders worse. The polydisperse
route is therefore first order and stays so until a mixture-capable
second-order treatment exists.
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

OUT = os.environ.get("TRANSFORM_OUT", "transform_test.json")


def _say(m):
    print(m, file=sys.stderr, flush=True)


def _save(d):
    with open(OUT, "w") as fh:
        json.dump(d, fh, indent=2)
        fh.write("\n")


def main():
    from generic_polydisperse_sas import GenericPolydisperseSAS as G
    import oZfixpointOperator as OZF

    out = {}
    Q = np.logspace(np.log10(0.3), np.log10(20), 160)

    _say("1/3 regression: transformType=1 unchanged")
    out["regression"] = {}
    for label, kw in (("HS_phi0.3_s0.2",
                       dict(potential="HardSphere", potentialArgs=(),
                            phi=0.3, srel=0.2, closure="Percus-Yevick")),
                      ("SW_phi0.2_s0.2",
                       dict(potential="SquareWell", potentialArgs=(1.0, 0.1),
                            phi=0.2, srel=0.2,
                            closure="Hypernetted-Chain"))):
        try:
            sas = G(kw.pop("potential"), kw.pop("potentialArgs"),
                    nbins=5, nFF=60, transformType=1, **kw)
            I = sas.I_exact(Q)
            out["regression"][label] = dict(
                Imax=float(np.nanmax(I)),
                Isum=float(np.nansum(I)))
        except Exception as exc:
            out["regression"][label] = f"{type(exc).__name__}: {exc}"
    _save(out)

    _say("2/3 alignment enforcement")
    out["alignment"] = {}
    #srel>0 with 5 classes gives sigma_ij that generally do NOT align, so
    #type 4 should refuse and name a pointsPerSigma that works.
    for pps in (100, 150, 200):
        try:
            sas = G("HardSphere", (), phi=0.3, srel=0.2, nbins=5, nFF=60,
                    closure="Percus-Yevick", transformType=4,
                    pointsPerSigma=pps, gridN=8191)
            I = sas.I_exact(Q)
            score, _ = OZF.coreAlignmentScore(
                np.asarray(sas.sigmaS, float), 1.0/pps)
            out["alignment"][pps] = dict(
                accepted=True, alignment=round(float(score), 4),
                Imax=float(np.nanmax(I)))
        except ValueError as exc:
            out["alignment"][pps] = dict(accepted=False,
                                         message=str(exc)[:180])
        except Exception as exc:
            out["alignment"][pps] = f"{type(exc).__name__}: {str(exc)[:120]}"
    _save(out)

    _say("3/3 accuracy against mixscatter, aligned grids")
    out["accuracy"] = {}
    try:
        import mixscatter as ms
        R = np.array([0.85, 1.0, 1.15])
        x = np.array([0.25, 0.5, 0.25])
        mix = ms.Mixture(radius=R, number_fraction=x)
        qq = np.linspace(0.05, 20, 200)
        ref = np.asarray(ms.PercusYevick(
            qq, mix, volume_fraction_total=0.2
        ).number_weighted_partial_structure_factor, float)
        out["accuracy"]["note"] = (
            "compared via mixscatter_bridge on the caller's q grid; the "
            "bridge's own interpolation floors this at ~1e-4, so read the "
            "RATIO between type 1 and type 4, not the absolute value")
        from mixscatter_bridge import OZLiquidStructure as OZ
        for tt in (1, 4):
            for pps, N in ((200, 8191), (400, 16383)):
                #Size the grid for the TRANSFORM, not for type 1 alone: the
                #DST-I wants 2^k - 1 (its FFT has length 2(N+1)) and the
                #DST-IV wants 2^k (it is a length-N transform). Using one
                #number for both leaves type 4 on a composite length --
                #16383 = 3 x 43 x 127 -- and costs it about a factor of 3 in
                #time. Accuracy, which is what this block measures, is
                #unaffected; the sizing is here so the two tools agree.
                from oZfixpointOperator import bestGridSize
                N = bestGridSize(N, tt)
                key = f"type{tt}_pps{pps}"
                try:
                    S = OZ(qq, mix, volume_fraction_total=0.2,
                           closure="Percus-Yevick", pointsPerSigma=pps,
                           gridN=N, transformType=tt
                           ).number_weighted_partial_structure_factor
                    e = float(np.max(np.abs(np.asarray(S) - ref))
                              / np.max(np.abs(ref)))
                    out["accuracy"][key] = e
                except Exception as exc:
                    out["accuracy"][key] = f"{type(exc).__name__}: {str(exc)[:110]}"
    except ImportError:
        out["accuracy"] = "mixscatter not installed"
    _save(out)

    _say(f"done -- {OUT}")


if __name__ == "__main__":
    main()
