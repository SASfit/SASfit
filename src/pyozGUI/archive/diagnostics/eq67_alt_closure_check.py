# -*- coding: utf-8 -*-
"""
Check eq.67 [BH02] ("the alternative closure", obtained on p.11939 by
contracting the Bhat self-consistency eq.63 with rho_j*X_j^(n) and summing
over j) against the solver's own converged (a, Bhat, Pi, X) -- for N=1,M=1.

eq.67: 2*pi*K^(n) * sum_j rho_j X_j^(n) Bhat_j(z_n)
     = sum_m 1/(z_n+z_m) * {sum_k rho_k a_k^(n) a_k^(m)}
           * sum_j rho_j X_j^(n) [-Pi_j^(m) + z_m X_j^(m)]
       + sum_j rho_j X_j^(n) Delta_tilde_j(z_n)

Note the prefactor here is 1/(z_n+z_m), NOT z_n/(z_n+z_m) as in eq.63 itself
-- so this is not a trivial re-statement of eq.63, it's a genuinely
different-looking equation (paper says it "can also be obtained" from 63,
implying it's an equivalent theorem, not an independent assumption).

At N=1, M=1 (n=m=1, sums collapse):
  LHS = 2*pi*K*rho*X*Bhat
  RHS = (1/(2z)) * {rho*a^2} * rho*X*[-Pi + z*X] + rho*X*Delta_tilde
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

    # Delta_tilde_j(z_n) -- same internal quantity computed inside _rhs_of_Bhat
    P, Delta_n = solver._P_Deltan(Pi, X)
    sig, A0, beta0 = solver.sigma, solver.A0, solver.beta0
    Dg = solver.Delta_geo
    j = 0
    term1 = -np.sum(rho*a*((1.0/z)*A0*(1+z*sig[j]/2) + beta0))
    term2 = -np.sum(rho*a**2*((np.pi/(z*Dg))*P + 0.5*(Delta_n + (sig[j]*np.pi/(2*Dg))*P)))
    Delta_tilde_j = term1 + term2

    lhs = 2*np.pi*K*rho0*X[0]*Bhat[0]
    S2 = np.sum(rho*a**2)
    rhs = (1.0/(2*z))*S2*rho0*X[0]*(-Pi[0] + z*X[0]) + rho0*X[0]*Delta_tilde_j
    return lhs, rhs, lhs-rhs, sol['residual_a'][0]


if __name__ == "__main__":
    cases = [(6.0, 1.0, 0.1), (6.0, 0.5, 0.1), (2.0, 0.3, 0.1), (2.5, 0.5, 0.1)]
    for Z, K, phi in cases:
        lhs, rhs, resid, resid55 = check(Z, K, phi)
        print(f"Z={Z} K={K} phi={phi}: eq.67 lhs={lhs:.6f} rhs={rhs:.6f} "
              f"residual={resid:.6e}   (eq.55 residual={resid55:.2e})")
