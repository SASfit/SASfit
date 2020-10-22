/*
 * Author(s) of this file:
 *   <your name> (<email address>)
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

scalar sasfit_sq___yukawa(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((K1 < 0.0), param, "K1(%lg) < 0",K1); // modify condition to your needs
	SASFIT_CHECK_COND1((LAMBDA1 < 0.0), param, "lambda1(%lg) < 0",LAMBDA1); // modify condition to your needs
	SASFIT_CHECK_COND1((K2 < 0.0), param, "K2(%lg) < 0",K2); // modify condition to your needs
	SASFIT_CHECK_COND1((LAMBDA2 < 0.0), param, "lambda2(%lg) < 0",LAMBDA2); // modify condition to your needs
	SASFIT_CHECK_COND1((PHI < 0.0), param, "phi(%lg) < 0",PHI); // modify condition to your needs
	SASFIT_CHECK_COND1((MOLARITY < 0.0), param, "molarity(%lg) < 0",MOLARITY); // modify condition to your needs

	// insert your code here
	return 0.0;
}

scalar sasfit_sq___yukawa_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq___yukawa_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

