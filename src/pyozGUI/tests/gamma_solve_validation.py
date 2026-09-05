# -*- coding: utf-8 -*-
"""
Validation of the Gamma-centric solve (spec section 7k) at N>1 -- the tests
that the old solve() failed. See spec 7i/7j for the diagnosis.
"""
import numpy as np, warnings
warnings.filterwarnings("ignore")
from polydisperse_yukawa_msa import PolydisperseOneYukawaMSA
from polydisperse_yukawa_sq import PolydisperseOneYukawaSq
from sq_validation import tail_amplitude

CASES = [(np.array([1.0, 2.0]), np.array([0.1, 0.02]), 6.0, 1.0),
         (np.array([1.0, 1.5]), np.array([0.15, 0.05]), 3.0, 0.7),
         (np.array([1.0, 1.5, 2.5]), np.array([0.08, 0.03, 0.006]), 4.0, 0.8)]


def eq68_residual(sv, sol):
    """contracted closure eq.68 [BH02], M=1: the paper's own 'M equations'."""
    rho, z, K = sv.rho, sv.z, sv.K
    a, Bhat, Pi, X = sol['a'], sol['Bhat'], sol['Pi'], sol['X']
    P, Dn = sol['P'], sol['Delta_n']
    A0, beta0, sig, Dg = sv.A0, sv.beta0, sv.sigma, sv.Delta_geo
    Dt = np.array([
        -np.sum(rho*a*((1.0/z)*A0*(1 + z*sig[j]/2) + beta0))
        - np.sum(rho*a**2*((np.pi/(z*Dg))*P + 0.5*(Dn + (sig[j]*np.pi/(2*Dg))*P)))
        for j in range(sv.N)])
    lhs = 2*np.pi*K*np.sum(rho*X*Bhat)
    S2 = np.sum(rho*a**2)
    rhs = -z*np.sum(rho*a*Pi) - 0.5*S2*np.sum(rho*X*Pi) + np.sum(rho*X*Dt)
    return lhs - rhs


def eq100_worst(sv, sol):
    g = sv.contact_value(sol)
    G, X, a = sol['gamma'], sol['X'], sol['a']
    worst = 0.0
    for i in range(sv.N):
        for j in range(sv.N):
            sij = 0.5*(sv.sigma[i] + sv.sigma[j])
            g0 = (sv.A0[j]*sv.sigma[i]/2 + sv.beta0[j])/(2*np.pi*sij)
            pred = g0 - (sv.z + G)*X[i]*a[j]/(2*np.pi*sij)
            worst = max(worst, abs(pred - g[i, j])/abs(g[i, j]))
    return worst


if __name__ == "__main__":
    print("=== N>1: Gamma-centric solve vs the old solve() ===")
    for sig, rho, Z, K in CASES:
        sv = PolydisperseOneYukawaMSA(sig, rho, z=Z, K=K)
        old, new = sv.solve(), sv.solve_gamma()
        Pio, Xo = sv._Pi_X(old['Bhat'])
        print(f"\n  sigma={sig} rho={rho} Z={Z} K={K}   Gamma={new['gamma']:.10f}")
        print(f"    Gamma spread (eq.72):  old={np.ptp(-Pio/Xo):.3e}   "
              f"new={np.max(np.abs(new['eq72_residual'])):.3e}")
        print(f"    eq.68 contracted closure residual: old={eq68_residual(sv, dict(old, Pi=Pio, X=Xo, gamma=0)):.3e}   "
              f"new={eq68_residual(sv, new):.3e}")
        print(f"    eq.55 componentwise residual:      old={np.max(np.abs(old['residual_a'])):.3e}   "
              f"new={np.max(np.abs(new['residual_a'])):.3e}")
        print(f"    eq.100 contact-value worst rel:    old={eq100_worst(sv, dict(old, gamma=(-Pio/Xo)[0], X=Xo)):.3e}   "
              f"new={eq100_worst(sv, new):.3e}")
        gn = sv.contact_value(new)
        print(f"    contact matrix symmetry |g-g^T|:   new={np.max(np.abs(gn-gn.T)):.2e}")
        sq = PolydisperseOneYukawaSq(sv, new)
        sqo = PolydisperseOneYukawaSq(sv, old)
        print(f"    large-Q tail of S_ij vs contact values (rel err):")
        for i in range(sv.N):
            for j in range(i, sv.N):
                sij = 0.5*(sig[i] + sig[j])
                pr = 4*np.pi*np.sqrt(rho[i]*rho[j])*sij
                en = abs(tail_amplitude(sq, i, j, sij) - pr*gn[i, j])/abs(pr*gn[i, j])
                go = sv.contact_value(old)
                eo = abs(tail_amplitude(sqo, i, j, sij) - pr*go[i, j])/abs(pr*go[i, j])
                tag = "diag" if i == j else "OFF "
                print(f"      {tag} ({i},{j}): old={eo:.3e}   new={en:.3e}")
