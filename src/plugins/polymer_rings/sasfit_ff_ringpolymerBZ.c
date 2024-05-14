/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define RG	param->p[0]

#define NU	param->p[2]
#define I0	param->p[3]
#define Q   param->p[MAXPAR-1]
#define KEPS    param->p[MAXPAR-2]
#define EPS param->p[MAXPAR-3]
#define MU  param->p[MAXPAR-4]

scalar Kernel_P_BZ(scalar x, sasfit_param * param) {
	return 2*(1-x)*exp(-MU*pow(x*(1-x),1+EPS)/ (pow(1-x,1+EPS)+pow(x,1+EPS)) );
}

scalar Kernel_KEPS_BZ(scalar x, sasfit_param * param) {
	return pow(x*(1-x),1+EPS)*(1-x)/ (pow(1-x,1+EPS)+pow(x,1+EPS));
}

scalar sasfit_ff_ringpolymerBZ(scalar q, sasfit_param * param)
{
    scalar sum;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((NU <= 0.0), param, "NU(%lg) <= 0",NU);
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs

	// insert your code here
    EPS = 2*NU-1;
    Q = q;

    KEPS = sasfit_integrate(0,1,&Kernel_KEPS_BZ,param);
    MU = gsl_pow_2(q*RG)/(6*KEPS);
    sum = sasfit_integrate(0,1,&Kernel_P_BZ,param);
    return sum*I0;
}

scalar sasfit_ff_ringpolymerBZ_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_ringpolymerBZ_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

