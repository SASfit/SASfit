/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define R0	param->p[0]
#define XI	param->p[1]
#define D	param->p[2]

scalar sasfit_sq_mass_fractal__gaussian_cut_off_(scalar q, sasfit_param * param)
{
    scalar P16;
	int status;

	gsl_sf_result pFq_1F1;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R0 <= 0.0), param, "r0(%lg) <= 0",R0); // modify condition to your needs
	SASFIT_CHECK_COND2((XI < R0), param, "xi(%lg) < r0(%lg)",XI,R0); // modify condition to your needs
	SASFIT_CHECK_COND1((D <= 1.0), param, "D(%lg) <= 1",D); // modify condition to your needs

	// insert your code here

	gsl_set_error_handler_off();

	if ((XI == 0) || (R0 == 0))
	{
		return 1.0;
	}
	P16 = gsl_sf_gamma(D/2.)*D/2.;
	P16 = P16*pow(XI/R0,D);

	status = gsl_sf_hyperg_1F1_e(D/2.,1.5,-0.25*pow(q*XI,2.),&pFq_1F1);

	if (status && (q*XI >= 10))
	{
		pFq_1F1.val = (sqrt(M_PI)*(pow(2.,D)/(pow(q,D)*pow(pow(XI,2),D/2.)*gsl_sf_gamma(1.5 - D/2.)) +
						(pow(4,1.5 - D/2.)*pow(q,-3 + D)*pow(-pow(XI,2),-1.5 + D/2.))/
						(exp((pow(q,2)*pow(XI,2))/4.)*gsl_sf_gamma(D/2.))))/2. ;
// gsl_sf_gamma(1.5)/gsl_sf_gamma(1.5-D/2.0)*pow(0.25*pow(q*xi,2.),D/2.);
	}
	else if (status && (q*XI < 10))
	{
		sasfit_param_set_err(param, DBGINFO("%s,q=%lf"), gsl_strerror(status), q);
		return SASFIT_RETURNVAL_ON_ERROR;
	} else {
		return 1.0+P16*pFq_1F1.val;
	}
	return P16;
}

scalar sasfit_sq_mass_fractal__gaussian_cut_off__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_mass_fractal__gaussian_cut_off__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

