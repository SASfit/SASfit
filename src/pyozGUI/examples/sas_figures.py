# -*- coding: utf-8 -*-
"""Figures for the polydisperse hard-sphere-Yukawa SAS layer."""
import numpy as np, warnings
warnings.filterwarnings("ignore")
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from polydisperse_yukawa_sas import (LogNormal, SchulzZimm, Sphere, CoreShell,
                                    PolydisperseYukawaSAS, discretize, moment_report)

# validated categorical palette (dataviz skill reference instance)
C = ["#2a78d6", "#eb6834", "#1baf7a", "#eda100", "#e87ba4", "#4a3aa7"]
INK, INK2, GRID = "#0b0b0b", "#52514e", "#d8d7d2"

plt.rcParams.update({
    "figure.facecolor": "#fcfcfb", "axes.facecolor": "#fcfcfb",
    "axes.edgecolor": GRID, "axes.labelcolor": INK, "text.color": INK,
    "xtick.color": INK2, "ytick.color": INK2, "font.size": 10,
    "axes.grid": True, "grid.color": GRID, "grid.linewidth": 0.6,
    "axes.spines.top": False, "axes.spines.right": False,
    "lines.linewidth": 2.0, "legend.frameon": False,
})

R0, PHI, ZS, K = 50.0, 0.20, 6.0, 1.0
SIG = 2*R0
Z = ZS/SIG


def fig_distribution(path="fig1_distribution.png"):
    fig, ax = plt.subplots(1, 2, figsize=(11, 3.8))
    for j, (d, lab) in enumerate([(LogNormal(R0, 0.30), "log-normal, rel.sd 0.31"),
                                  (SchulzZimm(R0, relsd=0.30), "Schulz-Zimm, rel.sd 0.30")]):
        R, w = discretize(d, 16)
        Rg = np.linspace(max(1.0, R.min()*0.7), R.max()*1.1, 600)
        ax[j].plot(Rg, d.pdf(Rg), color=C[0], label="continuous $f(R)$")
        ax[j].vlines(R, 0, w/np.gradient(R), color=C[1], lw=1.4, alpha=0.9)
        ax[j].plot(R, w/np.gradient(R), "o", ms=5, color=C[1],
                   label=f"{len(R)}-bin quadrature")
        worst = max(v[2] for v in moment_report(d, R, w).values())
        ax[j].set_title(f"{lab}\nworst moment error {worst:.1e}", fontsize=10)
        ax[j].set_xlabel("radius $R$ (Å)")
        ax[j].set_ylabel("$f(R)$")
        ax[j].legend()
    fig.tight_layout(); fig.savefig(path, dpi=160); plt.close(fig)
    return path


def fig_partials(path="fig2_partials.png"):
    m = PolydisperseYukawaSAS(LogNormal(R0, 0.25), PHI, Z, K, Sphere(), nbins=6)
    Q = np.geomspace(2e-3, 0.15, 220)
    S = m.S_partials(Q)
    fig, ax = plt.subplots(1, 2, figsize=(11, 3.8))
    pick = [(0, 0), (0, 5), (5, 5), (2, 3)]
    for n, (i, j) in enumerate(pick):
        ax[0].plot(Q, S[:, i, j], color=C[n],
                   label=f"$S_{{{i+1}{j+1}}}$  ($\\sigma$={m.sigma[i]:.0f}, {m.sigma[j]:.0f} Å)")
    ax[0].axhline(0, color=GRID, lw=1)
    ax[0].set_xscale("log"); ax[0].set_xlabel("$Q$ (Å$^{-1}$)")
    ax[0].set_ylabel("$S_{ij}^{\\rm AL}(Q)$")
    ax[0].set_title("partial structure factors (6 bins shown)", fontsize=10)
    ax[0].legend(fontsize=8)

    for n, s in enumerate([0.02, 0.15, 0.25, 0.35]):
        mm = PolydisperseYukawaSAS(LogNormal(R0, s), PHI, Z, K, Sphere(), nbins=14)
        ax[1].plot(Q, mm.S_number(Q), color=C[n],
                   label=f"rel.sd {np.sqrt(np.exp(s**2)-1):.2f}")
    ax[1].set_xscale("log"); ax[1].set_xlabel("$Q$ (Å$^{-1}$)")
    ax[1].set_ylabel("$S_N(Q)$")
    ax[1].set_title("number-number $S(Q)$ vs polydispersity", fontsize=10)
    ax[1].legend(fontsize=9)
    fig.tight_layout(); fig.savefig(path, dpi=160); plt.close(fig)
    return path


def fig_intensity(path="fig3_intensity.png"):
    m = PolydisperseYukawaSAS(LogNormal(R0, 0.25), PHI, Z, K, Sphere(), nbins=18)
    Q = np.geomspace(2e-3, 0.13, 160)
    Ie, Id = m.I_exact(Q), m.I_dilute(Q)
    Ida = m.I_decoupling(Q)
    Il = m.I_lma(Q)
    fig, ax = plt.subplots(2, 1, figsize=(7.2, 6.4), sharex=True,
                           gridspec_kw={"height_ratios": [2.4, 1]})
    ax[0].plot(Q, Ie, color=C[0], label="exact  $\\Sigma_{ij}\\sqrt{n_in_j}F_iF_jS_{ij}$")
    ax[0].plot(Q, Ida, color=C[1], ls="--", label="decoupling approx.")
    ax[0].plot(Q, Il, color=C[2], ls="-.", label="local monodisperse approx.")
    ax[0].plot(Q, Id, color=INK2, lw=1.2, ls=":", label="dilute ($\\phi\\to0$)")
    ax[0].set_xscale("log"); ax[0].set_yscale("log")
    ax[0].set_ylabel("$I(Q)$  (arb.)")
    ax[0].set_title(f"log-normal rel.sd 0.25, $\\phi$={PHI}, $Z$={ZS}, $K$={K}", fontsize=10)
    ax[0].legend(fontsize=9)
    ax[1].axhline(1, color=INK2, lw=1)
    ax[1].plot(Q, Ida/Ie, color=C[1], ls="--", label="decoupling / exact")
    ax[1].plot(Q, Il/Ie, color=C[2], ls="-.", label="local mono. / exact")
    ax[1].set_xscale("log"); ax[1].set_xlabel("$Q$ (Å$^{-1}$)")
    ax[1].set_ylabel("ratio to exact")
    ax[1].legend(fontsize=9)
    fig.tight_layout(); fig.savefig(path, dpi=160); plt.close(fig)
    return path


def _err(Ie, Ia, frac=0.01):
    """Error metric restricted to the Q range that carries signal: I_exact
    above `frac` of its low-Q value. A plain max-relative-error over all Q is
    dominated by the form-factor minima, where I -> 0 and any ratio explodes --
    and those minima are DEEPEST at low polydispersity, so that metric shows a
    spurious error floor exactly where both approximations should become exact."""
    m = Ie > frac*Ie[0]
    return np.max(np.abs(Ia[m]/Ie[m] - 1.0))


def fig_approx_error(path="fig4_approx_error.png"):
    """All six approaches SASfit implements, against the exact result.
    Formulas transcribed from SASfit's own manual, doc/manual/SASfit_ch3.tex."""
    Q = np.geomspace(3e-3, 0.10, 30)
    ss = [0.01, 0.03, 0.06, 0.10, 0.15, 0.20, 0.25, 0.30, 0.35]
    methods = [("monodisperse", "I_monodisperse", C[5], "v"),
               ("decoupling (Kotlarchyk-Chen)", "I_decoupling", C[1], "o"),
               ("local monodisperse (Pedersen)", "I_lma", C[2], "s"),
               ("partial structure factors", "I_partial_sf", C[0], "D"),
               ("scaling (Gazzillo)", "I_scaling", C[3], "^"),
               ("van der Waals one-fluid", "I_vdw1", C[4], "P")]
    relsd, errs = [], {n: [] for n, _, _, _ in methods}
    for s in ss:
        m = PolydisperseYukawaSAS(LogNormal(R0, s), PHI, Z, K, Sphere(), nbins=12)
        Ie = m.I_exact(Q)
        for n, fn, _, _ in methods:
            try:
                errs[n].append(100*_err(Ie, getattr(m, fn)(Q)))
            except Exception:
                errs[n].append(np.nan)
        relsd.append(np.sqrt(np.exp(s**2) - 1))

    fig, ax = plt.subplots(figsize=(8.4, 5.0))
    for n, _, col, mk in methods:
        # "partial structure factors" lands almost exactly on top of
        # "decoupling" -- that coincidence IS the result, so draw it dashed and
        # label it rather than letting one series silently hide the other.
        dashed = n.startswith("partial")
        ax.plot(relsd, errs[n], mk + ("--" if dashed else "-"), ms=6,
                color=col, label=n, lw=2.0,
                mfc="none" if dashed else col, zorder=3 if dashed else 2)
    ax.annotate("partial structure factors tracks decoupling:\nboth still use the"
                " MONODISPERSE $S$ at the\nmean radius, not the true $S_{ij}$",
                xy=(0.205, 70), xytext=(0.115, 260), fontsize=8, color=INK2,
                ha="left", va="top",
                arrowprops=dict(arrowstyle="-", color=INK2, lw=0.8))
    ax.set_yscale("log")
    ax.set_xlabel("number-weighted relative standard deviation of $R$")
    ax.set_ylabel("worst error in $I(Q)$  (%),\nover $Q$ carrying signal")
    ax.set_title("All six SASfit structure-factor approaches vs the exact "
                 f"$S_{{ij}}$\n$\\phi$={PHI}, $Z$={ZS}, $K$={K}, log-normal sizes",
                 fontsize=10)
    ax.legend(fontsize=8.5, ncol=2)
    fig.tight_layout(); fig.savefig(path, dpi=160); plt.close(fig)
    return path


if __name__ == "__main__":
    for f in (fig_distribution, fig_partials, fig_intensity, fig_approx_error):
        print("wrote", f())


def fig_three_interactions(path="fig5_three_interactions.png"):
    """SASfit's six approaches scored against an exact S_ij, for three
    different interactions: attractive Yukawa (K>0), repulsive Yukawa (K<0), and the
    Robertus multicomponent sticky-hard-sphere model (SASfit's own C engine).
    The approximation code is byte-identical across all three
    (PolydisperseSASBase), so differences are physics, not implementation."""
    from robertus_shs_sas import RobertusSHSSAS
    Qs = np.geomspace(3e-3, 0.10, 30)
    ss = [0.01, 0.03, 0.06, 0.10, 0.15, 0.20, 0.25, 0.30]
    methods = [("monodisperse", "I_monodisperse", C[5], "v", "-"),
               ("decoupling", "I_decoupling", C[1], "o", "-"),
               ("local monodisperse", "I_lma", C[2], "s", "-"),
               ("partial structure factors", "I_partial_sf", C[0], "D", "--"),
               ("scaling (Gazzillo)", "I_scaling", C[3], "^", "-"),
               ("van der Waals one-fluid", "I_vdw1", C[4], "P", "-")]

    def build(kind, s):
        if kind == "rep":
            return PolydisperseYukawaSAS(LogNormal(R0, s), PHI, Z, 1.0, Sphere(), nbins=12)
        if kind == "att":
            return PolydisperseYukawaSAS(LogNormal(R0, s), PHI, Z, -1.0, Sphere(), nbins=12)
        return RobertusSHSSAS(2*R0, np.sqrt(np.exp(s**2) - 1), PHI, 0.2, Sphere(), nbins=12)

    panels = [("rep", "attractive Yukawa\n$K=+1$, $Z=6$"),
              ("att", "repulsive Yukawa\n$K=-1$, $Z=6$"),
              ("shs", "sticky hard spheres\n(Robertus, $\\tau=0.2$)")]
    fig, axes = plt.subplots(1, 3, figsize=(13.5, 4.6), sharey=True)
    for ax, (kind, title) in zip(axes, panels):
        relsd, errs = [], {n: [] for n, _, _, _, _ in methods}
        for s in ss:
            try:
                m = build(kind, s)
                Ie = m.I_exact(Qs)
            except Exception:
                for n, _, _, _, _ in methods:
                    errs[n].append(np.nan)
                relsd.append(np.sqrt(np.exp(s**2) - 1))
                continue
            for n, fn, _, _, _ in methods:
                try:
                    errs[n].append(100*_err(Ie, getattr(m, fn)(Qs)))
                except Exception:
                    errs[n].append(np.nan)
            relsd.append(np.sqrt(np.exp(s**2) - 1))
        for n, _, col, mk, ls in methods:
            ax.plot(relsd, errs[n], mk + ls, ms=5.5, color=col, label=n, lw=1.9,
                    mfc="none" if ls == "--" else col, zorder=3 if ls == "--" else 2)
        ax.set_yscale("log")
        ax.set_title(title, fontsize=10)
        ax.set_xlabel("rel. s.d. of $R$")
    axes[0].set_ylabel("worst error in $I(Q)$  (%),\nover $Q$ carrying signal")
    axes[0].legend(fontsize=8, loc="lower right")
    fig.suptitle("SASfit's six structure-factor approaches vs an exact $S_{ij}$, "
                 f"three interactions  ($\\phi$={PHI})", fontsize=11)
    fig.tight_layout(rect=(0, 0, 1, 0.94))
    fig.savefig(path, dpi=160); plt.close(fig)
    return path
