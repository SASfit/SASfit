# -*- coding: utf-8 -*-
"""
DERIVATION of the corrected contact-value formula, and its numerical test.

From the C source, large-s asymptotics of gtilde(s) = Y_g(s):
  s*Y_tau(s) = a/s^2 + (a+b)/s - c*Z*e^-Z/(s+Z)
  Y_q(s) -> 0,  so  gtilde(s) -> e^-s * [(a+b) - c*Z*e^-Z]/s
  and since  sigma*g(sigma+) = lim s*e^s*gtilde(s):

      g(sigma+) = (a+b) - c*Z*e^-Z          [C-library variables]

Converting with the mapping validated in convention_map_check.py
(A = 2*pi*a, beta = pi*a + 2*pi*b, C = 2*pi*c):
      a + b = (A/2 + beta)/(2*pi),   c*Z*e^-Z = Z*C*e^-Z/(2*pi)
  =>
      2*pi*sigma_ij*g_ij(sigma_ij) = A_j*sigma_i/2 + beta_j
                                     - sum_m z_m*C_ij^(m)*e^{-z_m*sigma_ij}

i.e. the SAME structure as eq.20 [BH02] but with C ONLY -- NOT (C+D).
That is consistent: eq.20 is q'_ij(sigma_ji), the derivative of the full
factor function q = q^0 + sum_n D e^{-z_n r} (eq.17), whereas the contact
value picks up only q^0' (eq.18's core-region part). The coded formula
(eq.26 [BA]) is a third thing again, and matches neither.

Eliminating C via eq.21 (C+D = D*Bhat*e^{-z*sigma}/(delta*z)) and eq.22
(D = -delta*a*e^{z*sigma}), for N=1, M=1, sigma=1, delta=1:
      C = -a*Bhat/z + a*e^z
      z*C*e^-z = -a*Bhat*e^-z + a*z
  =>  2*pi*g(sigma+) = A/2 + beta + a*Bhat*e^-z - a*z

(The coded eq.26 has -a*z*e^-z where this has -a*z: at large z the coded
version silently drops almost the whole term, which is exactly the size of
the observed discrepancy.)
"""
import numpy as np, warnings
warnings.filterwarnings("ignore")
from polydisperse_yukawa_msa import PolydisperseOneYukawaMSA
from oneYukawaWrapper import one_yukawa_compute
from contact_from_clib_gtilde import tail_contact, gtilde_contact


def contact_fixed(solver, sol):
    """corrected N=1,M=1 contact value: 2*pi*g = A/2 + beta + a*Bhat*e^-z - a*z"""
    a, A, beta, Bhat = sol['a'][0], sol['A'][0], sol['beta'][0], sol['Bhat'][0]
    z = solver.z
    return (A/2 + beta + a*Bhat*np.exp(-z) - a*z)/(2*np.pi)


if __name__ == "__main__":
    print(f"{'Z':>5}{'K':>6}{'phi':>6}{'g_tail':>10}{'g_gtilde':>11}"
          f"{'g_FIXED':>10}{'g_coded':>10}{'err_FIXED':>12}")
    cases = [(6.0, 1.0, 0.1), (6.0, 0.5, 0.1), (2.0, 0.3, 0.1), (2.5, 0.5, 0.1),
             (3.0, 1.0, 0.3), (1.5, 0.2, 0.2), (8.0, 2.0, 0.15), (4.0, 0.8, 0.25),
             (6.0, 1e-6, 0.1)]
    for Z, K, phi in cases:
        _, ok, (a, b, c, d) = one_yukawa_compute(Z, K, phi, np.array([1.0]))
        if not ok:
            print(f"{Z:5.1f}{K:6.2f}{phi:6.2f}  Y_SolveEquations failed"); continue
        g_tail = tail_contact(Z, K, phi)
        g_gt = gtilde_contact(phi, Z, a, b, c, d)

        sigma = np.array([1.0]); rho = np.array([6*phi/np.pi])
        s = PolydisperseOneYukawaMSA(sigma, rho, z=Z, K=K)
        sol = s.solve()
        g_fix = contact_fixed(s, sol)
        g_cod = s.contact_value(sol)[0, 0]
        rel = abs(g_fix - g_gt)/abs(g_gt)
        print(f"{Z:5.1f}{K:6.2g}{phi:6.2f}{g_tail:10.5f}{g_gt:11.5f}"
              f"{g_fix:10.5f}{g_cod:10.5f}{rel:12.2e}")
