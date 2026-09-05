# -*- coding: utf-8 -*-
"""
Polydisperse hard-sphere-Yukawa (MSA / RMSA) notebook tab for oZgui.py.

This is a SELF-CONTAINED ttk.Frame subclass. It knows nothing about the OZgui
class and touches none of its state, so adding it to the GUI is a two-line
change in oZgui.py and removing it again is a two-line change back. The
existing Ornstein-Zernike solver tab is untouched by design.

    from polydisperse_yukawa_tab import PolydisperseYukawaTab
    tab = PolydisperseYukawaTab(someNotebook)
    someNotebook.add(tab, text="Polydisperse Yukawa")

It can also be run on its own for testing:

    python polydisperse_yukawa_tab.py

What it exposes, all of it already implemented and validated elsewhere in this
directory (see README_polydisperse_yukawa.md and docs/report/report.pdf):

  - a continuous size distribution (log-normal or Schulz-Zimm) discretised
    onto N components,
  - a form factor (homogeneous sphere or core-shell),
  - the analytic N-component one-Yukawa MSA closure and the exact partial
    structure factors S_ij(Q) that follow from it,
  - the rescaled closure (RMSA), with one common rescaling factor lambda
    chosen so that min_ij g_ij = 0,
  - the exact I(Q), and all six of SASfit's approximate schemes for combining
    a structure factor with a size distribution, so the error of each can be
    read straight off the plot.

SIGN CONVENTION, worth stating on screen and stating here: the MSA closure
sets c(r) = -beta U(r) outside the core, so K > 0 is an ATTRACTIVE tail and
K < 0 is a REPULSIVE one. This is the opposite of the sign convention some
SASfit plugins use for the Yukawa amplitude.

AI_USAGE.md note: the physics modules this drives were derived and verified by
the authors. The only new physics-adjacent code here is _apply_rmsa(), which
swaps a solved rescaled system into an already-constructed SAS object; it is
assembly of validated pieces rather than a new derivation, but it is flagged
here as wanting a look before it is relied on for published numbers.
"""
import os
import queue
import sys
import threading
import traceback

import tkinter as tk
from tkinter import ttk, filedialog, messagebox

import numpy as np
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import (FigureCanvasTkAgg,
                                               NavigationToolbar2Tk)

# The solver stack lives next to this file. Import failure must not take the
# whole GUI down -- oZgui.py checks IMPORT_ERROR and shows a placeholder tab.
IMPORT_ERROR = None
try:
    from polydisperse_yukawa_sas import (LogNormal, SchulzZimm, Gamma, Beta,
                                         Sphere, CoreShell,
                                         PolydisperseYukawaSAS, moment_report)
    from polydisperse_rmsa import solve_rmsa
    from polydisperse_yukawa_sq import PolydisperseOneYukawaSq
except Exception as _exc:                                  # pragma: no cover
    IMPORT_ERROR = _exc


# Approximation schemes, in SASfit's own numbering. The label is what the user
# sees; the attribute is the method on PolydisperseSASBase.
APPROX_SCHEMES = [
    ("0  monodisperse",              "I_monodisperse"),
    ("1  decoupling (Kotlarchyk-Chen)", "I_decoupling"),
    ("2  local monodisperse (Pedersen)", "I_lma"),
    ("3  partial structure factors", "I_partial_sf"),
    ("4  scaling (Gazzillo)",        "I_scaling"),
    ("5  van der Waals one-fluid",   "I_vdw1"),
]

# Parameters shown for each distribution: (label, key, default, hint).
# The panel rebuilds itself from this when the dropdown changes, so adding a
# distribution is a matter of adding a class in polydisperse_yukawa_sas.py,
# an entry here, and a branch in _makeDistribution().
DIST_PARAMS = {
    "Log-normal": [
        ("Mean radius", "R0",    "50.0", "in the same length unit as 1/z"),
        ("Rel. s.d.",   "relsd", "0.20", None),
    ],
    "Schulz-Zimm": [
        ("Mean radius", "R0",    "50.0", "in the same length unit as 1/z"),
        ("Rel. s.d.",   "relsd", "0.20", None),
    ],
    "Gamma": [
        ("Shape k",  "k",     "25.0", None),
        ("Scale th", "theta", "2.0",
         "mean = k*th,  rel.s.d. = 1/sqrt(k)\n"
         "same family as Schulz-Zimm: k = z+1, th = <R>/(z+1)"),
    ],
    "Beta": [
        ("R min", "Rmin", "20.0", None),
        ("R max", "Rmax", "90.0", "bounded support: 0 < R min < R max"),
        ("a",     "a",    "2.0",  None),
        ("b",     "b",    "5.0",  "shape parameters, both > 0"),
    ],
}
DIST_NAMES = list(DIST_PARAMS)

# The two independent solution routes of polydisperse_yukawa_msa.py. These are
# NOT iteration schemes -- the closure is solved in closed form -- so there is
# nothing here corresponding to the OZ solver tab's Picard/Anderson/KINSOL
# choice. They are two different algebraic reductions of the same equations,
# and they agree to machine precision; running both is a self-check.
ROUTE_GAMMA = "Gamma (eq.72)"
ROUTE_AMPL = "amplitudes (eq.55)"
ROUTE_BOTH = "both (cross-check)"
ROUTES = [ROUTE_GAMMA, ROUTE_AMPL, ROUTE_BOTH]

PLOT_TABS = [
    ("I(Q)",            "iq"),
    ("Approx. error",   "err"),
    ("S_ij(Q)",         "sij"),
    ("S(Q)",            "sq"),
    ("Size distribution", "dist"),
]


class _Result:
    """Everything one Compute produces, handed from the worker thread to the
    main thread through a queue (Tk is not thread-safe -- same pattern, and
    for the same reason, as OZgui._pollResultQueue)."""

    def __init__(self):
        self.Q = None
        self.I_exact = None
        self.approx = {}        # label -> I(Q)
        self.S = None           # (nQ, N, N)
        self.S_number = None
        self.R = None
        self.w = None
        self.sigma = None
        self.g = None           # contact matrix
        self.summary = ""
        self.label = ""


def _apply_rmsa(sas, K, lam_max=6.0):
    """Swap a solved RMSA system into an already-built PolydisperseYukawaSAS.

    S_partials() and hence I_exact() route through sas.sq, so replacing
    sas.solver/sas.sol/sas.sq with the rescaled ones is enough to put every
    downstream quantity on the RMSA footing. The number densities sas.rho and
    the radii sas.R are physical and deliberately NOT rescaled -- the form
    factor and the concentration are what they are; only the closure's
    effective hard core moves.

    K is passed explicitly rather than read from sas.solver, because for the
    RMSA path the SAS object is deliberately built at K = 0 (see
    _buildSAS): the whole point of RMSA is the regime where the plain MSA has
    no root at all, and PolydisperseYukawaSAS.__init__ solves the closure
    eagerly, so constructing it at the true K would raise before we ever got
    here.

    Returns the dict from solve_rmsa (lam, min_g, phi, phi_rescaled, status).
    """
    res = solve_rmsa(sas.solver.sigma, sas.solver.rho,
                     sas.solver.z, K, lam_max=lam_max)
    if res.get("lam") is None:
        return res
    sas.solver = res["solver"]
    sas.sol = res["sol"]
    sas.sq = PolydisperseOneYukawaSq(res["solver"], res["sol"])
    sas.__dict__.pop("_mono_cache", None)
    return res


def _makeDistribution(name, pars, momentRule=False):
    """Build the distribution object named by the dropdown from the parameter
    dict the control panel produced."""
    if name == "Log-normal":
        # the panel asks for the relative s.d. of R; convert to the log-space
        # width the class takes:  relsd = sqrt(e^{s^2} - 1)
        s = float(np.sqrt(np.log(1.0 + pars["relsd"]**2)))
        return LogNormal(pars["R0"], s,
                         rule="moment" if momentRule else "legendre")
    if name == "Schulz-Zimm":
        return SchulzZimm(pars["R0"], relsd=pars["relsd"])
    if name == "Gamma":
        return Gamma(pars["k"], pars["theta"])
    if name == "Beta":
        return Beta(pars["Rmin"], pars["Rmax"], pars["a"], pars["b"])
    raise ValueError(f"unknown distribution '{name}'")


def _explain(exc):
    """Turn the common, physically-meaningful failures into plain language.
    These are properties of the closure, not bugs: the MSA simply has no
    physical solution everywhere in the (phi, z, K) space."""
    msg = str(exc)
    if "no root for Gamma" in msg:
        return ("The MSA closure has no physical solution at this state point.\n"
                "\n"
                "This is a property of the mean-spherical approximation, not a\n"
                "numerical failure. It happens for strong coupling -- typically\n"
                "near the spinodal for attraction (K > 0), or at high volume\n"
                "fraction. Things to try:\n"
                "  - reduce |K| (weaker tail),\n"
                "  - reduce the volume fraction,\n"
                "  - increase the screening z (shorter-ranged tail),\n"
                "  - for strong REPULSION (K < 0), switch the closure to RMSA,\n"
                "    which rescales the core until the solution is physical.\n"
                "    RMSA does not rescue strong ATTRACTION.")
    if msg.startswith("RMSA:"):
        return ("RMSA could not find a rescaling factor.\n\n" + msg + "\n\n"
                "The rescaling inflates the hard cores until every g_ij is\n"
                "non-negative; if no factor up to the search limit achieves\n"
                "that, the state point is out of reach of this closure.")
    if "mpmath" in msg:
        return ("The moment-exact log-normal nodes need the mpmath package:\n\n"
                "    pip install mpmath\n\n"
                "Untick 'moment-exact nodes' to use the default rule instead.")
    if "undefined" in msg:
        return ("An approximate scheme has no solution in this state.\n\n" + msg)
    return f"{type(exc).__name__}: {msg}"


class PolydisperseYukawaTab(ttk.Frame):
    def __init__(self, master, **kw):
        super().__init__(master, **kw)

        if IMPORT_ERROR is not None:
            self._buildImportErrorNotice()
            return

        self.result = None
        self.resultQueue = queue.Queue()
        self.worker = None
        self._polling = False

        self.columnconfigure(1, weight=1)
        self.rowconfigure(0, weight=1)
        self._buildControls()
        self._buildPlots()
        self._syncEnabledStates()

    # ------------------------------------------------------------------
    def _buildImportErrorNotice(self):
        msg = ("The polydisperse Yukawa solver modules could not be imported,\n"
               "so this tab is inactive. The Ornstein-Zernike solver tab is\n"
               "unaffected.\n\n"
               f"{type(IMPORT_ERROR).__name__}: {IMPORT_ERROR}\n\n"
               "Expected next to oZgui.py:\n"
               "    polydisperse_yukawa_msa.py\n"
               "    polydisperse_yukawa_sq.py\n"
               "    polydisperse_yukawa_sas.py\n"
               "    polydisperse_sas_base.py\n"
               "    polydisperse_rmsa.py")
        ttk.Label(self, text=msg, justify="left", padding=20,
                  foreground="#a00").pack(anchor="nw")

    # ------------------------------------------------------------------
    def _buildControls(self):
        # The control panel is taller than the window on a default-sized
        # display, and Tk's font metrics differ between platforms, so a
        # fixed layout can and does push "Compute" off the bottom edge.
        # Two structural fixes rather than a bigger default window:
        #   * the action buttons and the status line live in a FOOTER packed
        #     to the bottom, so they are visible at any window height;
        #   * everything above them scrolls.
        left = ttk.Frame(self)
        left.grid(row=0, column=0, sticky="nsew")
        self.rowconfigure(0, weight=1)

        footer = ttk.Frame(left, padding=(6, 4))
        footer.pack(side="bottom", fill="x")
        ttk.Separator(left, orient="horizontal").pack(side="bottom", fill="x")

        scrollHost = ttk.Frame(left)
        scrollHost.pack(side="top", fill="both", expand=True)
        canvas = tk.Canvas(scrollHost, borderwidth=0, highlightthickness=0,
                           width=250)
        vbar = ttk.Scrollbar(scrollHost, orient="vertical",
                             command=canvas.yview)
        canvas.configure(yscrollcommand=vbar.set)
        vbar.pack(side="right", fill="y")
        canvas.pack(side="left", fill="both", expand=True)

        body = ttk.Frame(canvas, padding=6)
        window = canvas.create_window((0, 0), window=body, anchor="nw")

        def _onBodyConfigure(_e=None):
            canvas.configure(scrollregion=canvas.bbox("all"))
            # keep the canvas exactly as wide as the panel needs, so the
            # plots get all the remaining width
            canvas.configure(width=body.winfo_reqwidth())
        body.bind("<Configure>", _onBodyConfigure)
        canvas.bind("<Configure>",
                    lambda e: canvas.itemconfigure(window, width=e.width))

        def _wheel(event):
            delta = event.delta
            if event.num == 4:      # X11 sends button 4/5 rather than delta
                delta = 120
            elif event.num == 5:
                delta = -120
            canvas.yview_scroll(int(-delta/120), "units")
        for seq in ("<MouseWheel>", "<Button-4>", "<Button-5>"):
            canvas.bind_all(seq, lambda e: _wheel(e)
                            if str(e.widget).startswith(str(body)) or
                            str(e.widget).startswith(str(canvas)) else None)

        self._controlBody, self._controlFooter = body, footer
        left = body                      # everything below grids into body
        row = 0

        def header(text, r):
            ttk.Separator(left, orient="horizontal").grid(
                row=r, column=0, columnspan=2, sticky="ew", pady=(8, 2))
            ttk.Label(left, text=text, font=("TkDefaultFont", 9, "bold")).grid(
                row=r + 1, column=0, columnspan=2, sticky="w")
            return r + 2

        def entry(label, var, r, width=10, tip=None):
            ttk.Label(left, text=label).grid(row=r, column=0, sticky="e")
            e = ttk.Entry(left, textvariable=var, width=width)
            e.grid(row=r, column=1, sticky="w", padx=(4, 0))
            if tip:
                ttk.Label(left, text=tip, foreground="#666",
                          font=("TkDefaultFont", 7)).grid(
                              row=r + 1, column=0, columnspan=2, sticky="w")
                return r + 2
            return r + 1

        # ---- size distribution ----
        row = header("Size distribution", row)
        self.distVar = tk.StringVar(value="Log-normal")
        ttk.Label(left, text="Type:").grid(row=row, column=0, sticky="e")
        distBox = ttk.Combobox(left, textvariable=self.distVar, width=14,
                               state="readonly", values=DIST_NAMES)
        distBox.grid(row=row, column=1, sticky="w", padx=(4, 0))
        row += 1

        # Distribution-specific parameters live in their own frame, rebuilt
        # when the dropdown changes -- the same pattern oZgui.py already uses
        # for its potential parameters. Values entered for one distribution
        # are remembered if the user switches away and back.
        self.distParamFrame = ttk.Frame(left)
        self.distParamFrame.grid(row=row, column=0, columnspan=2, sticky="w")
        self.momentRuleVar = tk.BooleanVar(value=False)
        self.distVars = {}
        for name, params in DIST_PARAMS.items():
            for _lab, key, default, _tip in params:
                self.distVars.setdefault((name, key), tk.StringVar(value=default))
        row += 1

        self.nbinsVar = tk.StringVar(value="12")
        row = entry("Bins N:", self.nbinsVar, row)
        self.nsigVar = tk.StringVar(value="5.0")
        self.nsigRow = row
        row = entry("Truncate at:", self.nsigVar, row,
                    tip="+/- this many s.d. (unused for Beta: bounded support)")

        # ---- form factor ----
        row = header("Form factor", row)
        self.ffVar = tk.StringVar(value="Sphere")
        ttk.Label(left, text="Type:").grid(row=row, column=0, sticky="e")
        ffBox = ttk.Combobox(left, textvariable=self.ffVar, width=14,
                             state="readonly", values=["Sphere", "Core-shell"])
        ffBox.grid(row=row, column=1, sticky="w", padx=(4, 0))
        row += 1
        self.coreFracVar = tk.StringVar(value="0.6")
        ttk.Label(left, text="Core/outer ratio:").grid(row=row, column=0, sticky="e")
        self.coreFracEntry = ttk.Entry(left, textvariable=self.coreFracVar, width=10)
        self.coreFracEntry.grid(row=row, column=1, sticky="w", padx=(4, 0))
        row += 1
        self.rhoCoreVar = tk.StringVar(value="2.0")
        ttk.Label(left, text="SLD core / shell:").grid(row=row, column=0, sticky="e")
        sldFrame = ttk.Frame(left)
        sldFrame.grid(row=row, column=1, sticky="w", padx=(4, 0))
        self.rhoCoreEntry = ttk.Entry(sldFrame, textvariable=self.rhoCoreVar, width=5)
        self.rhoCoreEntry.pack(side="left")
        self.rhoShellVar = tk.StringVar(value="1.0")
        self.rhoShellEntry = ttk.Entry(sldFrame, textvariable=self.rhoShellVar, width=5)
        self.rhoShellEntry.pack(side="left", padx=(3, 0))
        row += 1

        # ---- interaction ----
        row = header("Hard-sphere-Yukawa interaction", row)
        self.phiVar = tk.StringVar(value="0.15")
        row = entry("Volume fraction:", self.phiVar, row)
        self.zVar = tk.StringVar(value="0.06")
        row = entry("Screening z:", self.zVar, row, tip="inverse length; z*sigma is the usual Z")
        self.KVar = tk.StringVar(value="1.0")
        row = entry("Contact K:", self.KVar, row,
                    tip="MSA sets c = -beta U:  K > 0 attractive,  K < 0 repulsive")

        self.closureVar = tk.StringVar(value="MSA")
        ttk.Label(left, text="Closure:").grid(row=row, column=0, sticky="e")
        ttk.Combobox(left, textvariable=self.closureVar, width=14, state="readonly",
                     values=["MSA", "RMSA (rescaled)"]).grid(
                         row=row, column=1, sticky="w", padx=(4, 0))
        row += 1

        # There is no fixed-point iteration here to accelerate -- see the
        # module docstring. What there IS is two independent closed-form
        # routes to the same solution, which is worth exposing because
        # running both is a live self-check.
        self.routeVar = tk.StringVar(value=ROUTES[0])
        ttk.Label(left, text="Solve via:").grid(row=row, column=0, sticky="e")
        ttk.Combobox(left, textvariable=self.routeVar, width=14, state="readonly",
                     values=ROUTES).grid(row=row, column=1, sticky="w", padx=(4, 0))
        row += 1
        ttk.Label(left, text="two closed-form routes to the same\n"
                            "solution; 'both' cross-checks them",
                  foreground="#666", font=("TkDefaultFont", 7),
                  justify="left").grid(row=row, column=0, columnspan=2, sticky="w")
        row += 1

        # ---- Q range ----
        row = header("Q range", row)
        self.QminVar = tk.StringVar(value="1e-3")
        row = entry("Q min:", self.QminVar, row)
        self.QmaxVar = tk.StringVar(value="0.3")
        row = entry("Q max:", self.QmaxVar, row)
        self.nQVar = tk.StringVar(value="200")
        row = entry("Points:", self.nQVar, row)

        # ---- approximations to overlay ----
        row = header("Compare against", row)
        self.approxVars = {}
        for label, attr in APPROX_SCHEMES:
            v = tk.BooleanVar(value=(attr == "I_decoupling"))
            ttk.Checkbutton(left, text=label, variable=v).grid(
                row=row, column=0, columnspan=2, sticky="w")
            self.approxVars[label] = (v, attr)
            row += 1
        ttk.Label(left, text="schemes 2-5 each solve O(N^2) auxiliary\n"
                             "monodisperse systems -- ~10 s apiece at N=12",
                  foreground="#666", font=("TkDefaultFont", 7),
                  justify="left").grid(row=row, column=0, columnspan=2, sticky="w")
        row += 1

        # ---- actions (in the always-visible footer) ----
        footer = self._controlFooter
        btns = ttk.Frame(footer)
        btns.pack(side="top", fill="x")
        self.computeButton = ttk.Button(btns, text="Compute", command=self._onCompute)
        self.computeButton.pack(side="left")
        ttk.Button(btns, text="Clear", command=self._onClear).pack(side="left", padx=(4, 0))
        ttk.Button(btns, text="Export...", command=self._onExport).pack(
            side="left", padx=(4, 0))

        self.statusVar = tk.StringVar(value="ready")
        ttk.Label(footer, textvariable=self.statusVar, foreground="blue",
                  wraplength=240, justify="left").pack(side="top", anchor="w",
                                                       pady=(4, 0))

        for w in (distBox, ffBox):
            w.bind("<<ComboboxSelected>>", lambda *_a: self._syncEnabledStates())
        self._rebuildDistParams()

    def _rebuildDistParams(self):
        """Show only the parameters the selected distribution actually takes."""
        for child in self.distParamFrame.winfo_children():
            child.destroy()
        name = self.distVar.get()
        r = 0
        for label, key, _default, tip in DIST_PARAMS.get(name, []):
            ttk.Label(self.distParamFrame, text=label + ":").grid(
                row=r, column=0, sticky="e")
            ttk.Entry(self.distParamFrame, textvariable=self.distVars[(name, key)],
                      width=10).grid(row=r, column=1, sticky="w", padx=(4, 0))
            r += 1
            if tip:
                ttk.Label(self.distParamFrame, text=tip, foreground="#666",
                          font=("TkDefaultFont", 7)).grid(
                              row=r, column=0, columnspan=2, sticky="w")
                r += 1
        if name == "Log-normal":
            # The log-normal is the one distribution with no closed-form
            # moment-exact Gaussian rule, so its default nodes reach only
            # ~1e-5 on <R^6> where Gamma and Beta reach 1e-16. This switches
            # to a Golub-Welsch rule built from the analytic moments.
            ttk.Checkbutton(self.distParamFrame,
                            text="moment-exact nodes",
                            variable=self.momentRuleVar).grid(
                                row=r, column=0, columnspan=2, sticky="w")
            r += 1
            ttk.Label(self.distParamFrame,
                      text="exact <R^n> to 1e-16 instead of ~1e-5\n"
                           "(needs mpmath; ignores 'Truncate at')",
                      foreground="#666", font=("TkDefaultFont", 7),
                      justify="left").grid(row=r, column=0, columnspan=2,
                                           sticky="w")
            r += 1

    def _syncEnabledStates(self):
        self._rebuildDistParams()
        state = "normal" if self.ffVar.get() == "Core-shell" else "disabled"
        for w in (self.coreFracEntry, self.rhoCoreEntry, self.rhoShellEntry):
            w.configure(state=state)

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
            self._decorateAxes(ax, key)
            canvas = FigureCanvasTkAgg(fig, master=frame)
            canvas.get_tk_widget().pack(fill="both", expand=True)
            NavigationToolbar2Tk(canvas, frame).update()
            self.axes[key] = ax
            self.canvases[key] = canvas

        # a text tab for the contact matrix and the run summary
        textFrame = ttk.Frame(self.notebook)
        self.notebook.add(textFrame, text="Summary")
        self.summaryText = tk.Text(textFrame, wrap="none", height=10,
                                   font=("TkFixedFont", 9))
        yscroll = ttk.Scrollbar(textFrame, orient="vertical",
                                command=self.summaryText.yview)
        self.summaryText.configure(yscrollcommand=yscroll.set)
        yscroll.pack(side="right", fill="y")
        self.summaryText.pack(side="left", fill="both", expand=True)

    @staticmethod
    def _decorateAxes(ax, key):
        if key == "iq":
            ax.set_xlabel("Q"); ax.set_ylabel("I(Q)")
            ax.set_xscale("log"); ax.set_yscale("log")
        elif key == "err":
            ax.set_xlabel("Q"); ax.set_ylabel(r"$|I_{approx}/I_{exact}-1|$")
            ax.set_xscale("log"); ax.set_yscale("log")
        elif key == "sij":
            ax.set_xlabel("Q"); ax.set_ylabel(r"$S_{ij}^{AL}(Q)$")
            ax.set_xscale("log")
        elif key == "sq":
            ax.set_xlabel("Q"); ax.set_ylabel("S(Q)  (number-number)")
            ax.set_xscale("log")
        elif key == "dist":
            ax.set_xlabel("R"); ax.set_ylabel("weight")

    # ------------------------------------------------------------------
    def _readInputs(self):
        """Parse and sanity-check the control panel. Raises ValueError with a
        message meant to be shown to the user."""
        def f(var, name, lo=None, hi=None):
            try:
                v = float(var.get())
            except ValueError:
                raise ValueError(f"{name}: '{var.get()}' is not a number")
            if lo is not None and v < lo:
                raise ValueError(f"{name} must be >= {lo}")
            if hi is not None and v > hi:
                raise ValueError(f"{name} must be <= {hi}")
            return v

        p = {}
        p["dist"] = name = self.distVar.get()
        # distribution parameters, with per-parameter bounds
        limits = {"R0": (1e-12, None), "relsd": (1e-6, 1.0),
                  "k": (1e-6, None), "theta": (1e-12, None),
                  "Rmin": (None, None), "Rmax": (None, None),
                  "a": (1e-6, None), "b": (1e-6, None)}
        p["distpars"] = {}
        for label, key, _default, _tip in DIST_PARAMS[name]:
            lo, hi = limits.get(key, (None, None))
            p["distpars"][key] = f(self.distVars[(name, key)], label, lo=lo, hi=hi)
        if name == "Beta":
            dp = p["distpars"]
            if dp["Rmin"] <= 0:
                raise ValueError("Beta: R min must be greater than 0")
            # The class enforces this too, but catching it here turns it into
            # an inline message on the control panel rather than a traceback
            # arriving from the worker thread.
            if dp["Rmax"] <= dp["Rmin"]:
                raise ValueError("Beta: R max must be greater than R min")
        p["momentRule"] = bool(self.momentRuleVar.get())
        p["nbins"] = int(f(self.nbinsVar, "Bins N", lo=1, hi=64))
        p["nsig"] = f(self.nsigVar, "Truncate at", lo=0.5, hi=10.0)
        p["phi"] = f(self.phiVar, "Volume fraction", lo=1e-6, hi=0.62)
        p["z"] = f(self.zVar, "Screening z", lo=1e-12)
        p["K"] = f(self.KVar, "Contact K")
        p["Qmin"] = f(self.QminVar, "Q min", lo=1e-12)
        p["Qmax"] = f(self.QmaxVar, "Q max", lo=1e-12)
        p["nQ"] = int(f(self.nQVar, "Points", lo=2, hi=5000))
        if p["Qmax"] <= p["Qmin"]:
            raise ValueError("Q max must exceed Q min")
        p["ff"] = self.ffVar.get()
        p["coreFrac"] = f(self.coreFracVar, "Core/outer ratio", lo=1e-3, hi=0.999)
        p["rhoCore"] = f(self.rhoCoreVar, "SLD core")
        p["rhoShell"] = f(self.rhoShellVar, "SLD shell")
        p["closure"] = self.closureVar.get()
        p["route"] = self.routeVar.get()
        p["approx"] = [(lab, attr) for lab, (v, attr) in self.approxVars.items()
                       if v.get()]
        return p

    def _onCompute(self):
        if self.worker is not None and self.worker.is_alive():
            self.statusVar.set("a calculation is already running")
            return
        try:
            p = self._readInputs()
        except ValueError as exc:
            self.statusVar.set(str(exc))
            messagebox.showerror("Invalid input", str(exc), parent=self)
            return

        self.computeButton.configure(state="disabled")
        self.statusVar.set("solving...")
        self.worker = threading.Thread(target=self._computeWorker, args=(p,),
                                       daemon=True)
        self.worker.start()
        if not self._polling:
            self._polling = True
            self.after(100, self._pollResultQueue)

    # ------------------------------------------------------------------
    def _computeWorker(self, p):
        """Runs OFF the main thread. Must not touch a single Tk widget --
        everything goes back through self.resultQueue."""
        def progress(msg):
            self.resultQueue.put(("status", msg))

        try:
            res = _Result()
            dist = _makeDistribution(p["dist"], p["distpars"],
                                     momentRule=p.get("momentRule", False))

            if p["ff"] == "Core-shell":
                ff = CoreShell(rho_core=p["rhoCore"], rho_shell=p["rhoShell"],
                               rho_solvent=0.0, ratio=p["coreFrac"])
            else:
                ff = Sphere()

            routeDiff = None
            rmsa = p["closure"].startswith("RMSA")
            # For RMSA, construct at K = 0 -- always solvable -- and put the
            # true K back afterwards, so that a state with no MSA root (which
            # is precisely when RMSA is wanted) can still be reached.
            sas = PolydisperseYukawaSAS(dist, p["phi"], p["z"],
                                        0.0 if rmsa else p["K"],
                                        formfactor=ff, nbins=p["nbins"],
                                        nsig=p["nsig"])
            if rmsa:
                sas.K = p["K"]
                sas.__dict__.pop("_mono_cache", None)

            route = p.get("route", ROUTE_GAMMA)
            if not rmsa and route != ROUTE_GAMMA:
                # solve() is singular at exactly K = 0; solve_gamma() takes
                # that branch analytically, so fall back rather than fail.
                if p["K"] == 0.0:
                    progress("K = 0: amplitude route is singular, using Gamma")
                else:
                    progress(f"re-solving via the {route} route...")
                    solAmp = sas.solver.solve()
                    if route == ROUTE_AMPL:
                        sas.sol = solAmp
                        sas.sq = PolydisperseOneYukawaSq(sas.solver, solAmp)
                    else:
                        gA = sas.solver.contact_value(solAmp)
                        gG = sas.solver.contact_value(sas.sol)
                        scale = max(float(np.max(np.abs(gG))), 1e-300)
                        routeDiff = float(np.max(np.abs(gA - gG)))/scale

            lines, notes = [], []
            if rmsa:
                rm = _apply_rmsa(sas, p["K"])
                if rm.get("lam") is None:
                    raise RuntimeError("RMSA: " + rm.get("status", "no solution"))
                lines.append(f"RMSA: lambda = {rm['lam']:.6f}   "
                             f"phi {rm['phi']:.4f} -> {rm['phi_rescaled']:.4f}")
                lines.append(f"      min g_ij = {rm['min_g']:.3e}   ({rm['status']})")
                res.label = f"RMSA lambda={rm['lam']:.4f}"
            else:
                res.label = "MSA"

            Q = np.logspace(np.log10(p["Qmin"]), np.log10(p["Qmax"]), p["nQ"])
            res.Q = Q
            progress("exact I(Q) and S_ij(Q)...")
            res.I_exact = sas.I_exact(Q)
            res.S = sas.S_partials(Q)
            res.S_number = sas.S_number(Q)
            res.R, res.w, res.sigma = sas.R, sas.w, sas.sigma
            res.g = sas.solver.contact_value(sas.sol)

            for n, (label, attr) in enumerate(p["approx"], 1):
                progress(f"approximation {n}/{len(p['approx'])}: {label.strip()}...")
                try:
                    res.approx[label] = getattr(sas, attr)(Q)
                except Exception as exc:
                    notes.append(f"{label}:\n    unavailable -- {exc}")

            # summary text
            lines.append("")
            if routeDiff is not None:
                lines.append(f"route cross-check   : {routeDiff:.2e}"
                             "   (eq.55 vs eq.72, relative)")
            lines.append(f"distribution        : {dist!r}")
            lines.append(f"components N        : {len(res.R)}")
            lines.append(f"sigma range         : {res.sigma.min():.4g} .. {res.sigma.max():.4g}")
            lines.append(f"z*sigma range       : {p['z']*res.sigma.min():.3f} .. "
                         f"{p['z']*res.sigma.max():.3f}")
            if isinstance(sas.sol, dict) and "gamma" in sas.sol:
                lines.append(f"Gamma               : {float(sas.sol['gamma']):.8g}")
            gmin, gmax = float(np.min(res.g)), float(np.max(res.g))
            # The RMSA rescaling condition drives min g_ij to zero, so it lands
            # at +/- a few 1e-15. Printing that as "-0.000000" and calling it
            # unphysical would be wrong -- tolerance it against the scale of g.
            gtol = 1e-9*max(1.0, abs(gmax))
            gminShown = 0.0 if abs(gmin) < gtol else gmin
            lines.append(f"contact g_ij        : {gminShown:.6f} .. {gmax:.6f}")
            asym = float(np.max(np.abs(res.g - res.g.T)))
            lines.append(f"|g - g^T|           : {asym:.2e}   (symmetry check)")
            if gmin < -gtol:
                lines.append("")
                lines.append("WARNING: g_ij(sigma+) < 0 -- this MSA solution is")
                lines.append("unphysical. Switch the closure to RMSA.")
            lines.append("")
            try:
                mr = moment_report(dist, res.R, res.w)
                lines.append("moment    discretised        analytic      rel.err")
                for n in sorted(mr):
                    num, ex, rel = mr[n]
                    lines.append(f"  <R^{n}>  {num:16.8g} {ex:16.8g} {rel:10.2e}")
            except Exception as exc:
                lines.append(f"(moment report unavailable: {exc})")
            if notes:
                lines.append("")
                lines.append("Approximations that have no solution in this state")
                lines.append("(a property of the scheme, not of the solver):")
                lines.extend(notes)
            res.summary = "\n".join(lines)

            self.resultQueue.put(("ok", res))
        except Exception as exc:
            self.resultQueue.put(("err", _explain(exc) + "\n\n"
                                  + "-"*60 + "\n" + traceback.format_exc()))

    def _pollResultQueue(self):
        try:
            while True:
                kind, payload = self.resultQueue.get_nowait()
                if kind == "status":
                    self.statusVar.set(payload)
                    continue
                if kind == "ok":
                    self.result = payload
                    self._replot()
                    self.statusVar.set(f"done  ({payload.label})")
                else:
                    self.statusVar.set("failed -- see Summary tab")
                    self.summaryText.delete("1.0", "end")
                    self.summaryText.insert("end", payload)
                    self.notebook.select(len(PLOT_TABS))
                self.computeButton.configure(state="normal")
        except queue.Empty:
            pass
        self.after(100, self._pollResultQueue)

    # ------------------------------------------------------------------
    def _onClear(self):
        self.result = None
        for key, ax in self.axes.items():
            ax.clear()
            self._decorateAxes(ax, key)
            self.canvases[key].draw_idle()
        self.summaryText.delete("1.0", "end")
        self.statusVar.set("cleared")

    def _replot(self):
        r = self.result
        if r is None:
            return
        for key, ax in self.axes.items():
            ax.clear()
            self._decorateAxes(ax, key)

        ax = self.axes["iq"]
        ax.plot(r.Q, r.I_exact, "k-", lw=2, label=f"exact ({r.label})")
        for label, I in r.approx.items():
            ax.plot(r.Q, np.abs(I), lw=1, label=label)
        ax.legend(fontsize=7)

        ax = self.axes["err"]
        pos = r.I_exact > 0
        for label, I in r.approx.items():
            with np.errstate(divide="ignore", invalid="ignore"):
                e = np.abs(I/np.where(pos, r.I_exact, np.nan) - 1.0)
            ax.plot(r.Q, e, lw=1, label=label)
        if r.approx:
            ax.legend(fontsize=7)
        else:
            ax.text(0.5, 0.5, "no schemes selected", ha="center",
                    transform=ax.transAxes, color="#888")

        ax = self.axes["sij"]
        N = r.S.shape[1]
        # N can be 12+, so N^2 curves would be unreadable -- show the diagonal
        # plus the extreme off-diagonal, which is what actually differs.
        for i in (0, N//2, N - 1):
            ax.plot(r.Q, r.S[:, i, i], lw=1.2, label=f"S_{i}{i}")
        if N > 1:
            ax.plot(r.Q, r.S[:, 0, N - 1], "--", lw=1.2, label=f"S_0{N-1}")
        ax.axhline(1.0, color="#bbb", lw=0.6)
        ax.legend(fontsize=7)

        ax = self.axes["sq"]
        ax.plot(r.Q, r.S_number, "k-", lw=1.5)
        ax.axhline(1.0, color="#bbb", lw=0.6)

        ax = self.axes["dist"]
        ax.bar(r.R, r.w, width=np.gradient(r.R)*0.8, color="#4477aa")
        ax.set_title(f"{len(r.R)} components", fontsize=9)

        for key in self.axes:
            self.canvases[key].draw_idle()

        self.summaryText.delete("1.0", "end")
        self.summaryText.insert("end", r.summary)

    # ------------------------------------------------------------------
    def _onExport(self):
        if self.result is None:
            self.statusVar.set("nothing to export -- press Compute first")
            return
        path = filedialog.asksaveasfilename(
            parent=self, defaultextension=".dat",
            filetypes=[("ASCII table", "*.dat *.txt"), ("All files", "*.*")],
            title="Export I(Q) and S(Q)")
        if not path:
            return
        r = self.result
        cols = [("Q", r.Q), ("I_exact", r.I_exact), ("S_number", r.S_number)]
        cols += [(lab.split()[0] + "_" + lab.split()[1] if len(lab.split()) > 1
                  else lab, I) for lab, I in r.approx.items()]
        try:
            with open(path, "w", encoding="utf-8") as fh:
                fh.write(f"# polydisperse hard-sphere-Yukawa, {r.label}\n")
                for line in r.summary.splitlines():
                    fh.write(f"# {line}\n")
                fh.write("# " + "\t".join(name for name, _ in cols) + "\n")
                for k in range(len(r.Q)):
                    fh.write("\t".join(f"{col[k]:.8e}" for _, col in cols) + "\n")
        except OSError as exc:
            messagebox.showerror("Export failed", str(exc), parent=self)
            return
        self.statusVar.set(f"exported {os.path.basename(path)}")


def main():                                                # pragma: no cover
    root = tk.Tk()
    root.title("Polydisperse hard-sphere-Yukawa (MSA / RMSA)")
    nb = ttk.Notebook(root)
    nb.pack(fill="both", expand=True)
    nb.add(PolydisperseYukawaTab(nb), text="Polydisperse Yukawa")
    root.geometry("1200x750")
    root.mainloop()


if __name__ == "__main__":                                 # pragma: no cover
    main()
