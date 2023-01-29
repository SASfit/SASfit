/*
 * Copyright 2012 David Zaslavsky <diazona@ellipsix.net>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gsl/gsl_math.h>
#include <gsl/gsl_errno.h>
#include "quasimontecarlo.h"

quasi_monte_state* quasi_monte_alloc(size_t dim) {
    quasi_monte_state* s = (quasi_monte_state*)malloc(sizeof(quasi_monte_state));
    if (s == NULL) {
        GSL_ERROR_VAL("failed to allocate space for quasi_monte_state", GSL_ENOMEM, 0);
    }
    s->x = (double*)malloc(dim * sizeof(double));
    if (s->x == NULL) {
        free(s);
        GSL_ERROR_VAL("failed to allocate space for working vector", GSL_ENOMEM, 0);
    }
    s->dim = dim;
    return s;
}

int quasi_monte_init(quasi_monte_state* s) {
    size_t i;
    for (i = 0; i < s->dim; i++) {
        s->x[i] = 0;
    }
    return GSL_SUCCESS;
}

int quasi_monte_integrate(gsl_monte_function* f, const double xl[], const double xu[], size_t dim, size_t max_calls, double max_relerr, double max_abserr, gsl_qrng* r, quasi_monte_state* state, double* result, double* abserr) {
    double volume = 1, mean = 0, variance_sum = 0, running_avg_abserr = GSL_POSINF;
    double* x = state->x;
    size_t n, i;
    // Check that the dimensionalities match
    if (dim != state->dim) {
        char errmsg[80];
        snprintf(errmsg, 80, "number of dimensions %ud doesn't match allocated size %ud", (unsigned int)dim, (unsigned int)(state->dim));
        GSL_ERROR(errmsg, GSL_EINVAL);
    }
    // Check the bounds for validity
    for (i = 0; i < dim; i++) {
        if (xu[i] <= xl[i]) {
            char errmsg[80];
            snprintf(errmsg, 80, "lower limit %f not less than upper limit %f", xl[i], xu[i]);
            GSL_ERROR(errmsg, GSL_EINVAL);
        }
        if (xu[i] - xl[i] > GSL_DBL_MAX) {
            char errmsg[80];
            snprintf(errmsg, 80, "integration range (%f,%f) is larger than limit %f", xl[i], xu[i], GSL_DBL_MAX);
            GSL_ERROR(errmsg, GSL_EINVAL);
        }
    }
    // Compute the volume of the region
    for (i = 0; i < dim; i++) {
        volume *= xu[i] - xl[i];
    }
    for (n = 0; n < max_calls; n++) {
        // Choose a quasirandom point in the integration region
        gsl_qrng_get(r, x);
        for (i = 0; i < dim; i++) {
            x[i] = xl[i] + (xu[i] - xl[i]) * x[i];
        }
        {
            double fval = GSL_MONTE_FN_EVAL(f, x);
            double d = fval - mean;
            mean += d / (n + 1.0);
            variance_sum += d * d * (n / (n + 1.0));
        }
        if (n > 1) {
            running_avg_abserr = sqrt(variance_sum / ((n + 1.0) * n));
            if (volume * running_avg_abserr < max_abserr) {
                break;
            }
            if (fabs(running_avg_abserr / mean) < max_relerr) {
                break;
            }
        }
    }
    *result = volume * mean;
    *abserr = volume * running_avg_abserr;
    return GSL_SUCCESS;
}

void quasi_monte_free(quasi_monte_state* s) {
    if (s == NULL) {
        return;
    }
    free(s->x);
    free(s);
}
