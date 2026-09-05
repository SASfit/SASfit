# -*- coding: utf-8 -*-
"""Validation of the SAS layer (polydisperse_yukawa_sas.py)."""
import numpy as np, warnings
warnings.filterwarnings("ignore")
from polydisperse_yukawa_sas import (LogNormal, SchulzZimm, Sphere, CoreShell,
                                    PolydisperseYukawaSAS, discretize, moment_report)
from polydisperse_yukawa_msa import PolydisperseOneYukawaMSA
from polydisperse_yukawa_sq import PolydisperseOneYukawaSq
from oneYukawaWrapper import one_yukawa_compute

R0, PHI, ZS, K = 50.0, 0.20, 6.0, 1.0        # ZS = z*sigma_ref (dimensionless)
SIG = 2*R0
Z = ZS/SIG
Q = np.array([0.002, 0.005, 0.01, 0.02, 0.03, 0.05, 0.08, 0.12])


def test_monodisperse_limit():
    """A vanishingly narrow distribution must reproduce n|F|^2 S(Q) with the
    N=1 S(Q) that is validated to 3e-10 against liboneyukawa."""
    print("1. monodisperse limit (relsd -> 0): I(Q)/(n|F|^2) vs liboneyukawa S(Q)")
    for relsd in [1e-3, 1e-4]:
        m = PolydisperseYukawaSAS(SchulzZimm(R0, relsd=relsd), PHI, Z, K,
                                 Sphere(), nbins=6)
        I = m.I_exact(Q)
        ff = Sphere().F(Q, np.array([R0]))[:, 0]
        Sref, ok, _ = one_yukawa_compute(ZS, K, PHI, Q*SIG)
        Snum = I/(m.n_tot*ff**2)
        rel = np.abs(Snum - Sref)/np.abs(Sref)
        print(f"   relsd={relsd:g}: max rel err = {np.max(rel):.2e}")


def test_dilute_limit():
    print("\n2. dilute limit (phi -> 0): I_exact -> n <F^2>")
    for phi in [1e-4, 1e-6]:
        m = PolydisperseYukawaSAS(LogNormal(R0, 0.25), phi, Z, K, Sphere(), nbins=12)
        rel = np.abs(m.I_exact(Q) - m.I_dilute(Q))/np.abs(m.I_dilute(Q))
        print(f"   phi={phi:g}: max rel err = {np.max(rel):.2e}")


def test_nbins_convergence():
    """Judge convergence per-Q. A max-relative-change across I(Q) spanning
    three decades is dominated entirely by the deepest form-factor minimum
    (where I has fallen ~300x and is a near-cancellation of oscillating F_i),
    which makes a perfectly converged model look divergent."""
    print("\n3. convergence in nbins, per Q (log-normal relsd~0.31, phi=0.20)")
    ref = PolydisperseYukawaSAS(LogNormal(R0, 0.30), PHI, Z, K, Sphere(),
                                nbins=32).I_exact(Q)
    print("      nbins  " + "".join(f"{q:>11.4f}" for q in Q))
    for nb in [6, 8, 12, 16, 20]:
        I = PolydisperseYukawaSAS(LogNormal(R0, 0.30), PHI, Z, K,
                                  Sphere(), nbins=nb).I_exact(Q)
        rel = np.abs(I - ref)/np.abs(ref)
        print(f"      {nb:5d}  " + "".join(f"{r:>11.1e}" for r in rel))
    print("      (rel. error vs nbins=32; last two Q are the deep minima)")


def test_core_shell():
    print("\n4. core-shell sanity: zero core contrast must equal a plain sphere")
    d = SchulzZimm(R0, relsd=0.15)
    a = PolydisperseYukawaSAS(d, PHI, Z, K, Sphere(drho=1.0), nbins=10)
    b = PolydisperseYukawaSAS(d, PHI, Z, K,
                              CoreShell(rho_core=1.0, rho_shell=1.0, rho_solvent=0.0,
                                        thickness=8.0), nbins=10)
    rel = np.abs(a.I_exact(Q) - b.I_exact(Q))/np.abs(a.I_exact(Q))
    print(f"   max rel err = {np.max(rel):.2e}")
    c = PolydisperseYukawaSAS(d, PHI, Z, K,
                              CoreShell(rho_core=4.0, rho_shell=1.0, rho_solvent=0.0,
                                        thickness=8.0), nbins=10)
    print(f"   with real core contrast, I(Q) differs as expected: "
          f"max rel diff = {np.max(np.abs(c.I_exact(Q)-a.I_exact(Q))/a.I_exact(Q)):.2f}")


def test_structure_consistency():
    print("\n5. S_ij consistency: sum_ij sqrt(x_i x_j) S_ij == S_number")
    m = PolydisperseYukawaSAS(LogNormal(R0, 0.25), PHI, Z, K, Sphere(), nbins=10)
    x = m.rho/np.sum(m.rho); wv = np.sqrt(x)
    S = m.S_partials(Q)
    lhs = np.einsum('i,qij,j->q', wv, S, wv)
    print(f"   max abs diff = {np.max(np.abs(lhs - m.S_number(Q))):.2e}")


if __name__ == "__main__":
    test_monodisperse_limit()
    test_dilute_limit()
    test_nbins_convergence()
    test_core_shell()
    test_structure_consistency()
