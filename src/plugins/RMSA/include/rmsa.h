/* ------------------------------------------------------------------
 * rmsa.h -- Rescaled Mean Spherical Approximation (RMSA) structure
 * factor for charged colloids / macroion solutions.
 *
 * C port of the algorithm in:
 *   J. B. Hayter and J. Penfold, Mol. Phys. 42, 109 (1981)      [MSA]
 *   J.-P. Hansen and J. B. Hayter, Mol. Phys. 46, 651 (1982)    [rescaling]
 *
 * This follows the "improved" (default, non-Fortran-legacy) algorithm
 * used by the Jscatter package (R. Biehl, PLOS ONE 14(6), e0218789,
 * 2019): the MSA quartic is solved for *all four* roots and the
 * physically correct one (giving g(r<sigma) = 0) is selected by a
 * direct evaluation of g(r), rather than the original Hayter-Penfold
 * Fortran Newton-iteration-on-an-estimated-root approach (which is
 * known to occasionally converge to the wrong branch).
 *
 * Naming: identifiers here describe what each quantity *is* rather
 * than mimicking the terse single-letter notation of the paper /
 * reference Fortran & Python code. See the table in README.md for a
 * full correspondence between these names, the paper's symbols, and
 * jscatter's variable names, if you need to cross-check against
 * either source.
 * ------------------------------------------------------------------ */
#ifndef RMSA_H
#define RMSA_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int    rescaleIterations;      /* >=0 number of rescaling iterations on success (0 if no rescaling was needed); <0 error, see rmsa_error_string() */

    double volumeFraction;         /* physical (input) volume fraction eta */
    double contactPotential;       /* physical contact potential gamma*exp(-k) ("gek" in the paper) */
    double screeningParam;         /* physical dimensionless screening constant kappa*sigma ("ak"; clipped to <=200) */
    double couplingConstant;       /* coupling constant "gamk" = 2*eta^(1/3)*contactPotential*exp(k - k/eta^(1/3)) */

    /* MSA closure coefficients (Hayter & Penfold eq. 6):
     *   c(x) = closureConstantCoeff + closureLinearCoeff*x + (eta/2)*closureConstantCoeff*x^3
     *          + closureSinhCoeff*sinh(kx)/x + closureCoshCoeff*(cosh(kx)-1)/x        (x<1)
     * evaluated for the rescaled problem (i.e. using rescaledVolumeFraction etc below).*/
    double closureConstantCoeff;   /* "A" in the paper */
    double closureLinearCoeff;     /* "B" in the paper */
    double closureSinhCoeff;       /* "C" in the paper */
    double closureCoshCoeff;       /* "F" in the paper */

    /* Auxiliary Laplace-transform-like quantities of h(x), H(s) =
     * integral_0^inf x h(x) exp(-sx) dx (paper eq. 9-11); not needed
     * for the structure factor itself, kept for completeness/debugging. */
    double laplaceHAtScreening;    /* "U" = H(k) in the paper */
    double laplaceHAtZero;         /* "V" = H(0) in the paper */

    double rescaledVolumeFraction;   /* "seta"; == volumeFraction if no rescaling was needed */
    double rescaledContactPotential; /* "sgek" */
    double rescaledScreeningParam;   /* "sak" */
    double rescalingFactor;          /* "scal" = (volumeFraction/rescaledVolumeFraction)^(1/3); 1.0 if no rescaling */
    double contactValue;             /* g(1+) ("g1"); ~0 after successful (re)scaling. On a -4 error this
                                       * holds the best candidate's g(1+) found (or -1.0 if no candidate
                                       * quartic root existed at all) -- a diagnostic, not a valid solution. */

    /* --- Numerical-quality diagnostics (not in the original algorithm;
     * see "Numerical diagnostics" in README.md) ---------------------- */

    /* |g(r=1-0.05)| for the physical root selected out of the MSA
     * quartic's candidates (see rmsa.c's SQFUN_FULL_SOLVE branch) --
     * the residual that decides whether a candidate is accepted
     * (threshold 0.1) and, among accepted candidates, which one is
     * "most physical". Smaller is better; NAN if no quartic root
     * search was needed for this solve (the screeningParam>15
     * asymptotic shortcut skips it entirely) or on error before a
     * residual could be computed. */
    double physicalRootResidual;

    /* Number of real (or near-real) roots of the MSA quartic that were
     * considered when picking the physical solution; 0 if no quartic
     * root search was needed (the screeningParam>15 asymptotic
     * shortcut) or on error before roots could be counted. More than
     * one candidate is normal and not itself a problem -- see
     * RMSA_WARN_NEAR_DEGENERATE_ROOT below for when it's worth
     * attention. */
    int numCandidateRoots;

    /* Bitwise OR of RMSAWarningFlags: known-fragile situations that
     * this solve encountered but which did not prevent it from
     * returning a nominally successful result. RMSA_WARN_NONE (0) if
     * none applied. These are heuristic, not hard errors -- unlike a
     * negative return code, a solve with warnings set still returns
     * its ordinary (>=0) rescaleIterations and a usable solution. */
    unsigned warnings;
} RMSASolution;

/* Bits for RMSASolution.warnings. */
enum RMSAWarningFlags {
    RMSA_WARN_NONE                     = 0,
    /* Two or more candidate quartic roots independently had a
     * physical-root residual under the same 0.1 acceptance threshold
     * used to pick the physical solution -- i.e. the choice between
     * them was a genuine judgment call, not a clear winner among
     * also-rans. This is exactly the situation that makes the
     * *original* Hayter-Penfold algorithm (PW-estimate + Newton
     * refinement, still used by SasView/SASfit) pick the wrong root
     * and jump discontinuously between neighboring parameter values
     * (see README.md's GSL/wrong-root discussion) -- this library's
     * all-roots selection avoids that failure mode, but the underlying
     * near-degeneracy it's built to handle is real, and results in
     * this regime are more sensitive to the input parameters than
     * usual. (Comparing candidates by the *ratio* of their residuals
     * instead of this shared-threshold criterion was tried first and
     * rejected: it fired even on the textbook Fig. 1 example from
     * Hayter & Penfold's own paper, whose runner-up candidate turned
     * out, on direct check, not to be a competing solution at all --
     * perturbing the input by 1e-9 relative moved the accepted
     * solution by a correspondingly infinitesimal amount.) */
    RMSA_WARN_NEAR_DEGENERATE_ROOT      = 1 << 0,
    /* The accepted root's physicalRootResidual, while under the 0.1
     * acceptance threshold, was not comfortably so (>0.01): the
     * physical-root selection is a heuristic (a manually-chosen
     * threshold on a discrete approximation to eq. 12), not an exact
     * criterion, so results just inside it are less certain than
     * results comfortably inside it. */
    RMSA_WARN_MARGINAL_PHYSICAL_ROOT    = 1 << 1,
    /* rescaledScreeningParam is in [10,15]: below the threshold where
     * rmsa.c switches to a cancellation-free asymptotic formula, but
     * high enough that closureSinhCoeff/closureCoshCoeff are computed
     * as differences of large sinh/cosh(screeningParam) terms and are
     * individually small in this region -- see README.md's "Known
     * Limitations". Does not usually affect S(Q) itself (their
     * contribution is correspondingly small), but is surfaced here in
     * case those coefficients are used directly. */
    RMSA_WARN_SCREENING_NEAR_ASYMPTOTIC = 1 << 2
};


/* Solve the (R)MSA problem for a given physical volume fraction,
 * contact potential (gamma*exp(-k)), and dimensionless screening
 * constant (kappa*sigma). Returns solution->rescaleIterations >= 0 on
 * success, or one of the negative error codes below (also returned by
 * the function) on failure:
 *   -1  Newton/secant rescaling iteration did not converge
 *   -3  could not rescale to g(1+) >= 0
 *   -4  no physical root (g(r<1) close to 0) found for the quartic
 */
int rmsa_solve(double volumeFraction, double contactPotential, double screeningParam,
               RMSASolution *solution);

/* Human-readable message for a negative rmsa_solve() return code. */
const char *rmsa_error_string(int errorCode);

/* Returns 1 if K=Q*sigma is small enough that rmsa_structure_factor()
 * uses its internal small-K Taylor series path rather than the general
 * formula (eq. 14) -- see README.md's "Known Limitations" for why the
 * general formula alone needs this (terms like 24/K^3 and 24/K^4 that
 * must cancel to a finite result as K->0, losing ~10 significant
 * digits unmitigated).
 *
 * You do not need to do anything differently for points where this
 * returns 1 -- rmsa_structure_factor() already handles them internally
 * (see rmsa.c's smallKStructureFactorTaylorSeries(), validated to
 * ~1e-4 worst-case relative error across a 150-case sweep) and this
 * function exists only if you want to know which code path was used
 * for a given K, e.g. for logging or diagnostics. Before this Taylor
 * series was added, this function's threshold was the boundary between
 * "trustworthy" and "not" -- that is no longer the case.              */
int rmsa_is_small_k_risky(double K);

/* Structure factor S(K) at K = Q*sigma = 2*R*Q, given a solved
 * RMSASolution. Uses the rescaled coefficients/parameters in
 * *solution, i.e. Hayter & Penfold's equation (14).                    */
double rmsa_structure_factor(const RMSASolution *solution, double K);

/* Exact analytic dS/dK, i.e. d(rmsa_structure_factor())/dK -- same K
 * convention as rmsa_structure_factor() itself (K = qSigma = 2*R*Q),
 * so the two are meant to be called with the same K. Derived
 * symbolically (not approximated by finite differences internally)
 * from the same closed-form S(K) this library already evaluates, so
 * there's no step size to choose and no truncation error -- only the
 * same floating-point precision S(K) itself has. Exactly 0 at K=0 (by
 * symmetry: S(K) is even in K, so its derivative is odd). Uses the
 * same small-K series switch as rmsa_structure_factor() for the same
 * reason (avoiding the general formula's catastrophic cancellation
 * there), differentiated term-by-term from that same series.          */
double rmsa_structure_factor_derivative(const RMSASolution *solution, double K);

/* Convenience top-level routine mirroring jscatter.sf.RMSA(): given
 * physical parameters, solve and evaluate S(Q) for an array of Q
 * values (units 1/length, matching 1/radius and 1/screeningLength).
 *
 *   radius           sigma/2 of the macroion (length units, e.g. nm)
 *   screeningLength  Debye screening length 1/kappa, same length units
 *                    (<=0 is treated as -> 0, i.e. screeningParam -> infinity)
 *   chargeParameter  contact potential "gamma" in kT (dimensionless;
 *                    NOT gamma*exp(-k), which is what rmsa_solve()
 *                    calls "contactPotential" -- rmsa_compute() applies
 *                    the exp(-k) itself). If deriving this from a
 *                    physical macroion charge yourself: gamma =
 *                    beta*(z_m*e)^2 / (pi*eps*eps0*sigma*(2+kappa*sigma)^2)
 *                    (Hayter & Penfold eqs. 2-3a combined; sigma=2*radius,
 *                    kappa*sigma=2*radius/screeningLength, beta=1/(kB*T)) --
 *                    or use rmsa_physical.h's rmsa_physical_to_dimensionless()
 *                    to do this (and the matching screeningLength/kappa
 *                    derivation) for you; see README.md's "Physical
 *                    parameter conversion" for the full formulas.
 *   volumeFraction   0 < volumeFraction < 1
 *   q                array of scattering vectors, same units as 1/radius
 *   structureFactorOut  output array, same length as q
 *   numPoints        number of q points
 *
 * Returns solution->rescaleIterations (>=0 on success, <0 on error,
 * see rmsa_error_string). *solution is filled in regardless, for
 * inspection of the solved coefficients.
 */
int rmsa_compute(double radius, double screeningLength, double chargeParameter, double volumeFraction,
                  const double *q, double *structureFactorOut, int numPoints, RMSASolution *solution);

/* ------------------------------------------------------------------
 * Performance note / rmsa_compute_cached()
 *
 * rmsa_compute() solves for the MSA closure coefficients (the
 * quartic-root search and physical-root selection in rmsa_solve() --
 * by far the expensive part) on *every* call, then evaluates S(Q) for
 * the q array given. Those coefficients depend only on
 * (volumeFraction, screeningParam, contactPotential) -- i.e. on
 * (radius, screeningLength, chargeParameter, volumeFraction) -- never
 * on q itself. So if you're calling this once per Q value (e.g. one q
 * at a time from an external finite-difference-derivative or
 * optimizer routine that doesn't let you batch), you are re-solving
 * for the exact same coefficients every time and only actually need
 * the (already fast) final evaluation step.
 *
 * Measured on this project's own hardware: solving costs roughly
 * 1-2 ms; evaluating S(Q) from an already-solved RMSASolution costs
 * roughly 0.1 microseconds -- a factor of 10,000-20,000. Two ways to
 * get this back:
 *
 *   1. If you can batch: pass every Q value you need (including the
 *      Q+h, Q-h points of a finite-difference derivative) as one `q`
 *      array to a single rmsa_compute() call. It already solves once
 *      and evaluates every point in the array from that one solve --
 *      no other change needed.
 *   2. If you can't batch (calls arrive one Q at a time from code you
 *      don't control): call rmsa_solve() yourself once and
 *      rmsa_structure_factor() per Q, keeping the RMSASolution
 *      alive across calls as long as the physical parameters haven't
 *      changed -- or use rmsa_compute_cached() below, which does
 *      exactly that automatically.
 * ------------------------------------------------------------------ */

/* Opaque-enough cache for rmsa_compute_cached(): remembers the
 * physical parameters and RMSASolution from the last call that used
 * this same cache, and skips the (expensive) solve step entirely if
 * the next call's (radius, screeningLength, chargeParameter,
 * volumeFraction) are bit-for-bit identical to those -- which they
 * will be if your calling code holds them fixed while only Q varies
 * (e.g. evaluating a finite-difference derivative with respect to Q).
 * If any of the four change even slightly (e.g. because *they*, not
 * Q, are what you're differentiating with respect to), the next call
 * correctly falls back to a fresh solve -- this can never return a
 * solution for the wrong parameters.
 *
 * Owned entirely by the caller, exactly like RMSASolution itself --
 * NOT hidden/global library state. Zero-initialize before first use
 * (e.g. `RMSACache cache = {0};`); safe to use concurrently from
 * multiple threads only if each thread has its own cache (same rule
 * as RMSASolution -- see README.md's "Thread safety").              */
typedef struct {
    int hasSolved;                  /* 0 until the first successful solve through this cache */
    double lastRadius;
    double lastScreeningLength;
    double lastChargeParameter;
    double lastVolumeFraction;
    RMSASolution solution;
} RMSACache;

/* Like rmsa_compute(), but looks up `cache` first: if (radius,
 * screeningLength, chargeParameter, volumeFraction) are bit-for-bit
 * identical to the previous call through this same cache, reuses the
 * cached RMSASolution and skips straight to evaluating q -- otherwise
 * solves fresh (updating the cache) exactly as rmsa_compute() would.
 * *solution, if non-NULL, is filled in the same way rmsa_compute()'s
 * is (a copy of the -- possibly cached -- solved RMSASolution), for
 * inspection; pass NULL if you don't need it.                        */
int rmsa_compute_cached(double radius, double screeningLength, double chargeParameter, double volumeFraction,
                         const double *q, double *structureFactorOut, int numPoints,
                         RMSACache *cache, RMSASolution *solution);

#ifdef __cplusplus
}
#endif
#endif /* RMSA_H */
