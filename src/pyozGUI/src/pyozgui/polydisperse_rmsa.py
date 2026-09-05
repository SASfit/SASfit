# -*- coding: utf-8 -*-
"""
Polydisperse RMSA with a COMMON rescaling factor.

Rescale every hard-core diameter by the SAME factor lambda (not a
size-dependent one, which would break the factorisation of the closure and
with it the analytic solution), and choose lambda so that ALL contact values
are non-negative -- i.e. the binding pair just reaches zero:

        min_ij g_ij(lambda*sigma_ij) = 0 .

The number densities rho_i are physical and unchanged, so the hard-core volume
fraction goes as phi -> lambda^3 phi automatically.

The rescaling re-references the closure, which (spec 7m / report section 8)
factorises as

        K_ij' = K delta_i' delta_j' ,   delta_i' = exp(-z (lambda-1) sigma_i/2)

so the rescaled system is a factored Yukawa system with delta_i != 1. That case
is now available in closed form once eq.22 is corrected to carry delta on the
ROW index (spec 7m), which is what makes this analytic rather than numerical.

Note the modelling choice being made here. A mixture has N(N+1)/2 contact
values but only one lambda, and they do not all reach zero together; requiring
the MINIMUM to vanish repairs the worst-behaved pair and leaves the others
strictly positive (over-corrected relative to their own thresholds). That is a
deliberate choice, not a derivation -- see the report.
"""
import numpy as np
from scipy.optimize import brentq

from polydisperse_yukawa_msa import PolydisperseOneYukawaMSA
from polydisperse_yukawa_sq import PolydisperseOneYukawaSq


def rescaled_system(sigma, rho, z, K, lam):
    """The MSA system equivalent to the physical one with the core inflated."""
    sigma = np.asarray(sigma, float)
    delta = np.exp(-z*(lam - 1.0)*sigma/2.0)
    return PolydisperseOneYukawaMSA(lam*sigma, np.asarray(rho, float),
                                    z=z, K=K, delta=delta)


def min_contact(sigma, rho, z, K, lam):
    """min_ij g_ij of the rescaled system; np.nan where it has no solution."""
    try:
        s = rescaled_system(sigma, rho, z, K, lam)
        if np.pi/6*np.sum(s.rho*s.sigma**3) >= 0.74:
            return np.nan
        return float(np.min(s.contact_value(s.solve_gamma())))
    except Exception:
        return np.nan


def solve_rmsa(sigma, rho, z, K, lam_max=6.0, nscan=120, tol=1e-12):
    """Find the common lambda making all g_ij non-negative (min = 0).

    Returns dict with lambda, the solved system, the contact matrix, the
    rescaled volume fraction, and a status string."""
    sigma = np.asarray(sigma, float); rho = np.asarray(rho, float)
    phi = np.pi/6*np.sum(rho*sigma**3)
    g0 = min_contact(sigma, rho, z, K, 1.0)
    if np.isfinite(g0) and g0 >= 0:
        s = rescaled_system(sigma, rho, z, K, 1.0)
        sol = s.solve_gamma()
        return dict(lam=1.0, solver=s, sol=sol, g=s.contact_value(sol),
                    phi=phi, phi_rescaled=phi, min_g=g0,
                    status="no rescaling needed (MSA already physical)")
    lo, hi = 1.0, None
    for lam in np.linspace(1.0 + 1e-3, lam_max, nscan):
        v = min_contact(sigma, rho, z, K, lam)
        if np.isfinite(v) and v >= 0:
            hi = lam; break
        if np.isfinite(v):
            lo = lam
    if hi is None:
        return dict(lam=None, status=f"no lambda <= {lam_max} makes all g_ij >= 0",
                    phi=phi, min_g=g0)
    lam = brentq(lambda L: min_contact(sigma, rho, z, K, L), lo, hi, xtol=tol)
    s = rescaled_system(sigma, rho, z, K, lam)
    sol = s.solve_gamma()
    g = s.contact_value(sol)
    return dict(lam=lam, solver=s, sol=sol, g=g, phi=phi,
                phi_rescaled=phi*lam**3, min_g=float(np.min(g)),
                status="rescaled")


def rmsa_structure(res, Q):
    """S_ij^AL(Q) of the rescaled (RMSA) system."""
    sq = PolydisperseOneYukawaSq(res['solver'], res['sol'])
    return np.array([sq.S_AL(q) for q in np.atleast_1d(Q)])


if __name__ == "__main__":
    print("Polydisperse RMSA: common lambda, condition min_ij g_ij = 0\n")
    cases = [(np.array([1.0, 1.6]), 0.05, 2.0, -3.0),
             (np.array([1.0, 1.6]), 0.02, 2.0, -3.0),
             (np.array([1.0, 1.6]), 0.10, 2.0, -3.0),
             (np.array([1.0, 1.4, 2.0]), 0.05, 2.0, -3.0),
             (np.array([1.0, 1.6]), 0.05, 1.0, -3.0),
             (np.array([1.0, 1.6]), 0.05, 2.0, -0.5)]
    frac = np.array([0.6, 0.4])
    for sig, phi, Z, K in cases:
        f = frac if len(sig) == 2 else np.array([0.5, 0.35, 0.15])
        rho = f/np.sum(f*np.pi*sig**3/6)*phi
        g_msa = min_contact(sig, rho, Z, K, 1.0)
        r = solve_rmsa(sig, rho, Z, K)
        if r['lam'] is None:
            print(f"  sigma={sig} phi={phi} Z={Z} K={K}: {r['status']}")
            continue
        gg = r['g']
        print(f"  sigma={sig} phi={phi:.2f} Z={Z} K={K}")
        print(f"    min g_ij (plain MSA) = {g_msa:9.5f}   ->  lambda = {r['lam']:.5f}, "
              f"phi_rescaled = {r['phi_rescaled']:.4f}")
        print(f"    rescaled contact matrix: {np.array2string(gg, precision=6)}"
              .replace("\n", "\n" + " "*29))
        print(f"    min = {r['min_g']:.2e}   all non-negative: {np.all(gg >= -1e-10)}")
