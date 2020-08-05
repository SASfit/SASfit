/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_peaks.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define DUMMY	param->p[1]
#define EPSILON	param->p[2]
#define P	param->p[3]
#define DUMMY	param->p[4]
#define ETA_C	param->p[5]
#define ETA_SOLV	param->p[6]

scalar sasfit_peaks_superegg_random(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((DUMMY < 0.0), param, "dummy(%lg) < 0",DUMMY); // modify condition to your needs
	SASFIT_CHECK_COND1((EPSILON < 0.0), param, "epsilon(%lg) < 0",EPSILON); // modify condition to your needs
	SASFIT_CHECK_COND1((P < 0.0), param, "p(%lg) < 0",P); // modify condition to your needs
	SASFIT_CHECK_COND1((DUMMY < 0.0), param, "dummy(%lg) < 0",DUMMY); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA_C < 0.0), param, "eta_c(%lg) < 0",ETA_C); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA_SOLV < 0.0), param, "eta_solv(%lg) < 0",ETA_SOLV); // modify condition to your needs

	// insert your code here
	return 0.0;
}

scalar sasfit_peaks_superegg_random_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_peaks_superegg_random_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

