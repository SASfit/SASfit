/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables
#define N	param->p[0]
#define L	GSL_MIN(param->p[1],param->p[2])
#define U	GSL_MAX(param->p[1],param->p[2])
#define ALPHA	param->p[3]
#define XI	param->p[4]
#define LAMBDA	fabs(param->p[5])
#define DELTA	1
#define GAMMA	param->p[7]

scalar mean_sd_johnson_sn(scalar x, sasfit_param * param) {
    return (-(GAMMA*LAMBDA) + DELTA*XI)/DELTA;
}

scalar median_sd_johnson_sn(scalar x, sasfit_param * param) {
    return -((GAMMA*LAMBDA)/DELTA) + XI;
}

scalar variance_sd_johnson_sn(scalar x, sasfit_param * param) {
    return gsl_pow_int(LAMBDA,2)/gsl_pow_int(DELTA,2);
}

scalar kurtosis_sd_johnson_sn(scalar x, sasfit_param * param) {
    return 3;
}

scalar skewness_sd_johnson_sn(scalar x, sasfit_param * param) {
    return 0;
}

scalar sasfit_sd_johnson_sn(scalar x, sasfit_param * param)
{
    /*
     *   THIS IS THE PROBABILITY DISTRIBUTION FUNCTION
     */
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((L < 0.0), param, "l(%lg) < 0",L); // modify condition to your needs
	SASFIT_CHECK_COND1((U < 0.0), param, "u(%lg) < 0",U); // modify condition to your needs
	SASFIT_CHECK_COND1((LAMBDA == 0.0), param, "lambda(%lg) == 0",LAMBDA); // modify condition to your needs
	SASFIT_CHECK_COND1((DELTA == 0.0), param, "delta(%lg) == 0",DELTA); // modify condition to your needs

	// insert your code here
	if (x==0 && ALPHA > 0) return 0;
	return N*exp(-0.5*gsl_pow_2(GAMMA+DELTA*(x-XI)/(LAMBDA)))*DELTA/(LAMBDA*sqrt(2*M_PI))*pow(x,-ALPHA);
}

scalar sasfit_sd_johnson_sn_f(scalar x, sasfit_param * param)
{
    /*
     *   THIS IS THE CUMMULITATIVE DISTRIBUTION FUNCTION
     */
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return N*0.5*gsl_sf_erfc(-(GAMMA+DELTA*(x-XI)/(LAMBDA))/sqrt(2));
}

scalar sasfit_sd_johnson_sn_v(scalar p, sasfit_param * param, int dist)
{
    /*
     *   THIS IS THE QUANTILE DISTRIBUTION FUNCTION
     */
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return (DELTA*XI-LAMBDA*GAMMA-sqrt(2)*LAMBDA*sasfit_erfinv(1-2*p))/DELTA;
}

