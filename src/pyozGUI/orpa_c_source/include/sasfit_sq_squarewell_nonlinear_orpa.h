#ifndef SQUAREWELL_NONLINEAR_ORPA_H
#define SQUAREWELL_NONLINEAR_ORPA_H

#define SW_ORPA_N_BASIS 4

typedef struct {
    double phi, rho_star, lam, x;
    int n_k;
    double *k_grid;
    double *Pl_grid[SW_ORPA_N_BASIS];
    double *c_hs_grid;
    double u[SW_ORPA_N_BASIS];
    int converged;
} sw_nonlinear_orpa_t;

sw_nonlinear_orpa_t *sw_nonlinear_orpa_create(double phi, double lam, double k_max, int n_k);
void sw_nonlinear_orpa_free(sw_nonlinear_orpa_t *m);
int sw_nonlinear_orpa_solve(sw_nonlinear_orpa_t *m, double eps_kT,
                             int n_continuation_steps, double eps_kT_start);
double sw_nonlinear_orpa_S(const sw_nonlinear_orpa_t *m, double k);

#endif
