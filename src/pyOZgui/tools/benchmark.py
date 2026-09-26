#!/usr/bin/env python3
"""Timing benchmark for the manuscript's (still missing) performance table.

    cd src/pyozgui/tools
    python benchmark.py > benchmark.json

Writes JSON to stdout and progress to stderr, so the progress does not
contaminate the data. Takes a few minutes.

WHY THIS EXISTS. The manuscript's central claim is that the exact
multicomponent treatment is "cheap enough to be the default", and there is
not one measured number behind it. These are the numbers it needs, and they
must come from a STATED machine -- record the CPU and Python/NumPy versions
alongside the output, which the script collects automatically.

WHAT IT MEASURES, and why each matters:

  classes      time per solve against the number of size classes p. The OZ
               solve is O(p^2) transforms; this shows the constant.
  schemes      time for each of the six approximation schemes once Gamma is
               converged. They should be nearly free -- that is the design
               argument for computing all six alongside the exact result,
               and it is currently unsupported by data.
  solvers      time per solve for each available solver on the same problem.
  grid         time against points per diameter, which dominates the
               narrow-well systems.
  transform    type-1 against type-4 at matched grids AND at matched
               accuracy. The second is the interesting one: type 4 is second
               order, so it reaches a given accuracy on a far coarser grid.
  verify       cost of ozLib.solve(verify=True) against verify=False.
  fit          a full three-parameter fit, which is what a user actually
               waits for.
"""
import json
import os
import platform
import sys
import time

import numpy as np


#---------------------------------------------------------------------------
#Put the package on sys.path. This script lives in tools/ while the modules
#it needs live in src/pyozgui/, so neither is importable from the other
#without help. Identical to the block in section53_survey.py -- which had the
#same bug, was fixed, and then this file was written without carrying the fix
#across.
_HERE = os.path.dirname(os.path.abspath(__file__))
for _cand in (_HERE,                              # script sits with the modules
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
    raise ImportError(
        f"cannot locate ozLib.py relative to {_HERE!r}. Set PYTHONPATH to the "
        "directory containing the pyozgui modules, or run this script from "
        "within that directory.")


def _say(msg):
    print(msg, file=sys.stderr, flush=True)


#Write after EVERY block, not once at the end. The first version dumped JSON
#only after all seven blocks finished, so when block 5 died on a bad import
#it threw away the four blocks that had already run. section53_survey.py
#learned this same lesson and flushes every state point; this file was
#written without carrying it across.
#
#Note the results go to a FILE, not just stdout: `python benchmark.py >
#benchmark.json` leaves an empty file if the run dies, which is exactly what
#happened. The file is written directly so partial results survive.
_OUTPATH = os.environ.get("BENCHMARK_OUT", "benchmark.json")


def _save(out):
    """Write results so far. Called after each block."""
    try:
        with open(_OUTPATH, "w") as fh:
            json.dump(out, fh, indent=2)
            fh.write("\n")
    except Exception as exc:                       # pragma: no cover
        print(f"could not write {_OUTPATH}: {exc}", file=sys.stderr)


def _t(fn, repeats=3):
    """Best of `repeats`, in seconds. Best, not mean: we want the machine's
    capability, not its background load."""
    best = float("inf")
    for _ in range(repeats):
        t0 = time.perf_counter()
        fn()
        best = min(best, time.perf_counter() - t0)
    return round(best, 4)


def main():
    import ozLib
    from generic_polydisperse_sas import GenericPolydisperseSAS as G

    out = {"machine": {
        "processor": platform.processor(),
        "machine": platform.machine(),
        "system": f"{platform.system()} {platform.release()}",
        "python": platform.python_version(),
        "numpy": np.__version__,
    }}
    Q = np.logspace(np.log10(0.3), np.log10(20), 160)

    _say("1/7 classes")
    out["classes"] = {}
    for p in (1, 3, 5, 7, 10):
        out["classes"][p] = _t(lambda p=p: G(
            "HardSphere", (), phi=0.3, srel=0.2, nbins=p, nFF=60,
            closure="Percus-Yevick").I_exact(Q))
    _save(out)

    _say("2/7 schemes")
    sas = G("HardSphere", (), phi=0.3, srel=0.2, nbins=5, nFF=60,
            closure="Percus-Yevick")
    out["schemes"] = {"I_exact": _t(lambda: sas.I_exact(Q))}
    for m in ("I_monodisperse", "I_decoupling", "I_lma", "I_partial_sf",
              "I_scaling", "I_vdw1"):
        out["schemes"][m] = _t(lambda m=m: getattr(sas, m)(Q))
    _save(out)

    _say("3/7 solvers")
    out["solvers"] = {}
    for name in ozLib.SOLVER_CLASSES:
        try:
            out["solvers"][name] = _t(lambda name=name: ozLib.solve(
                "HardSphere", phi=0.3, closure="Percus-Yevick",
                solver=name, verify=False, maxIterations=8000), repeats=2)
        except Exception as exc:
            out["solvers"][name] = f"FAILED: {type(exc).__name__}"
    _save(out)

    _say("4/7 grid")
    out["grid"] = {}
    for pps, N in ((50, 2047), (100, 4095), (200, 8191), (400, 16383),
                   (800, 32767)):
        out["grid"][pps] = _t(lambda pps=pps, N=N: ozLib.solve(
            "HardSphere", phi=0.3, closure="Percus-Yevick", verify=False,
            numberOfRadialSamplingPoints=N, hardSphereDiameterInPoints=pps,
            maxIterations=8000), repeats=2)
    _save(out)

    _say("5/7 transform type")
    import picardOZsolver

    #Inlined rather than imported from examples/educational_oz.py: that file
    #is deliberately outside the package (it is teaching material, imported
    #by nothing), so depending on it here made block 5 fail with
    #ModuleNotFoundError after blocks 1-4 had already run -- and, because
    #this script only wrote its JSON at the very end, threw their results
    #away. Exact Percus-Yevick hard-sphere S(q), Wertheim 1963/Thiele 1963.
    def analyticPercusYevickS(q, eta, sigma=1.0):
        k = np.asarray(q, float)*sigma
        a = (1 + 2*eta)**2/(1 - eta)**4
        b = -6*eta*(1 + eta/2)**2/(1 - eta)**4
        d = eta*a/2
        s_, co = np.sin(k), np.cos(k)
        i1 = (s_ - k*co)/k**3
        i2 = (2*k*s_ - (k*k - 2)*co - 2)/k**4
        i4 = ((4*k**3 - 24*k)*s_ - (k**4 - 12*k*k + 24)*co + 24)/k**6
        chat = -4*np.pi*sigma**3*(a*i1 + b*i2 + d*i4)
        return 1.0/(1.0 - (6*eta/(np.pi*sigma**3))*chat)

    def timedSolve(tt, pps, N):
        class S(picardOZsolver.PicardOZsolver):
            def __init__(self, *a, **k):
                super().__init__(*a, **k)
                self.transformType = tt
        t0 = time.perf_counter()
        s = S(port=0, numberOfRadialSamplingPoints=N,
              hardSphereDiameterInPoints=pps)
        s.setNumberOfIterations(20000)
        s.setVolumeDensity(0.3)
        s.setPotentialByName("HardSphere")
        s.doPYclosure()
        s.solve()
        dt = time.perf_counter() - t0
        q = np.asarray(s.getqArray())
        Sq = np.real(np.asarray(s.getSq()))
        m = (q > 0.3) & (q < 25)
        ex = analyticPercusYevickS(q[m], 0.3)
        err = float(np.max(np.abs(Sq[m] - ex))/np.max(np.abs(ex)))
        return dict(seconds=round(dt, 4), error=err)

    out["transform"] = {}
    #GRID SIZE PER TRANSFORM TYPE. These used to be fixed at 2^k - 1 for
    #both, which is right for the DST-I and wrong for the DST-IV: the former
    #goes through an FFT of length 2(N+1), the latter is a length-N
    #transform, so 16383 = 3 x 43 x 127 costs type 4 about a factor of 3.
    #Every earlier type-4 timing here was taken on that penalised grid, which
    #is why the second-order transform appeared SLOWER than the first-order
    #one at fine resolutions. Accuracy is unaffected either way.
    from oZfixpointOperator import bestGridSize
    for tt in (1, 4):
        for pps, N in ((100, 4095), (400, 16383), (800, 32767)):
            n = bestGridSize(N, tt)
            rec = timedSolve(tt, pps, n)
            rec["gridN"] = int(n)
            out["transform"][f"type{tt}_pps{pps}"] = rec
    _save(out)

    _say("6/7 verification cost")
    out["verify"] = {
        "off": _t(lambda: ozLib.solve("HardSphere", phi=0.3,
                                      closure="Percus-Yevick", verify=False),
                  repeats=2),
        "on": _t(lambda: ozLib.solve("HardSphere", phi=0.3,
                                     closure="Percus-Yevick", verify=True),
                 repeats=2)}
    _save(out)

    _say("7/7 full fit")
    try:
        from polydisperse_fit import PolydisperseFit
        rng = np.random.default_rng(1)
        Qd = np.logspace(np.log10(0.004), np.log10(0.25), 80)
        ref = G("HardSphere", (), phi=0.15, srel=0.15, nbins=3, nFF=60,
                closure="Percus-Yevick", meanRadius=50.0)
        It = 2.0e-3*ref.I_exact(Qd) + 0.01
        dI = 0.03*It
        Iobs = It + rng.normal(0, dI)
        P = {"meanRadius": (44., 25., 80.), "srel": (0.25, 0.03, 0.5),
             "phi": (0.25, 0.03, 0.45)}
        t0 = time.perf_counter()
        f = PolydisperseFit(Qd, Iobs, dI, potential="HardSphere",
                            closure="Percus-Yevick", parameters=P,
                            nbins=3, nFF=60)
        res = f.run(maxNfev=100)
        out["fit"] = dict(seconds=round(time.perf_counter() - t0, 3),
                          chi2=round(float(res["chi2_reduced"]), 4),
                          nfev=res.get("nfev"))
    except Exception as exc:
        out["fit"] = f"FAILED: {type(exc).__name__}: {exc}"

    _save(out)
    json.dump(out, sys.stdout, indent=2)
    print()
    _say(f"done -- results also written to {_OUTPATH}")


if __name__ == "__main__":
    main()
