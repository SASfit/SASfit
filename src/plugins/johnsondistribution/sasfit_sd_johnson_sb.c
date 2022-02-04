/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables
#define N	param->p[0]
#define L	param->p[1]
#define U	param->p[2]
#define XI	param->p[3]
#define LAMBDA	param->p[4]
#define DELTA	param->p[5]
#define GAMMA	param->p[6]

scalar median_sd_johnson_sb(scalar x, sasfit_param * param) {
    return LAMBDA/(1 + exp(GAMMA/DELTA)) + XI;
}

scalar sasfit_sd_johnson_sb(scalar x, sasfit_param * param)
{
    /*
     *   THIS IS THE PROBABILITY DISTRIBUTION FUNCTION
     */
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((x < 0.0), param, "x(%lg) < 0",x);
	SASFIT_CHECK_COND1((L < 0.0), param, "l(%lg) < 0",L); // modify condition to your needs
	SASFIT_CHECK_COND1((U < 0.0), param, "u(%lg) < 0",U); // modify condition to your needs
	SASFIT_CHECK_COND1((XI < 0.0), param, "xi(%lg) < 0",XI); // modify condition to your needs
	SASFIT_CHECK_COND1((LAMBDA <= 0.0), param, "lambda(%lg) <= 0",LAMBDA); // modify condition to your needs
	SASFIT_CHECK_COND1((DELTA < 0.0), param, "delta(%lg) < 0",DELTA); // modify condition to your needs
	SASFIT_CHECK_COND1((GAMMA < 0.0), param, "gamma(%lg) < 0",GAMMA); // modify condition to your needs

	// insert your code here
	if (x<=XI) return 0;
	if (x>=LAMBDA+XI) return 0;
	return (DELTA*LAMBDA*exp(-gsl_pow_int(GAMMA + DELTA*log((x - XI)/(LAMBDA - x + XI)),2)/2.))/(sqrt(2*M_PI)*(x - XI)*(LAMBDA - x + XI));
}

scalar sasfit_sd_johnson_sb_f(scalar x, sasfit_param * param)
{
    /*
     *   THIS IS THE CUMMULITATIVE DISTRIBUTION FUNCTION
     */
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	if (x>=LAMBDA+XI) return 1.0;
	if (XI<x && x< LAMBDA/2+XI) {
            return gsl_sf_erfc((-GAMMA + DELTA*log(-1 + LAMBDA/(x - XI)))/sqrt(2))/2.;
	}
	if (LAMBDA/2+XI<=x && x< LAMBDA+XI) {
            return (1 + gsl_sf_erf((GAMMA + DELTA*log(-1 + LAMBDA/(LAMBDA - x + XI)))/sqrt(2)))/2.;
	}
    return 0.0;
}

scalar sasfit_sd_johnson_sb_v(scalar p, sasfit_param * param, int dist)
{
    /*
     *   THIS IS THE QUANTILE DISTRIBUTION FUNCTION
     */
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	if (p<=0) return XI;
	if (p>=1) return LAMBDA+XI;
	if (N*p<sasfit_sd_johnson_sb_f(LAMBDA/2.+XI,param)) {
	    if ( !gsl_finite(exp((GAMMA + sqrt(2)*sasfit_erfinv(1-2*p))/DELTA)) ) return XI;
        return (LAMBDA + XI + exp((GAMMA + sqrt(2)*sasfit_erfinv(1-2*p))/DELTA)*XI) /
                         (1 + exp((GAMMA + sqrt(2)*sasfit_erfinv(1-2*p))/DELTA));
	}
	return (LAMBDA + exp((-GAMMA + sqrt(2)*sasfit_erfinv(-1 + 2*p))/DELTA)*LAMBDA +
            exp((-GAMMA + sqrt(2)*sasfit_erfinv(-1 + 2*p))/DELTA)*XI)/
            (1 + exp((-GAMMA + sqrt(2)*sasfit_erfinv(-1 + 2*p))/DELTA));
}

