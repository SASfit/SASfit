# -*- coding: utf-8 -*-
"""
Direct numerical check of the ORIGINAL closure eq.54 [BH02] (not the reduced
eq.55) against the (a, Bhat, A, beta) our solver already finds by solving
eq.55 -- for N=1, M=1 (single species, single Yukawa term), where every
index sum collapses to one term and lambda_{ji}=0.

Derivation used (all steps cite page-verified equations; see chat for the
by-hand derivation):

  D^(1) = -delta*a*exp(z*sigma)                                   [eq.22]

  Bhat(z) = 2*pi*rho*delta*gtilde(z)*exp(z*sigma)                 [eq.43, N=1]
    => gtilde(z) = Bhat(z)*exp(-z*sigma) / (2*pi*rho*delta)

  C^(1) + D^(1) = (2*pi/z)*rho*gtilde(z)*D^(1)                    [eq.21, N=1]
    => C^(1) = D^(1) * [ Bhat(z)*exp(-z*sigma)/(z*delta) - 1 ]
             = -a*Bhat(z)/z + delta*a*exp(z*sigma)

  qtilde(iz) via eq.53 (N=1, M=1, lambda_11=0, s=z_n=z so z_m/(s+z_m)=1/2):
    qtilde(iz) = sigma^3*psi1(z*sigma)*A + sigma^2*phi1(z*sigma)*beta
                 + (1/(2z))*[ (C+D) - C*exp(-z*sigma)
                              - z*sigma*phi0(z*sigma)*C*exp(-z*sigma) ]

  eq.54 (N=1, delta_ij=1 trivially):
    2*pi*K*delta^2/z  ?=  D^(1)*[1 - rho*qtilde(iz)]

Also cross-checks the qtilde(iz) value two independent ways: via eq.53
directly (above), and via solving eq.51 using gtilde(z) from eq.43 --
qtilde0prime(iz) [eq.52] = 2*pi*gtilde(z)*[1-rho*qtilde(iz)]. These two
routes share no free parameters, so agreement is a nontrivial check of the
eq.43<->eq.21 identification made in the derivation above.
"""
import numpy as np, warnings
warnings.filterwarnings("ignore")
from polydisperse_yukawa_msa import PolydisperseOneYukawaMSA, _phi0, _phi1, _psi1


def check(Z, K, phi, delta=1.0):
    sigma = np.array([1.0])
    rho = np.array([6 * phi / np.pi])
    solver = PolydisperseOneYukawaMSA(sigma, rho, z=Z, K=K, delta=np.array([delta]))
    sol = solver.solve()
    a, A, beta, Bhat = sol['a'][0], sol['A'][0], sol['beta'][0], sol['Bhat'][0]
    z, sig, rho0, d = Z, 1.0, rho[0], delta

    resid_55 = sol['residual_a'][0]

    D1 = -d * a * np.exp(z * sig)
    gtilde_z = Bhat * np.exp(-z * sig) / (2 * np.pi * rho0 * d)
    C1 = D1 * (Bhat * np.exp(-z * sig) / (z * d) - 1.0)
    # sanity: same formula, expanded
    C1_alt = -a * Bhat / z + d * a * np.exp(z * sig)
    assert np.isclose(C1, C1_alt), (C1, C1_alt)

    # eq.53 at s=z (lambda_11=0, single m=n term, z_m/(s+z_m)=1/2)
    qtilde_iz = (sig**3 * _psi1(z * sig) * A
                 + sig**2 * _phi1(z * sig) * beta
                 + (1.0 / (2 * z)) * ((C1 + D1)
                                      - C1 * np.exp(-z * sig)
                                      - z * sig * _phi0(z * sig) * C1 * np.exp(-z * sig)))

    # eq.54 itself
    lhs_54 = 2 * np.pi * K * d**2 / z
    rhs_54 = D1 * (1.0 - rho0 * qtilde_iz)
    resid_54 = lhs_54 - rhs_54

    # cross-check: eq.51 / eq.52 route for qtilde(iz), independent of eq.53
    qtilde0prime_iz = (((1 + z * sig / 2) * A + z * beta) * np.exp(-z * sig) / z**2
                        - 0.5 * np.exp(-2 * z * sig) * C1)
    lhs_51 = 2 * np.pi * gtilde_z * (1.0 - rho0 * qtilde_iz)
    resid_51 = lhs_51 - qtilde0prime_iz

    return dict(a=a, A=A, beta=beta, Bhat=Bhat, C1=C1, D1=D1, qtilde_iz=qtilde_iz,
                resid_55=resid_55, lhs_54=lhs_54, rhs_54=rhs_54, resid_54=resid_54,
                resid_51=resid_51)


if __name__ == "__main__":
    cases = [(6.0, 1.0, 0.1), (6.0, 0.5, 0.1), (2.0, 0.3, 0.1), (2.5, 0.5, 0.1)]
    for Z, K, phi in cases:
        r = check(Z, K, phi)
        print(f"Z={Z} K={K} phi={phi}")
        print(f"  a={r['a']:.6f}  A={r['A']:.6f}  beta={r['beta']:.6f}  Bhat={r['Bhat']:.6f}")
        print(f"  C1={r['C1']:.6f}  D1={r['D1']:.6f}  qtilde(iz)={r['qtilde_iz']:.6f}")
        print(f"  eq.55 residual (already solved):      {r['resid_55']:.3e}")
        print(f"  eq.54 lhs={r['lhs_54']:.6f}  rhs={r['rhs_54']:.6f}  residual={r['resid_54']:.6e}")
        print(f"  eq.51/52 cross-check residual (qtilde consistency): {r['resid_51']:.3e}")
        print()
