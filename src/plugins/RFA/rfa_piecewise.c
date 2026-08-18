/* ------------------------------------------------------------------
 * rfa_piecewise.c  --  see rfa_piecewise.h for the physics reference.
 *
 * Implements, for an arbitrary number n of piece-wise constant steps:
 *
 *   - the linear relations (27)-(32) giving A0,B0,S1,S2,S3 in terms
 *     of the still-unknown A_j,B_j (j=1..n);
 *   - a robust complex cubic solver for 1+S1 s+S2 s^2+S3 s^3=0;
 *   - the n (RFA1) or 2n (RFA2) transcendental equations (39)-(40)
 *     that fix the remaining unknowns, solved by a damped Newton
 *     iteration with a numerical Jacobian;
 *   - evaluation of F(s), G(s), the structure factor S(q) [eq. (2)]
 *     and the radial distribution function in the first shell
 *     1<=r<2 [eq. (15)].
 * ------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "include/rfa_piecewise.h"

#include <gsl/gsl_errno.h>
#include <gsl/gsl_poly.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_multiroots.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* --------------------------------------------------------------------
 * GSL's default error handler prints a message and calls abort() on
 * internal errors (e.g. a solver failing to make progress) -- and it
 * is a single PROCESS-WIDE function pointer, not something scoped per
 * thread or per call. Left at its default, one thread hitting an edge
 * case would kill every thread in the process, and gsl_set_error_handler()
 * itself would be unsafe to call concurrently.
 *
 * We disable it exactly once, via a compiler "constructor" attribute:
 * this runs during program start-up, before main() (and therefore
 * before any additional thread can possibly exist), so there is no
 * race to guard against -- no locking is needed or possible to get
 * wrong. GSL routines then report failures via ordinary return codes,
 * which solve_cubic()/solve_nonlinear() below check explicitly and
 * turn into an ordinary "not converged" result instead of a process
 * abort. This is a GCC/Clang extension; see rfa_piecewise.h ("Thread
 * safety") for the portable fallback (rfa_init()) for other compilers.
 * -------------------------------------------------------------------- */
#if defined(__GNUC__) || defined(__clang__)
__attribute__((constructor))
static void rfa_gsl_error_policy(void) { gsl_set_error_handler_off(); }
#endif

void rfa_init(void) { gsl_set_error_handler_off(); }

/* ==================================================================
 *  Small root-finding / nonlinear-solver utilities, built on GSL
 * ================================================================== */

/* Solve a s^3+b s^2+c s+d = 0 (a != 0) for its three complex roots
 * using GSL's gsl_poly_complex_solve, which finds the eigenvalues of
 * the polynomial's companion matrix (balanced QR algorithm).  This is
 * considerably more robust across degenerate/near-degenerate cases
 * (repeated roots, tiny leading coefficients, ...) than a hand-rolled
 * Cardano formula.  Returns 0 on success; on failure (only possible
 * with the GSL abort handler disabled, see above) the roots are set
 * to NaN and -1 is returned, so the caller can fail gracefully rather
 * than proceed with uninitialized/garbage roots.                      */
static int solve_cubic(double a, double b, double c, double d,
                        double complex root[3])
{
    double coeff[4] = { d, c, b, a };   /* GSL wants lowest->highest degree */
    double z[6];                        /* 3 roots, each stored as (re,im) */
    gsl_poly_complex_workspace *w = gsl_poly_complex_workspace_alloc(4);
    int status = gsl_poly_complex_solve(coeff, 4, w, z);
    gsl_poly_complex_workspace_free(w);
    if (status != GSL_SUCCESS) {
        for (int i = 0; i < 3; i++) root[i] = NAN + NAN*I;
        return -1;
    }
    for (int i = 0; i < 3; i++) root[i] = z[2*i] + z[2*i+1]*I;
    return 0;
}

typedef void (*ResidFunc)(const double *x, double *F, void *ctx);

/* Bridges our plain "double* in, double* out" residual callbacks to
 * the gsl_vector-based interface required by gsl_multiroot_function.   */
typedef struct {
    ResidFunc func;
    void      *userctx;
    int        dim;
    double    *xbuf;
    double    *Fbuf;
} GSLBridge;

static int gsl_residual_wrapper(const gsl_vector *xv, void *params, gsl_vector *fv)
{
    GSLBridge *br = (GSLBridge*)params;
    for (int i = 0; i < br->dim; i++) br->xbuf[i] = gsl_vector_get(xv, i);
    br->func(br->xbuf, br->Fbuf, br->userctx);
    for (int i = 0; i < br->dim; i++) gsl_vector_set(fv, i, br->Fbuf[i]);
    return GSL_SUCCESS;
}

/* Solve func(x)=0 (dim unknowns), starting from x, using GSL's
 * derivative-free Powell hybrid solver (MINPACK's hybrd algorithm,
 * gsl_multiroot_fsolver_hybrids).  This is substantially more robust
 * than a hand-written damped Newton iteration for the transcendental
 * RFA equations, which can be poorly conditioned close to the "no
 * solution" boundaries documented in the reference papers.  x is
 * overwritten with the best iterate found; returns 1 if convergence
 * to the requested (absolute, L1) residual tolerance was reached.
 *
 * Exact/near-exact repeat short-circuit: gsl_multiroot_fsolver_set()
 * already evaluates func(x) once internally (into s->f) before any
 * iteration happens. If the starting guess is already within
 * tolerance -- e.g. warm-starting from the exact same problem, or one
 * close enough that it happens to already satisfy the residual test
 * -- checking that *before* entering the loop avoids one wasted
 * gsl_multiroot_fsolver_iterate() call (a full Jacobian
 * estimate/update plus a trust-region step) that would otherwise run
 * even though the very first residual check afterwards would have
 * immediately reported convergence anyway. Idea via analogy with an
 * "exact-match cache" short-circuit used in a similar solved-state
 * caching project; see the "Performance" section of the README.       */
static int solve_nonlinear(int dim, ResidFunc func, void *userctx, double *x,
                            int maxiter, double tol, int *iters_out)
{
    GSLBridge br = { func, userctx, dim,
                      malloc(sizeof(double)*dim), malloc(sizeof(double)*dim) };
    gsl_multiroot_function f = { &gsl_residual_wrapper, (size_t)dim, &br };

    gsl_vector *xv = gsl_vector_alloc(dim);
    for (int i = 0; i < dim; i++) gsl_vector_set(xv, i, x[i]);

    const gsl_multiroot_fsolver_type *T = gsl_multiroot_fsolver_hybrids;
    gsl_multiroot_fsolver *s = gsl_multiroot_fsolver_alloc(T, dim);
    int status = gsl_multiroot_fsolver_set(s, &f, xv);

    int iter = 0;
    if (status == GSL_SUCCESS) {
        status = gsl_multiroot_test_residual(s->f, tol);   /* check BEFORE iterating */
        while (status == GSL_CONTINUE && iter < maxiter) {
            iter++;
            status = gsl_multiroot_fsolver_iterate(s);
            if (status)                     /* solver made no progress */
                break;
            status = gsl_multiroot_test_residual(s->f, tol);
        }
    }
    int converged = (status == GSL_SUCCESS);
    for (int i = 0; i < dim; i++) x[i] = gsl_vector_get(s->x, i);

    if (iters_out) *iters_out = iter;
    gsl_multiroot_fsolver_free(s);
    gsl_vector_free(xv);
    free(br.xbuf); free(br.Fbuf);
    return converged;
}


/* ==================================================================
 *  Core RFA algebra (general n)
 * ================================================================== */

/* Given the (still to be determined) coefficients A[1..n], B[1..n]
 * this computes A0,B0,S1,S2,S3 [eqs. (27)-(32)] and the three roots
 * of the common denominator together with its derivative there.
 * Returns 0 on success, -1 if the cubic solver failed (see
 * solve_cubic()) -- in that case d->s[] holds NaNs and d should not
 * be used further.                                                    */
static int compute_derived(int n, double eta, const double *lambda,
                            const double *A, const double *B,
                            RFADerived *d)
{
    double C1 = 0.0, C2 = 0.0, C3 = 0.0, C4 = 0.0;
    for (int j = 1; j <= n; j++) {
        double dj  = lambda[j] - 1.0;
        double dj1 = dj, dj2 = dj*dj, dj3 = dj2*dj, dj4 = dj3*dj;
        C1 += A[j]*dj1 - 1.0*B[j]*1.0;
        C2 += A[j]*dj2 - 2.0*B[j]*dj1;
        C3 += A[j]*dj3 - 3.0*B[j]*dj2;
        C4 += A[j]*dj4 - 4.0*B[j]*dj3;
    }

    double B0 = (1.0 + C1 + 0.5*eta*(1.0+4.0*C1+6.0*C2+4.0*C3+C4))
                / (1.0 + 2.0*eta);
    double S1 = B0 - (1.0 + C1);
    double S2 = 0.5*(1.0 + 2.0*C1 + C2) - B0;
    double S3 = 0.5*B0 - 1.0/(12.0*eta)
                - (1.0/6.0)*(1.0 + 3.0*C1 + 3.0*C2 + C3);
    double A0 = 1.0;
    for (int j = 1; j <= n; j++) A0 -= A[j];

    d->A0 = A0; d->B0 = B0; d->S1 = S1; d->S2 = S2; d->S3 = S3;
    int status = solve_cubic(S3, S2, S1, 1.0, d->s);
    for (int k = 0; k < 3; k++)
        d->Sp[k] = S1 + 2.0*S2*d->s[k] + 3.0*S3*d->s[k]*d->s[k];
    return status;
}

/* xi_j(r), the inverse Laplace transform of s R_j(s), eq. (26).       */
static double complex xi_j_eval_raw(int j, double r, double eta,
                                     const double *A, const double *B,
                                     const RFADerived *d)
{
    double complex sum = 0.0;
    for (int a = 0; a < 3; a++) {
        double complex sa  = d->s[a];
        double complex num = A[j] + B[j]*sa;
        sum += (num/d->Sp[a]) * sa * cexp(sa*r);
    }
    return -sum/(12.0*eta);
}

/* Evaluate the residuals of the continuity conditions:
 *   res39[j] : continuity of y(r)  at r=lambda_j   [eq. (39)]
 *   res40[j] : continuity of y'(r) at r=lambda_j   [eq. (40)]
 * res40 may be NULL (RFA1 only needs res39).  A[],B[] must already
 * contain the hard-core terms A[0],B[0] (indices 0..n).
 *
 * For a fixed root s_a, cexp(-lambda[i]*s_a) does not depend on the
 * outer index j at all -- only on i and a. The nested-loop form
 * "for j { for a { for i<j { ...cexp(-lambda[i]*s_a)... } } }" was
 * recomputing it from scratch for every j>i, i.e. n(n+1)/2 complex
 * exponentials per call (per root) where only n distinct values
 * exist, making this function's cost scale like O(n^2) instead of
 * O(n) for potentials with many steps. Precomputing the n*3 distinct
 * values once, before the j-loop, fixes that -- this function runs on
 * every nonlinear-solver residual evaluation (i.e. it dominates the
 * cost of rfa1_solve()/rfa2_solve()), so this is exactly the kind of
 * "same argument evaluated repeatedly" pattern worth caching, unlike
 * the S(q)-evaluation case discussed in the README's "Performance"
 * section (which is not on the hot path).
 *
 * The inner sum itself, inner(j,a) = sum_{i=0}^{j-1} (...), is also
 * recomputed from i=0 for every j even though inner(j,a) is just
 * inner(j-1,a) plus one new term -- an O(n^2) amount of (now-cheap)
 * arithmetic rather than O(n). Accumulating it incrementally as j
 * increases, instead of resumming from scratch, makes the whole
 * function O(n) overall.                                              */
static void residual_common(int n, double eta, const double *lambda,
                             const double *beps, const double *A,
                             const double *B, const RFADerived *d,
                             double *res39, double *res40)
{
    /* Stack-allocated (VLA), not malloc'd: this runs on every residual
     * evaluation, so avoiding heap allocation overhead here matters.
     * n is the number of potential steps (a handful to a few tens in
     * realistic use), never large enough to risk stack exhaustion.    */
    /* n>0?n:1, not n: a zero-length VLA is undefined behaviour per the
     * C standard (caught by UBSan for the legitimate n=0 case, a bare
     * hard sphere with no steps) even though it happens to work under
     * GCC in practice. The loops below never touch index 0 when n=0,
     * so the one wasted slot costs nothing.                           */
    double complex Eneg[n > 0 ? n : 1][3];  /* Eneg[i][a] = cexp(-lambda[i]*s_a), i=0..n-1 */
    for (int i = 0; i < n; i++)
        for (int a = 0; a < 3; a++)
            Eneg[i][a] = cexp(-lambda[i]*d->s[a]);

    double complex running[3] = { 0.0, 0.0, 0.0 };  /* sum_{i=0}^{j-1} for each root, updated as j grows */
    for (int j = 1; j <= n; j++) {
        for (int a = 0; a < 3; a++)
            running[a] += (A[j-1] + B[j-1]*d->s[a]) * Eneg[j-1][a];  /* add the i=j-1 term */

        double complex sum39 = 0.0, sum40 = 0.0;
        for (int a = 0; a < 3; a++) {
            double complex sa = d->s[a];
            double complex factor = cexp(lambda[j]*sa)/d->Sp[a] * running[a];
            sum39 += sa*factor;
            if (res40) sum40 += sa*sa*factor;
        }
        double mult  = exp(beps[j] - beps[j+1]) - 1.0;   /* eq (39)/(40) */
        double lhs39 = B[j]/d->S3;
        res39[j] = lhs39 - mult*creal(sum39);

        if (res40) {
            double lhs40 = A[j]/d->S3 - B[j]*d->S2/(d->S3*d->S3);
            res40[j] = lhs40 - mult*creal(sum40);
        }
    }
}

/* ------------------------------------------------------------------
 * Context and residual callbacks used by the Newton solver.
 * ------------------------------------------------------------------ */
typedef struct {
    const PWPotential *pot;
    double *Afixed;   /* size n+1, only used by RFA1 */
} ResidCtx;

static void residual_RFA1(const double *x, double *F, void *vctx)
{
    ResidCtx *ctx = (ResidCtx*)vctx;
    int n = ctx->pot->n;
    /* Stack-allocated (VLA), not malloc'd: like residual_common(), this
     * runs on every residual evaluation, so heap allocation overhead
     * here is worth avoiding.                                          */
    double A[n+1], B[n+1];
    A[0] = 0.0; B[0] = 0.0;  /* unused by compute_derived, silences warnings */
    for (int j = 1; j <= n; j++) { A[j] = ctx->Afixed[j]; B[j] = x[j-1]; }

    RFADerived d;
    int status = compute_derived(n, ctx->pot->eta, ctx->pot->lambda, A, B, &d);
    if (status != 0) {
        /* Cubic solve failed for this trial point: report a large but
         * finite residual so the solver steers away rather than being
         * fed NaNs (which some solvers handle poorly) or crashing.    */
        for (int j = 1; j <= n; j++) F[j-1] = 1e10;
        return;
    }
    A[0] = d.A0; B[0] = d.B0;

    double res39[n+1];
    residual_common(n, ctx->pot->eta, ctx->pot->lambda, ctx->pot->beps,
                     A, B, &d, res39, NULL);
    for (int j = 1; j <= n; j++) F[j-1] = res39[j];
}

static void residual_RFA2(const double *x, double *F, void *vctx)
{
    ResidCtx *ctx = (ResidCtx*)vctx;
    int n = ctx->pot->n;
    double A[n+1], B[n+1];
    A[0] = 0.0; B[0] = 0.0;  /* unused by compute_derived, silences warnings */
    for (int j = 1; j <= n; j++) { A[j] = x[j-1]; B[j] = x[n+j-1]; }

    RFADerived d;
    int status = compute_derived(n, ctx->pot->eta, ctx->pot->lambda, A, B, &d);
    if (status != 0) {
        for (int j = 1; j <= 2*n; j++) F[j-1] = 1e10;
        return;
    }
    A[0] = d.A0; B[0] = d.B0;

    double res39[n+1], res40[n+1];
    residual_common(n, ctx->pot->eta, ctx->pot->lambda, ctx->pot->beps,
                     A, B, &d, res39, res40);
    for (int j = 1; j <= n; j++) {
        F[j-1]   = res39[j];
        F[n+j-1] = res40[j];
    }
}

/* ==================================================================
 *  Public API
 * ================================================================== */

/* Is v non-NULL and v[1..n] finite and not absurdly large? Used to
 * reject a warm-start guess that is itself unusable -- either because
 * it's garbage (e.g. from a non-converged, runaway previous solve --
 * see the discussion in scan_sw_ss.c / README of solves that
 * "converge" per the residual test but land far outside any
 * physically sensible range) or because it's NULL (a RFASolution that
 * was zero-initialized but never solved, or was already passed to
 * rfa_solution_free() -- which sets exactly these fields to NULL --
 * and then mistakenly reused as a warm-start source afterwards).
 * Either way this falls back to the ordinary zero-density start
 * rather than dereferencing a null pointer or being poisoned by
 * garbage.                                                            */
static int guess_is_sane(int n, const double *v)
{
    if (!v) return 0;
    for (int j = 1; j <= n; j++) {
        if (!isfinite(v[j]) || fabs(v[j]) > 1e6) return 0;
    }
    return 1;
}

/* Cheap sanity check on a PWPotential, run before any allocation in
 * rfa1_solve_warm()/rfa2_solve_warm(). Catches malformed input that
 * would otherwise either (a) crash: n<0 makes "n+1"/"2*n" negative,
 * and passing that through size_t arithmetic in malloc() wraps around
 * to a request for an astronomical amount of memory (confirmed via
 * AddressSanitizer, not just suspected) rather than failing cleanly;
 * or (b) silently "converge" to a meaningless answer: nothing
 * previously stopped eta<=0 (unphysical or, at eta=0, mathematically
 * singular -- see the 1/(12*eta) terms throughout) from being handed
 * straight to the solver, which for negative eta happily returns
 * finite-looking, plausible-magnitude, entirely meaningless A_j,B_j
 * and S(q) with converged=1 and no indication anything was wrong.
 * lambda[] not being strictly increasing from 1, or beps[n+1] not
 * being exactly 0 (both required by the model definition -- see the
 * PWPotential doc comment), are checked too; violating either of
 * those already failed safely via non-convergence rather than
 * crashing or misleading, but checking explicitly is cheap and gives
 * a clean, immediate rejection instead of a wasted solve attempt.    */
static int pot_is_valid(const PWPotential *pot)
{
    if (pot->n < 0) return 0;
    if (!(pot->eta > 0.0) || !isfinite(pot->eta)) return 0;   /* rejects <=0, NaN */
    if (pot->lambda[0] != 1.0) return 0;
    for (int j = 1; j <= pot->n; j++)
        if (!(pot->lambda[j] > pot->lambda[j-1]) || !isfinite(pot->lambda[j])) return 0;
    if (pot->beps[pot->n+1] != 0.0) return 0;
    return 1;
}

/* Fills sol with a safe, empty "did not even attempt to solve" state:
 * all buffers NULL (so rfa_solution_free() remains a safe no-op) and
 * converged=0, for pot_is_valid() rejections.                         */
static void sol_set_invalid(RFASolution *sol, const PWPotential *pot)
{
    sol->n = pot->n; sol->eta = pot->eta;
    sol->lambda = sol->beps = sol->A = sol->B = NULL;
    sol->d = (RFADerived){0};
    sol->converged = 0;
    sol->newton_iters = 0;
}

int rfa1_solve_warm(const PWPotential *pot, const RFASolution *warm_start,
                     RFASolution *sol)
{
    if (!pot_is_valid(pot)) { sol_set_invalid(sol, pot); return 0; }
    int n = pot->n;

    /* Build the initial guess entirely from pot/warm_start BEFORE
     * touching sol in any way. This makes the function safe even if
     * sol and warm_start are the SAME object -- a natural "update
     * this solution in place" call pattern, e.g.
     *   rfa1_solve_warm(&new_pot, &sol, &sol);
     * -- which would otherwise read back the freshly-malloc'd (still
     * uninitialized) sol->B through warm_start->B after sol->B was
     * already overwritten below, silently producing a wrong "solved"
     * result instead of a real warm start.                           */
    double *Afixed = malloc(sizeof(double)*(n+1));
    double *x      = malloc(sizeof(double)*n);
    for (int j = 1; j <= n; j++) {
        double aj = exp(-pot->beps[j+1]) - exp(-pot->beps[j]);  /* eq (36) */
        Afixed[j] = aj;
        x[j-1]    = pot->lambda[j]*aj;                           /* eq (37), cold-start default */
    }
    /* RFA1 only has B_j as a free unknown (A_j is always pinned to the
     * -- possibly new -- zero-density value above), so warm-starting
     * only ever overrides the B_j initial guess.                      */
    if (warm_start && warm_start->n == n && guess_is_sane(n, warm_start->B)) {
        for (int j = 1; j <= n; j++) x[j-1] = warm_start->B[j];
    }

    /* Self-aliasing "warm-start in place" (sol == warm_start): every-
     * thing needed from warm_start has just been captured into x[]
     * above, so sol's (== warm_start's) OLD buffers can, and must, be
     * freed now, before they're overwritten below -- otherwise every
     * such in-place update leaks the previous allocation. This is
     * deliberately NOT done for the ordinary (non-aliased) case: sol
     * there is a plain output parameter whose incoming pointer values
     * are never inspected, matching every rfa1_solve()/rfa2_solve()
     * call throughout this project (typically `RFASolution sol;`,
     * left deliberately uninitialized) -- freeing those would free
     * garbage pointers, corrupting the heap instead of fixing a leak.
     * See test_warmstart_safety.c for the regression test.            */
    if (sol == warm_start) {
        free(sol->lambda); free(sol->beps); free(sol->A); free(sol->B);
    }

    sol->n = n; sol->eta = pot->eta;
    sol->lambda = malloc(sizeof(double)*(n+1));
    sol->beps   = malloc(sizeof(double)*(n+2));
    memcpy(sol->lambda, pot->lambda, sizeof(double)*(n+1));
    memcpy(sol->beps,   pot->beps,   sizeof(double)*(n+2));
    sol->A = malloc(sizeof(double)*(n+1));
    sol->B = malloc(sizeof(double)*(n+1));

    ResidCtx ctx = { pot, Afixed };
    int iters = 0;
    int conv = solve_nonlinear(n, residual_RFA1, &ctx, x, 300, 1e-11, &iters);

    for (int j = 1; j <= n; j++) { sol->A[j] = Afixed[j]; sol->B[j] = x[j-1]; }
    RFADerived d;
    int dstatus = compute_derived(n, pot->eta, pot->lambda, sol->A, sol->B, &d);
    sol->A[0] = d.A0; sol->B[0] = d.B0; sol->d = d;
    sol->converged = conv && (dstatus == 0);
    sol->newton_iters = iters;

    free(Afixed); free(x);
    return sol->converged;
}

int rfa1_solve(const PWPotential *pot, RFASolution *sol)
{
    return rfa1_solve_warm(pot, NULL, sol);
}

int rfa2_solve_warm(const PWPotential *pot, const RFASolution *warm_start,
                     RFASolution *sol)
{
    if (!pot_is_valid(pot)) { sol_set_invalid(sol, pot); return 0; }
    int n = pot->n;

    /* Same "read warm_start fully before touching sol" ordering as
     * rfa1_solve_warm() above, for the same aliasing-safety reason.   */
    double *x = malloc(sizeof(double)*2*n);
    for (int j = 1; j <= n; j++) {
        double aj = exp(-pot->beps[j+1]) - exp(-pot->beps[j]);  /* eq (36) */
        x[j-1]   = aj;
        x[n+j-1] = pot->lambda[j]*aj;                            /* eq (37), cold-start default */
    }
    if (warm_start && warm_start->n == n
        && guess_is_sane(n, warm_start->A) && guess_is_sane(n, warm_start->B)) {
        for (int j = 1; j <= n; j++) {
            x[j-1]   = warm_start->A[j];
            x[n+j-1] = warm_start->B[j];
        }
    }

    /* Self-aliasing "warm-start in place" (sol == warm_start): see the
     * matching comment in rfa1_solve_warm() above for why this check
     * is here and why it must NOT be applied unconditionally.         */
    if (sol == warm_start) {
        free(sol->lambda); free(sol->beps); free(sol->A); free(sol->B);
    }

    sol->n = n; sol->eta = pot->eta;
    sol->lambda = malloc(sizeof(double)*(n+1));
    sol->beps   = malloc(sizeof(double)*(n+2));
    memcpy(sol->lambda, pot->lambda, sizeof(double)*(n+1));
    memcpy(sol->beps,   pot->beps,   sizeof(double)*(n+2));
    sol->A = malloc(sizeof(double)*(n+1));
    sol->B = malloc(sizeof(double)*(n+1));

    ResidCtx ctx = { pot, NULL };
    int iters = 0;
    int conv = solve_nonlinear(2*n, residual_RFA2, &ctx, x, 300, 1e-11, &iters);

    for (int j = 1; j <= n; j++) { sol->A[j] = x[j-1]; sol->B[j] = x[n+j-1]; }
    RFADerived d;
    int dstatus = compute_derived(n, pot->eta, pot->lambda, sol->A, sol->B, &d);
    sol->A[0] = d.A0; sol->B[0] = d.B0; sol->d = d;
    sol->converged = conv && (dstatus == 0);
    sol->newton_iters = iters;

    free(x);
    return sol->converged;
}

int rfa2_solve(const PWPotential *pot, RFASolution *sol)
{
    return rfa2_solve_warm(pot, NULL, sol);
}

void rfa_solution_free(RFASolution *sol)
{
    free(sol->lambda); free(sol->beps); free(sol->A); free(sol->B);
    sol->lambda = sol->beps = sol->A = sol->B = NULL;
}

/* F(s) = sum_j R_j(s) exp(-(lambda_j-1)s),  eqs. (12),(22).            */
static double complex Fs_eval(const RFASolution *sol, double complex s)
{
    double complex Ftot = 0.0;
    const RFADerived *d = &sol->d;
    double complex denom = 1.0 + d->S1*s + d->S2*s*s + d->S3*s*s*s;
    for (int j = 0; j <= sol->n; j++) {
        double complex Rj = -(sol->A[j] + sol->B[j]*s)/(12.0*sol->eta*denom);
        Ftot += Rj*cexp(-(sol->lambda[j]-1.0)*s);
    }
    return Ftot;
}

/* --------------------------------------------------------------------
 * q -> 0 expansion of the structure factor.
 *
 * Direct complex evaluation of G(s) at s=i q via
 *   G(s) = s F(s) e^{-s} / (1+12 eta F(s) e^{-s})
 * is numerically unsafe for small q: since F(0)=-1/(12 eta) exactly
 * [eq. (10)], the denominator 1+12 eta F(s) e^{-s} is a near-perfect
 * cancellation of two O(1) numbers whenever s is small, and the
 * resulting relative error is amplified without bound as q -> 0 (this
 * is what produces the well known G(s) ~ 1/s^2 divergence -- see
 * eq. (9) -- but computed this way it swamps the finite, physical
 * S(q) with rounding noise already for q of order 1e-2 to 1e-4).
 *
 * The fix is to expand analytically instead of dividing numerically.
 * Write H(s) = F(s) e^{-s} = sum_j R_j(s) e^{-lambda_j s}.  Because
 * F(0)=-1/(12 eta) exactly, and (as follows from the same eq. (10)
 * constraints that fix S1,S2,S3,A0,B0) the first two derivatives also
 * satisfy H'(0)=H''(0)=0, one has
 *
 *   1 + 12 eta H(s) = 12 eta [H(s)-H(0)] = 12 eta s^3 K(s),
 *
 * with K(s) = sum_{m>=0} h_{m+3} s^m regular (h_k = H^{(k)}(0)/k!).
 * Hence
 *
 *   G(s) = s H(s) / [12 eta s^3 K(s)] = L(s)/s^2,   L(s)=H(s)/(12 eta K(s)),
 *
 * where L(s) = sum_m l_m s^m is again a perfectly regular power series
 * (computed by ordinary, cancellation-free power-series division).
 * One further finds l_1=0 [no 1/s term in G(s), consistent with
 * eq. (9)], so
 *
 *   [G(s)-G(-s)]/s = 2 (l_3 - l_5 q^2 + l_7 q^4 - l_9 q^6 + ...) ,  s=iq
 *
 *   S(q) = 1 - 4 pi rho (l_3 - l_5 q^2 + l_7 q^4 - ...)                (*)
 *
 * which reduces at q=0 to S(0) = 1-4 pi rho l_3, equivalent to eq.(11)
 * (the two are cross-checked against each other in test_series_q0.c).
 * Being a plain, convergent power series with no near-cancelling
 * subtractions, (*) is accurate and stable all the way down to q=0,
 * and remains a good approximation up to a sizeable fraction of the
 * radius of convergence set by the nearest root of the common
 * denominator, min_alpha |s_alpha|.
 * -------------------------------------------------------------------- */
#define RFA_HMAX 16                    /* Taylor order of H(s) kept    */
#define RFA_LMAX (RFA_HMAX - 3)        /* highest index of l_m we get  */
#define RFA_QSERIES_TERMS 6            /* l_3,l_5,...  up to l_{3+2*5} */

/* Taylor coefficients h_k = H^{(k)}(0)/k! of H(s)=F(s)e^{-s}
 *                         = sum_j R_j(s) e^{-lambda_j s},  k=0..order.  */
static void Hs_taylor(const RFASolution *sol, int order, double *out)
{
    const RFADerived *d = &sol->d;
    double S1 = d->S1, S2 = d->S2, S3 = d->S3;

    double *p = calloc(order+1, sizeof(double));  /* 1/(1+S1 s+S2 s^2+S3 s^3) */
    p[0] = 1.0;
    for (int k = 1; k <= order; k++) {
        double sum = 0.0;
        if (k >= 1) sum += S1*p[k-1];
        if (k >= 2) sum += S2*p[k-2];
        if (k >= 3) sum += S3*p[k-3];
        p[k] = -sum;
    }

    for (int k = 0; k <= order; k++) out[k] = 0.0;

    double *r = malloc(sizeof(double)*(order+1));
    double *e = malloc(sizeof(double)*(order+1));

    for (int j = 0; j <= sol->n; j++) {
        for (int k = 0; k <= order; k++) {
            double pkm1 = (k >= 1) ? p[k-1] : 0.0;
            r[k] = -(sol->A[j]*p[k] + sol->B[j]*pkm1)/(12.0*sol->eta);
        }
        double c = -sol->lambda[j];             /* exponent -lambda_j s (NOT -(lambda_j-1)s) */
        double fact = 1.0, cp = 1.0;
        for (int k = 0; k <= order; k++) {
            if (k > 0) { fact *= k; cp *= c; }
            e[k] = cp/fact;
        }
        for (int k = 0; k <= order; k++) {
            double sum = 0.0;
            for (int i = 0; i <= k; i++) sum += r[i]*e[k-i];
            out[k] += sum;
        }
    }
    free(p); free(r); free(e);
}

/* Same Taylor construction, but for F(s) itself (used only by the
 * eq.(11)-based cross-check of S(0), rfa_compressibility_S0).         */
static void Fs_taylor(const RFASolution *sol, int order, double *out)
{
    const RFADerived *d = &sol->d;
    double S1 = d->S1, S2 = d->S2, S3 = d->S3;

    double *p = calloc(order+1, sizeof(double));
    p[0] = 1.0;
    for (int k = 1; k <= order; k++) {
        double sum = 0.0;
        if (k >= 1) sum += S1*p[k-1];
        if (k >= 2) sum += S2*p[k-2];
        if (k >= 3) sum += S3*p[k-3];
        p[k] = -sum;
    }
    for (int k = 0; k <= order; k++) out[k] = 0.0;

    double *r = malloc(sizeof(double)*(order+1));
    double *e = malloc(sizeof(double)*(order+1));
    for (int j = 0; j <= sol->n; j++) {
        for (int k = 0; k <= order; k++) {
            double pkm1 = (k >= 1) ? p[k-1] : 0.0;
            r[k] = -(sol->A[j]*p[k] + sol->B[j]*pkm1)/(12.0*sol->eta);
        }
        double c = -(sol->lambda[j]-1.0);       /* exponent -(lambda_j-1)s */
        double fact = 1.0, cp = 1.0;
        for (int k = 0; k <= order; k++) {
            if (k > 0) { fact *= k; cp *= c; }
            e[k] = cp/fact;
        }
        for (int k = 0; k <= order; k++) {
            double sum = 0.0;
            for (int i = 0; i <= k; i++) sum += r[i]*e[k-i];
            out[k] += sum;
        }
    }
    free(p); free(r); free(e);
}

/* l_0..l_{RFA_LMAX} : Taylor coefficients of L(s)=H(s)/(12 eta K(s)),
 * K(s)=sum_m h_{m+3} s^m, obtained via ordinary power-series division
 * (a triangular recursion -- no subtractive cancellation involved).    */
static void L_series(const RFASolution *sol, double *l /* size RFA_LMAX+1 */)
{
    double h[RFA_HMAX+1];
    Hs_taylor(sol, RFA_HMAX, h);

    double k[RFA_LMAX+1];
    for (int m = 0; m <= RFA_LMAX; m++) k[m] = h[m+3];

    double twelveEta = 12.0*sol->eta;
    for (int m = 0; m <= RFA_LMAX; m++) {
        double sum = h[m];
        for (int i = 1; i <= m; i++) sum -= twelveEta*k[i]*l[m-i];
        l[m] = sum/(twelveEta*k[0]);
    }
}

/* Smallest |s_alpha| among the three roots of the common denominator:
 * sets the radius of convergence of the power series above.           */
static double min_abs_root(const RFASolution *sol)
{
    double m = cabs(sol->d.s[0]);
    for (int a = 1; a < 3; a++) {
        double v = cabs(sol->d.s[a]);
        if (v < m) m = v;
    }
    return m;
}

/* S(q) from the power series (*) above, using RFA_QSERIES_TERMS terms
 * l_3,l_5,...  Valid (and, by construction, numerically stable) for
 * q well inside the radius of convergence set by min_abs_root().      */
static double structure_factor_series(const RFASolution *sol, double q)
{
    double l[RFA_LMAX+1];
    L_series(sol, l);

    double rho = 6.0*sol->eta/M_PI;
    double q2  = q*q;
    double qpow = 1.0;             /* q^(2k) */
    double sum  = 0.0;
    double sign = 1.0;
    for (int k = 0; k < RFA_QSERIES_TERMS; k++) {
        int idx = 3 + 2*k;
        if (idx > RFA_LMAX) break;
        sum  += sign*l[idx]*qpow;
        sign  = -sign;
        qpow *= q2;
    }
    return 1.0 - 4.0*M_PI*rho*sum;
}

/* S(0), the exact q->0 limit, from eq. (11):
 *   S(0) = (24/5) eta^3 [6 F^(5)(0) - F^(6)(0)] - 1 + 8 eta + 2 eta^2
 * with F^(5)(0)=5! f5, F^(6)(0)=6! f6, so
 *   S(0) = 3456 eta^3 (f5 - f6) - 1 + 8 eta + 2 eta^2.
 * (This is numerically identical, up to roundoff, to
 * structure_factor_series(sol,0.0); both are provided as a mutual
 * cross-check -- see test_series_q0.c.)                                */
double rfa_compressibility_S0(const RFASolution *sol)
{
    double f[7];
    Fs_taylor(sol, 6, f);
    double eta = sol->eta;
    return 3456.0*eta*eta*eta*(f[5]-f[6]) - 1.0 + 8.0*eta + 2.0*eta*eta;
}

/* G(s) = s F(s) e^{-s} / (1+12 eta F(s) e^{-s}),  eq. (5)/(11).
 * Direct (complex-arithmetic) evaluation -- only safe away from s=0,
 * see the long comment above.                                          */
static double complex Gs_eval(const RFASolution *sol, double complex s)
{
    double complex F  = Fs_eval(sol, s);
    double complex es = cexp(-s);
    return s*F*es/(1.0 + 12.0*sol->eta*F*es);
}

/* Static structure factor S(q), q >= 0.  For q inside (a safety
 * fraction of) the radius of convergence set by the nearest root of
 * the common denominator, the cancellation-free power series (*) is
 * used; otherwise the direct complex-arithmetic formula is used. This
 * makes S(q) accurate and stable over the whole range q>=0.            */
double rfa_structure_factor(const RFASolution *sol, double q)
{
    double q_switch = 0.5*min_abs_root(sol);
    if (fabs(q) < q_switch)
        return structure_factor_series(sol, q);

    double rho = 6.0*sol->eta/M_PI;         /* eta=(pi/6) rho sigma^3  */
    double complex s = I*q;
    double complex Gp = Gs_eval(sol, s);
    double complex Gm = Gs_eval(sol, -s);
    double complex val = 1.0 - 2.0*M_PI*rho*(Gp - Gm)/s;   /* eq. (1)/(2) */
    return creal(val);
}

double rfa_rdf_first_shell(const RFASolution *sol, double r)
{
    if (r < 1.0 || r >= 2.0) return NAN;
    double sum = 0.0;
    for (int j = 0; j <= sol->n; j++) {
        double rr = r - sol->lambda[j];
        if (rr >= 0.0) {
            double complex xi = xi_j_eval_raw(j, rr, sol->eta,
                                               sol->A, sol->B, &sol->d);
            sum += creal(xi);
        }
    }
    return sum/r;                                              /* eq. (15) */
}

double rfa_contact_jump(const RFASolution *sol, int j)
{
    return sol->B[j]/(12.0*sol->eta*sol->lambda[j]*sol->d.S3);  /* eq. (24) */
}

/* ==================================================================
 *  Convenience helpers
 * ================================================================== */

int rfa_potential_init(PWPotential *pot, int n, double eta,
                        const double *lambda_steps, const double *beps_steps)
{
    double *lambda = malloc(sizeof(double)*(n+1));
    double *beps   = malloc(sizeof(double)*(n+2));

    lambda[0] = 1.0;
    for (int j = 1; j <= n; j++) {
        lambda[j] = lambda_steps[j-1];
        if (lambda[j] <= lambda[j-1]) {   /* not strictly increasing from 1 */
            free(lambda); free(beps);
            return 0;
        }
    }
    for (int j = 1; j <= n; j++) beps[j] = beps_steps[j-1];
    beps[n+1] = 0.0;

    pot->n = n; pot->eta = eta; pot->lambda = lambda; pot->beps = beps;
    return 1;
}

void rfa_potential_free(PWPotential *pot)
{
    free(pot->lambda); free(pot->beps);
    pot->lambda = pot->beps = NULL;
}

void rfa_solution_copy(const RFASolution *src, RFASolution *dst)
{
    int n = src->n;
    dst->n = n; dst->eta = src->eta;
    dst->lambda = malloc(sizeof(double)*(n+1));
    dst->beps   = malloc(sizeof(double)*(n+2));
    dst->A      = malloc(sizeof(double)*(n+1));
    dst->B      = malloc(sizeof(double)*(n+1));
    memcpy(dst->lambda, src->lambda, sizeof(double)*(n+1));
    memcpy(dst->beps,   src->beps,   sizeof(double)*(n+2));
    memcpy(dst->A,      src->A,      sizeof(double)*(n+1));
    memcpy(dst->B,      src->B,      sizeof(double)*(n+1));
    dst->d = src->d;
    dst->converged     = src->converged;
    dst->newton_iters  = src->newton_iters;
}
