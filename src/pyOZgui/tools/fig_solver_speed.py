#!/usr/bin/env python3
"""Regenerate the solver speed comparison figure from benchmark.json.

    cd src/pyOZgui/tools
    python benchmark.py          # produces benchmark.json
    python fig_solver_speed.py   # draws from it

Writes docs/figures/fig_solver_speed.pdf.

WHY THIS READS benchmark.json RATHER THAN HOLDING ITS OWN NUMBERS. The
previous version of this figure had no generator at all, so it could not be
redrawn -- and it had gone stale in a way that was invisible until someone
looked: it showed an "Anderson" solver that is no longer registered, and
lacked MDIIS and the three SUNDIALS Newton-Krylov variants. Worse, its
timings predated the discovery that GenericPolydisperseSAS defaulted to
Picard while ozLib.solve() defaulted to KINSOL, so the numbers were measured
on a configuration no user had.

Reading the benchmark output means the figure is as current as the last
benchmark run, and a solver added to or removed from ozLib.SOLVER_CLASSES
appears or disappears here without anyone remembering to edit a list.

ON THE AXIS. The earlier figure used a log scale, which crowded the minor
tick labels into each other -- "2x10^1 3x10^1 4x10^1" overlapped into an
unreadable smear. A log axis is the wrong choice here anyway: the spread is
about tenfold, which linear handles comfortably. The value is printed at the
end of each bar as well, so the reader never has to measure against a tick
at all, and the tick labels become decoration rather than data.
"""
import json
import os
import sys

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

_HERE = os.path.dirname(os.path.abspath(__file__))
SRC = os.path.join(_HERE, "benchmark.json")
OUT = os.path.abspath(os.path.join(_HERE, os.pardir, "docs", "figures",
                                   "fig_solver_speed.pdf"))


def main():
    if not os.path.isfile(SRC):
        raise SystemExit(f"{SRC} not found -- run benchmark.py first")
    data = json.load(open(SRC))
    solvers = data.get("solvers") or {}
    rows = sorted(((k, v) for k, v in solvers.items()
                   if isinstance(v, (int, float))),
                  key=lambda kv: kv[1], reverse=True)
    if not rows:
        raise SystemExit("benchmark.json has no solver timings")

    names = [k.replace("sundials4py: ", "SUNDIALS ") for k, _ in rows]
    times = [v*1000.0 for _, v in rows]          # seconds -> ms

    fig, ax = plt.subplots(figsize=(7.2, 0.42*len(rows) + 1.1))
    #The fastest bar highlighted: it is the package default, and which solver
    #that is matters more to a reader than any individual timing.
    colours = ["#2166ac" if i == len(rows) - 1 else "#9ecae1"
               for i in range(len(rows))]
    bars = ax.barh(names, times, color=colours, height=0.65)

    for bar, t in zip(bars, times):
        ax.text(bar.get_width() + max(times)*0.015,
                bar.get_y() + bar.get_height()/2,
                f"{t:.1f}", va="center", fontsize=8.5, color="0.25")

    ax.set_xlabel("solve time (ms)")
    ax.set_xlim(0, max(times)*1.16)
    ax.spines[["top", "right"]].set_visible(False)
    ax.tick_params(axis="y", length=0)
    ax.set_axisbelow(True)
    ax.xaxis.grid(True, color="0.9")

    phi = data.get("solverStatePoint", "hard spheres, Percus-Yevick")
    ax.set_title(f"Solver speed, {phi}", fontsize=10)
    fig.tight_layout()
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    fig.savefig(OUT, bbox_inches="tight")
    print(f"wrote {OUT}", file=sys.stderr)


if __name__ == "__main__":
    main()
