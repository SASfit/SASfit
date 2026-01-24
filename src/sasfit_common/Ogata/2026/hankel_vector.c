#include "hankel.h"

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
) {
    /* #pragma omp parallel for */
    for (int i = 0; i < nk; ++i) {
        H_out[i] = hankel_ogata_nu(
            f, user_data, nu, k_array[i], h,
            N_start, N_step, Nmax, tol, tab
        );
    }
}
