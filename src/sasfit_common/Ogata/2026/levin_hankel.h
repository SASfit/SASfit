#ifndef LEVIN_HANKEL_H
#define LEVIN_HANKEL_H

#include <complex.h>

typedef struct {
    double a, b;   /* integration limits */
    double k;      /* frequency */
    int    N;      /* collocation points / polynomial degree N-1 */
} levin_params_t;

/* F(r) in ∫ F(r) e^{i k r} dr */
typedef double complex (*levin_F_t)(double r, void *user);

/* Levin integrator for ∫_a^b F(r) e^{i k r} dr */
double complex levin_integrate_exp_ikr(
    levin_F_t F,
    void *user_data,
    const levin_params_t *params
);

/* Hankel tail: integer order n, from R0 to R1 */
typedef double (*hankel_f_t)(double r, void *user);

double hankel_tail_levin_int_order(
    hankel_f_t f,
    void *user_data,
    int n,
    double k,
    double R0,
    double R1,
    int N
);

#endif
