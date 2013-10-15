/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]

scalar sasfit_sq_sq_oz_1(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs

	// insert your code here
	if (sq1akima_T=NULL) {
        return 1.0;
	} else {
        return gsl_spline_eval(sq1akima_T,q*R,acc_spline1);
	}
}

scalar sasfit_sq_sq_oz_1_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_sq_oz_1_v(scalar q, sasfit_param * param, int dist)
{
    int i;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
	if (sq1akima_T==NULL) {
        sq1akima_T = gsl_spline_alloc(gsl_interp_akima,dist);
        gsl_spline_init(sq1akima_T, param->xarr, param->yarr, dist);
	} else {
	    gsl_spline_free(sq1akima_T);
        sq1akima_T = gsl_spline_alloc(gsl_interp_akima,dist);
        gsl_spline_init(sq1akima_T, param->xarr, param->yarr, dist);
	}
	// insert your code here
	return 0.0;
}

