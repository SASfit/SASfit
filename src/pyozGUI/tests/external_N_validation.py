# -*- coding: utf-8 -*-
"""
EXTERNALLY INDEPENDENT N>1 validation (spec section 7l).

Compares three things at N=2 and N=3:
  (1) the numerical mixture-OZ solver (mixture_oz_numerical.py), Richardson-
      extrapolated in the grid spacing dr (first-order convergent, p ~ 1,
      because of the hard-core discontinuity; extrapolation buys ~20x, giving
      ~1e-4 relative accuracy, verified against the exact N=1 answer)
  (2) the analytic solution with Ihat/Jhat rebuilt from [BH02] eq.30/32/33
      -- i.e. the corrected form derived in spec 7k
  (3) the same but with eq.35/eq.38 EXACTLY AS PRINTED

The numerical solver shares no equation with the analytic route: it iterates
OZ on a radial grid under the same physical closure ([BH02] eq.5/6) and
nothing else. So this test can distinguish (2) from (3) from the outside,
which every previous N>1 check could not.
"""
import numpy as np, warnings
warnings.filterwarnings("ignore")
from polydisperse_yukawa_msa import PolydisperseOneYukawaMSA
from polydisperse_yukawa_sq import PolydisperseOneYukawaSq
from mixture_oz_numerical import MixtureOZ

QS = np.array([1.0, 2.0, 3.5, 5.0, 7.5])


def analytic(sig, rho, Z, K, printed=False, qs=None):
    sv = PolydisperseOneYukawaMSA(sig, rho, z=Z, K=K)
    if printed:
        sv.Ihat, sv.Jhat = sv.Ihat_printed, sv.Jhat_printed
        sv.xi_hat, sv.gamma_hat = sv.xi_hat_printed, sv.gamma_hat_printed
        N = sv.N
        I = np.zeros((N, N)); J = np.zeros((N, N))
        for j in range(N):
            for l in range(N):
                I[j, l] = sv.Ihat[l, j]*rho[l]/rho[j]
                J[j, l] = sv.Jhat[l, j]*rho[l]/rho[j]
        sv.I, sv.J = I, J
    sol = sv.solve()
    sq = PolydisperseOneYukawaSq(sv, sol)
    # IMPORTANT: evaluate at the numerical solver's OWN grid k values, not at
    # the requested QS -- the nearest grid point can differ by ~0.07 in k, and
    # dS/dk near the peak is large enough for that to dominate the comparison.
    qgrid = QS if qs is None else qs
    S = np.array([sq.S_AL(q) for q in qgrid])
    return sv.contact_value(sol), S


def numerical_richardson(sig, rho, Z, K, drs=(0.005, 0.0025)):
    out = []
    for dr in drs:
        npts = int(round(20.48/dr))
        m = MixtureOZ(sig, rho, Z, K, npts=npts, dr=dr).solve(alpha=0.35, tol=1e-13)
        k, S = m.S_AL_grid()
        idx = [np.argmin(np.abs(k - q)) for q in QS]
        out.append((m.contact(), np.array([S[:, :, i] for i in idx]), k[idx]))
    (g1, S1, _), (g2, S2, kk) = out
    return g2 + (g2 - g1), S2 + (S2 - S1), kk       # p=1 Richardson


if __name__ == "__main__":
    cases = [(np.array([1.0, 2.0]), np.array([0.1, 0.02]), 6.0, 1.0),
             (np.array([1.0, 1.5, 2.5]), np.array([0.08, 0.03, 0.006]), 4.0, 0.8)]
    for sig, rho, Z, K in cases:
        print(f"\n=== sigma={sig} rho={rho} Z={Z} K={K} ===")
        gN, SN, kk = numerical_richardson(sig, rho, Z, K)
        gC, SC = analytic(sig, rho, Z, K, printed=False, qs=kk)
        gP, SP = analytic(sig, rho, Z, K, printed=True, qs=kk)
        N = len(sig)
        print("  contact values g_ij:")
        print(f"    {'(i,j)':>7}{'numerical':>12}{'corrected':>12}{'as-printed':>12}"
              f"{'err corr':>11}{'err print':>11}")
        for i in range(N):
            for j in range(i, N):
                ec = abs(gC[i, j] - gN[i, j])/abs(gN[i, j])
                ep = abs(gP[i, j] - gN[i, j])/abs(gN[i, j])
                print(f"    ({i},{j}){gN[i,j]:12.6f}{gC[i,j]:12.6f}{gP[i,j]:12.6f}"
                      f"{ec:11.2e}{ep:11.2e}")
        print("  S_ij(Q) worst relative error over Q =", np.round(kk, 3))
        for i in range(N):
            for j in range(i, N):
                ec = np.max(np.abs(SC[:, i, j] - SN[:, i, j])/np.abs(SN[:, i, j]))
                ep = np.max(np.abs(SP[:, i, j] - SN[:, i, j])/np.abs(SN[:, i, j]))
                print(f"    ({i},{j}):  corrected={ec:.2e}   as-printed={ep:.2e}")
