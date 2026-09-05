# -*- coding: utf-8 -*-
"""
Check the scaling-matrix relation eq.82 [BH02] (Mtilde . Lambda = Gamma),
which the paper's own section 4 says is REQUIRED for a full/consistent
solution -- something never imposed anywhere in polydisperse_yukawa_msa.py
so far. At N=1, M=1:

  eq.72: Pi = -Gamma*X            => Gamma = -Pi/X   (trivial defn at N=1)
  eq.75: a  =  Lambda*X           => Lambda = a/X     (trivial defn at N=1)
  eq.83: Mtilde = (1/(2z))*rho*[z*X^2 + X*Pi - X*Pi] = 0.5*rho*X^2
         (the middle two terms cancel exactly at n=m)
  eq.82: Mtilde*Lambda = Gamma
         => 0.5*rho*X^2 * (a/X) = -Pi/X
         => Pi = -0.5*rho*a*X^2

This is NOT automatically true from anything coded so far (Pi, X, a all
come from the eq.55/eq.34/37/62-64 machinery, none of which enforces this
scaling relation) -- it is a genuinely independent check.
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
    rho0 = rho[0]

    lhs_Pi = Pi[0]
    rhs_Pi = -0.5*rho0*a[0]*X[0]**2
    return lhs_Pi, rhs_Pi, lhs_Pi - rhs_Pi, sol['residual_a'][0]


if __name__ == "__main__":
    cases = [(6.0, 1.0, 0.1), (6.0, 0.5, 0.1), (2.0, 0.3, 0.1), (2.5, 0.5, 0.1)]
    for Z, K, phi in cases:
        lhs, rhs, resid, resid55 = check(Z, K, phi)
        print(f"Z={Z} K={K} phi={phi}: eq.82-derived Pi={lhs:.6f}  "
              f"-0.5*rho*a*X^2={rhs:.6f}  residual={resid:.6e}   (eq.55 residual={resid55:.2e})")
