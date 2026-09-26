#!/usr/bin/env python3
"""Regenerate Fig. 1: moment-matched discretisation of a Schulz distribution.

    cd src/pyOZgui/tools
    python fig1_discretisation.py

Writes docs/manuscript/figures/fig1_discretisation.pdf.

WHY THIS SCRIPT EXISTS. The figure was in the manuscript and its generator
was not: the PDF sat in figures/ with nothing in the repository able to
reproduce it. That is the same problem as a validation row measured against
software that will not build here -- a result the authors cannot re-run, and
a referee asking for any change to it would find there is nothing to change.

fig_survey.pdf names its generator in its own caption. This one now can too.

WHAT IT SHOWS. A Schulz distribution of relative width s = 0.25, with the
nodes and weights of the generalised Gauss-Laguerre rule that represents it
at three classes (left) and five (right). The point of the figure is that
three nodes are not a crude histogram of the curve: a p-point rule reproduces
the first 2p-1 moments EXACTLY, which is why so few classes suffice where
naive binning needs dozens. The moments are printed on each panel so the
claim is visible rather than asserted.
"""
import os
import sys

import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

_HERE = os.path.dirname(os.path.abspath(__file__))
for _cand in (os.path.join(_HERE, os.pardir, "src", "pyozgui"),
              os.path.join(_HERE, os.pardir), _HERE):
    _cand = os.path.abspath(_cand)
    if os.path.isfile(os.path.join(_cand, "polydisperse_nodes.py")):
        if _cand not in sys.path:
            sys.path.insert(0, _cand)
        break
else:
    raise ImportError("cannot locate polydisperse_nodes.py")

import polydisperse_nodes as P

OUT = os.path.join(_HERE, os.pardir, "docs", "manuscript", "figures",
                   "fig1_discretisation.pdf")
SREL = 0.25


def schulzPdf(x, srel, mean=1.0):
    """Schulz (gamma) density, mean `mean`, relative width `srel`."""
    from scipy.special import gammaln
    z = 1.0/(srel*srel) - 1.0
    a = (z + 1.0)/mean
    return np.exp((z + 1.0)*np.log(a) + z*np.log(np.maximum(x, 1e-300))
                  - a*x - gammaln(z + 1.0))


def main():
    x = np.linspace(1e-6, 2.6, 2000)
    pdf = schulzPdf(x, SREL)

    fig, axes = plt.subplots(1, 2, figsize=(9.2, 3.5), sharey=True)
    for ax, p in zip(axes, (3, 5)):
        #sizeClasses, NOT quantileClasses.
        #
        #The module offers both, and only one matches this figure's caption.
        #sizeClasses is the moment-matched generalised Gauss-Laguerre rule --
        #at p = 3 and p = 5 it returns <sigma> = 1.0000000000 and
        #<sigma^2> = 1.0625000000, which is exactly 1 + s^2 for s = 0.25.
        #quantileClasses is quantile-based and gives 0.9794 and 0.9596 at
        #p = 3: a perfectly good discretisation, but not the one the caption
        #describes and not one that reproduces moments exactly.
        #
        #Worth stating because the figure's whole claim is that a p-point
        #rule is exact to 2p-1 moments. Generated from the wrong routine it
        #would sit beside a caption asserting something it does not show.
        sig, w = P.sizeClasses("Schulz", SREL, p, meanSigma=1.0)
        ax.plot(x, pdf, "-", color="0.35", lw=1.6, zorder=1,
                label=f"Schulz, $s = {SREL}$")
        #Stems scaled to the curve so the two are readable together. The
        #weights are probabilities and the curve is a density, so they have
        #different units and any common axis is a presentational choice --
        #stated in the label rather than left for the reader to infer.
        scale = pdf.max()/w.max()
        ax.vlines(sig, 0, w*scale, color="#d7191c", lw=1.8, zorder=3)
        ax.plot(sig, w*scale, "o", color="#d7191c", ms=5, zorder=4,
                label=f"{p} nodes (weights, scaled)")
        #The moments, which are the actual claim of the figure.
        exact = [1.0, 1.0 + SREL**2]
        got = [float(np.sum(w*sig)), float(np.sum(w*sig**2))]
        ax.text(0.97, 0.72,
                f"$\\langle\\sigma\\rangle$ = {got[0]:.6f}\n"
                f"$\\langle\\sigma^2\\rangle$ = {got[1]:.6f}\n"
                f"exact: {exact[0]:.6f}, {exact[1]:.6f}",
                transform=ax.transAxes, ha="right", va="top", fontsize=7.5,
                family="monospace", color="0.25")
        ax.set_xlabel(r"$\sigma / \langle\sigma\rangle$")
        ax.set_xlim(0, 2.6)
        ax.set_ylim(0, pdf.max()*1.28)
        #LEGEND PLACED DELIBERATELY. A Schulz density at s = 0.25 peaks left
        #of centre and decays to the right, so the upper RIGHT is the empty
        #quadrant in both panels -- "best" put it over the peak in one and
        #over the tail in the other, which is why the two panels disagreed.
        #Fixing it explicitly also keeps them consistent with each other.
        ax.legend(loc="upper right", fontsize=8, framealpha=0.92,
                  borderpad=0.4, handlelength=1.6)
        ax.set_title(f"$p = {p}$ classes", fontsize=9)
    axes[0].set_ylabel("probability density")
    fig.tight_layout()
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    fig.savefig(OUT, bbox_inches="tight")
    print(f"wrote {os.path.abspath(OUT)}", file=sys.stderr)


if __name__ == "__main__":
    main()
