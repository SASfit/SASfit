/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar sasfit_sd_metalog_bl_bu(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((x < 0.0), param, "x(%lg) < 0",x);
	SASFIT_CHECK_COND1((N < 0.0), param, "N(%lg) < 0",N); // modify condition to your needs
	SASFIT_CHECK_COND1((BL < 0.0), param, "bl(%lg) < 0",BL); // modify condition to your needs
	SASFIT_CHECK_COND1((BU < 0.0), param, "bu(%lg) < 0",BU); // modify condition to your needs
	SASFIT_CHECK_COND2((BL == BU), param, "bl(%lg) == bu(%lf)",BL, BU); // modify condition to your needs
	// insert your code here

	if (x<=BL||x>=BU) return 0;
	if (gsl_finite(pow(x,-ALPHA))) {
        return N*metalogLogitPDF(x, param)*pow(x,-ALPHA);
    } else {
        return 0;
    }
}

scalar sasfit_sd_metalog_bl_bu_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sd_metalog_bl_bu_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

