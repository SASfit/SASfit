# -*- coding: utf-8 -*-
"""
Shared control bar for the polydisperse notebook tabs.

The "OZ solver" tab offers calculate / interrupt / clear all / delete last /
save / load / export and a run-history list. The polydisperse tabs
(polydisperse_yukawa_tab, ry_polydisperse_yukawa_tab, robertus_shs_tab) only
had Compute / Clear / Export. This module supplies the rest ONCE, as a mixin,
rather than repeating the same ~200 lines in three files that would then drift
apart.

Usage: inherit before ttk.Frame and call buildStandardControls() where the
button row belongs.

    class MyTab(PolydisperseTabControls, ttk.Frame):
        def _onCompute(self): ...      # tab-specific, must set self.result
        def _replot(self): ...         # tab-specific
        def _exportColumns(self):      # tab-specific
            return ["Q", "I"], [self.result.Q, self.result.I]

The mixin owns: self.runs (history), the button row, the history listbox and
the status line. It deliberately does NOT own the compute worker: interrupting
and re-plotting are tab-specific, so the mixin calls back into the tab.

WHY INTERRUPT IS BEST-EFFORT
----------------------------
oZgui.py stops a running solve by setting solverInstance.isInterrupted, which
the fixpoint loops check between iterations. That only works for tabs driving
an OZ solver (the RY tab). The analytic tabs -- MSA/RMSA and Robertus -- spend
their time inside a single numpy/fsolve call with no iteration boundary to
check, so there is nothing to poll and Interrupt can only prevent the RESULT
being used, not stop the computation. The button is disabled for those tabs
rather than pretending otherwise; passing supportsInterrupt=False does that.
"""
import json

import tkinter as tk
from tkinter import ttk, filedialog, messagebox

import numpy as np


class PolydisperseTabControls:
    """Mixin: standard button row, run history, save/load, export."""

    # ------------------------------------------------------------------
    def buildStandardControls(self, parent, supportsInterrupt=False,
                              solverChoices=None, statusWraplength=230):
        """Create the button row, history list and status line.

        solverChoices: optional {label: class} mapping. When given, a solver
        dropdown is shown and the chosen class is available as
        self.selectedSolverClass(). Only meaningful for tabs that actually run
        an iterative solver -- the analytic tabs pass None.
        """
        if not hasattr(self, "runs"):
            self.runs = []
        self._supportsInterrupt = supportsInterrupt
        self.solverChoices = solverChoices or {}

        row1 = ttk.Frame(parent)
        row1.pack(fill="x")
        self.computeBtn = ttk.Button(row1, text="Compute", command=self._onCompute)
        self.computeBtn.pack(side="left", padx=2)
        self.interruptBtn = ttk.Button(row1, text="Interrupt",
                                       command=self._onInterrupt, state="disabled")
        self.interruptBtn.pack(side="left", padx=2)
        ttk.Button(row1, text="Clear all", command=self._onClearAll).pack(side="left", padx=2)
        ttk.Button(row1, text="Delete last", command=self._onDeleteLast).pack(side="left", padx=2)

        row2 = ttk.Frame(parent)
        row2.pack(fill="x", pady=(4, 0))
        ttk.Button(row2, text="Save...", command=self._onSaveAll).pack(side="left", padx=2)
        ttk.Button(row2, text="Load...", command=self._onLoadAll).pack(side="left", padx=2)

        row3 = ttk.Frame(parent)
        row3.pack(fill="x", pady=(4, 0))
        self.exportFormatVar = tk.StringVar(value="ASCII")
        self._exportDispatch = {
            "ASCII": lambda: self._exportTable("\t", True, ".txt"),
            "CSV": lambda: self._exportTable(",", False, ".csv"),
            "Clipboard": self._exportClipboard,
            "PNG (all plots)": self._exportPNG,
        }
        ttk.Combobox(row3, textvariable=self.exportFormatVar, state="readonly",
                     width=15, values=list(self._exportDispatch)).pack(side="left")
        ttk.Button(row3, text="Export", command=self._onExportSelected).pack(
            side="left", padx=(4, 0))

        if self.solverChoices:
            row4 = ttk.Frame(parent)
            row4.pack(fill="x", pady=(6, 0))
            ttk.Label(row4, text="Solver:").pack(side="left")
            self.solverVar = tk.StringVar(value=next(iter(self.solverChoices)))
            width = max(len(k) for k in self.solverChoices) + 2
            ttk.Combobox(row4, textvariable=self.solverVar, state="readonly",
                         width=width,
                         values=list(self.solverChoices)).pack(side="left", padx=(4, 0))

        ttk.Label(parent, text="Run history:").pack(anchor="w", pady=(6, 0))
        self.historyList = tk.Listbox(parent, height=6, width=32,
                                      exportselection=False)
        self.historyList.pack(anchor="w")

        self.statusVar = tk.StringVar(value="ready")
        ttk.Label(parent, textvariable=self.statusVar, foreground="blue",
                  wraplength=statusWraplength, justify="left").pack(
            anchor="w", pady=(6, 0))

        if not supportsInterrupt:
            # Left permanently disabled with an explanatory tooltip-in-status
            # rather than silently doing nothing; see the module docstring.
            self.interruptBtn.configure(state="disabled")

    # ------------------------------------------------------------------
    def selectedSolverClass(self):
        """Class chosen in the solver dropdown, or None when not offered."""
        if not self.solverChoices:
            return None
        return self.solverChoices.get(self.solverVar.get())

    # ------------------------------------------------------------------
    def registerRun(self, result, label=None):
        """Append a finished result to the history. Tabs call this from their
        own completion handler."""
        label = label or getattr(result, "label", f"run {len(self.runs)+1}")
        result.label = label
        self.runs.append(result)
        self.historyList.insert("end", label)
        self.result = result

    def selectedRun(self):
        sel = self.historyList.curselection()
        if sel:
            return self.runs[sel[0]]
        return self.runs[-1] if self.runs else None

    # ------------------------------------------------------------------
    def _onInterrupt(self):
        solver = getattr(self, "solver", None)
        if solver is not None:
            solver.isInterrupted = True
            self.statusVar.set("interrupt requested...")
        else:
            self.statusVar.set("nothing to interrupt")

    def _onClearAll(self):
        self.runs = []
        self.result = None
        self.historyList.delete(0, "end")
        self._replot()
        self.statusVar.set("cleared")

    def _onDeleteLast(self):
        if not self.runs:
            return
        self.runs.pop()
        self.historyList.delete("end")
        self.result = self.runs[-1] if self.runs else None
        self._replot()
        self.statusVar.set(f"{len(self.runs)} run(s) left")

    # ------------------------------------------------------------------
    def _onSaveAll(self):
        if not self.runs:
            messagebox.showinfo("nothing to save", "compute something first")
            return
        path = filedialog.asksaveasfilename(
            defaultextension=".oz",
            filetypes=[("Ornstein-Zernike GUI data", "*.oz"), ("All files", "*.*")],
            title="Save all runs")
        if not path:
            return
        payload = {"format": "sasfit_polydisperse_tab_save_v1", "runs": []}
        for r in self.runs:
            entry = {"label": r.label}
            for key, val in vars(r).items():
                if key == "label":
                    continue
                if isinstance(val, np.ndarray):
                    entry[key] = {"__ndarray__": val.tolist()}
                elif isinstance(val, (int, float, str, bool, type(None))):
                    entry[key] = val
                elif isinstance(val, dict):
                    entry[key] = {k: (v.tolist() if isinstance(v, np.ndarray) else v)
                                  for k, v in val.items()}
            payload["runs"].append(entry)
        try:
            # allow_nan is relied on: some curves legitimately contain NaN
            # inside a hard core. Python's json writes/reads it as a de-facto
            # extension, which round-trips with json.load even though stricter
            # parsers reject it -- fine, since only this GUI reads the file.
            with open(path, "w") as fh:
                json.dump(payload, fh)
            self.statusVar.set(f"saved {len(self.runs)} run(s)")
        except OSError as e:
            messagebox.showerror("save failed", str(e))

    def _onLoadAll(self):
        path = filedialog.askopenfilename(
            filetypes=[("Ornstein-Zernike GUI data", "*.oz"), ("All files", "*.*")],
            title="Load runs")
        if not path:
            return
        try:
            with open(path) as fh:
                payload = json.load(fh)
            if payload.get("format") != "sasfit_polydisperse_tab_save_v1":
                messagebox.showerror("load failed", "unrecognised file format")
                return
            for entry in payload["runs"]:
                obj = _LoadedRun()
                for key, val in entry.items():
                    if isinstance(val, dict) and "__ndarray__" in val:
                        setattr(obj, key, np.array(val["__ndarray__"]))
                    elif isinstance(val, dict):
                        setattr(obj, key, {k: np.array(v) if isinstance(v, list) else v
                                           for k, v in val.items()})
                    else:
                        setattr(obj, key, val)
                self.registerRun(obj, obj.label)
            self._replot()
            self.statusVar.set(f"loaded {len(payload['runs'])} run(s)")
        except (OSError, json.JSONDecodeError, KeyError) as e:
            messagebox.showerror("load failed", str(e))

    # ------------------------------------------------------------------
    def _onExportSelected(self):
        handler = self._exportDispatch.get(self.exportFormatVar.get())
        if handler is not None:
            handler()

    def _tableText(self, delimiter, commentedHeader):
        cols, data = self._exportColumns()
        lines = [("# " if commentedHeader else "") + delimiter.join(cols)]
        n = len(data[0])
        for k in range(n):
            lines.append(delimiter.join(f"{d[k]:.10g}" for d in data))
        return "\n".join(lines) + "\n"

    def _exportTable(self, delimiter, commentedHeader, ext):
        run = self.selectedRun()
        if run is None:
            messagebox.showinfo("nothing to export", "compute something first")
            return
        path = filedialog.asksaveasfilename(
            defaultextension=ext,
            filetypes=[("data", "*" + ext), ("All files", "*.*")],
            initialfile=run.label + ext, title="Export")
        if not path:
            return
        try:
            with open(path, "w") as fh:
                fh.write(self._tableText(delimiter, commentedHeader))
            self.statusVar.set(f"exported to {path}")
        except OSError as e:
            messagebox.showerror("export failed", str(e))

    def _exportClipboard(self):
        if self.selectedRun() is None:
            messagebox.showinfo("nothing to copy", "compute something first")
            return
        # Tab-separated and no comment marker: the most broadly
        # paste-compatible plain-clipboard form for spreadsheets.
        self.clipboard_clear()
        self.clipboard_append(self._tableText("\t", False))
        self.statusVar.set("copied to clipboard")

    def _exportPNG(self):
        axes = getattr(self, "axes", None)
        if not axes:
            messagebox.showinfo("nothing to export", "no plots to save")
            return
        directory = filedialog.askdirectory(
            title="Choose a folder for the PNG files (one per plot)")
        if not directory:
            return
        import os
        try:
            for key, ax in axes.items():
                ax.figure.savefig(os.path.join(directory, f"plot_{key}.png"),
                                  dpi=150, bbox_inches="tight")
            self.statusVar.set(f"saved {len(axes)} PNG file(s)")
        except OSError as e:
            messagebox.showerror("export failed", str(e))

    # ------------------------------------------------------------------
    # Tabs must provide these.
    def _onCompute(self):
        raise NotImplementedError

    def _replot(self):
        raise NotImplementedError

    def _exportColumns(self):
        """Return (column_names, list_of_1d_arrays) for the selected run."""
        raise NotImplementedError


class _LoadedRun:
    """Plain namespace for a run restored from file."""
    pass
