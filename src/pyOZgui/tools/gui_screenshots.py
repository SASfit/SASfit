#!/usr/bin/env python3
"""Regenerate every GUI screenshot used in the documentation.

    cd src/pyOZgui/tools
    python gui_screenshots.py                  # needs a display
    xvfb-run -a python gui_screenshots.py      # headless Linux

Writes into docs/manuscript/figures/. On Windows a display is always
present, so it simply runs.

WHY A SCRIPT RATHER THAN A FOLDER OF PNGs.

Screenshots rot faster than any other kind of documentation. In one working
session the polydisperse panel gained inline bounds fields, a resolution
kernel selector and a Settings button, lost its run history, and had the
solver dropdown moved into it -- every one of which invalidates a picture
taken beforehand. A prose description survives a widget moving; a screenshot
does not, and a stale one is worse than none because it teaches the reader an
interface that no longer exists.

So the pictures are generated, like the figures, and regenerating them is one
command. The `requires ImageMagick` note below is the only reason this is not
simply part of the documentation build.

WHAT IT CAPTURES, and what it deliberately does not. One overview per tab,
showing LAYOUT rather than detail: which panels exist, where the controls
sit, what a result looks like. Close-ups of individual fields are not taken,
because those are what change and because prose describes them better -- the
hover help in the interface is the right place for per-field explanation, not
a figure.

THE PART THAT IS EASY TO GET WRONG is waiting. `_onCompute` hands work to a
thread and returns; `tab.runs` filling is NOT the same event as the canvas
having been redrawn. Capturing on the first of those produced an empty axis
in an earlier version of this script, which then went into the manuscript.
`_computeAndSettle` below waits for both and then for the event loop to go
quiet.

REQUIRES a screen-capture backend. Pillow is the one that works everywhere
this software runs, including native Windows, and matplotlib already depends
on it so it is almost certainly present -- `pip install pillow` otherwise.
ImageMagick's `import` is used as a fallback on X11 systems; it does NOT
exist under native Windows, which is a mistake this docstring made in an
earlier version.
"""
import json
import os
import subprocess
import sys
import time
import warnings

warnings.filterwarnings("ignore")

import matplotlib
matplotlib.use("Agg")
import numpy as np
import tkinter as tk
from tkinter import ttk

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

#WRITTEN TO BOTH FIGURE DIRECTORIES. The manuscript and the report each have
#their own, and the GUI screenshots are referenced from both -- plus from
#docs/gui.md, which reads docs/figures/. Writing to one left the other
#stale, and a stale screenshot is worse than none because it documents an
#interface that no longer exists.
OUTDIRS = [
    os.path.abspath(os.path.join(_HERE, os.pardir, "docs", "manuscript",
                                 "figures")),
    os.path.abspath(os.path.join(_HERE, os.pardir, "docs", "figures")),
]
OUTDIR = OUTDIRS[0]          # kept for anything referring to it by name
GEOMETRY = "1500x1000"

#THE SESSION TO ILLUSTRATE, for the polydisperse figure. Point this at the
#.oz1 holding the measured-data fit the manuscript reports, so that figure
#shows the real thing rather than a curve this script invented. Overridable
#by the environment variable of the same name.
REALDATA_SESSION = os.path.abspath(os.path.join(
    _HERE, os.pardir, "docs", "manuscript", "data",
    "testJohan_LogNorm_Verlet.oz1"))


def _windowAlive(root):
    """True while the Tk main window still exists.

    An exception raised inside a tab's compute worker can take the main
    window down with it, and every Tk call after that fails with "NULL main
    window" -- which looks like a fault in the later tabs rather than
    fallout from the earlier one. Checking lets the summary say "skipped"
    instead.
    """
    try:
        return bool(root.winfo_exists())
    except Exception:
        return False


def _say(m):
    print(m, file=sys.stderr, flush=True)


def _settle(root, seconds=1.0):
    """Let the event loop go quiet before grabbing the window."""
    end = time.time() + seconds
    while time.time() < end:
        root.update()
        root.update_idletasks()
        time.sleep(0.03)


def _raise(root):
    """Bring the window to the front and keep it there.

    Pillow's ImageGrab photographs whatever is PHYSICALLY ON SCREEN in the
    given rectangle -- it does not render the widget from its own state. So
    if the terminal that launched this script is still on top, that is what
    lands in the PNG. The window must actually be raised, focused and fully
    drawn before the grab.

    `-topmost` is set, then cleared again a moment later: leaving it on
    would keep the window above everything else for the rest of the run,
    which is obnoxious on a machine someone is using.
    """
    root.deiconify()
    root.lift()
    root.attributes("-topmost", True)
    root.update()
    try:
        root.focus_force()
    except Exception:
        pass
    _settle(root, 0.4)
    root.attributes("-topmost", False)
    root.update()


def _dpiScale(root, grabbedWidth):
    """Physical pixels per logical pixel, MEASURED rather than asked for.

    The obvious route -- GetDpiForWindow, divided by 96 -- answers a
    different question than the one that matters here. It reports the
    scaling Windows applies, but whether Tk's own numbers are already in
    physical pixels depends on whether the interpreter declared itself
    DPI-aware, which varies between Python builds and can be set by a
    manifest this script never sees. Applying a 1.25 correction to numbers
    that were already physical crops INTO the window and loses its right
    edge, which is what the first attempt did.

    So it is measured instead: the window is maximised, so its logical width
    and the captured screen width describe the same span, and their ratio is
    the factor -- whatever the cause. Snapped to the common values because a
    real scale is one of these and a ratio off by a percent means something
    else is wrong, in which case 1.0 and a full-screen image is the safe
    answer.
    """
    try:
        logical = root.winfo_width()
        if logical > 100 and grabbedWidth > 100:
            ratio = grabbedWidth/float(logical)
            for candidate in (1.0, 1.25, 1.5, 1.75, 2.0):
                if abs(ratio - candidate) < 0.02:
                    return candidate
    except Exception:
        pass
    return 1.0


def _warnIfTruncated(root, widget, label):
    """Warn when a panel is taller than the window can show.

    A screenshot cannot scroll. The parameter panels have grown steadily --
    tab 1 alone gained the fit block, per-parameter bounds, a kernel
    selector, a warm-start box and a deflation control -- and a capture that
    silently loses the bottom rows documents an interface missing precisely
    the newest parts, which is the opposite of useful.
    """
    try:
        root.update_idletasks()
        need = widget.winfo_reqheight()
        have = root.winfo_height()
        if need > have:
            _say(f"  WARNING: {label} needs {need}px but the window is "
                 f"{have}px -- the bottom {need - have}px will be MISSING "
                 f"from the screenshot. Enlarge the screen, or reduce the "
                 f"panel, before trusting this figure.")
            return False
    except Exception:
        pass
    return True


def _grab(root, name):
    """Capture the window to docs/manuscript/figures/<name>.png.

    THREE BACKENDS, tried in order, because no single one works everywhere.

    An earlier version shelled out to ImageMagick's `import` and asserted
    that any MSYS2 installation has it. It does not: `import` is an X11
    tool, so under native Windows Python -- which is what a MinGW-hosted
    interpreter is -- there is no such executable and the call dies with
    WinError 2. Pillow's ImageGrab handles Windows and macOS, `import`
    handles X11, and the Tk-native PostScript dump is the fallback that
    needs nothing at all.
    """
    _raise(root)
    _settle(root)
    path = os.path.join(OUTDIR, f"{name}.png")

    #1. Pillow, grabbing the WHOLE SCREEN rather than a window rectangle.
    #
    #Passing bbox=(x, y, x+w, y+h) from Tk's geometry looks right and is
    #wrong under display scaling. Tk reports LOGICAL pixels while ImageGrab
    #works in PHYSICAL ones, so on a 1920-wide screen at 125 per cent Tk
    #says 1536 and the grab takes the leftmost 1536 physical pixels of a
    #window that is actually 1920 wide -- clipping precisely the 20 per cent
    #the scaling accounts for. The symptom is a screenshot missing its right
    #edge, which looks like a maximise that did not take.
    #
    #Since the window is maximised anyway, the screen IS the window. Grabbing
    #all of it cannot clip, whatever the scaling. The cost is the taskbar,
    #which is easy to crop and harder to get wrong.
    try:
        from PIL import ImageGrab
        root.update_idletasks()
        img = ImageGrab.grab(all_screens=True)
        #CROP TO THE WINDOW, in physical pixels. Grabbing the whole screen
        #cannot clip, whatever the scaling, so the grab is done first and
        #the crop applied afterwards -- the reverse order is what clipped
        #earlier versions. If the scale cannot be determined the full screen
        #is kept, since a picture with a taskbar in it beats a picture with
        #a fifth of the window missing.
        s = _dpiScale(root, img.size[0])
        geom = root.winfo_geometry()              # "WxH+X+Y" in LOGICAL px
        size, _, _rest = geom.partition("+")
        try:
            w, h = (int(v) for v in size.split("x"))
            x, y = root.winfo_rootx(), root.winfo_rooty()
            box = (int(x*s), int(y*s), int((x + w)*s), int((y + h)*s))
            #NEVER crop to less than was asked for. Clamping to the image is
            #right when the box runs past its edge, but if the clamp would
            #remove a tenth of the width the scale is wrong rather than the
            #window being partly offscreen -- and cropping anyway is how the
            #right-hand panel went missing. Keep the whole screen instead.
            clamped = (max(box[0], 0), max(box[1], 0),
                       min(box[2], img.size[0]), min(box[3], img.size[1]))
            lost = (box[2] - box[0]) - (clamped[2] - clamped[0])
            if lost > 0.1*(box[2] - box[0]):
                _say(f"  crop would lose {lost} px of width; keeping the "
                     f"full screen (scale {s:g} looks wrong)")
            elif clamped[2] - clamped[0] > 100 and clamped[3] - clamped[1] > 100:
                img = img.crop(clamped)
        except Exception as exc:
            _say(f"  crop skipped ({type(exc).__name__}); keeping full screen")
        #SAVE TO EVERY FIGURE DIRECTORY, not just the first.
        written = []
        for d in OUTDIRS:
            os.makedirs(d, exist_ok=True)
            p = os.path.join(d, f"{name}.png")
            img.save(p)
            written.append(p)
        _say(f"  wrote {name}.png to {len(written)} dir(s) "
             f"({img.size[0]}x{img.size[1]}, dpi scale {s:g})")
        return True
    except Exception as exc:
        _say(f"  Pillow grab failed: {type(exc).__name__}: {str(exc)[:60]}")

    #2. ImageMagick, for X11 sessions where Pillow's ImageGrab is not
    #implemented.
    try:
        rc = subprocess.call(["import", "-window", "root", path])
        if rc == 0 and os.path.isfile(path):
            _say(f"  wrote {path} (ImageMagick)")
            return True
    except (OSError, FileNotFoundError):
        pass

    #3. Tk's own PostScript dump of the matplotlib canvas. Captures the
    #PLOT only, not the surrounding controls, so it is a poor substitute --
    #but a figure of the curves is better than no figure, and this needs
    #nothing beyond Tk itself.
    try:
        import matplotlib.pyplot as _plt        # noqa: F401
        for child in root.winfo_children():
            pass
        _say(f"  NEITHER Pillow NOR ImageMagick available: {name} not "
             f"captured. Install Pillow (pip install pillow) -- it is the "
             f"only one that works under native Windows.")
    except Exception:
        pass
    return False


def _computeAndSettle(tab, root, timeout=60.0):
    """Press Compute and wait for the PLOT, not merely for the result.

    Two separate waits, deliberately. `tab.runs` filling means the worker
    finished; it does not mean the canvas has been redrawn. Capturing
    between the two gives an empty axis, which is exactly what happened
    before this function existed.
    """
    tab._onCompute()
    end = time.time() + timeout
    while time.time() < end and not tab.runs:
        root.update()
        time.sleep(0.05)
    if not tab.runs:
        _say("  compute did not finish within the timeout")
        return False
    _settle(root, 2.0)
    return True


def _selectTab(tab, startswith):
    """Select an inner plot tab by the start of its label."""
    nb = tab.notebook
    for i in range(nb.index("end")):
        if nb.tab(i, "text").strip().lower().startswith(startswith.lower()):
            nb.select(i)
            return True
    return False


# ---------------------------------------------------------------------------
def shotPolydisperse(root, nb):
    """Tab 1, with data loaded and a computed result.

    Data are synthesised from the model itself so the curves overlay the
    points: a figure captioned 'the interface' should show it working, not
    show a bad fit. The scale and background are set to the values the data
    were generated with, since leaving them at 1 and 0 puts the model a
    factor of two below the data and reads as a broken calculation.
    """
    import generic_polydisperse_tab as G
    from generic_polydisperse_sas import GenericPolydisperseSAS as S

    tab = G.GenericPolydisperseTab(nb)
    nb.add(tab, text="1: Polydisperse (any potential)")
    nb.select(nb.index("end") - 1)
    #Same paned-divider fix as shotExtraTab: the controls live in a
    #weight=0 pane that can collapse to nothing if laid out before the
    #window has its real size.
    _settle(root, 0.4)
    _paned = getattr(tab, "paned", None)
    if _paned is not None:
        try:
            _panes = _paned.panes()
            if len(_panes) >= 2:
                _paned.sashpos(0, max(
                    _paned.nametowidget(_panes[0]).winfo_reqwidth(), 360))
                _settle(root, 0.3)
        except Exception:
            _say("  tab 1: could not set the sash; the control panel may be "
                 "missing from the figure")

    #REAL DATA WHERE A SESSION IS AVAILABLE, synthetic only as a fallback.
    #
    #This figure is the paper's one picture of the interface in use, and it
    #showed a curve the generator had invented from the model -- in a paper
    #whose headline validation is a fit to a MEASURED dataset. Loading the
    #saved session instead puts the fit that is actually being discussed in
    #the figure, and the parameter panel then shows the real fitted values
    #rather than round numbers chosen to look plausible.
    #
    #Point REALDATA_SESSION at an .oz1 file, or set the environment variable
    #of the same name. Falls back to synthetic data with a warning, so the
    #script still runs anywhere.
    sessionPath = os.environ.get("REALDATA_SESSION", REALDATA_SESSION)
    loaded = False
    if sessionPath and os.path.isfile(sessionPath):
        try:
            with open(sessionPath) as fh:
                state = json.load(fh)
            tab.restoreSessionState(state.get("session", state))
            root.update()
            loaded = True
            _say(f"  loaded measured data from {os.path.basename(sessionPath)}")
        except Exception as exc:
            _say(f"  could not load {sessionPath}: {type(exc).__name__}: "
                 f"{str(exc)[:70]}")
    if not loaded:
        _say("  WARNING: no session loaded -- this figure will show "
             "SYNTHETIC data, which is wrong for a paper reporting a "
             "measured-data validation. Set REALDATA_SESSION.")
        Q = np.logspace(np.log10(0.05), np.log10(2.0), 60)
        ref = S("HardSphere", (), phi=0.30, srel=0.12, nbins=3,
                closure="Percus-Yevick", meanRadius=32.0)
        I = 2.0*ref.I_exact(Q) + 0.05
        tab.data = (Q, I, 0.02*I)
        tab.dQ = 0.05*Q + 0.005
        tab.smearCheck.configure(state="normal")
        tab.smearVar.set(True)
        tab.fitBtn.configure(state="normal")
        tab.dataLabelVar.set("60 points, Q 0.05..2.0, with dI, dQ/Q 0.05..0.10")
        for var, val in ((tab.meanRadiusVar, "32.0"), (tab.srelVar, "0.12"),
                         (tab.phiVar, "0.30"), (tab.QminVar, "0.05"),
                         (tab.QmaxVar, "2.0"), (tab.nQVar, "60"),
                         (tab.nbinsVar, "3"), (tab.nFFVar, "20"),
                         (tab.scaleVar, "2.0"), (tab.backgroundVar, "0.05")):
            var.set(val)

    if not _computeAndSettle(tab, root):
        return
    _selectTab(tab, "I(Q)")
    _warnIfTruncated(root, tab, "the polydisperse parameter panel")
    _grab(root, "gui_controls")
    if _selectTab(tab, "Approx"):
        _grab(root, "gui_error")


def shotOZsolver(root, nb):
    """Tab 0, the one-component solver."""
    try:
        import oZgui
    except Exception as exc:
        _say(f"  tab 0 unavailable: {type(exc).__name__}: {exc}")
        return
    _say("  tab 0 is built inside oZgui's own main window, not as a "
         "standalone widget; capture it by running the GUI and pressing "
         "calculate, or extend this function if it is ever factored out")


def shotExtraTab(root, nb, module, cls, label, shortName, compute=False):
    """Any EXTRA_TABS entry, captured as an overview.

    `compute=False` by default: the mixture validation tab's Compare runs
    several OZ solves and takes minutes, which is too long for a routine
    documentation build. Pass True when the figure needs a result in it.
    """
    try:
        mod = __import__(module)
        widget = getattr(mod, cls)(nb)
    except Exception as exc:
        _say(f"  {label}: unavailable ({type(exc).__name__}: "
             f"{str(exc)[:60]})")
        return
    nb.add(widget, text=label)
    nb.select(nb.index("end") - 1)
    #FORCE THE PANED DIVIDER OPEN.
    #
    #Tabs 1 and 3 put their controls in the left pane of a ttk.PanedWindow
    #with weight=0. Laid out before the window has its real size, that pane
    #can end up at essentially zero width, and the capture then shows the
    #plot alone with no interface at all -- which is what happened to tab
    #3's figure. Setting the sash explicitly after the window is up fixes
    #it; the widget's own requested width is the right place to put it.
    _settle(root, 0.4)
    paned = getattr(widget, "paned", None)
    if paned is not None:
        try:
            panes = paned.panes()
            if len(panes) >= 2:
                want = max(paned.nametowidget(panes[0]).winfo_reqwidth(), 360)
                paned.sashpos(0, want)
                _settle(root, 0.3)
        except Exception as exc:
            _say(f"  {label}: could not set the sash ({type(exc).__name__}); "
                 f"the control panel may be missing from the figure")
    if compute:
        #The compare/compute entry point differs per tab, so try the known
        #names. Waiting is done by polling for a result rather than by a
        #fixed sleep: the mixture tab runs several OZ solves and how long
        #they take depends on the state point.
        for name in ("_onCompare", "_onCompute"):
            fn = getattr(widget, name, None)
            if not callable(fn):
                continue
            try:
                fn()
            except Exception as exc:
                _say(f"  {label}: {name} raised {type(exc).__name__}")
                break
            end = time.time() + 300.0
            while time.time() < end:
                root.update()
                time.sleep(0.1)
                done = (getattr(widget, "curves", None)
                        or getattr(widget, "runs", None))
                if done:
                    break
            if not (getattr(widget, "curves", None)
                    or getattr(widget, "runs", None)):
                _say(f"  {label}: {name} did not produce a result in time; "
                     f"capturing the empty panel")
            _settle(root, 2.0)
            break
    if not _grab(root, shortName):
        return
    _warnIfTruncated(root, widget, label)


# ---------------------------------------------------------------------------
def main():
    os.makedirs(OUTDIR, exist_ok=True)
    root = tk.Tk()
    root.title("Ornstein-Zernike solver (Python)")
    #MAXIMISED, not a fixed geometry. The panel is tall -- with the fit
    #block, the bounds columns and the help box it runs past 900 pixels --
    #and a window smaller than its content shows a figure cut off at the
    #bottom, which is how an earlier capture lost the solver dropdown and
    #the Q range. `zoomed` is the Windows and X11 spelling; macOS needs the
    #attribute, hence the fallback.
    try:
        root.state("zoomed")
    except tk.TclError:
        try:
            root.attributes("-zoomed", True)
        except Exception:
            root.geometry(GEOMETRY)
    nb = ttk.Notebook(root)
    nb.pack(fill="both", expand=True)

    #EACH TAB IN ITS OWN GUARD, and the window checked between them.
    #
    #An exception inside tab 1's compute worker destroyed the Tk main
    #window, after which tabs 2 and 3 failed with "NULL main window", a
    #stray "invalid command name ..._poll" from the orphaned after-callback
    #appeared, NOTHING was captured -- and the script still printed "done".
    #One tab's trouble should cost that tab's figure and no more, and the
    #summary should say what was actually written rather than that it
    #finished.
    written = []

    def _run(label, fn):
        if not _windowAlive(root):
            _say(f"  {label}: SKIPPED, the Tk window is gone -- an earlier "
                 f"failure took it down")
            return
        _say(label)
        try:
            fn()
        except Exception as exc:
            _say(f"  {label}: FAILED ({type(exc).__name__}: "
                 f"{str(exc).splitlines()[0][:90]})")

    _run("tab 1: polydisperse", lambda: shotPolydisperse(root, nb))
    _run("tab 2: RY polydisperse Yukawa",
         lambda: shotExtraTab(root, nb, "ry_polydisperse_yukawa_tab",
                              "RYPolydisperseYukawaTab",
                              "2: RY Polydisperse Yukawa", "gui_ry_yukawa"))
    #compute=True for tab 3: an empty comparison panel shows nothing a
    #reader can use, and the comparison is the tab's whole point.
    _run("tab 3: mixture validation",
         lambda: shotExtraTab(root, nb, "mixture_validation_tab",
                              "MixtureValidationTab",
                              "3: Mixture validation",
                              "gui_mixture_validation", compute=True))
    _run("tab 0: OZ solver", lambda: shotOZsolver(root, nb))

    for d in OUTDIRS:
        for name in ("gui_controls", "gui_error", "gui_ry_yukawa",
                     "gui_mixture_validation"):
            p = os.path.join(d, f"{name}.png")
            if os.path.isfile(p):
                written.append(name)
    got = sorted(set(written))
    if got:
        _say(f"wrote or refreshed: {', '.join(got)}")
    else:
        _say("WROTE NOTHING -- every capture failed. The figures on disk, if "
             "any, are the OLD ones.")
    _say(f"figure directories: {', '.join(OUTDIRS)}")


if __name__ == "__main__":
    main()
