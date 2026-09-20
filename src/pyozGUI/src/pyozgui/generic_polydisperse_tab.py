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
    from polydisperse_yukawa_sas import Sphere, CoreShell, CoreShellFixedShell
    from polydisperse_tab_controls import PolydisperseTabControls
    from polydisperse_fit import PolydisperseFit, loadCurve, Resolution
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
    #Session file identity. The index appears in the file EXTENSION (.oz0)
    #so a file from another tab is recognisable before it is opened, and the
    #NAME is checked on load -- the extension is a convenience, the name is
    #the guard. Keep the index in step with the position in oZgui.EXTRA_TABS;
    #if the order ever changes, old files keep loading because the check is
    #on the name, and only the extension becomes stale.
    SESSION_TAB_INDEX = 1
    SESSION_TAB_NAME = "Polydisperse (any potential)"
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

        ttk.Label(left, text="Size distribution",
                  font=("TkDefaultFont", 9, "bold")).grid(
            row=r, column=0, columnspan=2, sticky="w"); r += 1
        #The engine has supported four distributions all along
        #(polydisperse_nodes.DISTRIBUTIONS); the tab passed "Schulz" as a
        #literal, so the other three were unreachable from the interface.
        #
        #The quadrature nodes are moment-matched for every one of them, so
        #the choice costs nothing numerically -- but it changes the TAIL,
        #and the tail is what the high-order moments see. <sigma^6> governs
        #I(Q -> 0), so two distributions fitted to the same mean and relative
        #width can still differ visibly at low Q. That is a reason to try
        #more than one on real data rather than to assume Schulz.
        ttk.Label(left, text="Distribution:").grid(row=r, column=0, sticky="e")
        self.distVar = tk.StringVar(value="Schulz")
        ttk.Combobox(left, textvariable=self.distVar, width=12,
                     state="readonly",
                     values=["Schulz", "Gaussian", "LogNormal", "Weibull"]
                     ).grid(row=r, column=1, sticky="w")
        r += 1
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
        #Shell THICKNESS, not a core/outer ratio. The label used to read
        #"Core/outer: 0.6", which was ambiguous -- it did not say whether the
        #number was a ratio or a length, nor which radius carried the size
        #distribution. The model is now unambiguous: the polydispersity is on
        #the CORE radius, every particle carries the same shell, and the
        #interaction diameter is 2(R_core + dR). Units are whatever the
        #radius uses, i.e. the reciprocal of Q's.
        self.shellVar = entry("Shell dR:", "2.7")
        #LINK the potential's range parameter to the shell thickness.
        #
        #For SquareWell and StickyHardSphere the second argument `delta` is
        #the well WIDTH as an absolute length: the well runs from sigma to
        #sigma+delta, in the same units as the radius. So it is directly
        #comparable with the shell thickness, and for a particle whose
        #attraction comes from its surface layer the two should not be free
        #of one another.
        #
        #When linked, delta = c * dR is recomputed at EVERY iteration from
        #the current dR, so it tracks the shell as the fit moves it rather
        #than being set once. delta then stops being an independent fit
        #parameter -- which is the point: fitting both a shell thickness and
        #an unrelated interaction range invites them to trade against each
        #other.
        #
        #c is the user's choice: 1 if the attraction range IS the layer, 2
        #if two layers overlap when particles touch (the usual brush or
        #depletion geometry).
        self.linkDeltaVar = tk.BooleanVar(value=False)
        ttk.Checkbutton(left, text="link delta = c x dR",
                        variable=self.linkDeltaVar,
                        command=self._syncLinkDelta).grid(
            row=r, column=0, columnspan=2, sticky="w")
        r += 1
        self.linkCVar = entry("   c:", "2.0")
        self.rhoCoreVar = entry("SLD core:", "2.0")
        self.rhoShellVar = entry("SLD shell:", "1.0")
        #Capture the core-shell entries HERE, before any further entries are
        #added. This used to read `self._ffEntries[-3:]` after the fact,
        #which silently grabbed whichever three entries happened to be last
        #-- adding the scale and background boxes below would have greyed
        #THOSE out for a plain sphere while leaving the SLD boxes editable.
        self._ffEntries = [w for w in left.winfo_children()
                           if isinstance(w, ttk.Entry)][-3:]

        #Scale and background apply to EVERY form factor, so they live here
        #with the general model parameters rather than in the core-shell
        #block, and are captured after _ffEntries so they are never greyed
        #out with it.
        #
        #Their fit checkboxes are NOT here: they sit under "Fit to measured
        #data" with the other parameters, so that one list shows everything
        #that is free. Ticked means free, unticked means held at the value in
        #the entry -- the same meaning as for every other parameter.
        self.scaleVar = entry("scale:", "1.0")
        self.backgroundVar = entry("background:", "0.0")
        #A sloping background: I_bg = background + A * Q^(-4+d).
        #
        #A flat constant describes only incoherent scattering. Real curves
        #frequently carry a power-law tail as well -- Porod's Q^-4 for a
        #smooth sharp interface, shallower for a rough or fractal one -- and
        #with only a constant available the fit absorbs that slope into the
        #shape parameters, where it does real damage.
        #
        #The AMPLITUDE is linear and solved exactly with the scale and the
        #constant; only the exponent is fitted. So this costs one extra
        #parameter, not two.
        self.porodVar = tk.BooleanVar(value=False)
        ttk.Checkbutton(left, text="background + A x Q^(-4+d)",
                        variable=self.porodVar,
                        command=self._syncPorod).grid(
            row=r, column=0, columnspan=2, sticky="w")
        r += 1
        self.porodAVar = entry("   A:", "0.0")
        self.porodDVar = entry("   d:", "0.0")
        #Below this Q the power law is held constant rather than
        #extrapolated. Q^(-4+d) diverges at the origin and the smearing
        #kernel reaches below the data, so something must bound it.
        #
        #The default 1e-6 is effectively NO clamp for a typical data set:
        #with Q_min ~ 0.02 1/nm, Q^-2.5 at 1e-6 is ~1e15, and the smeared
        #value at the first measured point is then dominated by a region
        #with no measurements in it. Set it to the lowest measured Q to
        #suppress that entirely; values in between trade the two off. It is
        #exposed because the right choice depends on whether the power law
        #is believed to continue below the measured range.
        self.porodQminVar = entry("   Q clamp:", "1e-6")
        #Which optimiser runs when Fit is pressed.
        #
        #"scipy least_squares" is the default and right for routine work: a
        #trust-region Levenberg-Marquardt converges in ~30 model evaluations.
        #The bumps methods evaluate the model FAR more often, and each
        #evaluation here is a full OZ solve -- 0.05 s for one component but
        #2.5 s at seven size classes. Measured on a small test problem,
        #amoeba took 79 s where least_squares took about one second.
        #
        #Rough guide at 2.5 s per evaluation:
        #    least_squares  ~30 evaluations   ~1 minute
        #    amoeba         ~200              ~8 minutes
        #    de             ~2000             ~1.5 hours
        #    dream          ~10000+           ~7 hours
        #
        #So these are not alternatives for everyday fitting. What they ARE
        #for: `dream` samples a posterior and returns a correlation matrix,
        #the only honest way to see whether eleven free parameters are really
        #eleven -- shell against linkC, phi against radius, A against
        #background all trade. And a population method can cross between
        #minima that a local one cannot, which matters when a branch or a
        #sign is in question. Run those once on a converged fit rather than
        #while exploring, and reduce the size classes first.
        ttk.Label(left, text="Fitter:").grid(row=r, column=0, sticky="e")
        self.fitterVar = tk.StringVar(value="scipy least_squares")
        ttk.Combobox(left, textvariable=self.fitterVar, width=18,
                     state="readonly",
                     values=["scipy least_squares",
                             "scipy least_squares (10 starts)",
                             "nlopt LN_COBYLA (constrained)",
                             "nlopt LN_BOBYQA (constrained)",
                             "bumps amoeba",
                             "bumps de", "bumps dream", "bumps newton",
                             "bumps lm", "bumps pt"]).grid(
            row=r, column=1, sticky="w")
        r += 1
        #Warm start: seed each OZ solve with the previous converged gamma
        #instead of starting from zero.
        #
        #Faster -- measured 1.29x on one component, and more at seven size
        #classes where a single solve costs 2.5 s rather than 0.015 s. It
        #also keeps the fit on ONE branch of the closure equations, which
        #makes the residual smooth in the parameters; a cold start can land
        #on a different branch between adjacent iterations.
        #
        #Off by default, because it can also carry a fit onto a wrong branch
        #and hold it there. The residual check and the min S(Q) >= 0 screen
        #run on every solve and bound that risk, but the default should not
        #change behaviour silently.
        self.warmStartVar = tk.BooleanVar(value=False)
        ttk.Checkbutton(left, text="warm start OZ solver",
                        variable=self.warmStartVar).grid(
            row=r, column=0, columnspan=2, sticky="w")
        r += 1
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
        #Enabled only when the loaded file carries a 4th (dQ) column.
        self.smearVar = tk.BooleanVar(value=False)
        self.smearCheck = ttk.Checkbutton(
            left, text="apply Q resolution (dQ column)",
            variable=self.smearVar, state="disabled")
        self.smearCheck.grid(row=r, column=0, columnspan=2, sticky="w"); r += 1
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
        self._potEntries = []
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
            entryWidget = ttk.Entry(self.potFrame, textvariable=var, width=10)
            entryWidget.grid(
                row=i, column=1, sticky="w")
            self._potEntries.append(entryWidget)
            self.potParamVars.append(var)
        if not names:
            ttk.Label(self.potFrame, text="(no parameters)").grid(row=0, column=0, sticky="w")
        #Refresh the fit checkboxes: the potential's own parameters appear
        #there as pot0, pot1, ... and that list is built from potParamVars,
        #which has just been rebuilt. Without this the boxes are whatever the
        #PREVIOUS potential had -- or none at all, if the flags happened to
        #be built while potParamVars was still empty.
        #
        #This used to work only by accident of ordering, and adding another
        #caller of _rebuildFitFlags elsewhere broke it. Rebuild explicitly
        #wherever the underlying list changes.
        if hasattr(self, "fitVarFrame"):
            self._rebuildFitFlags()
        #Re-apply the delta link: the entries were just recreated, and
        #whether one of them is linkable depends on the potential that has
        #only now been selected.
        if hasattr(self, "linkDeltaVar"):
            self._syncLinkDelta()

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

    def _syncPorod(self):
        """Refresh the fit flags when the power-law term is toggled.

        `porodD` is offered only while the term is active: fitting an
        exponent that multiplies nothing would be a parameter with no effect
        on the model.
        """
        if hasattr(self, "fitVarFrame"):
            self._rebuildFitFlags()

    def _syncLinkDelta(self):
        """Grey out the linked potential argument and refresh the fit flags.

        The linked parameter is no longer independently adjustable, so it is
        disabled rather than left editable with its value silently ignored --
        an entry that accepts a number and then discards it is worse than no
        entry at all.
        """
        linked = self.linkDeltaVar.get() and self._deltaParamIndex() is not None
        idx = self._deltaParamIndex()
        for i, w in enumerate(getattr(self, "_potEntries", [])):
            try:
                w.configure(state="disabled" if (linked and i == idx)
                            else "normal")
            except Exception:
                pass
        if hasattr(self, "fitVarFrame"):
            self._rebuildFitFlags()

    def _deltaParamIndex(self):
        """Index of the potential argument that is a range, or None.

        Only the potentials whose second argument is an absolute well WIDTH
        qualify. Linking anything else would be meaningless -- a Yukawa
        screening length is an inverse length, and a Lennard-Jones epsilon is
        an energy.
        """
        if self.potentialVar.get() in ("SquareWell", "StickyHardSphere"):
            return 1 if len(getattr(self, "potParamVars", [])) > 1 else None
        return None

    def _syncFormFactor(self):
        state = "normal" if self.ffVar.get() == "Core-shell" else "disabled"
        for w in self._ffEntries:
            w.configure(state=state)
        #Rebuild the fit checkboxes too: "shell", "rhoCore" and "rhoShell"
        #are offered only for a core-shell form factor, so switching the
        #combobox has to refresh that list or the checkboxes never appear.
        #
        #Guarded because this also runs during construction, before the fit
        #frame exists.
        if hasattr(self, "fitVarFrame"):
            self._rebuildFitFlags()

    # ------------------------------------------------------------------
    #Fitting
    #
    #Scale and background are NOT listed here: they enter the model linearly
    #and are solved for exactly at every iteration by weighted linear least
    #squares (see polydisperse_fit). Handing them to the nonlinear optimiser
    #would add the two most strongly correlated parameters -- scale against
    #volume fraction, background against everything at high Q -- for no gain.
    FIT_BOUNDS = {"meanRadius": (1e-3, 1e5), "srel": (1e-3, 0.6),
                  #phi is a volume fraction, so 0 < phi < 1 is the physical
                  #range and the bound says exactly that. It is NOT narrowed
                  #to the hard-sphere packing limit (~0.64): a fitted phi
                  #that runs above it is a RESULT -- it says the model is
                  #wrong for this data -- and a bound that hid it would turn
                  #a diagnosis into a silent clamp.
                  "phi": (1e-9, 1.0), "closureParam": (1e-3, 50.0),
                  #Shell thickness is STRICTLY positive. Zero is not merely
                  #an edge case: at dR = 0 the core-shell form factor
                  #degenerates to a sphere and the two SLDs stop being
                  #separately identifiable, so the optimiser can park there
                  #and report convergence on a model with fewer effective
                  #parameters than it thinks. The lower bound is small
                  #rather than zero for that reason.
                  "shell": (1e-9, 1e4),
                  #c in delta = c * dR. Bounded 0 to 2 (Joachim): 0 removes
                  #the attraction entirely, 1 means the interaction range IS
                  #the layer, 2 means two layers overlap when particles
                  #touch. Values above 2 would put the well beyond the
                  #combined coatings, which the linked parametrisation is not
                  #meant to describe.
                  "linkC": (0.0, 2.0),
                  #rhoRatio = rhoShell/rhoCore. Fit THIS rather than the
                  #shell SLD itself.
                  #
                  #The absolute scattering length densities are perfectly
                  #correlated with the scale -- I depends on scale times the
                  #squared contrast -- so their magnitude carries no
                  #information and need not be fitted at all. What the data
                  #DO determine is the ratio of the two contrasts, and above
                  #all its SIGN.
                  #
                  #Fitting the ratio also makes the parameter refine. In
                  #1/cm^2 an SLD is ~1e10, and least_squares terminates on
                  #its gradient test after a single evaluation at that
                  #magnitude: the parameter never moves, and the fit returns
                  #the starting value with a meaningless uncertainty. A ratio
                  #of order one has no such problem.
                  #
                  #Bounds span both signs and are wide: a negative ratio
                  #means the contrasts oppose, and that must be an OUTCOME of
                  #the fit, not an imposition.
                  "rhoRatio": (-1e3, 1e3),
                  #d in the exponent -4+d. Bounded 0 to 3: d = 0 is Porod's
                  #law, and d < 3 keeps the tail steeper than Q^-1 as
                  #Joachim specified. Outside that range the term stops
                  #describing a background and starts competing with the
                  #form factor.
                  "porodD": (0.0, 3.0),
                  #SLDs ARE UNBOUNDED, deliberately, on both counts.
                  #
                  #Sign: the opposite sign of the shell contrast must be an
                  #OUTCOME of the fit, not something the bounds impose. If
                  #the refined shell SLD comes out opposite to the core with
                  #the data free to choose otherwise, that is evidence;
                  #bounded to one side it would be an assumption wearing the
                  #costume of a result. (A starting value of opposite sign is
                  #a different matter and is fine -- it says where the search
                  #begins, not where it may go.)
                  #
                  #Magnitude: the units are the user's choice and span many
                  #orders. In 1/cm^2 a typical SLD is ~2e10, so ANY finite
                  #bound picked here would be wrong for some convention --
                  #a limit of 1e3 would clamp such a value at the bound on
                  #the first step, and the fit would report convergence while
                  #pinned there. There is no honest finite choice, so there
                  #is none.
                  #
                  #Unbounded also sidesteps the fallback heuristic below
                  #(x/50, x*50), which inverts or collapses for a negative
                  #start.
                  "rhoCore": (-np.inf, np.inf),
                  "rhoShell": (-np.inf, np.inf)}

    def _rebuildFitFlags(self):
        for w in self.fitVarFrame.winfo_children():
            w.destroy()
        self.fitFlags = {}
        names = ["meanRadius", "srel", "phi"]
        #Scale and background belong in this list for every form factor.
        #They are not handed to the nonlinear optimiser -- they enter the
        #model linearly and are solved EXACTLY by weighted least squares at
        #each iteration, which is both free and better conditioned. Ticking
        #them means "solve them"; unticking means "hold at the entry value".
        #Both default to ticked, which reproduces the previous behaviour.
        names += ["scale", "background"]
        #The shell thickness is fittable ONLY for a core-shell form factor,
        #and adding it to FIT_BOUNDS is not enough on its own: this list is
        #what the interface actually offers, so a parameter missing here can
        #never be selected however the rest of the machinery is wired.
        if self.ffVar.get() == "Core-shell":
            names += ["shell", "rhoRatio"]
        #c is fittable only when the link is active -- otherwise it does
        #nothing, and offering it would invite fitting a parameter with no
        #effect on the model.
        if (getattr(self, "linkDeltaVar", None) is not None
                and self.linkDeltaVar.get()
                and self._deltaParamIndex() is not None):
            names.append("linkC")
        if getattr(self, "porodVar", None) is not None and self.porodVar.get():
            #A behaves like scale and background: ticked means SOLVED
            #exactly by the linear least squares, unticked means held at the
            #entry value. It is listed here so that choice is visible in the
            #same place as every other one, even though it never reaches the
            #nonlinear optimiser.
            names += ["porodA", "porodD"]
        if self.closureParamVar is not None:
            names.append("closureParam")
        for i in range(len(self.potParamVars)):
            #A linked potential argument is not independently fittable: it is
            #computed from the shell thickness, so offering a checkbox for it
            #would let the user fit a quantity that is then overwritten.
            if self.linkDeltaVar.get() and i == self._deltaParamIndex():
                continue
            names.append("pot%d" % i)
        for i, n in enumerate(names):
            #Default-ticked: the shape parameters one normally fits, plus
            #the LINEAR terms, which are solved exactly at no cost. porodA
            #belongs with scale and background here -- leaving it out made it
            #default to HELD at the entry value, so switching the power law
            #on contributed a term fixed at zero and did nothing at all.
            v = tk.BooleanVar(value=(n in ("meanRadius", "srel", "phi",
                                           "scale", "background",
                                           "porodA")))
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

    # ------------------------------------------------------------------
    #Session save/restore. The controls call these if they exist; see
    #polydisperse_tab_controls._onSaveAll.
    #
    #Everything that makes a session RESUMABLE lives here: the input fields,
    #which parameters were free, the measured data, and the last fit result.
    #Saving the computed runs alone gives curves to look at but leaves you
    #re-entering every field and re-loading the data file by hand -- which is
    #precisely the friction that stops people re-running a fit with one
    #parameter changed.

    #Every Tk entry worth restoring, as attribute name -> saved key. Listed
    #explicitly rather than swept up by introspection, so adding a field is a
    #deliberate act and a renamed one fails loudly instead of silently
    #dropping out of the save.
    _SESSION_VARS = (
        "meanRadiusVar", "srelVar", "phiVar", "nbinsVar", "nFFVar",
        "shellVar", "rhoCoreVar", "rhoShellVar", "scaleVar", "backgroundVar",
        "QminVar", "QmaxVar", "nQVar", "ffVar", "potentialVar",
        "closureVar", "closureParamVar", "closureParam2Var", "distVar",
        "linkDeltaVar", "linkCVar", "porodVar", "porodDVar", "porodAVar",
        "porodQminVar", "fitterVar", "warmStartVar",
        #smearVar decides whether the dQ column is USED. Omitting it would
        #restore the data and the resolution array but silently lose the
        #choice to apply them -- and an unsmeared fit biases the
        #polydispersity high by ~15 % without any sign in chi-squared.
        "smearVar",
    )

    def sessionState(self):
        state = {"entries": {}, "fitFlags": {}, "data": None, "fit": None}
        for name in self._SESSION_VARS:
            var = getattr(self, name, None)
            if var is not None:
                try:
                    state["entries"][name] = var.get()
                except Exception:
                    pass
        for name, var in getattr(self, "fitFlags", {}).items():
            try:
                state["fitFlags"][name] = bool(var.get())
            except Exception:
                pass
        for name in ("potParamVars",):
            vs = getattr(self, name, None)
            if vs:
                state[name] = [v.get() for v in vs]
        #The measured data travel WITH the file. Storing only a path would
        #break the moment the file moved, and these arrays are small beside
        #the runs already being written.
        data = getattr(self, "data", None)
        if data is not None:
            Q, I, dI = data
            state["data"] = {
                "Q": np.asarray(Q, float).tolist(),
                "I": np.asarray(I, float).tolist(),
                "dI": (None if dI is None
                       else np.asarray(dI, float).tolist()),
                "dQ": (None if getattr(self, "dQ", None) is None
                       else np.asarray(self.dQ, float).tolist()),
                "path": getattr(self, "dataPath", None)}
        fit = getattr(self, "fitResult", None)
        if isinstance(fit, dict):
            #The fit result carries NUMPY ARRAYS (Q and the fitted curve)
            #that the plotting code reads back as fr["Q"] and fr["fit"].
            #An earlier version filtered the dict to JSON-safe scalars, which
            #dropped exactly those two -- so a restored session had a
            #fitResult without "Q", and redrawing raised KeyError('Q')
            #outside the restore's own error handling, killing the whole
            #load and taking the data with it.
            saved = {}
            for k, v in fit.items():
                if isinstance(v, np.ndarray):
                    saved[k] = {"__ndarray__": v.tolist()}
                elif isinstance(v, (int, float, str, bool, type(None),
                                    dict, list)):
                    saved[k] = v
            state["fit"] = saved
        return state

    def restoreSessionState(self, state):
        for name, value in (state.get("entries") or {}).items():
            var = getattr(self, name, None)
            if var is not None:
                try:
                    var.set(value)
                except Exception:
                    pass
        #REBUILD THE DEPENDENT WIDGETS FIRST, and in this order.
        #
        #_onPotentialChanged destroys and recreates potParamVars from the
        #potential just restored, so it MUST run before those values are
        #written back -- an earlier version restored them first, zipping
        #against the previous potential's (often empty) list, so the values
        #vanished and the potential appeared unset.
        #
        #_rebuildFitFlags runs last because the checkbox list is built from
        #everything above it: the form factor decides whether shell and the
        #SLDs appear, the potential decides how many pot0..potN there are.
        for hook in ("_onPotentialChanged", "_onClosureChanged",
                     "_syncFormFactor", "_rebuildFitFlags"):
            fn = getattr(self, hook, None)
            if callable(fn):
                try:
                    fn()
                except Exception:
                    pass
        #Closure parameters are recreated by _onClosureChanged, so restore
        #them after it rather than with the other entries.
        for name in ("closureParamVar", "closureParam2Var"):
            value = (state.get("entries") or {}).get(name)
            var = getattr(self, name, None)
            if value is not None and var is not None:
                try:
                    var.set(value)
                except Exception:
                    pass
        vals = state.get("potParamVars")
        if vals:
            for v, value in zip(getattr(self, "potParamVars", []), vals):
                try:
                    v.set(value)
                except Exception:
                    pass
        for name, value in (state.get("fitFlags") or {}).items():
            var = getattr(self, "fitFlags", {}).get(name)
            if var is not None:
                try:
                    var.set(bool(value))
                except Exception:
                    pass
        d = state.get("data")
        if d:
            Q = np.asarray(d["Q"], float)
            I = np.asarray(d["I"], float)
            dI = None if d.get("dI") is None else np.asarray(d["dI"], float)
            self.data = (Q, I, dI)
            self.dQ = (None if d.get("dQ") is None
                       else np.asarray(d["dQ"], float))
            self.dataPath = d.get("path")
            #Enable the Fit button. It starts disabled and is otherwise only
            #enabled by _onLoadData, so a session that restored the data
            #still left it greyed out -- the data were there, the fit was
            #not reachable. Anything _onLoadData enables on receiving data
            #has to be enabled here too.
            if hasattr(self, "fitBtn"):
                try:
                    self.fitBtn.configure(state="normal")
                except Exception:
                    pass
            #The smear checkbox is disabled until a dQ column exists, so it
            #must be re-enabled here or the restored smearVar cannot take
            #effect and the entry silently reverts.
            if self.dQ is not None and hasattr(self, "smearCheck"):
                try:
                    self.smearCheck.configure(state="normal")
                except Exception:
                    pass
            #Rebuild the data description line. The attribute is
            #`dataLabelVar`; an earlier version guessed `dataInfoVar`, which
            #does not exist, so the hasattr guard silently skipped it and the
            #line stayed blank after a restore.
            if hasattr(self, "dataLabelVar"):
                try:
                    bits = [f"{Q.size} points",
                            f"Q {Q.min():.4g}..{Q.max():.4g}",
                            "with dI" if dI is not None else "no dI"]
                    if self.dQ is not None:
                        frac = self.dQ/np.where(Q > 0, Q, np.nan)
                        bits.append(f"dQ/Q {np.nanmin(frac):.3f}.."
                                    f"{np.nanmax(frac):.3f}")
                    bits.append("restored from session")
                    self.dataLabelVar.set(", ".join(bits))
                except Exception:
                    pass
        if state.get("fit") is not None:
            #Decode the arrays written by sessionState. Anything that still
            #lacks "Q" came from an older save, and plotting it would raise
            #KeyError -- so drop it rather than restore a fitResult that
            #cannot be drawn.
            fit = {}
            for k, v in state["fit"].items():
                if isinstance(v, dict) and "__ndarray__" in v:
                    fit[k] = np.asarray(v["__ndarray__"], float)
                else:
                    fit[k] = v
            self.fitResult = fit if ("Q" in fit and "fit" in fit) else None

    # ------------------------------------------------------------------
    def _onLoadData(self):
        from tkinter import filedialog
        path = filedialog.askopenfilename(
            title="Load measured I(Q)",
            filetypes=[("data", "*.dat *.txt *.csv *.ASC"), ("All files", "*.*")])
        if not path:
            return
        try:
            Q, I, dI, dQ = loadCurve(path, withResolution=True)
        except Exception as e:
            messagebox.showerror("load failed", str(e))
            return
        self.data = (Q, I, dI)
        #Remembered for the session save, so a restored file can say where
        #the data came from.
        self.dataPath = path
        #A FOURTH column is taken as the Q resolution. It is read as the
        #Gaussian SIGMA, not the FWHM -- the two differ by 2.355 and the wrong
        #one silently rescales the fitted polydispersity, which is exactly the
        #bias smearing exists to remove. The checkbox lets it be switched off
        #so the effect can be seen; it is not there to be left off.
        self.dQ = dQ
        bits = [f"{len(Q)} points", f"Q {Q.min():.4g}..{Q.max():.4g}"]
        bits.append("with dI" if dI is not None else "no dI")
        if dQ is not None:
            frac = dQ/np.where(Q > 0, Q, np.nan)
            bits.append(f"dQ/Q {np.nanmin(frac):.3f}..{np.nanmax(frac):.3f}")
            self.smearCheck.configure(state="normal")
            self.smearVar.set(True)
        else:
            bits.append("no dQ column")
            self.smearCheck.configure(state="disabled")
            self.smearVar.set(False)
        self.dataLabelVar.set(", ".join(bits))
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
        #Both of these are reachable from the fit-flag list, so omitting them
        #here raises KeyError the moment the box is ticked.
        if name == "shell":
            return float(self.shellVar.get())
        if name == "rhoCore":
            return float(self.rhoCoreVar.get())
        if name == "rhoShell":
            return float(self.rhoShellVar.get())
        if name == "rhoRatio":
            #Starting value from the two entries, so the interface stays in
            #the units the user thinks in while the FIT works in a ratio.
            core = float(self.rhoCoreVar.get())
            shell = float(self.rhoShellVar.get())
            return shell/core if core != 0.0 else 0.0
        if name == "linkC":
            return float(self.linkCVar.get())
        if name == "porodD":
            return float(self.porodDVar.get())
        if name == "porodA":
            return float(self.porodAVar.get())
        if name == "scale":
            return float(self.scaleVar.get())
        if name == "background":
            return float(self.backgroundVar.get())
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
        #Warn before a fit that would hold a linear term at exactly zero.
        #Both the background and the power-law amplitude default to 0 in
        #their entries, so an unticked box pins them at nothing -- the fit
        #then reports a converged result that a manual value beats easily,
        #which looks like a broken solver and is not.
        try:
            pre = self._readInputs()
        except Exception:
            pre = None
        if pre and pre.get("heldAtZero"):
            names = ", ".join(pre["heldAtZero"])
            if not messagebox.askokcancel(
                    "parameter held at zero",
                    f"These are unticked under 'Fit to measured data' and "
                    f"their entries are 0, so the fit will hold them at "
                    f"zero:\n\n    {names}\n\n"
                    f"They enter the model linearly and are solved EXACTLY "
                    f"at no cost when ticked. Leaving them at zero is "
                    f"usually unintended.\n\nFit anyway?"):
                return
        #scale and background are ticked like the rest, but they must NOT go
        #to the nonlinear optimiser: they enter the model linearly and are
        #solved exactly by weighted least squares inside the fitter. Their
        #flags are read separately in _readInputs and passed as fixedScale /
        #fixedBackground. Leaving them in `params` would hand the optimiser
        #the two most strongly correlated parameters for nothing.
        LINEAR = ("scale", "background", "porodA")
        nonlinearFree = [n for n in free if n not in LINEAR]
        #The amplitude degeneracy is now structural rather than something to
        #be refused: the fitted contrast parameter is the RATIO
        #rhoShell/rhoCore, and the overall magnitude lives entirely in the
        #scale. So scale and rhoRatio are independent and may be fitted
        #together. The earlier guard against fitting scale with BOTH SLDs is
        #no longer reachable, because neither absolute SLD is offered.
        if not nonlinearFree:
            messagebox.showinfo(
                "nothing to fit",
                "scale and background are solved exactly rather than fitted.\n"
                "Tick at least one other parameter.")
            return
        params = {}
        for n in nonlinearFree:
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
            #KEYWORDS, not positional. This read
            #    CoreShell(p["shell"], p["rhoCore"], p["rhoShell"])
            #but the signature is
            #    CoreShell(rho_core, rho_shell, rho_solvent, thickness, ratio)
            #so the shell THICKNESS was passed as rho_core, rho_core as
            #rho_shell, rho_shell as rho_solvent, and `thickness` was left
            #None -- which the constructor's own guard rejects, since it
            #requires exactly one of thickness and ratio. The core-shell path
            #could therefore never have produced a fit.
            #A FACTORY, not an instance: the shell thickness may itself be a
            #fitted parameter, so the form factor has to be rebuilt from the
            #current vector at every iteration. Passing an instance here is
            #what froze the thickness before.
            #
            #CoreShellFixedShell puts the polydispersity on the CORE, with
            #R_outer = R_core + shell. The older CoreShell is polydisperse in
            #the OUTER radius instead; the two are different physical models
            #and give different polydispersity and effective volume fraction.
            if p["ff"] == "Core-shell":
                def makeFF(shell=p["shell"], rhoCore=p["rhoCore"],
                           rhoShell=p["rhoShell"], rhoRatio=None,
                           **_ignored):
                    #The SHELL SLD comes from the fitted RATIO when one is
                    #supplied. Only the ratio is determined by the data --
                    #the overall magnitude is perfectly correlated with the
                    #scale -- so the fit varies rhoRatio and the core SLD
                    #stays at its entered value, setting the units.
                    rs = (rhoShell if rhoRatio is None
                          else float(rhoRatio)*float(rhoCore))
                    return CoreShellFixedShell(rho_core=rhoCore,
                                               rho_shell=rs,
                                               thickness=shell)
            else:
                makeFF = None
            fixed = {"phi": p["phi"], "srel": p["srel"],
                     "meanRadius": p["meanRadius"],
                     "shell": p["shell"],
                     "rhoCore": p["rhoCore"],
                     "rhoShell": p["rhoShell"],
                     "rhoRatio": p["rhoRatio"],
                     "linkC": p["linkC"],
                     "porodD": p["porodD"],
                     "closureParam": p["closureParam"],
                     "closureParam2": p["closureParam2"]}
            fitter = PolydisperseFit(
                Q, I, dI, potential=p["potential"],
                potentialArgs=p["potentialArgs"], closure=p["closure"],
                parameters=params, fixed=fixed,
                nbins=p["nbins"], nFF=p["nFF"],
                distribution=p["distribution"],
                resolution=p["resolution"],
                formfactorFactory=makeFF,
                solverClass=p["solverClass"],
                warmStart=p["warmStart"],
                usePorod=p["usePorod"],
                porodQmin=p["porodQmin"],
                fixedPorodAmplitude=(p["porodA"] if p["fixPorodA"] else None),
                linkedArg=((p["deltaIndex"], p["linkC"], "shell")
                           if p["linkDelta"] and p["deltaIndex"] is not None
                           else None),
                fixedScale=(p["scale"] if p["fixScale"] else None),
                fixedBackground=(p["background"] if p["fixBackground"]
                                 else None),
                #Live updates. The worker must NOT touch Tk, so it only puts
                #the payload on the queue; _poll draws it on the main thread.
                #Throttled to one per second inside the fitter.
                onProgress=lambda d: self.resultQueue.put(("fitProgress", d)),
                progressInterval=1.0,
                shouldStop=lambda: getattr(self, "_abortFit", False))
            self.resultQueue.put(("status", f"fitting {len(params)} parameters..."))
            #Dispatch to the chosen optimiser. Both return the same result
            #dict, so everything downstream -- the plot, the write-back, the
            #session -- is unaffected by the choice.
            #
            #The bumps methods do NOT receive maxNfev: they carry their own
            #stopping rules, and a population method capped at 200
            #evaluations would stop long before its population had converged,
            #giving a worse answer than least_squares for far more time.
            name = p.get("fitter", "scipy least_squares")
            if name.startswith("nlopt "):
                #Constrained fit. NLopt is here ONLY for constraints that
                #relate several parameters -- above all that the shell must
                #fit inside the smallest core class, dR < R(1-n*srel), which
                #no box bound can express. An unconstrained fit on synthetic
                #data drove the shell to 0.84 against a limit of 0.63, i.e.
                #thicker than the core it sits on, and the constrained fit
                #found the right answer with a LOWER chi-squared.
                from polydisperse_fit import fitWithNLopt
                out = fitWithNLopt(
                    fitter, method=name.split()[1],
                    constraints=("shellInsideSmallestCore",)
                    if p["ff"] == "Core-shell" else ())
            elif name.startswith("bumps "):
                from polydisperse_fit import fitWithBumps
                out = fitWithBumps(fitter, method=name.split(None, 1)[1])
            elif "starts" in name:
                #Multi-start: the cheap way to ask whether the minimum is
                #unique. Ten local fits cost ~300 evaluations against the
                #thousands a population method needs, and they answer the
                #question that matters here -- a local optimiser cannot
                #cross from a repulsive square well (epsilon > 0) to an
                #attractive one, nor between contrast signs, so it reports
                #whichever branch it started on.
                out = fitter.runMultiStart(
                    nStarts=10, maxNfev=200,
                    onProgress=lambda d: self.resultQueue.put((
                        "status",
                        f"start {d['start']}/{d['of']}: "
                        f"chi2 = {d['chi2_reduced']:.5g}, "
                        f"best {d['best']:.5g}")))
            else:
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
        #delta = c * dR, applied here so the CALCULATE path is consistent
        #with the fit. The fitter recomputes it per iteration from the
        #current dR; this sets it for a single evaluation.
        p["linkDelta"] = bool(self.linkDeltaVar.get())
        p["usePorod"] = bool(self.porodVar.get())
        p["porodD"] = f(self.porodDVar, "d", lo=0.0)
        p["porodA"] = f(self.porodAVar, "A")
        p["porodQmin"] = f(self.porodQminVar, "Q clamp", lo=0.0)
        p["fitter"] = self.fitterVar.get()
        p["warmStart"] = bool(self.warmStartVar.get())
        #Ticked means FREE (solved exactly), so "fixed" is the negation --
        #the same convention as scale and background.
        p["fixPorodA"] = not bool(
            self.fitFlags["porodA"].get() if "porodA" in self.fitFlags
            else True)
        p["linkC"] = f(self.linkCVar, "c", lo=0.0)
        p["deltaIndex"] = self._deltaParamIndex()
        if p["linkDelta"] and p["deltaIndex"] is not None:
            args = list(p["potentialArgs"])
            args[p["deltaIndex"]] = p["linkC"]*f(self.shellVar, "Shell dR",
                                                 lo=0.0)
            p["potentialArgs"] = tuple(args)
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
        p["distribution"] = self.distVar.get()
        #Shell THICKNESS in the same length units as the radius, NOT a
        #core/outer ratio. The bound used to be hi=1.0, which silently
        #rejected any real thickness -- 2.7 nm on a 32 nm core would have
        #been refused as out of range.
        p["shell"] = f(self.shellVar, "Shell dR", lo=0.0)
        p["rhoCore"] = f(self.rhoCoreVar, "SLD core")
        p["rhoShell"] = f(self.rhoShellVar, "SLD shell")
        #The fitted quantity is the RATIO; the entries stay in the user's
        #own units and set both the starting point and the scale of the
        #problem.
        p["rhoRatio"] = (p["rhoShell"]/p["rhoCore"]
                         if p["rhoCore"] != 0.0 else 0.0)
        #Scale and background: the VALUE is used only when the matching box
        #is ticked, otherwise it is solved exactly and the entry just
        #displays the last solved value.
        p["scale"] = f(self.scaleVar, "scale")
        p["background"] = f(self.backgroundVar, "background")
        #Ticked means FREE, so "fixed" is the negation. Read through
        #fitFlags with a default of True so a missing flag means free, which
        #is the previous behaviour.
        p["fixScale"] = not bool(
            self.fitFlags["scale"].get() if "scale" in self.fitFlags else True)
        p["fixBackground"] = not bool(
            self.fitFlags["background"].get()
            if "background" in self.fitFlags else True)
        #HELD AT ZERO is almost always a mistake, and must be computed HERE,
        #after all three fix flags exist. The entries default to 0, so
        #unticking one of these silently pins a linear term at nothing -- and
        #because the solve is exact for whatever remains free, the fit
        #converges happily and reports a chi-squared that cannot be improved
        #BY THE OPTIMISER, while a better value is obvious by hand.
        #
        #The linear solve itself is optimal: a brute-force grid over
        #(background, A) cannot beat it. So a background that comes back as
        #exactly 0 means it was never solved for.
        p["heldAtZero"] = [
            n for n, fixed, val in
            (("scale", p["fixScale"], p["scale"]),
             ("background", p["fixBackground"], p["background"]),
             ("A", p["fixPorodA"] and p["usePorod"], p["porodA"]))
            if fixed and val == 0.0]
        p["Qmin"] = f(self.QminVar, "Q min", lo=1e-12)
        p["Qmax"] = f(self.QmaxVar, "Q max", lo=1e-12)
        p["nQ"] = int(f(self.nQVar, "Points", lo=2, hi=5000))
        if p["Qmax"] <= p["Qmin"]:
            raise ValueError("Q max must exceed Q min")
        #Read on the MAIN thread and carried into the worker. selectedSolverClass()
        #reads a Tk StringVar, and Tk may only be touched from the thread that
        #owns the interpreter -- calling it inside _worker() raised
        #"main thread is not in main loop" and lost the whole run.
        #Built on the MAIN thread: it only touches numpy, but the Tk variable
        #that decides whether to use it may not be read from the worker.
        p["resolution"] = None
        #The measured data travel with the parameter set so the worker can
        #score the computed curve against them.
        p["fitData"] = getattr(self, "data", None)
        #How many parameters the FIT would treat as free, so the computed
        #chi^2 uses the same degrees of freedom and the two are comparable.
        p["nFreeParameters"] = sum(
            1 for v in getattr(self, "fitFlags", {}).values() if v.get())
        if self.smearVar.get() and getattr(self, "dQ", None) is not None:
            Q = self.data[0]
            p["resolution"] = Resolution(Q, self.dQ)
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
            #Must match the FIT path exactly (see _fitWorker): the same
            #CoreShellFixedShell, polydisperse in the CORE. If calculate and
            #fit used different form factors the fitted curve would not
            #reproduce on recalculation, which is the kind of discrepancy
            #that gets blamed on the solver for weeks.
            ff = (CoreShellFixedShell(rho_core=p["rhoCore"],
                                      rho_shell=p["rhoShell"],
                                      thickness=p["shell"])
                  if p["ff"] == "Core-shell" else Sphere())
            alphaNote = ""
            if p["findAlpha"]:
                #alpha is not a free parameter when this is ticked: it is
                #fixed by requiring the compressibility and virial routes to
                #the pressure to agree. Each trial value costs three OZ
                #solves, hence the progress messages.
                import generic_polydisperse_sas as _gsas
                #The consistency search solves the same model, so it must use
                #the same distribution -- omitting it here would tune alpha
                #for a Schulz system and then apply it to a Weibull one.
                searchKw = dict(potential=p["potential"],
                                potentialArgs=p["potentialArgs"],
                                srel=p["srel"], nbins=p["nbins"],
                                closure=p["closure"], phi=p["phi"],
                                closureParam2=p["closureParam2"],
                                meanRadius=p["meanRadius"],
                                distribution=p["distribution"],
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
                distribution=p["distribution"],
                formfactor=ff, solverClass=p["solverClass"])
            self.solver = sas.solver          # so Interrupt can reach it
            #RESOLUTION SMEARING, applied to EVERY curve or none.
            #
            #When a resolution is active the curves are evaluated on the
            #kernel's extended grid and smeared back onto the DATA Q points,
            #so the output is directly comparable with the measurement.
            #Otherwise the usual logarithmic grid is used.
            #
            #Applying it to the exact curve alone would be worse than not
            #applying it at all: the whole purpose of plotting the six
            #approximations beside the exact result is to show how much each
            #approximation costs, and if only one of them were smeared, part
            #of the difference would be instrumental rather than a property
            #of the approximation. Near a form-factor minimum -- where
            #smearing fills the minimum in, and where the approximations
            #differ most -- the two effects would be entirely confounded.
            resolution = p.get("resolution")
            if resolution is not None:
                Qout, Qmodel = resolution.Q, resolution.Qext
                smear = resolution
            else:
                Qout = np.logspace(np.log10(p["Qmin"]), np.log10(p["Qmax"]),
                                   p["nQ"])
                Qmodel, smear = Qout, None
            res = _Result()
            res.Q = Qout
            res.smeared = smear is not None
            #APPLY SCALE AND BACKGROUND. The model returns I in its own
            #internal units; what a measurement sees is
            #
            #    I_obs(Q) = scale * I_model(Q) + background
            #
            #The calculate path used to store I_model raw, so changing the
            #scale entry had no visible effect and the curve could not be
            #placed on the same axes as measured data. The fit path applied
            #both all along -- solved by linear least squares -- so the two
            #paths disagreed by exactly this factor.
            #
            #What the scale MEANS: I(Q) = n <|F|^2 S(Q)>, so the prefactor is
            #the particle NUMBER DENSITY together with whatever conversion
            #the chosen units of Q and of the SLD imply. It is not a free
            #fudge: given the SLD convention and the fitted phi it is
            #predictable, and a fitted scale far from that prediction says
            #something is wrong with the contrast, the concentration or the
            #absolute calibration. Worth comparing rather than accepting.
            scale = float(p.get("scale", 1.0))
            background = float(p.get("background", 0.0))
            res.scale, res.background = scale, background
            #The power-law background, on the same footing as the constant:
            #I_bg = background + A * Q^(-4+d). Built on the MODEL grid and
            #smeared with everything else, since it is part of the
            #observable rather than a correction applied afterwards.
            #
            #This was missing here while the fitter applied it, so Compute
            #and Fit disagreed by exactly this term -- a fitted curve would
            #not have reproduced on recalculation.
            porod = None
            if p.get("usePorod"):
                d = float(p.get("porodD", 0.0))
                A = float(p.get("porodA", 0.0))
                #CLAMPED below the lowest OUTPUT Q, exactly as the fitter
                #does (PolydisperseFit._porodColumn). Q^(-4+d) diverges at
                #the origin and the smearing grid reaches far below the data
                #-- unclamped, the smeared power law came out 6.6e5 times
                #the unsmeared value at the lowest measured point, and the
                #answer depended on an arbitrary guard rather than on the
                #physics. The power law describes the measured range; it says
                #nothing about Q -> 0, where any real curve turns over.
                qFloor = max(float(p.get("porodQmin") or np.min(Qout)), 1e-30)
                with np.errstate(divide="ignore", invalid="ignore"):
                    col = np.power(np.maximum(np.asarray(Qmodel, float),
                                              qFloor), -4.0 + d)
                col = np.where(np.isfinite(col), col, 0.0)
                porod = A*col
            res.porodA = (float(p.get("porodA", 0.0))
                          if p.get("usePorod") else None)
            res.porodD = (float(p.get("porodD", 0.0))
                          if p.get("usePorod") else None)

            def observable(curve):
                """model -> what a measurement would see, smeared if active."""
                total = scale*curve
                if porod is not None:
                    total = total + porod
                return (smear(total) if smear is not None else total) \
                    + background

            res.I_exact = observable(sas.I_exact(Qmodel))
            #S(Q) and the partials are diagnostics, not observables, so they
            #stay on the output grid unsmeared and unscaled -- smearing them
            #would misrepresent the structure factor itself.
            res.S = sas.S_partials(Qout)
            res.S_number = sas.S_number(Qout)
            #chi^2 of the COMPUTED curve against the loaded data, so the
            #quality of a hand-set parameter set is visible WITHOUT running a
            #fit. That makes Compute usable for exploring -- change a
            #parameter, see whether chi^2 improves -- which is how one finds
            #a starting point good enough for the optimiser to work from.
            #
            #Must come after res.I_exact exists; it is computed here rather
            #than in the plotting code because this is where the data, the
            #model and the resolution are all in scope.
            res.chi2 = None
            data = p.get("fitData")
            if data is not None:
                Qd, Id, dId = (np.asarray(a, float) if a is not None else None
                               for a in data)
                Ie = np.asarray(res.I_exact, float)
                #With a resolution active the model already lands on the data
                #grid; without one it is on the logarithmic display grid, so
                #interpolate in log-log, where a scattering curve is smooth.
                if smear is not None and Ie.size == Qd.size:
                    model = Ie
                else:
                    good = (np.asarray(Qout) > 0) & np.isfinite(Ie) & (Ie > 0)
                    model = (np.exp(np.interp(
                        np.log(Qd), np.log(np.asarray(Qout)[good]),
                        np.log(Ie[good]))) if good.sum() > 2 else None)
                if model is not None:
                    #SAME degrees of freedom as the fitter, which divides by
                    #N - nFree - 2. Using N - 2 here made the computed chi^2
                    #systematically smaller than the fit's for identical
                    #parameters, so the two could not be compared -- and an
                    #apparent improvement from changing A or the background
                    #by hand was partly this offset rather than a better fit.
                    nFree = int(p.get("nFreeParameters", 0))
                    n = max(Qd.size - nFree - 2, 1)
                    if dId is not None and np.all(dId > 0):
                        res.chi2 = float(np.sum(((model - Id)/dId)**2)/n)
                    else:
                        #No uncertainties: the same log-residual measure the
                        #fitter falls back on, so the two agree.
                        ok = (model > 0) & (Id > 0)
                        res.chi2 = float(np.sum(
                            (np.log(model[ok]) - np.log(Id[ok]))**2)/n)
                    #The model grid comes from the Q min / Q max entries, NOT
                    #from the data. If it does not span the data the
                    #interpolation above EXTRAPOLATES, and the chi^2 is
                    #meaningless where it does. Say so rather than report a
                    #number that cannot be compared with anything.
                    if smear is None and (Qd.min() < np.min(Qout)*(1 - 1e-9)
                                          or Qd.max() > np.max(Qout)*(1 + 1e-9)):
                        res.chi2Note = (
                            f"model grid {np.min(Qout):.4g}..{np.max(Qout):.4g} "
                            f"does not span the data "
                            f"{Qd.min():.4g}..{Qd.max():.4g}; chi^2 relies on "
                            f"extrapolation and is NOT comparable with the "
                            f"fit's")
            res.sigma = sas.sigma.copy()
            res.w = sas.w.copy()
            res.label = (f"{p['potential']}, {p['closure']}, "
                         f"{p['distribution']}, "
                         f"phi={p['phi']:g}, s={p['srel']:g}")
            notes = ""
            for label, method in APPROX_SCHEMES:
                try:
                    #Same grid, same smearing, same scale and background as
                    #the exact curve -- see the note above.
                    res.approx[label] = observable(getattr(sas, method)(Qmodel))
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
                             #Multi-start and constrained runs each report
                             #something the others cannot, and both answer a
                             #question a chi-squared alone does not: whether
                             #the minimum is unique, and whether the physical
                             #constraints were actually met.
                             *([f"{payload['nStarts']} starts, "
                                f"{payload['distinctMinima']} distinct "
                                f"minimum/minima"]
                               if payload.get("nStarts") else []),
                             *([("constraints: " + ", ".join(
                                 f"{k} = {v:+.4g}"
                                 + (" MET" if v <= 1e-8 else " VIOLATED")
                                 for k, v in
                                 payload["constraintValues"].items()))]
                               if payload.get("constraintValues") else []),
                             f"scale = {payload['scale']:.6g}   "
                             f"background = {payload['background']:.6g}"
                             + ("" if payload.get("porodAmplitude") is None
                                else f"   A = {payload['porodAmplitude']:.6g}"),
                             ""]
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
                            elif k == "shell":
                                self.shellVar.set(f"{v:.6g}")
                            elif k == "rhoCore":
                                self.rhoCoreVar.set(f"{v:.6g}")
                            elif k == "rhoShell":
                                self.rhoShellVar.set(f"{v:.6g}")
                            elif k == "rhoRatio":
                                #Write back as an SLD, which is what the
                                #entry shows and what the user reasons about.
                                core = float(self.rhoCoreVar.get())
                                self.rhoShellVar.set(f"{v*core:.6g}")
                            elif k == "linkC":
                                self.linkCVar.set(f"{v:.6g}")
                            elif k == "porodD":
                                self.porodDVar.set(f"{v:.6g}")
                            elif k.startswith("pot"):
                                self.potParamVars[int(k[3:])].set(f"{v:.6g}")
                        except Exception:
                            pass
                    #Write the SOLVED scale and background back too. They are
                    #not fitted by the optimiser, so they never appear in
                    #payload["parameters"] and would otherwise stay invisible
                    #-- yet a scale far from unity, or a negative background,
                    #is a diagnostic worth seeing. Skipped where the user has
                    #fixed the value, so a ticked box is never overwritten by
                    #the number it forced.
                    try:
                        if "scale" not in self.fitFlags or \
                                self.fitFlags["scale"].get():
                            self.scaleVar.set(f"{payload['scale']:.6g}")
                        if "background" not in self.fitFlags or \
                                self.fitFlags["background"].get():
                            self.backgroundVar.set(
                                f"{payload['background']:.6g}")
                        if (payload.get("porodAmplitude") is not None
                                and ("porodA" not in self.fitFlags
                                     or self.fitFlags["porodA"].get())):
                            self.porodAVar.set(
                                f"{payload['porodAmplitude']:.6g}")
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
                if kind == "fitProgress":
                    #Best point so far, at most once a second. Draw it as a
                    #provisional fit so the convergence is visible, and mark
                    #the legend so it cannot be mistaken for the final
                    #result. Deliberately does NOT write back into the
                    #parameter entries: those should change once, when the
                    #fit finishes, not flicker while it runs.
                    self.fitResult = {**payload, "provisional": True,
                                      "chi2_reduced": payload.get("cost")}
                    self._replot()
                    self.statusVar.set(
                        f"fitting... {payload.get('nEvaluations', '?')} "
                        f"evaluations, cost {payload.get('cost', float('nan')):.5g}")
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
                #chi^2 and the LINEAR terms go in the legend. A fit is judged
                #by its chi-squared, and putting it where the curve is saves
                #reading it off the Summary tab -- but the background matters
                #just as much and is easy to miss: it is the only term that
                #can move the curve vertically, so it absorbs whatever the
                #SHAPE cannot match. A NEGATIVE background is therefore a
                #diagnostic, not a rounding artefact: it means the model
                #overshoots and the fit is buying agreement by subtracting a
                #constant that cannot physically be negative. It is shown
                #rather than clamped, because clamping would hide exactly the
                #signal worth seeing.
                bits = [("fitting: cost = " if fr.get("provisional") else
                         "fit: $\\chi^2_{red}$ = ")
                        + f"{fr.get('chi2_reduced', float('nan')):.4g}"]
                bg = fr.get("background")
                if bg is not None:
                    bits.append(f"bg = {bg:.4g}" + (" (NEGATIVE)" if bg < 0
                                                    else ""))
                sc = fr.get("scale")
                if sc is not None:
                    bits.append(f"scale = {sc:.4g}")
                A = fr.get("porodAmplitude")
                if A is not None:
                    bits.append(f"A = {A:.4g}")
                axd.plot(fr["Q"], fr["fit"], "-", color="C3", lw=1.6,
                         label="\n".join(bits), zorder=3)
            axd.legend(fontsize=7)
        r = self.result
        if r is not None:
            ax = self.axes["iq"]
            lab = "exact"
            c2 = getattr(r, "chi2", None)
            if c2 is not None:
                lab = f"exact ($\\chi^2_{{red}}$ = {c2:.4g})"
                if getattr(r, "chi2Note", None):
                    #The chi^2 is not comparable with the fit's. Say so on
                    #the plot: a number that looks like a chi-squared and is
                    #not one invites exactly the wrong conclusion.
                    lab += "\n(EXTRAPOLATED - not comparable)"
            ax.plot(r.Q, r.I_exact, "k-", lw=2, label=lab)
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
