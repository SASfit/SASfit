/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define SIGMA	fabs(param->p[1])
#define XI	param->p[2]
#define DUMMY	param->p[3]
#define DUMMY2	param->p[4]
#define Y0	param->p[5]

scalar sasfit_ff_exponentiated_generalized_pareto__area_(scalar x, sasfit_param * param)
// https://en.wikipedia.org/wiki/Generalized_Pareto_distribution#Exponentiated_generalized_Pareto_distribution
{
    scalar y;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((SIGMA == 0.0), param, "sigma(%lg) == 0",SIGMA); // modify condition to your needs

	// insert your code here
	if (XI<0 && x>log(-SIGMA/XI)) return Y0;
	if (XI==0) return Y0+A*exp(x-exp(x/SIGMA))/SIGMA;
	y = Y0+A*exp(x)/SIGMA*pow(1+XI/SIGMA*exp(x),-1/XI-1);
	if (gsl_isinf(y)) y = Y0+A*exp(-x/XI)/SIGMA*pow(XI/SIGMA,-1./XI-1.);
	return y;
}

scalar sasfit_ff_exponentiated_generalized_pareto__area__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_exponentiated_generalized_pareto__area__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

