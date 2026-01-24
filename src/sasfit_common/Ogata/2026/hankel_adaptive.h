#ifndef HANKEL_ADAPTIVE_H
#define HANKEL_ADAPTIVE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef double (*hankel_func_t)(double r, void *user_data);

/* Scalar adaptive Hankel transform for integer order n */
double hankel_adaptive_scalar(
    hankel_func_t f,
    void *user_data,
    int n,
    double k,
    double tol
);

/* Debug variant: returns core and tail contributions and chosen h */
double hankel_adaptive_debug(
    hankel_func_t f,
    void *user_data,
    int n,
    double k,
    double tol,
    double *core_out,
    double *tail_out,
    double *h_used_out
);

/* Vectorized adaptive Hankel transform (independent per k, no reuse) */
void hankel_adaptive_vector(
    hankel_func_t f,
    void *user_data,
    int n,
    const double *k_array,
    int nk,
    double tol,
    double *H_out
);

/* Vectorized adaptive Hankel with Ogata table reuse across k.
   This is the one you want to use for performance. */
void hankel_adaptive_vector_reuse(
    hankel_func_t f,
    void *user_data,
    int n,
    const double *k_array,
    int nk,
    double tol,
    double *H_out
);

#ifdef __cplusplus
}
#endif

#endif /* HANKEL_ADAPTIVE_H */
