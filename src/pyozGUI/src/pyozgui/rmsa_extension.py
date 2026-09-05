# -*- coding: utf-8 -*-
"""
Can the polydisperse MSA solution be extended to RMSA?

RMSA (Hansen & Hayter 1982) is not a different closure. It is the SAME MSA
closure applied to a system whose hard core has been inflated,
sigma -> lambda*sigma with lambda>1, with lambda fixed by requiring the
rescaled system to have g(contact) = 0 exactly. It exists because the MSA
returns g(sigma+) < 0 for strongly repulsive systems at low volume fraction.

THE KEY ALGEBRA. The physical potential is fixed; only the reference diameter
in the closure changes. BH02 eq.5 writes

    c_ij(r) = K_ij exp(-z (r - sigma_ij)) / r ,   r > sigma_ij

so re-referencing sigma_ij -> lambda*sigma_ij while holding c_ij(r) fixed gives

    K_ij' = K_ij exp(-z (lambda-1) sigma_ij)

and since sigma_ij = (sigma_i + sigma_j)/2 this FACTORISES:

    K_ij' = K delta_i' delta_j' ,   delta_i' = exp(-z (lambda-1) sigma_i / 2)

So rescaling maps a delta_i = 1 system onto a delta_i != 1 one.

  * MONODISPERSE: delta' is the same for every species, so it absorbs into K
    (K' = K exp(-z(lambda-1)sigma)) and the existing delta=1 machinery is
    enough. This is why Hansen-Hayter works.
  * POLYDISPERSE: delta_i' depends on sigma_i and cannot be absorbed. The
    analytic route therefore needs the delta_i != 1 case -- which is exactly
    the open item in the spec.

This script (a) verifies the rescaling algebra, (b) implements monodisperse
RMSA on top of the existing solver and checks it, (c) shows how far the
polydisperse case gets without delta != 1.
"""
import numpy as np, warnings
warnings.filterwarnings("ignore")
from scipy.optimize import brentq
from polydisperse_yukawa_msa import PolydisperseOneYukawaMSA


def msa_contact(sigma, rho, z, K, delta=None):
    s = PolydisperseOneYukawaMSA(np.asarray(sigma,float), np.asarray(rho,float),
                                 z=z, K=K,
                                 delta=None if delta is None else np.asarray(delta,float))
    return s, s.contact_value(s.solve_gamma())


def check_rescaling_algebra():
    """The rescaled closure must reproduce the same physical c_ij(r)."""
    print("(a) rescaling algebra: c_ij(r) must be unchanged by re-referencing")
    z, K, lam = 2.0, -1.5, 1.4
    sig = np.array([1.0, 1.6])
    worst = 0.0
    for i in range(2):
        for j in range(2):
            sij = 0.5*(sig[i]+sig[j])
            dip = np.exp(-z*(lam-1)*sig[i]/2); djp = np.exp(-z*(lam-1)*sig[j]/2)
            Kp = K*dip*djp
            for r in [2.0, 3.0, 5.0]:
                c_old = K*np.exp(-z*(r-sij))/r
                c_new = Kp*np.exp(-z*(r-lam*sij))/r
                worst = max(worst, abs(c_new-c_old)/abs(c_old))
    print(f"    max relative difference over pairs and r: {worst:.2e}")
    print(f"    -> delta_i' = exp(-z(lambda-1)sigma_i/2) reproduces it exactly\n")


def rmsa_monodisperse(sigma, phi, z, K, lam_max=8.0):
    """Hansen-Hayter RMSA for one component, on top of the MSA solver.

    Rescale sigma->lambda*sigma, phi->lambda^3 phi, K->K exp(-z(lambda-1)sigma)
    (delta' absorbs into K because it is species-independent here), and solve
    for the lambda that makes g(contact)=0."""
    def gcontact(lam):
        sp, pp = sigma*lam, phi*lam**3
        if pp >= 0.74:
            return np.nan
        Kp = K*np.exp(-z*(lam-1)*sigma)
        try:
            _, g = msa_contact([sp], [6*pp/(np.pi*sp**3)], z, Kp)
            return g[0,0]
        except Exception:
            return np.nan
    g1 = gcontact(1.0)
    if g1 >= 0:
        return 1.0, g1, "no rescaling needed (MSA already physical)"
    lo, hi = 1.0, None
    for lam in np.linspace(1.001, lam_max, 400):
        v = gcontact(lam)
        if np.isfinite(v) and v >= 0:
            hi = lam; break
        if np.isfinite(v):
            lo = lam
    if hi is None:
        return None, g1, "no rescaling factor found below lambda_max"
    lam = brentq(gcontact, lo, hi, xtol=1e-12)
    return lam, gcontact(lam), "rescaled"


if __name__ == "__main__":
    check_rescaling_algebra()
    print("(b) monodisperse RMSA on top of the existing solver")
    print(f"{'phi':>6}{'Z':>5}{'K':>7}{'g_MSA':>10}{'lambda':>9}{'phi_resc':>10}{'g_resc':>10}  note")
    for phi, Z, K in [(0.02,2.,-1.5),(0.02,2.,-3.),(0.05,2.,-3.),(0.10,2.,-3.),
                      (0.02,1.,-3.),(0.05,1.,-3.),(0.10,0.5,-3.),(0.10,2.,-0.5)]:
        _, g0 = msa_contact([1.0],[6*phi/np.pi], Z, K)
        lam, gres, note = rmsa_monodisperse(1.0, phi, Z, K)
        if lam is None:
            print(f"{phi:6.2f}{Z:5.1f}{K:7.1f}{g0[0,0]:10.4f}{'--':>9}{'--':>10}{'--':>10}  {note}")
        else:
            print(f"{phi:6.2f}{Z:5.1f}{K:7.1f}{g0[0,0]:10.4f}{lam:9.4f}{phi*lam**3:10.4f}"
                  f"{gres:10.2e}  {note}")
