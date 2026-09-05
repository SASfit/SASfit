# -*- coding: utf-8 -*-
"""
Rogers-Young closure for polydisperse hard-core Yukawa mixtures.

Thin adapter onto the REAL multicomponent Ornstein-Zernike solver in
oZfixpointOperator.py / oZsolver.py (setPolydisperseHardCoreYukawaPotential's
matrix fixpoint operator), which does an actual iterative OZ solve with the
Rogers-Young mixing function f(r) = 1 - exp(-alpha r) and is validated against
B. D'Aguanno & R. Klein, Phys. Rev. A 46, 7652 (1992) / J. Chem. Soc. Faraday
Trans. 87, 379 (1991).

This REPLACES an earlier version of this file which re-exported
ry_polydisperse_core_py.RYPolydisperseYukawa -- a placeholder that had no
density at all, performed no OZ iteration, and built S_ij from an ad-hoc
closed-form blend. Nothing here uses it.

CONVENTIONS -- deliberately identical to the MSA/RMSA path
(polydisperse_yukawa_msa.PolydisperseOneYukawaMSA), so the two closures can be
compared on the same footing:

    RYPolydisperseYukawa(sigma, rho, z, K, alpha=0.5, delta=None)

    sigma : per-species hard-core DIAMETERS
    rho   : per-species NUMBER DENSITIES (not fractions) -- the volume
            fraction is already contained in these, so no separate phi
    z     : Yukawa screening parameter, an inverse length; z*sigma is the
            usual dimensionless Z
    K     : contact coupling. SAME SIGN CONVENTION AS MSA, which sets
            c(r) = -beta U:  K > 0 attractive, K < 0 repulsive
    alpha : Rogers-Young mixing parameter. alpha -> 0 recovers Percus-Yevick,
            alpha -> infinity recovers HNC. Not determined self-consistently
            here (that costs three extra OZ solves per trial value); use the
            OZ solver tab's "find thermodynamically consistent value" once and
            keep it fixed, as D'Aguanno & Klein themselves do.
    delta : optional per-species coupling amplitude for charge/coupling
            polydispersity, defaults to all-ones. Same role as MSA's delta.

S_matrix(q) returns the (N,N) partial structure factors in the
ASHCROFT-LANGRETH convention, S^AL_ij = delta_ij + sqrt(rho_i rho_j) h_ij(q),
matching what PolydisperseSASBase.I_exact expects (it forms
G = F*sqrt(rho) and contracts G S G). The underlying solver stores
D'Aguanno's convention S^DAG_ij = x_i delta_ij + n x_i x_j h_ij, and the
conversion is S^AL_ij = S^DAG_ij / sqrt(x_i x_j) -- getting this wrong is a
silent order-of-magnitude error in I(Q), so it is done in exactly one place
below and covered by a self-test.

POTENTIAL CONVENTION (resolved against the spec)
------------------------------------------------
Taken verbatim from docs/polydisperse_yukawa_spec.md section 1 (citing
BH02-5 and BA-4), so RY sits on exactly the same potential as MSA/RMSA:

    c_ij(r) = K_ij exp(-z (r - sigma_ij)) / r ,   r > sigma_ij
    K_ij    = K * delta_i * delta_j
    delta_i = d_i * exp(-z sigma_i / 2)
    sigma_ij = (sigma_i + sigma_j)/2,  g_ij(r) = 0 for r <= sigma_ij

and MSA sets c = -beta U. Note there is NO sigma_ij prefactor: an earlier
version of this file assumed one, written before the spec was located, which
would have made RY disagree with MSA by a K-proportional, size-dependent
factor. The `delta` argument below is d_i (the plain per-species amplitude);
the exp(-z sigma_i/2) factor is applied internally, so callers pass the same
thing they would pass to the MSA path.

All of this lives in _betaU() -- one function, one line of arithmetic.

The self-test checks the dilute limit S -> 1, symmetry of S^AL, and the
expected sign response to K. A quantitative K != 0 cross-check against
PolydisperseOneYukawaMSA at small K and small phi (where MSA's
c(r) = -beta U is exact, so the two closures must agree to O(K^2)) is the
remaining validation worth doing -- it is now meaningful, because the
potential is no longer in question.
"""
import numpy as np

from picardOZsolver import PicardOZsolver

# Available fixpoint iterations, in the order tried by solver="auto".
# Measured on a deliberately hard state point (three classes, z=2, K=1
# attractive, phi=0.1) and on an easy one (phi=0.02):
#
#   solver               hard point                     easy point
#   Picard               diverges (56 s wasted)         0.1 s
#   Anderson             converges, 15.7 s              0.1 s
#   scipy Anderson       converges, 6.3 s (357 steps)   0.2 s
#   Biggs-Andrews        claims convergence at 21       0.1 s
#                        steps but FAILS the residual
#                        check -- a good advertisement
#                        for checking rather than
#                        trusting the driver
#   AndersonGeneralized  TypeError: its constructor takes only `port`, so it
#                        rejects the grid keyword arguments. Pre-existing, and
#                        it is absent from ozLib's SOLVER_CLASSES anyway.
#
# scipy Anderson is therefore the default: it is the fastest of the ones that
# actually work, and roughly 9x faster than Picard even where Picard converges.
def _solverClasses():
    classes = []
    try:
        from scipyAndersonOZsolver import ScipyAndersonOZsolver
        classes.append(("scipy Anderson", ScipyAndersonOZsolver))
    except Exception:
        pass
    try:
        from andersonOZsolver import AndersonOZsolver
        classes.append(("Anderson", AndersonOZsolver))
    except Exception:
        pass
    classes.append(("Picard", PicardOZsolver))
    return classes

# Re-exported for callers that used the old module's distribution constants.
# They are not needed here: this class takes explicit sigma/rho arrays, so the
# CALLER owns the discretisation and can use any distribution it likes
# (log-normal, Schulz-Zimm, Gamma, Beta ...), exactly as the MSA path does.
DIST_SCHULZ_ZIMM = 1
DIST_LOGNORMAL = 2
DIST_GAUSSIAN = 3
DIST_WEIBULL = 4

# Radial grid. N+1 must be a power of two for the DST-I based transform; 4095
# at 100 points per unit diameter is the OZ solver's own default.
_GRID_N = 4095
_POINTS_PER_SIGMA = 100


def _betaU(r, sigma_ij, z, K, amp):
    """beta*U_ij(r) OUTSIDE the core, from the MSA closure c_ij(r) = -beta U.

    Convention taken verbatim from docs/polydisperse_yukawa_spec.md section 1
    (its eq. for c_ij, citing BH02-5), so RY sits on exactly the same potential
    as the MSA/RMSA path:

        c_ij(r) = sum_n K_ij^(n) exp(-z_n (r - sigma_ij)) / r ,   r > sigma_ij
        K_ij     = K * delta_i * delta_j        (factored/charge-like, BA-4)
        delta_i  = d_i * exp(-z sigma_i / 2)

    and MSA sets c = -beta U, hence the leading minus sign below. K > 0 gives
    beta U < 0, i.e. attraction, matching the MSA tab's own tooltip.

    Note there is NO sigma_ij prefactor -- an earlier version of this file
    assumed one (K * delta_i delta_j * sigma_ij * exp(...)/r) before the spec
    was available, which would have made RY disagree with MSA by a
    K-proportional, size-dependent factor. `amp` here is delta_i*delta_j,
    INCLUDING the exp(-z sigma_i/2) factors, which the caller-facing `delta`
    argument does not contain (see RYPolydisperseYukawa.__init__).
    """
    return -K * amp * np.exp(-z * (r - sigma_ij)) / r


class RYPolydisperseYukawa:
    """Rogers-Young polydisperse hard-core Yukawa. See module docstring."""

    def __init__(self, sigma, rho, z, K, alpha=0.5, delta=None,
                 gridN=_GRID_N, pointsPerSigma=_POINTS_PER_SIGMA,
                 maxIterations=8000, converged_tol=1e-6,
                 solverClass=None, checkPhysical=True, _retrying=False,
                 closure="Rogers-Young", closureParam2=None):
        sigma = np.atleast_1d(np.asarray(sigma, dtype=float)).ravel()
        rho = np.atleast_1d(np.asarray(rho, dtype=float)).ravel()
        if sigma.size != rho.size:
            raise ValueError("sigma and rho must have the same length "
                             f"(got {sigma.size} and {rho.size})")
        if np.any(sigma <= 0.0):
            raise ValueError("all sigma must be > 0")
        if np.any(rho < 0.0):
            raise ValueError("all rho must be >= 0")
        if not np.any(rho > 0.0):
            raise ValueError("at least one rho must be > 0")

        N = sigma.size
        self.N = self.p = N
        self.sigma = self.D = sigma.copy()
        self.rho = rho.copy()
        self.z = float(z)
        self.K = float(K)
        self.alpha = float(alpha)
        self.closure = closure
        self.closureParam2 = closureParam2
        self.delta = (np.ones(N) if delta is None
                      else np.atleast_1d(np.asarray(delta, float)).ravel())
        if self.delta.size != N:
            raise ValueError("delta must have the same length as sigma")

        self.x = self.rho / self.rho.sum()          # number fractions
        self.nTotal = float(self.rho.sum())
        self.phi = float(np.pi/6.0 * np.sum(self.rho * self.sigma**3))

        # --- work in units of the mean diameter -------------------------
        # The solver's grid is fixed in units of its own hardSphereDiameter
        # (== 1 by construction), so everything is scaled by Dref here and
        # scaled back in S_matrix(). Using the number-averaged diameter keeps
        # the scaled diameters O(1) for any distribution.
        if solverClass is None:
            solverClass = _solverClasses()[0][1]
        self.solverName = solverClass.__name__

        Dref = float(np.sum(self.x * self.sigma))
        self._Dref = Dref
        sig_s = self.sigma / Dref
        rho_s = self.rho * Dref**3
        z_s = self.z * Dref
        # kept for the consistency-search helpers below, which need to work
        # in the same scaled units the solver itself uses
        self._sig_s = sig_s
        self._z_s = z_s
        self.nTotal_scaled = float(rho_s.sum())

        sol = solverClass(port=0, numberOfRadialSamplingPoints=gridN,
                          hardSphereDiameterInPoints=pointsPerSigma)
        sol.setNumberOfIterations(maxIterations)

        # Inject the components directly. setPolydisperseHardCoreYukawaPotential()
        # cannot be reused: it derives sigma/x from a Schulz distribution via
        # Gauss-Laguerre quadrature and builds a screened-Coulomb potential
        # from valence and Bjerrum length, whereas here the caller supplies
        # the classes and the potential is the MSA-convention (z, K) Yukawa.
        # The attributes set below are exactly those the multicomponent
        # fixpoint operator reads.
        sol.numberOfComponents = N
        sol.componentDiameters = sig_s
        sol.componentFractions = self.x.copy()
        sol.componentDensities = rho_s
        sol.particleDensity = float(rho_s.sum())
        sol.volumeDensity = self.phi
        sol.activePotentialname = "PolydisperseYukawaExplicit"

        r = sol.getrArray()
        sij = 0.5 * (sig_s[:, None] + sig_s[None, :])
        # `delta` IS delta_i, not d_i. The spec writes
        # delta_i = d_i exp(-z sigma_i/2), but PolydisperseOneYukawaMSA takes
        # delta_i directly (its default `np.ones(N)` means delta_i = 1, not
        # d_i = 1), so applying the exponential here as well double-counts it.
        # Caught numerically: doing so made RY's dS/dK a factor ~7.2 weaker
        # than MSA's across three densities, matching 1/exp(-z*sigma/2)^2
        # = 1/0.135 = 7.4 for z*sigma ~ 2. Pass delta already including any
        # exp(-z sigma_i/2) factor, exactly as for the MSA path.
        amp = self.delta[:, None] * self.delta[None, :]
        isCore = r[None, None, :] < sij[:, :, None]
        with np.errstate(over="ignore", invalid="ignore"):
            bU = _betaU(r[None, None, :], sij[:, :, None], z_s, self.K,
                        amp[:, :, None])
        bU = np.where(isCore, 0.0, bU)
        sol.p2PpotentialInkTUnits = np.where(isCore, np.inf, bU)
        # exp(-betaU) can overflow for a strongly repulsive tail (K < 0); the
        # closure only ever multiplies by it, so clipping the exponent keeps a
        # transient finite instead of poisoning the whole vector with inf.
        sol.boltzmannOfP2Ppotential = np.where(isCore, 0.0,
                                               np.exp(np.clip(-bU, -700.0, 500.0)))
        sol.derivativeOfP2Ppotential = np.where(
            isCore, 0.0,
            -np.where(isCore, 0.0, bU) * (z_s + 1.0/np.where(r > 0, r, 1.0)))

        npairs = N*(N + 1)//2
        sol.setStartValue(np.zeros(npairs * gridN))

        #Closure dispatch. This used to be a hardcoded doRYclosure() call,
        #which meant the RY tab could only ever do Rogers-Young even though
        #the underlying multicomponent solver supports 19 closures. The
        #closure is looked up in ozLib.CLOSURE_SETTERS so that any closure
        #usable with a polydisperse potential can be driven from here, and
        #hence offered in a dropdown by the tab above.
        import ozLib
        if self.closure not in ozLib.CLOSURE_SETTERS:
            raise ValueError("unknown closure %r" % (self.closure,))
        if self.closure in ozLib.MULTICOMPONENT_INCAPABLE_CLOSURES:
            raise ValueError(
                "closure %r cannot be used with a polydisperse potential: it "
                "needs a one-component reference solve or is hard-sphere "
                "specific (see ozLib.MULTICOMPONENT_INCAPABLE_CLOSURES)."
                % (self.closure,))
        setterName, needsParam = ozLib.CLOSURE_SETTERS[self.closure]
        extra = ozLib.secondClosureParam(self.closure)
        if needsParam and extra is not None and self.closureParam2 is not None:
            getattr(sol, setterName)(self.alpha, self.closureParam2)
        elif needsParam:
            getattr(sol, setterName)(self.alpha)
        else:
            getattr(sol, setterName)()
        try:
            sol.solve()
            solveError = None
        except Exception as exc:                       # noqa: BLE001
            solveError = exc

        self.solver = sol

        # The solver may have failed outright (scipy's Anderson raises
        # NoConvergence) or converged but left no result. Either way
        # gammaMatrixMulticomponent may not exist, and reading it blindly gave
        # a bare AttributeError instead of a usable message -- found by the
        # unit test at phi = 1e-5, a nearly trivial state where scipy Anderson
        # nonetheless fails to satisfy its relative convergence criterion
        # because the solution is so close to gamma = 0.
        if solveError is not None or not hasattr(sol, "gammaMatrixMulticomponent"):
            if solverClass is None and not _retrying:
                # "auto": fall through to the next candidate solver rather than
                # giving up. Robustness varies enormously with state point --
                # see the table above _solverClasses().
                for _, cls in _solverClasses()[1:]:
                    try:
                        self.__init__(sigma, rho, z, K, alpha, delta,
                                      gridN=gridN, pointsPerSigma=pointsPerSigma,
                                      maxIterations=maxIterations,
                                      converged_tol=converged_tol,
                                      solverClass=cls,
                                      checkPhysical=checkPhysical,
                                      _retrying=True)
                        return
                    except Exception:                   # noqa: BLE001
                        continue
            raise RuntimeError(
                f"RY solve failed with {self.solverName} at phi={self.phi:.4g}, "
                f"z={self.z:.4g}, K={self.K:.4g}, alpha={self.alpha:.4g}"
                + (f": {type(solveError).__name__}: {solveError}"
                   if solveError is not None else
                   ": the solver produced no fixpoint"))

        # VERIFY CONVERGENCE. picardIteration() prints "Picard did not
        # converge after N steps" but then calls
        # derivePhysicalQuantitiesFromFixpoint() anyway and exposes no flag,
        # so an unconverged solve is otherwise indistinguishable from a good
        # one -- it just returns wrong numbers. Observed directly for an
        # attractive Yukawa (K=1, phi=0.1, z=2): alpha=0.2 returned NaN and
        # alpha=0.5 returned a plausible-looking S(Q)=2.65 after failing to
        # converge, against 2.07 from MSA/RMSA at the same state point. Same
        # lesson as the KINSOL false-success trap in the C core: never trust
        # the driver, recompute the residual.
        gam = sol.gammaMatrixMulticomponent
        flat = sol.packPairs(gam)
        image = sol.fixPointOperatorForGammaMulticomponent(flat)[0]
        self.residual = float(np.max(np.abs(image - flat))) \
            if np.all(np.isfinite(image)) else float("inf")
        self.isConverged = self.residual < converged_tol
        if not self.isConverged:
            raise RuntimeError(
                "RY Ornstein-Zernike iteration did not converge "
                f"(max|G(gamma)-gamma| = {self.residual:.3e} > {converged_tol:.1e}) "
                f"at phi={self.phi:.4g}, z={self.z:.4g}, K={self.K:.4g}, "
                f"alpha={self.alpha:.4g}. Strongly attractive states (large "
                "positive K) and small alpha are the usual causes; the result "
                "is NOT usable and has deliberately not been returned.")
        self._q = np.asarray(sol.getqArray(), float) / Dref   # back to caller units

        # Convert D'Aguanno -> Ashcroft-Langreth, once, here.
        Sdag = np.asarray(sol.partialStructureFactor, float)   # (N,N,nq)
        sx = np.sqrt(np.outer(self.x, self.x))
        self._S_AL = Sdag / sx[:, :, None]

        # PHYSICALITY SCREEN. A small residual is necessary but NOT sufficient:
        # the RY equations can have several fixed points, and different solvers
        # converge cleanly onto different ones. Measured at phi=0.1, K=1, z=2
        # with three classes: the hand-written Anderson gave S(0.5)=17.03 and
        # scipy Anderson gave 9.71, BOTH with residual ~1e-12 -- and both had
        # min S_NN(q) = -9.19 and -17.21 respectively. A structure factor is a
        # variance and cannot be negative, so both were spurious roots. Without
        # this screen one of them would be returned as if it were an answer.
        #
        # Checking S_NN rather than every S_ij keeps this cheap and is the
        # physically meaningful statement (the number-number structure factor
        # is what a scattering experiment measures).
        self.minS = None
        if checkPhysical:
            wv = np.sqrt(self.x)
            S_NN = np.einsum('i,ijk,j->k', wv, self._S_AL, wv)
            self.minS = float(np.min(S_NN))
            if self.minS < -1e-6:
                raise RuntimeError(
                    "RY solution is unphysical: min S_NN(q) = "
                    f"{self.minS:.4g} < 0, at phi={self.phi:.4g}, "
                    f"z={self.z:.4g}, K={self.K:.4g}, alpha={self.alpha:.4g} "
                    f"(solver {self.solverName}, residual {self.residual:.2e}). "
                    "A structure factor is a variance and cannot be negative, "
                    "so this is a spurious root of the closure rather than a "
                    "converged answer -- note the small residual. Strongly "
                    "attractive states (large positive K) at appreciable phi "
                    "are the usual cause; the closure genuinely has multiple "
                    "roots there. Pass checkPhysical=False to inspect it "
                    "anyway.")

        self.g_contact = None      # not provided by this route

    # ------------------------------------------------------------------
    def S_matrix(self, q):
        """(N,N) Ashcroft-Langreth partial structure factors at scalar q,
        linearly interpolated from the solver's own q grid."""
        qv = float(q)
        grid = self._q
        if qv <= grid[0]:
            return self._S_AL[:, :, 0].copy()
        if qv >= grid[-1]:
            # S -> delta_ij at large q
            return np.eye(self.N)
        k = int(np.searchsorted(grid, qv)) - 1
        k = min(max(k, 0), grid.size - 2)
        t = (qv - grid[k]) / (grid[k + 1] - grid[k])
        return (1.0 - t)*self._S_AL[:, :, k] + t*self._S_AL[:, :, k + 1]

    def S_number(self, q):
        """Number-number S(q) = sum_ij sqrt(x_i x_j) S^AL_ij."""
        wv = np.sqrt(self.x)
        return float(wv @ self.S_matrix(q) @ wv)


# Backwards-compatible alias: the old module exposed the wrapper class under
# this name as well.
RyPolydisperse = RYPolydisperseYukawa


def _chi_inv_compressibility(ry):
    """chi^-1 = 1 - n sum_ij x_i x_j chat_ij(0)   [D'Aguanno & Klein eq. 41].

    The scalar contraction is what makes a mixture's consistency condition a
    single equation rather than a compressibility MATRIX problem -- this is
    the form D'Aguanno & Klein actually use for the polydisperse RY scheme.
    chat_ij(0) is obtained by quadratic extrapolation of chat_ij to q = 0,
    since the DST grid starts at Delta_q, not 0.
    """
    sol = ry.solver
    c = sol.update_c(sol.gammaMatrixMulticomponent)
    chat = sol.hankelTransform(c, sol.Delta_r)          # (N,N,nq)
    x = ry.x
    s = np.einsum('i,j,ijk->k', x, x, chat)[:3]
    q = np.asarray(sol.getqArray(), float)[:3]
    x1, x2, x3 = q
    y1, y2, y3 = 1.0 - ry.nTotal_scaled * s
    return -(-x2**2*x3*y1 + x2*x3**2*y1 + x1**2*x3*y2 - x1*x3**2*y2
             - x1**2*x2*y3 + x1*x2**2*y3) / ((x1-x2)*(x1-x3)*(x2-x3))


def _beta_P(ry):
    """beta*P from the virial route, in the solver's scaled units.

    beta P / n = 1
                 + (2 pi/3) n sum_ij x_i x_j sigma_ij^3 g_ij(sigma_ij+)
                 - (2 pi/3) n sum_ij x_i x_j int r^3 g_ij(r) (beta U_ij)'(r) dr

    NOTE the hard-core CONTACT term (second line). D'Aguanno & Klein's eq. 43
    omits it because their macroions are so strongly charged that
    g_ij(sigma_ij+) ~ 0. That is NOT safe here: this model has a genuine hard
    core and K may be small, so contact is reachable and the term is
    first-order in the pressure. g_ij at contact is taken by linear
    extrapolation from the first two grid points outside sigma_ij, which is
    the least certain step in the whole consistency search.
    """
    sol = ry.solver
    gam = sol.gammaMatrixMulticomponent
    c = sol.update_c(gam)
    EN = sol.boltzmannOfP2Ppotential
    g = np.where(EN > 0, gam + c + 1.0, 0.0)
    r = np.asarray(sol.getrArray(), float)
    dr = sol.Delta_r
    x, n = ry.x, ry.nTotal_scaled
    sij = 0.5*(ry._sig_s[:, None] + ry._sig_s[None, :])

    tail = np.einsum('i,j,ijk->', x, x,
                     (r**3)[None, None, :] * g * sol.derivativeOfP2Ppotential)*dr

    N = ry.N
    gc = np.zeros((N, N))
    for i in range(N):
        for j in range(N):
            k = int(np.searchsorted(r, sij[i, j]))
            k = min(max(k, 0), r.size - 2)
            if g[i, j, k] <= 0.0 and g[i, j, k+1] > 0.0:
                k += 1
            if k + 1 < r.size and g[i, j, k] > 0.0:
                slope = (g[i, j, k+1] - g[i, j, k])/dr
                gc[i, j] = max(g[i, j, k] - slope*(r[k] - sij[i, j]), 0.0)
    contact = np.einsum('i,j,ij->', x, x, sij**3 * gc)

    return n*(1.0 + (2.0*np.pi/3.0)*n*contact - (2.0*np.pi/3.0)*n*tail)


def consistency_residual(sigma, rho, z, K, alpha, delta=None, drho_frac=0.01,
                         **kw):
    """chi^-1(compressibility) - chi^-1(virial). Zero => thermodynamically
    consistent, which is what fixes the Rogers-Young alpha.

    The pair potential here depends only on (sigma, z, K, delta) and NOT on
    density, so perturbing rho leaves it untouched automatically -- unlike the
    charge-parameterised route in oZfixpointOperator, where kappa depends on
    the counterion density and rebuilding at n +/- dn silently differentiates
    a density-dependent potential.
    """
    rho = np.asarray(rho, float)
    base = RYPolydisperseYukawa(sigma, rho, z, K, alpha, delta, **kw)
    chi_comp = _chi_inv_compressibility(base)
    P = []
    for f in (1.0 - drho_frac, 1.0 + drho_frac):
        r2 = RYPolydisperseYukawa(sigma, rho*f, z, K, alpha, delta, **kw)
        P.append(_beta_P(r2))
    dn = 2.0*drho_frac*base.nTotal_scaled
    return chi_comp - (P[1] - P[0])/dn


def solve_alpha(sigma, rho, z, K, delta=None, bracket=(1e-6, 100.0),
                tol=2e-3, progress=None, **kw):
    """Find the Rogers-Young alpha that makes the two pressure routes agree.

    Returns (alpha, residual). Costs three OZ solves per trial alpha, so this
    is seconds-to-minutes -- which is exactly why the SASfit plugin takes a
    fixed alpha instead and why alpha should be determined once here and then
    reused. Each trial is cold-started: warm-starting across qualitatively
    different alpha was found to destabilise the equivalent one-component
    search.
    """
    from scipy.optimize import brentq

    def f(a):
        res = consistency_residual(sigma, rho, z, K, a, delta, **kw)
        if progress:
            progress(f"alpha = {a:.4f}  ->  inconsistency {res:+.4g}")
        return res

    lo, hi = bracket
    flo, fhi = f(lo), f(hi)
    if flo*fhi > 0.0:
        # No sign change: return the end point closest to consistency rather
        # than failing outright, and say so via the returned residual.
        #
        # The default bracket is deliberately wide. Mathematically alpha lives
        # on (0, infinity) -- alpha -> 0 is Percus-Yevick, alpha -> infinity is
        # HNC -- and SASfit's own RY implementation scans roughly
        # 100/2^28 ... 100 for the same reason. The narrow (0.1, 3) used
        # earlier here was arbitrary and could miss a root lying below it.
        # Numerically the small-alpha end is well behaved: the mixing function
        # f(r) = 1 - exp(-alpha r) tends to zero smoothly and the closure's
        # (exp(f*gamma)-1)/f is protected by a series expansion for f < 1e-8,
        # so the residual simply asymptotes to its PY value (checked at
        # alpha = 1e-6, where it is finite and smooth, not divergent).
        #
        # When no root exists the residual is typically monotone in alpha and
        # already nonzero in the PY limit, i.e. PY itself is thermodynamically
        # inconsistent for that state and no amount of HNC mixing can fix it.
        # The caller then gets alpha at the PY end together with a clearly
        # nonzero residual, and MUST report that rather than treat the value
        # as a solution -- the returned alpha is a fallback, not a fit.
        a = lo if abs(flo) < abs(fhi) else hi
        return a, (flo if a == lo else fhi)
    a = brentq(f, lo, hi, xtol=tol)
    return a, consistency_residual(sigma, rho, z, K, a, delta, **kw)


def _selftest():                                           # pragma: no cover
    print("RY polydisperse Yukawa wrapper self-test")
    sig = np.array([0.8, 1.0, 1.3])
    x = np.array([0.3, 0.5, 0.2])

    def build(phi, K, alpha=0.5):
        rho = x * phi * 6.0/(np.pi * np.sum(x * sig**3))
        return RYPolydisperseYukawa(sig, rho, z=2.0, K=K, alpha=alpha)

    ry = build(1e-5, 0.0)
    S = ry.S_matrix(3.0)
    off = np.max(np.abs(S - np.eye(3)))
    print(f"  dilute limit (phi=1e-5, K=0): max|S - I| = {off:.3e}   "
          f"{'OK' if off < 5e-3 else 'FAIL'}")

    ry = build(0.2, 0.0)
    S = ry.S_matrix(3.0)
    sym = np.max(np.abs(S - S.T))
    print(f"  symmetry of S^AL at phi=0.2  : max|S - S^T| = {sym:.3e}   "
          f"{'OK' if sym < 1e-8 else 'FAIL'}")
    print(f"  S_number(3.0) at phi=0.2, K=0 = {ry.S_number(3.0):.5f}")
    print(f"  S_number(0.3) at phi=0.2, K=0 = {ry.S_number(0.3):.5f}")

    for K in (0.5, -0.5):
        try:
            r = build(0.2, K)
            print(f"  K={K:+.1f}: S_number(0.3) = {r.S_number(0.3):.5f}")
        except Exception as e:
            print(f"  K={K:+.1f}: FAILED ({type(e).__name__}: {e})")

    print("\n  Cross-check against PolydisperseOneYukawaMSA is NOT possible at")
    print("  present: that solver fails for N>1 (singular matrix at K=0, and")
    print("  'only 0-dimensional arrays can be converted to Python scalars'")
    print("  for K!=0). Consistent with its own DRAFT header and the closure")
    print("  defect recorded in docs/polydisperse_yukawa_spec.md section 7g.")
    print("  The potential convention itself is no longer in question -- it is")
    print("  taken from that spec's section 1; see _betaU().")


if __name__ == "__main__":                                 # pragma: no cover
    _selftest()
