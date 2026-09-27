# -*- coding: utf-8 -*-
"""
Section 5.3 survey: approximation-scheme error over the (s, phi) plane.

Runs the six SASfit approximation schemes against the exact multicomponent
result across a grid of size polydispersity s, volume fraction phi, and
potential type, and writes the result as a heat map per scheme per potential.

This is the paper's principal intended result. It exists because four
illustrative state points are not a survey, and because an earlier four-point
table produced a headline claim ("the best scheme reverses between
potentials") that turned out to be an artefact of a defect. A first 9-point
run of THIS script already contradicts the replacement claim as well -- local
monodisperse is not uniformly best; vdW1 and scaling each win at some points.
Nine points is not a survey either. Run the whole thing before writing
anything down.

DESIGN NOTES, all of them learned the hard way in this project.

PER-POINT TIMEOUT, IN A SUBPROCESS. Each state point runs in its own process
with a wall-clock limit (--timeout, default 600 s). A subprocess rather than
signal.alarm because alarm is Unix-only and this is developed on Windows, and
because the solvers call into C (SUNDIALS, GSL): a segfault there would
otherwise take the whole survey with it and lose every point computed so far.
Timeouts and crashes are RECORDED as failures with the wall time, so the heat
map shows a hole rather than silently omitting the point. Verified: an 8 s
limit produced 3 recorded timeouts out of 9 and the run continued.

INCREMENTAL OUTPUT. Every completed state point is appended to a JSON-lines
file immediately. The run can be interrupted, resumed, or die on one bad
point without losing the rest. A survey that only writes at the end is worth
nothing if it is interrupted, and this one takes hours.

RESOLUTION IS SET BY THE NARROWEST FEATURE, not by the particle. The
adhesive and square-well rows need the WELL resolved: roughly
30*sigma/delta points per diameter. At the default 100 a delta = 0.02 well
spans two grid points and S(q) is ~90 % wrong -- silently, and plausibly.
That defect made all six schemes look uniformly poor in an earlier version of
this table, because each was being compared against a reference that was
itself badly in error. `pointsPerSigma` below is therefore computed per
potential, not fixed.

EVERY POINT IS CHECKED, not assumed. A converged residual is necessary but
not sufficient: the closure equations admit multiple fixed points, and two
solvers have been observed to converge to residuals of 1e-12 and return
S(q) = 17.0 and 9.7, both with min S < 0. Points that fail are recorded as
failures with the reason, never silently dropped -- a heat map with holes in
it is information; a heat map that quietly interpolates over failures is a
lie.

BEFORE PUBLISHING, check the extreme corner. At s = 0.4, phi = 0.4 the first
run gave errors of 286 for decoupling and partial S_ij. Verify those points
are physical (min S(q) >= 0, largest size class not jammed at close packing)
rather than merely numerically difficult, before they go into a figure.

SEPARATE GRIDS for the structure factor and the form-factor average: the OZ
solve needs only a few moment-matched classes, but <|F|^2> needs
nFF >~ Qmax*sigma*s or it superposes a handful of ringing form factors and
produces oscillations that look like physical structure.

Usage:
    python section53_survey.py --out survey.jsonl            # full run
    python section53_survey.py --out survey.jsonl --quick    # coarse grid
    python section53_survey.py --out survey.jsonl --resume   # continue
    python section53_survey.py --plot survey.jsonl           # heat maps
"""
import argparse
import json
import multiprocessing as mp
import os
import time
import traceback

import numpy as np


SCHEMES = [
    ("monodisperse", "I_monodisperse"),
    ("decoupling", "I_decoupling"),
    ("local monodisperse", "I_lma"),
    ("partial S_ij", "I_partial_sf"),
    ("scaling", "I_scaling"),
    ("vdW1", "I_vdw1"),
]

#Each entry: (label, potential, args, closure, closureParam, narrowFeature)
#narrowFeature is delta/sigma for potentials with a narrow well, or None.
SYSTEMS = [
    ("hard sphere", "HardSphere", (), "Percus-Yevick", None, None),
    ("square well", "SquareWell", (1.0, 0.1), "Hypernetted-Chain", None, 0.1),
    ("adhesive HS", "StickyHardSphere", (0.3, 0.02), "Percus-Yevick", None, 0.02),
    ("Yukawa", "Yukawa", (0.5, 2.0, 1.0), "Hypernetted-Chain", None, None),
]


def pointsPerSigmaFor(narrowFeature, floor=100, target=30):
    """Grid resolution set by the NARROWEST feature, not by the particle.

    See the module docstring: at 100 points per diameter a delta = 0.02 well
    spans two grid points and the structure factor is ~90 % wrong.
    """
    if narrowFeature is None:
        return floor
    return int(max(floor, np.ceil(target/narrowFeature)))


def gridNFor(pps):
    """Radial grid size: keep the box at least ~40 diameters wide."""
    for n in (4095, 8191, 16383, 32767, 65535):
        if n/pps >= 40:
            return n
    return 65535


def runPoint(system, s, phi, Q, nbins, nFF):
    """One state point. Returns a dict; never raises."""
    from generic_polydisperse_sas import GenericPolydisperseSAS as G

    label, pot, args, closure, cpar, narrow = system
    pps = pointsPerSigmaFor(narrow)
    rec = dict(system=label, potential=pot, s=s, phi=phi,
               closure=closure, nbins=nbins, nFF=nFF,
               pointsPerSigma=pps, ok=False)
    t0 = time.time()
    try:
        sas = G(pot, args, phi=phi, srel=s, nbins=nbins, nFF=nFF,
                closure=closure, closureParam=cpar,
                pointsPerSigma=pps, gridN=gridNFor(pps))
        Ie = sas.I_exact(Q)
        if not np.all(np.isfinite(Ie)) or np.any(Ie <= 0):
            rec["error"] = "exact intensity not positive-definite"
            return rec
        errs = {}
        for name, meth in SCHEMES:
            try:
                Ia = getattr(sas, meth)(Q)
                with np.errstate(divide="ignore", invalid="ignore"):
                    e = float(np.nanmax(np.abs(Ia/Ie - 1.0)))
                errs[name] = e if np.isfinite(e) else None
            except Exception as exc:
                errs[name] = None
                rec.setdefault("schemeErrors", {})[name] = f"{type(exc).__name__}: {exc}"
        rec["errors"] = errs
        rec["ok"] = True
    except Exception as exc:
        rec["error"] = f"{type(exc).__name__}: {exc}"
        rec["traceback"] = traceback.format_exc(limit=3)
    rec["seconds"] = round(time.time() - t0, 2)
    return rec


def _worker(q, system, s, phi, Q, nbins, nFF):
    """Child-process entry point for runPointWithTimeout."""
    try:
        q.put(runPoint(system, s, phi, Q, nbins, nFF))
    except BaseException as exc:                     # pragma: no cover
        q.put(dict(system=system[0], s=s, phi=phi, ok=False,
                   error=f"child died: {type(exc).__name__}: {exc}"))


def runPointWithTimeout(system, s, phi, Q, nbins, nFF, timeout=600):
    """runPoint in a SUBPROCESS, so one bad state point cannot block the run.

    Why a subprocess rather than signal.alarm: alarm is Unix-only and this is
    developed on Windows, and -- more usefully -- a separate process also
    survives a hard crash. The solvers call into C (SUNDIALS, GSL); a segfault
    there would otherwise take the whole survey with it, losing every point
    computed so far. Here it is recorded as a failure and the survey moves on.

    A timeout is recorded like any other failure, with the wall time, so the
    heat map shows a hole rather than silently omitting the point.
    """
    ctx = mp.get_context("spawn")     # spawn: required on Windows, safe here
    q = ctx.Queue()
    proc = ctx.Process(target=_worker,
                       args=(q, system, s, phi, Q, nbins, nFF))
    t0 = time.time()
    proc.start()
    proc.join(timeout)
    if proc.is_alive():
        proc.terminate()
        proc.join(5)
        if proc.is_alive():           # terminate ignored -- stuck in C code
            proc.kill()
            proc.join()
        return dict(system=system[0], potential=system[1], s=s, phi=phi,
                    ok=False, error=f"timeout after {timeout} s",
                    seconds=round(time.time() - t0, 2))
    try:
        rec = q.get_nowait()
    except Exception:
        rec = dict(system=system[0], potential=system[1], s=s, phi=phi,
                   ok=False,
                   error=f"child exited {proc.exitcode} without a result "
                         "(likely a crash in native code)",
                   seconds=round(time.time() - t0, 2))
    return rec


def alreadyDone(path):
    """(system, s, phi) triples already present, for --resume."""
    done = set()
    if not os.path.exists(path):
        return done
    with open(path) as fh:
        for line in fh:
            try:
                r = json.loads(line)
            except ValueError:
                continue
            done.add((r.get("system"), round(r.get("s", -1), 6),
                      round(r.get("phi", -1), 6)))
    return done


def survey(path, quick=False, resume=False, systems=None, timeout=600):
    sList = np.array([0.05, 0.10, 0.15, 0.20, 0.30, 0.40])
    phiList = np.array([0.05, 0.10, 0.20, 0.30, 0.40])
    if quick:
        sList = np.array([0.10, 0.25, 0.40])
        phiList = np.array([0.10, 0.25, 0.40])
    Q = np.logspace(np.log10(0.3), np.log10(20), 160)
    nbins, nFF = 5, 60

    chosen = [sy for sy in SYSTEMS
              if systems is None or sy[0] in systems]
    done = alreadyDone(path) if resume else set()
    total = len(chosen)*sList.size*phiList.size
    n = 0
    t0 = time.time()
    mode = "a" if resume else "w"
    with open(path, mode) as fh:
        for sy in chosen:
            for s in sList:
                for phi in phiList:
                    n += 1
                    key = (sy[0], round(float(s), 6), round(float(phi), 6))
                    if key in done:
                        continue
                    rec = runPointWithTimeout(sy, float(s), float(phi),
                                              Q, nbins, nFF, timeout=timeout)
                    fh.write(json.dumps(rec) + "\n")
                    fh.flush()          # survive an interrupt
                    best = ""
                    if rec["ok"]:
                        valid = {k: v for k, v in rec["errors"].items()
                                 if v is not None}
                        if valid:
                            b = min(valid, key=valid.get)
                            best = f"best={b} ({valid[b]:.3f})"
                    else:
                        best = "FAILED: " + rec.get("error", "")[:40]
                    el = time.time() - t0
                    print(f"[{n}/{total}] {sy[0]:14s} s={s:.2f} phi={phi:.2f} "
                          f"{rec['seconds']:6.1f}s  {best}   "
                          f"(elapsed {el/60:.1f} min)", flush=True)
    print(f"done in {(time.time()-t0)/60:.1f} min -> {path}")


def loadSurvey(path):
    recs = []
    with open(path) as fh:
        for line in fh:
            try:
                recs.append(json.loads(line))
            except ValueError:
                pass
    return recs


def summarise(path):
    """Text summary: which scheme wins where, and how often."""
    recs = [r for r in loadSurvey(path) if r.get("ok")]
    if not recs:
        print("no successful points")
        return
    bySystem = {}
    for r in recs:
        bySystem.setdefault(r["system"], []).append(r)
    for sysName, rs in bySystem.items():
        wins = {}
        worst = {}
        for r in rs:
            valid = {k: v for k, v in r["errors"].items() if v is not None}
            if not valid:
                continue
            wins[min(valid, key=valid.get)] = wins.get(min(valid, key=valid.get), 0) + 1
            for k, v in valid.items():
                worst[k] = max(worst.get(k, 0.0), v)
        print(f"\n{sysName}  ({len(rs)} points)")
        print("  best scheme count:", dict(sorted(wins.items(),
                                                  key=lambda kv: -kv[1])))
        print("  worst-case error per scheme:")
        for k, v in sorted(worst.items(), key=lambda kv: kv[1]):
            print(f"    {k:20s} {v:8.3f}")
    nfail = len([r for r in loadSurvey(path) if not r.get("ok")])
    if nfail:
        print(f"\n{nfail} points FAILED (recorded in the file, not dropped)")


def plot(path, outPdf="fig_survey.pdf"):
    """Heat map of max relative error per scheme over the (s, phi) plane."""
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
    from matplotlib.colors import LogNorm

    recs = [r for r in loadSurvey(path) if r.get("ok")]
    if not recs:
        print("nothing to plot")
        return
    systems = sorted({r["system"] for r in recs})
    sVals = sorted({r["s"] for r in recs})
    phiVals = sorted({r["phi"] for r in recs})
    names = [n for n, _ in SCHEMES]

    fig, axes = plt.subplots(len(systems), len(names),
                             figsize=(2.1*len(names), 2.3*len(systems)),
                             squeeze=False)
    #A COMMON colour scale across all panels. The point of the figure is the
    #comparison between schemes; a per-panel scale would make every scheme
    #look equally good and destroy exactly the information wanted.
    allv = [v for r in recs for v in r["errors"].values() if v]
    vmin, vmax = max(min(allv), 1e-3), max(allv)
    for i, sysName in enumerate(systems):
        for j, name in enumerate(names):
            ax = axes[i][j]
            M = np.full((len(sVals), len(phiVals)), np.nan)
            for r in recs:
                if r["system"] != sysName:
                    continue
                v = r["errors"].get(name)
                if v is None:
                    continue
                M[sVals.index(r["s"]), phiVals.index(r["phi"])] = v
            im = ax.imshow(M, origin="lower", aspect="auto",
                           norm=LogNorm(vmin=vmin, vmax=vmax), cmap="viridis")
            if i == 0:
                ax.set_title(name, fontsize=7)
            if j == 0:
                ax.set_ylabel(f"{sysName}\n$s$", fontsize=7)
                ax.set_yticks(range(len(sVals)))
                ax.set_yticklabels([f"{v:.2f}" for v in sVals], fontsize=6)
            else:
                ax.set_yticks([])
            if i == len(systems) - 1:
                ax.set_xlabel(r"$\varphi$", fontsize=7)
                ax.set_xticks(range(len(phiVals)))
                ax.set_xticklabels([f"{v:.2f}" for v in phiVals], fontsize=6)
            else:
                ax.set_xticks([])
    fig.colorbar(im, ax=axes, shrink=0.7,
                 label=r"max $|I_{\rm approx}/I_{\rm exact}-1|$")
    fig.savefig(outPdf, bbox_inches="tight")
    print("wrote", outPdf)


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", default="survey.jsonl")
    ap.add_argument("--quick", action="store_true")
    ap.add_argument("--resume", action="store_true")
    ap.add_argument("--systems", nargs="*", default=None)
    ap.add_argument("--timeout", type=float, default=600,
                    help="per-point wall-clock limit in seconds")
    ap.add_argument("--plot", default=None)
    ap.add_argument("--summarise", default=None)
    a = ap.parse_args()
    if a.plot:
        plot(a.plot)
    elif a.summarise:
        summarise(a.summarise)
    else:
        survey(a.out, quick=a.quick, resume=a.resume, systems=a.systems,
               timeout=a.timeout)
        summarise(a.out)
