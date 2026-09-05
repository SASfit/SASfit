# -*- coding: utf-8 -*-
"""
General S_ij(Q) assembly for the N-species, M=1 Yukawa MSA solution.
Companion to polydisperse_yukawa_msa.py (which solves the closure).

Derivation and validation: see polydisperse_yukawa_spec.md section 7i.

Built on [BH02] eq.53 (whose transcription is validated to ~1e-16 against
[BH02] eq.51 AND, at N=1, reproduces liboneyukawa's Y_g == gtilde exactly at
every s). An earlier attempt used [BA] eq.28/29's rank-one decomposition
instead; that transcription came out wrong by a large s-dependent factor and
was abandoned -- see spec 7i and BA_vs_BH02_localize.py.

  qtilde_ij(is) = e^{-s*lambda_ji} * {
        sigma_i^3*psi1(s*sigma_i)*A_j + sigma_i^2*phi1(s*sigma_i)*beta_j
      + sum_m 1/(s+z_m) * [ (C_ij^(m)+D_ij^(m))*e^{-z_m*lambda_ji}
                            - C_ij^(m)*e^{-z_m*sigma_ij}
                            - z_m*sigma_i*phi0(s*sigma_i)*C_ij^(m)*e^{-z_m*sigma_ij} ] }
  lambda_ji = (sigma_j - sigma_i)/2,   sigma_ij = (sigma_i + sigma_j)/2

with, for M=1 and delta_i = 1 (eq.22, and eq.21 eliminated exactly as in
spec 7h using gtilde's symmetry):

  D_ij = -a_j*e^{z*sigma_ij}
  C_ij = -a_j*e^{z*sigma_j/2}*[ Bhat_i*e^{-z*sigma_i/2}/z - e^{z*sigma_i/2} ]

Baxter-Wertheim factorization ([BH02] eq.7 + eq.12), conjugated by
D = diag(sqrt(rho)) to give the symmetric Ashcroft-Langreth form
S_ij = delta_ij + sqrt(rho_i*rho_j)*htilde_ij(Q):

  m_ij(Q) = delta_ij - sqrt(rho_i*rho_j)*qtilde_ij(is)|_{s=-iQ}
  S_AL(Q) = [ m(Q) m(Q)^dagger ]^{-1}

m m^dagger is Hermitian by construction and equals the real symmetric matrix
[I - sqrt(rho_i rho_j) ctilde_ij], so S_AL comes out real symmetric and
positive definite automatically -- a built-in consistency check. Since q(r)
is real, evaluating at s=+iQ merely conjugates m and leaves S_AL unchanged.
"""
import numpy as np


# The auxiliary functions are shared with the solver module, which now
# evaluates them by correct Taylor series for |x| < 0.5 and is complex-safe
# (see the bug-fix note there): the S(Q) assembly needs s = -iQ, i.e. purely
# imaginary arguments, and small |x| as Q -> 0.
from polydisperse_yukawa_msa import _phi0, _phi1, _psi1


class PolydisperseOneYukawaSq:
    """S_ij(Q) assembly on top of a solved PolydisperseOneYukawaMSA."""

    def __init__(self, solver, sol):
        self.s = solver
        self.sol = sol
        sig, z = solver.sigma, solver.z
        a, Bhat = sol['a'], sol['Bhat']
        N = solver.N
        # sigma_ij and lambda_ji matrices
        self.sij = 0.5*(sig[:, None] + sig[None, :])
        self.lam_ji = 0.5*(sig[None, :] - sig[:, None])   # [i,j] -> lambda_ji
        # C_ij, D_ij  (eq.22 and eq.21-eliminated, M=1, delta=1)
        # eq.22 with delta on the ROW index (spec 7m) and eq.21 solved:
        #   D_ij = -delta_i a_j e^{z sigma_ij}
        #   C_ij = delta_i a_j e^{z sigma_ij}
        #          - (a_j/z) e^{z sigma_j/2} Bhat_i e^{-z sigma_i/2}
        d = solver.delta
        self.D = -d[:, None]*a[None, :]*np.exp(z*self.sij)
        self.C = (d[:, None]*a[None, :]*np.exp(z*self.sij)
                  - (a[None, :]/z)*np.exp(z*sig[None, :]/2)
                    * Bhat[:, None]*np.exp(-z*sig[:, None]/2))

    def qtilde(self, s):
        """qtilde_ij(is) via [BH02] eq.53, complex s. Returns (N,N) complex."""
        sv = self.s
        sig, z = sv.sigma, sv.z
        A, beta = self.sol['A'], self.sol['beta']
        s = complex(s)
        x = s*sig                                   # s*sigma_i (row index)
        psi = _psi1(x)[:, None]
        ph1 = _phi1(x)[:, None]
        ph0 = _phi0(x)[:, None]
        si3 = (sig**3)[:, None]
        si2 = (sig**2)[:, None]
        sig_i = sig[:, None]

        inner = (si3*psi*A[None, :] + si2*ph1*beta[None, :]
                 + (1.0/(s + z))*((self.C + self.D)*np.exp(-z*self.lam_ji)
                                  - self.C*np.exp(-z*self.sij)
                                  - z*sig_i*ph0*self.C*np.exp(-z*self.sij)))
        return np.exp(-s*self.lam_ji)*inner

    def m_matrix(self, Q):
        """m_ij(Q) = delta_ij - sqrt(rho_i rho_j) qtilde_ij(is)|_{s=-iQ}."""
        D = np.sqrt(self.s.rho)
        qt = self.qtilde(-1j*float(Q))
        return np.eye(self.s.N, dtype=complex) - D[:, None]*qt*D[None, :]

    def S_AL(self, Q):
        """Ashcroft-Langreth partial structure factors (N,N), real symmetric."""
        m = self.m_matrix(Q)
        return np.linalg.inv(m @ m.conj().T).real

    def S_number(self, Q):
        """Number-number S(Q) = sum_ij sqrt(x_i x_j) S_ij^AL; the N=1 S(Q)."""
        x = self.s.rho/np.sum(self.s.rho)
        w = np.sqrt(x)
        return float(w @ self.S_AL(Q) @ w)
