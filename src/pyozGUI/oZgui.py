# -*- coding: utf-8 -*-
'''
Python equivalent of sasfit.vfs/lib/app-sasfit/tcl/sasfit_OZ_solver.tcl
(SASfit's Tcl/Tk "Ornstein Zernike solver" GUI).

Reuses this directory's own OZ solver classes directly (no ctypes, no
Tcl, no RPC round-trip) -- run this file directly:

    python oZgui.py

Requirements: numpy, scipy, matplotlib (all already needed by the
solver classes themselves); tkinter (Python's standard GUI toolkit,
bundled with most Python installs -- on Debian/Ubuntu systems that
don't have it, `apt install python3-tk`); sundials4py only if you
pick the "sundials4py KINSOL" solver from the dropdown (pip install
sundials4py -- https://pypi.org/project/sundials4py/); openpyxl only
if you use the Excel export button (pip install openpyxl).

Mapped from the original Tcl GUI's layout:
  - left panel:  potential + closure dropdowns, dynamically-shown
                 parameter entries (matches the original's
                 oz_input_names/p0..p6 pattern, but built here via
                 the same introspection getAvailablePotentialNames()/
                 printPotentialSetterArguments() already use --
                 nothing to keep in sync by hand when a new potential
                 is added to oZfixpointOperator.py), volume fraction,
                 solver/algorithm choice, calculate/interrupt/clear
                 buttons, Save/Load (all runs, as a self-describing
                 .oz JSON file) and ASCII/CSV/Excel/PNG export of the
                 selected/last run, and a run history list for
                 overlaying multiple solved curves.
  - right panel: a tabbed set of plots -- S(Q), g(r), c(r), Gamma(r),
                 h(r), interaction potential U(r)/kT, bridge function
                 B(r), cavity function y(r), Mayer-f function f(r) --
                 matching the original's 9 BLT graph tabs one for one.
Not carried over from the Tcl GUI (out of scope for this equivalent):
  - the "assign to SQ plugin" mechanism (ties into SASfit's own
    fitting engine specifically, not meaningful standalone here) --
    replaced by the Save/Load/export buttons described above instead
  - the detailed KINSOL tuning sub-dialog (KINSetMAA, KINSetEtaForm,
    etc.) -- the solver classes here mostly use each library's own
    sensible defaults; self.numberOfIterations/self.convergenceCriterion
    are exposed instead, covering the same role at a simpler level
  - crosshair coordinate readout / clipboard export / zoom-stack
    (matplotlib's own toolbar below each plot covers zoom/pan/save)

The actual calculation workflow (SOLVER_CLASSES/CLOSURE_SETTERS and
what "calculate" does) now lives in ozLib.py, imported here rather
than duplicated -- so this GUI and ozLib.solve() called directly from
your own scripts always give identical results.
'''
import threading
import queue
import traceback
import json

import numpy as np
import tkinter as tk
from tkinter import ttk, messagebox, filedialog

import matplotlib
matplotlib.use("TkAgg")
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2Tk

from picardOZsolver import PicardOZsolver  # kept directly: used below only for the
                                            # potential-introspection "probe" instance,
                                            # not part of the solver dropdown machinery.
import ozLib
from ozLib import SOLVER_CLASSES, CLOSURE_SETTERS
_HAVE_SUNDIALS4PY = ozLib._HAVE_SUNDIALS4PY

# Curves shown, one tab each: (tab label, y-attribute-getter, ylabel)
# x is r (or q for S(Q)) throughout, taken from solver.getrArray() /
# the q-array the structure factor is naturally computed on. Keys
# match ozLib.CURVE_NAMES/OZResult.curves own keys exactly.
CURVE_TABS = [
    ("S(Q)",       "Sq",     "S(Q)"),
    ("g(r)",       "gr",     "g(r)"),
    ("c(r)",       "cr",     "c(r)"),
    ("Gamma(r)",   "gamma",  "\u0393(r)"),
    ("h(r)",       "hr",     "h(r)"),
    ("U(r)/kT",    "Ur",     "U(r) / kT"),
    ("B(r)",       "Br",     "B(r)"),
    ("y(r)",       "yr",     "y(r)"),
    ("f(r)",       "fr",     "f(r)"),
]


class RunResult:
    '''One computed/solved curve set, kept in the run history list --
    matches the original Tcl GUI's OZ(result,...) list-of-lists.'''
    def __init__(self, label, r, q, potential, closure, phi):
        self.label = label
        self.r = r
        self.q = q
        self.potential = potential
        self.closure = closure
        self.phi = phi
        self.curves = {}   # name -> y array, keys matching CURVE_TABS[*][1]


class OZgui:
    def __init__(self, root):
        self.root = root
        root.title("Ornstein-Zernike solver (Python)")

        self.runs = []              # list of RunResult, oldest first
        self.colors = plt_colors()
        self.solver = None          # currently-running OZsolver instance
        self.worker = None
        # Worker thread -> main thread communication goes through this
        # queue, polled from the main thread via root.after() below --
        # NOT via the worker thread calling root.after() itself. Tk is
        # not thread-safe in general; every Tk/Tkinter call must
        # originate from the thread that owns the interpreter. Found by
        # testing: calling self.root.after(...) directly from the
        # worker thread raised "RuntimeError: main thread is not in
        # main loop" (intermittently, depending on exactly how the
        # event loop is being driven) -- this queue+poll pattern is the
        # standard, reliably thread-safe way to hand a result back.
        self.resultQueue = queue.Queue()
        self.root.after(100, self._pollResultQueue)

        self._buildLeftPanel()
        self._buildRightPanel()

    # ------------------------------------------------------------------
    def _buildLeftPanel(self):
        left = ttk.Frame(self.root, padding=6)
        left.grid(row=0, column=0, sticky="ns")

        row = 0
        ttk.Label(left, text="Potential:").grid(row=row, column=0, sticky="e")
        # Potential names discovered via the same introspection
        # mechanism the solver classes already use for
        # printPotentialSetterArguments() -- add a new setXXXPotential()
        # method to oZfixpointOperator.py and it appears here
        # automatically, with no other code to touch.
        probe = PicardOZsolver(port=0)
        self.potentialNames = sorted(probe.getAvailablePotentialNames())
        self.potentialVar = tk.StringVar(value="HardSphere")
        self.potentialCombo = ttk.Combobox(left, textvariable=self.potentialVar,
                                            values=self.potentialNames, state="readonly", width=22)
        self.potentialCombo.grid(row=row, column=1, sticky="w")
        self.potentialCombo.bind("<<ComboboxSelected>>", lambda e: self._rebuildPotentialParams(probe))
        row += 1

        self.potParamsFrame = ttk.Frame(left)
        self.potParamsFrame.grid(row=row, column=0, columnspan=2, sticky="w", pady=(2, 8))
        row += 1
        self.potParamVars = []
        self._rebuildPotentialParams(probe)

        ttk.Label(left, text="Closure:").grid(row=row, column=0, sticky="e")
        self.closureVar = tk.StringVar(value="Percus-Yevick")
        closureCombo = ttk.Combobox(left, textvariable=self.closureVar,
                                     values=list(CLOSURE_SETTERS.keys()), state="readonly", width=22)
        closureCombo.grid(row=row, column=1, sticky="w")
        closureCombo.bind("<<ComboboxSelected>>", lambda e: self._rebuildClosureParam())
        row += 1

        self.closureParamFrame = ttk.Frame(left)
        self.closureParamFrame.grid(row=row, column=0, columnspan=2, sticky="w", pady=(2, 8))
        row += 1
        self.closureParamVar = None
        self._rebuildClosureParam()

        ttk.Label(left, text="Volume fraction \u03c6:").grid(row=row, column=0, sticky="e")
        self.phiVar = tk.StringVar(value="0.3")
        ttk.Entry(left, textvariable=self.phiVar, width=10).grid(row=row, column=1, sticky="w")
        row += 1

        ttk.Label(left, text="Solver:").grid(row=row, column=0, sticky="e")
        self.solverVar = tk.StringVar(value=next(iter(SOLVER_CLASSES)))
        ttk.Combobox(left, textvariable=self.solverVar, values=list(SOLVER_CLASSES.keys()),
                     state="readonly", width=22).grid(row=row, column=1, sticky="w")
        row += 1

        ttk.Label(left, text="X-axis scale:").grid(row=row, column=0, sticky="e")
        self.xScaleVar = tk.StringVar(value="symlog")
        xScaleCombo = ttk.Combobox(left, textvariable=self.xScaleVar, values=["symlog", "linear", "log", "asinh"],
                                    state="readonly", width=22)
        xScaleCombo.grid(row=row, column=1, sticky="w")
        # r/q start at a small but nonzero value (Delta_r/Delta_q, not
        # 0 -- see getrArray()/getqArray() in oZfixpointOperator.py),
        # so plain "log" would already work without a linthresh trick;
        # symlog as the default is still worth having since it keeps
        # the near-origin structure (contact region, low-q limit)
        # readable on a genuinely linear footing while still
        # compressing the long decay at large r/q into view, rather
        # than requiring the person to pick one or the other.
        xScaleCombo.bind("<<ComboboxSelected>>", lambda e: self._replot())
        row += 1

        ttk.Label(left, text="Symlog/asinh threshold:").grid(row=row, column=0, sticky="e")
        self.xThresholdVar = tk.StringVar(value="auto")
        xThresholdEntry = ttk.Entry(left, textvariable=self.xThresholdVar, width=10)
        xThresholdEntry.grid(row=row, column=1, sticky="w")
        # "auto" (the default) keeps the original behaviour: linthresh/
        # linear_width set from the actual smallest positive r or q
        # value currently plotted, which keeps that finest near-
        # contact/near-zero-q structure maximally resolved in the
        # CURVE shape -- but found by testing that this also makes the
        # tick marks themselves collapse to look just like plain log
        # scale (matplotlib's own SymmetricalLogLocator/AsinhLocator
        # only place a visibly distinct "linear-region" tick, e.g. a
        # tick at exactly 0 for symlog, once the threshold is a
        # non-negligible fraction of the plotted range) -- typing a
        # specific number here (e.g. 1 or 2) trades a little of that
        # fine near-origin curve resolution for a visibly distinct
        # linear region and tick behaviour instead; entering "auto"
        # (or anything that doesn't parse as a number) reverts to the
        # data-derived default.
        xThresholdEntry.bind("<Return>", lambda e: self._replot())
        xThresholdEntry.bind("<FocusOut>", lambda e: self._replot())
        row += 1

        ttk.Label(left, text="Grid points (N):").grid(row=row, column=0, sticky="e")
        self.gridPointsVar = tk.StringVar(value="auto")
        ttk.Entry(left, textvariable=self.gridPointsVar, width=10).grid(row=row, column=1, sticky="w")
        row += 1

        ttk.Label(left, text="Points per \u03c3:").grid(row=row, column=0, sticky="e")
        self.pointsPerSigmaVar = tk.StringVar(value="auto")
        ttk.Entry(left, textvariable=self.pointsPerSigmaVar, width=10).grid(row=row, column=1, sticky="w")
        # "auto" (the default for both fields) keeps this project's own
        # original grid exactly (4096 points, 100 points per hard-sphere
        # radius sigma) -- see oZfixpointOperator.py's own __init__ for
        # the full rationale. Grid points (N) sets the total array size
        # (and hence, together with points-per-sigma, the total real-
        # space range); points-per-sigma sets the near-contact
        # resolution. Increasing N at fixed points-per-sigma extends the
        # range without losing resolution -- the case that matters for
        # long-range/slowly-decaying potentials (e.g. small-kappa
        # Yukawa/DLVO tails), where the default range can otherwise
        # truncate the potential before it has actually decayed to zero
        # and distort S(q) at low q (verified directly earlier in this
        # project's own development: a 4x range extension reduced a
        # Yukawa potential's residual value at the grid edge from
        # ~1e-4 kT to ~1e-10 kT, and changed S(q->0) by about 33%).
        row += 1

        ttk.Label(left, text="Max iterations:").grid(row=row, column=0, sticky="e")
        self.maxIterVar = tk.StringVar(value="1000")
        ttk.Entry(left, textvariable=self.maxIterVar, width=10).grid(row=row, column=1, sticky="w")
        row += 1

        ttk.Label(left, text="Label:").grid(row=row, column=0, sticky="e")
        self.labelVar = tk.StringVar(value="run 1")
        ttk.Entry(left, textvariable=self.labelVar, width=22).grid(row=row, column=1, sticky="w")
        row += 1

        btnFrame = ttk.Frame(left)
        btnFrame.grid(row=row, column=0, columnspan=2, pady=8)
        row += 1
        self.calcBtn = ttk.Button(btnFrame, text="calculate", command=self._onCalculate)
        self.calcBtn.pack(side="left", padx=2)
        self.interruptBtn = ttk.Button(btnFrame, text="interrupt", command=self._onInterrupt, state="disabled")
        self.interruptBtn.pack(side="left", padx=2)
        ttk.Button(btnFrame, text="clear all", command=self._onClear).pack(side="left", padx=2)
        ttk.Button(btnFrame, text="delete last", command=self._onDeleteLast).pack(side="left", padx=2)

        # A second row, since the first is getting crowded -- saves/
        # loads ALL runs currently in the history list (not just the
        # selected one) as a single file, using a distinct extension
        # (.oz) so the file type is self-explanatory rather than a
        # generic .json/.txt -- this replaces the original Tcl GUI's
        # "assign to SQ plugin" mechanism (see this file's own
        # docstring), which only made sense inside the full SASfit
        # fitting engine, not standalone here.
        btnFrame2 = ttk.Frame(left)
        btnFrame2.grid(row=row, column=0, columnspan=2, pady=(0, 8))
        row += 1
        ttk.Button(btnFrame2, text="Save...", command=self._onSaveAll).pack(side="left", padx=2)
        ttk.Button(btnFrame2, text="Load...", command=self._onLoadAll).pack(side="left", padx=2)

        # A third row: standard-format export of the SELECTED run in
        # the history list below (or the last one, if none selected)
        # -- one run per export, deliberately not all-runs-at-once, to
        # keep the exported table a single simple sheet/file rather
        # than an increasingly wide one as more runs pile up. PNG is
        # the one exception: since each plot tab already overlays every
        # run at once, "export PNG" naturally means "save each tab's
        # current plot", not a single run's data.
        ttk.Label(left, text="Export selected/last run:").grid(row=row, column=0, columnspan=2, sticky="w")
        row += 1
        btnFrame3 = ttk.Frame(left)
        btnFrame3.grid(row=row, column=0, columnspan=2, pady=(0, 8))
        row += 1
        ttk.Button(btnFrame3, text="ASCII...", command=self._onExportASCII).pack(side="left", padx=2)
        ttk.Button(btnFrame3, text="CSV...", command=self._onExportCSV).pack(side="left", padx=2)
        ttk.Button(btnFrame3, text="Excel...", command=self._onExportExcel).pack(side="left", padx=2)
        ttk.Button(btnFrame3, text="PNG (all tabs)...", command=self._onExportPNG).pack(side="left", padx=2)

        ttk.Label(left, text="Run history:").grid(row=row, column=0, columnspan=2, sticky="w")
        row += 1
        self.historyList = tk.Listbox(left, height=10, width=32, exportselection=False)
        self.historyList.grid(row=row, column=0, columnspan=2, sticky="w")
        row += 1

        self.statusVar = tk.StringVar(value="ready")
        ttk.Label(left, textvariable=self.statusVar, foreground="blue").grid(
            row=row, column=0, columnspan=2, sticky="w", pady=(6, 0))

    def _rebuildPotentialParams(self, probe):
        for w in self.potParamsFrame.winfo_children():
            w.destroy()
        self.potParamVars = []
        methodName = "set" + self.potentialVar.get() + "Potential"
        import inspect
        argNames = inspect.getfullargspec(getattr(probe, methodName))[0][1:]
        for i, name in enumerate(argNames):
            ttk.Label(self.potParamsFrame, text=name + ":").grid(row=i, column=0, sticky="e")
            var = tk.StringVar(value="1.0")
            ttk.Entry(self.potParamsFrame, textvariable=var, width=10).grid(row=i, column=1, sticky="w")
            self.potParamVars.append(var)
        if not argNames:
            ttk.Label(self.potParamsFrame, text="(no parameters)").grid(row=0, column=0, sticky="w")

    def _rebuildClosureParam(self):
        for w in self.closureParamFrame.winfo_children():
            w.destroy()
        _, needsParam = CLOSURE_SETTERS[self.closureVar.get()]
        self.closureParamVar = None
        if needsParam:
            ttk.Label(self.closureParamFrame, text="\u03b1 / \u03b7:").grid(row=0, column=0, sticky="e")
            self.closureParamVar = tk.StringVar(value="1.0")
            ttk.Entry(self.closureParamFrame, textvariable=self.closureParamVar, width=10).grid(
                row=0, column=1, sticky="w")

    # ------------------------------------------------------------------
    def _buildRightPanel(self):
        right = ttk.Frame(self.root)
        right.grid(row=0, column=1, sticky="nsew")
        self.root.columnconfigure(1, weight=1)
        self.root.rowconfigure(0, weight=1)

        self.notebook = ttk.Notebook(right)
        self.notebook.pack(fill="both", expand=True)

        self.axes = {}
        self.canvases = {}
        for tabLabel, key, ylabel in CURVE_TABS:
            frame = ttk.Frame(self.notebook)
            self.notebook.add(frame, text=tabLabel)
            fig = Figure(figsize=(6, 5), dpi=100)
            ax = fig.add_subplot(111)
            ax.set_xlabel(r"$Q\sigma$" if key == "Sq" else r"$r/\sigma$")
            ax.set_ylabel(ylabel)
            # Matches self.xScaleVar's own default -- no data plotted
            # yet, so linthresh=1.0 is just a placeholder; _replot()
            # recomputes it from the actual data once a run exists.
            ax.set_xscale("symlog", linthresh=1.0)
            canvas = FigureCanvasTkAgg(fig, master=frame)
            canvas.get_tk_widget().pack(fill="both", expand=True)
            toolbar = NavigationToolbar2Tk(canvas, frame)
            toolbar.update()
            self.axes[key] = ax
            self.canvases[key] = canvas

    # ------------------------------------------------------------------
    def _onCalculate(self):
        if self.worker is not None and self.worker.is_alive():
            messagebox.showinfo("busy", "a calculation is already running")
            return
        try:
            phi = float(self.phiVar.get())
            maxIter = int(self.maxIterVar.get())
            potArgs = [float(v.get()) for v in self.potParamVars]
        except ValueError:
            messagebox.showerror("input error", "potential parameters, phi and max iterations must be numbers")
            return

        # "auto" (or anything unparseable) keeps ozLib.solve()'s own
        # defaults (None -> this project's original 4096-point,
        # 100-points-per-sigma grid, see oZfixpointOperator.py); a
        # valid positive integer overrides it. Parsed leniently (same
        # pattern as the symlog/asinh threshold field above) rather
        # than raising an input-error dialog for a non-numeric entry,
        # since "auto" itself is deliberately non-numeric.
        gridKwargs = {}
        gridPointsText = self.gridPointsVar.get().strip().lower()
        if gridPointsText != "auto":
            try:
                parsedN = int(gridPointsText)
                if parsedN > 1:
                    gridKwargs["numberOfRadialSamplingPoints"] = parsedN
            except ValueError:
                pass
        pointsPerSigmaText = self.pointsPerSigmaVar.get().strip().lower()
        if pointsPerSigmaText != "auto":
            try:
                parsedHsp = int(pointsPerSigmaText)
                if parsedHsp > 0:
                    gridKwargs["hardSphereDiameterInPoints"] = parsedHsp
            except ValueError:
                pass

        potentialName = self.potentialVar.get()
        closureName = self.closureVar.get()
        _, needsParam = CLOSURE_SETTERS[closureName]
        closureParam = None
        if needsParam:
            try:
                closureParam = float(self.closureParamVar.get())
            except ValueError:
                messagebox.showerror("input error", "closure parameter must be a number")
                return

        solverName = self.solverVar.get()
        label = self.labelVar.get() or f"run {len(self.runs)+1}"

        self.calcBtn.config(state="disabled")
        self.interruptBtn.config(state="normal")
        self.statusVar.set("calculating...")

        def work():
            try:
                def captureSolverInstance(solverInstance):
                    # lets the "interrupt" button below reach the live
                    # solver while it is still running -- ozLib.solve()
                    # itself only returns once the whole computation is
                    # already finished, see that function's own
                    # onSolverCreated docstring.
                    self.solver = solverInstance

                result = ozLib.solve(potential=potentialName, phi=phi, potentialArgs=potArgs,
                                      closure=closureName, closureParam=closureParam,
                                      solver=solverName, maxIterations=maxIter,
                                      onSolverCreated=captureSolverInstance, **gridKwargs)

                run = RunResult(label, result.r, result.q, potentialName, closureName, phi)
                run.curves = result.curves

                self.resultQueue.put(("done", run))
            except Exception:
                tb = traceback.format_exc()
                self.resultQueue.put(("error", tb))

        self.worker = threading.Thread(target=work, daemon=True)
        self.worker.start()

    def _pollResultQueue(self):
        try:
            while True:
                kind, payload = self.resultQueue.get_nowait()
                if kind == "done":
                    self._onCalculateDone(payload)
                else:
                    self._onCalculateError(payload)
        except queue.Empty:
            pass
        self.root.after(100, self._pollResultQueue)

    def _onCalculateDone(self, run):
        self.runs.append(run)
        self.historyList.insert("end", run.label)
        self._replot()
        self.calcBtn.config(state="normal")
        self.interruptBtn.config(state="disabled")
        self.statusVar.set(f"done: {run.label}")
        self.labelVar.set(f"run {len(self.runs)+1}")

    def _onCalculateError(self, tb):
        self.calcBtn.config(state="normal")
        self.interruptBtn.config(state="disabled")
        self.statusVar.set("error (see console)")
        print(tb)
        messagebox.showerror("calculation failed", tb.splitlines()[-1])

    def _onInterrupt(self):
        if self.solver is not None:
            self.solver.isInterrupted = True
        self.statusVar.set("interrupt requested...")

    def _onClear(self):
        self.runs = []
        self.historyList.delete(0, "end")
        self._replot()
        self.statusVar.set("cleared")

    def _onDeleteLast(self):
        if self.runs:
            self.runs.pop()
            self.historyList.delete("end")
            self._replot()

    def _onSaveAll(self):
        if not self.runs:
            messagebox.showinfo("nothing to save", "run at least one calculation first")
            return
        path = filedialog.asksaveasfilename(
            defaultextension=".oz",
            filetypes=[("Ornstein-Zernike GUI data", "*.oz"), ("All files", "*.*")],
            title="Save all plots")
        if not path:
            return
        data = {
            "format": "sasfit_oz_gui_save_v1",
            "runs": [
                {
                    "label": run.label,
                    "potential": run.potential,
                    "closure": run.closure,
                    "phi": run.phi,
                    "r": run.r.tolist(),
                    "q": run.q.tolist() if run.q is not None else None,
                    "curves": {name: arr.tolist() for name, arr in run.curves.items()},
                }
                for run in self.runs
            ],
        }
        try:
            with open(path, "w") as f:
                # allow_nan=True (the default) is relied on here: B(r)/y(r)
                # legitimately contain NaN inside the hard core (see
                # _onCalculate()'s own comment on why) -- Python's json
                # module writes/reads NaN as a de-facto (non-strict-JSON)
                # extension, which round-trips correctly between
                # json.dump()/json.load() even though some OTHER JSON
                # parsers would reject it; fine here since this format
                # is only meant to be read back by this same GUI.
                json.dump(data, f)
            self.statusVar.set(f"saved {len(self.runs)} run(s) to {path}")
        except OSError as e:
            messagebox.showerror("save failed", str(e))

    def _onLoadAll(self):
        path = filedialog.askopenfilename(
            filetypes=[("Ornstein-Zernike GUI data", "*.oz"), ("All files", "*.*")],
            title="Load plots")
        if not path:
            return
        try:
            with open(path, "r") as f:
                data = json.load(f)
            if data.get("format") != "sasfit_oz_gui_save_v1":
                messagebox.showerror("load failed", "unrecognised file format")
                return
            for runData in data["runs"]:
                qArr = np.array(runData["q"]) if runData.get("q") is not None else None
                run = RunResult(runData["label"], np.array(runData["r"]), qArr,
                                 runData["potential"], runData["closure"], runData["phi"])
                run.curves = {name: np.array(arr) for name, arr in runData["curves"].items()}
                self.runs.append(run)
                self.historyList.insert("end", run.label)
            self._replot()
            self.statusVar.set(f"loaded {len(data['runs'])} run(s) from {path}")
        except (OSError, json.JSONDecodeError, KeyError) as e:
            messagebox.showerror("load failed", str(e))

    def _getSelectedOrLastRun(self):
        sel = self.historyList.curselection()
        if sel:
            return self.runs[sel[0]]
        if self.runs:
            return self.runs[-1]
        return None

    def _writeTable(self, path, run, delimiter, commentedHeader):
        # Shared by ASCII and CSV export -- one column per curve
        # (r, q, then every CURVE_TABS entry in its usual order). r[i]
        # and q[i] are the same underlying Hankel-transform grid point i
        # expressed in real vs reciprocal space (see getqArray() in
        # oZfixpointOperator.py), so putting both as plain columns of
        # the same row-aligned table is correct, not a mismatch.
        keys = [key for _, key, _ in CURVE_TABS]
        header = delimiter.join(["r", "q"] + keys)
        n = len(run.r)
        with open(path, "w") as f:
            f.write(("# " if commentedHeader else "") + header + "\n")
            for i in range(n):
                row = [f"{run.r[i]:.10g}",
                       f"{run.q[i]:.10g}" if run.q is not None else ""]
                for key in keys:
                    y = run.curves.get(key)
                    val = y[i] if y is not None else float("nan")
                    row.append(f"{val:.10g}")
                f.write(delimiter.join(row) + "\n")

    def _onExportASCII(self):
        run = self._getSelectedOrLastRun()
        if run is None:
            messagebox.showinfo("nothing to export", "run at least one calculation first")
            return
        path = filedialog.asksaveasfilename(
            defaultextension=".txt",
            filetypes=[("ASCII text", "*.txt"), ("All files", "*.*")],
            initialfile=run.label + ".txt", title="Export as ASCII")
        if not path:
            return
        try:
            self._writeTable(path, run, delimiter="\t", commentedHeader=True)
            self.statusVar.set(f"exported '{run.label}' to {path} (ASCII)")
        except OSError as e:
            messagebox.showerror("export failed", str(e))

    def _onExportCSV(self):
        run = self._getSelectedOrLastRun()
        if run is None:
            messagebox.showinfo("nothing to export", "run at least one calculation first")
            return
        path = filedialog.asksaveasfilename(
            defaultextension=".csv",
            filetypes=[("CSV", "*.csv"), ("All files", "*.*")],
            initialfile=run.label + ".csv", title="Export as CSV")
        if not path:
            return
        try:
            # No leading "#" here (unlike ASCII): CSV readers (Excel
            # included) generally expect the header on a plain first
            # row, not a commented-out one.
            self._writeTable(path, run, delimiter=",", commentedHeader=False)
            self.statusVar.set(f"exported '{run.label}' to {path} (CSV)")
        except OSError as e:
            messagebox.showerror("export failed", str(e))

    def _onExportExcel(self):
        run = self._getSelectedOrLastRun()
        if run is None:
            messagebox.showinfo("nothing to export", "run at least one calculation first")
            return
        try:
            import openpyxl
        except ImportError:
            messagebox.showerror("missing dependency",
                                  "Excel export needs the 'openpyxl' package.\n"
                                  "Install it with: pip install openpyxl")
            return
        path = filedialog.asksaveasfilename(
            defaultextension=".xlsx",
            filetypes=[("Excel workbook", "*.xlsx"), ("All files", "*.*")],
            initialfile=run.label + ".xlsx", title="Export as Excel")
        if not path:
            return
        try:
            wb = openpyxl.Workbook()
            ws = wb.active
            # Excel sheet names: max 31 chars, and can't contain
            # \ / * ? : [ ] -- sanitised here since run.label is
            # free-form user text, not guaranteed to already be a valid
            # sheet name.
            import re
            sheetName = re.sub(r'[\\/*?:\[\]]', "_", run.label)[:31] or "OZ data"
            ws.title = sheetName
            keys = [key for _, key, _ in CURVE_TABS]
            ws.append(["r", "q"] + keys)
            n = len(run.r)
            for i in range(n):
                row = [float(run.r[i]), float(run.q[i]) if run.q is not None else None]
                for key in keys:
                    y = run.curves.get(key)
                    val = float(y[i]) if y is not None else None
                    # openpyxl can't write NaN as a numeric cell value
                    # (Br/yr legitimately contain some, see
                    # _onCalculate()'s own comment on why) -- write an
                    # empty cell instead, same convention spreadsheets
                    # themselves use for "no value here".
                    row.append(None if (val is not None and val != val) else val)
                ws.append(row)
            wb.save(path)
            self.statusVar.set(f"exported '{run.label}' to {path} (Excel)")
        except OSError as e:
            messagebox.showerror("export failed", str(e))

    def _onExportPNG(self):
        if not self.runs:
            messagebox.showinfo("nothing to export", "run at least one calculation first")
            return
        directory = filedialog.askdirectory(title="Choose a folder for the PNG files (one per tab)")
        if not directory:
            return
        import os
        saved = []
        try:
            for tabLabel, key, ylabel in CURVE_TABS:
                fname = os.path.join(directory, f"oz_{key}.png")
                self.axes[key].figure.savefig(fname, dpi=150, bbox_inches="tight")
                saved.append(fname)
            self.statusVar.set(f"saved {len(saved)} PNG file(s) to {directory}")
        except OSError as e:
            messagebox.showerror("export failed", str(e))

    def _replot(self):
        xScale = self.xScaleVar.get()
        # "auto" (or anything unparseable) keeps the original data-
        # derived behaviour; a valid positive number overrides it.
        # Parsed once here rather than per-tab since it doesn't depend
        # on which curve/tab is being drawn.
        manualThreshold = None
        thresholdText = self.xThresholdVar.get().strip().lower()
        if thresholdText != "auto":
            try:
                parsedThreshold = float(thresholdText)
                if parsedThreshold > 0:
                    manualThreshold = parsedThreshold
            except ValueError:
                pass

        for tabLabel, key, ylabel in CURVE_TABS:
            ax = self.axes[key]
            ax.clear()
            ax.set_xlabel(r"$Q\sigma$" if key == "Sq" else r"$r/\sigma$")
            ax.set_ylabel(ylabel)
            xMinPositive = None
            for i, run in enumerate(self.runs):
                y = run.curves.get(key)
                if y is None:
                    continue
                color = self.colors[i % len(self.colors)]
                # S(Q) lives on the reciprocal-space q-grid, not the
                # real-space r-grid every other curve here uses --
                # found by testing/inspection that this was previously
                # always plotted against run.r regardless, which is
                # wrong for this one curve specifically (run.q was
                # computed but never actually used).
                x = run.q if key == "Sq" and run.q is not None else run.r
                ax.plot(x, y, label=run.label, color=color, linewidth=1.2)
                positive = x[x > 0]
                if positive.size:
                    xMinPositive = positive.min() if xMinPositive is None else min(xMinPositive, positive.min())
            threshold = manualThreshold if manualThreshold is not None else (xMinPositive if xMinPositive else 1.0)
            if xScale == "symlog":
                # r/q here start at a small but nonzero value (see
                # getrArray()/getqArray() in oZfixpointOperator.py), so
                # there's no genuine need for a linear region around
                # exactly 0 -- but symlog's own default linthresh (1.0)
                # would otherwise swallow this tool's typically much
                # finer near-contact structure into that "linear" zone,
                # flattening exactly the detail a log-like scale is
                # meant to bring out. Basing linthresh on the actual
                # smallest x-value currently plotted (the "auto"
                # default) keeps that region maximally resolved in the
                # curve's own shape -- found by testing, though, that
                # this also makes the tick marks collapse to look just
                # like plain log scale, since matplotlib's own
                # SymmetricalLogLocator only places a visibly distinct
                # "linear-region" tick (e.g. exactly at 0) once the
                # threshold is a non-negligible fraction of the range;
                # the manual override above exists for exactly this
                # tradeoff.
                ax.set_xscale("symlog", linthresh=threshold)
            elif xScale == "asinh":
                # matplotlib's modern equivalent of the arcsinh axis
                # scaling SASfit itself used historically (before
                # matplotlib had a symlog/asinh scale to reach for) --
                # unlike symlog's linthresh, which draws a hard line
                # between an exactly-linear zone and an exactly-log
                # one, asinh (literally arcsinh(x/linear_width)) has no
                # such sharp edge: it curves smoothly from the very
                # start, already mildly compressing by x=linear_width
                # rather than staying exactly linear out to that point.
                # Same auto-vs-manual threshold reasoning as symlog above.
                ax.set_xscale("asinh", linear_width=threshold)
            else:
                ax.set_xscale(xScale)
            if self.runs:
                ax.legend(fontsize=8)
            self.canvases[key].draw_idle()


def plt_colors():
    import matplotlib.pyplot as plt
    return plt.rcParams["axes.prop_cycle"].by_key()["color"]


def main():
    root = tk.Tk()
    app = OZgui(root)
    root.geometry("1200x750")
    root.mainloop()


if __name__ == "__main__":
    main()
