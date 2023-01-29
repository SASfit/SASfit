#include <gsl/gsl_qrng.h>
#include "quasimontecarlo.h"

double func1(double* x, size_t dim, void* params) {
    return 1 / (x[0] + x[1]);
}

int main(int argc, char** argv) {
    const size_t dim = 2;
    double xl[] = {1.5, 0.5};
    double xu[] = {4.5, 6};
    double result, error;
    
    gsl_monte_function f;
    f.f = func1;
    f.dim = dim;
    f.params = NULL;
    
    gsl_qrng* qrng = gsl_qrng_alloc(gsl_qrng_halton, dim);
    
    quasi_monte_state* s = quasi_monte_alloc(dim);
    quasi_monte_integrate(&f, xl, xu, dim, 1000000, 1e-5, 0, qrng, s, &result, &error);
    quasi_monte_free(s);
    
    gsl_qrng_free(qrng);
    
    printf("%f ± %f\n", result, error);
    return 0;
}
