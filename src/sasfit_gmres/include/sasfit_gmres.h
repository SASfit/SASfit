/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */


#include <stdio.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>

#include "../sasfit_old/include/sasfit.h"

typedef struct {
// input values
    int maxit;
    int lmaxit;
    int lmeth;
    int restart_limit;
    double etamax;
// output values
    int ierr;
    char errstr[256];
    int n;
} nsolip;

scalar parab3p(scalar, scalar, scalar, scalar, scalar);

