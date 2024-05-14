/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar sasfit_sd_metalog_logit(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND2((BL == BU), param, "bl(%lg) == bu(%lf)",BL, BU); // modify condition to your needs
	// insert your code here

	if (x<=BL||x>=BU) return 0;
	if (gsl_finite(pow(x,-ALPHA))) {
        return N*metalogLogitPDF(x, param)*pow(x,-ALPHA);
    } else {
        return 0;
    }
}

scalar sasfit_sd_metalog_logit_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sd_metalog_logit_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

