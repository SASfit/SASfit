# -*- coding: utf-8 -*-
"""
A SELF-CONTAINED test of the eq.22 index, using only physics -- no appeal to
[BA] and no transcription of any other paper.

[BH02] eq.21 and eq.22 read

    eq.21   C_ij + D_ij = (2 pi / z_n) sum_k rho_k gtilde_ik D_kj
    eq.22   D_ij = -delta_j a_j e^{z_n sigma_ij}          (AS PRINTED)

Note first that eq.22 as printed is odd on its face: the left-hand side
carries a row index i, but the right-hand side depends on i only through the
symmetric combination sigma_ij = (sigma_i + sigma_j)/2.  All the "charge"
information sits on the column.

Substituting the printed eq.22 into eq.21 gives

    C_ij + D_ij = -(2 pi / z) delta_j a_j sum_k rho_k gtilde_ik e^{z sigma_kj},

so C_ij inherits a delta_j.  Substituting the corrected

    eq.22'  D_ij = -delta_i a_j e^{z_n sigma_ij}

instead gives

    C_ij + D_ij = -(2 pi / z) a_j sum_k rho_k gtilde_ik delta_k e^{z sigma_kj},

so C_ij carries the CONTRACTION over delta, not delta_j itself.

The contact value follows from C, so the two choices give different g_ij.  The
pair distribution function must satisfy g_ij = g_ji.  That single requirement
decides between them, with no external reference at all.

At delta_i = 1 the two forms coincide identically, which is why this can only
be tested with charge polydispersity -- and why the misprint survived.
"""
import numpy as np

from polydisperse_yukawa_msa import PolydisperseOneYukawaMSA


def contact_with_delta_on(sv, sol, index):
    """Contact matrix built with delta carried by `index` ('i' = corrected
    row index, 'j' = column index exactly as [BH02] eq.22 prints it)."""
    a, A, beta, Bhat = sol['a'], sol['A'], sol['beta'], sol['Bhat']
    z, sig, N, d = sv.z, sv.sigma, sv.N, sv.delta
    g = np.zeros((N, N))
    for i in range(N):
        for j in range(N):
            sij = 0.5*(sig[i] + sig[j])
            dd = d[i] if index == 'i' else d[j]
            val = (A[j]*sig[i]/2 + beta[j]
                   - z*dd*a[j] + a[j]*Bhat[i]*np.exp(-z*sig[i]))
            g[i, j] = val/(2*np.pi*sij)
    return g


def asym(g):
    return np.max(np.abs(g - g.T))/max(np.max(np.abs(g)), 1e-300)


if __name__ == "__main__":
    print(__doc__)
    print("="*78)
    print("Relative asymmetry  max|g - g^T| / max|g|  of the contact matrix")
    print("="*78)
    print(f"{'case':>44} {'delta on i':>12} {'delta on j':>12}")

    cases = [
        ("N=2 delta=(1,1)      [degenerate]",
         np.array([0.8, 1.2]), np.array([0.15, 0.08]), np.array([1.0, 1.0])),
        ("N=2 delta=(1,0.7)",
         np.array([0.8, 1.2]), np.array([0.15, 0.08]), np.array([1.0, 0.7])),
        ("N=2 delta=(1.4,0.5)",
         np.array([0.8, 1.2]), np.array([0.15, 0.08]), np.array([1.4, 0.5])),
        ("N=3 delta=(1.3,1,0.6)",
         np.array([0.7, 1.0, 1.4]), np.array([0.10, 0.09, 0.03]),
         np.array([1.3, 1.0, 0.6])),
        ("N=4 delta=(1.5,1.1,0.9,0.4)",
         np.array([0.6, 0.9, 1.3, 1.8]), np.array([0.09, 0.07, 0.04, 0.015]),
         np.array([1.5, 1.1, 0.9, 0.4])),
    ]
    for label, sig, rho, d in cases:
        sv = PolydisperseOneYukawaMSA(sig, rho, z=3.0, K=0.6, delta=d)
        sol = sv.solve()
        gi = contact_with_delta_on(sv, sol, 'i')
        gj = contact_with_delta_on(sv, sol, 'j')
        print(f"{label:>44} {asym(gi):12.2e} {asym(gj):12.2e}")

    print()
    print("Reading: with delta on the row index the contact matrix is symmetric")
    print("to machine precision; with delta on the column index, as eq.22 prints")
    print("it, the asymmetry is O(10%) as soon as the deltas differ. The two")
    print("coincide exactly when all delta_i are equal (first row).")
