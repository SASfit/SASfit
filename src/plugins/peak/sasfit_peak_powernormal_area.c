/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */


#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define AREA	param->p[0]
#define X0	    param->p[1]
#define SIGMA	param->p[2]
#define DUMMY   param->p[3]
#define P	    param->p[4]
#define BACKGR	param->p[5]

scalar sasfit_peak_powernormal_area(scalar x, sasfit_param * param)
{
	scalar z;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((SIGMA == 0.0), param, "sigma(%lg) = 0",SIGMA); // modify condition to your needs
	SASFIT_CHECK_COND1((P <= 0.0), param, "p(%lg) < 0",P); // modify condition to your needs

	// insert your code here

	z = (x-X0)/fabs(SIGMA);
	return AREA*P/fabs(SIGMA)
			* gsl_ran_gaussian_pdf(z,1.0) 
			* pow(gsl_cdf_gaussian_P(-z,1.0),P-1)+BACKGR;
}

scalar sasfit_peak_powernormal_area_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_peak_powernormal_area_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}