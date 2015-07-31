/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
#include "include/sasfit_peaks_utils.h"

// define shortcuts for local parameters/variables
#define AREA	param->p[0]
#define X0	param->p[1]
#define BETA	param->p[2]
#define GAMMA	param->p[3]
#define BACKGR	param->p[4]

scalar sasfit_peak_fatique_lifeArea(scalar x, sasfit_param * param)
{
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
