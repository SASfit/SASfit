# -*- coding: utf-8 -*-
"""
DERIVE Jhat/gamma_hat from [BH02] eq.30 + eq.33 and compare with eq.38/39.

eq.33:  X_i = delta_i + sigma_i phi0(z sigma_i) Bhat_i + sigma_i Delta_n
eq.30:  Delta_n = -(2 pi/(z^2 Delta)) sum_l rho_l (1 + z sigma_l/2) delta_l
                  - (2 pi/Delta) sum_l rho_l sigma_l^3 psi1(z sigma_l) Bhat_l

Substituting eq.30 into eq.33 and matching X_j = gamma_hat_j + sum_l Jhat_jl Bhat_l
(eq.37) gives, with beta0_j = (pi/Delta) sigma_j:

  gamma_hat_j = delta_j - (2 beta0_j/z^2) sum_l rho_l delta_l (1 + z sigma_l/2)
      -> IDENTICAL to eq.39 as printed.                                  [OK]

  Jhat_jl = delta_jl sigma_j phi0(z sigma_j) - 2 rho_l beta0_j sigma_l^3 psi1(z sigma_l)
      -> eq.38 as printed has instead
         delta_jl sigma_j phi0(z sigma_l) - 2 rho_l beta0_L sigma_j^3 psi1(z sigma_j)
         i.e. j and l INTERCHANGED in the second term (identical at N=1).

Since gamma_hat comes out matching eq.39 exactly from the same substitution,
eq.30/33 are the trustworthy pair, and eq.38 as printed looks like a misprint.
This script checks the derived Jhat against route B numerically at N>1.

Also tests the three expressions for P^(n) against each other (eq.27, 28, 31),
since route A/route B disagreed on P even at N=1.
"""
import numpy as np, warnings
warnings.filterwarnings("ignore")
from polydisperse_yukawa_msa import PolydisperseOneYukawaMSA, _phi0, _psi1
from PiX_two_routes import route_B


def Jhat_derived(sv):
    rho, sig, z = sv.rho, sv.sigma, sv.z
    N = sv.N
    J = np.zeros((N, N))
    for j in range(N):
        for l in range(N):
            J[j, l] = ((sig[j]*_phi0(z*sig[j]) if j == l else 0.0)
                       - 2*rho[l]*sv.beta0[j]*sig[l]**3*_psi1(z*sig[l]))
    return J


def Jhat_printed(sv):
    """eq.38 exactly as printed (beta0 with the summed index l)."""
    rho, sig, z = sv.rho, sv.sigma, sv.z
    N = sv.N
    J = np.zeros((N, N))
    for j in range(N):
        for l in range(N):
            J[j, l] = ((sig[j]*_phi0(z*sig[l]) if j == l else 0.0)
                       - 2*rho[l]*sv.beta0[l]*sig[j]**3*_psi1(z*sig[j]))
    return J


if __name__ == "__main__":
    rng = np.random.default_rng(1)
    cases = [(np.array([1.0]), np.array([0.19]), 6.0),
             (np.array([1.0, 2.0]), np.array([0.1, 0.02]), 6.0),
             (np.array([1.0, 1.5, 2.5]), np.array([0.08, 0.03, 0.006]), 4.0)]
    print("X from eq.37 with derived vs printed Jhat, against route B (eq.30+33):")
    for sig, rho, Z in cases:
        sv = PolydisperseOneYukawaMSA(sig, rho, z=Z, K=1.0)
        Jd, Jp = Jhat_derived(sv), Jhat_printed(sv)
        print(f"\n  sigma={sig} Z={Z}")
        for t in range(2):
            Bh = rng.normal(size=len(sig))
            _, XB, DnB, _ = route_B(sv, Bh)
            Xd = sv.gamma_hat + Jd @ Bh
            Xp = sv.gamma_hat + Jp @ Bh
            print(f"    trial {t}: |X_derived - X_routeB|={np.max(np.abs(Xd-XB)):.3e}   "
                  f"|X_printed - X_routeB|={np.max(np.abs(Xp-XB)):.3e}")

    print("\n\nThe three P^(n) expressions (eq.27 used by the code, eq.28, eq.31):")
    for sig, rho, Z in cases:
        sv = PolydisperseOneYukawaMSA(sig, rho, z=Z, K=1.0)
        Bh = rng.normal(size=len(sig))
        PiB, XB, DnB, _ = route_B(sv, Bh)
        Dg, z, d = sv.Delta_geo, sv.z, sv.delta
        P27 = (Dg/(np.pi*z))*np.sum(rho*(sv.A0*XB + 2*sv.beta0*(z*XB - PiB)))
        P28 = ((sv.zeta2 - Dg*z/np.pi)*DnB
               + np.sum(rho*(sig**2*_phi0(sig*z)*Bh + sig*d)))
        P31 = np.sum(rho*sv.beta0*XB) - z*DnB
        print(f"  sigma={sig} Z={Z}:  eq.27={P27:12.6f}  eq.28={P28:12.6f}  eq.31={P31:12.6f}")
