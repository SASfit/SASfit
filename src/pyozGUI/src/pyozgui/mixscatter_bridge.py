# -*- coding: utf-8 -*-
"""
Use this package's Ornstein-Zernike closures inside `mixscatter`.

`mixscatter` supplies a well-built polydisperse scattering model -- particle
form factors from radial profiles, mixtures, measurable intensities -- but
only TWO structure factors: `PercusYevick` and `VerletWeis`, both analytic
hard-sphere solutions, and it performs no iteration at all. This module
supplies the missing half, so that any of this package's closures and
potentials can be combined with mixscatter's form factors.

VALIDATED
---------
Against mixscatter's own `PercusYevick` (the analytic Vrij (1979) mixture
solution), for radius = [0.85, 1.0, 1.15], x = [0.25, 0.5, 0.25], phi = 0.2:

    pointsPerSigma = 100   0.0104
    pointsPerSigma = 200   0.0019
    pointsPerSigma = 400   0.0009

i.e. 0.09 % at a fine grid, converging as the grid is refined.

That comparison is also what exposed the identical-cores defect in
`setHardSpherePotential` (fixed): before it, the discrepancy was 5.2 %, did
NOT converge under refinement, and was linear in the size spread. If a future
change makes this agreement stop converging, suspect the pair potentials
before suspecting this adapter.

HOW LITTLE IS NEEDED
--------------------
mixscatter's extension point is the `LiquidStructureLike` protocol, which
requires exactly ONE property:

    number_weighted_partial_structure_factor -> (n, n, nq) array

and it is in the D'Aguanno number-weighted convention

    S_ij = x_i delta_ij + n x_i x_j hhat_ij

which is what oZsolver stores natively in `partialStructureFactor`. Verified
against mixscatter's own PercusYevick: the diagonal tends to x_i and the
off-diagonal to zero at high q, for x = (0.25, 0.5, 0.25). So no convention
conversion is required at all -- unusual, and worth stating, because getting
this wrong elsewhere in this project inflated I(Q) by a factor depending only
on the number of size classes.

WHAT THIS BUYS
--------------
mixscatter gains 23 closures instead of 2, and arbitrary pair potentials
instead of hard spheres only -- charged systems, adhesive spheres, square
wells, double Yukawa. This package gains mixscatter's form-factor machinery,
which is richer than its own two hand-written ones: `ParticleBuilder` composes
particles from radial profiles (constant, linear, gradient shells), giving
core-shell and graded-density particles without writing any amplitude.

Keeping BOTH available in one place is useful in itself: mixscatter's
analytic PY is exact, so switching between it and the numerical route is a
standing self-check.

WHAT IT DOES NOT DO
-------------------
* mixscatter works in RADIUS; this package works in hard-core DIAMETER. The
  adapter converts (sigma = 2R) and assumes the scattering radius equals the
  hard-core radius, which is mixscatter's assumption too.
* No instrumental resolution: mixscatter's `measurable_intensity` does not
  smear. Use this package's `Resolution` on the result if that matters.
* The wavevector is dimensionless (q*sigma) unless the mixture radii are given
  in physical units, in which case q is a genuine inverse length. mixscatter
  imposes no convention, so consistency is the caller's responsibility.
* NARROW WELLS need a fine grid. The default pointsPerSigma = 100 is fine for
  hard spheres but badly wrong for e.g. adhesive spheres with delta = 0.02,
  where the well spans two grid points and S(q) is ~90 % in error. Rule of
  thumb: pointsPerSigma >~ 30 * sigma/delta.
"""
import numpy as np


class OZLiquidStructure:
    """A `LiquidStructureLike` backed by this package's OZ solver.

    Drop-in replacement for `mixscatter.PercusYevick` in any mixscatter
    function that accepts a `LiquidStructureLike`:

        import mixscatter as ms
        from mixscatter_bridge import OZLiquidStructure

        mixture = ms.Mixture(radius=[0.85, 1.0, 1.15],
                             number_fraction=[0.25, 0.5, 0.25])
        model   = ms.SimpleSphere(q, mixture, contrast=1.0)
        struct  = OZLiquidStructure(q, mixture, volume_fraction_total=0.3,
                                    closure="Rogers-Young", closureParam=0.5)
        I = ms.measurable_intensity(struct, model)

    potential / potentialArgs select any potential this package can make
    polydisperse; the default is hard spheres, matching mixscatter's own two
    structure factors so that a comparison is like for like.
    """

    def __init__(self, wavevector, mixture, volume_fraction_total,
                 closure="Percus-Yevick", closureParam=None,
                 closureParam2=None, potential="HardSphere",
                 potentialArgs=(), solverClass=None, gridN=4095,
                 pointsPerSigma=100, maxIterations=6000):
        import ozLib

        self.wavevector = np.atleast_1d(np.asarray(wavevector, float))
        self.mixture = mixture
        self.volume_fraction_total = float(volume_fraction_total)
        self.closure = closure

        radius = np.asarray(mixture.radius, float)
        x = np.asarray(mixture.number_fraction, float)
        x = x/x.sum()
        #mixscatter is in radius, this package in hard-core diameter.
        sigma = 2.0*radius
        #Work in units of the MEAN diameter, because the solver's radial grid
        #spans only about 41 diameters: handing it physical sigma of order 100
        #would put every hard core off the end of the grid. The q axis is
        #rescaled by the same factor on the way out.
        self._L = float(np.sum(x*sigma))
        sigmaReduced = sigma/self._L

        if solverClass is None:
            from picardOZsolver import PicardOZsolver
            solverClass = PicardOZsolver
        sol = solverClass(port=0, numberOfRadialSamplingPoints=gridN,
                          hardSphereDiameterInPoints=pointsPerSigma)
        sol.setNumberOfIterations(maxIterations)
        sol.setVolumeDensity(self.volume_fraction_total)

        self._buildPotential(sol, potential, potentialArgs, sigmaReduced, x)
        self._applyClosure(sol, ozLib, closure, closureParam, closureParam2)
        sol.solve()
        self._verify(sol)
        self.solver = sol

        self._q = np.asarray(sol.getqArray(), float)
        p = getattr(sol, "numberOfComponents", 1)
        if p == 1:
            self._S = np.real(np.asarray(sol.getSq(), float)).reshape(1, 1, -1)
        else:
            self._S = np.asarray(sol.partialStructureFactor, float)

    # ------------------------------------------------------------------
    @staticmethod
    def _buildPotential(sol, potential, potentialArgs, sigmaReduced, x):
        """Install an explicit set of size classes, not a fitted distribution.

        setPolydispersePotential() derives its own moment-matched classes from
        a distribution; here the classes are GIVEN by the mixture, so the pair
        matrices are built directly with the same reduced-tail rule
        (sigma_ij = (sigma_i+sigma_j)/2, u_ij(r) = u(r/sigma_ij)).
        """
        p = len(sigmaReduced)
        N = sol.numberOfRadialSamplingPoints
        r = sol.Delta_r*(np.arange(N).astype(float) + 1.0)

        sol.numberOfComponents = p
        sol.componentDiameters = sigmaReduced
        sol.componentFractions = x
        thirdMoment = float(np.sum(x*sigmaReduced**3))
        sol.particleDensity = sol.volumeDensity/((np.pi/6.0)*thirdMoment)
        sol.componentDensities = sol.particleDensity*x

        EN = np.zeros((p, p, N)); U = np.zeros((p, p, N))
        REP = np.zeros((p, p, N)); ATT = np.zeros((p, p, N))
        setter = getattr(sol, "set" + potential + "Potential")
        try:
            for i in range(p):
                for j in range(i, p):
                    sij = 0.5*(sigmaReduced[i] + sigmaReduced[j])
                    #The rescaled grid is what puts each pair's core at
                    #sigma_ij. A setter that ignores getrArray() defeats this
                    #silently -- see the identical-cores note in the module
                    #docstring.
                    sol._rArrayOverride = r/sij
                    sol.repulsivePartOfP2Ppotential = np.zeros(N)
                    sol.attractivePartOfP2Ppotential = np.zeros(N)
                    setter(*potentialArgs)
                    EN[i, j] = EN[j, i] = np.asarray(sol.boltzmannOfP2Ppotential, float)
                    u = getattr(sol, "p2PpotentialInkTUnits", None)
                    if u is not None and np.ndim(u) == 1:
                        U[i, j] = U[j, i] = np.asarray(u, float)
                    REP[i, j] = REP[j, i] = np.asarray(sol.repulsivePartOfP2Ppotential, float)
                    ATT[i, j] = ATT[j, i] = np.asarray(sol.attractivePartOfP2Ppotential, float)
        finally:
            sol._rArrayOverride = None

        #Same fallback as setPolydispersePotential: without it the closures
        #that read the split see an all-zero potential, and c(r) = 0 becomes
        #an exact fixed point -- the S(q) = 1 failure met earlier.
        if not (np.any(REP != 0.0) or np.any(ATT != 0.0)):
            REP = -np.log(np.clip(EN, 1e-300, None))
            ATT = np.zeros_like(EN)
        if not np.any(U != 0.0):
            with np.errstate(divide="ignore"):
                U = np.where(EN > 0.0, -np.log(np.clip(EN, 1e-300, None)), np.inf)

        sol.boltzmannOfP2Ppotential = EN
        sol.p2PpotentialInkTUnits = U
        sol.repulsivePartOfP2Ppotential = REP
        sol.attractivePartOfP2Ppotential = ATT
        sol.setStartValue(np.zeros(sol.numberOfUniquePairs()*N))
        sol.activePotentialname = "Mixture" + potential

    @staticmethod
    def _applyClosure(sol, ozLib, closure, param, param2):
        if closure in ozLib.MULTICOMPONENT_INCAPABLE_CLOSURES:
            raise ValueError(
                f"closure {closure!r} needs a one-component reference solve or "
                "is hard-sphere specific, so it cannot be used with a mixture "
                "(see ozLib.MULTICOMPONENT_INCAPABLE_CLOSURES)")
        setterName, needsParam = ozLib.CLOSURE_SETTERS[closure]
        extra = ozLib.secondClosureParam(closure)
        if needsParam and extra is not None and param2 is not None:
            getattr(sol, setterName)(param, param2)
        elif needsParam:
            if param is None:
                raise ValueError(f"closure {closure!r} needs a parameter value")
            getattr(sol, setterName)(param)
        else:
            getattr(sol, setterName)()

    def _verify(self, sol):
        """Never trust the driver: it reports convergence but returns results
        regardless, and the closure equations admit multiple fixed points."""
        if not hasattr(sol, "gammaMatrixMulticomponent"):
            raise RuntimeError(f"{self.closure} produced no fixpoint")
        flat = sol.packPairs(sol.gammaMatrixMulticomponent)
        image = sol.fixPointOperatorForGammaMulticomponent(flat)[0]
        if not np.all(np.isfinite(image)):
            raise RuntimeError(f"{self.closure} diverged")
        res = float(np.max(np.abs(image - flat)))
        if res > 1e-3:
            raise RuntimeError(f"{self.closure} did not converge "
                               f"(max|G(gamma)-gamma| = {res:.3e})")

    # ------------------------------------------------------------------
    @property
    def number_weighted_partial_structure_factor(self):
        """(n, n, nq) in the D'Aguanno convention, on the requested q grid.

        This single property is the whole `LiquidStructureLike` protocol. No
        convention conversion is applied: the solver already stores
        S_ij = x_i delta_ij + n x_i x_j hhat_ij, which is what mixscatter
        expects -- verified against its own PercusYevick.

        Beyond the solver's own q grid the partials tend to x_i delta_ij, so
        that is the value imposed rather than a clamp of the last computed
        point.
        """
        n = self._S.shape[0]
        x = np.asarray(self.mixture.number_fraction, float)
        x = x/x.sum()
        limit = np.diag(x)
        #q axis: the solve is in units of the mean diameter.
        qr = self.wavevector*self._L
        out = np.empty((n, n, qr.size))
        for i in range(n):
            for j in range(n):
                out[i, j] = np.interp(qr, self._q, self._S[i, j],
                                      left=self._S[i, j, 0], right=limit[i, j])
        return out

    @property
    def average_structure_factor(self):
        """Number-number S(q), summed over the partials."""
        return self.number_weighted_partial_structure_factor.sum(axis=(0, 1))

    def __repr__(self):
        return (f"OZLiquidStructure(closure={self.closure!r}, "
                f"{self._S.shape[0]} components, "
                f"phi={self.volume_fraction_total:g})")
