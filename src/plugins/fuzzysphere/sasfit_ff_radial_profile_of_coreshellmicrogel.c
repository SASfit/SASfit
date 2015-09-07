/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define W_CORE		param->p[0]
#define SIGMA_CORE	param->p[1]
#define W_SH		param->p[2]
#define SIGMA_SHIN	param->p[3]
#define D			param->p[4]
#define SIGMA_OUT	param->p[5]
#define ETA_CORE	param->p[6]
#define ETA_SHELL	param->p[7]
#define ETA_SOL		param->p[8]


#define R_CORE		W_CORE+SIGMA_CORE
#define R_OUT		R_CORE+D+SIGMA_SHIN+W_SH+SIGMA_OUT
#define R_SHIN		R_CORE+D

scalar prof(scalar x,scalar ir,scalar isigma) {
	scalar r,sigma;
	r = fabs(ir);
	sigma=fabs(isigma);
	if (fabs(x)<=(r-sigma)) {
		return 1;
	} else if ((r-sigma)<fabs(x) && fabs(x)<= r) {
		return (1-0.5*gsl_pow_2(((fabs(x)-r)+sigma)/sigma));
	} else if (r<fabs(x) && fabs(x)<= (r+sigma)) {
		return (0.5*gsl_pow_2(((r-fabs(x))+sigma)/sigma));
	} else {
		return 0;
	}
}
scalar sasfit_ff_radial_profile_of_coreshellmicrogel(scalar r, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return ETA_SOL 
		+(ETA_SHELL-ETA_SOL) *prof(r,R_OUT,SIGMA_OUT)
		-(ETA_SHELL-ETA_SOL) *prof(r,R_SHIN,SIGMA_SHIN)
		+(ETA_CORE -ETA_SOL) *prof(r,R_CORE,SIGMA_CORE);
}

scalar sasfit_ff_radial_profile_of_coreshellmicrogel_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_radial_profile_of_coreshellmicrogel_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

