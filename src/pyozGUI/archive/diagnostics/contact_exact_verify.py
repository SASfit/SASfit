# -*- coding: utf-8 -*-
"""
High-precision confirmation. The C library's own coefficients give the contact
value EXACTLY (no asymptotic limit, no FFT, no tail fit):

    g(sigma+) = (a + b) - c*Z*e^-Z

(derived in contact_formula_fix.py from s*Y_tau's large-s expansion; every term
of s*Y_tau is accounted for, so this is exact, not leading-order).

General N-species, M=1, delta_i=1 form of the corrected formula, obtained by
eliminating C_ij via eq.21 + eq.22 with gtilde's symmetry (see spec 7h):

    2*pi*sigma_ij*g_ij(sigma_ij) = A_j*sigma_i/2 + beta_j
                                   - z*a_j + a_j*Bhat_i*e^{-z*sigma_i}

which for N=1, sigma=1 reduces to A/2 + beta - z*a + a*Bhat*e^-z.
"""
import numpy as np, warnings
warnings.filterwarnings("ignore")
from polydisperse_yukawa_msa import PolydisperseOneYukawaMSA
from oneYukawaWrapper import one_yukawa_compute


def contact_general(solver, sol):
    """corrected contact matrix, N species, M=1, delta=1."""
    a, A, beta, Bhat = sol['a'], sol['A'], sol['beta'], sol['Bhat']
    z, sig, N = solver.z, solver.sigma, solver.N
    g = np.zeros((N, N))
    for i in range(N):
        for j in range(N):
            sij = 0.5*(sig[i] + sig[j])
            val = A[j]*sig[i]/2 + beta[j] - z*a[j] + a[j]*Bhat[i]*np.exp(-z*sig[i])
            g[i, j] = val/(2*np.pi*sij)
    return g


if __name__ == "__main__":
    print("Exact comparison: C-lib (a+b)-c*Z*e^-Z  vs  corrected BH02 formula")
    print(f"{'Z':>5}{'K':>7}{'phi':>6}{'g_exact_Clib':>14}{'g_corrected':>13}{'rel_err':>11}")
    cases = [(6.0, 1.0, 0.1), (6.0, 0.5, 0.1), (2.0, 0.3, 0.1), (2.5, 0.5, 0.1),
             (3.0, 1.0, 0.3), (1.5, 0.2, 0.2), (8.0, 2.0, 0.15), (4.0, 0.8, 0.25),
             (10.0, 3.0, 0.35), (0.5, 0.1, 0.05)]
    worst = 0.0
    for Z, K, phi in cases:
        _, ok, (a, b, c, d) = one_yukawa_compute(Z, K, phi, np.array([1.0]))
        if not ok:
            print(f"{Z:5.1f}{K:7.2f}{phi:6.2f}  Y_SolveEquations failed"); continue
        g_exact = (a + b) - c*Z*np.exp(-Z)
        sigma = np.array([1.0]); rho = np.array([6*phi/np.pi])
        s = PolydisperseOneYukawaMSA(sigma, rho, z=Z, K=K)
        sol = s.solve()
        g_corr = contact_general(s, sol)[0, 0]
        rel = abs(g_corr - g_exact)/abs(g_exact)
        worst = max(worst, rel)
        print(f"{Z:5.1f}{K:7.2f}{phi:6.2f}{g_exact:14.9f}{g_corr:13.9f}{rel:11.2e}")
    print(f"\nworst relative error: {worst:.2e}")

    print("\nRegression: K->0 hard-sphere limit, N=1 and N=2 (must still be exact)")
    print("  reference: Lebowitz PY hard-sphere mixture,")
    print("             g_ij(sigma_ij) = 1/(1-x3) + 3*x2/(1-x3)^2 * s_i*s_j/(s_i+s_j)")
    print("             with x_n = (pi/6) sum_k rho_k sigma_k^n.")

    def lebowitz(sigma, rho):
        x2 = np.pi/6*np.sum(rho*sigma**2)
        x3 = np.pi/6*np.sum(rho*sigma**3)
        si, sj = sigma[:, None], sigma[None, :]
        return 1/(1 - x3) + 3*x2/(1 - x3)**2 * si*sj/(si + sj)

    # NOTE: solve() is singular at exactly K=0 (the a_j parametrisation degenerates
    # there); solve_gamma() takes the K=0 branch analytically (Gamma -> 0).  Using
    # solve() here is what made this block raise LinAlgError.
    worst0 = 0.0
    for phi in [0.1, 0.2, 0.3]:
        sigma = np.array([1.0]); rho = np.array([6*phi/np.pi])
        s = PolydisperseOneYukawaMSA(sigma, rho, z=6.0, K=0.0)
        g = contact_general(s, s.solve_gamma())[0, 0]
        exact = lebowitz(sigma, rho)[0, 0]
        worst0 = max(worst0, abs(g - exact)/exact)
        print(f"  N=1 phi={phi}: corrected={g:.10f}  PY exact={exact:.10f}  "
              f"rel={abs(g-exact)/exact:.1e}")

    for sigma, rho in [(np.array([0.8, 1.2]), np.array([0.15, 0.08])),
                       (np.array([0.6, 1.0, 1.5]), np.array([0.10, 0.09, 0.03]))]:
        s = PolydisperseOneYukawaMSA(sigma, rho, z=6.0, K=0.0)
        g = contact_general(s, s.solve_gamma())
        exact = lebowitz(sigma, rho)
        rel = np.max(np.abs(g - exact)/exact)
        worst0 = max(worst0, rel)
        print(f"  N={len(sigma)} sigma={sigma}: max rel={rel:.1e}")

    print(f"\nworst K=0 relative error: {worst0:.2e}")
