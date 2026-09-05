# -*- coding: utf-8 -*-
"""
Task 10 diagnostic (cont'd): does using the un-hatted B (eq.25 [BA]) instead
of the hatted B-hat (eq.74-82 [BA]) in the contact-value formula (eq.26 [BA])
close the gap found against liboneyukawa.so / ozLib (see spec sections 7b)?

eq.26: 2*pi*sigma_ij*g_ij(sigma_ij) = A_j*(sigma_i/2) + beta_j
                                       - (z*delta_i - B_i)*a_j*exp(-z*sigma_ij)
eq.25: B_i^(m) = 2*pi*sum_k rho_k*delta_k^(m)*g_ik(sigma_ik)   [un-hatted, self-referential]

`contact_value()` in polydisperse_yukawa_msa.py plugs in Bhat (solved via
the Pi/X/Delta-tilde self-consistency loop, eq.74-82) where eq.26 actually
wants this self-referential, un-hatted B. Since B depends on the contact
value it appears in, eq.25+eq.26 together are solvable directly for g(sigma)
without Bhat at all -- for N=1, M=1 this reduces (by hand) to:

    g(sigma) = (A*sigma/2 + beta - z*delta*a*exp(-z*sigma))
               / (2*pi*(sigma - rho*delta*a*exp(-z*sigma)))

Result: this "new" formula and the "old" (Bhat-based) formula agree with
each other to ~1% everywhere tested, including at small Z where
exp(-z*sigma) isn't tiny (so a real B-vs-Bhat difference would show up if
there were one) -- and BOTH remain far (15-40%) from the reference. So the
B-vs-Bhat substitution in eq.26 is NOT the bug; A, beta, a themselves (and
therefore the primary closure eq./Pi-X-Bhat self-consistency loop feeding
them) are the next thing to re-check.
"""
import numpy as np, warnings
warnings.filterwarnings("ignore")
from polydisperse_yukawa_msa import PolydisperseOneYukawaMSA
from oneYukawaWrapper import one_yukawa_compute


def tail_contact(Z, K, phi, qlo=300.0, qhi=995.0, n=6000):
    q = np.linspace(qlo, qhi, n)
    Sq, ok, _ = one_yukawa_compute(Z, K, phi, q)
    assert ok, "Y_SolveEquations failed to find a solution"
    y = (Sq - 1.0) * q**2 / np.cos(q)
    mask = np.abs(np.cos(q)) > 0.3
    return np.median(y[mask]) / (24.0 * phi)


def compare(Z, K, phi):
    sigma = np.array([1.0]); rho = np.array([6 * phi / np.pi]); delta = np.array([1.0])
    solver = PolydisperseOneYukawaMSA(sigma, rho, z=Z, K=K, delta=delta)
    sol = solver.solve()
    a, A, beta, Bhat = sol['a'][0], sol['A'][0], sol['beta'][0], sol['Bhat'][0]
    z, sig, rho0, d = Z, 1.0, rho[0], delta[0]

    G_old = (A * sig / 2 + beta - (z * d - Bhat) * a * np.exp(-z * sig)) / (2 * np.pi * sig)
    G_new = (A * sig / 2 + beta - z * d * a * np.exp(-z * sig)) / (2 * np.pi * (sig - rho0 * d * a * np.exp(-z * sig)))
    gt = tail_contact(Z, K, phi)
    return gt, G_old, G_new


if __name__ == "__main__":
    cases = [(6.0, 0.5, 0.1), (6.0, 1.0, 0.1), (2.0, 0.3, 0.1), (2.5, 0.5, 0.1)]
    print(f"{'Z':>5}{'K':>7}{'reference':>12}{'old (Bhat)':>13}{'new (eq.25 B)':>15}")
    for Z, K, phi in cases:
        gt, gold, gnew = compare(Z, K, phi)
        print(f"{Z:5.1f}{K:7.2f}{gt:12.4f}{gold:13.4f}{gnew:15.4f}")
