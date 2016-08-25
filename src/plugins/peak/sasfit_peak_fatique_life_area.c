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
#define X0	param->p[1]
#define DUMMY	param->p[2]
#define BETA	param->p[3]
#define GAMMA	param->p[4]
#define BACKGR	param->p[5]

scalar sasfit_peak_fatique_life_area(scalar x, sasfit_param * param) {
	scalar z;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((BETA <= 0.0), param, "beta(%lg) < 0",BETA); // modify condition to your needs
	SASFIT_CHECK_COND1((GAMMA <= 0.0), param, "gamma(%lg) < 0",GAMMA); // modify condition to your needs

	// insert your code here
	z = (x-X0)/BETA;
	if (z<=0) return BACKGR;

	return AREA/BETA
			* (sqrt(z)+sqrt(1/z))/(2*GAMMA*z)
			* gsl_ran_gaussian_pdf((sqrt(z)+sqrt(1/z))/GAMMA,1)+BACKGR;
}


scalar sasfit_peak_fatique_life_area_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_peak_fatique_life_area_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}