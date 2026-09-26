#!/usr/bin/env python3
"""Regenerate Fig. 3: spurious oscillations from too few form-factor classes.

    cd src/pyOZgui/tools
    python fig3_oscillation.py

Writes docs/manuscript/figures/fig3_oscillation.pdf.

WHAT IT SHOWS, and why the figure earns its place. The structure factor and
the form factor average need DIFFERENT numbers of size classes, and the
figure is the argument for that.

S(Q) is smooth in sigma, so a handful of moment-matched classes represent it
well -- that is what Fig. 1 is about. |F(Q)|^2 oscillates, and its zeros move
with sigma, so averaging it over the same handful superposes five ringing
curves instead of smoothing them into an envelope. The result is not a small
error: it is structure that is not there, at Q values where a fitter will
happily attribute it to the interaction.

Three curves, following the caption:

  * direct integration with 4000 points -- the answer, essentially smooth;
  * the same five classes used for the structure factor -- five superposed
    form factors, visibly wrong;
  * sixty classes for the form-factor average alone -- envelope restored.

The rule of thumb it supports is in the interface's own hover help:
classes(form factor) >~ Qmax * sigma * s.
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
                   "fig3_oscillation.pdf")
SREL = 0.30
QMAX = 25.0


def sphereF2(q, R):
    """|F(q)|^2 for a sphere of radius R, volume-weighted.

    Written out rather than imported so the figure does not depend on which
    form-factor class happens to be current: the point being made is about
    the AVERAGE, not about this particular F.
    """
    x = np.outer(q, R)
    j1 = np.where(x < 1e-8, x/3.0, (np.sin(x) - x*np.cos(x))/np.maximum(x, 1e-300)**2)
    V = 4.0/3.0*np.pi*R**3
    return (3.0*V*j1/np.maximum(x, 1e-300))**2


def schulzPdf(x, srel, mean=1.0):
    from scipy.special import gammaln
    z = 1.0/(srel*srel) - 1.0
    a = (z + 1.0)/mean
    return np.exp((z + 1.0)*np.log(a) + z*np.log(np.maximum(x, 1e-300))
                  - a*x - gammaln(z + 1.0))


def averaged(q, sig, w):
    """Weight-averaged |F|^2 over the given classes."""
    return sphereF2(q, sig) @ w


def main():
    q = np.linspace(0.05, QMAX, 1400)

    #DIRECT INTEGRATION, the reference. 4000 points on a fine grid over the
    #support, trapezoid -- no quadrature cleverness, so it cannot inherit the
    #artefact being demonstrated.
    #
    #The normalisation is easy to get wrong and the error is not obvious: a
    #first version normalised the density to unit integral AND then
    #multiplied the sum by the grid spacing, so the reference sat a factor of
    #60 above the two quadrature curves. The shapes were right, so the figure
    #still made its point while being quantitatively meaningless -- which is
    #the kind of plot that gets published. All three curves must coincide as
    #Q -> 0, since every discretisation reproduces the zeroth moment.
    Rg = np.linspace(1e-4, 3.5, 4000)
    pdf = schulzPdf(Rg, SREL)
    trapz = np.trapezoid if hasattr(np, "trapezoid") else np.trapz
    pdf = pdf/trapz(pdf, Rg)
    direct = trapz(sphereF2(q, Rg)*pdf[None, :], Rg, axis=1)

    sig5, w5 = P.sizeClasses("Schulz", SREL, 5, meanSigma=1.0)
    sig60, w60 = P.sizeClasses("Schulz", SREL, 60, meanSigma=1.0)
    #BOTH ROUTES MUST DESCRIBE THE SAME PARTICLES. The direct integration
    #above is over a Schulz distribution of mean 1 in the variable passed to
    #sphereF2, i.e. mean RADIUS 1. sizeClasses with meanSigma = 1 returns
    #nodes of mean 1 in the same variable, so they are used as they come.
    #
    #Halving them -- on the reasoning that sizeClasses returns diameters --
    #made the two distributions differ by a factor of two in radius, and
    #since F^2 goes as V^2 and so as R^6 the curves sat 2^6 = 64 apart. The
    #shapes were unaffected, so the figure still made its point while being
    #quantitatively meaningless, and only the Q -> 0 check below caught it.
    #If a future version needs diameters, change BOTH routes together.
    five = averaged(q, sig5, w5)
    sixty = averaged(q, sig60, w60)

    #The three must agree at Q -> 0. If they do not, the normalisation is
    #wrong rather than the quadrature, and the figure would be making its
    #point with numbers that do not mean anything.
    ratios = (five[0]/direct[0], sixty[0]/direct[0])
    if not all(abs(r - 1.0) < 0.05 for r in ratios):
        print(f"WARNING: curves disagree at Q -> 0 by {ratios} -- "
              f"normalisation, not quadrature", file=sys.stderr)

    fig, ax = plt.subplots(figsize=(6.0, 4.0))
    n = direct[0]
    ax.semilogy(q, direct/n, "-", color="0.15", lw=1.8, zorder=3,
                label="direct integration, 4000 points")
    ax.semilogy(q, five/n, "-", color="#d7191c", lw=1.2, zorder=2,
                label="5 classes (as used for $S(Q)$)")
    ax.semilogy(q, sixty/n, "--", color="#2166ac", lw=1.4, zorder=4,
                label="60 classes, form factor only")
    ax.set_xlabel(r"$Q\,\langle R \rangle$")
    ax.set_ylabel(r"$\langle |F(Q)|^2 \rangle$, normalised")
    ax.set_xlim(0, QMAX)
    ax.set_ylim(direct.min()/n*0.3, 2.0)
    #LEGEND LOWER LEFT. The curves decay from upper left to lower right and
    #the five-class artefact is worst at large Q, so the lower-left corner is
    #the only quadrant that stays empty; "best" placed it over the first
    #minimum, which is exactly where the three curves first separate.
    ax.legend(loc="lower left", fontsize=8, framealpha=0.92)
    ax.set_title(f"Schulz, $s = {SREL}$, dilute limit", fontsize=9)
    fig.tight_layout()
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    fig.savefig(OUT, bbox_inches="tight")
    print(f"wrote {os.path.abspath(OUT)}", file=sys.stderr)


if __name__ == "__main__":
    main()
