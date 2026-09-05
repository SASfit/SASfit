# -*- coding: utf-8 -*-
"""
Task 10 diagnostic (cont'd): re-checked the primary closure equation (eq.55
[BH02]) and everything feeding it (A_j^0/beta_j^0, xi-hat/gamma-hat, Pi/X,
P^(n)/Delta^(n), the Bhat self-consistency eq.62-64) line-by-line against
the page images -- all transcribe exactly. Also checked for root
multiplicity in the (quadratic-in-a) closure, since the reference C library
needed explicit physical-root selection among 4 quartic roots.

Part 1: does the solver find the same root from many different starting
guesses (and is there an alternate, physically-plausible root nearby)?
Part 2: brute-force scan of a in [-20,20] for self-consistent roots (solve
Bhat given a, then evaluate the closure residual at that (a,Bhat) pair).

Result: only one physically sane root exists (positive contact value), and
the solver already finds it from every reasonable starting guess -- root
selection is not the bug. See spec section 7d for the full write-up: one
real (but N=1-invisible) bug was found and fixed in Ihat/Jhat (see
polydisperse_yukawa_msa.py's own comment there), but the N=1 discrepancy
itself remains open after this check.
"""
import numpy as np, warnings
warnings.filterwarnings("ignore")
from polydisperse_yukawa_msa import PolydisperseOneYukawaMSA

Z, K, phi = 6.0, 1.0, 0.1
sigma = np.array([1.0])
rho = np.array([6 * phi / np.pi])


def multi_start_check():
    print("Part 1: convergence from many starting guesses for 'a'")
    for a0 in [0.0, -2.0, -5.0, -10.0, 2.0, 5.0, 10.0, -0.3]:
        solver = PolydisperseOneYukawaMSA(sigma, rho, z=Z, K=K)
        sol = solver.solve(a0=np.array([a0]))
        g = solver.contact_value(sol)[0, 0]
        print(f"  a0={a0:7.2f}  ->  a={sol['a'][0]:10.5f}  Bhat={sol['Bhat'][0]:8.5f}  "
              f"resid={np.max(np.abs(sol['residual_a'])):.2e}  iters={sol['iterations']:4d}  "
              f"g(contact)={g:.5f}")


def brute_force_root_scan():
    print("\nPart 2: brute-force root scan, a in [-20, 20]")
    solver = PolydisperseOneYukawaMSA(sigma, rho, z=Z, K=K)
    a_grid = np.linspace(-20, 20, 4001)
    resid = []
    for aval in a_grid:
        a = np.array([aval])
        Bhat = solver._solve_Bhat_given_a(a)
        resid.append(solver._residual_a(a, Bhat)[0])
    resid = np.array(resid)
    signchange = np.where(np.diff(np.sign(resid)) != 0)[0]
    print("  approx self-consistent root locations:")
    for idx in signchange:
        print(f"    a in [{a_grid[idx]:.4f}, {a_grid[idx+1]:.4f}]  "
              f"resid: {resid[idx]:.4f} -> {resid[idx+1]:.4f}")


if __name__ == "__main__":
    multi_start_check()
    brute_force_root_scan()
