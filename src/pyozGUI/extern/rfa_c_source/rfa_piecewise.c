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

#if defined(__GNUC__) || defined(__clang__)
__attribute__((constructor))
static void rfa_gsl_error_policy(void) { gsl_set_error_handler_off(); }
#endif

void rfa_init(void) { gsl_set_error_handler_off(); }

static int solve_cubic(double a, double b, double c, double d,
                        double complex root[3])
{
    double coeff[4] = { d, c, b, a };
    double z[6];
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
        status = gsl_multiroot_test_residual(s->f, tol);
        while (status == GSL_CONTINUE && iter < maxiter) {
            iter++;
            status = gsl_multiroot_fsolver_iterate(s);
            if (status)
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

static void residual_common(int n, double eta, const double *lambda,
                             const double *beps, const double *A,
                             const double *B, const RFADerived *d,
                             double *res39, double *res40)
{
    double complex Eneg[n > 0 ? n : 1][3];
    for (int i = 0; i < n; i++)
        for (int a = 0; a < 3; a++)
            Eneg[i][a] = cexp(-lambda[i]*d->s[a]);

    double complex running[3] = { 0.0, 0.0, 0.0 };
    for (int j = 1; j <= n; j++) {
        for (int a = 0; a < 3; a++)
            running[a] += (A[j-1] + B[j-1]*d->s[a]) * Eneg[j-1][a];

        double complex sum39 = 0.0, sum40 = 0.0;
        for (int a = 0; a < 3; a++) {
            double complex sa = d->s[a];
            double complex factor = cexp(lambda[j]*sa)/d->Sp[a] * running[a];
            sum39 += sa*factor;
            if (res40) sum40 += sa*sa*factor;
        }
        double mult  = exp(beps[j] - beps[j+1]) - 1.0;
        double lhs39 = B[j]/d->S3;
        res39[j] = lhs39 - mult*creal(sum39);

        if (res40) {
            double lhs40 = A[j]/d->S3 - B[j]*d->S2/(d->S3*d->S3);
            res40[j] = lhs40 - mult*creal(sum40);
        }
    }
}

typedef struct {
    const PWPotential *pot;
    double *Afixed;
} ResidCtx;

static void residual_RFA1(const double *x, double *F, void *vctx)
{
    ResidCtx *ctx = (ResidCtx*)vctx;
    int n = ctx->pot->n;
    double A[n+1], B[n+1];
    A[0] = 0.0; B[0] = 0.0;
    for (int j = 1; j <= n; j++) { A[j] = ctx->Afixed[j]; B[j] = x[j-1]; }

    RFADerived d;
    int status = compute_derived(n, ctx->pot->eta, ctx->pot->lambda, A, B, &d);
    if (status != 0) {
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
    A[0] = 0.0; B[0] = 0.0;
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

static int guess_is_sane(int n, const double *v)
{
    if (!v) return 0;
    for (int j = 1; j <= n; j++) {
        if (!isfinite(v[j]) || fabs(v[j]) > 1e6) return 0;
    }
    return 1;
}

static int pot_is_valid(const PWPotential *pot)
{
    if (pot->n < 0) return 0;
    if (!(pot->eta > 0.0) || !isfinite(pot->eta)) return 0;
    if (pot->lambda[0] != 1.0) return 0;
    for (int j = 1; j <= pot->n; j++)
        if (!(pot->lambda[j] > pot->lambda[j-1]) || !isfinite(pot->lambda[j])) return 0;
    if (pot->beps[pot->n+1] != 0.0) return 0;
    return 1;
}

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

    double *Afixed = malloc(sizeof(double)*(n+1));
    double *x      = malloc(sizeof(double)*n);
    for (int j = 1; j <= n; j++) {
        double aj = exp(-pot->beps[j+1]) - exp(-pot->beps[j]);
        Afixed[j] = aj;
        x[j-1]    = pot->lambda[j]*aj;
    }
    if (warm_start && warm_start->n == n && guess_is_sane(n, warm_start->B)) {
        for (int j = 1; j <= n; j++) x[j-1] = warm_start->B[j];
    }

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

    double *x = malloc(sizeof(double)*2*n);
    for (int j = 1; j <= n; j++) {
        double aj = exp(-pot->beps[j+1]) - exp(-pot->beps[j]);
        x[j-1]   = aj;
        x[n+j-1] = pot->lambda[j]*aj;
    }
    if (warm_start && warm_start->n == n
        && guess_is_sane(n, warm_start->A) && guess_is_sane(n, warm_start->B)) {
        for (int j = 1; j <= n; j++) {
            x[j-1]   = warm_start->A[j];
            x[n+j-1] = warm_start->B[j];
        }
    }

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

#define RFA_HMAX 16
#define RFA_LMAX (RFA_HMAX - 3)
#define RFA_QSERIES_TERMS 6

static void Hs_taylor(const RFASolution *sol, int order, double *out)
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
        double c = -sol->lambda[j];
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
        double c = -(sol->lambda[j]-1.0);
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

static void L_series(const RFASolution *sol, double *l)
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

static double min_abs_root(const RFASolution *sol)
{
    double m = cabs(sol->d.s[0]);
    for (int a = 1; a < 3; a++) {
        double v = cabs(sol->d.s[a]);
        if (v < m) m = v;
    }
    return m;
}

static double structure_factor_series(const RFASolution *sol, double q)
{
    double l[RFA_LMAX+1];
    L_series(sol, l);

    double rho = 6.0*sol->eta/M_PI;
    double q2  = q*q;
    double qpow = 1.0;
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

double rfa_compressibility_S0(const RFASolution *sol)
{
    double f[7];
    Fs_taylor(sol, 6, f);
    double eta = sol->eta;
    return 3456.0*eta*eta*eta*(f[5]-f[6]) - 1.0 + 8.0*eta + 2.0*eta*eta;
}

static double complex Gs_eval(const RFASolution *sol, double complex s)
{
    double complex F  = Fs_eval(sol, s);
    double complex es = cexp(-s);
    return s*F*es/(1.0 + 12.0*sol->eta*F*es);
}

double rfa_structure_factor(const RFASolution *sol, double q)
{
    double q_switch = 0.5*min_abs_root(sol);
    if (fabs(q) < q_switch)
        return structure_factor_series(sol, q);

    double rho = 6.0*sol->eta/M_PI;
    double complex s = I*q;
    double complex Gp = Gs_eval(sol, s);
    double complex Gm = Gs_eval(sol, -s);
    double complex val = 1.0 - 2.0*M_PI*rho*(Gp - Gm)/s;
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
    return sum/r;
}

double rfa_contact_jump(const RFASolution *sol, int j)
{
    return sol->B[j]/(12.0*sol->eta*sol->lambda[j]*sol->d.S3);
}

int rfa_potential_init(PWPotential *pot, int n, double eta,
                        const double *lambda_steps, const double *beps_steps)
{
    double *lambda = malloc(sizeof(double)*(n+1));
    double *beps   = malloc(sizeof(double)*(n+2));

    lambda[0] = 1.0;
    for (int j = 1; j <= n; j++) {
        lambda[j] = lambda_steps[j-1];
        if (lambda[j] <= lambda[j-1]) {
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
