# -*- coding: utf-8 -*-
"""
Task 10 diagnostic: extract the N=1 contact value g(sigma+) from SASfit's own
liboneyukawa.so *without* any FFT/real-space grid, via the large-q asymptotic
tail of S(Q), and compare it against polydisperse_yukawa_msa.py's analytic
contact_value().

Why not just FFT-invert the library's own S(Q) (via its PairCorrelation)?
Tried that first -- even at K->0 (exact PY answer known) it misses by ~2.6%,
diagnosed as Gibbs ringing / finite-grid error from inverting a function with
a jump discontinuity at r=sigma. Abandoned as too imprecise.

Instead: for any fluid with a hard core at r=sigma=1, standard large-q
liquid-state asymptotics give (reduced units, rho = 6*phi/pi):
    S(q) - 1  ->  24*phi*g(sigma+)*cos(q)/q^2      as q -> infinity
No grid, no FFT -- just evaluate SqOneYukawa directly at large q and fit the
amplitude. This reproduces the exact K->0 hard-sphere contact value
(1+phi/2)/(1-phi)^2 to 5 significant figures, so the method itself is trusted.

Result: for K!=0 this disagrees with contact_value(), growing like O(K) at
small K, with the WRONG SIGN of trend (liboneyukawa's contact value correctly
*rises* with attractive K; contact_value() falls). Flipping the sign of K fed
into the spec's solver flips the trend direction but not the magnitude -- so
this isn't a simple K-vs-(-K) convention mismatch between the two codes.
Logged in polydisperse_yukawa_spec.md section 7b as an open item: needs a
fresh hand re-check of the primary closure / B-hat self-consistency /
contact_value equations against the [BA]/[BH02] page images, not another
numerical patch.
"""
import numpy as np
import warnings
from oneYukawaWrapper import one_yukawa_compute
from polydisperse_yukawa_msa import PolydisperseOneYukawaMSA

warnings.filterwarnings("ignore")


def contact_from_tail(Z, K, phi, qlo=300.0, qhi=995.0, n=6000):
    """g(sigma+) extracted from liboneyukawa.so's own S(Q), via the large-q tail."""
    q = np.linspace(qlo, qhi, n)
    Sq, ok, coeffs = one_yukawa_compute(Z, K, phi, q)
    assert ok, "Y_SolveEquations failed to find a solution"
    y = (Sq - 1.0) * q**2 / np.cos(q)
    mask = np.abs(np.cos(q)) > 0.3   # avoid dividing near zeros of cos(q)
    return np.median(y[mask]) / (24.0 * phi), coeffs


def mine(Z, K, phi):
    """g(sigma+) from this project's own N=1 solver/contact_value()."""
    sigma = np.array([1.0])
    rho = np.array([6 * phi / np.pi])
    solver = PolydisperseOneYukawaMSA(sigma, rho, z=Z, K=K)
    sol = solver.solve()
    return solver.contact_value(sol)[0, 0], sol['residual_a']


if __name__ == "__main__":
    g_exact = (1 + 0.2 / 2) / (1 - 0.2) ** 2
    print(f"K->0 exact hard-sphere check (phi=0.2): tail-extracted = "
          f"{contact_from_tail(6.0, 1e-6, 0.2)[0]:.5f}  exact = {g_exact:.5f}\n")

    Z, phi = 6.0, 0.1
    print(f"{'K':>8} {'tail-g (lib)':>14} {'mine-g(+K)':>12} {'mine-g(-K)':>12} {'my residual':>12}")
    for K in [0.01, 0.05, 0.1, 0.2, 0.4, 0.8, 1.6]:
        gtail, _ = contact_from_tail(Z, K, phi)
        gmine_pos, res = mine(Z, K, phi)
        gmine_neg, _ = mine(Z, -K, phi)
        print(f"{K:8.4f} {gtail:14.5f} {gmine_pos:12.5f} {gmine_neg:12.5f} {np.max(np.abs(res)):12.2e}")
