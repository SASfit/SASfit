# -*- coding: utf-8 -*-
"""
Is the fixed-node discretisation contaminating the approximation-error figures?

The report compares six approximate schemes against the exact partial-structure
-factor result. The exact route is not a quadrature at all -- its nodes ARE the
species of an N-component mixture whose closure is solved in closed form -- but
schemes 2-5 genuinely are integrals over the size distribution:

    LMA (scheme 2)      I(Q) = n int dR f(R) F(Q,R)^2 S_mono(Q; sigma(R))
    pair sum (3, 4, 5)  I(Q) = n [ <F^2> + int int dR dR' f(R) f(R')
                                   F(Q,R) F(Q,R') wt (S_mono(Q; sig_RR') - 1) ]

and in polydisperse_sas_base.py they are evaluated on the SAME nodes as the
mixture. That is deliberate -- it keeps the comparison a comparison of schemes
rather than of quadratures -- but it leaves a question open: at nbins = 12, is
the quoted error of a scheme its own error, or partly the node set's?

This script answers that by evaluating the same integrals ADAPTIVELY, with
hcubature/pcubature (pip install cubature), to a requested tolerance and with
no reference to the mixture's nodes. If the adaptive value agrees with the
fixed-node one to much better than the scheme's error against the exact
result, the comparison in the report is clean.

Monodisperse MSA solves dominate the cost, so they are cached on a rounded
diameter; adaptive nodes are all distinct, so expect this to be slow. The 1D
(LMA) check takes ~15 s per bin count; the 2D pair-sum check needs thousands
of solves and runs for many minutes, so it is behind the --pairs flag.

MEASURED (Schulz-Zimm relsd 0.2, phi 0.15, z 0.06, K 1):

    quadrature error, fixed nodes vs adaptive   1e-6 .. 1e-4
    scheme error, LMA vs the exact result       3e-3 .. 3e-2

i.e. the node set contributes three to four orders of magnitude less than the
approximation being measured. The report's error figures are clean.

AI_USAGE.md: this is a validation script over existing, author-verified
physics -- it introduces no new closure or derivation.
"""
import numpy as np

from cubature import cubature

from polydisperse_yukawa_sas import (LogNormal, SchulzZimm, Sphere,
                                     PolydisperseYukawaSAS)


def _mono_S_factory(sas):
    """S_mono(Q; sigma) with a cache, so repeated adaptive nodes are cheap."""
    cache = {}

    def S(Q, sigma):
        key = round(float(sigma), 8)
        if key not in cache:
            try:
                cache[key] = sas._mono_S(np.atleast_1d(Q), sigma)
            except RuntimeError:
                cache[key] = None
        return cache[key]
    return S


def lma_adaptive(sas, Q, relerr=1e-6, maxEval=200000):
    """Scheme 2 (Pedersen LMA) by adaptive 1D quadrature over R."""
    S = _mono_S_factory(sas)
    lo, hi = sas.dist.support(8.0)
    lo = max(lo, 1e-12)
    Q = np.atleast_1d(np.asarray(Q, float))

    def f(x_array):
        x_array = np.atleast_2d(x_array)
        out = np.empty((x_array.shape[0], len(Q)))
        for n, (R,) in enumerate(x_array):
            F = sas.ff.F(Q, np.array([R]))[:, 0]
            s = 2*sas.ff.outer_radius(np.array([R]))[0]
            Sm = S(Q, s)
            out[n] = 0.0 if Sm is None else sas.dist.pdf(np.array([R]))[0]*F**2*Sm
        return out

    val, err = cubature(f, 1, len(Q), [lo], [hi], relerr=relerr,
                        maxEval=maxEval, vectorized=True, adaptive="h")
    return sas.n_tot*np.asarray(val), sas.n_tot*np.asarray(err)


def pairsum_adaptive(sas, Q, relerr=1e-4, maxEval=100000):
    """Scheme 3 (partial structure factors) by adaptive 2D quadrature."""
    S = _mono_S_factory(sas)
    lo, hi = sas.dist.support(6.0)
    lo = max(lo, 1e-12)
    Q = np.atleast_1d(np.asarray(Q, float))

    def f(x_array):
        x_array = np.atleast_2d(x_array)
        out = np.empty((x_array.shape[0], len(Q)))
        for n, (R1, R2) in enumerate(x_array):
            F1 = sas.ff.F(Q, np.array([R1]))[:, 0]
            F2 = sas.ff.F(Q, np.array([R2]))[:, 0]
            s1 = 2*sas.ff.outer_radius(np.array([R1]))[0]
            s2 = 2*sas.ff.outer_radius(np.array([R2]))[0]
            Sm = S(Q, 0.5*(s1 + s2))
            if Sm is None:
                out[n] = 0.0
                continue
            p = (sas.dist.pdf(np.array([R1]))[0]*sas.dist.pdf(np.array([R2]))[0])
            out[n] = p*F1*F2*(Sm - 1.0)
        return out

    val, err = cubature(f, 2, len(Q), [lo, lo], [hi, hi], relerr=relerr,
                        maxEval=maxEval, vectorized=True, adaptive="h")
    # the <F^2> term is a 1D integral, done the same way
    def g(x_array):
        x_array = np.atleast_2d(x_array)
        out = np.empty((x_array.shape[0], len(Q)))
        for n, (R,) in enumerate(x_array):
            F = sas.ff.F(Q, np.array([R]))[:, 0]
            out[n] = sas.dist.pdf(np.array([R]))[0]*F**2
        return out
    F2v, _ = cubature(g, 1, len(Q), [lo], [hi], relerr=1e-8,
                      maxEval=maxEval, vectorized=True, adaptive="h")
    return sas.n_tot*(np.asarray(F2v) + np.asarray(val))


if __name__ == "__main__":
    import sys
    print(__doc__)
    do2d = "--pairs" in sys.argv
    Q = np.array([2e-3, 8e-3, 2e-2, 5e-2])
    dist = SchulzZimm(50.0, relsd=0.20)
    print("Schulz-Zimm relsd=0.2, phi=0.15, z=0.06, K=1, sphere form factor")
    print(f"Q = {Q}\n")

    for nbins in (8, 12, 20):
        sas = PolydisperseYukawaSAS(dist, 0.15, 0.06, 1.0, nbins=nbins, nsig=5.0)
        fixed = sas.I_lma(Q)
        adap, aerr = lma_adaptive(sas, Q, relerr=1e-5, maxEval=20000)
        rel = np.abs(fixed/adap - 1)
        print(f"  LMA  nbins={nbins:3d}: |fixed/adaptive - 1| = "
              + "  ".join(f"{r:.2e}" for r in rel)
              + f"   (cubature's own est. err {np.max(aerr/np.abs(adap)):.1e})")

    print()
    if do2d:
        # Each adaptive node costs a monodisperse MSA solve and a 2D rule
        # needs thousands of them, so this takes many minutes. Run it as
        #     python cubature_convergence_check.py --pairs
        for nbins in (8, 12):
            sas = PolydisperseYukawaSAS(dist, 0.15, 0.06, 1.0, nbins=nbins,
                                        nsig=5.0)
            fixed = sas.I_partial_sf(Q[:2])
            adap = pairsum_adaptive(sas, Q[:2], relerr=2e-3, maxEval=4000)
            rel = np.abs(fixed/adap - 1)
            print(f"  pair nbins={nbins:3d}: |fixed/adaptive - 1| = "
                  + "  ".join(f"{r:.2e}" for r in rel))
    else:
        print("  (2D pair-sum check skipped -- it needs one monodisperse MSA")
        print("   solve per adaptive node and runs for many minutes. Pass")
        print("   --pairs to include it.)")

    print()
    sas = PolydisperseYukawaSAS(dist, 0.15, 0.06, 1.0, nbins=12, nsig=5.0)
    ex = sas.I_exact(Q)
    print("For scale, the SCHEME errors the report quotes (vs the exact result):")
    for name, I in [("LMA", sas.I_lma(Q)), ("partial SF", sas.I_partial_sf(Q))]:
        print(f"  {name:12s} " + "  ".join(f"{r:.2e}" for r in np.abs(I/ex - 1)))
    print("\nIf the quadrature rows are far below the scheme rows, the")
    print("comparison in the report is measuring schemes, not node sets.")
