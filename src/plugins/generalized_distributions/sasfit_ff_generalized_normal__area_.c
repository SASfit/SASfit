/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define ALPHA	fabs(param->p[1])
#define KAPPA	param->p[2]
#define XI	param->p[3]
#define DUMMY	param->p[4]
#define Y0	param->p[5]

scalar sasfit_ff_generalized_normal__area_(scalar x, sasfit_param * param)
{
    // https://en.wikipedia.org/wiki/Generalized_normal_distribution
    scalar y,z;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((ALPHA == 0.0), param, "alpha(%lg) == 0",ALPHA); // modify condition to your needs

	// insert your code here
	y=Y0;
	if (KAPPA>0 && x>=XI+ALPHA/KAPPA) return y;
	if (KAPPA<0 && x<=XI+ALPHA/KAPPA) return y;
	if (KAPPA==0) {
        z=(x-XI)/ALPHA;
	} else {
	    z=-1./KAPPA*log(1-KAPPA*(x-XI)/ALPHA);
	}
	y = y+A*exp(-z*z/2)/sqrt(2*M_PI)/(ALPHA-KAPPA*(x-XI));
	return y;
}

scalar sasfit_ff_generalized_normal__area__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_generalized_normal__area__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

