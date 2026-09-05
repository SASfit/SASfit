# -*- coding: utf-8 -*-
"""
Isolate whether [BA] eq.28's M matrix (as transcribed in
polydisperse_yukawa_sq.py) is itself right, independently of the
Fourier/factorization step used for S(Q).

Test: [BA] eq.32/33/34 give the Laplace transform of r*g(r) from the SAME
M-matrix ingredients:
   mutilde_ij(s) = -atil_i b_j - ctil_i d_j - sum_m etil_i^(m) f_j^(m)   (eq.30)
      atil_i = (1 + s sigma_i/2) e^{-s sigma_i/2}/s^2                    (eq.31)
      ctil_i = e^{-s sigma_i/2}/s
      etil_i^(m) = e^{-s sigma_i/2} z_m/(s+z_m) * Xcal_i^(m)
      Xcal_i^(m) = e^{-z_m sigma_i}(Bcal_i^(m)/z_m - delta_i^(m))
   D_tau = Det| delta_ab - sum_i u_i^(a) v_i^(b) |                       (eq.34)
   2*pi*gtilde_ij(s) = -mutilde_ij(s)/D_tau(s)                           (eq.33)
   [and eq.32's matrix form: 2*pi*gtilde = mutilde @ inv(M)]

and we already PROVED (spec 7h) that gtilde(s) == liboneyukawa's Y_g(s)
exactly. So this compares against an exact, independent reference at real s.

Also checks det(M) == D_tau (the rank-reduction identity
det(I_N - U V^T) = det(I_{M+2} - V^T U)), which must hold identically.
"""
import numpy as np, warnings
warnings.filterwarnings("ignore")
from polydisperse_yukawa_msa import PolydisperseOneYukawaMSA
from polydisperse_yukawa_sq import PolydisperseOneYukawaSq, _c_phi0, _c_phi1, _c_psi1
from oneYukawaWrapper import one_yukawa_compute
from convention_map_check import Y_g


def uv(sqobj, s):
    sv = sqobj.s
    sig, rho, z, delta = sv.sigma, sv.rho, sv.z, sv.delta
    A, beta, a = sqobj.sol['A'], sqobj.sol['beta'], sqobj.sol['a']
    s = complex(s)
    x = s*sig
    eph, emh = np.exp(s*sig/2), np.exp(-s*sig/2)
    u1 = rho*sig**3*_c_psi1(x)*eph;  v1 = A*emh
    u2 = rho*sig**2*_c_phi1(x)*eph;  v2 = beta*emh
    Bz = sqobj.Bcal/z
    varphi1 = (np.exp(z*sig/2)/(s+z))*(Bz - np.exp(-z*sig)*(Bz-delta)*(1+z*sig*_c_phi0(x)))
    u3 = rho*sig**2*eph*varphi1;     v3 = a*np.exp(z*sig/2)
    return [u1, u2, u3], [v1, v2, v3]


def mutilde_and_Dtau(sqobj, s):
    sv = sqobj.s
    sig, z, delta = sv.sigma, sv.z, sv.delta
    A, beta, a = sqobj.sol['A'], sqobj.sol['beta'], sqobj.sol['a']
    s = complex(s)
    emh = np.exp(-s*sig/2)
    atil = (1 + s*sig/2)*emh/s**2
    ctil = emh/s
    Xcal = np.exp(-z*sig)*(sqobj.Bcal/z - delta)
    etil = emh*(z/(s+z))*Xcal
    b, d, f = A*emh, beta*emh, a*np.exp(z*sig/2)
    mut = -np.outer(atil, b) - np.outer(ctil, d) - np.outer(etil, f)

    U, V = uv(sqobj, s)
    G = np.zeros((3, 3), dtype=complex)
    for A_ in range(3):
        for B_ in range(3):
            G[A_, B_] = np.sum(U[A_]*V[B_])
    Dtau = np.linalg.det(np.eye(3) - G)
    return mut, Dtau


if __name__ == "__main__":
    for Z, K, phi in [(6.0, 1.0, 0.1), (2.0, 0.3, 0.1)]:
        sv = PolydisperseOneYukawaMSA(np.array([1.0]), np.array([6*phi/np.pi]), z=Z, K=K)
        sol = sv.solve()
        sq = PolydisperseOneYukawaSq(sv, sol)
        _, ok, (a, b, c, d) = one_yukawa_compute(Z, K, phi, np.array([1.0]))
        print(f"\nZ={Z} K={K} phi={phi}  Bcal={sq.Bcal[0]:.6f}  Bhat={sol['Bhat'][0]:.6f}")
        print(f"{'s':>7}{'Y_g(s)=gtilde':>16}{'eq.33 gtilde':>15}{'eq.32 gtilde':>15}"
              f"{'det M':>13}{'D_tau':>13}")
        for s in [0.5, 1.0, 2.0, Z, 5.0, 10.0]:
            ref = Y_g(s, phi, Z, a, b, c, d)
            mut, Dtau = mutilde_and_Dtau(sq, s)
            M = sq._M_matrix(s)
            g33 = (-mut[0, 0]/Dtau/(2*np.pi)).real
            g32 = ((mut @ np.linalg.inv(M))[0, 0]/(2*np.pi)).real
            print(f"{s:7.2f}{ref:16.8f}{g33:15.8f}{g32:15.8f}"
                  f"{M[0,0].real:13.6f}{Dtau.real:13.6f}")
