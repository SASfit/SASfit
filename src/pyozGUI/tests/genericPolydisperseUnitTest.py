# -*- coding: utf-8 -*-
"""
Unit tests for the generic polydisperse extension: any potential x any
closure x any form factor.

Companion to multicomponentUnitTest.py, kept separate because these cover the
later generic machinery (setPolydispersePotential, polydisperse_nodes,
generic_polydisperse_sas) rather than the original multicomponent solver.

    python -m unittest genericPolydisperseUnitTest -v
    python -m unittest genericPolydisperseUnitTest.SizeClasses   # fast subset

As in the companion file, every test corresponds to a defect that actually
occurred, and most of those returned PLAUSIBLE numbers rather than crashing --
which is the whole reason for testing them. Two in particular are worth
knowing about before editing anything here:

  * A single class stored as a (1,1,N) array passed a bit-identical check on
    getRDF().max() while breaking every curve-valued caller. Tests therefore
    assert SHAPE as well as value.
  * Interpolating the Kronecker delta of S^AL onto a finer size grid turned
    the incoherent sum into a coherent one and reinstated form-factor
    oscillations. That is checked against an independent brute-force
    integration, not against the code's own previous output.

Modules that may not be present yet are skipped rather than failing, so this
file can be deployed ahead of them.
"""
import math
import unittest

import numpy as np


def _requires(name):
    try:
        return __import__(name)
    except Exception as exc:                                  # pragma: no cover
        raise unittest.SkipTest(f"{name} unavailable: {exc}")


def _solver(phi, N=4095, maxit=4000):
    from picardOZsolver import PicardOZsolver
    s = PicardOZsolver(port=0, numberOfRadialSamplingPoints=N)
    s.setNumberOfIterations(maxit)
    s.setVolumeDensity(phi)
    return s


# ----------------------------------------------------------------------
class SizeClasses(unittest.TestCase):
    """polydisperse_nodes: which quadrature rule, and the physical guard."""

    DISTS = ("Schulz", "Gaussian", "LogNormal", "Weibull")

    def test_all_distributions_moment_exact(self):
        """Every distribution must reproduce moments 0..2p-1 exactly.

        Not approximately: a p-point Gaussian rule is exact for those moments
        by construction, so anything above ~1e-12 means the rule is wrong.
        Schulz and Gaussian use closed-form classical rules; log-normal and
        Weibull need high-precision Golub-Welsch, because the moments-to-nodes
        map is ill-conditioned in float64 even though the moments themselves
        are analytic.
        """
        nodes = _requires("polydisperse_nodes")
        for dist in self.DISTS:
            for p in (2, 3, 5):
                with self.subTest(distribution=dist, p=p):
                    try:
                        err = nodes.momentError(dist, 0.2, p)
                    except ImportError as exc:
                        self.skipTest(str(exc))       # mpmath missing
                    self.assertLess(err, 1e-12)

    def test_weights_normalised_and_diameters_positive(self):
        nodes = _requires("polydisperse_nodes")
        for dist in self.DISTS:
            with self.subTest(distribution=dist):
                try:
                    sig, x = nodes.sizeClasses(dist, 0.25, 5)
                except ImportError as exc:
                    self.skipTest(str(exc))
                self.assertAlmostEqual(float(x.sum()), 1.0, places=12)
                self.assertTrue(np.all(sig > 0.0))

    def test_gaussian_guard_is_relative_not_just_positive(self):
        """Gauss-Hermite puts a node at sigma ~ 1e-4 around s = 0.35.

        That is positive, so a bare 'sigma > 0' test passes it -- but it is
        still a zero-diameter particle, and it would plant a hard core at
        sigma_ij ~ sigma_j/2 against every partner while carrying no weight.
        The guard is therefore relative to the mean, and drops such nodes.
        """
        nodes = _requires("polydisperse_nodes")
        for s in (0.35, 0.5):
            with self.subTest(s=s):
                sig, x = nodes.sizeClasses("Gaussian", s, 5)
                self.assertGreater(float(sig.min()), 1e-3)
                self.assertAlmostEqual(float(x.sum()), 1.0, places=12)
                self.assertLess(len(sig), 5)     # a node really was dropped
        # narrow distributions must keep all five
        sig, _ = nodes.sizeClasses("Gaussian", 0.2, 5)
        self.assertEqual(len(sig), 5)


# ----------------------------------------------------------------------
class GenericPotential(unittest.TestCase):
    """setPolydispersePotential: reuse of the one-component setters."""

    #The p = 1 reduction is checked with the SAME parameters as the recorded
    #one-component regression values, including Lennard-Jones at epsilon = 0.8
    #with HNC.
    CASES = (("HardSphere", (), "doPYclosure"),
             ("SquareWell", (1.0, 0.1), "doPYclosure"),
             ("LennardJones", (0.8,), "doHNCclosure"))

    #Multicomponent solves use a WEAKER Lennard-Jones well. Construction of
    #the pair matrices and convergence of the closure are separate questions,
    #and conflating them makes a test that fails for a physical reason look
    #like a broken builder: PY on a polydisperse Lennard-Jones fluid does not
    #converge at epsilon = 0.8 (NaN at both phi = 0.1 and 0.2), while
    #epsilon = 0.6 and 0.3 converge cleanly. PY is known to be poor for
    #strongly attractive tails, so this is a closure-domain limit rather than
    #a defect in the generic route -- but it is recorded here rather than
    #quietly avoided.
    SOLVE_CASES = (("HardSphere", (), "doPYclosure"),
                   ("SquareWell", (1.0, 0.1), "doPYclosure"),
                   ("LennardJones", (0.6,), "doPYclosure"))

    def test_single_class_is_bit_identical_to_one_component(self):
        """srel = 0, one class must take the ORIGINAL scalar path exactly.

        Equality is exact on purpose. This is the sharpest check that the
        generic route leaves validated physics untouched.
        """
        for pot, args, closure in self.CASES:
            with self.subTest(potential=pot):
                a = _solver(0.3)
                a.setPotentialByName(pot, *args)
                getattr(a, closure)()
                a.solve()
                b = _solver(0.3)
                b.setPolydispersePotential(pot, args, 0.0, 1)
                getattr(b, closure)()
                b.solve()
                np.testing.assert_allclose(np.asarray(a.getRDF()),
                                           np.asarray(b.getRDF()),
                                           rtol=0, atol=0)

    def test_single_class_stores_one_dimensional_arrays(self):
        """A (1,1,N) array passes a .max() check but breaks every curve.

        isMulticomponent() is false either way, so the scalar code path runs
        on three-dimensional input and every derived quantity comes back
        shaped (1,1,N). getRDF().max() is still correct, which is exactly how
        this hid; np.interp and the approximation schemes then fail with
        "object too deep for desired array".
        """
        s = _solver(0.3)
        s.setPolydispersePotential("HardSphere", (), 0.0, 1)
        s.doPYclosure()
        s.solve()
        self.assertEqual(np.ndim(s.boltzmannOfP2Ppotential), 1)
        self.assertEqual(np.ndim(np.asarray(s.getRDF())), 1)

    def test_multicomponent_builds_pair_matrices(self):
        """Construction only -- every potential must yield (p,p,N) arrays."""
        for pot, args, _ in self.CASES:
            with self.subTest(potential=pot):
                s = _solver(0.2)
                s.setPolydispersePotential(pot, args, 0.2, 3)
                self.assertEqual(np.shape(s.boltzmannOfP2Ppotential), (3, 3, 4095))
                self.assertEqual(np.shape(s.repulsivePartOfP2Ppotential), (3, 3, 4095))
                self.assertEqual(s.numberOfComponents, 3)

    def test_multicomponent_solves(self):
        """...and the solve converges where the closure is applicable."""
        for pot, args, closure in self.SOLVE_CASES:
            with self.subTest(potential=pot):
                s = _solver(0.2)
                s.setPolydispersePotential(pot, args, 0.2, 3)
                getattr(s, closure)()
                s.solve()
                g = np.asarray(s.getRDF())
                self.assertTrue(np.all(np.isfinite(g)))
                self.assertGreater(float(g.max()), 1.0)

    def test_charge_coupled_potentials_are_refused(self):
        """DLVO and friends must be rejected, not mis-modelled.

        Their amplitude scales with particle size and kappa depends on the
        whole distribution through the counterion density, so the
        identical-reduced-tail rule is simply wrong for them.
        """
        from picardOZsolver import PicardOZsolver
        for name in PicardOZsolver.CHARGE_COUPLED_POTENTIALS:
            with self.subTest(potential=name):
                s = _solver(0.2)
                s.setPolydispersePotential(name, (1.0, 10.0, 1.0, 1.0), 0.2, 3)
                self.assertEqual(getattr(s, "numberOfComponents", 1), 1)

    def test_all_distributions_solve(self):
        _requires("polydisperse_nodes")
        for dist in ("Schulz", "Gaussian", "LogNormal", "Weibull"):
            with self.subTest(distribution=dist):
                s = _solver(0.2)
                try:
                    s.setPolydispersePotential("HardSphere", (), 0.2, 3,
                                               distribution=dist)
                except ImportError as exc:
                    self.skipTest(str(exc))
                s.doPYclosure()
                s.solve()
                self.assertGreater(float(np.asarray(s.getRDF()).max()), 1.0)


# ----------------------------------------------------------------------
class NewClosures(unittest.TestCase):
    """Closures taken from OrnsteinZernike.jl, checked against its formulas."""

    GAMMA = np.linspace(-0.5, 2.0, 9)

    def _ourBridge(self, closureCall, args):
        """B = ln g + beta u - gamma, evaluated OUTSIDE a hard core."""
        s = _solver(0.3)
        s.setPotentialByName("HardSphere")
        r = s.getrArray()
        idx = np.where(r > 1.0)[0][:self.GAMMA.size]
        G = np.zeros_like(r)
        G[idx] = self.GAMMA
        getattr(s, closureCall)(*args)
        c = s.update_c(G)
        with np.errstate(invalid="ignore", divide="ignore"):
            return np.log(G[idx] + c[idx] + 1.0) - G[idx]   # beta u = 0 here

    def test_khanpour(self):
        a = 0.5
        got = self._ourBridge("doKhanpourClosure", (a,))
        want = np.log1p(a*self.GAMMA)/a - self.GAMMA
        np.testing.assert_allclose(got, want, rtol=0, atol=1e-12)

    def test_modified_verlet(self):
        a = 1.6
        got = self._ourBridge("doModifiedVerletClosure", (a,))
        g = self.GAMMA
        want = np.where(g < 0, -(g**2)/2, -(g**2)/2/(1 + a*g/2))
        np.testing.assert_allclose(got, want, rtol=0, atol=1e-12)

    def test_extended_ry_reduces_to_ry_at_zero_coefficient(self):
        """a = 0 must give plain Rogers-Young EXACTLY.

        An internal check that does not depend on the Julia transcription
        being right, and the one that matters: without it a new closure can
        silently be a duplicate of an existing one.
        """
        ery = self._ourBridge("doExtendedRYclosure", (1.0, 0.0))
        ry = self._ourBridge("doRYclosure", (1.0,))
        np.testing.assert_allclose(ery, ry, rtol=0, atol=1e-12)

    def test_extended_ry_second_parameter_has_an_effect(self):
        """...and a != 0 must actually change the answer.

        The GUI could reach alpha but not a, so Extended RY silently ran as
        plain RY -- a new closure that appeared to work while being a copy.
        """
        a0 = self._ourBridge("doExtendedRYclosure", (1.0, 0.0))
        a3 = self._ourBridge("doExtendedRYclosure", (1.0, 0.3))
        self.assertGreater(float(np.max(np.abs(a3 - a0))), 1e-3)

    def test_registered_in_gui_dropdown(self):
        ozLib = _requires("ozLib")
        for name in ("Khanpour", "Modified Verlet", "Extended Rogers-Young"):
            with self.subTest(closure=name):
                self.assertIn(name, ozLib.CLOSURE_SETTERS)
                self.assertIn(name, ozLib.CONSISTENT_PARAMETER_CLOSURES)
        self.assertIsNotNone(ozLib.secondClosureParam("Extended Rogers-Young"))
        self.assertIsNone(ozLib.secondClosureParam("Rogers-Young"))

    def test_multicomponent_closure_availability(self):
        """Closures needing a one-component reference must not be offered."""
        ozLib = _requires("ozLib")
        allowed = ozLib.multicomponentCapableClosures()
        for bad in ("Reference HNC", "Modified HNC", "Rescaled MSA", "EuRah"):
            self.assertNotIn(bad, allowed)
        for good in ("Percus-Yevick", "Rogers-Young", "HMSA"):
            self.assertIn(good, allowed)


# ----------------------------------------------------------------------
class FormFactorResolution(unittest.TestCase):
    """The structure/form-factor resolution split and the delta bug."""

    Q = np.logspace(math.log10(5), math.log10(30), 300)

    @classmethod
    def _ripple(cls, I):
        lg = np.log(I)
        trend = np.polyval(np.polyfit(np.log(cls.Q), lg, 6), np.log(cls.Q))
        return float(np.exp(np.max(lg - trend)) - np.exp(np.min(lg - trend)))

    @classmethod
    def _bruteForce(cls, srel, n=4000):
        """Independent reference: <|F|^2> by direct integration.

        Deliberately NOT computed with any of the machinery under test -- the
        whole point is an answer that does not depend on it.
        """
        from scipy.special import gammaln
        sig = np.linspace(max(1e-4, 1 - 6*srel), 1 + 6*srel, n)
        t = 1/srel**2 - 1
        w = np.exp((t + 1)*np.log(t + 1) - gammaln(t + 1)
                   + t*np.log(sig) - (t + 1)*sig)
        w = w/w.sum()
        R = sig/2.0
        out = []
        for q in cls.Q:
            x = q*R
            F = 3.0*(np.sin(x) - x*np.cos(x))/x**3*(4/3*np.pi*R**3)
            out.append(np.sum(w*F**2))
        return np.array(out)

    def test_dilute_limit_matches_brute_force(self):
        """At phi -> 0, S is the identity and I(Q) must be <|F|^2>.

        This is what exposed the real bug: interpolating S^AL WHOLE onto a
        finer size grid smears the Kronecker delta into a band, converting the
        incoherent sum_i |F_i|^2 into the coherent |sum_i F_i|^2 -- which
        oscillates. I_exact gave a ripple of 2.79 at phi = 1e-6 while
        I_dilute, which never touches S, was correct at 0.040.
        """
        gsas = _requires("generic_polydisperse_sas")
        ref = self._ripple(self._bruteForce(0.3))
        sas = gsas.GenericPolydisperseSAS(
            "HardSphere", (), phi=1e-6, srel=0.3, nbins=5,
            closure="Percus-Yevick", nFF=40)
        got = self._ripple(sas.I_exact(self.Q))
        self.assertLess(abs(got - ref), 0.05,
                        f"I_exact ripple {got:.4f} vs brute force {ref:.4f}")

    def test_ripple_converged_in_form_factor_classes(self):
        gsas = _requires("generic_polydisperse_sas")
        vals = []
        for nff in (40, 120):
            sas = gsas.GenericPolydisperseSAS(
                "HardSphere", (), phi=0.15, srel=0.3, nbins=5,
                closure="Percus-Yevick", nFF=nff)
            vals.append(self._ripple(sas.I_exact(self.Q)))
        self.assertLess(abs(vals[1] - vals[0]), 0.02)

    def test_mean_radius_is_a_pure_change_of_units(self):
        """Q*sigma and Q*R must be invariant under the length scale.

        The OZ solve stays in reduced units regardless -- its radial grid
        spans only ~41 diameters, so a physical sigma of 100 would put every
        hard core off the end of it.
        """
        gsas = _requires("generic_polydisperse_sas")
        out = []
        for R0 in (None, 50.0):
            L = 1.0 if R0 is None else 2*R0
            Q = np.linspace(1.0/L, 15.0/L, 700)
            sas = gsas.GenericPolydisperseSAS(
                "HardSphere", (), phi=0.4, srel=0.0, nbins=1,
                closure="Percus-Yevick", meanRadius=R0)
            S = sas.S_number(Q)
            out.append(Q[int(np.argmax(S))]*sas.sigma[0])
        self.assertAlmostEqual(out[0], out[1], places=6)
        self.assertAlmostEqual(out[0], 6.6, delta=0.5)   # hard spheres: ~2 pi


# ----------------------------------------------------------------------
class ConsistencySearch(unittest.TestCase):
    """The compressibility/virial routes, against analytic Percus-Yevick."""

    ETA = 0.3

    def test_routes_match_analytic_py_hard_spheres(self):
        gsas = _requires("generic_polydisperse_sas")
        eta = self.ETA
        chi_exact = (1 + 2*eta)**2/(1 - eta)**4
        bP_exact = 1 + 4*eta*(1 + eta/2)/(1 - eta)**2
        sas = gsas.GenericPolydisperseSAS(
            "HardSphere", (), phi=eta, srel=0.0, nbins=1,
            closure="Percus-Yevick")
        chi = gsas._chiInvCompressibility(sas)
        bP = gsas._betaP(sas)/sas.solver.particleDensity
        self.assertAlmostEqual(chi/chi_exact, 1.0, delta=0.06)
        self.assertAlmostEqual(bP/bP_exact, 1.0, delta=0.05)

    def test_py_residual_reproduces_known_inconsistency(self):
        """PY hard spheres have a KNOWN nonzero inconsistency, +1.2482 at
        eta = 0.3. Reproducing its size and sign is what makes the search
        trustworthy -- a residual that merely 'looks small' proves nothing."""
        gsas = _requires("generic_polydisperse_sas")
        eta = self.ETA
        f = 1 + (4*eta + 2*eta**2)/(1 - eta)**2
        fp = (4 + 8*eta)/(1 - eta)**3
        exact = (1 + 2*eta)**2/(1 - eta)**4 - (f + eta*fp)
        got = gsas.consistencyResidual(
            None, potential="HardSphere", potentialArgs=(), srel=0.0,
            nbins=1, closure="Percus-Yevick", phi=eta)
        self.assertGreater(got, 0.0)
        self.assertAlmostEqual(got/exact, 1.0, delta=0.15)

    def test_ry_residual_brackets_zero_and_tends_to_hnc(self):
        gsas = _requires("generic_polydisperse_sas")
        kw = dict(potential="HardSphere", potentialArgs=(), srel=0.0,
                  nbins=1, phi=self.ETA)
        lo = gsas.consistencyResidual(0.1, closure="Rogers-Young", **kw)
        hi = gsas.consistencyResidual(2.0, closure="Rogers-Young", **kw)
        self.assertGreater(lo, 0.0)
        self.assertLess(hi, 0.0)          # a root exists between them
        big = gsas.consistencyResidual(10.0, closure="Rogers-Young", **kw)
        hnc = gsas.consistencyResidual(None, closure="Hypernetted-Chain", **kw)
        self.assertAlmostEqual(big, hnc, delta=0.05)

    def test_solve_alpha_finds_a_consistent_value(self):
        """Consistency is judged RELATIVE to chi^-1, which is of order 10 for
        a dense fluid: an absolute residual of 1e-3 is a relative 1e-4, and
        judging it absolutely rejected a perfectly good root."""
        gsas = _requires("generic_polydisperse_sas")
        alpha, res, scale = gsas.solveAlpha(
            potential="HardSphere", potentialArgs=(), srel=0.0, nbins=1,
            closure="Rogers-Young", phi=self.ETA)
        self.assertGreater(alpha, 0.05)
        self.assertLess(alpha, 1.0)
        self.assertLess(abs(res)/scale, 1e-3)


if __name__ == "__main__":
    unittest.main(verbosity=2)
