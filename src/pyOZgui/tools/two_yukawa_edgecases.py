#!/usr/bin/env python3
"""The two two-Yukawa cases that do NOT agree to 1e-8, examined.

    cd src/pyOZgui/tools
    python two_yukawa_edgecases.py

With the K-sign convention corrected, sasmodels and the vendored reference
agree to 2e-8, 7e-8 and 3e-8 on authors_sample, phi_0.35 and
weak_attraction. Two cases behave differently:

  phi_0.10        the vendored reference reports "no root found"
  close_screening the two agree only to 2e-3, a thousand times worse than
                  the rest but a thousand times better than a wrong
                  convention

Neither is a convention problem -- the convention is settled by the three
that agree at 1e-8. Both sit where this package's own notes already record
the reference misbehaving: close_screening (Z1 = 6, Z2 = 4) is listed there
as the case where it "refuses (no physical root)", and the two-Yukawa
equations admit several roots whose selection is delicate when Z1 and Z2 are
close.

WHAT THIS DECIDES. If the vendored copy is to be retired in favour of
sasmodels -- BSD-licensed, installable, no citation-ware grant to explain --
then these are the cases where that swap changes an answer, and they should
be understood rather than waved through. The question for each is not
"which is closer to the other" but "is the result PHYSICAL": S(q) >= 0
everywhere, and S -> 1 at large q. A code that returns a physical answer
where the other refuses is the better one, whatever the agreement statistic
says.
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
    dict(name="phi_0.10",        K1=6.0, K2=-1.0, Z1=10.0, Z2=2.0, phi=0.10),
    dict(name="close_screening", K1=6.0, K2=-1.0, Z1=6.0,  Z2=4.0, phi=0.20),
    #for contrast: one that agrees to 1e-8, so the diagnostics below have a
    #known-good row to be read against
    dict(name="authors_sample",  K1=6.0, K2=-1.0, Z1=10.0, Z2=2.0, phi=0.20),
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


def verdict(S, qs):
    """Is this a physical structure factor?"""
    if S is None:
        return "refused"
    if not np.all(np.isfinite(S)):
        return "NOT FINITE"
    bad = []
    if S.min() < -1e-9:
        bad.append(f"min S = {S.min():.4g} < 0")
    tail = S[qs > 0.8*qs.max()]
    if abs(float(np.mean(tail)) - 1.0) > 0.05:
        bad.append(f"S -> {np.mean(tail):.4g} at large q, not 1")
    return "physical" if not bad else "UNPHYSICAL: " + "; ".join(bad)


qs = np.linspace(0.05, 25.0, 400)
print(f"{'case':18s} {'implementation':12s} {'minS':>9s} {'maxS':>9s} "
      f"{'S(large q)':>11s}  verdict")
for c in CASES:
    try:
        A = sasSk(qs, c["phi"], c["K1"], c["K2"], c["Z1"], c["Z2"])
    except Exception as exc:
        A = None
        print(f"{c['name']:18s} {'sasmodels':12s} {type(exc).__name__}")
    #K signs flipped: the vendored routine takes positive K as REPULSIVE
    #where Liu, Chen & Chen (and sasmodels) take it as attractive.
    try:
        B = np.asarray(ref.twoYukawa(qs, 0.5, -c["K1"], -c["K2"],
                                     c["Z1"], c["Z2"], c["phi"]), float)
        #THE VENDORED ROUTINE DOES NOT RAISE WHEN IT FAILS. It prints
        #"Output TY_SolveEquation: no root found" to stdout and returns
        #something 0-dimensional, so a caller checking only for exceptions
        #sails past it and crashes later on an indexing error three frames
        #away. Treat a wrong shape as the refusal it is.
        if B.shape != qs.shape:
            B = None
    except Exception:
        B = None
    for lab, S in (("sasmodels", A), ("vendored", B)):
        if S is None:
            print(f"{c['name']:18s} {lab:12s} {'--':>9s} {'--':>9s} "
                  f"{'--':>11s}  refused")
            continue
        tail = float(np.mean(S[qs > 0.8*qs.max()]))
        print(f"{c['name']:18s} {lab:12s} {S.min():9.5f} {S.max():9.5f} "
              f"{tail:11.5f}  {verdict(S, qs)}")
    if A is not None and B is not None:
        rel = float(np.max(np.abs(A - B))/np.max(np.abs(A)))
        where = qs[int(np.argmax(np.abs(A - B)))]
        print(f"{'':18s} {'-> rel':12s} {rel:9.2e}  largest at q*sigma "
              f"= {where:.3f}")
    print()
