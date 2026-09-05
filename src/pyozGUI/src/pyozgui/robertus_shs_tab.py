# -*- coding: utf-8 -*-
"""
Polydisperse adhesive (sticky) hard-sphere notebook tab for oZgui.py.

Same self-contained ttk.Frame pattern as polydisperse_yukawa_tab.py: it knows
nothing about the OZgui class and touches none of its state, so adding it is
one line in oZgui.py's EXTRA_TABS and removing it is one line back. Import or
construction failure costs only this tab.

    from robertus_shs_tab import RobertusSHSTab
    tab = RobertusSHSTab(someNotebook)
    someNotebook.add(tab, text="Robertus SHS")

It can also be run on its own for testing:

    python robertus_shs_tab.py

What it shows, all of it already implemented and validated elsewhere in this
directory:

  - a continuous size distribution (log-normal, Schulz-Zimm, Gaussian or
    Weibull) discretised onto N classes by the C engine itself,
  - a form factor (homogeneous sphere or core-shell),
  - the EXACT polydisperse partial structure factors S_ij(Q) of the Robertus
    multicomponent Percus-Yevick adhesive-hard-sphere model, from SASfit's own
    C engine (src/plugins/robertus_shs/robertus_shs_core.c via
    robertusWrapper.py), after Robertus, Philipse, Joosten & Levine,
    J. Chem. Phys. 90, 4482 (1989),
  - the exact I(Q), and all six of SASfit's approximate schemes for combining
    a structure factor with a size distribution, so the error of each can be
    read straight off the plot.

Because RobertusSHSSAS derives from the same PolydisperseSASBase as the Yukawa
model, the approximation code driving the "Approx. error" plot is
byte-identical between the two tabs. Comparing them is therefore a genuine
test of how those approximations fare for a completely different interaction,
not a comparison of two different implementations.

CONVENTION, worth keeping in mind: stickiness tau is size-INDEPENDENT (the
paper's own assumption) and SMALLER tau means STICKIER -- the opposite
direction to what "strength" usually suggests. Below roughly tau ~ 0.1 the
Percus-Yevick closure has no physical solution at all and the engine will say
so; that is a property of the closure, not a bug.
"""
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

from polydisperse_tab_controls import PolydisperseTabControls

# The solver stack lives next to this file. Import failure must not take the
# whole GUI down -- the tab shows a placeholder instead.
IMPORT_ERROR = None
try:
    from polydisperse_yukawa_sas import Sphere, CoreShell
    from robertus_shs_sas import RobertusSHSSAS
    from robertusWrapper import (DIST_LOGNORMAL, DIST_SCHULZ_ZIMM,
                                 DIST_GAUSSIAN, DIST_WEIBULL)
except Exception as _exc:                                  # pragma: no cover
    IMPORT_ERROR = _exc


# Approximation schemes, in SASfit's own numbering. Identical list to the
# Yukawa tab's, and driving the identical methods on PolydisperseSASBase.
APPROX_SCHEMES = [
    ("0  monodisperse",                  "I_monodisperse"),
    ("1  decoupling (Kotlarchyk-Chen)",  "I_decoupling"),
    ("2  local monodisperse (Pedersen)", "I_lma"),
    ("3  partial structure factors",     "I_partial_sf"),
    ("4  scaling (Gazzillo)",            "I_scaling"),
    ("5  van der Waals one-fluid",       "I_vdw1"),
]

# Distributions the C engine implements, mapped to its own integer codes.
DIST_CHOICES = [
    ("Log-normal",  "DIST_LOGNORMAL"),
    ("Schulz-Zimm", "DIST_SCHULZ_ZIMM"),
    ("Gaussian",    "DIST_GAUSSIAN"),
    ("Weibull",     "DIST_WEIBULL"),
]

PLOT_TABS = [
    ("I(Q)",              "iq"),
    ("Approx. error",     "err"),
    ("S_ij(Q)",           "sij"),
    ("S(Q)",              "sq"),
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
        self.summary = ""
        self.label = ""


def _explain(exc):
    """Turn an engine failure into something a user can act on. The adhesive
    Percus-Yevick closure genuinely has no solution over part of the
    (phi, tau) plane, so 'no solution' is a physical statement here, not a
    numerical accident."""
    msg = str(exc)
    low = msg.lower()
    if "lambda" in low or "no real solution" in low or "tau" in low:
        return ("The Percus-Yevick adhesive closure has no physical solution "
                "at this state point.\n\n" + msg + "\n\n"
                "What to try:\n"
                "  - INCREASE tau (less sticky). Smaller tau is stickier, and\n"
                "    below the critical tau_c(phi) no real solution exists.\n"
                "  - reduce the volume fraction phi.\n"
                "  - reduce the polydispersity, which shifts tau_c.")
    if "converge" in low:
        return ("The multicomponent closure did not converge.\n\n" + msg + "\n\n"
                "Try fewer size classes, a smaller polydispersity, or a state\n"
                "point further from the no-solution boundary (larger tau).")
    return msg


class RobertusSHSTab(PolydisperseTabControls, ttk.Frame):
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
        msg = ("The polydisperse sticky-hard-sphere modules could not be "
               "imported,\nso this tab is inactive. The other tabs are "
               "unaffected.\n\n"
               f"{type(IMPORT_ERROR).__name__}: {IMPORT_ERROR}\n\n"
               "Expected next to oZgui.py:\n"
               "    robertus_shs_sas.py\n"
               "    robertusWrapper.py\n"
               "    polydisperse_sas_base.py\n"
               "    polydisperse_yukawa_sas.py\n\n"
               "robertusWrapper.py loads the compiled engine from\n"
               "src/plugins/robertus_shs/, so that plugin must be built.")
        ttk.Label(self, text=msg, justify="left", padding=20,
                  foreground="#a00").pack(anchor="nw")

    # ------------------------------------------------------------------
    def _buildControls(self):
        # Same structural approach as the Yukawa tab: the action buttons and
        # status line live in a footer packed to the bottom, so they cannot be
        # pushed off-screen by a taller parameter block above them.
        outer = ttk.Frame(self, padding=6)
        outer.grid(row=0, column=0, sticky="ns")
        footer = ttk.Frame(outer)
        footer.pack(side="bottom", fill="x", pady=(8, 0))
        left = ttk.Frame(outer)
        left.pack(side="top", fill="both", expand=True)

        row = 0

        def label(text, tip=None):
            # reads `row` from the enclosing scope but never assigns it, so no
            # `nonlocal` declaration is needed here (unlike entry() below)
            lab = ttk.Label(left, text=text)
            lab.grid(row=row, column=0, sticky="e", padx=(0, 4))
            if tip:
                lab.configure(foreground="#333")
            return lab

        self._entryWidgets = {}

        def entry(text, default, tip=None, width=12, key=None):
            nonlocal row
            label(text, tip)
            var = tk.StringVar(value=default)
            ent = ttk.Entry(left, textvariable=var, width=width)
            ent.grid(row=row, column=1, sticky="w")
            if key:
                self._entryWidgets[key] = ent
            if tip:
                ttk.Label(left, text=tip, foreground="#666",
                          wraplength=210, justify="left").grid(
                    row=row + 1, column=0, columnspan=2, sticky="w",
                    pady=(0, 4))
                row += 1
            row += 1
            return var

        ttk.Label(left, text="Sticky hard spheres (Robertus, PY)",
                  font=("TkDefaultFont", 10, "bold")).grid(
            row=row, column=0, columnspan=2, sticky="w", pady=(0, 6))
        row += 1

        self.DmeanVar = entry("Mean diameter D", "100.0",
                              "same length unit as Q^-1")
        self.srelVar = entry("Rel. s.d.", "0.20",
                             "relative width of the size distribution")
        self.phiVar = entry("Volume fraction", "0.20")
        self.tauVar = entry("Stickiness tau", "0.20",
                            "SMALLER tau = STICKIER. Below tau_c(phi) the PY "
                            "closure has no real solution.")
        self.nbinsVar = entry("Size classes", "12",
                              "number of discrete classes the engine uses")

        label("Distribution")
        self.distVar = tk.StringVar(value=DIST_CHOICES[0][0])
        ttk.Combobox(left, textvariable=self.distVar, width=14, state="readonly",
                     values=[n for n, _ in DIST_CHOICES]).grid(
            row=row, column=1, sticky="w")
        row += 1

        ttk.Separator(left, orient="horizontal").grid(
            row=row, column=0, columnspan=2, sticky="ew", pady=6)
        row += 1

        label("Form factor")
        self.ffVar = tk.StringVar(value="Sphere")
        ffBox = ttk.Combobox(left, textvariable=self.ffVar, width=14,
                             state="readonly", values=["Sphere", "Core-shell"])
        ffBox.grid(row=row, column=1, sticky="w")
        ffBox.bind("<<ComboboxSelected>>", lambda e: self._syncEnabledStates())
        row += 1

        self.shellVar = entry("Shell thickness", "10.0",
                              "core-shell only", key="shell")
        self.rhoCoreVar = entry("rho core", "1.0", "core-shell only",
                                key="rhoCore")
        self.rhoShellVar = entry("rho shell", "2.0", "core-shell only",
                                 key="rhoShell")

        ttk.Separator(left, orient="horizontal").grid(
            row=row, column=0, columnspan=2, sticky="ew", pady=6)
        row += 1

        self.QminVar = entry("Q min", "1e-3")
        self.QmaxVar = entry("Q max", "0.5")
        self.nQVar = entry("Q points", "300")

        self.labelVar = entry("Label", "run 1", width=16)

        # ---- footer ----
        # Standard control set shared with the other polydisperse tabs (see
        # polydisperse_tab_controls.py): Compute / Interrupt / Clear all /
        # Delete last, Save / Load, and the export selector, plus the run
        # history list and status line.
        #
        # supportsInterrupt=False and no solver dropdown: this model is
        # ANALYTIC. The Percus-Yevick lambda(i,j) system is solved by a single
        # fsolve call with no iteration boundary to poll, so there is nothing
        # for an interrupt to check and no fixpoint strategy to choose.
        # Offering either would imply a control that does nothing.
        self.buildStandardControls(footer, supportsInterrupt=False,
                                    solverChoices=None)

    def _syncEnabledStates(self):
        isCoreShell = self.ffVar.get() == "Core-shell"
        state = "normal" if isCoreShell else "disabled"
        for w in self._entryWidgets.values():
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
            ax.set_xlabel("D"); ax.set_ylabel("weight")

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
        p["Dmean"] = f(self.DmeanVar, "Mean diameter", lo=1e-12)
        p["srel"] = f(self.srelVar, "Rel. s.d.", lo=0.0, hi=0.99)
        p["phi"] = f(self.phiVar, "Volume fraction", lo=1e-9, hi=0.74)
        p["tau"] = f(self.tauVar, "Stickiness tau", lo=1e-6)
        p["nbins"] = int(f(self.nbinsVar, "Size classes", lo=1, hi=50))
        p["distName"] = self.distVar.get()
        p["ff"] = self.ffVar.get()
        p["shell"] = f(self.shellVar, "Shell thickness", lo=0.0)
        p["rhoCore"] = f(self.rhoCoreVar, "rho core")
        p["rhoShell"] = f(self.rhoShellVar, "rho shell")
        p["Qmin"] = f(self.QminVar, "Q min", lo=1e-12)
        p["Qmax"] = f(self.QmaxVar, "Q max", lo=1e-12)
        p["nQ"] = int(f(self.nQVar, "Q points", lo=2, hi=20000))
        if p["Qmax"] <= p["Qmin"]:
            raise ValueError("Q max must be greater than Q min")
        p["label"] = self.labelVar.get() or "run"
        return p

    # ------------------------------------------------------------------
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
        self.statusVar.set("computing...")
        self.worker = threading.Thread(target=self._computeWorker, args=(p,),
                                       daemon=True)
        self.worker.start()
        if not self._polling:
            self._polling = True
            self.after(100, self._pollResultQueue)

    def _computeWorker(self, p):
        try:
            distCode = dict(DIST_CHOICES)[p["distName"]]
            dist = {"DIST_LOGNORMAL": DIST_LOGNORMAL,
                    "DIST_SCHULZ_ZIMM": DIST_SCHULZ_ZIMM,
                    "DIST_GAUSSIAN": DIST_GAUSSIAN,
                    "DIST_WEIBULL": DIST_WEIBULL}[distCode]

            if p["ff"] == "Core-shell":
                ff = CoreShell(p["shell"], p["rhoCore"], p["rhoShell"])
            else:
                ff = Sphere()

            sas = RobertusSHSSAS(p["Dmean"], p["srel"], p["phi"], p["tau"],
                                 formfactor=ff, nbins=p["nbins"], dist=dist)

            Q = np.logspace(np.log10(p["Qmin"]), np.log10(p["Qmax"]), p["nQ"])

            res = _Result()
            res.Q = Q
            res.label = p["label"]
            res.I_exact = sas.I_exact(Q)
            res.S = sas.S_partials(Q)
            res.S_number = sas.S_number(Q)
            res.sigma = np.asarray(sas.sigma).copy()
            res.R = np.asarray(sas.R).copy()
            res.w = np.asarray(sas.w).copy()

            for label, method in APPROX_SCHEMES:
                try:
                    res.approx[label] = getattr(sas, method)(Q)
                except Exception as e:
                    # One unavailable approximation must not lose the run --
                    # e.g. the monodisperse reference has no PY solution at a
                    # state point where the polydisperse system does.
                    res.approx[label] = None
                    res.summary += f"{label}: unavailable ({e})\n"

            lines = [f"label            : {p['label']}",
                     f"mean diameter D  : {p['Dmean']:g}",
                     f"rel. s.d.        : {p['srel']:g}",
                     f"volume fraction  : {p['phi']:g}",
                     f"stickiness tau   : {p['tau']:g}   (smaller = stickier)",
                     f"distribution     : {p['distName']}",
                     f"size classes     : {len(res.sigma)}",
                     f"sigma_eff        : {sas.sigma_eff():g}",
                     "",
                     "class    diameter      weight"]
            for i, (d, w) in enumerate(zip(res.sigma, res.w)):
                lines.append(f"{i:5d} {d:14.6g} {w:12.6g}")
            lines.append("")
            lines.append("max |I_approx/I_exact - 1| over the plotted Q range:")
            for label, _ in APPROX_SCHEMES:
                Ia = res.approx.get(label)
                if Ia is None:
                    lines.append(f"  {label:34s}  (unavailable)")
                else:
                    with np.errstate(divide="ignore", invalid="ignore"):
                        e = np.abs(Ia/res.I_exact - 1.0)
                    lines.append(f"  {label:34s}  {np.nanmax(e):.3e}")
            res.summary = "\n".join(lines) + ("\n\n" + res.summary
                                               if res.summary else "")

            self.resultQueue.put(("done", res))
        except Exception as e:
            self.resultQueue.put(("error", (e, traceback.format_exc())))

    def _pollResultQueue(self):
        try:
            while True:
                kind, payload = self.resultQueue.get_nowait()
                if kind == "done":
                    # registerRun() (from PolydisperseTabControls) appends to
                    # the run history and sets self.result, so Clear all /
                    # Delete last / Save / Export all operate on it.
                    self.registerRun(payload, payload.label)
                    self._replot()
                    self.statusVar.set(f"done: {payload.label}")
                else:
                    exc, tb = payload
                    print(tb, file=sys.stderr)
                    self.statusVar.set("failed (see Summary tab)")
                    self.summaryText.delete("1.0", "end")
                    self.summaryText.insert("end", _explain(exc))
                self.computeBtn.configure(state="normal")
        except queue.Empty:
            pass
        self.after(100, self._pollResultQueue)

    def _onClear(self):
        self.result = None
        self._replot()
        self.summaryText.delete("1.0", "end")
        self.statusVar.set("cleared")

    def _exportColumns(self):
        """Columns handed to the shared exporter (ASCII/CSV/clipboard)."""
        r = self.selectedRun()
        cols = ["Q", "I_exact", "S_number"]
        data = [r.Q, r.I_exact, r.S_number]
        for label, _ in APPROX_SCHEMES:
            Ia = r.approx.get(label)
            if Ia is not None:
                cols.append("scheme" + label.split()[0])
                data.append(Ia)
        return cols, data

    # ------------------------------------------------------------------
    def _replot(self):
        for _, key in PLOT_TABS:
            ax = self.axes[key]
            ax.clear()
            self._decorateAxes(ax, key)

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
                    e = np.abs(Ia/r.I_exact - 1.0)
                ax.plot(r.Q, e, lw=1, label=label)
            ax.legend(fontsize=7)

            ax = self.axes["sij"]
            n = r.S.shape[1]
            # A full N x N legend is unreadable for a dozen classes, so only
            # the diagonal is labelled and the off-diagonals are drawn faint.
            for i in range(n):
                for j in range(i, n):
                    if i == j:
                        ax.plot(r.Q, r.S[:, i, j], lw=1.2,
                                label=f"S_{i}{j}" if n <= 6 else None)
                    else:
                        ax.plot(r.Q, r.S[:, i, j], lw=0.5, alpha=0.35)
            if n <= 6:
                ax.legend(fontsize=7)

            ax = self.axes["sq"]
            ax.plot(r.Q, r.S_number, "b-", lw=1.5)

            ax = self.axes["dist"]
            ax.bar(r.sigma, r.w, width=0.02*np.mean(r.sigma), color="#4477aa")

            self.summaryText.delete("1.0", "end")
            self.summaryText.insert("end", r.summary)

        for key in self.canvases:
            self.canvases[key].draw_idle()

    # ------------------------------------------------------------------
    def _onExport(self):
        if self.result is None:
            messagebox.showinfo("nothing to export", "compute something first")
            return
        path = filedialog.asksaveasfilename(
            defaultextension=".txt",
            filetypes=[("ASCII text", "*.txt"), ("CSV", "*.csv"),
                       ("All files", "*.*")],
            initialfile=self.result.label + ".txt",
            title="Export I(Q)")
        if not path:
            return
        r = self.result
        delim = "," if path.lower().endswith(".csv") else "\t"
        cols = ["Q", "I_exact", "S_number"]
        data = [r.Q, r.I_exact, r.S_number]
        for label, _ in APPROX_SCHEMES:
            Ia = r.approx.get(label)
            if Ia is not None:
                cols.append("scheme" + label.split()[0])
                data.append(Ia)
        try:
            with open(path, "w") as fh:
                fh.write(("# " if delim == "\t" else "") + delim.join(cols) + "\n")
                for k in range(len(r.Q)):
                    fh.write(delim.join(f"{d[k]:.10g}" for d in data) + "\n")
            self.statusVar.set(f"exported to {path}")
        except OSError as e:
            messagebox.showerror("export failed", str(e))


def main():                                                # pragma: no cover
    root = tk.Tk()
    root.title("Polydisperse sticky hard spheres (Robertus)")
    tab = RobertusSHSTab(root)
    tab.pack(fill="both", expand=True)
    root.geometry("1150x750")
    root.mainloop()


if __name__ == "__main__":                                 # pragma: no cover
    main()
