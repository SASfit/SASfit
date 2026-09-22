# -*- coding: utf-8 -*-
"""
Mixture validation tab: exact analytic references beside the numerical solver.

This tab exists because of what happened during development. A defect that
gave every pair in a mixture the same hard core survived every internal
consistency test in this package, and was found only when the numerical
result was placed beside an independently written analytic one. The
comparison that found it took a few minutes; the defect had been present for
a long time and had already produced published-looking numbers.

So the comparison is worth having permanently, one click away, rather than as
a script somebody has to remember to run.

WHAT IT SHOWS
-------------
For a mixture of hard spheres or charged hard spheres, up to three curves:

  numerical      this package's Ornstein-Zernike solver, any closure
  mixscatter     the analytic Vrij (1979) mixture Percus-Yevick solution
  Gazzillo MSA   the analytic mean-spherical solution for polydisperse
                 CHARGED hard spheres, Appendix A of arXiv:cond-mat/9909153

The first two are the same model, so they should agree to discretisation
error and the difference should SHRINK as the grid is refined. That last
point matters more than the size of the difference: the identical-cores
defect showed a 5.2 % discrepancy that did not converge, whereas ordinary
grid error falls roughly fourfold for a fourfold refinement. The tab reports
the ratio between two grids for exactly this reason.

The Gazzillo curve is a DIFFERENT MODEL when charges are on: it is the
primitive model with explicit microions, whereas the numerical route follows
D'Aguanno and Klein in eliminating the microions into an effective
screened-Coulomb macroion potential. Disagreement there is expected and is
not evidence of a defect. With the charge switched off both reduce to
mixture PY and must agree to machine precision -- which is the useful check.

DEPENDENCIES
------------
mixscatter is optional; without it that curve is simply omitted. The Gazzillo
reference is implemented in this package and always available.
"""
import json
import queue
import threading
import traceback

import tkinter as tk
from tkinter import ttk, messagebox

import numpy as np
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import (FigureCanvasTkAgg,
                                               NavigationToolbar2Tk)

IMPORT_ERROR = None
try:
    import gazzillo_msa as GZ
    from polydisperse_nodes import sizeClasses
    import ozLib
except Exception as _exc:                                  # pragma: no cover
    IMPORT_ERROR = _exc

try:
    import mixscatter as _ms
except Exception:
    _ms = None


def _solverChoices():
    """Available fixed-point solvers, most robust first.

    Each import is guarded because the optional back ends -- SUNDIALS above
    all -- may not be built. Order matters: the first entry becomes the
    default, and this tab wants the most robust one, since the convergence
    test only becomes meaningful above about phi = 0.4 and plain Picard
    diverges near 0.45.
    """
    choices = {}
    try:
        from sundials4pyKinsolFPOZsolver import Sundials4pyKinsolFPOZsolver
        choices["SUNDIALS KIN_FP (Anderson)"] = Sundials4pyKinsolFPOZsolver
    except Exception:
        pass
    try:
        from scipyAndersonOZsolver import ScipyAndersonOZsolver
        choices["scipy Anderson"] = ScipyAndersonOZsolver
    except Exception:
        pass
    try:
        from andersonOZsolver import AndersonOZsolver
        choices["Anderson"] = AndersonOZsolver
    except Exception:
        pass
    try:
        from picardOZsolver import PicardOZsolver
        choices["Picard / Mann"] = PicardOZsolver
    except Exception:
        pass
    return choices


class MixtureValidationTab(ttk.Frame):
    #Session file identity -- see GenericPolydisperseTab. Index 3 matches the
    #position in the notebook (0: OZ solver, 1: Polydisperse, 2: RY Yukawa).
    #This tab has NO Save/Load: it is a validation tool with nothing worth
    #persisting, so the index serves only to number the label.
    SESSION_TAB_INDEX = 3
    SESSION_TAB_NAME = "Mixture validation"
    def __init__(self, master, **kw):
        super().__init__(master, **kw)
        if IMPORT_ERROR is not None:
            ttk.Label(self, justify="left", padding=20, foreground="#a00",
                      text=("The mixture validation modules could not be "
                            "imported, so this tab is inactive.\n"
                            "The other tabs are unaffected.\n\n"
                            f"{type(IMPORT_ERROR).__name__}: {IMPORT_ERROR}")
                      ).pack(anchor="nw")
            return
        self.resultQueue = queue.Queue()
        self.worker = None
        self._polling = False
        self.curves = {}
        #A DRAGGABLE DIVIDER between the parameters and the plots, as in the
        #Polydisperse tab. The panel now carries four columns of controls
        #plus a help box, and a fixed width can only ever be wrong for
        #somebody -- the widest row depends on the solver name and the font.
        self.paned = ttk.PanedWindow(self, orient="horizontal")
        self.paned.grid(row=0, column=0, columnspan=2, sticky="nsew")
        self.columnconfigure(0, weight=1)
        self.columnconfigure(1, weight=0)
        self.rowconfigure(0, weight=1)
        self._buildControls()
        self._buildPlot()

    # ------------------------------------------------------------------
    #HOVER HELP. One box at the bottom instead of three permanent grey
    #paragraphs, which between them took a third of the panel for text read
    #once and then scrolled past for the rest of the session.
    HELP = {
        "charge": ("Charged systems are DISABLED here. The Gazzillo MSA is "
                   "available analytically, but the polydisperse builder "
                   "cannot make a charge-coupled potential, so there is no "
                   "numerical curve to compare it with -- and a comparison "
                   "of one curve is not a validation. For a charged "
                   "numerical solve see the RY Polydisperse Yukawa tab, "
                   "which is a DIFFERENT model (effective screened Coulomb, "
                   "not explicit microions)."),
        "closure": ("Fixed at Percus-Yevick: both analytic references ARE "
                    "PY, so the numerical solve must use it too or the "
                    "difference would be physics rather than numerics. The "
                    "Polydisperse tab offers the other eighteen closures."),
        "refine": ("The coarse/fine ratio is the diagnostic that matters: "
                   "with factor f and convergence order p it should be f^p, "
                   "so at 4x expect 4 if first order and 16 if second. An "
                   "error that does NOT shrink indicates a defect, not "
                   "discretisation. Both the point count and the grid size "
                   "are scaled, so the real-space range is identical and "
                   "the comparison is a genuine refinement."),
        "pps": ("Radial resolution of the coarse solve. The fine solve uses "
                "this times the refinement factor."),
        "solver": ("Which fixed-point solver runs. Picard is the plainest "
                   "and diverges above about phi = 0.45; the Anderson "
                   "family converges there and is faster. This tab is about "
                   "ACCURACY rather than solver behaviour, so the most "
                   "robust available solver is usually the right choice -- "
                   "all of them converge to the same fixed point, and the "
                   "residual is reported so you can check."),
        "mannAlpha": ("Damping for the Picard/Mann iteration: "
                      "x_(n+1) = (1-a) x_n + a T(x_n). a = 1 is plain "
                      "Picard; below 1 it is Mann's iteration, slower but "
                      "convergent where undamped Picard diverges. Applies "
                      "only to the Picard/Mann solver -- the accelerated "
                      "ones choose their own step."),
        "gridK": ("The coarse grid is N = 2^k - 1 radial points, which is "
                 "what makes the DST-I's underlying FFT a power of two -- "
                 "9.5x faster than an awkward length. The FINE grid is "
                 "scaled by the refinement factor, so the real-space range "
                 "r_max = N*sigma/pps is the same for both and the "
                 "comparison is a genuine refinement. k = 12 gives 4095, "
                 "and with 100 points per diameter that is r_max = 41 "
                 "sigma; raise it if the correlations have not decayed by "
                 "there."),
        "maxIter": ("Iterations before the solve gives up. Generous by "
                    "default: the residual is reported, and a solve that "
                    "stops early would show as an error floor that does not "
                    "shrink under refinement -- indistinguishable from the "
                    "defect this tab looks for."),
        "phi": ("Total volume fraction. Raise it to make the convergence "
                "test meaningful: below about 0.3 the errors sit at the "
                "q-interpolation floor and the ratio is noise."),
    }

    def _bindHelp(self, widget, key):
        text = self.HELP.get(key)
        if not text:
            return
        #No <Leave> binding: the help stays on the last thing hovered, so it
        #does not blank as the cursor travels towards the entry it describes.
        widget.bind("<Enter>", lambda e, t=text: self.helpVar.set(t), add="+")

    def _buildControls(self):
        outer = ttk.Frame(self.paned, padding=6)
        #weight=0: extra width goes to the plots when the window grows.
        self.paned.add(outer, weight=0)
        r = 0
        self._entryWidgets = {}

        def entry(label, default, width=12, help=None):
            nonlocal r
            lab = ttk.Label(outer, text=label)
            lab.grid(row=r, column=0, sticky="e")
            v = tk.StringVar(value=default)
            e = ttk.Entry(outer, textvariable=v, width=width)
            e.grid(row=r, column=1, sticky="w")
            self._entryWidgets[label] = e
            if help:
                self._bindHelp(lab, help)
                self._bindHelp(e, help)
            r += 1
            return v

        ttk.Label(outer, text="Mixture", font=("TkDefaultFont", 9, "bold")
                  ).grid(row=r, column=0, columnspan=2, sticky="w"); r += 1
        self.meanRadiusVar = entry("Mean radius:", "50.0")
        self.srelVar = entry("Rel. s.d.:", "0.15")
        self.nbinsVar = entry("Classes:", "5")
        self.phiVar = entry("Volume fraction:", "0.20")

        ttk.Separator(outer, orient="horizontal").grid(
            row=r, column=0, columnspan=2, sticky="ew", pady=6); r += 1
        ttk.Label(outer, text="Charge (0 = neutral)",
                  font=("TkDefaultFont", 9, "bold")).grid(
            row=r, column=0, columnspan=2, sticky="w"); r += 1
        #CHARGE IS DISABLED, not removed.
        #
        #This tab is for COMPARISON, and the charged mode compares nothing:
        #the polydisperse builder cannot construct a charge-coupled
        #potential, so no numerical curve can be produced, and the Gazzillo
        #MSA is left plotted on its own. A validation tab that sometimes
        #validates nothing invites precisely the wrong conclusion -- the
        #reader sees a curve in a window headed "validation" and takes it as
        #checked.
        #
        #The entries stay, greyed, rather than being deleted: the charged
        #Gazzillo code is correct and valuable, and the moment a
        #charge-coupled builder exists this becomes a real comparison. The
        #state="disabled" lines below are then the only thing to remove.
        self.zVar = entry("Macroion z:", "0.0")
        self.lbVar = entry("Bjerrum length:", "7.189")
        self.sigma1Var = entry("Microion diam.:", "5.0")
        for _label in ("Macroion z:", "Bjerrum length:", "Microion diam.:"):
            self._entryWidgets[_label].configure(state="disabled")
            self._bindHelp(self._entryWidgets[_label], "charge")

        ttk.Separator(outer, orient="horizontal").grid(
            row=r, column=0, columnspan=2, sticky="ew", pady=6); r += 1
        ttk.Label(outer, text="Closure:").grid(row=r, column=0, sticky="e")
        #PERCUS-YEVICK, stated rather than selected.
        #
        #Both references are PY: mixscatter's Vrij mixture solution, and
        #Gazzillo's MSA in its neutral limit. This tab validates the
        #NUMERICAL MACHINERY -- grid, transform, matrix solve, size-class
        #construction -- so the closure must be identical on both sides, and
        #a dropdown with one entry is a control that cannot be used.
        #
        #Offering the others was a trap. Choosing HNC or Martynov-Sarkisov
        #gives a large "relative difference" against a PY reference that is
        #a genuine difference between CLOSURES, shown in a panel that means
        #numerical error everywhere else, beside a convergence ratio that
        #then measures nothing. "Closures needing no consistency parameter"
        #is the wrong criterion for the same reason: HNC needs none and is
        #still not PY.
        #
        #A DIFFERENT feature would be worth having: analytic PY beside a
        #numerical HNC, to show how much the closure itself matters. That is
        #a closure comparison, not a validation, and it would need the error
        #panel relabelled -- "relative difference" there currently means
        #numerical error. Kept as a StringVar so that feature can restore a
        #selector without touching anything downstream.
        self.closureVar = tk.StringVar(value="Percus-Yevick")
        ttk.Label(outer, text="Percus-Yevick").grid(
            row=r, column=1, sticky="w"); r += 1
        self.ppsVar = entry("Points per sigma:", "100", help="pps")
        #REFINEMENT FACTOR between the coarse and fine solves, as a fixed
        #choice rather than a free entry.
        #
        #The convergence test compares the two errors: with factor f and
        #convergence order p the ratio should be f^p, so at f = 4 a
        #first-order scheme gives 4 and a second-order one 16 -- while a
        #defect that is not a discretisation error at all gives about 1,
        #which is how the identical-cores bug announced itself.
        #
        #Powers of two only. An arbitrary factor is fine arithmetically but
        #muddies the comparison: the grid should divide cleanly, and f = 3
        #or 7 gives expected ratios (9, 49) that are harder to hold in the
        #head than 4 and 16. Offering four values also makes the trade
        #explicit -- a larger f separates first from second order more
        #sharply and raises the coarse error, which this test NEEDS, since
        #below about 0.5 % the ratio is interpolation noise and the tab says
        #so. The fine solve costs accordingly: f = 8 is sixty-four times the
        #work of the coarse one.
        ttk.Label(outer, text="Refinement factor:").grid(
            row=r, column=0, sticky="e")
        self.refineVar = tk.StringVar(value="4")
        ttk.Combobox(outer, textvariable=self.refineVar, width=10,
                     state="readonly",
                     values=["2", "4", "8", "16"]).grid(
            row=r, column=1, sticky="w")
        r += 1
        #SOLVER AND DAMPING. Picard diverges above about phi = 0.45, which
        #is precisely where the convergence test becomes meaningful -- below
        #0.3 the errors sit at the q-interpolation floor. So the tab needs a
        #solver that reaches the useful regime.
        #
        #All of them converge to the same fixed point and the residual is
        #reported, so the choice is about robustness rather than accuracy.
        #The default is the most robust available.
        self.solverChoices = _solverChoices()
        _solverLab = ttk.Label(outer, text="Solver:")
        _solverLab.grid(row=r, column=0, sticky="e")
        self.solverVar = tk.StringVar(
            value=next(iter(self.solverChoices)) if self.solverChoices
            else "")
        _solverBox = ttk.Combobox(outer, textvariable=self.solverVar,
                                  state="readonly", width=26,
                                  values=list(self.solverChoices))
        _solverBox.grid(row=r, column=1, sticky="w")
        self._bindHelp(_solverLab, "solver")
        self._bindHelp(_solverBox, "solver")
        r += 1
        self.mannAlphaVar = entry("   damping a:", "1.0", help="mannAlpha")
        #GRID EXPONENT and iteration budget, both hardcoded until now (4095
        #and 60000). The grid matters because it sets the real-space range
        #of the COARSE solve, and whether 41 sigma is enough depends on phi;
        #the budget matters because a solve that stops early looks exactly
        #like the defect this tab hunts for.
        #
        #Inline rather than behind a Settings button: two fields do not earn
        #a dialog, and the panel has room now the grey paragraphs are gone.
        #TRANSFORM TYPE is deliberately absent -- type 4 needs every pair
        #core between grid points, which cannot be arranged for a mixture
        #since the sigma_i come from Gaussian quadrature and are irrational.
        _kLab = ttk.Label(outer, text="Grid exponent k:")
        _kLab.grid(row=r, column=0, sticky="e")
        #The entry and the derived-N label share ONE frame in column 1.
        #Putting the label in column 2 let the grid stretch column 1 to the
        #width of the help box below it, which pushed the N text far to the
        #right and left a large gap.
        _kFrame = ttk.Frame(outer)
        _kFrame.grid(row=r, column=1, sticky="w")
        self.gridKVar = tk.StringVar(value="12")
        _kEntry = ttk.Entry(_kFrame, textvariable=self.gridKVar, width=12)
        _kEntry.pack(side="left")
        self._nLabelVar = tk.StringVar(value="")
        ttk.Label(_kFrame, textvariable=self._nLabelVar,
                  foreground="grey").pack(side="left", padx=(6, 0))
        self._bindHelp(_kLab, "gridK")
        self._bindHelp(_kEntry, "gridK")
        r += 1

        def _showN(*_):
            try:
                k = int(float(self.gridKVar.get()))
                f = int(float(self.refineVar.get()))
            except ValueError:
                self._nLabelVar.set("?")
                return
            self._nLabelVar.set(
                f"N = {(1 << k) - 1} / {(1 << k)*f - 1}")
        self.gridKVar.trace_add("write", _showN)
        self.refineVar.trace_add("write", _showN)
        _showN()
        self.maxIterVar = entry("Max iterations:", "60000", help="maxIter")

        def _syncMann(*_):
            e = self._entryWidgets.get("   damping a:")
            if e is not None:
                e.configure(state="normal"
                            if "Picard" in self.solverVar.get()
                            else "disabled")
        self.solverVar.trace_add("write", _syncMann)
        _syncMann()

        #THE HELP BOX, sized for the LONGEST entry so the panel does not
        #move as the cursor travels. A Text rather than a Label: its height
        #is in lines and does not follow its content, and a longer entry is
        #reachable by scrolling rather than simply cut.
        self.helpVar = tk.StringVar(value="")
        _wrapPx, _cpl = 250, 40
        _lines = max((len(t) + _cpl - 1)//_cpl for t in self.HELP.values())
        _hf = ttk.Frame(outer, relief="sunken", borderwidth=1)
        _hf.grid(row=r, column=0, columnspan=2, sticky="ew", pady=(6, 0))
        r += 1
        _ht = tk.Text(_hf, height=min(_lines, 9), width=34, wrap="word",
                      relief="flat", background="#f4f4f4",
                      foreground="#204a87", font=("TkDefaultFont", 8))
        _hb = ttk.Scrollbar(_hf, orient="vertical", command=_ht.yview)
        _ht.configure(yscrollcommand=_hb.set, state="disabled")
        _hb.pack(side="right", fill="y")
        _ht.pack(side="left", fill="both", expand=True)

        def _setHelp(*_):
            _ht.configure(state="normal")
            _ht.delete("1.0", "end")
            _ht.insert("1.0", self.helpVar.get())
            _ht.configure(state="disabled")
        self.helpVar.trace_add("write", _setHelp)
        self.helpVar.set(self.HELP["refine"])

        ttk.Separator(outer, orient="horizontal").grid(
            row=r, column=0, columnspan=2, sticky="ew", pady=6); r += 1
        self.QminVar = entry("Q min:", "1e-3")
        self.QmaxVar = entry("Q max:", "0.3")
        self.nQVar = entry("Points:", "200")

        btns = ttk.Frame(outer)
        btns.grid(row=r, column=0, columnspan=2, sticky="w", pady=(8, 0)); r += 1
        self.computeBtn = ttk.Button(btns, text="Compare",
                                     command=self._onCompare)
        self.computeBtn.pack(side="left", padx=2)
        ttk.Button(btns, text="Clear", command=self._onClear).pack(side="left", padx=2)
        #Save/load for this tab is self-contained -- see _onSave. The file
        #keeps the settings and the computed curves, which is the whole
        #state here.
        ttk.Button(btns, text="Save...", command=self._onSave).pack(side="left", padx=2)
        ttk.Button(btns, text="Load...", command=self._onLoad).pack(side="left", padx=2)

        self.statusVar = tk.StringVar(
            value="ready" + ("" if _ms is not None else
                             "  (mixscatter not installed)"))
        ttk.Label(outer, textvariable=self.statusVar, foreground="#00008b",
                  wraplength=250, justify="left").grid(
            row=r, column=0, columnspan=2, sticky="w", pady=(6, 0)); r += 1

        #SCROLLBARS. The summary grew well past its fourteen lines -- size
        #classes, per-curve errors, solver residuals, the convergence
        #verdict and its explanation -- and without a scrollbar everything
        #below the fold was simply unreachable. With wrap="none" the long
        #lines were cut horizontally as well, so a warning could be present
        #and invisible in both directions at once.
        summaryFrame = ttk.Frame(outer)
        summaryFrame.grid(row=r, column=0, columnspan=2, sticky="nsew",
                          pady=(6, 0))
        #MINIMUM HEIGHT, not just a requested one. `height=14` on the Text
        #is what it ASKS for; grid will happily shrink it below that when
        #another row wants the space, which left two visible lines even on a
        #full-screen window -- and the summary is where the residuals and
        #the convergence verdict live, so it was the one thing that had to
        #stay readable.
        outer.rowconfigure(r, weight=1, minsize=210)
        summaryFrame.rowconfigure(0, weight=1)
        summaryFrame.columnconfigure(0, weight=1)
        self.summary = tk.Text(summaryFrame, width=34, height=14,
                               font=("TkFixedFont", 8), wrap="none")
        self.summary.grid(row=0, column=0, sticky="nsew")
        _sv = ttk.Scrollbar(summaryFrame, orient="vertical",
                            command=self.summary.yview)
        _sv.grid(row=0, column=1, sticky="ns")
        _sh = ttk.Scrollbar(summaryFrame, orient="horizontal",
                            command=self.summary.xview)
        _sh.grid(row=1, column=0, sticky="ew")
        self.summary.configure(yscrollcommand=_sv.set,
                               xscrollcommand=_sh.set)

    def _buildPlot(self):
        right = ttk.Frame(self.paned)
        #weight=1: the plots take the extra space.
        self.paned.add(right, weight=1)
        self.fig = Figure(figsize=(6.5, 5.5), dpi=100)
        self.ax = self.fig.add_subplot(211)
        self.axErr = self.fig.add_subplot(212)
        self._decorate()
        self.canvas = FigureCanvasTkAgg(self.fig, master=right)
        self.canvas.get_tk_widget().pack(fill="both", expand=True)
        NavigationToolbar2Tk(self.canvas, right).update()

    def _decorate(self):
        self.ax.set_xlabel(r"$Q$  [1/length]")
        self.ax.set_ylabel(r"$S_M(Q)$")
        self.ax.set_xscale("log")
        self.axErr.set_xlabel(r"$Q$  [1/length]")
        self.axErr.set_ylabel("relative difference")
        self.axErr.set_xscale("log")
        self.axErr.set_yscale("log")

    # ------------------------------------------------------------------
    def setComboValues(self, combo, var, values, default=None,
                       status=True):
        """Repoint a combobox, resetting the variable if its value is gone.

        A ttk.Combobox does NOT clear its variable when the value list
        changes: a readonly box will happily go on displaying a choice that
        is no longer offered, and `var.get()` will keep returning it. The
        selection then reaches the calculation while the list says it is
        unavailable -- a control showing one thing while the code uses
        another, which is the defect this package has met repeatedly and
        never noticed quickly.

        So: keep the current value if it survives, otherwise fall back to
        `default` (or the first entry) and SAY SO in the status line. A
        silent reset is better than a stale one but still a surprise; the
        user chose that closure for a reason and deserves to know it was
        taken away.
        """
        values = list(values)
        combo.configure(values=values)
        current = var.get()
        if current in values:
            return current
        newValue = (default if default in values
                    else (values[0] if values else ""))
        var.set(newValue)
        if status and current and hasattr(self, "statusVar"):
            self.statusVar.set(
                f"'{current}' is not available here; switched to "
                f"'{newValue}'")
        return newValue

    def _readInputs(self):
        def f(var, name, lo=None, hi=None):
            try:
                v = float(var.get())
            except ValueError:
                raise ValueError(f"{name}: {var.get()!r} is not a number")
            if lo is not None and v < lo:
                raise ValueError(f"{name} must be >= {lo}")
            if hi is not None and v > hi:
                raise ValueError(f"{name} must be <= {hi}")
            return v
        p = dict(
            meanRadius=f(self.meanRadiusVar, "Mean radius", lo=1e-12),
            srel=f(self.srelVar, "Rel. s.d.", lo=0.0, hi=0.9),
            nbins=int(f(self.nbinsVar, "Classes", lo=1, hi=40)),
            phi=f(self.phiVar, "Volume fraction", lo=1e-9, hi=0.6),
            #FORCED NEUTRAL. The charge fields are disabled, but a session
            #saved before that could still carry a non-zero z, and
            #restoreSessionState sets the variable regardless of the
            #widget's state. Reading it as zero here is what actually keeps
            #the charged branch unreachable -- greying an entry controls the
            #interface, not the calculation.
            z=0.0,
            lb=f(self.lbVar, "Bjerrum length", lo=0.0),
            sigma1=f(self.sigma1Var, "Microion diameter", lo=1e-12),
            closure=self.closureVar.get(),
            solverClass=(self.solverChoices.get(self.solverVar.get())
                         if getattr(self, "solverChoices", None) else None),
            mannAlpha=f(self.mannAlphaVar, "damping a", lo=1e-6, hi=1.0),
            gridK=int(f(self.gridKVar, "Grid exponent k", lo=8, hi=18)),
            maxIter=int(f(self.maxIterVar, "Max iterations",
                          lo=100, hi=10_000_000)),
            pps=int(f(self.ppsVar, "Points per sigma", lo=20, hi=4000)),
            refine=int(f(self.refineVar, "Refinement factor", lo=2, hi=16)),
            Qmin=f(self.QminVar, "Q min", lo=1e-12),
            Qmax=f(self.QmaxVar, "Q max", lo=1e-12),
            nQ=int(f(self.nQVar, "Points", lo=8, hi=2000)),
        )
        if p["Qmax"] <= p["Qmin"]:
            raise ValueError("Q max must exceed Q min")
        return p

    def _onClear(self):
        self.curves = {}
        self.summary.delete("1.0", "end")
        self._replot()

    # ------------------------------------------------------------------
    #Save / load.
    #
    #This tab does not use PolydisperseTabControls -- it has no run list and
    #its own plotting -- so it carries its own pair rather than inheriting
    #one. The file records the INPUT SETTINGS and the computed CURVES, which
    #together are the whole state: everything else is derived.
    #
    #The extension is .oz3, matching this tab's SESSION_TAB_INDEX, and the
    #tab NAME is written into the file and checked on load. The extension is
    #a convenience; the name is the guard. Loading another tab's file would
    #otherwise set whichever entries happened to share a name and silently
    #ignore the rest -- a state half one model and half another, reported as
    #a successful load.
    _SESSION_VARS = ("meanRadiusVar", "srelVar", "nbinsVar", "phiVar",
                     "zVar", "lbVar", "sigma1Var", "closureVar", "ppsVar",
                     "QminVar", "QmaxVar", "nQVar")

    def _onSave(self):
        from tkinter import filedialog
        path = filedialog.asksaveasfilename(
            defaultextension=".oz3",
            filetypes=[("Mixture validation session", "*.oz3"),
                       ("Ornstein-Zernike GUI data", "*.oz*"),
                       ("All files", "*.*")],
            title="Save mixture validation session")
        if not path:
            return
        payload = {"format": "sasfit_mixture_validation_save_v1",
                   "tab": self.SESSION_TAB_NAME,
                   "tabIndex": self.SESSION_TAB_INDEX,
                   "entries": {}, "curves": {}}
        for name in self._SESSION_VARS:
            var = getattr(self, name, None)
            if var is not None:
                try:
                    payload["entries"][name] = var.get()
                except Exception:
                    pass
        for key, val in (getattr(self, "curves", None) or {}).items():
            if isinstance(val, np.ndarray):
                payload["curves"][key] = {"__ndarray__": val.tolist()}
            elif isinstance(val, (int, float, str, bool, type(None), list)):
                payload["curves"][key] = val
            elif isinstance(val, dict):
                payload["curves"][key] = {
                    k: ({"__ndarray__": v.tolist()}
                        if isinstance(v, np.ndarray) else v)
                    for k, v in val.items()}
        try:
            #allow_nan is relied on, as elsewhere in this package: some
            #curves legitimately contain NaN inside a hard core.
            with open(path, "w") as fh:
                #indent=2 and sorted keys: the settings are what anyone wants
                #to read or hand-edit, and unindented JSON puts them all on
                #one line. Sorting also makes two saves diff cleanly.
                json.dump(payload, fh, indent=2, sort_keys=True)
                fh.write("\n")
            self.statusVar.set(f"saved to {path}")
        except OSError as exc:
            messagebox.showerror("save failed", str(exc))

    def _onLoad(self):
        from tkinter import filedialog
        path = filedialog.askopenfilename(
            filetypes=[("Mixture validation session", "*.oz3"),
                       ("Ornstein-Zernike GUI data", "*.oz*"),
                       ("All files", "*.*")],
            title="Load mixture validation session")
        if not path:
            return
        try:
            with open(path) as fh:
                payload = json.load(fh)
        except (OSError, json.JSONDecodeError) as exc:
            messagebox.showerror("load failed", str(exc))
            return
        if payload.get("format") != "sasfit_mixture_validation_save_v1":
            messagebox.showerror("load failed", "unrecognised file format")
            return
        saved = payload.get("tab")
        if saved is not None and saved != self.SESSION_TAB_NAME:
            messagebox.showerror(
                "wrong tab",
                f"This file was saved from '{saved}' but this is "
                f"'{self.SESSION_TAB_NAME}'. Open it in the tab it came "
                f"from.")
            return

        def decode(v):
            if isinstance(v, dict) and "__ndarray__" in v:
                return np.asarray(v["__ndarray__"], float)
            if isinstance(v, dict):
                return {k: decode(x) for k, x in v.items()}
            return v

        for name, value in (payload.get("entries") or {}).items():
            var = getattr(self, name, None)
            if var is not None:
                try:
                    var.set(value)
                except Exception:
                    pass
        self.curves = {k: decode(v)
                       for k, v in (payload.get("curves") or {}).items()}
        #Redraw separately: a plotting failure must not be reported as a
        #failed load, since the settings and curves are already in place.
        try:
            self._replot()
            self.statusVar.set(f"loaded {path}")
        except Exception as exc:
            self.statusVar.set(
                f"loaded {path}; replot failed "
                f"({type(exc).__name__}: {exc})")

    def _onCompare(self):
        if self.worker is not None and self.worker.is_alive():
            messagebox.showinfo("busy", "a comparison is already running")
            return
        try:
            p = self._readInputs()
        except ValueError as exc:
            messagebox.showerror("input error", str(exc))
            return
        self.computeBtn.configure(state="disabled")
        self.statusVar.set("comparing...")
        self.worker = threading.Thread(target=self._worker, args=(p,), daemon=True)
        self.worker.start()
        if not self._polling:
            self._polling = True
            self.after(150, self._poll)

    # ------------------------------------------------------------------
    def _worker(self, p):
        try:
            Q = np.logspace(np.log10(p["Qmin"]), np.log10(p["Qmax"]), p["nQ"])
            sigma, x = sizeClasses("Schulz", p["srel"], p["nbins"],
                                   2.0*p["meanRadius"])
            out = {"Q": Q, "sigma": sigma, "x": x, "lines": {}, "notes": [],
                   #Carried with the result so _summarise can say what ratio
                   #to expect; it has no access to the input dict.
                   "refine": p["refine"]}

            #--- Gazzillo analytic reference -----------------------------
            if abs(p["z"]) > 0:
                sig, z, rho = GZ.primitiveModelSystem(
                    2.0*p["meanRadius"], p["z"], p["srel"], p["phi"],
                    sigma1=p["sigma1"], bjerrum=p["lb"])
                Scharged = GZ.measurableStructureFactor(
                    Q, sig, z, rho, None, p["lb"])
                gam, _ = GZ.solveGamma(sig, z, rho, p["lb"])
                #SANITY-CHECK THE CHARGED SOLVE BEFORE PLOTTING IT.
                #
                #A structure factor is a variance: positive and of order
                #one. This path has been seen to return -2e21 with
                #2*Gamma*<sigma> = 0.000 -- the MSA screening parameter
                #degenerating to zero rather than converging, so the returned
                #array is not a structure factor at all.
                #
                #Plotting it anyway is worse here than anywhere else in the
                #package: this is the tab whose entire purpose is to say
                #whether a calculation can be trusted, and it was presenting
                #a failed solve as an analytic REFERENCE.
                Sfin = np.asarray(Scharged, float)
                bad = (not np.all(np.isfinite(Sfin))
                       or np.min(Sfin) < -1e-6
                       or np.max(np.abs(Sfin)) > 1e3
                       or abs(gam) < 1e-12)
                if bad:
                    out["notes"].append(
                        f"CHARGED SOLVE FAILED and is NOT plotted: "
                        f"2*Gamma*<sigma> = {2*gam*2*p['meanRadius']:.3g}, "
                        f"S(Q) spans {np.nanmin(Sfin):.3g} to "
                        f"{np.nanmax(Sfin):.3g}. A structure factor must be "
                        f"positive and of order 1; a screening parameter of "
                        f"zero means the MSA condition was not solved. Try a "
                        f"smaller charge, a larger Bjerrum length, or a "
                        f"lower volume fraction.")
                else:
                    out["lines"]["Gazzillo MSA (charged)"] = Sfin
                    out["notes"].append(
                        f"Gazzillo: {len(sig)} species, 2*Gamma*<sigma> = "
                        f"{2*gam*2*p['meanRadius']:.3f}")
            else:
                rhoT = p["phi"]/((np.pi/6.0)*float(np.sum(x*sigma**3)))
                out["lines"]["Gazzillo MSA (neutral = PY)"] = \
                    GZ.measurableStructureFactor(Q, sigma, np.zeros_like(sigma),
                                                 rhoT*x, None, 0.0)

            #--- mixscatter analytic Vrij PY ----------------------------
            if _ms is not None and abs(p["z"]) == 0:
                mix = _ms.Mixture(radius=sigma/2.0, number_fraction=x)
                py = _ms.PercusYevick(Q, mix, volume_fraction_total=p["phi"])
                S = np.asarray(py.number_weighted_partial_structure_factor, float)
                SAL = S/np.sqrt(np.outer(x, x))[:, :, None]
                F = self._sphereF(Q, sigma)
                F2 = np.sum(x[None, :]*F**2, axis=1)
                w = F/np.sqrt(F2)[:, None]
                sx = np.sqrt(x)
                out["lines"]["mixscatter (Vrij PY)"] = np.einsum(
                    'a,b,qa,qb,abq->q', sx, sx, w, w, SAL)

            #--- numerical solver, two grids ----------------------------
            #The bridge builds the potential with setPolydispersePotential,
            #which REFUSES charge-coupled potentials -- so the numerical curve
            #is a HARD-SPHERE mixture. Comparing that against the charged
            #Gazzillo reference would be comparing two different systems, and
            #the difference would say nothing about either implementation. So
            #the numerical curve is computed only in the neutral case, where
            #the comparison is like for like.
            if abs(p["z"]) > 0:
                out["notes"].append(
                    "numerical curve omitted: the polydisperse builder cannot "
                    "make a charge-coupled potential, so it would be a "
                    "hard-sphere mixture and not comparable with the charged "
                    "reference. Set z = 0 to compare implementations, or use "
                    "the RY Polydisperse Yukawa tab for a charged numerical "
                    "solve (a DIFFERENT model from Gazzillo's: effective "
                    "screened Coulomb versus explicit microions).")
            #BOTH pps AND N scale with the refinement, so that r_max =
            #N*sigma/pps is IDENTICAL for the two solves and the comparison
            #is a genuine refinement rather than a trade between near-contact
            #resolution and box size. See _numerical for what fixing N does
            #to the low-Q end.
            #
            #N stays of the form 2^k - 1, which is what makes the DST-I's
            #underlying FFT a power of two -- (4095+1)*f - 1 for f a power of
            #two, so 4095 -> 16383 -> 32767 -> 65535.
            coarseN = (1 << p["gridK"]) - 1
            fineN = (coarseN + 1)*p["refine"] - 1
            for tag, pps, gN in (((("numerical (coarse)", p["pps"], coarseN),
                                   ("numerical (fine)",
                                    p["refine"]*p["pps"], fineN)))
                                 if abs(p["z"]) == 0 else ()):
                try:
                    out["lines"][tag] = self._numerical(Q, sigma, x, p, pps,
                                                        gridN=gN)
                    out.setdefault("residuals", {})[tag] = \
                        getattr(self, "_lastResidual", None)
                except Exception as exc:
                    out["notes"].append(f"{tag} failed: {exc}")

            self.resultQueue.put(("done", out))
        except Exception as exc:
            self.resultQueue.put(("error", (exc, traceback.format_exc())))

    @staticmethod
    def _sphereF(Q, sigma):
        Xs = 0.5*np.asarray(Q, float)[:, None]*np.asarray(sigma, float)[None, :]
        V = (np.pi/6.0)*np.asarray(sigma, float)**3
        return V[None, :]*GZ._j1_over_x(Xs)

    def _numerical(self, Q, sigma, x, p, pps, gridN=None):
        """S_M(Q) from this package's solver, on explicit size classes.

        `gridN` scales WITH pps, so that refining the grid is a genuine
        refinement.

        The total real-space range is r_max = N*dr = N*sigma/pps, and the
        lowest accessible wavevector is q_min = pi/r_max. Refining pps at
        FIXED N therefore shrinks the box by the same factor: resolution
        near contact improves while the correlation tail is truncated, so
        the "fine" curve is better at high Q and WORSE at low Q. That is
        visible in the plot as the two numerical curves crossing.

        It also makes the convergence ratio meaningless, since it then
        mixes two errors moving in opposite directions -- which is why a
        ratio of 3.06 appeared where first order predicts 8.

        The old rule was a threshold ladder (4095 below 100 points per
        sigma, 16383 below 400, else 32767). Between 100 and 400 it happened
        to quadruple N alongside pps and the box stayed put by luck; at 800
        it gave only 32767, so r_max halved and low-Q accuracy fell away.
        """
        from mixscatter_bridge import OZLiquidStructure

        class _Mix:                       # minimal MixtureLike
            pass
        m = _Mix()
        m.radius = sigma/2.0
        m.number_fraction = x
        if gridN is None:
            gridN = 4095 if pps <= 100 else (16383 if pps <= 400 else 32767)
        oz = OZLiquidStructure(Q, m, volume_fraction_total=p["phi"],
                               closure=p["closure"], pointsPerSigma=pps,
                               gridN=gridN,
                               solverClass=p.get("solverClass"),
                               mannAlpha=p.get("mannAlpha"),
                               maxIterations=p.get("maxIter", 60000))
        #Mann damping, where the chosen solver honours it. Set after
        #construction is impossible -- OZLiquidStructure solves in its
        #__init__ -- so it goes in through solverClass and the attribute is
        #applied by the bridge.
        self._lastResidual = getattr(oz, "residual", None)
        SAL = oz.number_weighted_partial_structure_factor/ \
            np.sqrt(np.outer(x, x))[:, :, None]
        F = self._sphereF(Q, sigma)
        F2 = np.sum(x[None, :]*F**2, axis=1)
        w = F/np.sqrt(F2)[:, None]
        sx = np.sqrt(x)
        return np.einsum('a,b,qa,qb,abq->q', sx, sx, w, w, SAL)

    # ------------------------------------------------------------------
    def _poll(self):
        try:
            while True:
                kind, payload = self.resultQueue.get_nowait()
                if kind == "done":
                    self.curves = payload
                    self._summarise()
                    self._replot()
                    self.statusVar.set("done")
                else:
                    exc, tb = payload
                    self.statusVar.set("failed")
                    self.summary.delete("1.0", "end")
                    self.summary.insert("end", f"{type(exc).__name__}: {exc}\n\n{tb}")
                self.computeBtn.configure(state="normal")
        except queue.Empty:
            pass
        self.after(150, self._poll)

    def _summarise(self):
        c = self.curves
        lines = c.get("lines", {})
        txt = [f"{len(c['sigma'])} size classes",
               f"sigma {c['sigma'].min():.3g} .. {c['sigma'].max():.3g}", ""]
        ref = None
        for name in ("mixscatter (Vrij PY)", "Gazzillo MSA (neutral = PY)",
                     "Gazzillo MSA (charged)"):
            if name in lines:
                ref = name
                break
        if ref:
            txt.append(f"reference: {ref}")
            if "charged" in ref:
                txt.append("  (analytic only -- see note below)")
            base = lines[ref]
            errs = {}
            for name, y in lines.items():
                if name == ref:
                    continue
                e = float(np.max(np.abs(y - base))/max(np.max(np.abs(base)), 1e-30))
                errs[name] = e
                txt.append(f"  {name:22s} {e:.5f}")
            co, fi = errs.get("numerical (coarse)"), errs.get("numerical (fine)")
            if co and fi and fi > 0:
                ratio = co/fi
                #State what to EXPECT for the chosen refinement, since it is
                #no longer fixed at 4: with factor f and order p the ratio
                #should be f^p. Without this the reader has to remember
                #which factor was used to know whether 4 is good or bad.
                fr = int((self.curves or {}).get("refine", 4))
                #SOLVER RESIDUALS BESIDE THE ERRORS. If the fine solve's
                #residual is not well below the errors being compared, the
                #ratio is measuring the stopping rule and not the grid --
                #which is exactly how "the fine grid is less precise at low
                #Q" arose. Reporting both makes that readable instead of
                #something to be deduced.
                res = (self.curves or {}).get("residuals") or {}
                if res:
                    txt.append("")
                    txt.append("solver residual max|G(gamma)-gamma|:")
                    for k, v in res.items():
                        txt.append(f"  {k:22s} "
                                   + ("n/a" if v is None else f"{v:.3e}"))
                    worst = max((v for v in res.values() if v is not None),
                                default=None)
                    if worst is not None and worst > 0.1*min(co, fi):
                        txt.append("  WARNING: the residual is not small "
                                   "beside the differences above, so the")
                        txt.append("  comparison is limited by the solver, "
                                   "not by the grid.")
                txt += ["", f"coarse/fine ratio: {ratio:.2f}",
                        f"  (at {fr}x refinement: expect {fr} if first "
                        f"order, {fr*fr} if second; ~1 means the error is "
                        f"NOT discretisation)"]
                #The convergence verdict is only meaningful when grid error
                #actually dominates. Below ~0.5 % the residual is set by the
                #q-interpolation and by where in q the two curves differ, and
                #the ratio is then noise. Reporting "not converging" there
                #would cry wolf -- which this warning did on its first run,
                #at errors of 0.001.
                if co < 0.005:
                    txt.append("  already at the interpolation floor; the")
                    txt.append("  ratio is not meaningful this close to")
                    txt.append("  agreement. Raise phi or lower the grid to")
                    txt.append("  test convergence properly.")
                elif ratio > 2.0:
                    txt.append("  converging -- consistent with grid error")
                else:
                    txt.append("  NOT converging -- this is the signature of")
                    txt.append("  a defect, not discretisation. Check the")
                    txt.append("  pair potentials before the interpolation.")
        txt += [""] + c.get("notes", [])
        self.summary.delete("1.0", "end")
        self.summary.insert("end", "\n".join(txt))

    def _replot(self):
        self.ax.clear(); self.axErr.clear(); self._decorate()
        c = self.curves
        if c:
            Q = c["Q"]
            #A WIDE PALE BAND FOR THE REFERENCE, thin saturated lines on top.
            #
            #Four curves at similar widths look like one line, which hides
            #the very thing the tab is for: the reader cannot tell whether
            #four curves agree or three are missing. Drawing the reference as
            #a broad translucent ribbon and everything else as a thin opaque
            #line on top makes agreement READABLE -- a thin line centred in
            #the ribbon agrees, one riding its edge does not -- without
            #needing the error panel.
            #
            #Dash patterns differ as well as colours, so the distinction
            #survives greyscale printing and the common colour-vision
            #deficiencies.
            styles = {"mixscatter (Vrij PY)": dict(
                          color="#9e9e9e", lw=7.0, zorder=1, alpha=0.45,
                          solid_capstyle="round"),
                      "Gazzillo MSA (neutral = PY)": dict(
                          color="#e08214", lw=2.6, ls=(0, (7, 3)), zorder=2),
                      "Gazzillo MSA (charged)": dict(
                          color="#542788", lw=2.0, ls=(0, (1, 1.6)), zorder=3),
                      "numerical (coarse)": dict(
                          color="#d7191c", lw=1.4, ls=(0, (5, 2, 1, 2)),
                          zorder=4),
                      "numerical (fine)": dict(
                          color="#2166ac", lw=1.1, zorder=5)}
            ref, base = None, None
            for name, y in c["lines"].items():
                self.ax.plot(Q, y, label=name, **styles.get(name, {}))
                if ref is None and name.startswith(("mixscatter", "Gazzillo")):
                    ref, base = name, y
            if base is not None:
                for name, y in c["lines"].items():
                    if name == ref:
                        continue
                    with np.errstate(divide="ignore", invalid="ignore"):
                        #SAME COLOUR AND DASH AS THE UPPER PANEL.
                        #
                        #Passing no style let matplotlib assign its own
                        #cycle, so a curve drawn orange above appeared green
                        #below and the two panels had to be read separately.
                        #The whole value of the error panel is saying which
                        #of the curves above is misbehaving, which needs the
                        #identity carried across.
                        #
                        #The reference itself is excluded (it would be
                        #identically zero), so its ribbon style is never
                        #needed here; everything else keeps its colour but
                        #at a readable weight, since the pale wide ribbon
                        #would be unreadable on a log axis spanning sixteen
                        #decades.
                        st = dict(styles.get(name, {}))
                        st.pop("alpha", None)
                        st.pop("solid_capstyle", None)
                        st["lw"] = 1.1
                        st.pop("zorder", None)
                        self.axErr.plot(Q, np.abs(y/base - 1.0),
                                        label=f"{name} vs {ref}", **st)
                if self.axErr.get_legend_handles_labels()[0]:
                    self.axErr.legend(fontsize=6.5, framealpha=0.9,
                                      loc="lower left")
            #The structure factor rises to its peak on the RIGHT of this
            #plot and is flat and low on the left, so "upper left" put the
            #legend straight over the low-Q plateau -- the part a reader
            #checks first, since that is where the compressibility lives.
            #"lower right" is the empty quadrant here.
            self.ax.legend(fontsize=7, framealpha=0.9, loc="lower right")
        self.fig.tight_layout()
        self.canvas.draw_idle()


def main():                                                # pragma: no cover
    root = tk.Tk()
    root.title("Mixture validation")
    tab = MixtureValidationTab(root)
    tab.pack(fill="both", expand=True)
    root.geometry("1150x750")
    root.mainloop()


if __name__ == "__main__":                                 # pragma: no cover
    main()
