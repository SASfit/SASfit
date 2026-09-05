# -*- coding: utf-8 -*-
"""
Pure algebraic identity test of the Pi/X machinery at N>1 -- no solving needed.

[BH02] gives TWO independent routes to (Delta^(n), X, Pi, P) for a given Bhat:

  route A (what the solver uses):
     Pi = xi_hat + Ihat @ Bhat            (eq.34, with Ihat from eq.35, xi eq.36)
     X  = gamma_hat + Jhat @ Bhat         (eq.37, with Jhat from eq.38, gam eq.39)
     Delta_n = -(1/z^2) sum_l rho_l [X_l A0_l + beta0_l (z X_l - 2 Pi_l)]  (eq.29)
     P = (Delta/(pi z)) sum_l rho_l [A0_l X_l + 2 beta0_l (z X_l - Pi_l)]  (eq.27)

  route B (explicit in Bhat, never touches Ihat/Jhat/xi/gamma):
     Delta_n = -(2 pi/(z^2 Delta)) sum_l rho_l (1+z sigma_l/2) delta_l
               - (2 pi/Delta) sum_l rho_l sigma_l^3 psi1(z sigma_l) Bhat_l   (eq.30)
     X_i  = delta_i + sigma_i Bhat_i phi0(z sigma_i) + sigma_i Delta_n       (eq.33)
     Pi_j = Bhat_j + (1 + sigma_j z/2) Delta_n
            + (1/2) sigma_j sum_l rho_l beta0_l X_l                          (eq.32)
     P = sum_l rho_l beta0_l X_l - z Delta_n                                 (eq.31)

If every transcription is right the two must agree identically, for ANY Bhat.
They are equal at N=1 by construction; disagreement at N>1 localizes a
species-index error in eq.35/36/38/39.
"""
import numpy as np, warnings
warnings.filterwarnings("ignore")
from polydisperse_yukawa_msa import PolydisperseOneYukawaMSA, _phi0, _psi1


def route_B(sv, Bhat):
    rho, sig, z, d, Dg = sv.rho, sv.sigma, sv.z, sv.delta, sv.Delta_geo
    Dn = (-(2*np.pi/(z**2*Dg))*np.sum(rho*(1 + z*sig/2)*d)
          - (2*np.pi/Dg)*np.sum(rho*sig**3*_psi1(z*sig)*Bhat))
    X = d + sig*Bhat*_phi0(z*sig) + sig*Dn
    S = np.sum(rho*sv.beta0*X)
    Pi = Bhat + (1 + sig*z/2)*Dn + 0.5*sig*S
    P = S - z*Dn
    return Pi, X, Dn, P


def route_A(sv, Bhat):
    Pi, X = sv._Pi_X(Bhat)
    P, Dn = sv._P_Deltan(Pi, X)
    return Pi, X, Dn, P


if __name__ == "__main__":
    rng = np.random.default_rng(0)
    cases = [(np.array([1.0]), np.array([0.19]), 6.0),
             (np.array([1.0, 2.0]), np.array([0.1, 0.02]), 6.0),
             (np.array([1.0, 1.5]), np.array([0.15, 0.05]), 3.0),
             (np.array([1.0, 1.5, 2.5]), np.array([0.08, 0.03, 0.006]), 4.0)]
    for sig, rho, Z in cases:
        sv = PolydisperseOneYukawaMSA(sig, rho, z=Z, K=1.0)
        print(f"\n  sigma={sig} rho={rho} Z={Z}   (3 random Bhat vectors)")
        for t in range(3):
            Bh = rng.normal(size=len(sig))
            PiA, XA, DnA, PA = route_A(sv, Bh)
            PiB, XB, DnB, PB = route_B(sv, Bh)
            print(f"    trial {t}:  max|Pi_A-Pi_B|={np.max(np.abs(PiA-PiB)):.3e}   "
                  f"max|X_A-X_B|={np.max(np.abs(XA-XB)):.3e}   "
                  f"|Dn_A-Dn_B|={abs(DnA-DnB):.3e}   |P_A-P_B|={abs(PA-PB):.3e}")
