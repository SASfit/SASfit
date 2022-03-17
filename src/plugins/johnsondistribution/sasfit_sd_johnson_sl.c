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
#define XI	param->p[3]
#define LAMBDA	fabs(param->p[4])
#define DELTA	param->p[5]
#define GAMMA	param->p[6]

scalar mean_sd_johnson_sl(scalar x, sasfit_param * param) {
    return exp((1 - 2*DELTA*GAMMA)/(2.*gsl_pow_int(DELTA,2)))*LAMBDA + XI;
}

scalar median_sd_johnson_sl(scalar x, sasfit_param * param) {
    return LAMBDA/exp(GAMMA/DELTA) + XI;
}

scalar variance_sd_johnson_sl(scalar x, sasfit_param * param) {
    return exp((1 - 2*DELTA*GAMMA)/gsl_pow_int(DELTA,2))*(-1 + exp(1/gsl_pow_int(DELTA,2)))*
   gsl_pow_int(LAMBDA,2);
}

scalar kurtosis_sd_johnson_sl(scalar x, sasfit_param * param) {
    return -3 + exp(2/gsl_pow_int(DELTA,2))*(3 +
      exp(1/gsl_pow_int(DELTA,2))*(2 + exp(1/gsl_pow_int(DELTA,2))));
}

scalar skewness_sd_johnson_sl(scalar x, sasfit_param * param) {
    return sqrt(-1 + exp(1/gsl_pow_int(DELTA,2)))*(2 + exp(1/gsl_pow_int(DELTA,2)));
}


scalar sasfit_sd_johnson_sl(scalar x, sasfit_param * param)
{
    /*
     *   THIS IS THE PROBABILITY DISTRIBUTION FUNCTION
     */
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((x < 0.0), param, "x(%lg) < 0",x);
	SASFIT_CHECK_COND1((N < 0.0), param, "N(%lg) < 0",N); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "l(%lg) < 0",L); // modify condition to your needs
	SASFIT_CHECK_COND1((U < 0.0), param, "u(%lg) < 0",U); // modify condition to your needs
	SASFIT_CHECK_COND1((LAMBDA == 0.0), param, "lambda(%lg) == 0",LAMBDA); // modify condition to your needs

	// insert your code here

	if (x<=L || x>=U) return 0;
	if (x<=XI) return 0.0;
	return N*exp(-0.5*gsl_pow_2(GAMMA+log((x-XI)/LAMBDA)/DELTA))/(DELTA*sqrt(2*M_PI)*(x-XI));
}

scalar sasfit_sd_johnson_sl_f(scalar x, sasfit_param * param)
{
    /*
     *   THIS IS THE CUMMULITATIVE DISTRIBUTION FUNCTION
     */
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	if (x<=XI) return 0.0;
	if (x<=LAMBDA+XI) return N*0.5*gsl_sf_erfc(-((GAMMA + log((x - XI)/LAMBDA)/DELTA)/sqrt(2)));
	return N*0.5*(1 + gsl_sf_erf((GAMMA + log((x - XI)/LAMBDA)/DELTA)/sqrt(2)));
}

scalar sasfit_sd_johnson_sl_v(scalar p, sasfit_param * param, int dist)
{
    /*
     *   THIS IS THE QUANTILE DISTRIBUTION FUNCTION
     */
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	if (p<0) return GSL_NAN;
	if (p>1) return GSL_NAN;
//	if (p<=LAMBDA+XI) return (exp(-DELTA*GAMMA - sqrt(2)*DELTA*sasfit_erfinv(1-2*p))*LAMBDA+XI)/N;
	return (exp(-DELTA*GAMMA + sqrt(2)*DELTA*sasfit_erfinv(-1 + 2*p))*LAMBDA + XI)/N;
}

