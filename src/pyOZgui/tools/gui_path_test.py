#!/usr/bin/env python3
"""Exercise the GUI's own Compute and Fit paths, headlessly.

    cd src/pyOZgui/tools
    python gui_path_test.py

Writes gui_path_test.json; progress to stderr, no redirect needed.

WHY THIS EXISTS, and it is worth reading before adding to it.

Session 5 added a great deal to `generic_polydisperse_tab.py` -- scale and
background, a power-law term, resolution smearing across all seven curves, a
fitter selector, warm starting, a chi-squared on computed curves. Nearly
every piece was "verified" by parsing the file or by testing its arithmetic
in isolation, and nearly every piece was broken in the same way: the
machinery worked, and the WIRING between layers did not.

The defects that reached the user, all silent:

  * the form factor was built by the GUI and never passed to the fitter, so
    every core-shell fit silently fitted solid spheres;
  * `scale` was applied by the fit path and not by the calculate path, so a
    fitted curve did not reproduce on recalculation;
  * the solver choice was read from the combobox and dropped;
  * `_currentValue` raised KeyError the moment `shell` was ticked;
  * a chi-squared block masked an array with the wrong grid, raising on
    EVERY Compute with data loaded.

Not one of those survives a single call to `_worker()`. They survived
review, unit tests of the underlying functions, and repeated assurances from
the author that the code was deployed and working.

So: this test presses the buttons. It builds the real tab under a virtual
display, fills the real entries, and calls the real worker functions. It is
worth more than twenty tests of the arithmetic beneath them.

REQUIREMENTS. A display, or xvfb:

    xvfb-run -a python gui_path_test.py      # headless Linux
    python gui_path_test.py                  # anywhere with a display

On Windows a display is always present, so it simply runs.
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

import matplotlib
matplotlib.use("Agg")                      # before any pyplot import

OUT = os.environ.get("GUI_PATH_OUT", "gui_path_test.json")


def _say(m):
    print(m, file=sys.stderr, flush=True)


def _save(d):
    with open(OUT, "w") as fh:
        json.dump(d, fh, indent=2, sort_keys=True)
        fh.write("\n")


def makeTab():
    """A real GenericPolydisperseTab on a real (hidden) root window."""
    import tkinter as tk
    from tkinter import ttk
    import generic_polydisperse_tab as G

    root = tk.Tk()
    root.withdraw()
    nb = ttk.Notebook(root)
    tab = G.GenericPolydisperseTab(nb)
    nb.add(tab, text="test")
    return root, tab


def syntheticData(tab, nQ=40, withDQ=True, seed=0):
    """A small curve from the model itself, so the truth is known.

    Deliberately SMALL: this test is about whether the paths run, not about
    accuracy, and a 40-point curve with 3 size classes keeps a full Compute
    under a few seconds.
    """
    from generic_polydisperse_sas import GenericPolydisperseSAS as G
    Q = np.logspace(np.log10(0.05), np.log10(2.0), nQ)
    ref = G("HardSphere", (), phi=0.20, srel=0.15, nbins=3,
            closure="Percus-Yevick", meanRadius=1.0)
    I = 2.0*ref.I_exact(Q) + 0.05
    dI = 0.02*I
    dQ = 0.05*Q + 0.01 if withDQ else None
    tab.data = (Q, I, dI)
    tab.dQ = dQ
    if dQ is not None:
        tab.smearCheck.configure(state="normal")
    tab.smearVar.set(bool(withDQ))
    tab.fitBtn.configure(state="normal")
    return Q, I, dI


def runWorker(root, tab, timeout=180.0):
    """Call _worker() directly and drain the queue. Returns (result, error).

    The worker is called SYNCHRONOUSLY rather than in a thread: a thread
    would swallow the traceback into the queue and the test would see only
    'an error happened'. Here an exception propagates and names itself.
    """
    p = tab._readInputs()
    tab._worker(p)
    res, err = None, None
    while not tab.resultQueue.empty():
        kind, payload = tab.resultQueue.get_nowait()
        if kind == "done":
            res = payload
        elif kind == "error":
            err = payload
    return res, err


def countSolves(fn, *a, **k):
    """Run `fn` and count how many OZ solves it performs.

    An endless-looking Compute was reported in session 5 and turned out to
    be the pair-sum schemes doing p(p+1)/2 monodisperse solves EACH -- about
    ninety in total at seven size classes, entirely by design. Counting them
    distinguishes 'slow by construction' from 'looping', which took several
    rounds of guesswork to establish by hand.
    """
    import io
    import contextlib
    buf = io.StringIO()
    with contextlib.redirect_stdout(buf):
        out = fn(*a, **k)
    text = buf.getvalue()
    return out, sum(1 for line in text.split("\n")
                    if "converged after" in line)


def main():
    results = {}

    # ---- 1. the tab builds at all ------------------------------------
    _say("1/6 building the tab")
    try:
        root, tab = makeTab()
        results["build"] = "ok"
    except Exception as exc:
        results["build"] = f"{type(exc).__name__}: {exc}"
        _save(results)
        raise
    _save(results)

    # ---- 2. Compute with NO data -------------------------------------
    #The default path, and the one that kept working while the others broke.
    _say("2/6 compute, no data")
    tab.QminVar.set("0.05")
    tab.QmaxVar.set("2.0")
    tab.nQVar.set("40")
    tab.nbinsVar.set("3")
    tab.nFFVar.set("20")
    t0 = time.time()
    try:
        (res, err), nSolves = countSolves(runWorker, root, tab)
        results["computeNoData"] = dict(
            seconds=round(time.time() - t0, 2), solves=nSolves,
            error=None if err is None else f"{type(err[0]).__name__}: {err[0]}",
            finite=bool(res is not None
                        and np.all(np.isfinite(np.asarray(res.I_exact)))))
    except Exception as exc:
        results["computeNoData"] = f"RAISED {type(exc).__name__}: {exc}"
    _save(results)

    # ---- 3. Compute WITH data, unsmeared ------------------------------
    #This is the case that raised on every call in session 5: the chi-squared
    #block masked the intensity with the wrong Q grid. It cannot be reached
    #without loading data, which is why it was never seen in testing.
    _say("3/6 compute with data, smearing OFF")
    syntheticData(tab, withDQ=False)
    t0 = time.time()
    try:
        (res, err), nSolves = countSolves(runWorker, root, tab)
        results["computeDataUnsmeared"] = dict(
            seconds=round(time.time() - t0, 2), solves=nSolves,
            error=None if err is None else f"{type(err[0]).__name__}: {err[0]}",
            chi2=None if res is None else getattr(res, "chi2", None))
    except Exception as exc:
        results["computeDataUnsmeared"] = f"RAISED {type(exc).__name__}: {exc}"
    _save(results)

    # ---- 4. Compute WITH data, smeared --------------------------------
    #The other half of the same defect: with smearing active the model grid
    #and the output grid DIFFER, and code that assumes one length breaks.
    _say("4/6 compute with data, smearing ON")
    syntheticData(tab, withDQ=True)
    t0 = time.time()
    try:
        (res, err), nSolves = countSolves(runWorker, root, tab)
        results["computeDataSmeared"] = dict(
            seconds=round(time.time() - t0, 2), solves=nSolves,
            error=None if err is None else f"{type(err[0]).__name__}: {err[0]}",
            chi2=None if res is None else getattr(res, "chi2", None))
    except Exception as exc:
        results["computeDataSmeared"] = f"RAISED {type(exc).__name__}: {exc}"
    _save(results)

    # ---- 5. every form factor, and the power-law term -----------------
    #Core-shell is the path where the form factor was built and then dropped,
    #so a fit silently used spheres. Exercising both settings of every
    #switch is the cheapest guard against that class of defect.
    _say("5/6 form factors and the power-law background")
    results["variants"] = {}
    for ff in ("Sphere", "Core-shell"):
        for porod in (False, True):
            tab.ffVar.set(ff)
            tab._syncFormFactor()
            tab.porodVar.set(porod)
            tab._syncPorod()
            tab.porodAVar.set("0.001")
            tab.porodDVar.set("1.5")
            tab.porodQminVar.set("0.05")
            key = f"{ff}_porod{'On' if porod else 'Off'}"
            try:
                res, err = runWorker(root, tab)
                results["variants"][key] = (
                    "ok" if err is None
                    else f"{type(err[0]).__name__}: {err[0]}")
            except Exception as exc:
                results["variants"][key] = f"RAISED {type(exc).__name__}: {exc}"
            _save(results)

    # ---- 6. EVERY CONTROL REACHES THE MODEL ---------------------------
    #THE FAILURE MODE THIS WHOLE FILE EXISTS FOR, in its purest form: a
    #control that looks connected and is not. It has happened repeatedly --
    #the form factor built and never passed, the solver choice dropped, the
    #fit flags wiped so ten ticked boxes fitted one parameter, and the Mann
    #damping guarded behind a hasattr that never fired because the attribute
    #does not exist until something sets it.
    #
    #None of those raised. Each produced a plausible result from a model
    #that was not the one on screen. So rather than check that a control
    #EXISTS, spy on the solver and assert the value ARRIVED.
    _say("6/7 do the controls reach the solver?")
    results["controlsReachModel"] = {}
    try:
        import picardOZsolver
        from generic_polydisperse_sas import GenericPolydisperseSAS as _G
        seen = {}

        class _Spy(picardOZsolver.PicardOZsolver):
            def solve(self, *a, **k):
                seen["mannAlpha"] = getattr(self, "mannAlpha", None)
                seen["transformType"] = getattr(self, "transformType", None)
                return super().solve(*a, **k)

        for alpha in (1.0, 0.5):
            seen.clear()
            _G("HardSphere", (), phi=0.2, srel=0.12, nbins=3,
               closure="Percus-Yevick", solverClass=_Spy, mannAlpha=alpha)
            results["controlsReachModel"][f"mannAlpha={alpha}"] = (
                "ok" if seen.get("mannAlpha") == alpha
                else f"LOST: solver saw {seen.get('mannAlpha')!r}")
        #The solver CLASS itself: selectedSolverClass() returning None sends
        #every solve to the default Picard with nothing reported anywhere.
        cls = tab.selectedSolverClass()
        results["controlsReachModel"]["solverClass"] = (
            "NONE -- every solve falls back to the default"
            if cls is None else getattr(cls, "__name__", str(cls)))
    except Exception as exc:
        results["controlsReachModel"] = f"RAISED {type(exc).__name__}: {exc}"
    _save(results)

    # ---- 7. FIT, then COMPUTE, and demand the same chi-squared --------
    #THE INVARIANT THAT KEPT BREAKING. The two paths diverged four separate
    #times in one session -- over the form factor, the scale, the resolution
    #smearing and the power-law term -- and each time a fitted curve failed
    #to reproduce when recalculated. One assertion covers all of them.
    _say("7/7 fit, then compute with the fitted values")
    tab.ffVar.set("Sphere")
    tab._syncFormFactor()
    tab.porodVar.set(False)
    tab._syncPorod()
    tab.smearVar.set(False)
    for name, flag in tab.fitFlags.items():
        flag.set(name in ("phi", "srel", "scale", "background"))
    #THE FLAGS THEMSELVES. A reset left over from an earlier layout wiped
    #every inline checkbox, so fitFlags held only the potential arguments
    #and a fit varied ONE parameter while ten showed ticked. Assert the
    #dictionary contains what the panel offers.
    results["fitFlagsPresent"] = sorted(tab.fitFlags)
    try:
        p = tab._readInputs()
        params = {}
        for n in ("phi", "srel"):
            x = tab._currentValue(n)
            lo, hi = tab.FIT_BOUNDS[n]
            params[n] = (x, min(lo, x*0.999), max(hi, x*1.001))
        tab._fitWorker(p, params)
        fit = None
        while not tab.resultQueue.empty():
            kind, payload = tab.resultQueue.get_nowait()
            if kind == "fit":
                fit = payload
            elif kind == "error":
                raise payload[0]
        if fit is None:
            results["fitThenCompute"] = "no fit result returned"
        else:
            #Write the fitted values into the entries exactly as _poll does,
            #then recompute and compare.
            for k, v in fit["parameters"].items():
                if k == "phi":
                    tab.phiVar.set(f"{v:.10g}")
                elif k == "srel":
                    tab.srelVar.set(f"{v:.10g}")
            tab.scaleVar.set(f"{fit['scale']:.10g}")
            tab.backgroundVar.set(f"{fit['background']:.10g}")
            res, err = runWorker(root, tab)
            c2fit = float(fit["chi2_reduced"])
            c2com = None if res is None else getattr(res, "chi2", None)
            results["fitThenCompute"] = dict(
                chi2Fit=c2fit, chi2Compute=c2com,
                error=None if err is None
                else f"{type(err[0]).__name__}: {err[0]}",
                #They are computed with the same degrees of freedom, so they
                #should agree closely. A large discrepancy means the two
                #paths are evaluating DIFFERENT MODELS, which is the defect
                #this whole test exists to catch.
                agree=(c2com is not None
                       and abs(c2fit - c2com) <= 0.05*max(abs(c2fit), 1e-30)))
    except Exception as exc:
        results["fitThenCompute"] = f"RAISED {type(exc).__name__}: {exc}"
    _save(results)

    # ---- verdict ------------------------------------------------------
    bad = []
    for k, v in results.items():
        if isinstance(v, str) and ("RAISED" in v or "Error" in v):
            bad.append(k)
        elif isinstance(v, dict):
            if v.get("error"):
                bad.append(k)
            if k == "fitThenCompute" and v.get("agree") is False:
                bad.append("fitThenCompute (chi2 mismatch)")
            for sub, sv in v.items():
                if isinstance(sv, str) and ("RAISED" in sv or "Error" in sv
                                            or "LOST" in sv
                                            or sv.startswith("NONE")):
                    bad.append(f"{k}.{sub}")
    #The fit flags: if the inline checkboxes are missing from the dictionary,
    #a fit silently varies only what remains. Ten are expected for a sphere
    #with the power law off; fewer means the panel and the fit disagree.
    flags = results.get("fitFlagsPresent") or []
    for expected in ("meanRadius", "srel", "phi", "scale", "background"):
        if expected not in flags:
            bad.append(f"fitFlags missing {expected!r}")
    results["FAILURES"] = bad
    _save(results)
    _say(f"done -- {OUT}; {len(bad)} failure(s): {bad}" if bad
         else f"done -- {OUT}; all paths ran")


if __name__ == "__main__":
    main()
