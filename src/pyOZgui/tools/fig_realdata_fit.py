#!/usr/bin/env python3
"""Draw the measured-data fit from a saved .oz1 session.

    cd src/pyOZgui/tools
    python fig_realdata_fit.py <session.oz1>

Writes fig_realdata_fit.pdf into BOTH figure directories.

WHY THIS EXISTS. The manuscript reports a fit to real data whose volume
fraction agrees with an independent measurement to within 0.8 sigma, and
until now illustrated it with a GUI screenshot of SYNTHETIC data -- the
screenshot generator makes its own curve from the model. A paper that
reports a real-data validation and shows a made-up curve invites exactly the
question it should be answering.

Reads the session file rather than refitting, so the figure shows the fit
that is actually being described, down to the last digit, and cannot drift
from it. The .oz1 carries the measured Q, I, dI and dQ alongside the fitted
curve and every fitted parameter.

ON THE RESIDUAL PANEL. Plotted as (I_fit - I_data)/dI, so the eye reads it
against the error bars rather than against the intensity -- which matters
here, because the reduced chi-squared of 6.5 comes mostly from a merge
between two instrument geometries aligned by eye. On this panel that shows
up as a step in the middle of the Q range rather than as scatter, and a
reader can see for themselves that the model is not the problem. On a
log-intensity plot the same defect is invisible.
"""
import json
import os
import sys

import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

_HERE = os.path.dirname(os.path.abspath(__file__))
OUTDIRS = [
    os.path.abspath(os.path.join(_HERE, os.pardir, "docs", "manuscript",
                                 "figures")),
    os.path.abspath(os.path.join(_HERE, os.pardir, "docs", "figures")),
]


def _arr(x):
    """Unwrap the {'__ndarray__': [...]} form the session writer uses."""
    if isinstance(x, dict) and "__ndarray__" in x:
        return np.asarray(x["__ndarray__"], float)
    return np.asarray(x, float)


def main(path):
    sess = json.load(open(path))["session"]
    data, fit = sess["data"], sess["fit"]

    Q = _arr(data["Q"])
    I = _arr(data["I"])
    dI = _arr(data["dI"]) if data.get("dI") is not None else None
    Qf = _arr(fit["Q"])
    If = _arr(fit["fit"])
    par = fit.get("parameters") or {}
    chi2 = fit.get("chi2_reduced")

    fig, (ax, axr) = plt.subplots(
        2, 1, figsize=(6.4, 5.6), sharex=True,
        gridspec_kw={"height_ratios": [3, 1], "hspace": 0.06})

    if dI is not None:
        ax.errorbar(Q, I, yerr=dI, fmt="o", ms=2.6, lw=0, elinewidth=0.6,
                    color="0.35", label="measured", zorder=2)
    else:
        ax.plot(Q, I, "o", ms=2.6, color="0.35", label="measured", zorder=2)
    ax.plot(Qf, If, "-", lw=1.6, color="#c0392b", label="fit", zorder=3)
    ax.set_xscale("log")
    ax.set_yscale("log")
    ax.set_ylabel(r"$I(Q)$")
    ax.spines[["top", "right"]].set_visible(False)

    #The numbers the text quotes, on the figure, so the two cannot drift.
    bits = []
    if "phi" in par:
        bits.append(rf"$\varphi_{{\rm core}} = {par['phi']:.3f}$")
    if "meanRadius" in par:
        bits.append(rf"$\langle R\rangle = {par['meanRadius']:.1f}$")
    if "srel" in par:
        bits.append(rf"$s_{{\rm rel}} = {par['srel']:.3f}$")
    if chi2 is not None:
        bits.append(rf"$\chi^2_{{\rm red}} = {chi2:.1f}$")
    ax.text(0.02, 0.04, "\n".join(bits), transform=ax.transAxes,
            fontsize=8.5, va="bottom", color="0.25")
    ax.legend(frameon=False, fontsize=9, loc="upper right")

    #RESIDUALS IN UNITS OF THE ERROR BAR. The interesting structure is the
    #merge step, and it is only visible against dI.
    if dI is not None and Qf.shape == Q.shape:
        r = (If - I)/np.where(dI > 0, dI, np.nan)
        axr.axhline(0, color="0.75", lw=0.8)
        axr.plot(Q, r, "o", ms=2.4, color="0.35")
        axr.set_ylabel(r"$(I_{\rm fit}-I)/\sigma$", fontsize=9)
        lim = np.nanpercentile(np.abs(r), 99)*1.2
        if np.isfinite(lim) and lim > 0:
            axr.set_ylim(-lim, lim)
    axr.set_xlabel(r"$Q$  [1/length]")
    axr.spines[["top", "right"]].set_visible(False)

    for d in OUTDIRS:
        os.makedirs(d, exist_ok=True)
        out = os.path.join(d, "fig_realdata_fit.pdf")
        fig.savefig(out, bbox_inches="tight")
    print(f"wrote fig_realdata_fit.pdf to {len(OUTDIRS)} dir(s)",
          file=sys.stderr)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        raise SystemExit("usage: fig_realdata_fit.py <session.oz1>")
    main(sys.argv[1])
