# -*- coding: utf-8 -*-
"""
Localize the error by comparing [BA] eq.28-31's building blocks against
[BH02] eq.52/53, whose transcription was already validated to ~1e-16 (the
eq.51-vs-eq.53 cross-check in eq54_direct_check.py).

[BH02] eq.52, N=1, sigma=1:
  qt0p(s) = [(1+s/2)A + s*beta]*e^-s/s^2 - [z/(s+z)]*e^{-(s+z)}*C
[BH02] eq.53, N=1, sigma=1, lambda=0:
  qt(s) = psi1(s)*A + phi1(s)*beta
          + [1/(s+z)]*[ (C+D) - C*e^-z - z*phi0(s)*C*e^-z ]
so  M_BH02(s) = 1 - rho*qt(s)   and   2*pi*gtilde(s) = qt0p(s)/M_BH02(s).

[BA] eq.30/31 claims mutilde(s) = qt0p(s), and eq.28/29 claims
M_BA(s) = 1 - rho*qt(s). Compare each.
"""
import numpy as np, warnings
warnings.filterwarnings("ignore")
from polydisperse_yukawa_msa import PolydisperseOneYukawaMSA, _phi0, _phi1, _psi1
from polydisperse_yukawa_sq import PolydisperseOneYukawaSq
from M_matrix_diagnostic import mutilde_and_Dtau
from oneYukawaWrapper import one_yukawa_compute
from convention_map_check import Y_g


def bh02_pieces(sv, sol, s):
    z = sv.z
    A, beta, a, Bhat = sol['A'][0], sol['beta'][0], sol['a'][0], sol['Bhat'][0]
    D1 = -a*np.exp(z)                       # eq.22, delta=sigma=1
    C1 = -a*Bhat/z + a*np.exp(z)            # eq.21 solved (spec 7g)
    qt0p = ((1 + s/2)*A + s*beta)*np.exp(-s)/s**2 - (z/(s+z))*np.exp(-(s+z))*C1
    qt = (_psi1(s)*A + _phi1(s)*beta
          + (1/(s+z))*((C1 + D1) - C1*np.exp(-z) - z*_phi0(s)*C1*np.exp(-z)))
    return qt0p, qt, C1, D1


if __name__ == "__main__":
    Z, K, phi = 6.0, 1.0, 0.1
    sv = PolydisperseOneYukawaMSA(np.array([1.0]), np.array([6*phi/np.pi]), z=Z, K=K)
    sol = sv.solve(); sq = PolydisperseOneYukawaSq(sv, sol)
    rho = sv.rho[0]
    _, ok, (ca, cb, cc, cd) = one_yukawa_compute(Z, K, phi, np.array([1.0]))
    print(f"Z={Z} K={K} phi={phi}   Bcal={sq.Bcal[0]:.6f}  Bhat={sol['Bhat'][0]:.6f}")
    print(f"{'s':>6}{'qt0p(BH02)':>14}{'mutilde(BA)':>14}{'ratio':>9}"
          f"{'M_BH02':>11}{'M_BA':>11}{'ratio':>9}{'gt_BH02':>12}{'Y_g':>12}")
    for s in [0.5, 1.0, 2.0, 4.0, 6.0, 10.0]:
        qt0p, qt, C1, D1 = bh02_pieces(sv, sol, s)
        M_bh = 1 - rho*qt
        mut, _ = mutilde_and_Dtau(sq, s)
        M_ba = sq._M_matrix(s)[0, 0].real
        gt_bh = qt0p/M_bh/(2*np.pi)
        ref = Y_g(s, phi, Z, ca, cb, cc, cd)
        print(f"{s:6.2f}{qt0p:14.8f}{mut[0,0].real:14.8f}{mut[0,0].real/qt0p:9.4f}"
              f"{M_bh:11.6f}{M_ba:11.6f}{M_ba/M_bh:9.4f}{gt_bh:12.8f}{ref:12.8f}")
