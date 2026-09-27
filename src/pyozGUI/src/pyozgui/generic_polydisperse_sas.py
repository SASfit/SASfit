# -*- coding: utf-8 -*-
"""
Exact I(Q) for ANY polydisperse potential/closure combination.

Bridges two pieces that already exist:

  * oZfixpointOperator.setPolydispersePotential(), which turns any of the
    eighteen one-component setXXXPotential() methods into a multicomponent
    (p,p,N) pair potential (additive cores sigma_ij = (sigma_i+sigma_j)/2 and
    an identical reduced tail u_ij(r) = u(r/sigma_ij)), and
  * PolydisperseSASBase, which supplies the exact I(Q) plus all six of
    SASfit's approximate schemes for combining a structure factor with a size
    distribution.

The result is that "any potential x any closure x any form factor" needs no
new physics here -- only the plumbing below, plus the two convention
conversions that have caused real bugs in this project before and are
therefore each done in exactly one place:

  * D'Aguanno -> Ashcroft-Langreth,  S^AL_ij = S^DAG_ij / sqrt(x_i x_j).
    PolydisperseSASBase.I_exact() forms G = F*sqrt(rho) and contracts G S G,
    which requires the AL convention (diagonal -> 1 in the dilute limit).
    The solver stores the number-fraction form (diagonal -> x_i). Getting
    this wrong inflates I(Q) by a factor that depends only on the number of
    size classes, which is easy to mistake for a physical effect.
  * interpolation from the solver's own fixed q grid onto the requested Q.

Charge-coupled potentials (DLVO, DLVOHydra, IonicMicrogel) are NOT available
through this route: their amplitude scales with particle size and kappa
depends on the whole distribution, so the reduced-tail mixing rule is wrong
for them. setPolydispersePotential() refuses them. The dedicated polydisperse
hard-core Yukawa model is reached through its own route instead:
solver.setPolydisperseHardCoreYukawaPotential(), wrapped by
rypolydisperseWrapper.RYPolydisperseYukawa.
"""
import numpy as np

from polydisperse_sas_base import PolydisperseSASBase
from polydisperse_yukawa_sas import Sphere
import ozLib


class GenericPolydisperseSAS(PolydisperseSASBase):
    """Exact I(Q) for a polydisperse potential solved numerically.

    ::

        potential      : any name from solver.getAvailablePotentialNames()
                         that is not charge-coupled, e.g. "HardSphere",
                         "SquareWell", "StickyHardSphere", "LennardJones",
                         "SoftSphere", ...
        potentialArgs  : that setter's own arguments, in reduced units
        closure        : any ozLib.CLOSURE_SETTERS label that works
                         multicomponent
        closureParam   : the closure's alpha/eta, when it needs one
        closureParam2  : second closure scalar where declared (Extended RY's a)
    """

    def __init__(self, potential, potentialArgs, phi, srel, nbins=3,
                 closure="Percus-Yevick", closureParam=None, closureParam2=None,
                 formfactor=None, meanDiameter=1.0,
                 solverClass=None, gridN=4095, pointsPerSigma=100,
                 onSolverCreated=None, mannAlpha=None,
                 maxIterations=6000, converged_tol=1e-6,
                 nFF=None, distribution="Schulz", meanRadius=None,
                 transformType=1):
        """nFF: number of size classes used for the FORM-FACTOR average.

        The structure factor and the form-factor average need very different
        resolutions in sigma, because they behave differently there::

          S_ij(Q)   varies SMOOTHLY with size, so 3-5 moment-matched classes
                    already reproduce it -- and the OZ solve costs O(p^2)
                    pair transforms, so p wants to stay small.
          <|F|^2>   OSCILLATES: at high Q the phase Q*R spans about
                    Q*<sigma>*s radians across the distribution, so a handful
                    of classes superposes a handful of ringing form factors
                    instead of smearing them. Measured with s = 0.3 up to
                    Q = 30, the relative ripple stays ~1.9 from 3 to 12
                    classes and only falls to 0.71 by 30 -- i.e. no amount of
                    moment-exactness fixes it, because the integrand is not
                    polynomial in R.

        So the two averages are decoupled: the OZ equations are solved on
        `nbins` classes and the resulting S_ij is interpolated onto `nFF`
        classes for the intensity sum. Interpolating S is safe precisely
        because it varies slowly in sigma. Rule of thumb for choosing nFF:
        nFF >~ Q_max*<sigma>*s.

        SOLVER AND GRID. `solverClass` selects the fixed-point solver and
        `mannAlpha` sets the damping of the Picard/Mann iteration,
        x <- (1-a) x + a T(x). At a = 1 that is plain Picard, which diverges
        above about phi = 0.45; below 1 it is Mann's iteration, slower but
        convergent where Picard is not -- it rescued a Lennard-Jones case at
        a = 0.5. Solvers that choose their own step ignore it.

        `gridN` and `pointsPerSigma` must be SCALED TOGETHER when refining.
        The real-space range is r_max = N*sigma/pointsPerSigma, so raising
        the resolution at fixed N shrinks the box: the result improves near
        contact and degrades at low Q, and a convergence test done that way
        measures the trade rather than the discretisation.

        `transformType` is 1 (DST-I) or 4 (DST-IV), and the choice matters
        more than it looks. Type 1 places grid points ON the hard core, so
        the discontinuity is carried by a node that is neither inside nor
        outside and the excluded volume is wrong at FIRST order; type 4
        places them at (n+1/2)dr, the core falls between points, and the
        step is resolved to second order. Measured against the exact
        Percus-Yevick compressibility at phi = 0.40 and 100 points per
        diameter: type 1 gives S(0) low by 5.27 per cent, type 4 by 0.036 --
        a factor of 148 on the same grid, and the error is independent of
        r_max. Type 4 is correct only for ONE size class, however, since it
        needs every pair core between grid points and the sigma_ij of a
        mixture are mutually incommensurate. Use
        `oZfixpointOperator.bestGridSize(n, transformType)` rather than
        choosing N by hand: type 1 wants 2^k - 1 and type 4 wants 2^k.
        """
        self.potential = potential
        self.potentialArgs = tuple(potentialArgs)
        self.phi = float(phi)
        self.srel = float(srel)
        self.closure = closure
        self.closureParam = closureParam
        self.closureParam2 = closureParam2
        self.ff = formfactor if formfactor is not None else Sphere()
        self.meanDiameter = float(meanDiameter)
        #PolydisperseSASBase.I_lma() reports skipped bins as (i, sigma,
        #z*sigma, weight) -- a Yukawa-specific leak in an otherwise
        #model-agnostic base class, and its ONLY use of self.z. Zero is
        #honest for a potential that has no screening parameter: the
        #diagnostic then shows 0 rather than inventing a number.
        self.z = 0.0
        #transformType: 1 (default) or 4. Type 4 uses a half-offset grid and
        #is SECOND order rather than first, because the hard core then falls
        #between grid points instead of on one -- about 140x more accurate at
        #100 points per diameter. It is not yet the default because switching
        #moves every recorded reference value; see docs/NEXT_SESSION.md.
        #
        #For a MIXTURE it additionally requires that every pair core sigma_ij
        #sit between grid points. That is checked in _solve() and raises if
        #violated, because a misaligned type-4 grid silently drops back to
        #first order.
        if transformType not in (1, 4):
            raise ValueError(
                f"transformType must be 1 or 4, got {transformType!r}")
        self._transformType = int(transformType)
        self._solverKw = dict(solverClass=solverClass, gridN=gridN,
                              pointsPerSigma=pointsPerSigma,
                              maxIterations=maxIterations,
                              converged_tol=converged_tol)

        self.distribution = distribution
        #Physical length scale. The OZ equations are ALWAYS solved in reduced
        #units (mean diameter = 1): the solver's radial grid spans only about
        #41 diameters, so handing it a physical sigma of, say, 100 would put
        #every hard core off the end of the grid. The scale is therefore
        #applied AFTER the solve, to the diameters, the number densities and
        #the q axis -- none of which changes the physics, because phi and the
        #reduced tail parameters are scale invariant.
        #
        #meanRadius=None keeps everything dimensionless, so Q means Q*sigma.
        #Giving a mean radius makes Q a genuine inverse length, matching the
        #convention of the polydisperse Yukawa tab.
        self.meanRadius = None if meanRadius is None else float(meanRadius)
        #Damping for the Picard/Mann iteration; None leaves the solver's own
        #default (1.0, i.e. undamped Picard).
        self.mannAlpha = mannAlpha
        self._L = 1.0 if self.meanRadius is None else 2.0*self.meanRadius
        #Set BEFORE _solve, since that is where the hook fires.
        if onSolverCreated is not None:
            self.onSolverCreated = onSolverCreated
        sol = self._solve(srel, nbins, self.phi)
        self.solver = sol
        #Coarse classes: what the OZ equations were actually solved on.
        self.sigmaS = np.asarray(sol.componentDiameters, float).copy()*self._L
        self._q = np.asarray(sol.getqArray(), float)
        self._S_AL = self._toAshcroftLangreth(sol)

        #Fine classes for the form-factor average. Defaults to the coarse set,
        #so behaviour is unchanged unless nFF is asked for.
        if nFF is None or int(nFF) <= len(self.sigmaS) or srel <= 0.0:
            self.sigma = self.sigmaS.copy()
            #n scales as 1/L^3 so that phi = (pi/6) n <sigma^3> is preserved.
            self.rho = np.asarray(sol.componentDensities, float).copy()/self._L**3
            self._fine = False
        else:
            #A DENSE, near-equispaced grid -- deliberately NOT another
            #Gaussian rule. Gauss nodes cluster where they optimise polynomial
            #exactness, which is precisely the wrong criterion for an
            #oscillatory integrand: using sizeClasses() here made the high-Q
            #ripple WORSE, not better (wiggle 1.91 -> 3.14 at nFF = 20),
            #because the outermost nodes sit far out with tiny weights and
            #ring at very high Q*R. Smearing needs the oscillation RESOLVED,
            #so the nodes must be spaced finely across the support.
            sig_f, x_f = self._denseGrid(
                distribution, srel, int(nFF), meanDiameter,
                hull=(self.sigmaS.min()/self._L, self.sigmaS.max()/self._L))
            self.sigma = sig_f*self._L
            #Same total number density, redistributed over the fine classes.
            self.rho = float(np.sum(sol.componentDensities))*x_f/self._L**3
            self._rhoCoarse = (np.asarray(sol.componentDensities, float).copy()
                               /self._L**3)
            self._fine = True
        self.R = self.ff.outer_radius(self.sigma/2.0)
        self.n_tot = float(self.rho.sum())
        self.w = self.rho/self.n_tot
        self._mono_cache = {}

    # ------------------------------------------------------------------
    def _makeSolver(self, gridN, pointsPerSigma, solverClass, maxIterations):
        if solverClass is None:
            #DEFAULT TO WHATEVER ozLib DEFAULTS TO, rather than hardcoding a
            #solver here. This used to fall back to PicardOZsolver, which is
            #the SLOWEST of the nine available (0.068 s against SUNDIALS
            #KIN_FP's 0.0053 on the same problem) and diverges outright above
            #phi = 0.42, while ozLib.solve() has defaulted to KIN_FP for some
            #time. So a script calling this class directly, and the
            #polydisperse tab before a solver is chosen, silently got a
            #different and much weaker solver than ozLib.solve() would have
            #given -- which is why numerics_test.py prints pages of Picard
            #convergence messages for a package whose default is not Picard.
            #
            #Reading ozLib's own registry keeps the two in step by
            #construction instead of by two places happening to agree.
            try:
                import ozLib
                solverClass = next(iter(ozLib.SOLVER_CLASSES.values()))[0]
            except Exception:
                from picardOZsolver import PicardOZsolver
                solverClass = PicardOZsolver
        sol = solverClass(port=0, numberOfRadialSamplingPoints=gridN,
                          hardSphereDiameterInPoints=pointsPerSigma)
        #Mann damping, where the solver supports it. x_{n+1} =
        #(1-a) x_n + a T(x_n): a = 1 is plain Picard, below 1 is Mann's
        #iteration -- slower, but convergent where undamped Picard diverges.
        #Set with getattr/hasattr because only the Picard solver has it; the
        #accelerated solvers manage their own step.
        a = getattr(self, "mannAlpha", None)
        if a is not None:
            #Set UNCONDITIONALLY, not behind hasattr(sol, "mannAlpha").
            #PicardOZsolver reads it with getattr(self, 'mannAlpha', 1.0),
            #so the attribute does not exist until something assigns it --
            #and the hasattr guard therefore never fired, leaving the
            #damping entry with no effect at all. Solvers that manage their
            #own step simply ignore the attribute.
            sol.mannAlpha = float(a)
        #transformType must be set BEFORE the potential, because getrArray()
        #depends on it: type 4 puts the grid at (n+1/2)*Delta_r rather than
        #(n+1)*Delta_r, so a potential built under one and transformed under
        #the other is evaluated on the wrong radii.
        sol.transformType = self._transformType
        sol.setNumberOfIterations(maxIterations)
        return sol

    def _applyClosure(self, sol):
        setterName, needsParam = ozLib.CLOSURE_SETTERS[self.closure]
        if self.closure in ozLib.MULTICOMPONENT_INCAPABLE_CLOSURES:
            raise RuntimeError(
                f"closure {self.closure!r} cannot be used with a polydisperse "
                "potential: it needs a one-component reference solve or is "
                "hard-sphere specific (see "
                "ozLib.MULTICOMPONENT_INCAPABLE_CLOSURES).")
        extra = ozLib.secondClosureParam(self.closure)
        if needsParam and extra is not None and self.closureParam2 is not None:
            getattr(sol, setterName)(self.closureParam, self.closureParam2)
        elif needsParam:
            if self.closureParam is None:
                raise RuntimeError(
                    f"closure {self.closure!r} needs a parameter value")
            getattr(sol, setterName)(self.closureParam)
        else:
            getattr(sol, setterName)()

    #Optional callable invoked with the freshly built solver, BEFORE it is
    #configured and solved. Set it on the instance-to-be via the class or a
    #subclass; PolydisperseFit uses it to seed a warm start.
    #
    #It has to be a hook here rather than something the caller does after
    #construction, because __init__ SOLVES: by the time the constructor
    #returns, sas.solver has already run and seeding it is too late. That
    #cost a round of debugging when the warm start silently did nothing.
    onSolverCreated = None

    def _solve(self, srel, nbins, phi):
        kw = self._solverKw
        sol = self._makeSolver(kw["gridN"], kw["pointsPerSigma"],
                               kw["solverClass"], kw["maxIterations"])
        sol.setVolumeDensity(phi)
        if self.potential == "PolydisperseHardCoreYukawa":
            #Charge-coupled: NOT reachable through setPolydispersePotential(),
            #which refuses it. Two things break the identical-reduced-tail
            #rule for a charged system:
            #  * the amplitude is per-species, A_i = Z_i sqrt(L_B)
            #    exp(kappa sigma_i/2)/(1 + kappa sigma_i/2) with Z_i scaling as
            #    sigma_i^n, so pairs do NOT share one reduced tail;
            #  * kappa is a GLOBAL quantity, kappa^2 = 4 pi L_B sum_i n_i Z_i,
            #    fixed by the whole distribution through the counterion
            #    density rather than by any single pair.
            #
            #Its own setter already handles both, so the charged model is
            #reached by dispatching to it. Everything downstream -- closure,
            #form factor, the six approximation schemes, the plots -- is
            #generic and needs no change, because it only ever sees the
            #assembled (p,p,N) potential.
            #
            #potentialArgs are (relativeStandardDeviation is taken from srel,
            #numberOfComponents from nbins):
            #    (referenceValence, bjerrumLengthInSigmaUnits, chargeExponent)
            #chargeExponent is the sigma^n scaling: 2 = constant surface
            #charge density (D'Aguanno-Klein), 1 = linear in size
            #(renormalisation saturated), 0 = size independent.
            args = list(self.potentialArgs) + [200.0, 7.01/250.0, 2.0]
            Z, LB, chargeExp = args[0], args[1], args[2]
            sol.setPolydisperseHardCoreYukawaPotential(
                srel, nbins, Z, LB, self.meanDiameter, chargeExp)
        else:
            sol.setPolydispersePotential(self.potential, self.potentialArgs,
                                         srel, nbins, self.meanDiameter,
                                         self.distribution)
        if getattr(sol, "numberOfComponents", 1) < 1:
            raise RuntimeError("potential setup failed")
        #Type 4 is second order ONLY when every pair core sigma_ij falls
        #between grid points. For one component that is automatic; for a
        #mixture sigma_ij takes p(p+1)/2 values and alignment has to be
        #checked. A misaligned type-4 grid silently reverts to FIRST order --
        #measured at three orders of magnitude worse -- so this raises rather
        #than warns, and suggests a pointsPerSigma that works.
        #
        #Must be called HERE, after the potential has set componentDiameters,
        #not in _makeSolver where they do not exist yet.
        if self._transformType == 4:
            sol.checkTransformAlignment()
        self._applyClosure(sol)
        #The hook fires HERE, immediately before the solve, not when the
        #solver was created. Setting the density, the potential and the
        #closure all reset the solver's starting vector, so a seed applied
        #at creation time is accepted and then silently overwritten -- the
        #iteration counts gave it away, unchanged at 166 either way.
        if callable(self.onSolverCreated):
            self.onSolverCreated(sol)
        sol.solve()
        # Never trust the driver: picardIteration() prints a warning on
        # failure but returns results anyway, with no flag for callers. Check
        # the residual of the fixpoint map itself.
        self._verify(sol)
        self._verifyPhysical(sol)
        return sol

    def _verifyPhysical(self, sol):
        """Reject a converged but UNPHYSICAL solution.

        The residual check above establishes that the solver found a fixed
        point. It does not establish that the fixed point is a physical one,
        and those are different questions: the closure equations admit
        several solutions, and at a Lennard-Jones state tested during
        development every Newton-Krylov variant converged to residuals below
        1e-11 on branches with min S(Q) near -38. Genuine roots, entirely
        unphysical.

        A structure factor is a variance, so S(Q) >= 0 for every Q. That is
        the criterion the bifurcation literature uses to discard branches,
        and it is the only cheap discriminator available here -- g_max, by
        contrast, looks perfectly reasonable on those branches (1.43 against
        a correct 2.16), so screening the wrong quantity catches nothing.

        This matters most during a FIT, where the optimiser explores
        parameter space and will follow a wrong branch happily, reporting a
        good chi-squared. Raising here turns that into a failed evaluation,
        which the fitter already handles by penalising the point.
        """
        try:
            S = np.real(np.asarray(sol.getSq(), float))
        except Exception:
            return              # nothing to screen; the residual check stands
        if S.size == 0 or not np.all(np.isfinite(S)):
            return
        worst = float(np.min(S))
        #A small negative excursion is discretisation noise, not a different
        #branch: the wrong branches are negative by tens, not by 1e-9.
        if worst < -1e-6:
            raise RuntimeError(
                f"{self.closure} converged to an UNPHYSICAL solution for "
                f"{self.potential} at phi={self.phi:g}: min S(Q) = "
                f"{worst:.4g} < 0. The residual is small, so this is a "
                f"genuine root of the closure equations on a "
                f"negative-compressibility branch, not a failed solve.")

    def _verify(self, sol):
        tol = self._solverKw["converged_tol"]
        if getattr(sol, "numberOfComponents", 1) > 1:
            if not hasattr(sol, "gammaMatrixMulticomponent"):
                raise RuntimeError(
                    f"{self.closure} solve produced no fixpoint for "
                    f"{self.potential} at phi={self.phi:g}")
            flat = sol.packPairs(sol.gammaMatrixMulticomponent)
            image = sol.fixPointOperatorForGammaMulticomponent(flat)[0]
            if not np.all(np.isfinite(image)):
                raise RuntimeError(f"{self.closure} solve diverged for "
                                   f"{self.potential} at phi={self.phi:g}")
            res = float(np.max(np.abs(image - flat)))
            if res > max(tol*1e3, 1e-6):
                raise RuntimeError(
                    f"{self.closure} solve did not converge for "
                    f"{self.potential} at phi={self.phi:g} "
                    f"(max|G(gamma)-gamma| = {res:.3e})")
        else:
            #One component: the base solver does NOT expose its converged
            #Gamma (x_0 stays the START value, and picardIteration() feeds the
            #solution straight into derivePhysicalQuantitiesFromFixpoint
            #without storing it), so the residual test above is not available.
            #Checking that the derived curves are finite is what can honestly
            #be checked here. An earlier version tested the residual against
            #x_0 and therefore rejected EVERY monodisperse reference solve --
            #which surfaced as all six approximation schemes failing with
            #"no monodisperse MSA solution", a misleading message from the
            #base class rather than a real physical obstruction.
            g = np.asarray(sol.getRDF(), float)
            S = np.real(np.asarray(sol.getSq(), float))
            if not (np.all(np.isfinite(g)) and np.all(np.isfinite(S))):
                raise RuntimeError(
                    f"{self.closure} monodisperse reference solve failed for "
                    f"{self.potential} at phi={self.phi:g}")


    @staticmethod
    def _denseGrid(distribution, srel, n, meanSigma, nsig=4.0, hull=None):
        """Dense trapezoidal grid over the distribution, weights from its pdf.

        Spans mean +/- nsig standard deviations (clipped at a small positive
        diameter, since a hard core cannot sit at sigma <= 0), and normalises
        the weights so that sum x_i = 1 exactly.
        """
        lo = max(meanSigma*(1.0 - nsig*srel), 1e-3*meanSigma)
        hi = meanSigma*(1.0 + nsig*srel)
        if hull is not None:
            #Stay INSIDE the coarse hull. S_ij is only known at the coarse
            #classes, so a fine node outside them can only be clamped -- and
            #clamping a wide interval to a single S value re-introduces
            #exactly the ringing this grid exists to remove. Measured: with
            #the fine grid running 0.001..2.20 against a coarse hull of
            #0.496..2.477, I_exact showed a ripple of 2.79 even at
            #phi = 1e-6, where S must be ~1, while I_dilute (which never
            #touches S) was correct at 0.040.
            lo = max(lo, hull[0])
            hi = min(hi, hull[1])
        sig = np.linspace(lo, hi, int(n))
        if distribution == "Schulz":
            t = 1.0/srel**2 - 1.0
            zp = t + 1.0
            from scipy.special import gammaln
            u = sig/meanSigma
            logf = (zp*np.log(zp) - gammaln(zp) + t*np.log(u) - zp*u)
            pdf = np.exp(logf)
        elif distribution == "LogNormal":
            s2 = np.log(1.0 + srel*srel)
            mu = np.log(meanSigma) - 0.5*s2
            pdf = np.exp(-0.5*(np.log(sig) - mu)**2/s2)/(sig*np.sqrt(2*np.pi*s2))
        elif distribution == "Gaussian":
            sd = srel*meanSigma
            pdf = np.exp(-0.5*((sig - meanSigma)/sd)**2)/(sd*np.sqrt(2*np.pi))
        elif distribution == "Weibull":
            from polydisperse_nodes import _weibull_shape_from_cv
            from scipy.special import gamma as gammafn
            k = _weibull_shape_from_cv(srel)
            lam = meanSigma/gammafn(1.0 + 1.0/k)
            z = sig/lam
            pdf = (k/lam)*z**(k - 1.0)*np.exp(-z**k)
        else:
            raise ValueError("unknown distribution %r" % (distribution,))
        w = pdf/pdf.sum()
        return sig, w

    @staticmethod
    def _toAshcroftLangreth(sol):
        """(N,N,nq) D'Aguanno partials -> Ashcroft-Langreth. See module docstring."""
        p = getattr(sol, "numberOfComponents", 1)
        if p == 1:
            Sq = np.real(np.asarray(sol.getSq(), float)).reshape(1, 1, -1)
            return Sq
        Sdag = np.asarray(sol.partialStructureFactor, float)
        x = np.asarray(sol.componentFractions, float)
        return Sdag/np.sqrt(np.outer(x, x))[:, :, None]

    # ------------------------------------------------------------------
    def _interpolateToFine(self, Scoarse):
        """(nc,nc) partial matrix on the coarse sigma set -> (nf,nf) on the fine one.

        Bilinear in (sigma_i, sigma_j), clamped at the ends. Legitimate only
        because S_ij varies slowly with size -- which is exactly the property
        that lets the OZ solve use few classes in the first place. The form
        factor, which does NOT vary slowly, is never interpolated: it is
        evaluated directly at the fine diameters.
        """
        cs = self.sigmaS
        if cs.size == 1:
            return np.full((self.sigma.size, self.sigma.size), Scoarse[0, 0])
        idx = np.clip(np.searchsorted(cs, self.sigma) - 1, 0, cs.size - 2)
        t = (self.sigma - cs[idx])/(cs[idx + 1] - cs[idx])
        t = np.clip(t, 0.0, 1.0)
        A = (1 - t)[:, None]*(1 - t)[None, :]
        B = t[:, None]*(1 - t)[None, :]
        C = (1 - t)[:, None]*t[None, :]
        D = t[:, None]*t[None, :]
        i0, i1 = idx[:, None], (idx + 1)[:, None]
        j0, j1 = idx[None, :], (idx + 1)[None, :]
        return (A*Scoarse[i0, j0] + B*Scoarse[i1, j0]
                + C*Scoarse[i0, j1] + D*Scoarse[i1, j1])

    def _crInterpolator(self):
        """Catmull-Rom spline over the whole partial-structure matrix.

        Built lazily and cached: construction costs about 90 ms at the
        standard 4095-point grid, against a few microseconds per evaluation,
        and a single fit makes hundreds of evaluations. Building it per call
        would cost more than the linear interpolation it replaces.

        SAFE TO CACHE because `_S_AL` is assigned exactly once, in
        `__init__` -- one solve per instance. If that ever changes, whatever
        reassigns it must also do `self.__dict__.pop("_crSpline", None)`, or
        this will go on interpolating the previous solution and return a
        plausible curve for the wrong state point.

        Returns None if `splines` is not installed or the grid is too short
        for a four-point stencil; the caller falls back to linear, which is
        less accurate rather than wrong.
        """
        cached = getattr(self, "_crSpline", "unset")
        if cached != "unset":
            return cached
        spline = None
        try:
            import splines
            nq = self._q.size
            if nq >= 4:
                n = self._S_AL.shape[0]
                flat = self._S_AL.reshape(n*n, nq).T
                spline = splines.CatmullRom(list(flat), alpha=0,
                                            endconditions="natural")
        except Exception:
            spline = None
        self._crSpline = spline
        return spline

    def S_partials(self, Q):
        """(nQ, N, N) Ashcroft-Langreth partials, interpolated onto Q.

        BELOW the solver's own q grid the partials are EXTRAPOLATED, not
        clamped. S(q) for a liquid is even and analytic at the origin --
        the transform of a short-ranged radially symmetric h(r) has no odd
        term -- so S_ij(q) = A + B q^2 is the leading behaviour rather than
        a convenient fit. A and B come from a least-squares fit over the
        whole quadratic region (q*sigma <= 1), not from the two lowest
        points: the grid is uniform in q with spacing pi/r_max, so a finer
        grid puts MORE points below any given q, and a two-point estimate
        would determine B from the difference of two nearly equal numbers
        and get worse as the grid is refined.

        Clamping instead, which this did before, holds the partials FLAT
        below the grid. Smearing the lowest measured points pulls values in
        from there, so a structure factor with a strong low-q upturn was
        biased towards its value at q_0, and the bias grew with dQ.

        A DIVERGENCE IS REFUSED rather than extrapolated. S ~ c q^-alpha
        means the compressibility is running away -- at or near a spinodal
        -- and there is no finite S(0) to extrapolate TO, so a parabola
        through it returns a number that means nothing and the fit
        downstream would use it without complaint. The discriminant is the
        log-log slope of the trace over the lowest few grid points, with a
        threshold of -0.1: hard spheres at phi = 0.10, 0.30 and 0.45 give
        +0.0009, -0.0000 and -0.0008, while a synthetic S proportional to
        1/q gives -1.000 exactly, so there are three orders of headroom.
        Extending the q grid removes the need to extrapolate at all and the
        error message says so.

        ABOVE the grid the partials go to delta_ij, since S -> 1 when the
        wavelength is short against any correlation.
        """
        #Incoming Q is in the caller's units; the stored grid is reduced, so
        #convert with q_reduced = Q * (mean physical diameter).
        Q = np.atleast_1d(np.asarray(Q, float))*self._L
        n = self._S_AL.shape[0]
        out = np.empty((Q.size, n, n))
        eye = np.eye(n)
        #IS THE LOW-q END EXTRAPOLABLE AT ALL? Decided once, before the loop.
        #
        #A liquid's S(q) is analytic at the origin, S(0) = rho k_B T chi_T
        #finite, so a Taylor form A + B q^2 is the leading behaviour and
        #extrapolating to it is legitimate. A DIVERGENCE, S ~ c q^-alpha, is a
        #different physical situation: the compressibility is running away,
        #the system is at or near a spinodal or critical point, and there is
        #no S(0) to extrapolate TO. Fitting a parabola through it returns a
        #finite number that means nothing, and the fit downstream would use
        #it without complaint.
        #
        #The discriminant is the log-log slope of the trace over the lowest
        #few grid points. Near zero: analytic, extrapolate. Clearly negative:
        #a power law, and better to refuse than to invent a value.
        if not hasattr(self, "_lowQslope"):
            m = min(5, self._q.size)
            tr = np.trace(self._S_AL[:, :, :m]).astype(float)
            good = tr > 0
            if np.count_nonzero(good) >= 3:
                self._lowQslope = float(np.polyfit(
                    np.log(self._q[:m][good]), np.log(tr[good]), 1)[0])
            else:
                self._lowQslope = 0.0
        if self._lowQslope < -0.1 and np.any(Q <= self._q[0]):
            raise RuntimeError(
                f"S(q) diverges at low q (log-log slope {self._lowQslope:.3f} "
                f"over the lowest grid points, i.e. S ~ q^{self._lowQslope:.2f}"
                f"), so there is no finite S(0) to extrapolate to and a "
                f"Taylor form would invent one. The compressibility is "
                f"running away: the state point is at or near a spinodal. "
                f"Move away from it, or extend the q grid below "
                f"{self._q[0]/self._L:.4g} so that no extrapolation is "
                f"needed.")
        for k, q in enumerate(Q):
            if q <= self._q[0]:
                #GUINIER-LIKE EXTRAPOLATION, not a clamp.
                #
                #S(q) for a liquid is an even, analytic function of q at
                #small q -- the Fourier transform of a short-ranged, radially
                #symmetric h(r) has no odd term -- so
                #
                #    S_ij(q) = A_ij + B_ij q^2 + O(q^4)
                #
                #is the leading behaviour rather than a convenient fit.
                #
                #A AND B COME FROM A LEAST-SQUARES FIT OVER THE WHOLE
                #QUADRATIC REGION, not from the two lowest points. The grid is
                #uniform in q with spacing pi/r_max, so a finer grid puts MORE
                #points below any given q -- at r_max = 41 sigma the spacing
                #is about 0.077/sigma and there are a dozen points before
                #q sigma = 1. Taking only q_0 and q_1 would determine B from
                #the difference of two nearly equal numbers, and would get
                #WORSE as the grid is refined, which is precisely backwards.
                #
                #The fit window is q sigma <= 1, where the quartic term is
                #still small, capped so a very fine grid does not spend
                #thousands of points on it, and floored at two so a coarse
                #grid still has something to fit.
                if not hasattr(self, "_lowQfit"):
                    m = int(np.searchsorted(self._q, 1.0))
                    m = max(2, min(m, 24, self._q.size))
                    qf = self._q[:m]
                    #Columns [1, q^2]: no linear term, by the parity argument
                    #above. Including one would fit noise and break the
                    #symmetry S(q) = S(-q) that the transform guarantees.
                    M = np.stack([np.ones_like(qf), qf*qf], axis=1)
                    coef, *_ = np.linalg.lstsq(
                        M, self._S_AL[:, :, :m].reshape(-1, m).T, rcond=None)
                    self._lowQfit = (coef[0].reshape(n, n),
                                     coef[1].reshape(n, n), m)
                A, B, _m = self._lowQfit
                out[k] = A + B*q*q
                #A structure factor is a variance and cannot be negative. If
                #the quadratic undershoots -- possible when the window catches
                #curvature the q^2 form does not capture -- fall back to the
                #lowest computed point rather than hand back something
                #unphysical.
                if np.any(np.diag(out[k]) < 0.0):
                    out[k] = self._S_AL[:, :, 0]
            elif q >= self._q[-1]:
                out[k] = eye          # S -> delta_ij at large q
            else:
                #CUBIC (Catmull-Rom) via the splines package, not linear.
                #
                #Measured on a hard-sphere S(q) at phi = 0.40 on the standard
                #4095-point grid, linear interpolation is wrong by about 1e-3
                #between nodes -- the same size as the discretisation
                #discrepancies this package spends real effort chasing, so an
                #interpolation artefact was sitting in the range where the
                #physics is being measured. Cubic drops it by two orders, and
                #the gap widens as the grid is refined since linear is
                #O(dq^2) and cubic O(dq^4).
                #
                #The spline carries the whole (N,N) matrix as N*N vector
                #components, so one evaluation returns every partial. Built
                #once and cached: construction is ~90 ms at this grid against
                #a few microseconds per evaluation, and a fit makes hundreds.
                #
                #alpha=0 (uniform parameterisation) is correct ONLY because
                #the DST grid is uniform, q[k] = dq*(k+1). On a log-spaced
                #grid this would be silently wrong rather than inaccurate.
                spline = self._crInterpolator()
                if spline is None:
                    i = int(np.searchsorted(self._q, q)) - 1
                    i = min(max(i, 0), self._q.size - 2)
                    t = (q - self._q[i])/(self._q[i + 1] - self._q[i])
                    out[k] = ((1.0 - t)*self._S_AL[:, :, i]
                              + t*self._S_AL[:, :, i + 1])
                else:
                    u = (q - self._q[0])/(self._q[1] - self._q[0])
                    u = min(max(u, 0.0), float(self._q.size - 1))
                    out[k] = np.asarray(spline.evaluate(u),
                                        float).reshape(n, n)
        if self._fine:
            #Interpolate only the SMOOTH part. S^AL_ij = delta_ij +
            #sqrt(rho_i rho_j) h_ij: the Kronecker delta is a sharp diagonal
            #and bilinear interpolation smears it into a band, which turns the
            #incoherent sum sum_i |F_i|^2 into the coherent |sum_i F_i|^2 and
            #so puts the form-factor oscillations straight back. Measured: at
            #phi = 1e-6, where S must be the identity, interpolating S whole
            #gave a ripple of 2.69 against a brute-force reference of 0.038.
            #h_ij is what varies slowly with size, so h is interpolated and
            #the delta is re-imposed exactly on the fine grid.
            nc = self.sigmaS.size
            nf = self.sigma.size
            rc = self._rhoCoarse
            fine = np.empty((Q.size, nf, nf))
            scale_f = np.sqrt(np.outer(self.rho, self.rho))
            for k in range(Q.size):
                h_c = (out[k] - np.eye(nc))/np.sqrt(np.outer(rc, rc))
                fine[k] = np.eye(nf) + scale_f*self._interpolateToFine(h_c)
            return fine
        return out

    def S_number(self, Q):
        wv = np.sqrt(self.w)
        return np.array([float(wv @ S @ wv) for S in self.S_partials(Q)])

    # ------------------------------------------------------------------
    def _mono_S(self, Q, sigma_eff):
        """Monodisperse S(Q) at sigma_eff, same potential/closure/state.

        Required by the base class's approximation schemes. Solved as a
        genuine one-component problem (srel = 0, one class) rather than
        reusing a polydisperse solve.

        CACHE KEY. The reduced one-component solve depends on the potential,
        its reduced arguments, the closure and the volume fraction -- NOT on
        sigma_eff, which enters only through the rescaling of q below. Keying
        the cache on sigma_eff therefore re-solved the SAME problem once per
        distinct diameter: 60 identical solves for the local monodisperse
        approximation at nFF = 60, and 59 more for the pair-sum schemes.
        Verified bit-identical (max|S_i - S_0| = 0) for HardSphere, SquareWell
        and Yukawa, so a single key is correct as well as ~50 times faster.

        WHY THIS IS SOUND, and where it would not be. The local monodisperse
        approximation puts each size class in a bath of its own size at the
        SAME volume fraction, and for a potential whose parameters are
        expressed in units of sigma the reduced solution depends only on phi.
        So S(Q; sigma_i) = S_reduced(Q sigma_i) exactly. That fails for a
        potential carrying an ABSOLUTE length (a screening length in physical
        units) or one whose amplitude is size-coupled, because then the
        reduced problem differs per class. The generic polydisperse builder
        refuses charge-coupled potentials, so those cases cannot arrive here;
        the check below guards against a future one that could.

        Verified bit-identical against the per-sigma cache across three
        potentials and all six approximation schemes: worst relative change
        0.00e+00.
        """
        #A size-coupled potential would make the reduced one-component problem
        #differ per class, and a single cached solve would then be silently
        #wrong rather than merely slow. Fail loudly instead.
        #
        #CHARGE_COUPLED_POTENTIALS lives in oZfixpointOperator, not here --
        #imported locally so this module keeps working if that name moves.
        try:
            from oZfixpointOperator import CHARGE_COUPLED_POTENTIALS as _CC
        except Exception:
            _CC = ('DLVO', 'DLVOHydra', 'IonicMicrogel')
        if self.potential in _CC:
            raise RuntimeError(
                f"{self.potential} couples its parameters to particle size, so "
                "the reduced one-component solve is NOT the same for every "
                "diameter and _mono_S's single-solve cache would be wrong. "
                "The approximation schemes are not available for it.")
        key = (self.potential, tuple(np.atleast_1d(self.potentialArgs).ravel())
               if np.size(self.potentialArgs) else (),
               self.closure, self.closureParam, round(float(self.phi), 12))
        if key not in self._mono_cache:
            self._mono_cache[key] = self._solve(0.0, 1, self.phi)
        sol = self._mono_cache[key]
        q = np.asarray(sol.getqArray(), float)
        S = np.real(np.asarray(sol.getSq(), float))
        #sigma_eff enters ONLY here: the one-component solve is in units of
        #its own diameter, so q is rescaled rather than re-solved.
        return np.interp(np.atleast_1d(np.asarray(Q, float))*sigma_eff, q, S,
                         left=S[0], right=1.0)


# ----------------------------------------------------------------------
# Thermodynamic consistency: fixing the closure parameter
# ----------------------------------------------------------------------
def _chiInvCompressibility(sas):
    """chi^-1 = 1 - n sum_ij x_i x_j chat_ij(0)   [D'Aguanno & Klein eq. 41].

    The molar-fraction contraction is what makes a MIXTURE's consistency
    condition a single scalar equation rather than a compressibility matrix
    problem. chat_ij(0) comes from a quadratic extrapolation to q = 0, since
    the DST grid starts at Delta_q rather than 0.
    """
    sol = sas.solver
    if getattr(sol, "numberOfComponents", 1) > 1:
        c = sol.update_c(sol.gammaMatrixMulticomponent)
        chat = sol.hankelTransform(c, sol.Delta_r)
        x = np.asarray(sol.componentFractions, float)
        y = 1.0 - sol.particleDensity*np.einsum('i,j,ijk->k', x, x, chat)[:3]
    else:
        c = np.asarray(sol.getDirectCorrelationFunction(), float)
        chat = sol.hankelTransform(c, sol.Delta_r)
        y = 1.0 - sol.particleDensity*chat[:3]
    q = np.asarray(sol.getqArray(), float)[:3]
    x1, x2, x3 = q
    y1, y2, y3 = y
    return -(-x2**2*x3*y1 + x2*x3**2*y1 + x1**2*x3*y2 - x1*x3**2*y2
             - x1**2*x2*y3 + x1*x2**2*y3)/((x1-x2)*(x1-x3)*(x2-x3))


def _betaP(sas):
    """beta*P from the virial route.

    beta P/n = 1 + (2pi/3) n sum_ij x_i x_j sigma_ij^3 g_ij(sigma_ij+)
                 - (2pi/3) n sum_ij x_i x_j int r^3 g_ij (beta U_ij)' dr

    The CONTACT term (second line) is kept. D'Aguanno & Klein omit it
    legitimately -- their macroions are so strongly charged that
    g(sigma+) ~ 0 -- but that is not safe for a general potential with a
    reachable hard core, where the term is first order in the pressure.
    g at contact is taken by linear extrapolation from the first two grid
    points outside sigma_ij, which is the least certain step here.

    (beta U)' is differentiated NUMERICALLY from the stored potential,
    because the generic builder reuses arbitrary one-component setters and
    cannot know their analytic derivative.
    """
    sol = sas.solver
    r = np.asarray(sol.getrArray(), float)
    dr = sol.Delta_r
    multi = getattr(sol, "numberOfComponents", 1) > 1
    if multi:
        gam = sol.gammaMatrixMulticomponent
        c = sol.update_c(gam)
        EN = sol.boltzmannOfP2Ppotential
        g = np.where(EN > 0, gam + c + 1.0, 0.0)
        x = np.asarray(sol.componentFractions, float)
        sig = np.asarray(sol.componentDiameters, float)
        sij = 0.5*(sig[:, None] + sig[None, :])
        U = np.asarray(sol.p2PpotentialInkTUnits, float)
    else:
        g = np.asarray(sol.getRDF(), float)[None, None, :]
        EN = np.asarray(sol.boltzmannOfP2Ppotential, float)[None, None, :]
        x = np.array([1.0])
        sij = np.array([[sol.hardSphereDiameter]])
        U = np.asarray(sol.p2PpotentialInkTUnits, float)[None, None, :]
        g = np.where(EN > 0, g, 0.0)
    n = sol.particleDensity
    Ufin = np.where(np.isfinite(U), U, 0.0)
    dU = np.gradient(Ufin, dr, axis=-1)
    dU = np.where(EN > 0, dU, 0.0)          # derivative only outside the core
    tail = np.einsum('i,j,ijk->', x, x, (r**3)[None, None, :]*g*dU)*dr

    p = sij.shape[0]
    gc = np.zeros((p, p))
    for i in range(p):
        for j in range(p):
            k = int(np.searchsorted(r, sij[i, j]))
            k = min(max(k, 0), r.size - 2)
            if g[i, j, k] <= 0.0 and g[i, j, k + 1] > 0.0:
                k += 1
            if k + 1 < r.size and g[i, j, k] > 0.0:
                slope = (g[i, j, k + 1] - g[i, j, k])/dr
                gc[i, j] = max(g[i, j, k] - slope*(r[k] - sij[i, j]), 0.0)
    contact = np.einsum('i,j,ij->', x, x, sij**3*gc)
    return n*(1.0 + (2.0*np.pi/3.0)*n*contact - (2.0*np.pi/3.0)*n*tail)


def consistencyResidual(alpha, drho_frac=0.01, **kw):
    """chi^-1(compressibility) - chi^-1(virial). Zero => consistent.

    The reduced-tail pair potential built by setPolydispersePotential()
    depends only on the diameters and the tail parameters, NOT on density, so
    perturbing phi leaves it untouched. (That is not automatic: for the
    charge-coupled Yukawa route kappa depends on the counterion density, and
    rebuilding at phi +/- dphi silently differentiates a density-dependent
    potential -- a mistake that produced a residual stuck near +65.)
    """
    phi = kw.pop("phi")
    base = GenericPolydisperseSAS(phi=phi, closureParam=alpha, **kw)
    chi = _chiInvCompressibility(base)
    P = []
    for f in (1.0 - drho_frac, 1.0 + drho_frac):
        P.append(_betaP(GenericPolydisperseSAS(phi=phi*f, closureParam=alpha, **kw)))
    dn = (base.solver.particleDensity/phi)*(2.0*drho_frac*phi)
    return chi - (P[1] - P[0])/dn


def solveAlpha(bracket=(1e-6, 100.0), tol=2e-4, progress=None, **kw):
    """Closure parameter that makes the two pressure routes agree.

    Returns (alpha, residual, scale) where `scale` is chi^-1 at the returned
    alpha. Consistency must be judged RELATIVE to that: chi^-1 is of order 10
    for a dense hard-sphere fluid, so an absolute residual of 1e-3 is a
    relative 1e-4 and is converged, not a failure. Judging it absolutely made
    a perfectly good root (alpha = 0.2443, residual -1.2e-3) report as "no
    consistent value found".

    A residual that is large RELATIVE to the scale means NO consistent value
    exists in the bracket -- the closest end point is returned and the caller
    must report it as a fallback rather than a fit. That happens: the residual
    is often monotone and already nonzero in the alpha -> 0 limit, i.e. the
    base closure is itself inconsistent for that state and no amount of mixing
    repairs it.
    """
    from scipy.optimize import brentq

    def f(a):
        res = consistencyResidual(a, **kw)
        if progress:
            progress(f"alpha = {a:.4f} -> inconsistency {res:+.4g}")
        return res

    lo, hi = bracket
    flo, fhi = f(lo), f(hi)
    if flo*fhi > 0.0:
        a = lo if abs(flo) < abs(fhi) else hi
        res = flo if a == lo else fhi
    else:
        a = brentq(f, lo, hi, xtol=tol)
        res = consistencyResidual(a, **kw)
    phi = kw.get("phi")
    scale = abs(_chiInvCompressibility(
        GenericPolydisperseSAS(closureParam=a, **kw))) or 1.0
    return a, res, scale
