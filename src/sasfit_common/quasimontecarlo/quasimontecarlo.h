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

#ifndef _QUASIMONTECARLO_H_INCLUDE
#define _QUASIMONTECARLO_H_INCLUDE

#include <gsl/gsl_monte.h>
#include <gsl/gsl_qrng.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  size_t dim;
  double *x;
} quasi_monte_state;

quasi_monte_state* quasi_monte_alloc(size_t dim);
int quasi_monte_init(quasi_monte_state* s);
int quasi_monte_integrate(gsl_monte_function* f, const double xl[], const double xu[], size_t dim, size_t max_calls, double max_relerr, double max_abserr, gsl_qrng* r, quasi_monte_state* state, double* result, double* abserr);
void quasi_monte_free(quasi_monte_state* s);

#ifdef __cplusplus
}
#endif

#endif
