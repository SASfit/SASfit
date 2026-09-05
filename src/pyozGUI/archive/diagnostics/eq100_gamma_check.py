# -*- coding: utf-8 -*-
"""
Two things, both from pages of [BH02]/[BA] not previously read.

PART 1 -- eq.100 [BH02] p.11943 is an INDEPENDENT contact-value formula,
from a page we had never opened, and therefore an independent check on the
corrected formula of spec section 7h:

  2*pi*sigma_ij*[g_ij(sigma_ij) - g^0_ij(sigma_ij)] = -(z+Gamma)*X_i*a_j

with g^0 the hard-sphere mixture contact value (2*pi*sigma_ij*g^0_ij =
A^0_j*sigma_i/2 + beta^0_j) and Gamma the M x M (here scalar) scaling matrix
of eq.72: Pi_i^(n) = -sum_m Gamma_nm X_i^(m), so at M=1, Gamma = -Pi_i/X_i
-- which must come out SPECIES-INDEPENDENT, itself a nontrivial check.

Related relations on the same page, also tested:
  eq.75  : a_i = Lambda*X_i                    (Lambda species-independent)
  eq.100b: 2*pi*K*X_i*X_j = -2*Gamma*(z+Gamma)*X_i*X_j/D_2
  eq.101 : D_2 = sum_k rho_k X_k^2
  eq.104 : sum_k rho_k a_k^2 = 4*pi*K   and   (1/pi)*sum_k rho_k a_k A^0_k
                                              = z*pi*K*Delta_n/Delta
"""
import numpy as np, warnings
warnings.filterwarnings("ignore")
from polydisperse_yukawa_msa import PolydisperseOneYukawaMSA


def check(sigma, rho, Z, K, label):
    s = PolydisperseOneYukawaMSA(np.asarray(sigma, float), np.asarray(rho, float), z=Z, K=K)
    sol = s.solve()
    a, Bhat = sol['a'], sol['Bhat']
    Pi, X = s._Pi_X(Bhat)
    P, Delta_n = s._P_Deltan(Pi, X)
    g = s.contact_value(sol)

    Gam = -Pi/X                       # eq.72 at M=1, per species
    Lam = a/X                         # eq.75 at M=1, per species
    D2 = np.sum(s.rho*X**2)           # eq.101
    N = s.N

    print(f"\n{label}:  Z={Z} K={K} sigma={sigma} rho={np.round(rho,5)}")
    print(f"  Gamma per species = {np.array2string(Gam, precision=10)}"
          f"   spread={np.ptp(Gam):.2e}   (eq.72: must be species-independent)")
    print(f"  Lambda per species= {np.array2string(Lam, precision=10)}"
          f"   spread={np.ptp(Lam):.2e}   (eq.75)")
    G = Gam[0]

    # eq.100b / eq.101
    lhs, rhs = 2*np.pi*K, -2*G*(Z+G)/D2
    print(f"  eq.100b: 2*pi*K={lhs:.10f}  -2*G*(z+G)/D2={rhs:.10f}  rel={abs(lhs-rhs)/abs(lhs):.2e}")

    # eq.104 helper identities
    s1, s1r = np.sum(s.rho*a**2), 4*np.pi*K
    print(f"  eq.104a: sum rho*a^2={s1:.10f}  4*pi*K={s1r:.10f}  rel={abs(s1-s1r)/abs(s1r):.2e}")
    s2 = (1/np.pi)*np.sum(s.rho*a*s.A0)
    s2r = Z*np.pi*K*Delta_n/s.Delta_geo
    print(f"  eq.104b: (1/pi)sum rho*a*A0={s2:.10f}  z*pi*K*Dn/D={s2r:.10f}  rel={abs(s2-s2r)/max(abs(s2r),1e-300):.2e}")

    # eq.100 -- the contact-value check
    print("  eq.100 (contact value) vs section 7h formula:")
    worst = 0.0
    for i in range(N):
        for j in range(N):
            sij = 0.5*(s.sigma[i] + s.sigma[j])
            g0 = (s.A0[j]*s.sigma[i]/2 + s.beta0[j])/(2*np.pi*sij)
            pred = g0 + (-(Z + G)*X[i]*a[j])/(2*np.pi*sij)
            rel = abs(pred - g[i, j])/abs(g[i, j])
            worst = max(worst, rel)
            print(f"    g[{i},{j}]: 7h={g[i,j]:.10f}  eq.100={pred:.10f}  rel={rel:.2e}")
    print(f"  worst eq.100 rel err = {worst:.2e}")
    return worst


if __name__ == "__main__":
    w = []
    w.append(check([1.0], [6*0.1/np.pi], 6.0, 1.0, "N=1"))
    w.append(check([1.0], [6*0.3/np.pi], 3.0, 1.0, "N=1 dense"))
    w.append(check([1.0, 2.0], [0.1, 0.02], 6.0, 1.0, "N=2 polydisperse"))
    w.append(check([1.0, 1.5, 2.5], [0.08, 0.03, 0.006], 4.0, 0.8, "N=3 polydisperse"))
    print(f"\n=== worst across all cases: {max(w):.2e} ===")
