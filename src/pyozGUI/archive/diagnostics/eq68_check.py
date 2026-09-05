# -*- coding: utf-8 -*-
"""
Check eq.68 [BH02] directly (the "new set of M equations for the scaling
matrix Gamma" -- this is literally the equation quoted in the paper's own
abstract as THE general solution) against our solved (a, Bhat, Pi, X), for
N=1, M=1 (n=m=1, single term):

  2*pi*K*rho*X*Bhat = -z*rho*a*Pi - (z/(2z))*{rho*a^2}*rho*X*Pi + rho*X*Delta_tilde
                     = -z*rho*a*Pi - 0.5*rho^2*a^2*X*Pi + rho*X*Delta_tilde
"""
import numpy as np, warnings
warnings.filterwarnings("ignore")
from polydisperse_yukawa_msa import PolydisperseOneYukawaMSA


def check(Z, K, phi, delta=1.0):
    sigma = np.array([1.0]); rho = np.array([6*phi/np.pi])
    solver = PolydisperseOneYukawaMSA(sigma, rho, z=Z, K=K, delta=np.array([delta]))
    sol = solver.solve()
    a, Bhat = sol['a'], sol['Bhat']
    Pi, X = solver._Pi_X(Bhat)
    z, rho0 = Z, rho[0]
    P, Delta_n = solver._P_Deltan(Pi, X)
    sig, A0, beta0, Dg = solver.sigma, solver.A0, solver.beta0, solver.Delta_geo
    j = 0
    term1 = -np.sum(rho*a*((1.0/z)*A0*(1+z*sig[j]/2) + beta0))
    term2 = -np.sum(rho*a**2*((np.pi/(z*Dg))*P + 0.5*(Delta_n + (sig[j]*np.pi/(2*Dg))*P)))
    Delta_tilde_j = term1 + term2

    lhs = 2*np.pi*K*rho0*X[0]*Bhat[0]
    rhs = -z*rho0*a[0]*Pi[0] - 0.5*rho0**2*a[0]**2*X[0]*Pi[0] + rho0*X[0]*Delta_tilde_j
    return lhs, rhs, lhs-rhs, sol['residual_a'][0]


if __name__ == "__main__":
    cases = [(6.0, 1.0, 0.1), (6.0, 0.5, 0.1), (2.0, 0.3, 0.1), (2.5, 0.5, 0.1)]
    for Z, K, phi in cases:
        lhs, rhs, resid, resid55 = check(Z, K, phi)
        print(f"Z={Z} K={K} phi={phi}: eq.68 lhs={lhs:.6f} rhs={rhs:.6f} "
              f"residual={resid:.6e}   (eq.55 residual={resid55:.2e})")
