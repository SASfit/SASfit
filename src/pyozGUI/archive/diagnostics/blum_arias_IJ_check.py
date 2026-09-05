# -*- coding: utf-8 -*-
"""
LITERATURE ADJUDICATION of the eq.35 / eq.38 misprint in [BH02].

[BH02] = Blum & Hernando, J. Phys.: Condens. Matter 14, 11933 (2002)
[BA]   = Blum & Arias, "Structure of multi-component/multi-Yukawa mixtures",
         arXiv:cond-mat/0602477v2 (2006)

The two papers print the SAME two matrices, four years apart:

  [BH02] eq.35   Ihat_jl = d_jl + rho_l [ beta0_l (sig_j^2/2) phi0(z sig_j)
                                          - (A0_l + z beta0_l) sig_j^3 psi1(z sig_j) ]
  [BH02] eq.38   Jhat_jl = d_jl sig_j phi0(z sig_l)
                           - 2 rho_l beta0_l sig_j^3 psi1(z sig_j)

  [BA]   eq.76   Ihat_jl = d_jl - rho_l sig_l^2 [ beta0_j phi1(z sig_l)
                                                  + A0_j sig_l psi1(z sig_l) ]
                 Jhat_jl = d_jl sig_j phi0(z sig_l)
                           - 2 rho_l beta0_j sig_l^3 psi1(z sig_l)

They differ by a j <-> l interchange on the AMPLITUDE subscripts (beta0, A0)
and on the ARGUMENTS of phi/psi.  The rho_l weight stays on l in both.

The two Ihat forms are otherwise algebraically identical, via the identity
[BA] eq.24 prints explicitly:

        phi1(x) = x psi1(x) - phi0(x)/2 .

Indeed, swapping j<->l in [BH02] eq.35's bracket gives
    +beta0_j sig_l^2/2 phi0 - z beta0_j sig_l^3 psi1 - A0_j sig_l^3 psi1
  = -beta0_j sig_l^2 [ z sig_l psi1 - phi0/2 ] - A0_j sig_l^3 psi1
  = -beta0_j sig_l^2 phi1(z sig_l)             - A0_j sig_l^3 psi1(z sig_l)
  = [BA] eq.76.
So eq.35 and eq.76 are the SAME equation up to the index interchange -- which
is what makes the interchange, rather than a different derivation, the
explanation.

This script checks numerically which of the two the solver's own
transcription-free construction agrees with.  The solver builds Ihat/Jhat by
probing the affine map Bhat -> (Pi, X) of [BH02] eq.30/32/33 with the standard
basis, so it never transcribes eq.35 or eq.38 at all.

Result (see bottom): the probe agrees with [BA] eq.76 to machine precision and
disagrees with [BH02] eq.35/38 by O(1e-2) for every N > 1, while all three
coincide at N = 1 (where j == l makes the interchange invisible).
"""
import numpy as np

from polydisperse_yukawa_msa import (PolydisperseOneYukawaMSA,
                                     _phi0, _phi1, _psi1)


def IJ_blum_arias(sv):
    """[BA] eq.76, transcribed from arXiv:cond-mat/0602477v2 p.14."""
    z, sig, rho, N = sv.z, sv.sigma, sv.rho, sv.N
    A0, b0 = sv.A0, sv.beta0
    I = np.zeros((N, N))
    J = np.zeros((N, N))
    for j in range(N):
        for l in range(N):
            kron = 1.0 if j == l else 0.0
            x_l = z*sig[l]
            I[j, l] = kron - rho[l]*sig[l]**2*(b0[j]*_phi1(x_l)
                                               + A0[j]*sig[l]*_psi1(x_l))
            J[j, l] = (kron*sig[j]*_phi0(x_l)
                       - 2*rho[l]*b0[j]*sig[l]**3*_psi1(x_l))
    return I, J


def IJ_blum_hernando_printed(sv):
    """[BH02] eq.35 / eq.38 exactly as printed (p.11937)."""
    z, sig, rho, N = sv.z, sv.sigma, sv.rho, sv.N
    A0, b0 = sv.A0, sv.beta0
    I = np.zeros((N, N))
    J = np.zeros((N, N))
    for j in range(N):
        for l in range(N):
            kron = 1.0 if j == l else 0.0
            x_j = z*sig[j]
            I[j, l] = kron + rho[l]*(b0[l]*(sig[j]**2/2)*_phi0(x_j)
                                     - (A0[l] + z*b0[l])*sig[j]**3*_psi1(x_j))
            J[j, l] = (kron*sig[j]*_phi0(z*sig[l])
                       - 2*rho[l]*b0[l]*sig[j]**3*_psi1(x_j))
    return I, J


def rel(a, b):
    d = np.max(np.abs(a - b))
    s = max(np.max(np.abs(a)), np.max(np.abs(b)), 1e-300)
    return d/s


if __name__ == "__main__":
    print(__doc__.split("This script")[0])
    print("="*74)
    print("Solver's probe of eq.30/32/33  vs  [BA] eq.76  vs  [BH02] eq.35/38")
    print("="*74)
    print(f"{'case':>26} {'probe vs [BA]':>16} {'probe vs [BH02]':>18}")

    cases = [
        ("N=1  sigma=[1]",            np.array([1.0]),           np.array([0.2])),
        ("N=2  sigma=[0.8,1.2]",      np.array([0.8, 1.2]),      np.array([0.15, 0.08])),
        ("N=3  sigma=[1,1.5,2.5]",    np.array([1.0, 1.5, 2.5]), np.array([0.08, 0.03, 0.006])),
        ("N=4  sigma=[.6,.9,1.3,1.8]",np.array([0.6, 0.9, 1.3, 1.8]),
                                      np.array([0.09, 0.07, 0.04, 0.015])),
    ]
    for label, sig, rho in cases:
        for z in (2.0, 6.0):
            sv = PolydisperseOneYukawaMSA(sig, rho, z=z, K=0.5)
            xi_p, I_p, gam_p, J_p = sv._PiX_affine_from_eq30_32_33()
            I_ba, J_ba = IJ_blum_arias(sv)
            I_bh, J_bh = IJ_blum_hernando_printed(sv)
            e_ba = max(rel(I_p, I_ba), rel(J_p, J_ba))
            e_bh = max(rel(I_p, I_bh), rel(J_p, J_bh))
            print(f"{label+f'  z={z}':>26} {e_ba:16.3e} {e_bh:18.3e}")

    print()
    print("Also checking the two VECTORS, which the two papers agree on:")
    print("  [BH02] eq.36 = [BA] eq.77 (xi-hat),  [BH02] eq.39 = [BA] eq.77 (gamma-hat)")
    for label, sig, rho in cases:
        sv = PolydisperseOneYukawaMSA(sig, rho, z=4.0, K=0.5)
        xi_p, _, gam_p, _ = sv._PiX_affine_from_eq30_32_33()
        print(f"{label:>26}   xi: {rel(xi_p, sv.xi_hat_printed):.2e}"
              f"   gamma: {rel(gam_p, sv.gamma_hat_printed):.2e}")

    print()
    print("And the identity [BA] eq.24 that maps eq.35 onto eq.76:")
    x = np.array([1e-3, 0.1, 0.5, 1.0, 3.0, 10.0, 40.0])
    print("  max|phi1(x) - (x*psi1(x) - phi0(x)/2)| =",
          f"{np.max(np.abs(_phi1(x) - (x*_psi1(x) - _phi0(x)/2))):.2e}")
