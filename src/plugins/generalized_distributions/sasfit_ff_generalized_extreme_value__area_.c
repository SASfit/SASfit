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
#define MU	param->p[3]
#define DUMMY	param->p[4]
#define Y0	param->p[5]

scalar tx(scalar x, sasfit_param * param) {
    scalar z;
    z=(x-MU)/SIGMA;
    if (XI==0) return exp(-z);
    return pow(1+XI*z,-1/XI);
}

scalar sasfit_ff_generalized_extreme_value__area_(scalar x, sasfit_param * param)
// https://en.wikipedia.org/wiki/Generalized_extreme_value_distribution
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((SIGMA == 0.0), param, "sigma(%lg) == 0",SIGMA); // modify condition to your needs

	// insert your code here
	if (XI>0 && x<MU-SIGMA/XI) return Y0;
	if (XI<0 && x>MU-SIGMA/XI) return Y0;
	if (XI==0) return Y0+A/SIGMA*tx(x,param)*exp(-tx(x,param));
	return Y0+A/SIGMA*pow(tx(x,param),XI+1)*exp(-tx(x,param));
}

scalar sasfit_ff_generalized_extreme_value__area__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_generalized_extreme_value__area__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

