# -*- coding: utf-8 -*-
"""
Given that convention_map_check.py just showed our (a, Bhat) are IDENTICAL to
the C library's converted solution (K_recov/K_C = 1.000000 exactly), the
closure is not the bug -- the contact-value assembly must be. So: extract
g(sigma+) a fourth, fully independent way -- from the large-s asymptotics of
the C library's own Y_g(s), which the mapping showed IS gtilde(s) = L[r*g(r)]:

  gtilde(s) = int_sigma^inf dr r g(r) e^{-s r}  ~  e^{-s*sigma}*sigma*g(sigma+)/s
  => sigma*g(sigma+) = lim_{s->inf} s * e^{s*sigma} * gtilde(s)

This needs no FFT, no S(Q) tail fit, and no BH02 contact formula at all.
Then compare against:
  - the S(Q) large-q tail reference (spec 7b)
  - eq.20 [BH02]:  2*pi*sigma*g = A/2 + beta - Z*(C+D)*e^-Z
  - eq.26 [BA]  :  2*pi*sigma*g = A/2 + beta - (Z*delta - Bhat)*a*e^-Z  (what's coded)
"""
import numpy as np, warnings
warnings.filterwarnings("ignore")
from polydisperse_yukawa_msa import PolydisperseOneYukawaMSA
from oneYukawaWrapper import one_yukawa_compute
from convention_map_check import Y_g


def tail_contact(Z, K, phi, qlo=300.0, qhi=995.0, n=6000):
    q = np.linspace(qlo, qhi, n)
    Sq, ok, _ = one_yukawa_compute(Z, K, phi, q)
    assert ok
    y = (Sq - 1.0)*q**2/np.cos(q)
    mask = np.abs(np.cos(q)) > 0.3
    return np.median(y[mask])/(24.0*phi)


def gtilde_contact(phi, Z, a, b, c, d, s=400.0):
    return s*np.exp(s)*Y_g(s, phi, Z, a, b, c, d)


if __name__ == "__main__":
    print(f"{'Z':>5}{'K':>6}{'phi':>6}{'g_tail':>10}{'g_gtilde':>11}"
          f"{'g_eq20':>10}{'g_eq26(coded)':>15}")
    for Z, K, phi in [(6.0, 1.0, 0.1), (6.0, 0.5, 0.1), (2.0, 0.3, 0.1),
                      (2.5, 0.5, 0.1), (3.0, 1.0, 0.3)]:
        _, ok, (a, b, c, d) = one_yukawa_compute(Z, K, phi, np.array([1.0]))
        g_tail = tail_contact(Z, K, phi)
        g_gt = gtilde_contact(phi, Z, a, b, c, d)

        A, beta, C, D = 2*np.pi*a, np.pi*a + 2*np.pi*b, 2*np.pi*c, 2*np.pi*d
        g_eq20 = (A/2 + beta - Z*(C + D)*np.exp(-Z))/(2*np.pi)

        sigma = np.array([1.0]); rho = np.array([6*phi/np.pi])
        s = PolydisperseOneYukawaMSA(sigma, rho, z=Z, K=K)
        sol = s.solve()
        g_eq26 = s.contact_value(sol)[0, 0]

        print(f"{Z:5.1f}{K:6.2f}{phi:6.2f}{g_tail:10.5f}{g_gt:11.5f}"
              f"{g_eq20:10.5f}{g_eq26:15.5f}")
