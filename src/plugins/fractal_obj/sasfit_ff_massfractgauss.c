/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define RG	param->p[0]
#define D	param->p[1]
#define I0	param->p[2]

scalar sasfit_ff_massfractgauss(scalar q, sasfit_param * param)
{
    scalar xi, tmp;
	gsl_sf_result pFq_1F1;
	int status;

	SASFIT_ASSERT_PTR(param);

	gsl_set_error_handler_off();

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((D <= 1.0), param, "D(%lg) <= 1",D);

	if (RG == 0) return I0;

	xi = sqrt(4.0*RG*RG/D);
	status = gsl_sf_hyperg_1F1_e((3-D)/2.0, 1.5, pow(q*RG,2.0)/D, &pFq_1F1);
	tmp = status;
	if (status && (q*xi >= 10))
	{
		return I0*pow(D,D/2.0)*gsl_sf_gamma(1.5)/gsl_sf_gamma((3.0-D)/2.0)
			*pow(q*RG,-D);
	}
	else if (status && (q*xi < 10))
	{
		sasfit_param_set_err(param, DBGINFO("%s,q=%lf\n%s"), gsl_strerror(status), q,"could not calc MassFractGauss");
		return SASFIT_RETURNVAL_ON_ERROR;
	} else {
		return I0 * exp(-q*q*RG*RG/D)*pFq_1F1.val;
	}

	SASFIT_CHECK_COND1((status==1), param, "could not calc MassFractGauss",tmp);
	return I0 * exp(-q*q*RG*RG/D) * pFq_1F1.val;
}

scalar sasfit_ff_massfractgauss_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_massfractgauss_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

