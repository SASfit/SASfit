# -*- coding: utf-8 -*-
"""
Generic polydisperse notebook tab: ANY potential x ANY closure x ANY form
factor.

This is the tab the other three polydisperse tabs are special cases of:

    Polydisperse Yukawa      analytic MSA/RMSA, one closure
    Robertus SHS             analytic PY adhesive spheres, one closure
    RY Polydisperse Yukawa   numerical, one closure

Here the potential, the closure and the form factor are all chosen
independently, and the exact I(Q) is shown alongside all six of SASfit's
approximate schemes for combining a structure factor with a size
distribution, so the error of each can be read straight off the plot.

The physics lives elsewhere and is merely driven from here:

    oZfixpointOperator.setPolydispersePotential()  builds the (p,p,N) pair
        potential from any one-component setter, using additive hard cores
        sigma_ij = (sigma_i+sigma_j)/2 and an identical reduced tail
        u_ij(r) = u(r/sigma_ij)
    generic_polydisperse_sas.GenericPolydisperseSAS  solves it with the
        chosen closure and exposes I_exact plus the six schemes
    polydisperse_tab_controls.PolydisperseTabControls  supplies the standard
        button row, run history, save/load and export

WHAT IS AND IS NOT OFFERED
--------------------------
Potentials: every setXXXPotential() found by introspection EXCEPT the
charge-coupled ones (DLVO, DLVOHydra, IonicMicrogel, and the dedicated
polydisperse Yukawa). Their amplitude scales with particle size and kappa
depends on the whole distribution, so the reduced-tail mixing rule is simply
wrong for them; they have their own tabs.

Closures: ozLib.multicomponentCapableClosures(), i.e. everything except those
needing a one-component reference solve or a hard-sphere-specific
construction. Choosing one of those is not a matter of taste but of
structure, so they are not offered rather than being offered and failing.
"""
import queue
import sys
import threading
import traceback
import inspect

import tkinter as tk
from tkinter import ttk, messagebox

import numpy as np
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import (FigureCanvasTkAgg,
                                               NavigationToolbar2Tk)

IMPORT_ERROR = None
try:
    import ozLib
    from picardOZsolver import PicardOZsolver
    from generic_polydisperse_sas import GenericPolydisperseSAS
    from polydisperse_yukawa_sas import Sphere, CoreShell
    from polydisperse_tab_controls import PolydisperseTabControls
    from polydisperse_fit import PolydisperseFit, loadCurve
except Exception as _exc:                                  # pragma: no cover
    IMPORT_ERROR = _exc
    PolydisperseTabControls = object                       # so the class body parses


APPROX_SCHEMES = [
    ("0  monodisperse",                  "I_monodisperse"),
    ("1  decoupling (Kotlarchyk-Chen)",  "I_decoupling"),
    ("2  local monodisperse (Pedersen)", "I_lma"),
    ("3  partial structure factors",     "I_partial_sf"),
    ("4  scaling (Gazzillo)",            "I_scaling"),
    ("5  van der Waals one-fluid",       "I_vdw1"),
]

PLOT_TABS = [
    ("I(Q)",              "iq"),
    ("Approx. error",     "err"),
    ("S_ij(Q)",           "sij"),
    ("S(Q)",              "sq"),
    ("Size distribution", "dist"),
]


class _Result:
    """One completed calculation, handed worker -> main thread by queue."""
    def __init__(self):
        self.Q = None
        self.I_exact = None
        self.approx = {}
        self.S = None
        self.S_number = None
        self.sigma = None
        self.w = None
        self.summary = ""
        self.label = ""


class GenericPolydisperseTab(PolydisperseTabControls, ttk.Frame):
    def __init__(self, master, **kw):
        super().__init__(master, **kw)
        if IMPORT_ERROR is not None:
            ttk.Label(self, justify="left", padding=20, foreground="#a00",
                      text=("The generic polydisperse modules could not be "
                            "imported, so this tab is inactive.\n"
                            "The other tabs are unaffected.\n\n"
                            f"{type(IMPORT_ERROR).__name__}: {IMPORT_ERROR}")
                      ).pack(anchor="nw")
            return

        self.result = None
        self.runs = []
        self.data = None
        self.fitResult = None
        self._abortFit = False
        self.resultQueue = queue.Queue()
        self.worker = None
        self._polling = False
        self._probe = PicardOZsolver(port=0)

        self.columnconfigure(1, weight=1)
        self.rowconfigure(0, weight=1)
        self._buildControls()
        self._buildPlots()
        self._onPotentialChanged()
        self._onClosureChanged()
        self._syncFormFactor()

    # ------------------------------------------------------------------
    def _availablePotentials(self):
        names = sorted(self._probe.getAvailablePotentialNames())
        skip = set(PicardOZsolver.CHARGE_COUPLED_POTENTIALS)
        #PolydisperseHardCoreYukawa is charge-coupled, so the generic builder
        #refuses it -- but it has its OWN setter which handles the per-species
        #amplitude (Z_i ~ sigma_i^n) and the global kappa, and
        #GenericPolydisperseSAS dispatches to that setter by name. So it is
        #put back into the list: only the OTHER charge-coupled potentials
        #(DLVO, DLVOHydra, IonicMicrogel), which have no polydisperse setter
        #of their own, stay out.
        skip.discard("PolydisperseHardCoreYukawa")
        out = [n for n in names if n not in skip]
        if "PolydisperseHardCoreYukawa" not in out:
            out.append("PolydisperseHardCoreYukawa")
        return sorted(out)

    def _buildControls(self):
        outer = ttk.Frame(self, padding=6)
        outer.grid(row=0, column=0, sticky="nsew")
        #The action buttons and status line go in a footer packed to the
        #BOTTOM first, so they always stay visible and are never scrolled off.
        footer = ttk.Frame(outer)
        footer.pack(side="bottom", fill="x", pady=(8, 0))

        #Everything above them scrolls. The parameter block grows with the
        #chosen potential -- the charged Yukawa adds three fields, Extended
        #Rogers-Young a fourth -- and on a laptop screen that overflowed the
        #window with no way to reach the lower entries.
        canvas = tk.Canvas(outer, borderwidth=0, highlightthickness=0, width=300)
        vbar = ttk.Scrollbar(outer, orient="vertical", command=canvas.yview)
        canvas.configure(yscrollcommand=vbar.set)
        vbar.pack(side="right", fill="y")
        canvas.pack(side="left", fill="both", expand=True)
        left = ttk.Frame(canvas)
        window = canvas.create_window((0, 0), window=left, anchor="nw")
        #Two bindings, both needed: the first keeps the scrollable region in
        #step as widgets are added or removed (the parameter fields are
        #rebuilt whenever the potential or closure changes), the second makes
        #the inner frame follow the canvas width so nothing is clipped
        #horizontally.
        left.bind("<Configure>",
                  lambda e: canvas.configure(scrollregion=canvas.bbox("all")))
        canvas.bind("<Configure>",
                    lambda e: canvas.itemconfigure(window, width=e.width))

        def _wheel(event):
            #Windows and macOS deliver <MouseWheel> with a delta; X11 sends
            #Button-4/5 instead, so both are bound.
            if getattr(event, "num", None) == 4:
                delta = -1
            elif getattr(event, "num", None) == 5:
                delta = 1
            else:
                delta = -1 if event.delta > 0 else 1
            canvas.yview_scroll(delta, "units")

        for widget in (canvas, left):
            widget.bind("<MouseWheel>", _wheel)
            widget.bind("<Button-4>", _wheel)
            widget.bind("<Button-5>", _wheel)
        self._controlCanvas = canvas

        r = 0
        ttk.Label(left, text="Potential", font=("TkDefaultFont", 9, "bold")
                  ).grid(row=r, column=0, columnspan=2, sticky="w"); r += 1
        self.potentialVar = tk.StringVar(value="HardSphere")
        box = ttk.Combobox(left, textvariable=self.potentialVar, width=22,
                           state="readonly", values=self._availablePotentials())
        box.grid(row=r, column=0, columnspan=2, sticky="w"); r += 1
        box.bind("<<ComboboxSelected>>", lambda e: self._onPotentialChanged())
        self.potFrame = ttk.Frame(left)
        self.potFrame.grid(row=r, column=0, columnspan=2, sticky="w"); r += 1
        self.potParamVars = []

        ttk.Separator(left, orient="horizontal").grid(
            row=r, column=0, columnspan=2, sticky="ew", pady=6); r += 1

        ttk.Label(left, text="Closure", font=("TkDefaultFont", 9, "bold")
                  ).grid(row=r, column=0, columnspan=2, sticky="w"); r += 1
        self.closureVar = tk.StringVar(value="Percus-Yevick")
        cbox = ttk.Combobox(left, textvariable=self.closureVar, width=22,
                            state="readonly",
                            values=ozLib.multicomponentCapableClosures())
        cbox.grid(row=r, column=0, columnspan=2, sticky="w"); r += 1
        cbox.bind("<<ComboboxSelected>>", lambda e: self._onClosureChanged())
        self.closureFrame = ttk.Frame(left)
        self.closureFrame.grid(row=r, column=0, columnspan=2, sticky="w"); r += 1
        self.closureParamVar = None
        self.closureParam2Var = None
        #Solve for alpha instead of taking it from the field. Only offered for
        #closures that carry a free parameter AND are listed as capable of the
        #compressibility-vs-virial search; the entry is disabled while it is
        #ticked, so the two cannot silently disagree.
        self.findAlphaVar = tk.BooleanVar(value=False)
        self.findAlphaCheck = ttk.Checkbutton(
            left, text="solve \u03b1 by thermodynamic consistency",
            variable=self.findAlphaVar, command=self._syncAlphaEntry)
        self.findAlphaCheck.grid(row=r, column=0, columnspan=2, sticky="w"); r += 1
        ttk.Label(left, text="costs 3 OZ solves per trial value",
                  foreground="grey", wraplength=230, justify="left").grid(
            row=r, column=0, columnspan=2, sticky="w"); r += 1

        ttk.Separator(left, orient="horizontal").grid(
            row=r, column=0, columnspan=2, sticky="ew", pady=6); r += 1

        def entry(label, default, width=10):
            nonlocal r
            ttk.Label(left, text=label).grid(row=r, column=0, sticky="e")
            v = tk.StringVar(value=default)
            ttk.Entry(left, textvariable=v, width=width).grid(row=r, column=1, sticky="w")
            r += 1
            return v

        ttk.Label(left, text="Size distribution (Schulz)",
                  font=("TkDefaultFont", 9, "bold")).grid(
            row=r, column=0, columnspan=2, sticky="w"); r += 1
        self.meanRadiusVar = entry("Mean radius:", "50.0")
        ttk.Label(left, text="sets the length scale; Q is then a genuine inverse "
                             "length. The scattering radius equals the HARD-CORE "
                             "radius, R = sigma/2.",
                  foreground="grey", wraplength=230, justify="left").grid(
            row=r, column=0, columnspan=2, sticky="w"); r += 1
        self.srelVar = entry("Rel. s.d.:", "0.20")
        self.nbinsVar = entry("Classes (S):", "3")
        self.nFFVar = entry("Classes (form f.):", "40")
        ttk.Label(left, text="S(Q) needs few classes (moment-matched); the form "
                             "factor oscillates and needs many. Rule of thumb: "
                             "classes(form f.) >~ Qmax*sigma*s",
                  foreground="grey", wraplength=230, justify="left").grid(
            row=r, column=0, columnspan=2, sticky="w"); r += 1
        self.phiVar = entry("Volume fraction:", "0.20")

        ttk.Separator(left, orient="horizontal").grid(
            row=r, column=0, columnspan=2, sticky="ew", pady=6); r += 1

        ttk.Label(left, text="Form factor").grid(row=r, column=0, sticky="e")
        self.ffVar = tk.StringVar(value="Sphere")
        fbox = ttk.Combobox(left, textvariable=self.ffVar, width=12,
                            state="readonly", values=["Sphere", "Core-shell"])
        fbox.grid(row=r, column=1, sticky="w"); r += 1
        fbox.bind("<<ComboboxSelected>>", lambda e: self._syncFormFactor())
        self.shellVar = entry("Core/outer:", "0.6")
        self.rhoCoreVar = entry("SLD core:", "2.0")
        self.rhoShellVar = entry("SLD shell:", "1.0")
        self._ffEntries = []
        for w in left.winfo_children():
            if isinstance(w, ttk.Entry):
                self._ffEntries.append(w)
        self._ffEntries = self._ffEntries[-3:]

        ttk.Separator(left, orient="horizontal").grid(
            row=r, column=0, columnspan=2, sticky="ew", pady=6); r += 1
        self.QminVar = entry("Q min:", "1e-4")
        self.QmaxVar = entry("Q max:", "0.3")
        self.nQVar = entry("Points:", "200")

        ttk.Separator(left, orient="horizontal").grid(
            row=r, column=0, columnspan=2, sticky="ew", pady=6); r += 1
        ttk.Label(left, text="Fit to measured data",
                  font=("TkDefaultFont", 9, "bold")).grid(
            row=r, column=0, columnspan=2, sticky="w"); r += 1
        self.dataLabelVar = tk.StringVar(value="no data loaded")
        ttk.Label(left, textvariable=self.dataLabelVar, foreground="grey",
                  wraplength=230, justify="left").grid(
            row=r, column=0, columnspan=2, sticky="w"); r += 1
        dbtn = ttk.Frame(left)
        dbtn.grid(row=r, column=0, columnspan=2, sticky="w"); r += 1
        ttk.Button(dbtn, text="Load data...", command=self._onLoadData).pack(side="left", padx=2)
        self.fitBtn = ttk.Button(dbtn, text="Fit", command=self._onFit, state="disabled")
        self.fitBtn.pack(side="left", padx=2)
        #Which parameters to vary. The current field values are the starting
        #guess, so the workflow is Compute first to get roughly right by eye,
        #then tick and Fit -- which matters because every fit evaluation is a
        #full Ornstein-Zernike solve.
        ttk.Label(left, text="vary:", foreground="grey").grid(
            row=r, column=0, sticky="e")
        self.fitVarFrame = ttk.Frame(left)
        self.fitVarFrame.grid(row=r, column=1, sticky="w"); r += 1
        self.fitFlags = {}
        self._rebuildFitFlags()

        self.buildStandardControls(footer, supportsInterrupt=True,
                                    solverChoices=self._solverChoices())

    @staticmethod
    def _solverChoices():
        out = {}
        for label, mod, cls in (
                ("SUNDIALS KIN_FP", "sundials4pyKinsolFPOZsolver", "Sundials4pyKinsolFPOZsolver"),
                ("scipy Anderson", "scipyAndersonOZsolver", "ScipyAndersonOZsolver"),
                ("Anderson", "andersonOZsolver", "AndersonOZsolver"),
                ("Picard", "picardOZsolver", "PicardOZsolver")):
            try:
                out[label] = getattr(__import__(mod, fromlist=[cls]), cls)
            except Exception:
                pass
        return out

    # ------------------------------------------------------------------
    def _onPotentialChanged(self):
        for w in self.potFrame.winfo_children():
            w.destroy()
        self.potParamVars = []
        setter = getattr(self._probe, "set" + self.potentialVar.get() + "Potential")
        spec = inspect.getfullargspec(setter)
        names = spec[0][1:]
        #The charged Yukawa takes its width and class count from the size
        #distribution block, not from the potential fields, so only the
        #genuinely potential-specific arguments are shown. Order must match
        #what GenericPolydisperseSAS unpacks: (Z, L_B, chargeExponent).
        if self.potentialVar.get() == "PolydisperseHardCoreYukawa":
            names = ["referenceValence", "bjerrumLengthInSigmaUnits",
                     "chargeExponent"]
            spec = None
        nDef = len(spec.defaults) if (spec is not None and spec.defaults) else 0
        first = len(names) - nDef
        for i, name in enumerate(names):
            ttk.Label(self.potFrame, text=name + ":").grid(row=i, column=0, sticky="e")
            initial = "1.0"
            if self.potentialVar.get() == "PolydisperseHardCoreYukawa":
                #Defaults reproducing the D'Aguanno-Klein test system:
                #Z = 200, L_B = 7.01 A with <sigma> = 250 A, and exponent 2
                #(constant surface charge density).
                initial = {"referenceValence": "200.0",
                           "bjerrumLengthInSigmaUnits": "0.02804",
                           "chargeExponent": "2.0"}[name]
            elif spec is not None and i >= first:
                v = spec.defaults[i - first]
                if isinstance(v, (int, float)) and not isinstance(v, bool):
                    initial = repr(float(v))
            var = tk.StringVar(value=initial)
            ttk.Entry(self.potFrame, textvariable=var, width=10).grid(
                row=i, column=1, sticky="w")
            self.potParamVars.append(var)
        if not names:
            ttk.Label(self.potFrame, text="(no parameters)").grid(row=0, column=0, sticky="w")

    def _onClosureChanged(self):
        for w in self.closureFrame.winfo_children():
            w.destroy()
        self.closureParamVar = None
        self.closureParam2Var = None
        self._alphaEntry = None
        name = self.closureVar.get()
        _, needsParam = ozLib.CLOSURE_SETTERS[name]
        if needsParam:
            ttk.Label(self.closureFrame, text="\u03b1 / \u03b7:").grid(row=0, column=0, sticky="e")
            self.closureParamVar = tk.StringVar(value="1.0")
            self._alphaEntry = ttk.Entry(self.closureFrame,
                                         textvariable=self.closureParamVar, width=10)
            self._alphaEntry.grid(row=0, column=1, sticky="w")
        extra = ozLib.secondClosureParam(name)
        if extra is not None:
            pname, default, descr = extra
            ttk.Label(self.closureFrame, text=pname + ":").grid(row=1, column=0, sticky="e")
            self.closureParam2Var = tk.StringVar(value=repr(float(default)))
            ttk.Entry(self.closureFrame, textvariable=self.closureParam2Var,
                      width=10).grid(row=1, column=1, sticky="w")
            ttk.Label(self.closureFrame, text=descr, foreground="grey",
                      wraplength=220, justify="left").grid(
                row=2, column=0, columnspan=2, sticky="w")
        if hasattr(self, "findAlphaCheck"):
            self._syncAlphaEntry()
        #The set of fittable parameters depends on the closure (its alpha) and
        #the potential (its own arguments), so the check-boxes are rebuilt
        #whenever either changes.
        if hasattr(self, "fitVarFrame"):
            self._rebuildFitFlags()

    def _syncAlphaEntry(self):
        name = self.closureVar.get()
        _, needsParam = ozLib.CLOSURE_SETTERS[name]
        capable = needsParam and name in ozLib.CONSISTENT_PARAMETER_CLOSURES
        self.findAlphaCheck.configure(state="normal" if capable else "disabled")
        if not capable:
            self.findAlphaVar.set(False)
        if self._alphaEntry is not None:
            self._alphaEntry.configure(
                state="disabled" if self.findAlphaVar.get() else "normal")

    def _syncFormFactor(self):
        state = "normal" if self.ffVar.get() == "Core-shell" else "disabled"
        for w in self._ffEntries:
            w.configure(state=state)

    # ------------------------------------------------------------------
    #Fitting
    #
    #Scale and background are NOT listed here: they enter the model linearly
    #and are solved for exactly at every iteration by weighted linear least
    #squares (see polydisperse_fit). Handing them to the nonlinear optimiser
    #would add the two most strongly correlated parameters -- scale against
    #volume fraction, background against everything at high Q -- for no gain.
    FIT_BOUNDS = {"meanRadius": (1e-3, 1e5), "srel": (1e-3, 0.6),
                  "phi": (1e-4, 0.6), "closureParam": (1e-3, 50.0)}

    def _rebuildFitFlags(self):
        for w in self.fitVarFrame.winfo_children():
            w.destroy()
        self.fitFlags = {}
        names = ["meanRadius", "srel", "phi"]
        if self.closureParamVar is not None:
            names.append("closureParam")
        for i in range(len(self.potParamVars)):
            names.append("pot%d" % i)
        for i, n in enumerate(names):
            v = tk.BooleanVar(value=(n in ("meanRadius", "srel", "phi")))
            ttk.Checkbutton(self.fitVarFrame, text=n, variable=v).grid(
                row=i//2, column=i % 2, sticky="w")
            self.fitFlags[n] = v

    def _onInterrupt(self):
        """Stop a running FIT as well as a running single solve.

        The mixin's version only sets solverInstance.isInterrupted, which the
        fixpoint loops poll -- that stops one Ornstein-Zernike solve. During a
        fit the solves are created inside PolydisperseFit and are not reachable
        that way, and stopping one of them would merely make the optimiser take
        the next step. So a separate flag is raised, which the fit checks
        between evaluations and turns into a FitAborted.
        """
        self._abortFit = True
        super()._onInterrupt()
        self.statusVar.set("interrupt requested; stopping after the current "
                           "solve...")

    def _onLoadData(self):
        from tkinter import filedialog
        path = filedialog.askopenfilename(
            title="Load measured I(Q)",
            filetypes=[("data", "*.dat *.txt *.csv *.ASC"), ("All files", "*.*")])
        if not path:
            return
        try:
            Q, I, dI = loadCurve(path)
        except Exception as e:
            messagebox.showerror("load failed", str(e))
            return
        self.data = (Q, I, dI)
        self.dataLabelVar.set(f"{len(Q)} points, Q {Q.min():.4g}..{Q.max():.4g}"
                              + (", with dI" if dI is not None else ", no dI"))
        self.fitBtn.configure(state="normal")
        self._replot()

    def _currentValue(self, name):
        if name == "meanRadius":
            return float(self.meanRadiusVar.get())
        if name == "srel":
            return float(self.srelVar.get())
        if name == "phi":
            return float(self.phiVar.get())
        if name == "closureParam":
            return float(self.closureParamVar.get())
        if name.startswith("pot"):
            return float(self.potParamVars[int(name[3:])].get())
        raise KeyError(name)

    def _onFit(self):
        if getattr(self, "data", None) is None:
            messagebox.showinfo("no data", "load a data file first")
            return
        try:
            p = self._readInputs()
        except ValueError as e:
            messagebox.showerror("input error", str(e))
            return
        free = [n for n, v in self.fitFlags.items() if v.get()]
        if not free:
            messagebox.showinfo("nothing to fit", "tick at least one parameter")
            return
        params = {}
        for n in free:
            x = self._currentValue(n)
            lo, hi = self.FIT_BOUNDS.get(n, (x/50.0 if x > 0 else -abs(x)*50,
                                             abs(x)*50 + 1.0))
            params[n] = (x, min(lo, x*0.999), max(hi, x*1.001))
        self.computeBtn.configure(state="disabled")
        self.fitBtn.configure(state="disabled")
        self.interruptBtn.configure(state="normal")
        self._abortFit = False
        self.statusVar.set("fitting (each step is an OZ solve)...")
        self.worker = threading.Thread(target=self._fitWorker,
                                       args=(p, params), daemon=True)
        self.worker.start()
        if not self._polling:
            self._polling = True
            self.after(120, self._poll)

    def _fitWorker(self, p, params):
        try:
            Q, I, dI = self.data
            ff = (CoreShell(p["shell"], p["rhoCore"], p["rhoShell"])
                  if p["ff"] == "Core-shell" else Sphere())
            fixed = {"phi": p["phi"], "srel": p["srel"],
                     "meanRadius": p["meanRadius"],
                     "closureParam": p["closureParam"],
                     "closureParam2": p["closureParam2"]}
            fitter = PolydisperseFit(
                Q, I, dI, potential=p["potential"],
                potentialArgs=p["potentialArgs"], closure=p["closure"],
                parameters=params, fixed=fixed,
                nbins=p["nbins"], nFF=p["nFF"],
                distribution="Schulz",
                shouldStop=lambda: getattr(self, "_abortFit", False))
            self.resultQueue.put(("status", f"fitting {len(params)} parameters..."))
            out = fitter.run(maxNfev=200)
            self.resultQueue.put(("fit", out))
        except Exception as e:
            self.resultQueue.put(("error", (e, traceback.format_exc())))

    # ------------------------------------------------------------------
    def _buildPlots(self):
        right = ttk.Frame(self)
        right.grid(row=0, column=1, sticky="nsew")
        self.notebook = ttk.Notebook(right)
        self.notebook.pack(fill="both", expand=True)
        self.axes = {}
        self.canvases = {}
        for label, key in PLOT_TABS:
            frame = ttk.Frame(self.notebook)
            self.notebook.add(frame, text=label)
            fig = Figure(figsize=(6, 5), dpi=100)
            ax = fig.add_subplot(111)
            self._decorate(ax, key)
            canvas = FigureCanvasTkAgg(fig, master=frame)
            canvas.get_tk_widget().pack(fill="both", expand=True)
            NavigationToolbar2Tk(canvas, frame).update()
            self.axes[key] = ax
            self.canvases[key] = canvas
        tf = ttk.Frame(self.notebook)
        self.notebook.add(tf, text="Summary")
        self.summaryText = tk.Text(tf, wrap="none", font=("TkFixedFont", 9))
        sb = ttk.Scrollbar(tf, orient="vertical", command=self.summaryText.yview)
        self.summaryText.configure(yscrollcommand=sb.set)
        sb.pack(side="right", fill="y")
        self.summaryText.pack(side="left", fill="both", expand=True)

    @staticmethod
    def _decorate(ax, key):
        if key == "iq":
            ax.set_xlabel(r"$Q$  [1/length]"); ax.set_ylabel("I(Q)")
            ax.set_xscale("log"); ax.set_yscale("log")
        elif key == "err":
            ax.set_xlabel(r"$Q$  [1/length]"); ax.set_ylabel(r"$|I_{approx}/I_{exact}-1|$")
            ax.set_xscale("log"); ax.set_yscale("log")
        elif key == "sij":
            ax.set_xlabel(r"$Q$  [1/length]"); ax.set_ylabel(r"$S_{ij}^{AL}(Q)$"); ax.set_xscale("log")
        elif key == "sq":
            ax.set_xlabel(r"$Q$  [1/length]"); ax.set_ylabel("S(Q) (number-number)"); ax.set_xscale("log")
        elif key == "dist":
            ax.set_xlabel(r"diameter $\sigma$  [length]"); ax.set_ylabel("weight")

    # ------------------------------------------------------------------
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
        p = {}
        p["potential"] = self.potentialVar.get()
        p["potentialArgs"] = tuple(f(v, "potential parameter") for v in self.potParamVars)
        p["closure"] = self.closureVar.get()
        p["findAlpha"] = bool(self.findAlphaVar.get())
        p["closureParam"] = (None if p["findAlpha"] else
                             (f(self.closureParamVar, "closure parameter")
                              if self.closureParamVar is not None else None))
        p["closureParam2"] = (f(self.closureParam2Var, "second closure parameter")
                              if self.closureParam2Var is not None else None)
        p["meanRadius"] = f(self.meanRadiusVar, "Mean radius", lo=1e-12)
        p["srel"] = f(self.srelVar, "Rel. s.d.", lo=0.0, hi=0.99)
        p["nbins"] = int(f(self.nbinsVar, "Classes (S)", lo=1, hi=20))
        p["nFF"] = int(f(self.nFFVar, "Classes (form f.)", lo=1, hi=1000))
        p["phi"] = f(self.phiVar, "Volume fraction", lo=1e-9, hi=0.74)
        p["ff"] = self.ffVar.get()
        p["shell"] = f(self.shellVar, "Core/outer", lo=0.0, hi=1.0)
        p["rhoCore"] = f(self.rhoCoreVar, "SLD core")
        p["rhoShell"] = f(self.rhoShellVar, "SLD shell")
        p["Qmin"] = f(self.QminVar, "Q min", lo=1e-12)
        p["Qmax"] = f(self.QmaxVar, "Q max", lo=1e-12)
        p["nQ"] = int(f(self.nQVar, "Points", lo=2, hi=5000))
        if p["Qmax"] <= p["Qmin"]:
            raise ValueError("Q max must exceed Q min")
        #Read on the MAIN thread and carried into the worker. selectedSolverClass()
        #reads a Tk StringVar, and Tk may only be touched from the thread that
        #owns the interpreter -- calling it inside _worker() raised
        #"main thread is not in main loop" and lost the whole run.
        p["solverClass"] = self.selectedSolverClass()
        return p

    def _onCompute(self):
        if self.worker is not None and self.worker.is_alive():
            messagebox.showinfo("busy", "a calculation is already running")
            return
        try:
            p = self._readInputs()
        except ValueError as e:
            messagebox.showerror("input error", str(e))
            return
        self.computeBtn.configure(state="disabled")
        self.interruptBtn.configure(state="normal")
        self.statusVar.set("computing...")
        self.worker = threading.Thread(target=self._worker, args=(p,), daemon=True)
        self.worker.start()
        if not self._polling:
            self._polling = True
            self.after(120, self._poll)

    def _worker(self, p):
        try:
            ff = (CoreShell(p["shell"], p["rhoCore"], p["rhoShell"])
                  if p["ff"] == "Core-shell" else Sphere())
            alphaNote = ""
            if p["findAlpha"]:
                #alpha is not a free parameter when this is ticked: it is
                #fixed by requiring the compressibility and virial routes to
                #the pressure to agree. Each trial value costs three OZ
                #solves, hence the progress messages.
                import generic_polydisperse_sas as _gsas
                searchKw = dict(potential=p["potential"],
                                potentialArgs=p["potentialArgs"],
                                srel=p["srel"], nbins=p["nbins"],
                                closure=p["closure"], phi=p["phi"],
                                closureParam2=p["closureParam2"],
                                meanRadius=p["meanRadius"],
                                formfactor=ff, solverClass=p["solverClass"])
                alpha, resid, scale = _gsas.solveAlpha(
                    progress=lambda m: self.resultQueue.put(("status", m)),
                    **searchKw)
                p["closureParam"] = alpha
                #Relative test: chi^-1 sets the scale (order 10 for a dense
                #fluid), so an absolute residual of 1e-3 is a relative 1e-4.
                if abs(resid) < 1e-3*scale:
                    alphaNote = (f"alpha = {alpha:.4f} [consistent: residual "
                                 f"{resid:+.2e}, relative {abs(resid)/scale:.1e}]")
                else:
                    #Report rather than present a fallback as a fit: the
                    #residual is often monotone and already nonzero in the
                    #alpha -> 0 limit, i.e. the base closure is inconsistent
                    #for that state and no mixing repairs it.
                    alphaNote = (f"alpha = {alpha:.4f}  [NO consistent value "
                                 f"found; best residual {resid:+.3g}]")
                self.resultQueue.put(("status", alphaNote))
            sas = GenericPolydisperseSAS(
                p["potential"], p["potentialArgs"], p["phi"], p["srel"],
                nbins=p["nbins"], nFF=p["nFF"], closure=p["closure"],
                meanRadius=p["meanRadius"],
                closureParam=p["closureParam"], closureParam2=p["closureParam2"],
                formfactor=ff, solverClass=p["solverClass"])
            self.solver = sas.solver          # so Interrupt can reach it
            Q = np.logspace(np.log10(p["Qmin"]), np.log10(p["Qmax"]), p["nQ"])
            res = _Result()
            res.Q = Q
            res.I_exact = sas.I_exact(Q)
            res.S = sas.S_partials(Q)
            res.S_number = sas.S_number(Q)
            res.sigma = sas.sigma.copy()
            res.w = sas.w.copy()
            res.label = (f"{p['potential']}, {p['closure']}, "
                         f"phi={p['phi']:g}, s={p['srel']:g}")
            notes = ""
            for label, method in APPROX_SCHEMES:
                try:
                    res.approx[label] = getattr(sas, method)(Q)
                except Exception as e:
                    #One unavailable scheme must not lose the run: the
                    #monodisperse reference can have no solution at a state
                    #where the polydisperse system does.
                    res.approx[label] = None
                    notes += f"{label}: unavailable ({e})\n"
            lines = ([alphaNote] if alphaNote else []) + [
                     f"potential      : {p['potential']}{p['potentialArgs']}",
                     f"closure        : {p['closure']}"
                     + (f"  alpha={p['closureParam']:g}" if p['closureParam'] is not None else "")
                     + (f"  a={p['closureParam2']:g}" if p['closureParam2'] is not None else ""),
                     f"volume fraction: {p['phi']:g}",
                     f"rel. s.d.      : {p['srel']:g}   classes: {len(res.sigma)}",
                     f"sigma_eff      : {sas.sigma_eff():g}",
                     f"form factor    : {p['ff']}",
                     "", "class      sigma        weight"]
            for i, (s_, w_) in enumerate(zip(res.sigma, res.w)):
                lines.append(f"{i:5d} {s_:12.6g} {w_:12.6g}")
            lines += ["", "max |I_approx/I_exact - 1| over the plotted range:"]
            for label, _ in APPROX_SCHEMES:
                Ia = res.approx.get(label)
                if Ia is None:
                    lines.append(f"  {label:34s}  (unavailable)")
                else:
                    with np.errstate(divide="ignore", invalid="ignore"):
                        e = np.abs(Ia/res.I_exact - 1.0)
                    lines.append(f"  {label:34s}  {np.nanmax(e):.3e}")
            res.summary = "\n".join(lines) + (("\n\n" + notes) if notes else "")
            self.resultQueue.put(("done", res))
        except Exception as e:
            self.resultQueue.put(("error", (e, traceback.format_exc())))

    def _poll(self):
        try:
            while True:
                kind, payload = self.resultQueue.get_nowait()
                if kind == "fit":
                    self.fitResult = payload
                    lines = [(("FIT (INTERRUPTED -- best point so far)  "
                               if payload.get("aborted") else "FIT  ")
                              + f"chi2_red = {payload['chi2_reduced']:.4f}   "
                              + f"{payload['nEvaluations']} evaluations "
                              + f"({payload['failedEvaluations']} failed)"),
                             f"scale = {payload['scale']:.6g}   "
                             f"background = {payload['background']:.6g}", ""]
                    for k, v in payload["parameters"].items():
                        lines.append(f"   {k:14s} {v:.6g}")
                        #Write the fitted value back into the field it came
                        #from, so a following Compute reproduces the fit.
                        try:
                            if k == "meanRadius":
                                self.meanRadiusVar.set(f"{v:.6g}")
                            elif k == "srel":
                                self.srelVar.set(f"{v:.6g}")
                            elif k == "phi":
                                self.phiVar.set(f"{v:.6g}")
                            elif k == "closureParam":
                                self.closureParamVar.set(f"{v:.6g}")
                            elif k.startswith("pot"):
                                self.potParamVars[int(k[3:])].set(f"{v:.6g}")
                        except Exception:
                            pass
                    lines.append("")
                    lines.append(payload["message"])
                    self.summaryText.delete("1.0", "end")
                    self.summaryText.insert("end", "\n".join(lines))
                    self._replot()
                    self.statusVar.set(
                        (("fit interrupted; best chi2_red = ")
                         if payload.get("aborted") else "fit done: chi2_red = ")
                        + f"{payload['chi2_reduced']:.4f}")
                    self.computeBtn.configure(state="normal")
                    self.fitBtn.configure(state="normal")
                    self.interruptBtn.configure(state="disabled")
                    continue
                if kind == "status":
                    self.statusVar.set(payload)
                    continue
                if kind == "done":
                    self.registerRun(payload, payload.label)
                    self._replot()
                    self.statusVar.set("done: " + payload.label)
                else:
                    exc, tb = payload
                    print(tb, file=sys.stderr)
                    self.statusVar.set("failed (see Summary tab)")
                    self.summaryText.delete("1.0", "end")
                    self.summaryText.insert("end", str(exc))
                self.computeBtn.configure(state="normal")
                self.interruptBtn.configure(state="disabled")
        except queue.Empty:
            pass
        self.after(120, self._poll)

    # ------------------------------------------------------------------
    def _exportColumns(self):
        r = self.selectedRun()
        cols = ["Q", "I_exact", "S_number"]
        data = [r.Q, r.I_exact, r.S_number]
        for label, _ in APPROX_SCHEMES:
            Ia = r.approx.get(label)
            if Ia is not None:
                cols.append("scheme" + label.split()[0])
                data.append(Ia)
        return cols, data

    def _replot(self):
        for _, key in PLOT_TABS:
            self.axes[key].clear()
            self._decorate(self.axes[key], key)
        #Measured data and the fitted curve are drawn whether or not a Compute
        #result exists, so a freshly loaded file is visible at once.
        d = getattr(self, "data", None)
        if d is not None:
            Qd, Id, dId = d
            axd = self.axes["iq"]
            if dId is not None:
                axd.errorbar(Qd, Id, yerr=dId, fmt="o", ms=2.5, lw=0.6,
                             color="0.35", label="data", zorder=1)
            else:
                axd.plot(Qd, Id, "o", ms=2.5, color="0.35", label="data", zorder=1)
            fr = getattr(self, "fitResult", None)
            if fr is not None:
                axd.plot(fr["Q"], fr["fit"], "-", color="C3", lw=1.6,
                         label="fit", zorder=3)
        r = self.result
        if r is not None:
            ax = self.axes["iq"]
            ax.plot(r.Q, r.I_exact, "k-", lw=2, label="exact")
            for label, _ in APPROX_SCHEMES:
                Ia = r.approx.get(label)
                if Ia is not None:
                    ax.plot(r.Q, Ia, lw=1, label=label)
            ax.legend(fontsize=7)
            ax = self.axes["err"]
            for label, _ in APPROX_SCHEMES:
                Ia = r.approx.get(label)
                if Ia is None:
                    continue
                with np.errstate(divide="ignore", invalid="ignore"):
                    ax.plot(r.Q, np.abs(Ia/r.I_exact - 1.0), lw=1, label=label)
            ax.legend(fontsize=7)
            ax = self.axes["sij"]
            n = r.S.shape[1]
            for i in range(n):
                for j in range(i, n):
                    if i == j:
                        ax.plot(r.Q, r.S[:, i, j], lw=1.2,
                                label=f"S_{i}{j}" if n <= 6 else None)
                    else:
                        ax.plot(r.Q, r.S[:, i, j], lw=0.5, alpha=0.35)
            if n <= 6:
                ax.legend(fontsize=7)
            self.axes["sq"].plot(r.Q, r.S_number, "b-", lw=1.5)
            self.axes["dist"].bar(r.sigma, r.w,
                                  width=0.03*max(np.mean(r.sigma), 1e-12),
                                  color="#4477aa")
            self.summaryText.delete("1.0", "end")
            self.summaryText.insert("end", r.summary)
        for key in self.canvases:
            self.canvases[key].draw_idle()


def main():                                                # pragma: no cover
    root = tk.Tk()
    root.title("Generic polydisperse model")
    tab = GenericPolydisperseTab(root)
    tab.pack(fill="both", expand=True)
    root.geometry("1250x800")
    root.mainloop()


if __name__ == "__main__":                                 # pragma: no cover
    main()
