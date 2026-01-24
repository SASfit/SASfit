#include <math.h>
#include <stdlib.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include "levin_hankel.h"

/* map r in [a,b] to t in [-1,1] */
static inline double r_to_t(double r, double a, double b) {
    return 2.0*(r - a)/(b - a) - 1.0;
}
static inline double t_to_r(double t, double a, double b) {
    return a + 0.5*(t + 1.0)*(b - a);
}

/* polynomial eval sum c_k t^k */
static double complex poly_eval(const double complex *c, int N, double t) {
    double complex res = 0.0 + 0.0*I;
    double complex tp = 1.0 + 0.0*I;
    for (int k = 0; k < N; ++k) {
        res += c[k]*tp;
        tp *= t;
    }
    return res;
}

double complex levin_integrate_exp_ikr(
    levin_F_t F,
    void *user_data,
    const levin_params_t *params
) {
    int N = params->N;
    double a = params->a;
    double b = params->b;
    double k = params->k;

    double *t_nodes = malloc(N*sizeof(double));
    double *r_nodes = malloc(N*sizeof(double));
    if (!t_nodes || !r_nodes) { free(t_nodes); free(r_nodes); return 0.0 + 0.0*I; }

    for (int j = 0; j < N; ++j) {
        double theta = M_PI*(j + 0.5)/N;
        double t = cos(theta);
        t_nodes[j] = t;
        r_nodes[j] = t_to_r(t, a, b);
    }

    gsl_matrix_complex *M = gsl_matrix_complex_alloc(N, N);
    gsl_vector_complex *rhs = gsl_vector_complex_alloc(N);
    gsl_vector_complex *c = gsl_vector_complex_alloc(N);
    if (!M || !rhs || !c) {
        gsl_matrix_complex_free(M); gsl_vector_complex_free(rhs); gsl_vector_complex_free(c);
        free(t_nodes); free(r_nodes);
        return 0.0 + 0.0*I;
    }

    double dtdr = 2.0/(b - a);

    for (int j = 0; j < N; ++j) {
        double t = t_nodes[j];
        double r = r_nodes[j];
        double complex Fj = F(r, user_data);

        for (int col = 0; col < N; ++col) {
            double tk = 1.0;
            for (int m = 0; m < col; ++m) tk *= t;

            double dtk_dt = 0.0;
            if (col >= 1) {
                double tkm1 = 1.0;
                for (int m = 0; m < col-1; ++m) tkm1 *= t;
                dtk_dt = col*tkm1;
            }

            double complex val = dtk_dt*dtdr + I*k*tk;
            gsl_matrix_complex_set(M, j, col,
                gsl_complex_rect(creal(val), cimag(val)));
        }

        gsl_vector_complex_set(rhs, j,
            gsl_complex_rect(creal(Fj), cimag(Fj)));
    }

    gsl_permutation *p = gsl_permutation_alloc(N);
    int sign;
    gsl_linalg_complex_LU_decomp(M, p, &sign);
    gsl_linalg_complex_LU_solve(M, p, rhs, c);

    double complex *c_arr = malloc(N*sizeof(double complex));
    for (int i = 0; i < N; ++i) {
        gsl_complex ci = gsl_vector_complex_get(c, i);
        c_arr[i] = GSL_REAL(ci) + I*GSL_IMAG(ci);
    }

    double t_a = r_to_t(a, a, b);
    double t_b = r_to_t(b, a, b);
    double complex g_a = poly_eval(c_arr, N, t_a);
    double complex g_b = poly_eval(c_arr, N, t_b);

    double complex Ival = g_b*cexp(I*k*b) - g_a*cexp(I*k*a);

    free(c_arr);
    gsl_permutation_free(p);
    gsl_matrix_complex_free(M);
    gsl_vector_complex_free(rhs);
    gsl_vector_complex_free(c);
    free(t_nodes); free(r_nodes);

    return Ival;
}

/* ---------- Hankel tail using Levin + Bessel asymptotics ---------- */

typedef struct {
    hankel_f_t f;
    void *user;
    double k;
    int n;
} hankel_tail_ctx_t;

static double complex F_tail_wrapper(double r, void *user) {
    hankel_tail_ctx_t *ctx = (hankel_tail_ctx_t *)user;
    double fval = ctx->f(r, ctx->user);
    double k = ctx->k;
    /* amplitude: f(r)*r*sqrt(2/(pi*k*r)) times extra sqrt(r) to get F(r) */
    /* we want F(r) such that ∫ F(r) e^{i k r} gives cos term;
       final phase shift handled outside. */
    double amp = fval * r * sqrt(2.0/(M_PI*k*r));
    return amp + 0.0*I;
}

double hankel_tail_levin_int_order(
    hankel_f_t f,
    void *user_data,
    int n,
    double k,
    double R0,
    double R1,
    int N
) {
    hankel_tail_ctx_t ctx = {f, user_data, k, n};
    levin_params_t params;
    params.a = R0;
    params.b = R1;
    params.k = k;
    params.N = N;

    double alpha_n = n*M_PI/2.0 + M_PI/4.0;

    double complex I_exp = levin_integrate_exp_ikr(
        F_tail_wrapper, &ctx, &params
    );

    /* Hankel tail contribution ~ Re( e^{-i alpha_n} * I_exp ) */
    double complex weighted = cexp(-I*alpha_n) * I_exp;
    return creal(weighted);
}
