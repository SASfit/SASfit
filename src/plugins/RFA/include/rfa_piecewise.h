/* ------------------------------------------------------------------
 * rfa_piecewise.h
 *
 * Rational-Function Approximation (RFA) for the static structure
 * factor and radial distribution function of a fluid whose particles
 * interact through a hard core of unit diameter plus an arbitrary
 * number n of piece-wise constant potential steps (square wells
 * and/or square shoulders):
 *
 *   phi(r) = infinity                        r < 1
 *          = eps_1                    1        < r < lambda_1
 *          = eps_2                    lambda_1  < r < lambda_2
 *            ...
 *          = eps_n                    lambda_{n-1} < r < lambda_n
 *          = 0                        r > lambda_n
 *
 * Reference (equation numbers quoted throughout the .c file refer to
 * this paper):
 *
 *   A. Santos, S.B. Yuste and M. Lopez de Haro,
 *   "Rational-function approximation for fluids interacting via
 *    piece-wise constant potentials", Condens. Matter Phys. 15,
 *   23602 (2012).
 *
 * and its explicit n=2 illustration in
 *
 *   A. Santos, S.B. Yuste, M. Lopez de Haro, M. Barcenas, P. Orea,
 *   "Structural properties of fluids interacting via piece-wise
 *    constant potentials with a hard core", J. Chem. Phys. 139,
 *   074505 (2013).
 *
 * Two flavours of the theory are implemented:
 *
 *   RFA1 : the n coefficients A_j (j=1..n) are fixed at their exact
 *          zero-density values [eq. (38)] and only the n coefficients
 *          B_j are determined by imposing continuity of the cavity
 *          function y(r) at each step [eq. (39)] -- n transcendental
 *          equations.
 *
 *   RFA2 : both A_j and B_j (j=1..n), i.e. 2n unknowns, are determined
 *          by additionally imposing continuity of y'(r) at each step
 *          [eqs. (39)+(40)] -- 2n transcendental equations.
 *
 * A 3-step potential (n=3) and a general n-step potential are both
 * just particular uses of this same engine -- see demo_3step.c and
 * demo_nstep.c.
 *
 * Thread safety
 * -------------
 * The engine keeps no global or static mutable state: every quantity
 * lives either in a caller-owned PWPotential/RFASolution or in
 * function-local buffers, and every GSL workspace/solver object is
 * allocated and freed within a single call, never shared. As a result:
 *
 *   - Any number of threads may call rfa1_solve()/rfa2_solve() at the
 *     same time, as long as each call is given its own RFASolution
 *     (the PWPotential input is only read, so many threads may safely
 *     share -- but not concurrently mutate -- the same one).
 *   - Once a RFASolution has been solved, it may be read concurrently
 *     by any number of threads via rfa_structure_factor(),
 *     rfa_compressibility_S0(), rfa_rdf_first_shell() and
 *     rfa_contact_jump(): none of these write to the RFASolution.
 *   - A single RFASolution object must not be solved (rfa1_solve/
 *     rfa2_solve) or freed (rfa_solution_free) concurrently with any
 *     other access to that same object -- guard that with your own
 *     lock (e.g. a pthread_rwlock_t: solve/free take the write lock,
 *     queries take the read lock) if one RFASolution is refreshed
 *     in place while other threads may be reading it.
 *
 * The one process-wide (not per-thread) piece of state this library
 * touches is GSL's error handler, which by default calls abort() on
 * internal errors. On GCC/Clang it is disabled automatically via a
 * constructor function that runs before main() -- and therefore
 * before any thread but the initial one can exist, so this requires
 * no locking. On other compilers, call rfa_init() once yourself,
 * before creating any additional threads.  See test_threads.c for a
 * pthreads stress test exercising both bullet points above.
 * ------------------------------------------------------------------ */
#ifndef RFA_PIECEWISE_H
#define RFA_PIECEWISE_H

#include <complex.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Disables GSL's default abort-on-error handler (see "Thread safety"
 * above). Idempotent. Called automatically on GCC/Clang; on other
 * compilers, call this once yourself, before spawning any threads
 * that will use this library.                                         */
void rfa_init(void);

/* ------------------------------------------------------------------
 * Description of the piece-wise constant potential + thermodynamic
 * state.  All lengths are in units of the hard-core diameter sigma=1.
 *
 *   lambda[0]      = 1                     (fixed, hard core)
 *   lambda[1..n]   = lambda_1 < lambda_2 < ... < lambda_n
 *   beps[1..n]     = eps_j / (kB T)          (>0 shoulder, <0 well)
 *   beps[n+1]      = 0                     (fixed convention eps_{n+1}=0)
 *   eta            = (pi/6) rho sigma^3      (packing fraction)
 *
 * lambda[] must have n+1 entries (indices 0..n).
 * beps[]   must have n+2 entries (indices 0..n+1; beps[0] is unused).
 * ------------------------------------------------------------------ */
typedef struct {
    int     n;
    double  eta;
    double *lambda;   /* size n+1 */
    double *beps;     /* size n+2 */
} PWPotential;

/* Coefficients of the common denominator 1+S1 s+S2 s^2+S3 s^3 and its
 * three (generally complex) roots, plus the two coefficients A0,B0
 * of the hard-core term R_0(s).                                       */
typedef struct {
    double complex s[3];    /* roots of 1+S1 s+S2 s^2+S3 s^3=0 */
    double complex Sp[3];   /* S1+2 S2 s+3 S3 s^2 at each root  */
    double A0, B0, S1, S2, S3;
} RFADerived;

/* A fully solved RFA1 or RFA2 model: everything needed to evaluate
 * G(s), S(q) and g(r) in the first shell.                             */
typedef struct {
    int         n;
    double      eta;
    double     *lambda;  /* size n+1, private copy */
    double     *beps;    /* size n+2, private copy */
    double     *A;       /* size n+1, A[0..n]      */
    double     *B;       /* size n+1, B[0..n]      */
    RFADerived  d;
    int         converged;
    int         newton_iters;
} RFASolution;

/* Solve the RFA1 / RFA2 equations for the given potential.  Returns 1
 * on convergence to the requested tolerance, 0 otherwise (the best
 * iterate found is still returned in *sol, but should be regarded
 * with caution -- the papers above document parameter regions, e.g.
 * high density and/or deep or wide wells, where no solution of the
 * RFA equations exists).
 *
 * pot is validated before anything else is attempted: n<0, eta<=0 or
 * non-finite, lambda[] not strictly increasing from 1, or beps[n+1]
 * not exactly 0 (all required by the model -- see PWPotential above)
 * make this return 0 immediately, with every RFASolution buffer left
 * NULL. n=0 (a bare hard sphere, no steps at all) is valid and solves
 * trivially. This validation exists because, without it, negative eta
 * (unphysical) could previously "converge" to finite, plausible-
 * looking A_j,B_j and S(q) with no indication anything was wrong, and
 * negative n could reach malloc() with a size_t-wrapped negative size
 * -- see test_input_validation.c for the regression tests.
 *
 * Call rfa_solution_free() when done, REGARDLESS of the return value:
 * buffers are allocated (or, after a validation failure, explicitly
 * left NULL, which rfa_solution_free() also handles safely) whether
 * or not the solve converged.                                        */
int rfa1_solve(const PWPotential *pot, RFASolution *sol);
int rfa2_solve(const PWPotential *pot, RFASolution *sol);

/* Same as rfa1_solve()/rfa2_solve() (including the same pot
 * validation -- see above), but the nonlinear solve starts from
 * warm_start's B_j (RFA1) or A_j,B_j (RFA2) instead of the built-in
 * zero-density formula -- pass NULL for the ordinary (cold-start)
 * behaviour, equivalent to rfa1_solve()/rfa2_solve().
 *
 * This matters when solving a *sequence* of slightly-perturbed
 * potentials, e.g. for a numerical derivative of S(q) with respect to
 * eta, T*, lambda, ...: since A_j,B_j (and hence the roots, which are
 * a deterministic function of them) move only slightly for a slightly
 * perturbed potential, warm-starting from the previous point's
 * solution typically cuts the number of nonlinear-solver iterations
 * (and therefore wall-clock time) by 3-10x compared to the generic
 * zero-density starting guess -- see the "Performance: warm-starting"
 * section of the README and bench_warmstart.c for measurements.
 *
 * warm_start does not need to be converged, and its n/lambda need not
 * match pot's exactly (only n is checked -- differing lambda/eta/beps
 * is exactly the point); if warm_start->n != pot->n, or the guess
 * contains a non-finite or wildly-out-of-range value, this silently
 * falls back to the ordinary zero-density start rather than risk
 * feeding the solver a bad guess.
 *
 * sol may alias warm_start (rfa2_solve_warm(&pot, &sol, &sol), i.e.
 * "update this solution in place") -- handled correctly and without
 * leaking: see test_warmstart_safety.c.                               */
int rfa1_solve_warm(const PWPotential *pot, const RFASolution *warm_start,
                     RFASolution *sol);
int rfa2_solve_warm(const PWPotential *pot, const RFASolution *warm_start,
                     RFASolution *sol);

void rfa_solution_free(RFASolution *sol);

/* Static structure factor S(q), q >= 0.  Internally this dispatches
 * between two mathematically equivalent evaluations of the same
 * exact formula [eq. (1)/(2)]: a cancellation-free power series in q^2
 * (built from analytic derivatives of F(s) at s=0) for q inside a
 * safety fraction of the radius of convergence, and the direct
 * complex-arithmetic formula otherwise -- see rfa_piecewise.c for the
 * derivation. This makes S(q) accurate and numerically stable for the
 * *entire* range q>=0, including q=0 itself, with no special-casing
 * required by the caller.                                              */
double rfa_structure_factor(const RFASolution *sol, double q);

/* S(0), the exact q->0 (compressibility) limit, from the analytic
 * Taylor expansion of F(s) at s=0 to order s^6 [eq. (11)]; no
 * finite-difference numerics are involved. Provided directly (in
 * addition to being reachable as rfa_structure_factor(sol,0.0), which
 * uses an independent derivation) as a cross-check and for
 * convenience.                                                         */
double rfa_compressibility_S0(const RFASolution *sol);

/* Radial distribution function in the first shell, 1 <= r < 2.  This
 * expression is exact within the RFA model (not a low-density
 * truncation), since only a single term of the formal density series
 * for g(r) contributes there.  Returns NAN outside [1,2).             */
double rfa_rdf_first_shell(const RFASolution *sol, double r);

/* Jump of g(r) at the j-th step,  g(lambda_j^+) - g(lambda_j^-).       */
double rfa_contact_jump(const RFASolution *sol, int j);

/* --------------------------------------------------------------------
 * Convenience helpers
 * -------------------------------------------------------------------- */

/* Builds a PWPotential from just the "per-step" data, so the caller
 * doesn't need to know about the lambda[0]=1 and beps[n+1]=0 padding
 * convention (a plausible off-by-one source of error when building
 * lambda[]/beps[] by hand -- every example in this project does it
 * correctly, but a new caller doing it themselves is one array-index
 * mistake away from silently solving the wrong potential).
 *
 * lambda_steps and beps_steps must each have exactly n entries:
 *   lambda_steps[0..n-1] = lambda_1..lambda_n  (all > 1, increasing)
 *   beps_steps[0..n-1]   = eps_1/kT .. eps_n/kT
 *
 * Allocates pot->lambda (size n+1) and pot->beps (size n+2) and fills
 * in the padding automatically. Free with rfa_potential_free().
 * Returns 1 on success, 0 if lambda_steps is not strictly increasing
 * from 1 (the one thing this can't silently fix for you).             */
int rfa_potential_init(PWPotential *pot, int n, double eta,
                        const double *lambda_steps, const double *beps_steps);

void rfa_potential_free(PWPotential *pot);

/* Deep-copies a solved RFASolution (independent lambda/beps/A/B
 * buffers, not shared with src). dst is treated exactly like the dst
 * of rfa1_solve()/rfa2_solve(): it is unconditionally (re)initialized,
 * so if it already holds live buffers from an earlier solve, free
 * those first (rfa_solution_free()) to avoid leaking them -- the same
 * convention as everywhere else in this API. src and dst may not
 * alias (copying onto yourself is a no-op you don't need this for).   */
void rfa_solution_copy(const RFASolution *src, RFASolution *dst);

#ifdef __cplusplus
}
#endif
#endif /* RFA_PIECEWISE_H */
