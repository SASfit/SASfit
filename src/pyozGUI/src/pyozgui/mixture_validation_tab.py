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


class MixtureValidationTab(ttk.Frame):
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
        self.columnconfigure(1, weight=1)
        self.rowconfigure(0, weight=1)
        self._buildControls()
        self._buildPlot()

    # ------------------------------------------------------------------
    def _buildControls(self):
        outer = ttk.Frame(self, padding=6)
        outer.grid(row=0, column=0, sticky="nsew")
        r = 0

        def entry(label, default, width=12):
            nonlocal r
            ttk.Label(outer, text=label).grid(row=r, column=0, sticky="e")
            v = tk.StringVar(value=default)
            ttk.Entry(outer, textvariable=v, width=width).grid(
                row=r, column=1, sticky="w")
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
        self.zVar = entry("Macroion z:", "0.0")
        self.lbVar = entry("Bjerrum length:", "7.189")
        self.sigma1Var = entry("Microion diam.:", "5.0")
        ttk.Label(outer,
                  text=("With z = 0 the Gazzillo reference reduces to mixture "
                        "PY and must agree with mixscatter to machine "
                        "precision. With z != 0 it is a DIFFERENT model "
                        "(explicit microions), so disagreement with the "
                        "numerical curve is expected."),
                  foreground="grey", wraplength=250, justify="left").grid(
            row=r, column=0, columnspan=2, sticky="w"); r += 1

        ttk.Separator(outer, orient="horizontal").grid(
            row=r, column=0, columnspan=2, sticky="ew", pady=6); r += 1
        ttk.Label(outer, text="Closure:").grid(row=r, column=0, sticky="e")
        self.closureVar = tk.StringVar(value="Percus-Yevick")
        ttk.Combobox(outer, textvariable=self.closureVar, width=20,
                     state="readonly",
                     values=ozLib.multicomponentCapableClosures()
                     ).grid(row=r, column=1, sticky="w"); r += 1
        self.ppsVar = entry("Points per sigma:", "100")
        ttk.Label(outer,
                  text=("The coarse/fine ratio below is the diagnostic that "
                        "matters: genuine grid error falls ~4x for a 4x "
                        "refinement. An error that does NOT shrink indicates "
                        "a defect, not discretisation."),
                  foreground="grey", wraplength=250, justify="left").grid(
            row=r, column=0, columnspan=2, sticky="w"); r += 1

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

        self.statusVar = tk.StringVar(
            value="ready" + ("" if _ms is not None else
                             "  (mixscatter not installed)"))
        ttk.Label(outer, textvariable=self.statusVar, foreground="#00008b",
                  wraplength=250, justify="left").grid(
            row=r, column=0, columnspan=2, sticky="w", pady=(6, 0)); r += 1

        self.summary = tk.Text(outer, width=34, height=14,
                               font=("TkFixedFont", 8), wrap="none")
        self.summary.grid(row=r, column=0, columnspan=2, sticky="nsew", pady=(6, 0))
        outer.rowconfigure(r, weight=1)

    def _buildPlot(self):
        right = ttk.Frame(self)
        right.grid(row=0, column=1, sticky="nsew")
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
            z=f(self.zVar, "Macroion z"),
            lb=f(self.lbVar, "Bjerrum length", lo=0.0),
            sigma1=f(self.sigma1Var, "Microion diameter", lo=1e-12),
            closure=self.closureVar.get(),
            pps=int(f(self.ppsVar, "Points per sigma", lo=20, hi=4000)),
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
            out = {"Q": Q, "sigma": sigma, "x": x, "lines": {}, "notes": []}

            #--- Gazzillo analytic reference -----------------------------
            if abs(p["z"]) > 0:
                sig, z, rho = GZ.primitiveModelSystem(
                    2.0*p["meanRadius"], p["z"], p["srel"], p["phi"],
                    sigma1=p["sigma1"], bjerrum=p["lb"])
                out["lines"]["Gazzillo MSA (charged)"] = \
                    GZ.measurableStructureFactor(Q, sig, z, rho, None, p["lb"])
                gam, _ = GZ.solveGamma(sig, z, rho, p["lb"])
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
            for tag, pps in ((("numerical (coarse)", p["pps"]),
                              ("numerical (fine)", 4*p["pps"]))
                             if abs(p["z"]) == 0 else ()):
                try:
                    out["lines"][tag] = self._numerical(Q, sigma, x, p, pps)
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

    def _numerical(self, Q, sigma, x, p, pps):
        """S_M(Q) from this package's solver, on explicit size classes."""
        from mixscatter_bridge import OZLiquidStructure

        class _Mix:                       # minimal MixtureLike
            pass
        m = _Mix()
        m.radius = sigma/2.0
        m.number_fraction = x
        gridN = 4095 if pps <= 100 else (16383 if pps <= 400 else 32767)
        oz = OZLiquidStructure(Q, m, volume_fraction_total=p["phi"],
                               closure=p["closure"], pointsPerSigma=pps,
                               gridN=gridN)
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
                txt += ["", f"coarse/fine ratio: {ratio:.2f}"]
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
            styles = {"mixscatter (Vrij PY)": dict(color="k", lw=2.0),
                      "Gazzillo MSA (neutral = PY)": dict(color="k", lw=2.0),
                      "Gazzillo MSA (charged)": dict(color="k", lw=2.0),
                      "numerical (coarse)": dict(color="C3", lw=1.0, ls="--"),
                      "numerical (fine)": dict(color="C0", lw=1.2)}
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
                        self.axErr.plot(Q, np.abs(y/base - 1.0), lw=1.0,
                                        label=f"{name} vs {ref}")
                if self.axErr.get_legend_handles_labels()[0]:
                    self.axErr.legend(fontsize=6.5)
            self.ax.legend(fontsize=7)
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
