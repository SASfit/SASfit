/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define B	param->p[1]
#define C	param->p[2]
#define Q	param->p[3]
#define P	param->p[4]
#define ETA_C	param->p[5]
#define ETA_SOLV	param->p[6]

scalar sasfit_ff_superellipsoid_opo(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B < 0.0), param, "b(%lg) < 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C < 0.0), param, "c(%lg) < 0",C); // modify condition to your needs
	SASFIT_CHECK_COND1((Q < 0.0), param, "q(%lg) < 0",Q); // modify condition to your needs
	SASFIT_CHECK_COND1((P < 0.0), param, "p(%lg) < 0",P); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA_C < 0.0), param, "eta_c(%lg) < 0",ETA_C); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA_SOLV < 0.0), param, "eta_solv(%lg) < 0",ETA_SOLV); // modify condition to your needs

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_superellipsoid_opo_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_superellipsoid_opo_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

