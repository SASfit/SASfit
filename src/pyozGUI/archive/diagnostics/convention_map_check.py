# -*- coding: utf-8 -*-
"""
Map liboneyukawa's Baxter (a,b,c,d) onto BH02's (A, beta, C^(n), D^(n)),
by reading the C source's OWN closure equations rather than guessing.

From 2Y_OneYukawa.c:
  Y_sigma(s) = -(a/2 + b + c*e^-Z)/s + a/s^3 + b/s^2 + (c+d)/(s+Z)
  Y_tau(s)   = b/s^2 + a*(1/s^3 + 1/s^2) - c*Z*e^-Z/(s*(s+Z))
  Y_q(s)     = Y_sigma(s) - e^-s * Y_tau(s)
  Y_g(s)     = s*Y_tau(s)*e^-s / (1 - 12*phi*Y_q(s))
  Y_LinearEquation_3:  K*e^Z = Z*d*(1 - 12*phi*Y_q(Z))
  Y_NonlinearEquation: (c+d) = 12*phi*d*Y_g(Z)/Z   [after substituting Y_g's defn]

Structural correspondences (12*phi == 2*pi*rho exactly, since rho=6*phi/pi):

 (i) BH02 eq.51 (N=1):  2*pi*gtilde(s)*[1-rho*qtilde(is)] = qtilde0prime(is)
     vs C-lib Y_g(s) = s*Y_tau(s)*e^-s/(1-12*phi*Y_q(s))
     => qtilde(is) = 2*pi*Y_q(s)  and  Y_g(s) = gtilde(s) EXACTLY
        (the C library's Y_g IS the Laplace transform of r*g(r))

 (ii) eq.43 (N=1): Bhat = 2*pi*rho*gtilde(z)*e^z = 12*phi*e^Z*Y_g(Z)
      -- and the C source's own v = 24*phi*K*e^Z*Y_g(Z) is therefore v = 2*K*Bhat

 (iii) eq.52 vs s*Y_tau(s), term by term in 1/s^2, 1/s, 1/(s+Z):
      A = 2*pi*a ;  beta = pi*a + 2*pi*b ;  C^(1) = 2*pi*c ;  D^(1) = 2*pi*d
      This also makes eq.21 (C+D = D*Bhat*e^-Z/Z) IDENTICAL to the C source's
      own Y_NonlinearEquation -- a nontrivial structural confirmation.

 (iv) eq.22 (D = -delta*a_BH*e^{z*sigma}) then forces
      a_BH = -2*pi*d*e^-Z

Part 1 validates (iii) in the exact hard-sphere limit, where our A^0/beta^0 are
already independently verified to machine precision against Lebowitz/PY -- using
the C library's own K=0 linear equations solved directly (NOT Y_SolveEquations,
which is ill-conditioned near K=0 and returns a wrong root there; that, not a
bad derivation, is what the earlier spec 7f 'b mismatch' actually was).

Part 2 then asks the decisive question at K!=0: feed the C library's converted
(a_BH, Bhat) into OUR closure residual and solve for the K that would zero it.
If our closure math is right and only the K convention differs, the recovered
K should be a clean function of K_Clib (e.g. K_Clib*e^Z or K_Clib*e^-Z).
"""
import numpy as np, warnings
warnings.filterwarnings("ignore")
from polydisperse_yukawa_msa import PolydisperseOneYukawaMSA
from oneYukawaWrapper import one_yukawa_compute


def Y_sigma(s, Z, a, b, c, d):
    return -(a/2. + b + c*np.exp(-Z))/s + a*s**-3 + b*s**-2 + (c+d)*(s+Z)**-1

def Y_tau(s, Z, a, b, c):
    return b*s**-2 + a*(s**-3 + s**-2) - s**-1 * c*Z*np.exp(-Z)*(s+Z)**-1

def Y_q(s, Z, a, b, c, d):
    return Y_sigma(s, Z, a, b, c, d) - np.exp(-s)*Y_tau(s, Z, a, b, c)

def Y_g(s, phi, Z, a, b, c, d):
    return s*Y_tau(s, Z, a, b, c)*np.exp(-s)/(1 - 12*phi*Y_q(s, Z, a, b, c, d))


def hs_limit_ab(phi):
    """Solve the C source's OWN Y_LinearEquation_1/2 at c=d=0 (true K->0
    hard-sphere limit), bypassing the ill-conditioned quartic solver."""
    # eq1: b - 12*phi*(-a/8 - b/6) = 0
    # eq2: 1 - a - 12*phi*(-a/3 - b/2) = 0
    p = 12*phi
    M = np.array([[p/8.,      1 + p/6.],
                  [-1 + p/3., p/2.    ]])
    rhs = np.array([0.0, -1.0])
    a, b = np.linalg.solve(M, rhs)
    return a, b


def part1():
    print("Part 1: hard-sphere limit -- C-lib (a,b) vs BH02 (A^0, beta^0)")
    print(f"{'phi':>6}{'a_C':>10}{'b_C':>10}{'2*pi*a':>12}{'A^0':>12}"
          f"{'pi*a+2*pi*b':>14}{'beta^0':>12}")
    for phi in [0.05, 0.1, 0.2, 0.3, 0.4]:
        a, b = hs_limit_ab(phi)
        sigma = np.array([1.0]); rho = np.array([6*phi/np.pi])
        s = PolydisperseOneYukawaMSA(sigma, rho, z=1.0, K=0.0)
        A0, beta0 = s.A0[0], s.beta0[0]
        print(f"{phi:6.2f}{a:10.5f}{b:10.5f}{2*np.pi*a:12.6f}{A0:12.6f}"
              f"{np.pi*a + 2*np.pi*b:14.6f}{beta0:12.6f}")

    # also confirm against the textbook Baxter alpha = a^2
    print("\n  cross-check a_C^2 vs (1+2eta)^2/(1-eta)^4 [Baxter 1968 eq.17, mu=0]:")
    for phi in [0.1, 0.2, 0.3]:
        a, _ = hs_limit_ab(phi)
        print(f"    phi={phi}: a_C^2={a**2:.6f}  alpha_Baxter={(1+2*phi)**2/(1-phi)**4:.6f}")


def part2():
    print("\nPart 2: K!=0 -- convert C-lib solution to BH02 vars, recover K")
    print(f"{'Z':>5}{'K_C':>7}{'phi':>6}{'a_BH':>11}{'Bhat':>11}"
          f"{'K_recov':>12}{'K_recov/K_C':>13}{'e^Z':>10}")
    for Z, K_C, phi in [(6.0, 1.0, 0.1), (6.0, 0.5, 0.1), (2.0, 0.3, 0.1),
                        (2.5, 0.5, 0.1), (1.0, 0.5, 0.2), (3.0, 1.0, 0.3)]:
        _, ok, (a, b, c, d) = one_yukawa_compute(Z, K_C, phi, np.array([1.0]))
        if not ok:
            print(f"{Z:5.1f}{K_C:7.2f}{phi:6.2f}   Y_SolveEquations failed")
            continue
        rho0 = 6*phi/np.pi
        a_BH = -2*np.pi*d*np.exp(-Z)
        Bhat = 12*phi*np.exp(Z)*Y_g(Z, phi, Z, a, b, c, d)

        sigma = np.array([1.0]); rho = np.array([rho0])
        # residual_a is affine in K: resid(K) = 2*pi*K*delta/z + (K-independent part)
        s0 = PolydisperseOneYukawaMSA(sigma, rho, z=Z, K=0.0)
        r0 = s0._residual_a(np.array([a_BH]), np.array([Bhat]))[0]
        K_recov = -r0 * Z / (2*np.pi)   # solve 2*pi*K/Z + r0 = 0
        print(f"{Z:5.1f}{K_C:7.2f}{phi:6.2f}{a_BH:11.5f}{Bhat:11.5f}"
              f"{K_recov:12.6f}{K_recov/K_C:13.6f}{np.exp(Z):10.2f}")


if __name__ == "__main__":
    part1()
    part2()
