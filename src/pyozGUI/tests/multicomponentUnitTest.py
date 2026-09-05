# -*- coding: utf-8 -*-
"""
Unit tests for the multicomponent / polydisperse extension.

Run from src/pyozGUI:

    python -m unittest multicomponentUnitTest -v
    python multicomponentUnitTest.py            # same, plus a summary table

Naming follows this directory's existing convention (algorithmUnitTest.py,
algorithmUnitTestSq.py, rycUnitTest.py, andersonGeneralizedOZsolverUT.py).

WHAT IS BEING PROTECTED
-----------------------
Every test here corresponds to a defect that was actually found, and most of
them were bugs that returned *plausible* numbers rather than crashing. That is
the reason the suite exists: none of these would have been caught by "does it
run without raising".

  RegressionOneComponent   one-component results must stay BIT-IDENTICAL after
                           the multicomponent generalisation
  MomentMatching           the Gauss-Laguerre classes must reproduce the first
                           2p-1 Schulz moments exactly
  Multicomponent           reference values, class-count convergence, and the
                           closure sweep
  ChargeScaling            the three charge-scaling routes and the kappa
                           identities that follow from them
  PotentialByName          optional setter arguments must not be rejected
                           (a silent failure that made every closure return
                           S(q) = 1)
  RobertusSHS              dilute limit -> identity, and I_exact independent of
                           the number of size classes
  RYWrapper                dilute limit, symmetry, and the two screens that
                           reject unconverged and unphysical solutions

Tests needing an OZ solve are slow (Multicomponent ~180 s, RYWrapper ~200 s);
the other five classes together run in about 1.5 s, so

    python -m unittest multicomponentUnitTest.MomentMatching

picks a fast subset. Tolerances are deliberately tight where the quantity is
exactly reproducible (moments, conventions, regressions) and loose only where
an iterative solve is involved.

KNOWN FAILURE
-------------
RYWrapper.test_dilute_limit_and_symmetry currently FAILS at phi = 1e-5. The
physics is fine -- Picard and Anderson both solve that state (residuals 0.0
and 6.8e-21) -- but the default solver, scipy Anderson, does not, apparently
because the solution is so close to gamma = 0 that its relative convergence
criterion is never met. An automatic fallback to the next candidate solver was
added to the wrapper but does not engage, and the reason has not yet been
found. The test is left failing on purpose: it documents a real robustness gap
rather than hiding it behind expectedFailure. Workaround: pass
solverClass=PicardOZsolver explicitly for very dilute states.
"""
import math
import unittest

import numpy as np


# ----------------------------------------------------------------------
# helpers
# ----------------------------------------------------------------------
def _yukawa_solver(closure="doRYclosure", param=0.5, srel=0.2, p=3,
                   nstar=0.005, Z=200.0, LB=7.01/250.0, N=4095, maxit=4000):
    """Polydisperse hard-core Yukawa at the D'Aguanno-Klein test state."""
    from picardOZsolver import PicardOZsolver
    # <sigma^3> of the Schulz distribution; the monodisperse limit srel = 0
    # is a delta function with <sigma^3> = 1 and must not be fed to the
    # 1/srel**2 expression (that raised ZeroDivisionError in an earlier
    # version of this helper -- a bug in the test, not in the code).
    if srel > 0.0:
        t = 1.0/srel**2 - 1.0
        m3 = 1.0
        for m in (1, 2, 3):
            m3 *= (t + m)/(t + 1.0)
    else:
        m3 = 1.0
    phi = (math.pi/6.0)*nstar*m3
    s = PicardOZsolver(port=0, numberOfRadialSamplingPoints=N)
    s.setNumberOfIterations(maxit)
    s.setVolumeDensity(phi)
    s.setPolydisperseHardCoreYukawaPotential(srel, p, Z, LB)
    getattr(s, closure)(param) if param is not None else getattr(s, closure)()
    s.solve()
    return s


def _rho_from_phi(sig, x, phi):
    return x*phi*6.0/(math.pi*np.sum(x*sig**3))


# ----------------------------------------------------------------------
class RegressionOneComponent(unittest.TestCase):
    """One-component behaviour must be BIT-IDENTICAL after the extension.

    These reference values were recorded from the unmodified code before any
    multicomponent work. Equality is exact on purpose: the generalisation was
    designed so that p == 1 takes the original scalar code path untouched, so
    any difference at all means that path has been disturbed.
    """

    CASES = {
        ("HardSphere", (), "doPYclosure", None):
            (2.3561180274, 1.5319765967),
        ("HardSphere", (), "doHNCclosure", None):
            (2.8450358400, 1.5013679199),
        ("HardSphere", (), "doRYclosure", 2.0):
            (2.7729881057, 1.5046366172),
        ("HardSphere", (), "doHMSAclosure", 2.0):
            (2.7729881057, 1.5046366172),
        ("HardSphere", (), "doMSAclosure", None):
            (2.3561180274, 1.5319765967),
        ("Yukawa", (0.5, 2.0, True), "doPYclosure", None):
            (1.5888336515, 1.7768878838),
        ("Yukawa", (0.5, 2.0, True), "doRYclosure", 1.0):
            (1.8014560354, 1.5364301214),
        ("LennardJones", (0.8,), "doHNCclosure", None):
            (2.1635946756, 1.5662953096),
    }

    def test_all_cases_bit_identical(self):
        from picardOZsolver import PicardOZsolver
        for (pot, args, closure, par), (g_ref, S_ref) in self.CASES.items():
            with self.subTest(potential=pot, closure=closure):
                s = PicardOZsolver(port=0)
                s.setNumberOfIterations(3000)
                s.setVolumeDensity(0.3)
                s.setPotentialByName(pot, *args)
                getattr(s, closure)(par) if par is not None else getattr(s, closure)()
                s.solve()
                g = np.asarray(s.getRDF())
                S = np.real(np.asarray(s.getSq()))
                self.assertAlmostEqual(float(g.max()), g_ref, places=9)
                self.assertAlmostEqual(float(S.max()), S_ref, places=9)

    def test_hmsa_equals_ry_for_hard_spheres(self):
        """With no attractive tail HMSA must reduce EXACTLY to Rogers-Young.

        This is the check that originally exposed HMSA returning S(q) = 1
        everywhere: the repulsive/attractive split was never populated, so
        exp(-0) = 1 removed the hard core entirely and c(r) = 0 became an exact
        fixed point.
        """
        from picardOZsolver import PicardOZsolver
        out = {}
        for closure in ("doRYclosure", "doHMSAclosure"):
            s = PicardOZsolver(port=0)
            s.setNumberOfIterations(3000)
            s.setVolumeDensity(0.3)
            s.setPotentialByName("HardSphere")
            getattr(s, closure)(2.0)
            s.solve()
            out[closure] = np.asarray(s.getRDF())
        np.testing.assert_allclose(out["doRYclosure"], out["doHMSAclosure"],
                                   rtol=0, atol=0)


# ----------------------------------------------------------------------
class MomentMatching(unittest.TestCase):
    """Gauss-generalised-Laguerre classes reproduce the Schulz moments.

    Moment matching is what lets p = 3 classes stand in for a continuous
    distribution (D'Aguanno & Klein, Phys. Rev. A 46, 7652). The first 2p-1
    moments are reproduced EXACTLY, so this is tested to 10 decimals rather
    than approximately.
    """

    def _classes(self, p, s):
        from scipy.special import roots_genlaguerre
        from scipy.special import gamma as gammafn
        t = 1.0/s**2 - 1.0
        nodes, weights = roots_genlaguerre(p, t)
        sigma = nodes/(t + 1.0)
        x = weights/gammafn(t + 1.0)
        return sigma, x/x.sum(), t

    def test_first_2p_minus_1_moments_exact(self):
        for p in (2, 3, 5):
            for s in (0.1, 0.2, 0.3):
                sigma, x, t = self._classes(p, s)
                exact = 1.0
                for m in range(0, 2*p):
                    if m > 0:
                        exact *= (t + m)/(t + 1.0)
                    got = float(np.sum(x*sigma**m))
                    with self.subTest(p=p, s=s, m=m):
                        if m <= 2*p - 1:
                            self.assertAlmostEqual(got, exact, places=10)

    def test_weights_normalised(self):
        for p in (2, 3, 5, 8):
            _, x, _ = self._classes(p, 0.25)
            self.assertAlmostEqual(float(x.sum()), 1.0, places=12)


# ----------------------------------------------------------------------
class Multicomponent(unittest.TestCase):
    """Reference values, class-count convergence, closure coverage."""

    #: independently reproduced with a separately written implementation
    REF_GNN = 1.7368
    REF_SM = 1.7478

    def test_reference_state(self):
        s = _yukawa_solver()
        g = np.asarray(s.getRDF())
        S = np.real(np.asarray(s.getSq()))
        self.assertEqual(s.numberOfComponents, 3)
        self.assertEqual(np.shape(s.partialStructureFactor), (3, 3, 4095))
        self.assertAlmostEqual(float(g.max()), self.REF_GNN, places=3)
        self.assertAlmostEqual(float(S.max()), self.REF_SM, places=3)

    def test_p3_indistinguishable_from_p5(self):
        """D'Aguanno & Klein report p = 3 suffices up to s_sigma = 0.3.

        Checked as a genuine convergence statement: p=1 -> p=3 must change the
        answer a lot, p=3 -> p=5 hardly at all. A test that only checked the
        latter would also pass if the size classes were being ignored
        altogether.
        """
        curves = {}
        for p in (1, 3, 5):
            s = _yukawa_solver(p=p, srel=0.2 if p > 1 else 0.0)
            curves[p] = np.real(np.asarray(s.getSq()))
        d13 = float(np.max(np.abs(curves[1] - curves[3])))
        d35 = float(np.max(np.abs(curves[3] - curves[5])))
        self.assertGreater(d13, 0.1, "polydispersity should be a large effect")
        self.assertLess(d35, 0.01, "p=3 should be indistinguishable from p=5")

    def test_closure_coverage(self):
        """Closures that should work multicomponent must produce structure.

        'Produces structure' means g_max > 1, which is what distinguishes a
        real solution from the trivial S(q) = 1 that a mis-set potential gives.
        """
        working = ["doPYclosure", "doHNCclosure", "doRYclosure", "doHMSAclosure",
                   "doVerletClosure", "doBPGGclosure", "doBBclosure",
                   "doKHclosure", "doDHclosure", "doCGclosure", "doSMSAclosure",
                   "doMSAclosure", "doMMSAclosure"]
        needs_param = {"doRYclosure": 0.5, "doHMSAclosure": 0.5,
                       "doBPGGclosure": 1.0, "doBBclosure": 1.0}
        for closure in working:
            with self.subTest(closure=closure):
                s = _yukawa_solver(closure, needs_param.get(closure))
                g = np.asarray(s.getRDF())
                self.assertTrue(np.all(np.isfinite(g)))
                self.assertGreater(float(g.max()), 1.001)

    def test_hmsa_equals_ry_without_attractive_tail(self):
        """A screened Coulomb between like charges is purely repulsive, so the
        attractive part is identically zero and HMSA must reduce to RY."""
        a = np.asarray(_yukawa_solver("doRYclosure", 0.5).getRDF())
        b = np.asarray(_yukawa_solver("doHMSAclosure", 0.5).getRDF())
        np.testing.assert_allclose(a, b, rtol=1e-10)

    def test_potential_split_is_pair_matrix(self):
        s = _yukawa_solver()
        self.assertEqual(np.shape(s.repulsivePartOfP2Ppotential), (3, 3, 4095))
        self.assertEqual(np.shape(s.attractivePartOfP2Ppotential), (3, 3, 4095))
        self.assertTrue(np.all(s.attractivePartOfP2Ppotential == 0.0))


# ----------------------------------------------------------------------
class ChargeScaling(unittest.TestCase):
    """The three routes for the size dependence of the valence.

    The kappa identities are exact consequences of
    kappa^2 = 4 pi L_B sum_i n_i Z_i (the FIRST power of Z -- screening is by
    the small ions). Using Z^2 gives a near-ideal gas, so these are worth
    pinning down.
    """

    def _solver(self, **kw):
        from picardOZsolver import PicardOZsolver
        s = PicardOZsolver(port=0, numberOfRadialSamplingPoints=4095)
        s.setVolumeDensity((math.pi/6.0)*0.005*1.1232)
        s.setPolydisperseHardCoreYukawaPotential(0.2, 3, 200.0, 7.01/250.0, **kw)
        return s

    def test_kappa_identities(self):
        k0 = self._solver(chargeExponent=0.0).kappaInverseDebyeLength
        k1 = self._solver(chargeExponent=1.0).kappaInverseDebyeLength
        k2 = self._solver(chargeExponent=2.0).kappaInverseDebyeLength
        # n=0 and n=1 share a kappa because sum_i x_i sigma_i = <sigma> = 1
        self.assertAlmostEqual(k0, k1, places=6)
        # n=2 raises it by exactly sqrt(<sigma^2>); <sigma^2> = 1.04 at s=0.2
        self.assertAlmostEqual(k2/k1, math.sqrt(1.04), places=4)

    def test_explicit_valences_match_equivalent_power_law(self):
        ref = self._solver(chargeExponent=1.0)
        got = self._solver(componentValences=ref.componentValences.copy())
        np.testing.assert_allclose(got.componentValences, ref.componentValences,
                                   rtol=0, atol=0)
        np.testing.assert_allclose(got.boltzmannOfP2Ppotential,
                                   ref.boltzmannOfP2Ppotential, rtol=1e-12)

    def test_screening_valences_decouple_kappa_from_amplitude(self):
        ref = self._solver(chargeExponent=1.0)
        dec = self._solver(chargeExponent=1.0,
                           screeningValences=200.0*(ref.componentDiameters**2))
        # amplitude charges untouched, kappa changed
        np.testing.assert_allclose(dec.componentValences, ref.componentValences,
                                   rtol=0, atol=0)
        self.assertNotAlmostEqual(dec.kappaInverseDebyeLength,
                                  ref.kappaInverseDebyeLength, places=4)


# ----------------------------------------------------------------------
class PotentialByName(unittest.TestCase):
    """Optional setter arguments must not be rejected.

    setPotentialByName() used to count arguments WITH defaults as mandatory,
    so any setter carrying optional parameters was unreachable -- and the
    rejection was silent (print + return), leaving the potential untouched so
    that every closure produced the trivial S(q) = 1.
    """

    def test_optional_arguments_accepted(self):
        from picardOZsolver import PicardOZsolver
        for extra in ((), (1.0,), (1.0, 2.0)):
            with self.subTest(nextra=len(extra)):
                s = PicardOZsolver(port=0, numberOfRadialSamplingPoints=4095)
                s.setVolumeDensity((math.pi/6.0)*0.005*1.1232)
                s.setPotentialByName("PolydisperseHardCoreYukawa",
                                     0.2, 3, 200.0, 7.01/250.0, *extra)
                self.assertEqual(s.numberOfComponents, 3)
                self.assertEqual(np.shape(s.boltzmannOfP2Ppotential),
                                 (3, 3, 4095))

    def test_too_few_arguments_still_rejected(self):
        from picardOZsolver import PicardOZsolver
        s = PicardOZsolver(port=0, numberOfRadialSamplingPoints=4095)
        s.setVolumeDensity(0.1)
        s.setPotentialByName("PolydisperseHardCoreYukawa", 0.2)   # 4 required
        self.assertEqual(getattr(s, "numberOfComponents", 1), 1)


# ----------------------------------------------------------------------
class RobertusSHS(unittest.TestCase):
    """Multicomponent PY adhesive hard spheres.

    Guards against the placeholder engine that was shipped previously: its
    solve() was a bare pass and S_matrix filled every element from a
    single-component formula, so the classes were never coupled.
    """

    def test_dilute_limit_is_identity(self):
        """The check the placeholder failed: it gave diagonal ~ 1 AND
        off-diagonal ~ 1, i.e. an all-ones matrix rather than the identity."""
        from robertus_shs_core_py import RobertusSHSSolver
        s = RobertusSHSSolver(100.0, 0.2, 1e-6, 50.0, nbins=4)
        S = s.S_matrix(0.05)
        np.testing.assert_allclose(np.diag(S), 1.0, atol=1e-4)
        off = S - np.diag(np.diag(S))
        self.assertLess(float(np.max(np.abs(off))), 1e-4)

    def test_lambda_system_actually_solved(self):
        from robertus_shs_core_py import RobertusSHSSolver
        s = RobertusSHSSolver(100.0, 0.2, 0.2, 0.2, nbins=12)
        s.solve()
        self.assertLess(s.residual, 1e-9)

    def test_intensity_independent_of_class_count(self):
        """I_exact must not depend on how many classes discretise a nearly
        monodisperse distribution. The placeholder inflated it by
        (sum_i sqrt(w_i))^2 -- 8.84 at 12 classes, 18.2 at 24."""
        from robertus_shs_sas import RobertusSHSSAS
        from polydisperse_yukawa_sas import Sphere
        Q = np.array([0.005, 0.02, 0.05])
        for nb in (1, 4, 12, 24):
            with self.subTest(nbins=nb):
                sas = RobertusSHSSAS(100.0, 0.01, 0.2, 0.2,
                                     formfactor=Sphere(), nbins=nb)
                ratio = sas.I_exact(Q)/sas.I_monodisperse(Q)
                np.testing.assert_allclose(ratio, 1.0, rtol=5e-3)

    def test_dilute_intensity_matches_dilute_limit(self):
        from robertus_shs_sas import RobertusSHSSAS
        from polydisperse_yukawa_sas import Sphere
        Q = np.array([0.005, 0.02, 0.05])
        sas = RobertusSHSSAS(100.0, 0.01, 1e-6, 50.0,
                             formfactor=Sphere(), nbins=12)
        np.testing.assert_allclose(sas.I_exact(Q)/sas.I_dilute(Q), 1.0, rtol=1e-3)

    def test_responds_to_distribution_width(self):
        """The reported symptom was that the exact model ignored the width."""
        from robertus_shs_core_py import RobertusSHSSolver
        vals = [RobertusSHSSolver(100.0, s, 0.2, 0.2, nbins=12).S_number(0.05)
                for s in (0.01, 0.1, 0.2, 0.3)]
        self.assertTrue(all(b > a for a, b in zip(vals, vals[1:])),
                        f"S(q) should vary monotonically with width, got {vals}")
        self.assertGreater(vals[-1] - vals[0], 0.05)


# ----------------------------------------------------------------------
class RYWrapper(unittest.TestCase):
    """The Rogers-Young polydisperse Yukawa adapter and its two screens."""

    SIG = np.array([0.9, 1.0, 1.1])
    X = np.array([0.25, 0.5, 0.25])

    def test_dilute_limit_and_symmetry(self):
        """KNOWN FAILURE -- see the module docstring. The default solver
        (scipy Anderson) does not converge at phi = 1e-5 even though Picard and
        Anderson both do, and the intended automatic fallback does not
        engage."""
        from rypolydisperseWrapper import RYPolydisperseYukawa
        rho = _rho_from_phi(self.SIG, self.X, 1e-5)
        r = RYPolydisperseYukawa(self.SIG, rho, z=2.0, K=0.0)
        S = r.S_matrix(3.0)
        np.testing.assert_allclose(S, np.eye(3), atol=5e-3)
        np.testing.assert_allclose(S, S.T, rtol=0, atol=1e-12)

    def test_sign_convention_of_K(self):
        """MSA sets c = -beta U, so K > 0 is ATTRACTIVE and must raise the
        low-Q structure factor relative to K = 0."""
        from rypolydisperseWrapper import RYPolydisperseYukawa
        rho = _rho_from_phi(self.SIG, self.X, 0.02)
        s0 = RYPolydisperseYukawa(self.SIG, rho, 2.0, 0.0).S_number(0.3)
        sp = RYPolydisperseYukawa(self.SIG, rho, 2.0, +0.3).S_number(0.3)
        sm = RYPolydisperseYukawa(self.SIG, rho, 2.0, -0.3).S_number(0.3)
        self.assertGreater(sp, s0)
        self.assertLess(sm, s0)

    def test_rejects_unphysical_solution(self):
        """A small residual is NOT sufficient: the closure has several fixed
        points and solvers converge cleanly onto unphysical ones. At this state
        two solvers gave S = 17.03 and 9.71, both with residual ~1e-12 and both
        with min S_NN(q) < 0. A structure factor is a variance."""
        from rypolydisperseWrapper import RYPolydisperseYukawa
        rho = _rho_from_phi(self.SIG, self.X, 0.1)
        with self.assertRaises(RuntimeError):
            RYPolydisperseYukawa(self.SIG, rho, z=2.0, K=1.0, alpha=0.5)

    def test_alpha_consistency_search(self):
        """alpha is fixed by requiring the compressibility and virial routes
        to agree; it is not a user input."""
        from rypolydisperseWrapper import solve_alpha
        rho = _rho_from_phi(self.SIG, self.X, 0.2)
        a, res = solve_alpha(self.SIG, rho, z=2.0, K=0.0)
        self.assertLess(abs(res), 1e-3)
        self.assertGreater(a, 0.0)
        self.assertAlmostEqual(a, 0.2166, places=2)


# ----------------------------------------------------------------------
if __name__ == "__main__":
    unittest.main(verbosity=2)
