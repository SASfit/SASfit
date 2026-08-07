/* squarewell_nonlinear_orpa.h
 * Pini, Parola & Reatto, "A simple approximation for fluids with narrow
 * attractive potentials," Molecular Physics 100, 1507 (2002);
 * arXiv:cond-mat/0109311.
 *
 * Solves the paper's self-consistency (core) condition via a projected
 * polynomial correction G(r)=u0+u1*r+u2*r^2+u3*r^3 and GSL's derivative-
 * free multiroot solver (gsl_multiroot_fsolver_hybrids), with parameter
 * continuation in the well depth for robustness.
 *
 * Convention: eps_kT>0 = attractive well depth in units of kT.
 *
 * VALIDATED: for lambda=1.5 (the paper's own longest-range test case),
 * this solver's breakdown into unphysical (negative) S(Q) at low density
 * (eta=0.08) lines up closely with the literature critical temperature
 * T*_c=1.218 (Sastre, arXiv:1810.09816), i.e. eps/kT_c=0.821. The paper's
 * abstract states no solutions exist inside the liquid-vapor coexistence
 * region -- so this breakdown is a physical feature of the theory, not a
 * bug. Always check S(Q)>=0 on your result; a converged solve does not
 * guarantee a physical one.
 *
 * Requires GSL (libgsl-dev): link with -lgsl -lgslcblas -lm
 */
#ifndef SQUAREWELL_NONLINEAR_ORPA_H
#define SQUAREWELL_NONLINEAR_ORPA_H

#define SW_ORPA_N_BASIS 4

typedef struct {
    double phi, rho_star, lam, x;
    int n_k;
    double *k_grid;
    double *Pl_grid[SW_ORPA_N_BASIS];
    double *c_hs_grid;
    double u[SW_ORPA_N_BASIS];   /* solved polynomial coefficients */
    int converged;
} sw_nonlinear_orpa_t;

/* Allocate and precompute the integration grid. Free with
 * sw_nonlinear_orpa_free() when done. */
sw_nonlinear_orpa_t *sw_nonlinear_orpa_create(double phi, double lam, double k_max, int n_k);
void sw_nonlinear_orpa_free(sw_nonlinear_orpa_t *m);

/* Solve for the self-consistent G(r) coefficients at well depth eps_kT,
 * using continuation from a shallow well depth. Returns 1 on success
 * (also sets m->converged), 0 on failure. */
int sw_nonlinear_orpa_solve(sw_nonlinear_orpa_t *m, double eps_kT,
                             int n_continuation_steps, double eps_kT_start);

/* Structure factor at reduced wavevector k*=Q*sigma, using the solved u. */
double sw_nonlinear_orpa_S(const sw_nonlinear_orpa_t *m, double k);

#endif
