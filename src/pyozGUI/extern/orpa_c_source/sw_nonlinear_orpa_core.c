#include "include/sasfit_sq_squarewell_nonlinear_orpa.h"
#include "include/squarewell_common.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_multiroots.h>

static double trapz(const double *y, const double *xg, int n)
{
    double s = 0.0;
    for (int i = 0; i < n - 1; i++)
        s += 0.5 * (y[i] + y[i+1]) * (xg[i+1] - xg[i]);
    return s;
}

sw_nonlinear_orpa_t *sw_nonlinear_orpa_create(double phi, double lam, double k_max, int n_k)
{
    sw_nonlinear_orpa_t *m = malloc(sizeof(sw_nonlinear_orpa_t));
    m->phi = phi;
    m->rho_star = 6.0 * phi / M_PI;
    m->lam = lam;
    m->n_k = n_k;
    m->x = 0.0;
    m->converged = 0;
    memset(m->u, 0, sizeof(m->u));

    m->k_grid = malloc(n_k * sizeof(double));
    m->c_hs_grid = malloc(n_k * sizeof(double));
    for (int l = 0; l < SW_ORPA_N_BASIS; l++) m->Pl_grid[l] = malloc(n_k * sizeof(double));

    for (int i = 0; i < n_k; i++) {
        double k = 1e-4 + (k_max - 1e-4) * i / (n_k - 1);
        m->k_grid[i] = k;
        m->c_hs_grid[i] = sw_c0_hat(k, phi);
        for (int l = 0; l < SW_ORPA_N_BASIS; l++) m->Pl_grid[l][i] = sw_Phat(l, k);
    }
    return m;
}

void sw_nonlinear_orpa_free(sw_nonlinear_orpa_t *m)
{
    free(m->k_grid); free(m->c_hs_grid);
    for (int l = 0; l < SW_ORPA_N_BASIS; l++) free(m->Pl_grid[l]);
    free(m);
}

static int orpa_residual(const gsl_vector *u, void *params, gsl_vector *f)
{
    sw_nonlinear_orpa_t *m = (sw_nonlinear_orpa_t *)params;
    int n_k = m->n_k;
    double *c = malloc(n_k * sizeof(double));
    double *integrand = malloc(n_k * sizeof(double));

    for (int i = 0; i < n_k; i++) {
        double ci = m->c_hs_grid[i] + sw_fhat_mayer(m->k_grid[i], m->x, m->lam);
        for (int j = 0; j < SW_ORPA_N_BASIS; j++)
            ci += gsl_vector_get(u, j) * m->Pl_grid[j][i];
        c[i] = ci;
    }

    for (int l = 0; l < SW_ORPA_N_BASIS; l++) {
        for (int i = 0; i < n_k; i++) {
            double X = c[i] / (1.0 - m->rho_star * c[i]);
            integrand[i] = m->k_grid[i]*m->k_grid[i] * m->Pl_grid[l][i] * X;
        }
        double val = trapz(integrand, m->k_grid, n_k) / (2.0*M_PI*M_PI) + SW_PHAT0[l];
        gsl_vector_set(f, l, val);
    }

    free(c); free(integrand);
    return GSL_SUCCESS;
}

static int solve_step(sw_nonlinear_orpa_t *m, double x, const double *u0, double *u_out)
{
    m->x = x;
    const gsl_multiroot_fsolver_type *T = gsl_multiroot_fsolver_hybrids;
    gsl_multiroot_fsolver *s = gsl_multiroot_fsolver_alloc(T, SW_ORPA_N_BASIS);

    gsl_multiroot_function F = {&orpa_residual, SW_ORPA_N_BASIS, m};
    gsl_vector *u_init = gsl_vector_alloc(SW_ORPA_N_BASIS);
    for (int j = 0; j < SW_ORPA_N_BASIS; j++) gsl_vector_set(u_init, j, u0[j]);

    gsl_multiroot_fsolver_set(s, &F, u_init);

    int status, iter = 0;
    do {
        iter++;
        status = gsl_multiroot_fsolver_iterate(s);
        if (status) break;
        status = gsl_multiroot_test_residual(s->f, 1e-8);
    } while (status == GSL_CONTINUE && iter < 200);

    int ok = (status == GSL_SUCCESS);
    if (ok)
        for (int j = 0; j < SW_ORPA_N_BASIS; j++) u_out[j] = gsl_vector_get(s->x, j);

    gsl_vector_free(u_init);
    gsl_multiroot_fsolver_free(s);
    return ok;
}

int sw_nonlinear_orpa_solve(sw_nonlinear_orpa_t *m, double eps_kT,
                             int n_continuation_steps, double eps_kT_start)
{
    double u[SW_ORPA_N_BASIS] = {0, 0, 0, 0};
    int converged = 1;

    if (n_continuation_steps < 2) n_continuation_steps = 2;

    for (int i = 0; i < n_continuation_steps; i++) {
        double e = eps_kT_start + (eps_kT - eps_kT_start) * i / (n_continuation_steps - 1);
        double x = exp(e) - 1.0;
        double u_new[SW_ORPA_N_BASIS];
        int ok = solve_step(m, x, u, u_new);
        if (!ok) { converged = 0; break; }
        memcpy(u, u_new, sizeof(u));
    }

    memcpy(m->u, u, sizeof(u));
    m->x = exp(eps_kT) - 1.0;
    m->converged = converged;
    return converged;
}

double sw_nonlinear_orpa_S(const sw_nonlinear_orpa_t *m, double k)
{
    double c = sw_c0_hat(k, m->phi) + sw_fhat_mayer(k, m->x, m->lam);
    for (int j = 0; j < SW_ORPA_N_BASIS; j++) c += m->u[j] * sw_Phat(j, k);
    return 1.0 / (1.0 - m->rho_star * c);
}
