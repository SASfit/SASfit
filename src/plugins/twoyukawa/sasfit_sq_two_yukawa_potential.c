/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define K1	param->p[1]
#define LAMBDA1	param->p[2]
#define K2	param->p[3]
#define LAMBDA2	param->p[4]
#define PHI	param->p[5]
#define MOLARITY	param->p[6]
#define Z1  param->p[MAXPAR-1]
#define Z2  param->p[MAXPAR-2]

scalar sasfit_sq_two_yukawa_potential(scalar r, sasfit_param * param)
{

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((LAMBDA1 <= 0.0), param, "lambda1(%lg) < 0",LAMBDA1); // modify condition to your needs
	SASFIT_CHECK_COND1((LAMBDA2 <= 0.0), param, "lambda2(%lg) < 0",LAMBDA2); // modify condition to your needs
	// insert your code here
	Z1 = 1./LAMBDA1;
	Z2 = 1./LAMBDA2;
    if(fabs(Z1) < 0.001) Z1 = 0.001;			// values near zero are very bad for the calculation
	if(fabs(Z2) < 0.001) Z2 = 0.001;			// values near zero are very bad for the calculation
	if(fabs(K1) < 0.001) K1 = 0.001;			// values near zero are very bad for the calculation
	if(fabs(K2) < 0.001) K2 = 0.001;
	if (fabs(r)<2*R) return 1e42;
	return -K1*exp(-Z1*(r/(2*R)-1))-K2*exp(-Z2*(r/(2*R)-1));
}

scalar sasfit_sq_two_yukawa_potential_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_two_yukawa_potential_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

