#!/usr/bin/env python3
"""Is route (b) worth taking: equally spaced classes + type 4, or quadrature
classes + type 1?

    cd src/pyOZgui/tools
    python route_b_test.py

Writes route_b_test.json; progress to stderr, no redirect needed.

THE QUESTION. Type 4 is second order but needs every pair core sigma_ij to
fall between grid points. Moment-matched quadrature nodes are irrational, so
that is generically unsatisfiable; EQUALLY SPACED classes satisfy it easily.
But moment matching is the reason three classes reproduce a continuous
distribution at all. So: does the second-order gain outweigh losing
moment-exactness?

TWO SEPARATE THINGS, deliberately measured apart, because conflating them is
the obvious way to get a wrong answer:

  A. SOLVER accuracy -- given a FIXED discrete mixture, how well does our
     solver reproduce mixscatter's analytic Vrij solution? This isolates the
     transform. Compared on the SOLVER'S NATIVE q GRID: going through
     mixscatter_bridge's interpolation floors any comparison at ~1e-4 and
     hides the whole effect.

  B. DISCRETISATION accuracy -- given a continuous Schulz distribution, how
     well does each node set reproduce its moments? This isolates the
     quadrature and needs no solver at all. <sigma^3> and <sigma^6> govern
     I(Q -> 0).

A tells us what type 4 buys. B tells us what equal spacing costs. Route (b)
wins only if A's gain exceeds B's loss AT THE SAME p -- and since the solve
is O(p^2), needing more classes is expensive.
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

OUT = os.environ.get("ROUTE_B_OUT", "route_b_test.json")
SREL = 0.20          # relative width of the Schulz distribution
PHI = 0.20


def _say(m):
    print(m, file=sys.stderr, flush=True)


def _save(d):
    with open(OUT, "w") as fh:
        json.dump(d, fh, indent=2)
        fh.write("\n")


def equallySpacedClasses(p, srel, meanSigma=1.0, nWidths=2.5):
    """p equally spaced classes across mean +- nWidths*sigma, Schulz-weighted.

    Equal spacing is what makes type-4 alignment achievable: sigma_ij then
    takes only 2p-1 evenly spaced values instead of p(p+1)/2 irrational ones.
    """
    from scipy.stats import gamma as gammadist
    lo = max(meanSigma*(1.0 - nWidths*srel), 1e-3)
    hi = meanSigma*(1.0 + nWidths*srel)
    sig = np.linspace(lo, hi, p)
    t = 1.0/srel**2 - 1.0
    w = gammadist.pdf(sig*(t + 1.0)/meanSigma, t + 1.0)
    return sig, w/w.sum()


def nativeGridError(sigma, x, phi, pps, N, transformType):
    """Max relative difference from mixscatter, on the SOLVER'S OWN q grid.

    Reuses `OZLiquidStructure._buildPotential`, which is a staticmethod and
    installs an explicit set of size classes. Earlier versions of this
    function guessed at `setPolydisperseHardSphereMixture` and
    `getSijMatrix`, neither of which exists.

    The comparison is deliberately NOT made through the bridge: the bridge
    interpolates onto the caller's q grid and floors any comparison at
    ~1.2e-04, which is enough to hide a 770x effect entirely. Here both sides
    are evaluated on the solver's own q points.
    """
    import mixscatter as ms
    import picardOZsolver
    import ozLib
    from mixscatter_bridge import OZLiquidStructure as OZ

    L = float(np.sum(x*sigma))            # mean diameter, the reduced unit
    sigmaReduced = sigma/L

    sol = picardOZsolver.PicardOZsolver(
        port=0, numberOfRadialSamplingPoints=N,
        hardSphereDiameterInPoints=pps)
    sol.transformType = transformType
    sol.setNumberOfIterations(8000)
    sol.setVolumeDensity(phi)

    OZ._buildPotential(sol, "HardSphere", (), sigmaReduced, x)
    if transformType == 4:
        sol.checkTransformAlignment()
    sol.doPYclosure()
    t0 = time.perf_counter()
    sol.solve()
    dt = time.perf_counter() - t0

    q = np.asarray(sol.getqArray(), float)/L      # back to caller units
    m = (q > 0.05) & (q < 20.0)
    mix = ms.Mixture(radius=sigma/2.0, number_fraction=x)
    ref = np.asarray(ms.PercusYevick(
        q[m], mix, volume_fraction_total=phi
    ).number_weighted_partial_structure_factor, float)
    own = np.asarray(sol.partialStructureFactor, float)[:, :, m]
    err = float(np.max(np.abs(own - ref))/np.max(np.abs(ref)))
    return dict(error=err, seconds=round(dt, 4), nq=int(m.sum()))


def main():
    import polydisperse_nodes as PN

    out = {"settings": dict(srel=SREL, phi=PHI),
           "B_discretisation": {}, "A_solver": {}}

    # ---- B: what does equal spacing COST? (no solver involved) -----------
    _say("B: discretisation accuracy (moments)")
    exact = PN.analyticMoments("Schulz", SREL, 7, 1.0)
    for p in (3, 5, 7, 10, 15):
        rec = {}
        try:
            sq, xq = PN.sizeClasses("Schulz", SREL, p, 1.0)
            rec["quadrature"] = {
                f"m{k}": abs(float(np.sum(xq*sq**k))/exact[k] - 1.0)
                for k in (3, 6)}
        except Exception as exc:
            rec["quadrature"] = f"{type(exc).__name__}: {exc}"
        se, xe = equallySpacedClasses(p, SREL)
        rec["equal"] = {f"m{k}": abs(float(np.sum(xe*se**k))/exact[k] - 1.0)
                        for k in (3, 6)}
        out["B_discretisation"][p] = rec
        _save(out)

    # ---- A: what does type 4 BUY? (fixed discrete mixture) ---------------
    _say("A: solver accuracy on the native grid")
    for p in (3, 5):
        se, xe = equallySpacedClasses(p, SREL)
        for tt, pps, N in ((1, 200, 8191), (4, 200, 8191),
                           (1, 400, 16383), (4, 400, 16383)):
            key = f"p{p}_type{tt}_pps{pps}"
            _say(f"   {key}")
            try:
                out["A_solver"][key] = nativeGridError(se, xe, PHI, pps, N, tt)
            except Exception as exc:
                out["A_solver"][key] = f"{type(exc).__name__}: {str(exc)[:130]}"
            _save(out)

    _save(out)
    _say(f"done -- {OUT}")


if __name__ == "__main__":
    main()
