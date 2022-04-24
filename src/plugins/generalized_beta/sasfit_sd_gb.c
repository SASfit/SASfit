/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables
#define N	param->p[0]
#define A	param->p[1]
#define B	fabs(param->p[2])
#define C	(1.0/(1+exp(-param->p[3])))
#define P	fabs(param->p[4])
#define Q	fabs(param->p[5])


scalar sasfit_sd_gb(scalar x, sasfit_param * param)
{
    /*
     *   THIS IS THE PROBABILITY DISTRIBUTION FUNCTION
     */
    gsl_sf_result Bpq;
    int ierr;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((x < 0.0), param, "x(%lg) < 0",x);
	SASFIT_CHECK_COND1((A == 0.0), param, "a(%lg) == 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B == 0.0), param, "b(%lg) == 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((P == 0.0), param, "p(%lg) == 0",P); // modify condition to your needs
	SASFIT_CHECK_COND1((Q == 0.0), param, "q(%lg) == 0",Q); // modify condition to your needs

	// insert your code here
	if (x<=0) return 0.0;
	if (pow(x,A)>=pow(B,A)/(1-C)) return 0.0;
	gsl_set_error_handler_off();
	ierr = gsl_sf_beta_e(P,Q,&Bpq);
	if (ierr) {
        sasfit_err("%s B(%lf,%lf)+-%lf\n", gsl_strerror (ierr),P,Q,Bpq.err);
        return 0;
	}
	return N*fabs(A)*pow(x,A*P-1)*pow(1-(1-C)*pow(x/B,A),Q-1)/(pow(B,A*P)*Bpq.val*pow(1+C*pow(x/B,A),P+Q));
}

scalar sasfit_sd_gb_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sd_gb_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

