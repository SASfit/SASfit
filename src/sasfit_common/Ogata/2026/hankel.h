#ifndef HANKEL_H
#define HANKEL_H

#include <stddef.h>
#include <stdint.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* User-supplied radial function f(r, user_data) */
typedef double (*hankel_func_t)(double r, void *user_data);

/* Ogata table structure */
typedef struct {
    double h;
    int    Nmax;
    double *t;
    double *x;
    double *w;
} hankel_ogata_table_t;

/* ---------------- Ogata table management ---------------- */
hankel_ogata_table_t *hankel_ogata_table_create(double h, int Nmax);
void hankel_ogata_table_destroy(hankel_ogata_table_t *tab);

/* ---------------- Ogata DE Hankel transform [0,∞) ---------------- */
double hankel_ogata_nu(
    hankel_func_t f,
    void *user_data,
    double nu,
    double k,
    double h,
    int N_start,
    int N_step,
    int Nmax,
    double tol,
    const hankel_ogata_table_t *tab /* optional */
);

/* Vectorized version */
void hankel_ogata_vector(
    hankel_func_t f,
    void *user_data,
    double nu,
    const double *k_array,
    int nk,
    double h,
    int N_start,
    int N_step,
    int Nmax,
    double tol,
    const hankel_ogata_table_t *tab,
    double *H_out
);

/* ---------------- Finite-interval Hankel transform [0,R] ---------------- */
double hankel_finite_GL(
    hankel_func_t f,
    void *user_data,
    double nu,
    double k,
    double R,
    int n_gl
);

/* Adaptive finite GL: start at n_gl_start and increase until relative change < tol */
double hankel_finite_GL_adaptive(
    hankel_func_t f,
    void *user_data,
    double nu,
    double k,
    double R,
    int n_gl_start,
    double tol
);

/* Composite Filon finite-interval Hankel: fits quadratics to g(r)=r*f(r) on panels
   and evaluates moments of t^q J_n(t) via numerical integration. If npanels<=0
   an internal panel-sizing strategy is used. */
double hankel_finite_filon(
    hankel_func_t f,
    void *user_data,
    double nu,
    double k,
    double R,
    int npanels,
    double tol
);

void hankel_finite_GL_vector(
    hankel_func_t f,
    void *user_data,
    double nu,
    const double *k_array,
    int nk,
    double R,
    int n_gl,
    double *H_out
);

void hankel_finite_GL_adaptive_vector(
    hankel_func_t f,
    void *user_data,
    double nu,
    const double *k_array,
    int nk,
    double R,
    int n_gl_start,
    double tol,
    double *H_out
);

/* Ogata with simple Richardson extrapolation (assumes error ~ h^2) */
double hankel_ogata_nu_extrapolate(
    hankel_func_t f,
    void *user_data,
    double nu,
    double k,
    double h,
    int N_start,
    int N_step,
    int Nmax,
    double tol,
    const hankel_ogata_table_t *tab
);

#endif /* HANKEL_H */
