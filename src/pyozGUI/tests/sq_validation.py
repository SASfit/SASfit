# -*- coding: utf-8 -*-
"""
Validation suite for the S_ij(Q) assembly (spec section 7i).

(1) N=1 vs liboneyukawa's SqOneYukawa/Y_pc, and vs its exact q==0 branch.
(2) Monodisperse collapse: N species with all sigma_i equal must reproduce N=1.
(3) N>1 structural checks: S real symmetric, positive definite, -> I at large Q.
(4) N>1 large-Q tail vs the author-verified contact values (spec 7h). For any
    hard-core mixture, h_ij jumps by g_ij(sigma_ij) at r=sigma_ij, so
        S_ij^AL(Q) - delta_ij -> sqrt(rho_i rho_j)*4*pi*sigma_ij
                                 * g_ij(sigma_ij) * cos(Q sigma_ij)/Q^2
    This is the check that actually exercises the polydisperse structure
    (sigma_ij != sigma_i, lambda_ji != 0), which the monodisperse collapse
    cannot -- and it ties S(Q) back to an independently verified quantity.
"""
import numpy as np, warnings
warnings.filterwarnings("ignore")
from polydisperse_yukawa_msa import PolydisperseOneYukawaMSA
from polydisperse_yukawa_sq import PolydisperseOneYukawaSq


def tail_amplitude(sq, i, j, sij, qlo=2000.0, qhi=2600.0, n=4000):
    """extract A_ij from S_ij - delta_ij ~ A_ij cos(Q sigma_ij)/Q^2"""
    qs = np.linspace(qlo, qhi, n)
    vals = []
    for q in qs:
        c = np.cos(q*sij)
        if abs(c) < 0.35:
            continue
        S = sq.S_AL(q)
        vals.append((S[i, j] - (1.0 if i == j else 0.0))*q**2/c)
    return np.median(vals)


if __name__ == "__main__":
    cases = [(np.array([1.0, 2.0]), np.array([0.1, 0.02]), 6.0, 1.0),
             (np.array([1.0, 1.5]), np.array([0.15, 0.05]), 3.0, 0.7),
             (np.array([1.0, 1.5, 2.5]), np.array([0.08, 0.03, 0.006]), 4.0, 0.8)]
    print("N>1 large-Q tail of S_ij vs 4*pi*sqrt(rho_i rho_j)*sigma_ij*g_ij(sigma_ij)")
    worst = 0.0
    for sig, rho, Z, K in cases:
        sv = PolydisperseOneYukawaMSA(sig, rho, z=Z, K=K)
        sol = sv.solve()
        sq = PolydisperseOneYukawaSq(sv, sol)
        g = sv.contact_value(sol)
        print(f"\n  sigma={sig}  rho={rho}  Z={Z}  K={K}")
        N = len(sig)
        for i in range(N):
            for j in range(i, N):
                sij = 0.5*(sig[i] + sig[j])
                meas = tail_amplitude(sq, i, j, sij)
                pred = 4*np.pi*np.sqrt(rho[i]*rho[j])*sij*g[i, j]
                rel = abs(meas - pred)/abs(pred)
                worst = max(worst, rel)
                print(f"    ({i},{j}) sigma_ij={sij:4.2f}: tail={meas:12.7f}  "
                      f"4*pi*sqrt(rr)*s*g={pred:12.7f}  rel={rel:.2e}")
    print(f"\nworst relative error = {worst:.2e}")
