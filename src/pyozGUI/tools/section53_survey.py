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
file immediately and flushed. The run can be interrupted, resumed, or die on
one bad point without losing the rest. A survey that only writes at the end
is worth nothing if it is interrupted, and this one takes hours.

RESOLUTION IS SET BY THE NARROWEST FEATURE, not by the particle. The
adhesive and square-well systems need the WELL resolved: roughly
30*sigma/delta points per diameter. At the default 100 a delta = 0.02 well
spans two grid points and S(q) is ~90 % wrong -- silently, and plausibly.
That defect made all six schemes look uniformly poor in an earlier version of
this table, because each was being compared against a reference that was
itself badly in error. `pointsPerSigma` is therefore computed per potential,
not fixed: hard sphere and Yukawa get 100, square well 300, adhesive 1500.

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
    python section53_survey.py --summarise survey.jsonl      # text summary
    python section53_survey.py --plot survey.jsonl           # heat maps
"""
import argparse
import json
import multiprocessing as mp
import os
import sys
import time
import traceback

import numpy as np


#---------------------------------------------------------------------------
#Put the package on sys.path. This script lives in tools/ while the modules
#it needs live in src/pyozgui/, so neither is importable from the other
#without help.
#
#This MUST be at module level, not inside main(): the "spawn" start method
#re-imports this module in every child process, so anything done only in
#main() is absent there. That is exactly how this failed -- a run reported
#120 consecutive "child died: ModuleNotFoundError" in about thirty seconds,
#because the parent never needed the import and only the child did.
_HERE = os.path.dirname(os.path.abspath(__file__))
for _cand in (_HERE,                              # script sits with the modules
              os.path.join(_HERE, os.pardir, "src", "pyozgui"),
              os.path.join(_HERE, os.pardir, "src", "pyOZgui"),
              os.path.join(_HERE, os.pardir, "pyozgui"),
              os.path.join(_HERE, os.pardir)):
    _cand = os.path.abspath(_cand)
    if os.path.isfile(os.path.join(_cand, "generic_polydisperse_sas.py")):
        if _cand not in sys.path:
            sys.path.insert(0, _cand)
        break
else:
    #Fail loudly and once, rather than 120 times inside child processes.
    raise ImportError(
        "cannot locate generic_polydisperse_sas.py relative to "
        f"{_HERE!r}. Set PYTHONPATH to the directory containing the pyozgui "
        "modules, or run this script from within that directory.")


SCHEMES = [
    ("monodisperse", "I_monodisperse"),
    ("decoupling", "I_decoupling"),
    ("local monodisperse", "I_lma"),
    ("partial S_ij", "I_partial_sf"),
    ("scaling", "I_scaling"),
    ("vdW1", "I_vdw1"),
]

#Each entry: (label, potential, args, closure, closureParam, narrowFeature)
#narrowFeature is delta/sigma for potentials with a narrow well, else None.
SYSTEMS = [
    ("hard sphere", "HardSphere", (), "Percus-Yevick", None, None),
    ("square well", "SquareWell", (1.0, 0.1), "Hypernetted-Chain", None, 0.1),
    ("adhesive HS", "StickyHardSphere", (0.3, 0.02), "Percus-Yevick", None, 0.02),
    ("Yukawa", "Yukawa", (0.5, 2.0, 1.0), "Hypernetted-Chain", None, None),
]


def solverCascade():
    """Solvers in the order the manual documents, filtered to what is present.

    The manual says: "Anderson acceleration (SUNDIALS KINSOL KIN_FP) is tried
    first, with Newton-Krylov (GMRES with line search) and damped Picard
    iteration as successive fallbacks." The survey used to take whatever
    GenericPolydisperseSAS defaulted to, which is Picard -- so it did the
    slowest thing everywhere, and disagreed with its own documentation.

    SUNDIALS bindings are optional and `ozLib.SOLVER_CLASSES` only carries a
    KINSOL entry where they are installed, so the tiers are matched by
    substring and missing ones are skipped rather than raising.

    `scipy Anderson` and `Biggs-Andrews` are included as further fallbacks.
    Both are fast once configured: scipy Anderson was the SLOWEST solver
    available until its `alpha` was set (see scipyAndersonOZsolver.solve --
    it is 15 to 66 times faster with alpha = 1.0), and Biggs-Andrews is
    marginally the quickest of all on the points measured. Neither is in the
    documented cascade, so both sit below Picard rather than above it; move
    them up only after --compare-solvers on the systems in question.

    Returns a list of solver NAMES (keys of ozLib.SOLVER_CLASSES).
    """
    import ozLib
    avail = list(ozLib.SOLVER_CLASSES)
    tiers = (("kin",),                              # SUNDIALS KINSOL KIN_FP
             ("anderson acceleration",),             # in-house Anderson
             ("newton",),                            # Newton-Krylov / GMRES
             ("picard",),                            # damped Picard
             ("scipy anderson",),                    # needs alpha=1.0 set
             ("biggs",))                             # fast, undocumented
    order = []
    for tier in tiers:
        for want in tier:
            hit = [k for k in avail
                   if want in k.lower() and k not in order]
            if hit:
                order.append(hit[0])
                break
    #Anything installed but unlisted still gets a turn, last.
    order += [k for k in avail if k not in order]
    return order


def solveWithCascade(potential, args, phi, s, nbins, nFF, closure, cpar,
                     pps, gridN, Q, solvers=None):
    """Try each solver in turn; return (sas, Ie, record of what happened).

    Records the wall time of EVERY attempt, not just the successful one, so a
    timing table can be built from the survey output: users need to know what
    each strategy costs to decide which gives useful information first.

    MEASURED, hard spheres, five classes, 160 Q points:

        s=0.20 phi=0.20   Picard 1.74 s   Anderson 0.56 s   Newton-Krylov 1.20 s
        s=0.10 phi=0.40   Picard 10.80 s  Anderson 1.60 s   Newton-Krylov 2.81 s

    i.e. 3.1x and 6.8x over Picard, the gain GROWING with the difficulty of
    the point -- which is where the survey spends its time. All solvers agreed
    to 1e-11 or better, so the reference does not move.

    An aside worth knowing, from --compare-solvers: `scipy Anderson` was the
    slowest solver available -- 28 s on a point the others did in 0.7 s --
    purely because scipy's `alpha` was left unset. With alpha = 1.0 it is the
    fastest. See scipyAndersonOZsolver.solve for the measurements.

    NOTE ON CORRECTNESS, not just speed. Different solvers have been observed
    to converge to DIFFERENT fixed points on this problem -- at one strongly
    coupled state two accelerated solvers returned S(Q) = 17.0 and 9.7, both
    with residuals ~1e-12 and both with min S(Q) < 0. The survey measures
    approximations against "the exact result", so if the solver lands on a
    different root the reference itself moves. Use --compare-solvers to check
    agreement whenever the cascade or a potential changes.
    """
    import ozLib
    from generic_polydisperse_sas import GenericPolydisperseSAS as G

    names = solvers if solvers is not None else solverCascade()
    timings, attempts = {}, []
    for name in names:
        cls, _linear = ozLib.SOLVER_CLASSES[name]
        t0 = time.time()
        try:
            sas = G(potential, args, phi=phi, srel=s, nbins=nbins, nFF=nFF,
                    closure=closure, closureParam=cpar,
                    pointsPerSigma=pps, gridN=gridN, solverClass=cls)
            Ie = sas.I_exact(Q)
            dt = time.time() - t0
            timings[name] = round(dt, 3)
            if np.all(np.isfinite(Ie)) and np.all(Ie > 0):
                attempts.append(f"{name}: ok in {dt:.1f} s")
                return sas, Ie, dict(solverUsed=name, solverTimings=timings,
                                     solverAttempts=attempts)
            attempts.append(f"{name}: non-positive-definite after {dt:.1f} s")
        except Exception as exc:
            dt = time.time() - t0
            timings[name] = round(dt, 3)
            attempts.append(f"{name}: {type(exc).__name__} after {dt:.1f} s")
    return None, None, dict(solverUsed=None, solverTimings=timings,
                            solverAttempts=attempts)


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


def runPoint(system, s, phi, Q, nbins, nFF, solvers=None):
    """One state point. Returns a dict; never raises."""
    label, pot, args, closure, cpar, narrow = system
    pps = pointsPerSigmaFor(narrow)
    rec = dict(system=label, potential=pot, s=s, phi=phi,
               closure=closure, nbins=nbins, nFF=nFF,
               pointsPerSigma=pps, ok=False)
    t0 = time.time()
    try:
        sas, Ie, solverInfo = solveWithCascade(
            pot, args, phi, s, nbins, nFF, closure, cpar,
            pps, gridNFor(pps), Q, solvers=solvers)
        rec.update(solverInfo)
        if sas is None:
            rec["error"] = ("no solver produced a positive-definite exact "
                            "intensity; see solverAttempts")
            #Set seconds on THIS path too. It was missing, and because this is
            #an early return the survey loop then raised KeyError('seconds')
            #on the first such point -- losing the run. Any early return from
            #this function must populate every field the caller reads.
            rec["seconds"] = round(time.time() - t0, 2)
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
                rec.setdefault("schemeErrors", {})[name] = \
                    f"{type(exc).__name__}: {exc}"
        rec["errors"] = errs
        rec["ok"] = True
    except Exception as exc:
        rec["error"] = f"{type(exc).__name__}: {exc}"
        rec["traceback"] = traceback.format_exc(limit=3)
    rec["seconds"] = round(time.time() - t0, 2)
    return rec


def _worker(q, system, s, phi, Q, nbins, nFF):
    """Child-process entry point for runPointWithTimeout."""
    t0 = time.time()
    try:
        q.put(runPoint(system, s, phi, Q, nbins, nFF))
    except BaseException as exc:                     # pragma: no cover
        q.put(dict(system=system[0], potential=system[1], s=s, phi=phi,
                   ok=False, seconds=round(time.time() - t0, 2),
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

    NOTE the "spawn" start method re-imports the main module in the child, so
    this must be called from a script or behind an `if __name__ == "__main__"`
    guard -- not from an interactive session or piped stdin, where the child
    cannot find __main__ and exits immediately.
    """
    ctx = mp.get_context("spawn")     # spawn: required on Windows, safe here
    q = ctx.Queue()
    proc = ctx.Process(target=_worker,
                       args=(q, system, s, phi, Q, nbins, nFF))
    t0 = time.time()
    proc.start()
    #WAIT ON THE QUEUE, NOT ON THE PROCESS. proc.join(timeout) is the obvious
    #way to write this and it does NOT reliably return: a child that has put
    #its result on the queue cannot exit until the queue's feeder thread has
    #been drained by the parent, so join() and the child can wait on each
    #other. Observed directly here -- an 8 s limit on hard spheres fired
    #correctly, but adhesive-sphere points ran 36 minutes past a 400 s limit
    #without the timeout firing at all. Blocking on q.get() both collects the
    #result and bounds the wait.
    rec = None
    try:
        rec = q.get(timeout=timeout)
    except Exception:
        rec = None
    if rec is None:
        #Distinguish the two failure modes by WALL TIME, not by is_alive() or
        #the exit code: once terminated, a timed-out child reports exitcode
        #-15 (SIGTERM) from our own terminate(), which reads exactly like a
        #crash. Elapsed time is unambiguous.
        elapsed = time.time() - t0
        timedOut = elapsed >= 0.95*timeout
        if proc.is_alive():
            proc.terminate()
            proc.join(5)
            if proc.is_alive():       # terminate ignored -- stuck in C code
                proc.kill()
                proc.join()
        return dict(system=system[0], potential=system[1], s=s, phi=phi,
                    ok=False,
                    error=(f"timeout after {timeout} s" if timedOut else
                           f"child exited {proc.exitcode} without a result "
                           f"after {elapsed:.1f} s (likely a crash in "
                           f"native code)"),
                    seconds=round(elapsed, 2))
    proc.join(10)
    if proc.is_alive():               # result collected; child lingering
        proc.terminate()
        proc.join(5)
    return rec


def alreadyDone(path):
    """(system, s, phi) triples already present AND SUCCESSFUL, for --resume.

    Deliberately excludes failed points, so --resume RETRIES them. The
    alternative -- skipping anything with a record -- means a transient
    failure is permanent: a run that failed on every point because of an
    import error would leave a file that --resume then skips entirely,
    reporting "no successful points" for ever. That happened.

    Consequence to be aware of: a point that fails deterministically (a
    genuine non-convergence, or a timeout on a state point that is simply too
    slow) will be retried on every resume. Delete or filter the file if that
    becomes tiresome.
    """
    done = set()
    if not os.path.exists(path):
        return done
    with open(path) as fh:
        for line in fh:
            try:
                r = json.loads(line)
            except ValueError:
                continue
            if not r.get("ok"):
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

    chosen = [sy for sy in SYSTEMS if systems is None or sy[0] in systems]
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
                    if rec["ok"]:
                        valid = {k: v for k, v in rec["errors"].items()
                                 if v is not None}
                        b = min(valid, key=valid.get) if valid else "?"
                        note = (f"best={b} ({valid[b]:.3f})" if valid
                                else "no valid scheme")
                    else:
                        note = "FAILED: " + rec.get("error", "")[:40]
                    #.get, not [...]: a record from an unexpected path may be
                    #missing a field, and the survey must not die reporting
                    #progress. This exact line raised KeyError('seconds').
                    print(f"[{n}/{total}] {sy[0]:14s} s={s:.2f} phi={phi:.2f} "
                          f"{rec.get('seconds', float('nan')):7.1f}s  {note}   "
                          f"(elapsed {(time.time()-t0)/60:.1f} min)", flush=True)
    print(f"done in {(time.time()-t0)/60:.1f} min -> {path}")


def compareSolvers(system=None, s=0.20, phi=0.20, nbins=5, nFF=60):
    """Time every available solver on one state point and check AGREEMENT.

    Speed alone is not enough to justify switching: different solvers have
    landed on different fixed points on this problem. This reports both, so
    the decision is made on evidence rather than on the speed-up alone.

    Run it whenever the cascade order changes, a new solver is installed, or
    a new potential is added.
    """
    import ozLib
    system = system or SYSTEMS[0]
    Q = np.logspace(np.log10(0.3), np.log10(20), 160)
    base = None
    print(f"{system[0]}  s={s}  phi={phi}  ({nbins} classes, {Q.size} Q)")
    print(f"{'solver':24s} {'wall':>9s}  {'agreement vs first':>19s}")
    rest = [k for k in ozLib.SOLVER_CLASSES if k not in solverCascade()]
    for name in solverCascade() + rest:
        rec = runPoint(system, s, phi, Q, nbins, nFF, solvers=[name])
        t = (rec.get("solverTimings") or {}).get(name, float("nan"))
        if not rec.get("ok"):
            print(f"{name:24s} {t:9.2f}s  FAILED: {rec.get('error','')[:30]}")
            continue
        e = rec["errors"].get("local monodisperse")
        if base is None:
            base, agree = e, 0.0
        else:
            agree = abs(e/base - 1.0) if base else float("nan")
        print(f"{name:24s} {t:9.2f}s  {agree:19.2e}")


def solverTimingSummary(path):
    """Per-solver wall times harvested from a completed survey file.

    This is the data behind the timing table the manuscript needs: what each
    strategy costs per Ornstein-Zernike solve, so a user can judge which will
    give useful information first.
    """
    recs = loadSurvey(path)
    agg, used = {}, {}
    for r in recs:
        for name, t in (r.get("solverTimings") or {}).items():
            agg.setdefault(name, []).append(t)
        u = r.get("solverUsed")
        if u:
            used[u] = used.get(u, 0) + 1
    if not agg:
        print("no solver timings in this file -- it predates the cascade. "
              "Rerun, or delete it and rerun, to collect them.")
        return
    print("wall time per state point, by solver:")
    for name, v in sorted(agg.items(), key=lambda kv: np.median(kv[1])):
        v = np.asarray(v, float)
        print(f"  {name:24s} n={v.size:4d} median={np.median(v):8.2f}s "
              f"worst={v.max():9.2f}s")
    print("solver that succeeded:",
          dict(sorted(used.items(), key=lambda kv: -kv[1])))


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
        wins, worst = {}, {}
        for r in rs:
            valid = {k: v for k, v in r["errors"].items() if v is not None}
            if not valid:
                continue
            b = min(valid, key=valid.get)
            wins[b] = wins.get(b, 0) + 1
            for k, v in valid.items():
                worst[k] = max(worst.get(k, 0.0), v)
        print(f"\n{sysName}  ({len(rs)} points)")
        print("  best-scheme count:",
              dict(sorted(wins.items(), key=lambda kv: -kv[1])))
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
    im = None
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
    ap.add_argument("--compare-solvers", action="store_true",
                    dest="compareSolvers",
                    help="time every solver on one point and check agreement")
    ap.add_argument("--solver-timing", default=None, dest="solverTiming",
                    help="per-solver wall times from a completed survey file")
    a = ap.parse_args()
    if a.compareSolvers:
        compareSolvers()
    elif a.solverTiming:
        solverTimingSummary(a.solverTiming)
    elif a.plot:
        plot(a.plot)
    elif a.summarise:
        summarise(a.summarise)
    else:
        survey(a.out, quick=a.quick, resume=a.resume, systems=a.systems,
               timeout=a.timeout)
        summarise(a.out)
