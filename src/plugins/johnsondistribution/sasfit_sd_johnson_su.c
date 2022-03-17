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
#define DELTA	fabs(param->p[5])
#define GAMMA	param->p[6]

scalar mean_sd_johnson_su(scalar x, sasfit_param * param) {
    return ((-exp(-(-1 - 2*DELTA*GAMMA)/(2.*gsl_pow_int(DELTA,2))) +
        exp((1 - 2*DELTA*GAMMA)/(2.*gsl_pow_int(DELTA,2))))*LAMBDA)/2. + XI;
}

scalar median_sd_johnson_su(scalar x, sasfit_param * param) {
    return XI - LAMBDA*sinh(GAMMA/DELTA);
}

scalar variance_sd_johnson_su(scalar x, sasfit_param * param) {
    return ((-1 + exp(1/gsl_pow_int(DELTA,2)))*
     (exp(1/gsl_pow_int(DELTA,2)) + 2*exp((2*GAMMA)/DELTA) +
       exp((1 + 4*DELTA*GAMMA)/gsl_pow_int(DELTA,2)))*gsl_pow_int(LAMBDA,2))/
   (4.*exp((2*GAMMA)/DELTA));
}

scalar kurtosis_sd_johnson_su(scalar x, sasfit_param * param) {
    return (4*exp((2 + 2*DELTA*GAMMA)/gsl_pow_int(DELTA,2))*
      (2 + exp(1/gsl_pow_int(DELTA,2))) +
     4*exp((2 + 6*DELTA*GAMMA)/gsl_pow_int(DELTA,2))*
      (2 + exp(1/gsl_pow_int(DELTA,2))) +
     6*exp((4*GAMMA)/DELTA)*(1 + 2*exp(1/gsl_pow_int(DELTA,2))) +
     exp(2/gsl_pow_int(DELTA,2))*(-3 +
        exp(2/gsl_pow_int(DELTA,2))*
         (3 + exp(1/gsl_pow_int(DELTA,2))*(2 + exp(1/gsl_pow_int(DELTA,2))))) +
     exp((2 + 8*DELTA*GAMMA)/gsl_pow_int(DELTA,2))*
      (-3 + exp(2/gsl_pow_int(DELTA,2))*
         (3 + exp(1/gsl_pow_int(DELTA,2))*(2 + exp(1/gsl_pow_int(DELTA,2))))))/
   gsl_pow_int(exp(1/gsl_pow_int(DELTA,2)) + 2*exp((2*GAMMA)/DELTA) +
     exp((1 + 4*DELTA*GAMMA)/gsl_pow_int(DELTA,2)),2);
}

scalar skewness_sd_johnson_su(scalar x, sasfit_param * param) {
    return (exp(1/(2.*gsl_pow_int(DELTA,2)))*sqrt(-1 + exp(1/gsl_pow_int(DELTA,2)))*
     (3*exp((2*GAMMA)/DELTA) - 3*exp((4*GAMMA)/DELTA) +
       exp(1/gsl_pow_int(DELTA,2))*(2 + exp(1/gsl_pow_int(DELTA,2))) -
       exp((1 + 6*DELTA*GAMMA)/gsl_pow_int(DELTA,2))*
        (2 + exp(1/gsl_pow_int(DELTA,2)))))/
   pow(exp(1/gsl_pow_int(DELTA,2)) + 2*exp((2*GAMMA)/DELTA) +
     exp((1 + 4*DELTA*GAMMA)/gsl_pow_int(DELTA,2)),1.5);
}

scalar sasfit_sd_johnson_su(scalar x, sasfit_param * param)
{   scalar z;
    /*
     *   THIS IS THE PROBABILITY DISTRIBUTION FUNCTION
     */
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((N < 0.0), param, "N(%lg) < 0",N); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "l(%lg) < 0",L); // modify condition to your needs
	SASFIT_CHECK_COND1((U < 0.0), param, "u(%lg) < 0",U); // modify condition to your needs
	SASFIT_CHECK_COND1((XI < 0.0), param, "xi(%lg) < 0",XI); // modify condition to your needs
	SASFIT_CHECK_COND1((LAMBDA == 0.0), param, "lambda(%lg) == 0",LAMBDA); // modify condition to your needs
	SASFIT_CHECK_COND1((DELTA == 0.0), param, "delta(%lg) == 0",DELTA); // modify condition to your needs

	// insert your code here

	if (x<=L || x>=U) return 0;
	z = (x-XI)/LAMBDA;
	return N*DELTA/(LAMBDA*sqrt(2*M_PI)*sqrt(z*z+1))*exp(-0.5*gsl_pow_2(GAMMA+DELTA*gsl_asinh(z)));
}

scalar sasfit_sd_johnson_su_f(scalar x, sasfit_param * param)
{   scalar z;
    /*
     *   THIS IS THE CUMMULITATIVE DISTRIBUTION FUNCTION
     */
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	z = (x-XI)/LAMBDA;
	return N*0.5*(1+gsl_sf_erf((GAMMA+DELTA*gsl_asinh(z))/sqrt(2)));
}

scalar sasfit_sd_johnson_su_v(scalar p, sasfit_param * param, int dist)
{
    /*
     *   THIS IS THE QUANTILE DISTRIBUTION FUNCTION
     */
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return (XI + LAMBDA*sinh(DELTA*(sqrt(2)*sasfit_erfinv(-1 + 2*p)-GAMMA)))/N;;
}

