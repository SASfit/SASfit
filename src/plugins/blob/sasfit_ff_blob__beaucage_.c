/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>
#include <math.h>

// define shortcuts for local parameters/variables
#define RG	param->p[0]
#define P	param->p[1]
#define DUMMY	param->p[2]
#define ALPHA	param->p[3]

scalar sasfit_ff_blob__beaucage_(scalar q, sasfit_param * param)
{
    scalar blob;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RG <= 0.0), param, "Rg(%lg) <= 0",RG); // modify condition to your needs

	// insert your code here
	if (q==0) {
        return 0;
	}
    blob = ALPHA*pow(gsl_pow_3(gsl_sf_erf(q*RG/sqrt(6)))/q,P);
	return blob;
}

scalar sasfit_ff_blob__beaucage__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_blob__beaucage__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

