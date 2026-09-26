#!/usr/bin/env python3
"""Verify the manuscript's validation table against references that are here.

    cd src/pyOZgui/tools
    python validation_table_test.py

Writes validation_table_test.json; progress to stderr.

WHY. Four rows of that table were measured against jscatter, which does not
build under MinGW -- its dependency chain pulls in MDAnalysis, JupyterLab,
nglview, pdb2pqr and netCDF4. So the numbers could not be re-run on the
machine the paper is written on, and re-measuring one of them showed why
that matters: the one-component Percus-Yevick row, quoted at 0.12 per cent,
turned out to be a DST-IV figure. With the first-order DST-I at the same
resolution the same comparison gives 0.70 to 2.33 per cent, and the table
did not say which transform produced it.

This script uses only references that live in this repository:

    Percus-Yevick      analytic_py_hardsphere, the Wertheim-Thiele closed
                       form written out from the literature -- independent
                       of any other implementation, and agreeing with
                       jscatter to 1e-8 where jscatter is available
    RMSA               rmsaWrapper, the Hayter-Penfold C library
    adhesive spheres   jscatter if present, else skipped; the convergence
                       behaviour is checked instead, which is what the
                       manuscript actually claims about that row

so it runs wherever the package does.

WHAT IT IS NOT. Agreement here does not validate the PHYSICS -- the closed
forms and this solver implement the same equations, so they should agree and
a disagreement means one of them has a bug. What it measures is the
DISCRETISATION error of the numerical route against an exact answer, which
is the quantity the table reports and the one that changes when the grid,
the resolution or the transform changes.
"""
import json
import os
import sys

import numpy as np

_HERE = os.path.dirname(os.path.abspath(__file__))
for _cand in (os.path.join(_HERE, os.pardir, "src", "pyozgui"),
              os.path.join(_HERE, os.pardir), _HERE):
    _cand = os.path.abspath(_cand)
    if os.path.isfile(os.path.join(_cand, "ozLib.py")):
        if _cand not in sys.path:
            sys.path.insert(0, _cand)
        break
else:
    raise ImportError("cannot locate ozLib.py")
if _HERE not in sys.path:
    sys.path.insert(0, _HERE)

OUT = os.environ.get("VALIDATION_TABLE_OUT", "validation_table_test.json")
RESULTS = {}


def _say(m):
    print(m, file=sys.stderr, flush=True)


def _save():
    with open(OUT, "w") as fh:
        json.dump(RESULTS, fh, indent=2, sort_keys=True)
        fh.write("\n")


def record(name, value, note=""):
    RESULTS[name] = {"value": value, "note": note}
    _save()


# ---------------------------------------------------------------------------
def rowPercusYevick():
    """One-component PY, against the Wertheim-Thiele closed form.

    Reported per transform type AND per resolution, because that is exactly
    what the table failed to state. The polydisperse route cannot use type 4
    -- it needs every pair core between grid points and the sigma_ij of a
    mixture are incommensurate -- so the type-1 column is the one that
    applies to this paper's subject.
    """
    from analytic_py_hardsphere import structureFactorPY
    from generic_polydisperse_sas import GenericPolydisperseSAS as G
    from oZfixpointOperator import bestGridSize

    q = np.linspace(0.1, 25.0, 200)
    table = {}
    for phi in (0.2, 0.3, 0.4):
        ref = structureFactorPY(q, 0.5, phi)
        row = {}
        for tt in (1, 4):
            for pps in (100, 400):
                #Grid scaled with the resolution so r_max is unchanged --
                #N/pps = 40.96 sigma throughout. Refining pps at fixed N
                #would shrink the box and measure the trade rather than the
                #discretisation.
                N = bestGridSize(4095*(pps//100), tt)
                sas = G("HardSphere", (), phi=phi, srel=1e-6, nbins=1,
                        closure="Percus-Yevick", meanRadius=0.5,
                        gridN=N, pointsPerSigma=pps, transformType=tt)
                S = np.asarray(sas.S_partials(q), float)[:, 0, 0]
                err = float(np.max(np.abs(S - ref))/np.max(np.abs(ref)))
                row[f"type{tt}_pps{pps}"] = err*100.0
                row[f"type{tt}_pps{pps}_rmax"] = N/pps
        table[f"phi={phi}"] = row
    record("percusYevick", table,
           "per cent, max relative over 0.1 <= q <= 25. The table's quoted "
           "0.12 % corresponds to the type-4 column; type 1 is an order of "
           "magnitude worse and is what the polydisperse route uses.")


# ---------------------------------------------------------------------------
def rowRMSA():
    """RMSA against the Hayter-Penfold library, nine state points.

    The table claims 'exact (9/9)'. That is a strong claim and worth
    re-checking: it means the numerical route reproduces the analytic one to
    machine precision at every point, not merely closely.

    Needs librmsa.dll/.so beside rmsaWrapper.py. Skipped with a note rather
    than failed if absent, since the library is built separately.
    """
    try:
        from rmsaWrapper import rmsa_compute
    except Exception as exc:
        record("rmsa", None, f"skipped: {type(exc).__name__}: {exc}")
        return

    q = np.linspace(0.05, 20.0, 200)
    R = 0.5
    points = [(phi, gam, scl)
              for phi in (0.1, 0.2, 0.3)
              for gam, scl in ((5.0, 0.5), (10.0, 0.3), (20.0, 0.2))]
    out = {}
    for phi, gam, scl in points:
        key = f"phi={phi},gamma={gam},scl={scl}"
        try:
            #rmsa_compute returns a TUPLE (structureFactor, status, solution),
            #not an object with an .S attribute. Reading it as the latter
            #produced nine identical ValueErrors -- the wrapper was working
            #from the first call and the test was wrong, which is worth
            #noting because the error message pointed at numpy rather than
            #at the unpacking.
            S, status, sol = rmsa_compute(R, scl, gam, phi, q)
            S = np.asarray(S, float)
            out[key] = {"status": int(status),
                        "min_S": float(np.min(S)),
                        "max_S": float(np.max(S)),
                        "finite": bool(np.all(np.isfinite(S))),
                        "physical": bool(np.min(S) > -1e-9),
                        "rescaled": bool(
                            getattr(sol, "rescaleIterations", 0) > 0),
                        "rescaleIterations": int(
                            getattr(sol, "rescaleIterations", -1))}
        except Exception as exc:
            out[key] = f"{type(exc).__name__}: {str(exc)[:80]}"
    record("rmsa", out,
           "rmsa_compute at the nine state points behind the table's "
           "'exact (9/9)'. Without a SECOND Hayter-Penfold implementation "
           "there is nothing here to compare against, so this records only "
           "that each point solves, returns a physical S(Q), and whether "
           "the rescaling actually engaged -- which is the part that "
           "distinguishes RMSA from plain MSA and the part most likely to "
           "be silently skipped. The 9/9 claim itself was measured against "
           "jscatter and cannot be re-verified without it.")


# ---------------------------------------------------------------------------
def rowAdhesiveSpheres():
    """Adhesive spheres: the CONVERGENCE claim, which needs no reference.

    The manuscript does not quote an agreement figure for this row -- it
    points at the under-resolved-well section, whose claim is that the error
    is governed by the number of points ACROSS THE WELL and falls at first
    order in dr/delta. That claim can be checked without any external
    reference at all, by refining and watching the ratio: a first-order
    error halves for a doubling, and an error that does NOT shrink is
    something else.

    Published table, for comparison: 2 points -> 0.916, 4 -> 0.324,
    8 -> 0.142, 16 -> 0.068, 32 -> 0.035.
    """
    from generic_polydisperse_sas import GenericPolydisperseSAS as G
    from oZfixpointOperator import bestGridSize

    q = np.linspace(0.1, 25.0, 200)
    phi, tau, delta = 0.2, 0.2, 0.02
    #The finest grid is the reference: at 64 points across the well it is
    #some thirty times better resolved than the coarsest, which is enough
    #for the ratios below to mean something.
    fine = None
    curves = {}
    for pps in (100, 200, 400, 800, 1600, 3200):
        N = bestGridSize(4095*max(pps//100, 1), 1)
        try:
            sas = G("StickyHardSphere", (tau, delta), phi=phi, srel=1e-6,
                    nbins=1, closure="Percus-Yevick", meanRadius=0.5,
                    gridN=N, pointsPerSigma=pps, transformType=1)
            curves[pps] = np.asarray(sas.S_partials(q), float)[:, 0, 0]
        except Exception as exc:
            curves[pps] = f"{type(exc).__name__}: {str(exc)[:60]}"
    ok = {k: v for k, v in curves.items() if not isinstance(v, str)}
    if len(ok) < 3:
        record("adhesiveSpheres", {k: str(v)[:80] for k, v in curves.items()},
               "too few resolutions solved to judge convergence")
        return
    fineKey = max(ok)
    fine = ok[fineKey]
    errs = {}
    for pps, S in sorted(ok.items()):
        if pps == fineKey:
            continue
        errs[f"{pps*delta:.0f}_across_well"] = float(
            np.max(np.abs(S - fine))/np.max(np.abs(fine)))
    keys = sorted(errs, key=lambda s: float(s.split("_")[0]))
    ratios = [errs[a]/errs[b] for a, b in zip(keys, keys[1:]) if errs[b] > 0]
    record("adhesiveSpheres",
           {"errors_vs_finest": errs,
            "successive_ratios": ratios,
            "reference_resolution": f"{fineKey*delta:.0f} points across well"},
           "self-convergence, no external reference needed. Ratios near 2 "
           "for each doubling is first order in dr/delta, which is what the "
           "manuscript claims; near 1 would mean the error is not "
           "discretisation. Published: 2 pts -> 0.916, 8 -> 0.142, "
           "32 -> 0.035.")


# ---------------------------------------------------------------------------
def main():
    for name, fn in (("Percus-Yevick vs closed form", rowPercusYevick),
                     ("RMSA via the Hayter-Penfold library", rowRMSA),
                     ("adhesive spheres, self-convergence",
                      rowAdhesiveSpheres)):
        _say(name)
        try:
            fn()
        except Exception as exc:
            import traceback
            record(fn.__name__, None, f"RAISED {type(exc).__name__}: {exc}")
            traceback.print_exc()
    _say(f"done -- {OUT}")


if __name__ == "__main__":
    main()
