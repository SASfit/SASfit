# -*- coding: utf-8 -*-
"""
Rogers-Young (RY) polydisperse Yukawa structure factor notebook tab for oZgui.py.

This is a SELF-CONTAINED ttk.Frame subclass. It calculates exact structure factors
using the Rogers-Young closure approximation and compares them against various
polydispersity approximation schemes.

    from ry_polydisperse_yukawa_tab import RYPolydisperseYukawaTab
    tab = RYPolydisperseYukawaTab(someNotebook)
    someNotebook.add(tab, text="RY Polydisperse Yukawa")

It can also be run on its own for testing:

    python ry_polydisperse_yukawa_tab.py

The Rogers-Young closure is an alternative to MSA that can reach higher
concentrations and different interaction regimes.
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
from polydisperse_tab_controls import PolydisperseTabControls


def _solverChoices():
    """Fixpoint strategies offered in this tab's Solver dropdown.

    Only the ones that exist in this installation are listed. Order is the
    measured preference on this problem: SUNDIALS KIN_FP is fastest and, on
    hard states, fails cleanly instead of converging onto a spurious root;
    scipy Anderson converges where plain Picard diverges; Picard is the
    fallback of last resort but is the most robust at extreme dilution.
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
        choices["Picard"] = PicardOZsolver
    except Exception:
        pass
    return choices


IMPORT_ERROR = None
try:
    from polydisperse_yukawa_sas import (LogNormal, SchulzZimm, Gamma, Beta,
                                         Sphere, CoreShell, moment_report)
    from polydisperse_sas_base import PolydisperseSASBase
    from rypolydisperseWrapper import RYPolydisperseYukawa
    import rypolydisperseWrapper as rypw
except Exception as _exc:                                  # pragma: no cover
    IMPORT_ERROR = _exc


# Approximation schemes
APPROX_SCHEMES = [
    ("0  monodisperse",              "I_monodisperse"),
    ("1  decoupling (Kotlarchyk-Chen)", "I_decoupling"),
    ("2  local monodisperse (Pedersen)", "I_lma"),
    ("3  partial structure factors", "I_partial_sf"),
    ("4  scaling (Gazzillo)",        "I_scaling"),
    ("5  van der Waals one-fluid",   "I_vdw1"),
]

# Size distribution parameters
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

PLOT_TABS = [
    ("S(Q)",            "sq"),
    ("S_ij(Q)",         "sij"),
    ("Size distribution", "dist"),
]


class _Result:
    """Everything one Compute produces."""
    def __init__(self):
        self.Q = None
        self.S = None           # (nQ, N, N)
        self.S_number = None
        self.R = None
        self.w = None
        self.sigma = None
        # Rogers-Young mixing parameter, SOLVED for (not user input) by
        # requiring the compressibility and virial pressure routes to agree.
        self.alpha = None
        self.alpha_residual = None
        self.alpha_converged = False
        self.summary = ""
        self.label = ""


def _makeDistribution(name, pars):
    """Build the distribution object named by the dropdown."""
    if name == "Log-normal":
        s = float(np.sqrt(np.log(1.0 + pars["relsd"]**2)))
        return LogNormal(pars["R0"], s)
    if name == "Schulz-Zimm":
        return SchulzZimm(pars["R0"], relsd=pars["relsd"])
    if name == "Gamma":
        return Gamma(pars["k"], pars["theta"])
    if name == "Beta":
        return Beta(pars["Rmin"], pars["Rmax"], pars["a"], pars["b"])
    raise ValueError(f"unknown distribution '{name}'")


def _explain(exc):
    """Turn common failures into plain language."""
    msg = str(exc)
    if "library not found" in msg.lower():
        return ("The RY Polydisperse Yukawa library could not be loaded.\n\n"
                "Make sure the plugin is properly built and installed.")
    return f"{type(exc).__name__}: {msg}"


class RYPolydisperseYukawaTab(PolydisperseTabControls, ttk.Frame):
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
        msg = ("The RY Polydisperse Yukawa solver modules could not be imported,\n"
               "so this tab is inactive. The Ornstein-Zernike solver tab is\n"
               "unaffected.\n\n"
               f"{type(IMPORT_ERROR).__name__}: {IMPORT_ERROR}\n\n"
               "Expected next to oZgui.py:\n"
               "    polydisperse_yukawa_sas.py\n"
               "    polydisperse_sas_base.py\n"
               "    rypolydisperseWrapper.py")
        ttk.Label(self, text=msg, justify="left", padding=20,
                  foreground="#a00").pack(anchor="nw")

    # ------------------------------------------------------------------
    def _buildControls(self):
        left = ttk.Frame(self)
        left.grid(row=0, column=0, sticky="nsew")
        self.rowconfigure(0, weight=1)

        footer = ttk.Frame(left, padding=(6, 4))
        footer.pack(side="bottom", fill="x")
        ttk.Separator(left, orient="horizontal").pack(side="bottom", fill="x")

        scrollHost = ttk.Frame(left)
        scrollHost.pack(side="top", fill="both", expand=True)
        canvas = tk.Canvas(scrollHost, borderwidth=0, highlightthickness=0, width=250)
        vbar = ttk.Scrollbar(scrollHost, orient="vertical", command=canvas.yview)
        canvas.configure(yscrollcommand=vbar.set)
        vbar.pack(side="right", fill="y")
        canvas.pack(side="left", fill="both", expand=True)

        body = ttk.Frame(canvas, padding=6)
        window = canvas.create_window((0, 0), window=body, anchor="nw")

        def _onBodyConfigure(_e=None):
            canvas.configure(scrollregion=canvas.bbox("all"))
            canvas.configure(width=body.winfo_reqwidth())
        body.bind("<Configure>", _onBodyConfigure)
        canvas.bind("<Configure>", lambda e: canvas.itemconfigure(window, width=e.width))

        def _wheel(event):
            delta = event.delta
            if event.num == 4:
                delta = 120
            elif event.num == 5:
                delta = -120
            canvas.yview_scroll(int(-delta/120), "units")
        for seq in ("<MouseWheel>", "<Button-4>", "<Button-5>"):
            canvas.bind_all(seq, lambda e: _wheel(e)
                            if str(e.widget).startswith(str(body)) or
                            str(e.widget).startswith(str(canvas)) else None)

        self._controlBody, self._controlFooter = body, footer
        left = body
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

        self.distParamFrame = ttk.Frame(left)
        self.distParamFrame.grid(row=row, column=0, columnspan=2, sticky="w")
        self.distVars = {}
        for name, params in DIST_PARAMS.items():
            for _lab, key, default, _tip in params:
                self.distVars.setdefault((name, key), tk.StringVar(value=default))
        row += 1

        self.nbinsVar = tk.StringVar(value="12")
        row = entry("Bins N:", self.nbinsVar, row)

        # ---- form factor ----
        row = header("Form factor", row)
        self.ffVar = tk.StringVar(value="Sphere")
        ttk.Label(left, text="Type:").grid(row=row, column=0, sticky="e")
        ffBox = ttk.Combobox(left, textvariable=self.ffVar, width=14,
                             state="readonly", values=["Sphere", "Core-shell"])
        ffBox.grid(row=row, column=1, sticky="w", padx=(4, 0))
        row += 1

        # ---- interaction (RY specific) ----
        row = header("Yukawa interaction (RY)", row)
        self.phiVar = tk.StringVar(value="0.15")
        row = entry("Volume fraction:", self.phiVar, row)
        self.zVar = tk.StringVar(value="0.06")
        row = entry("Screening z:", self.zVar, row)
        self.KVar = tk.StringVar(value="1.0")
        row = entry("Contact K:", self.KVar, row)
        # No RY alpha entry: alpha is not a free parameter. Rogers-Young fixes
        # it by requiring the compressibility and virial routes to the pressure
        # to agree, so it is SOLVED for on every Compute (see _computeWorker's
        # call to rypolydisperseWrapper.solve_alpha) and reported in the
        # Summary tab. Exposing it as an input invited inconsistent results.
        self.alphaVar = None

        # ---- Q range ----
        row = header("Q range", row)
        self.QminVar = tk.StringVar(value="1e-3")
        row = entry("Q min:", self.QminVar, row)
        self.QmaxVar = tk.StringVar(value="0.3")
        row = entry("Q max:", self.QmaxVar, row)
        self.nQVar = tk.StringVar(value="200")
        row = entry("Points:", self.nQVar, row)

        # ---- actions ----
        # Standard control set shared with the other polydisperse tabs (see
        # polydisperse_tab_controls.py): Compute / Interrupt / Clear all /
        # Delete last, Save / Load, the export selector, the run-history list
        # and the status line.
        #
        # Unlike the analytic MSA/RMSA and Robertus tabs, this one drives a
        # genuine iterative Ornstein-Zernike solve, so BOTH extras apply:
        #   - supportsInterrupt=True: the fixpoint loops poll
        #     solverInstance.isInterrupted between iterations, exactly as
        #     oZgui.py's own interrupt button relies on.
        #   - a solver dropdown: rypolydisperseWrapper accepts solverClass,
        #     and which fixpoint strategy is used matters a great deal here.
        #     Measured on this problem, scipy Anderson converges where Picard
        #     diverges, and SUNDIALS KIN_FP is faster still AND refuses hard
        #     states rather than converging onto a spurious root -- so the
        #     choice is worth exposing rather than hard-coding.
        footer = self._controlFooter
        self.buildStandardControls(footer, supportsInterrupt=True,
                                    solverChoices=_solverChoices(),
                                    statusWraplength=240)
        self.computeButton = self.computeBtn   # name used elsewhere in this file

        distBox.bind("<<ComboboxSelected>>", lambda *_a: self._syncEnabledStates())
        self._rebuildDistParams()

    def _rebuildDistParams(self):
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

    def _syncEnabledStates(self):
        self._rebuildDistParams()

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
        if key == "sq":
            ax.set_xlabel("Q"); ax.set_ylabel("S(Q)  (number-number)")
            ax.set_xscale("log")
        elif key == "sij":
            ax.set_xlabel("Q"); ax.set_ylabel(r"$S_{ij}(Q)$")
            ax.set_xscale("log")
        elif key == "dist":
            ax.set_xlabel("R"); ax.set_ylabel("weight")

    # ------------------------------------------------------------------
    def _readInputs(self):
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
            if dp["Rmax"] <= dp["Rmin"]:
                raise ValueError("Beta: R max must be greater than R min")
        p["nbins"] = int(f(self.nbinsVar, "Bins N", lo=1, hi=64))
        p["phi"] = f(self.phiVar, "Volume fraction", lo=1e-6, hi=0.62)
        p["z"] = f(self.zVar, "Screening z", lo=1e-12)
        p["K"] = f(self.KVar, "Contact K")
        # p["alpha"] deliberately absent -- solved for, not read from the GUI.
        p["Qmin"] = f(self.QminVar, "Q min", lo=1e-12)
        p["Qmax"] = f(self.QmaxVar, "Q max", lo=1e-12)
        p["nQ"] = int(f(self.nQVar, "Points", lo=2, hi=5000))
        if p["Qmax"] <= p["Qmin"]:
            raise ValueError("Q max must exceed Q min")
        # Form factor. This was missing, so _computeWorker's own
        # `if p["ff"] == "Core-shell"` raised KeyError: 'ff' on every single
        # Compute -- self.ffVar was built in _buildControls and read in the
        # worker, but never carried across in between.
        p["ff"] = self.ffVar.get()
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
        self.worker = threading.Thread(target=self._computeWorker, args=(p,), daemon=True)
        self.worker.start()
        if not self._polling:
            self._polling = True
            self.after(100, self._pollResultQueue)

    def _computeWorker(self, p):
        def progress(msg):
            self.resultQueue.put(("status", msg))

        try:
            res = _Result()
            dist = _makeDistribution(p["dist"], p["distpars"])

            if p["ff"] == "Core-shell":
                ff = CoreShell()
            else:
                ff = Sphere()

            progress("Building RY distribution...")
            # Use log-normal discretization to create components
            mean_key = "R0" if "R0" in p["distpars"] else "D0"
            R0 = p["distpars"][mean_key]
            relsd = p["distpars"].get("relsd", 0.2)
            
            # Create size distribution for sigma (diameter)
            sigmas = np.linspace(R0 * (1 - 3*relsd), R0 * (1 + 3*relsd), p["nbins"])
            
            # Weight by log-normal distribution
            s = np.sqrt(np.log(1.0 + relsd**2))
            weights = np.exp(-0.5 * ((np.log(sigmas) - np.log(R0))/s)**2)
            weights /= np.sum(weights)
            
            # Number densities from volume fraction
            sigma3 = np.sum(weights * sigmas**3)
            x_array = weights * p["phi"] * 6 / (np.pi * sigma3)
            
            progress("Computing RY structure factor...")
            # rypolydisperseWrapper's signature deliberately mirrors
            # PolydisperseOneYukawaMSA's: (sigma, rho, z, K, alpha, delta).
            # x_array above is already NUMBER DENSITIES (weights scaled by
            # phi*6/(pi*<sigma^3>)), so the volume fraction is contained in
            # them and must NOT be passed separately -- doing so shifted every
            # argument by one and put arrays where scalars were expected,
            # which is what raised "TypeError: only 0-dimensional arrays can
            # be converted to Python scalars" on every Compute.
            #
            # alpha is NOT a user input. Rogers-Young's whole point is that the
            # mixing parameter is FIXED by thermodynamic consistency: the
            # compressibility and virial routes to the pressure are required to
            # agree. solve_alpha() does that search (three OZ solves per trial
            # value, hence the progress messages).
            progress("Solving for thermodynamically consistent alpha...")
            alpha, alpha_res = rypw.solve_alpha(sigmas, x_array, p["z"], p["K"],
                                                 bracket=(0.02, 3.0),
                                                 progress=progress)
            res.alpha = alpha
            res.alpha_residual = alpha_res
            # A large leftover residual means no alpha in the bracket makes the
            # two routes agree -- report it rather than quietly presenting the
            # nearest endpoint as if it were a solution.
            res.alpha_converged = abs(alpha_res) < 1e-3
            progress(f"alpha = {alpha:.4f}" if res.alpha_converged else
                     f"alpha = {alpha:.4f} (NOT consistent, residual {alpha_res:+.3g})")
            ry = rypw.RYPolydisperseYukawa(sigmas, x_array, p["z"], p["K"], alpha)
            
            Q = np.logspace(np.log10(p["Qmin"]), np.log10(p["Qmax"]), p["nQ"])
            res.Q = Q
            
            progress("Calculating S matrices...")
            S_list = []
            S_num_list = []
            for q in Q:
                S = ry.S_matrix(q)
                S_list.append(S)
                n_total = np.sum(x_array)
                w = x_array / n_total
                wv = np.sqrt(w)
                S_num = float(wv @ S @ wv)
                S_num_list.append(S_num)
            
            res.S = np.array(S_list)
            res.S_number = np.array(S_num_list)
            res.R = sigmas / 2.0
            res.w = weights
            res.sigma = sigmas
            
            lines = []
            lines.append(f"distribution        : {dist!r}")
            lines.append(f"components N        : {len(sigmas)}")
            lines.append(f"sigma range         : {sigmas.min():.4g} .. {sigmas.max():.4g}")
            lines.append(f"z*sigma range       : {p['z']*sigmas.min():.3f} .. {p['z']*sigmas.max():.3f}")
            lines.append(f"phi                 : {p['phi']:.4g}")
            lines.append(f"z                   : {p['z']:.4g}")
            lines.append(f"K                   : {p['K']:.4g}")
            lines.append(f"alpha (RY)          : {res.alpha:.4g}"
                         + ("  [thermodynamically consistent,"
                            f" residual {res.alpha_residual:+.2e}]"
                            if res.alpha_converged else
                            "  [NO consistent alpha found in [0.02, 3];"
                            f" best residual {res.alpha_residual:+.3g}]"))
            
            res.summary = "\n".join(lines)
            res.label = f"RY K={p['K']:.4f} alpha={res.alpha:.4f}"
            self.resultQueue.put(("result", res))
        except Exception as exc:
            self.resultQueue.put(("error", _explain(exc)))

    def _pollResultQueue(self):
        try:
            while True:
                kind, data = self.resultQueue.get_nowait()
                if kind == "status":
                    self.statusVar.set(data)
                elif kind == "result":
                    self._displayResult(data)
                elif kind == "error":
                    self.statusVar.set("error")
                    messagebox.showerror("Computation failed", str(data), parent=self)
        except queue.Empty:
            pass
        finally:
            if self.worker is None or not self.worker.is_alive():
                self._polling = False
                self.computeButton.configure(state="normal")
            else:
                self.after(100, self._pollResultQueue)

    def _displayResult(self, res):
        # registerRun() (from PolydisperseTabControls) appends to the run
        # history and sets self.result, so Clear all / Delete last / Save /
        # Export all operate on it.
        self.registerRun(res, res.label)
        self._plotSQ()
        self._plotSij()
        self._plotDist()
        self.summaryText.configure(state="normal")
        self.summaryText.delete("1.0", "end")
        self.summaryText.insert("1.0", res.summary)
        self.summaryText.configure(state="disabled")
        self.statusVar.set(res.label)

    def _plotSQ(self):
        ax = self.axes["sq"]
        ax.clear()
        self._decorateAxes(ax, "sq")
        if self.result is None or self.result.S_number is None:
            return
        res = self.result
        ax.semilogx(res.Q, res.S_number, "k-", linewidth=2)
        ax.grid(True, alpha=0.3)
        self.canvases["sq"].draw()

    def _plotSij(self):
        ax = self.axes["sij"]
        ax.clear()
        self._decorateAxes(ax, "sij")
        if self.result is None or self.result.S is None:
            return
        res = self.result
        S = res.S
        nij = S.shape[1]
        colors = plt.cm.viridis(np.linspace(0, 1, nij * (nij + 1) // 2))
        idx = 0
        for i in range(nij):
            for j in range(i, nij):
                ax.semilogx(res.Q, S[:, i, j], color=colors[idx], alpha=0.7,
                           label=f"S_{i}{j}" if nij <= 3 else None)
                idx += 1
        if nij <= 3:
            ax.legend(loc="best", fontsize=8)
        ax.grid(True, alpha=0.3)
        self.canvases["sij"].draw()

    def _plotDist(self):
        ax = self.axes["dist"]
        ax.clear()
        self._decorateAxes(ax, "dist")
        if self.result is None:
            return
        res = self.result
        ax.bar(res.sigma, res.w, width=0.7*np.diff(res.sigma).mean(), alpha=0.7, edgecolor="black")
        ax.grid(True, alpha=0.3, axis="y")
        self.canvases["dist"].draw()

    def _onClear(self):
        self.result = None
        for ax in self.axes.values():
            ax.clear()
        for canvas in self.canvases.values():
            canvas.draw()
        self.summaryText.configure(state="normal")
        self.summaryText.delete("1.0", "end")
        self.summaryText.configure(state="disabled")
        self.statusVar.set("cleared")

    def _onExport(self):
        if self.result is None:
            messagebox.showwarning("No result", "No result to export.", parent=self)
            return
        fd = filedialog.asksaveasfile(mode="w", defaultextension=".csv",
                                      filetypes=[("CSV", "*.csv"), ("Excel", "*.xlsx"),
                                                ("All files", "*")], parent=self)
        if fd is None:
            return
        try:
            res = self.result
            if fd.name.endswith(".xlsx"):
                try:
                    import openpyxl
                    wb = openpyxl.Workbook()
                    ws = wb.active
                    ws.append(["Q", "S_number"])
                    for q, s in zip(res.Q, res.S_number):
                        ws.append([q, s])
                    wb.save(fd.name)
                except ImportError:
                    messagebox.showerror("Export failed",
                                       "openpyxl not installed", parent=self)
                    return
            else:
                fd.write("Q,S_number\n")
                for q, s in zip(res.Q, res.S_number):
                    fd.write(f"{q},{s}\n")
            fd.close()
            self.statusVar.set(f"exported to {os.path.basename(fd.name)}")
        except Exception as exc:
            messagebox.showerror("Export failed", str(exc), parent=self)


try:
    import matplotlib.pyplot as plt
except ImportError:
    plt = None


if __name__ == "__main__":
    root = tk.Tk()
    root.title("RY Polydisperse Yukawa Tab Test")
    tab = RYPolydisperseYukawaTab(root)
    tab.pack(fill="both", expand=True)
    root.mainloop()
