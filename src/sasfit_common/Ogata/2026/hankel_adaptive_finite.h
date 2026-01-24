#ifndef HANKEL_ADAPTIVE_FINITE_H
#define HANKEL_ADAPTIVE_FINITE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef double (*hankel_func_t)(double r, void *user_data);

/* Scalar finite-range hybrid Hankel transform on [0, R] */
double hankel_adaptive_finite(
    hankel_func_t f,
    void *user_data,
    int n,
    double k,
    double R,
    double tol
);

/* Vectorized finite-range hybrid Hankel transform on [0, R] */
void hankel_adaptive_finite_vector(
    hankel_func_t f,
    void *user_data,
    int n,
    const double *k_array,
    int nk,
    double R,
    double tol,
    double *H_out
);

#ifdef __cplusplus
}
#endif

#endif /* HANKEL_ADAPTIVE_FINITE_H */
