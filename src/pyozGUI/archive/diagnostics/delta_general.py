# -*- coding: utf-8 -*-
"""
STATUS: SUPERSEDED as production code, RETAINED as an independent cross-check.

The N-dimensional fixed point implemented here was the first route to general
delta_i.  It was later found that eq.22 [BH02] carries a misprint (the delta
sits on the wrong index; invisible at delta=1), and with the corrected

    D_ij = -delta_i a_j e^{z sigma_ij}

the general-delta contact value and C_ij close in CLOSED FORM -- no fixed
point.  That closed form is what polydisperse_yukawa_msa.contact_value() and
polydisperse_yukawa_sq build, and what report section 3.5 documents.

This module is kept because it reaches the contraction T_i by a different road:
the closed form gets T from Bhat (eq.43), this one gets it from the Laplace
route eq.51/52/53 and an N x N linear solve.  Both assume the corrected eq.22,
so this is a check of the C_ij assembly and of the Bhat shortcut under general
delta -- NOT an independent test of the eq.22 correction itself (that one is in
report sec. 3.5).  Agreement is at machine precision:

    N=1 delta=1                4.4e-16
    N=2 delta=(1, 0.7)         0.0
    N=3 delta=(1.3, 1, 0.6)    8.9e-16

The derivation notes below also record how the eq.22 misprint was cornered.

------------------------------------------------------------------------
Contact values and C_ij for GENERAL delta_i (charge/coupling polydispersity),
which is also what polydisperse RMSA needs (spec 7m; report section 8).

DERIVATION (mine -- wants independent verification per AI_USAGE.md; it uses
only equations already validated elsewhere in this project: [BH02] eq.21, 22,
51, 52, 53).

eq.22:  D_ij = -delta_j a_j e^{z sigma_ij}
              = -delta_j a_j e^{z sigma_i/2} e^{z sigma_j/2}

eq.21:  C_ij + D_ij = (2 pi/z) sum_k rho_k gtilde_ik(z) D_kj
                    = -(2 pi/z) delta_j a_j e^{z sigma_j/2} * T_i

with the UNIT-weighted contraction

        T_i = sum_k rho_k gtilde_ik(z) e^{z sigma_k/2} .

So C is rank-structured -- it is fixed by the N-vector T, not by an N x N
matrix:

        C_ij = -D_ij - c_j T_i ,      c_j = (2 pi/z) delta_j a_j e^{z sigma_j/2}

This is the crux. For delta_i = 1 the Laplace-transform quantity Bhat supplies
T directly (Bhat_j = 2 pi e^{z sigma_j/2} T_j by eq.43 and gtilde's symmetry),
which is the shortcut used in spec 7h. For general delta, Bhat gives only the
DELTA-WEIGHTED contraction sum_i rho_i delta_i gtilde_ij e^{z sigma_i/2}, so T
must be obtained separately. Equation 51 supplies it:

        2 pi gtilde(s) M(s) = qtilde0prime(is),   M_lj = delta_lj - rho_l qtilde_lj(is)
   =>   gtilde = qtilde0prime M^{-1} / (2 pi)
   =>   T = qtilde0prime M^{-1} v / (2 pi),   v_k = rho_k e^{z sigma_k/2}

evaluated at s = z. Both qtilde0prime (eq.52) and qtilde (eq.53) depend on C,
hence on T, so this is a fixed point in N dimensions -- cheap, and it collapses
to the closed form above when delta = 1.
"""
import numpy as np

from polydisperse_yukawa_msa import _phi0, _phi1, _psi1


def _pieces(sv, sol, C, D, s):
    """qtilde0prime (eq.52) and qtilde (eq.53) at real s, given C and D."""
    sig, z = sv.sigma, sv.z
    A, beta = sol['A'], sol['beta']
    sij = 0.5*(sig[:, None] + sig[None, :])
    lam_ji = 0.5*(sig[None, :] - sig[:, None])
    sig_i = sig[:, None]

    # eq.52
    qt0p = (((1 + s*sig_i/2)*A[None, :] + s*beta[None, :])*np.exp(-s*sij)/s**2
            - (z/(s + z))*np.exp(-(s + z)*sij)*C)
    # eq.53
    x = s*sig
    inner = ((sig**3)[:, None]*_psi1(x)[:, None]*A[None, :]
             + (sig**2)[:, None]*_phi1(x)[:, None]*beta[None, :]
             + (1.0/(s + z))*((C + D)*np.exp(-z*lam_ji)
                              - C*np.exp(-z*sij)
                              - z*sig_i*_phi0(x)[:, None]*C*np.exp(-z*sij)))
    qt = np.exp(-s*lam_ji)*inner
    return qt0p, qt


def solve_CD(sv, sol, tol=1e-14, maxiter=500):
    """C_ij, D_ij and T_i for arbitrary delta_i. Returns (C, D, T, iters)."""
    sig, z, rho, delta = sv.sigma, sv.z, sv.rho, sv.delta
    a = sol['a']
    sij = 0.5*(sig[:, None] + sig[None, :])
    # eq.22 with the CORRECTED index (delta on i, not j -- see the report,
    # sec. 3.5).  With delta on j this reduces to the printed eq.22; both
    # coincide at delta = 1.
    D = -delta[:, None]*a[None, :]*np.exp(z*sij)
    # eq.21 contraction: with delta on i the weight rides on the k-sum inside
    # T, so the j-dependent prefactor loses its delta.
    c = (2*np.pi/z)*a*np.exp(z*sig/2)                # c_j
    v = rho*delta*np.exp(z*sig/2)                    # v_k

    # start from the delta=1 closed form when available, else zeros
    T = sol['Bhat']*np.exp(-z*sig/2)/(2*np.pi)
    for it in range(maxiter):
        C = -D - np.outer(T, c)
        qt0p, qt = _pieces(sv, sol, C, D, z)
        M = np.eye(sv.N) - rho[:, None]*qt
        T_new = (qt0p @ np.linalg.solve(M, v))/(2*np.pi)
        err = np.max(np.abs(T_new - T))
        T = T_new
        if err < tol:
            break
    return -D - np.outer(T, c), D, T, it + 1


def contact_general(sv, sol):
    """2 pi sigma_ij g_ij = A_j sigma_i/2 + beta_j - z C_ij e^{-z sigma_ij}
    (spec 7h eq. for the contact value; C only, not C+D), for any delta_i."""
    sig, z = sv.sigma, sv.z
    A, beta = sol['A'], sol['beta']
    C, D, T, _ = solve_CD(sv, sol)
    sij = 0.5*(sig[:, None] + sig[None, :])
    val = (A[None, :]*sig[:, None]/2 + beta[None, :]
           - z*C*np.exp(-z*sij))
    return val/(2*np.pi*sij)
