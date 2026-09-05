# -*- coding: utf-8 -*-
"""
Task 10 diagnostic (cont'd): is the N=1 discrepancy against liboneyukawa.so/
ozLib (see spec sections 7b-7d) a matter of the closure equation picking the
wrong root of 'a', or something more structural?

Part 1: scan 'a' as a free parameter, computing self-consistent Bhat/A/beta
(via the already page-image-verified eq.23-24/27-33 machinery) and the
resulting contact value + closure residual at each trial a, for
(Z,K,phi)=(6,1,0.1), reference g=2.13166 (from oneyukawa_tail_contact_check.py).
Finds the trial a that reproduces the reference contact value (~8.2) and
checks whether it is anywhere near a zero of the closure residual.

Result: it is not close (residual ~12.6 there, vs a clean zero-crossing at
a=-0.971 -- the solver's own root -- and at a=-14.24, unphysical). The scan
is smooth and monotonic through that region: no missed nearby root. So this
is not a root-selection bug; the gap is structural (see spec section 7e).

Part 2: sanity-checks liboneyukawa's own Baxter (a,b,c,d) against the
textbook Percus-Yevick hard-sphere Baxter coefficients at K->0, to see how
far a from-memory identification of those coefficients could be pushed
(flagged in the spec as a possible, not-yet-completed, next avenue) --
|a_Clib| matches (1+2*eta)/(1-eta)^2 to 5-6 sig figs (up to an overall sign),
but b_Clib does not match the b formula recalled from memory, which was not
independently re-derived before setting this avenue aside.
"""
import numpy as np, warnings
warnings.filterwarnings("ignore")
from polydisperse_yukawa_msa import PolydisperseOneYukawaMSA
from oneYukawaWrapper import one_yukawa_compute


def tail_contact(Z, K, phi, qlo=300.0, qhi=995.0, n=6000):
    q = np.linspace(qlo, qhi, n)
    Sq, ok, _ = one_yukawa_compute(Z, K, phi, q)
    assert ok
    y = (Sq - 1.0) * q**2 / np.cos(q)
    mask = np.abs(np.cos(q)) > 0.3
    return np.median(y[mask]) / (24.0 * phi)


def scan_a(Z, K, phi, a_values):
    sigma = np.array([1.0]); rho = np.array([6 * phi / np.pi])
    solver = PolydisperseOneYukawaMSA(sigma, rho, z=Z, K=K)
    rows = []
    for aval in a_values:
        a = np.array([float(aval)])
        Bhat = solver._solve_Bhat_given_a(a)
        Pi, X = solver._Pi_X(Bhat)
        P, Delta_n = solver._P_Deltan(Pi, X)
        A = solver.A0 + (np.pi / solver.Delta_geo) * a * P
        beta = solver.beta0 + a * Delta_n
        g = solver.contact_value(dict(a=a, A=A, beta=beta, Bhat=Bhat))[0, 0]
        resid = solver._residual_a(a, Bhat)[0]
        rows.append((aval, g, resid))
    return rows


def baxter_PY(eta):
    """Textbook Baxter (1968)/Wertheim PY hard-sphere factorization
    coefficients (sigma=1): Q(r) = (a/2)(r^2-1) + b(r-1), 0<r<1."""
    a = (1 + 2 * eta) / (1 - eta) ** 2
    b = -3 * eta * (1 + eta / 2) / (1 - eta) ** 2
    return a, b


if __name__ == "__main__":
    Z, K, phi = 6.0, 1.0, 0.1
    print(f"Part 1: a-scan at Z={Z}, K={K}, phi={phi} (reference g={tail_contact(Z,K,phi):.5f})")
    print(f"{'a':>8}{'g(contact)':>12}{'closure_resid':>15}")
    for aval, g, resid in scan_a(Z, K, phi, [-14.24, -1.0, -0.971, 6.0, 8.0, 8.2, 8.5, 10.0]):
        print(f"{aval:8.3f}{g:12.5f}{resid:15.4f}")

    print("\nPart 2: liboneyukawa Baxter (a,b) vs. textbook PY hard-sphere, K->0")
    for phi_val in [0.1, 0.2, 0.3]:
        _, ok, (a, b, c, d) = one_yukawa_compute(6.0, 1e-6, phi_val, np.array([1.0]))
        a_ref, b_ref = baxter_PY(phi_val)
        print(f"  phi={phi_val}: C-lib a={a:.6f} b={b:.6f}  |  textbook a={a_ref:.6f} b={b_ref:.6f}")
