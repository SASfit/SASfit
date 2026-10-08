#!/usr/bin/env python3
"""Which convention makes the vendored two-Yukawa agree with sasmodels?

    cd src/pyOZgui/tools
    python two_yukawa_convention_scan.py

A direct comparison differs by 24 to 62 per cent. Both implementations
descend from the same Yun Liu MATLAB, so a difference that large is a
convention mismatch rather than a disagreement about the physics -- the same
diagnosis that held for RMSA, where sasmodels computed the contact potential
and this package took the amplitude of the same Yukawa tail, quantities
differing by exp(kappa sigma).

Four things can differ here and arguing about which is slower than asking:

  * THE LENGTH UNIT. twoYukawa() takes a radius; CalTYSk works at diameter
    1 and therefore wants q*sigma. Passing radius = 0.5 makes the diameter
    1, which is the intended match -- but if either code reduces by the
    radius instead, q is wrong by two and everything shifts.
  * THE SIGN OF K. "Positive repulsive" is not universal, and the two-Yukawa
    literature is not consistent about it. Liu, Chen & Chen write the tail
    as -K exp(-Z(r-1))/r, so a code following that has K positive for
    ATTRACTION.
  * THE PAIRING. (K1,Z1) and (K2,Z2) must travel together; swapping one
    pair's K without its Z makes a different potential, and both codes
    reorder internally to keep Z1 > Z2.
  * Z SCALING, if one measures the decay in units of the radius and the
    other in diameters.

A candidate that wins at ONE state point is a coincidence. One that wins at
all four, across different phi and coupling, is the convention. That
distinction is the whole value of scanning rather than reasoning: the RMSA
case had a best-fitting factor that drifted from 2.2 to 3.6 across state
points, which is what finally ruled out a units explanation and sent us to
the original paper.

THIS IS A DIAGNOSTIC, not a test to keep. Once the convention is known,
two_yukawa_sasmodels_check.py should apply it with a comment saying what it
is and how it was established.
"""
import os
import sys

import numpy as np

_HERE = os.path.dirname(os.path.abspath(__file__))
if _HERE not in sys.path:
    sys.path.insert(0, _HERE)

import two_yukawa_reference as ref
from sasmodels.TwoYukawa.CalTYSk import (K_MIN, Z_MIN, Z_MIN_DIFF, CalTYSk)

CASES = (
    dict(name="authors_sample",  K1=6.0, K2=-1.0, Z1=10.0, Z2=2.0, phi=0.20),
    dict(name="phi_0.10",        K1=6.0, K2=-1.0, Z1=10.0, Z2=2.0, phi=0.10),
    dict(name="phi_0.35",        K1=6.0, K2=-1.0, Z1=10.0, Z2=2.0, phi=0.35),
    dict(name="weak_attraction", K1=2.0, K2=-1.0, Z1=10.0, Z2=2.0, phi=0.20),
)


def sasSk(qs, phi, K1, K2, Z1, Z2):
    if abs(K1) < K_MIN:
        K1 = -K_MIN if K1 < 0 else K_MIN
    if abs(K2) < K_MIN:
        K2 = -K_MIN if K2 < 0 else K_MIN
    Z1, Z2 = max(Z1, Z_MIN), max(Z2, Z_MIN)
    if abs(Z1 - Z2) < Z_MIN_DIFF:
        Z1 = Z2 + Z_MIN_DIFF
    if Z1 < Z2:
        Z1, Z2 = Z2, Z1
        K1, K2 = K2, K1
    out = CalTYSk(Z1, Z2, K1, K2, phi, np.asarray(qs, float),
                  warnFlag=False, debugFlag=False)
    return np.asarray(out[0] if isinstance(out, (tuple, list)) else out, float)


qs = np.linspace(0.05, 25.0, 300)

for c in CASES:
    print(f"\n=== {c['name']}  K1={c['K1']} K2={c['K2']} "
          f"Z1={c['Z1']} Z2={c['Z2']} phi={c['phi']}")
    #The sasmodels side is held fixed at the case as written; the scan is
    #over how the SAME system is expressed to the vendored routine.
    A = sasSk(qs, c["phi"], c["K1"], c["K2"], c["Z1"], c["Z2"])
    trials = (
        ("radius 0.5, as written      ", 0.5, c["K1"], c["K2"], c["Z1"], c["Z2"]),
        ("radius 1.0, as written      ", 1.0, c["K1"], c["K2"], c["Z1"], c["Z2"]),
        ("radius 0.5, K signs flipped ", 0.5, -c["K1"], -c["K2"], c["Z1"], c["Z2"]),
        ("radius 1.0, K signs flipped ", 1.0, -c["K1"], -c["K2"], c["Z1"], c["Z2"]),
        ("radius 0.5, pairs swapped   ", 0.5, c["K2"], c["K1"], c["Z2"], c["Z1"]),
        ("radius 0.5, Z halved        ", 0.5, c["K1"], c["K2"], c["Z1"]/2, c["Z2"]/2),
        ("radius 0.5, Z doubled       ", 0.5, c["K1"], c["K2"], c["Z1"]*2, c["Z2"]*2),
        ("radius 0.5, flip + swap     ", 0.5, -c["K2"], -c["K1"], c["Z2"], c["Z1"]),
    )
    best = []
    for label, R, K1, K2, Z1, Z2 in trials:
        try:
            B = np.asarray(ref.twoYukawa(qs, R, K1, K2, Z1, Z2, c["phi"]),
                           float)
            rel = float(np.max(np.abs(A - B))/np.max(np.abs(A)))
            print(f"   {label} rel {rel:.4e}")
            best.append((rel, label))
        except Exception as exc:
            print(f"   {label} -- {type(exc).__name__}")
    if best:
        best.sort()
        print(f"   BEST: {best[0][1].strip()}  rel {best[0][0]:.4e}")
