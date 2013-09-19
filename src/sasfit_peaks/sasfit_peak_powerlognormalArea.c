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
#define SIGMA	param->p[2]
#define SHAPE	param->p[3]
#define P	param->p[4]
#define BACKGR	param->p[5]

scalar sasfit_peak_powerlognormalArea(scalar x, sasfit_param * param)
{
	scalar z;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((SIGMA == 0.0), param, "sigma(%lg) = 0",SIGMA); // modify condition to your needs
	SASFIT_CHECK_COND1((SHAPE == 0.0), param, "shape(%lg) = 0",SHAPE); // modify condition to your needs
	SASFIT_CHECK_COND1((P < 0.0), param, "p(%lg) < 0",P); // modify condition to your needs

	// insert your code here

	z = (x-X0)/fabs(SIGMA);
	if (z<=0.0) return BACKGR;
	return AREA*P/(z*fabs(SIGMA*SHAPE))
			* gsl_ran_gaussian_pdf(log(z)/SHAPE,1.0) 
			* pow(gsl_cdf_gaussian_P(-log(z)/SHAPE,1.0),P-1)+BACKGR;
}

