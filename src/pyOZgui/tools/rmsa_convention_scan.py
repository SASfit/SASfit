#!/usr/bin/env python3
"""Find which parameter convention makes our RMSA agree with sasmodels.

    cd src/pyOZgui/tools
    python rmsa_convention_scan.py

A direct comparison disagrees by 20 to 36 per cent, growing with volume
fraction and charge -- the signature of a mismatched convention rather than
of two different physics. Three things could differ, and arguing about which
is slower than asking:

  * gamma: sasmodels writes the contact potential with sigma = DIAMETER and
    a squared denominator,
        gamma = beta (ze)^2 / (pi perm sigma (2 + kappa sigma)^2),
    while the expression jscatter documents uses the RADIUS and a first
    power. Those are not the same quantity, so a factor is plausible.
  * screening: rmsa_compute takes a screening LENGTH; some codes take
    kappa itself. Passing one for the other inverts the dependence.
  * radius vs diameter in the first argument.

So rather than reason about it, try the candidates and report which
reproduces the reference. A factor that works at one state point is a
coincidence; one that works at all three, across different phi and charge,
is the convention.

THIS IS A DIAGNOSTIC, not a test to keep. Once the convention is known,
rmsa_vs_sasmodels.py should apply it explicitly, with a comment saying what
it is and how it was established.
"""
import json
import os
import sys

import numpy as np

_HERE = os.path.dirname(os.path.abspath(__file__))
for _cand in (os.path.join(_HERE, os.pardir, "src", "pyozgui"), _HERE):
    _cand = os.path.abspath(_cand)
    if os.path.isfile(os.path.join(_cand, "rmsaWrapper.py")):
        if _cand not in sys.path:
            sys.path.insert(0, _cand)
        break

ref = json.load(open(os.path.join(_HERE, "rmsa_sasmodels_reference.json")))
q = np.asarray(ref["q"], float)
from rmsaWrapper import rmsa_compute


def tryOne(label, radius, screening, gamma, phi, expect):
    try:
        S, status, _ = rmsa_compute(radius, screening, gamma, phi, q)
        if status < 0:
            return label, None, f"status {status}"
        S = np.asarray(S, float)
        return label, float(np.max(np.abs(S - expect))/np.max(np.abs(expect))), ""
    except Exception as exc:
        return label, None, type(exc).__name__


for p in ref["points"]:
    expect = np.asarray(p["S"], float)
    D = p["diameterA"]
    R = D/2.0
    lD = p["screeningLengthA"]          # 1/kappa, in Angstrom
    kap = 1.0/lD
    g = p["gamma"]
    phi = p["phi"]
    print(f"\n=== d={D} phi={phi} z={p['charge']}  "
          f"(kappa*sigma={p['kappaSigma']:.3f}, gamma={g:.3f})")
    trials = [
        ("radius, 1/kappa, gamma          ", R,  lD,  g),
        ("radius, 1/kappa, gamma/2        ", R,  lD,  g/2),
        ("radius, 1/kappa, gamma*2        ", R,  lD,  g*2),
        ("radius, 1/kappa, gamma*4        ", R,  lD,  g*4),
        ("radius, 1/kappa, gamma/4        ", R,  lD,  g/4),
        ("DIAMETER, 1/kappa, gamma        ", D,  lD,  g),
        ("radius, kappa (not 1/k), gamma  ", R,  kap, g),
        ("radius, 1/kappa, gamma*(2+ks)   ", R,  lD,  g*(2+p["kappaSigma"])),
    ]
    best = []
    for label, r_, s_, g_ in trials:
        lbl, rel, note = tryOne(label, r_, s_, g_, phi, expect)
        best.append((rel if rel is not None else 9e9, lbl, note))
        shown = f"{rel:.4e}" if rel is not None else f"-- {note}"
        print(f"   {lbl} rel {shown}")
    best.sort()
    print(f"   BEST: {best[0][1].strip()}  rel {best[0][0]:.4e}")
