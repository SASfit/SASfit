/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables
#define N	param->p[0]
#define LAMBDA	fabs(param->p[1])
#define THETA	fabs(param->p[2])
#define ALPHA	fabs(param->p[3])
#define BETA	fabs(param->p[4])
#define GAMMA	fabs(param->p[5])

scalar sasfit_sd_p_guwelop(scalar x, sasfit_param * param)
{
    scalar pGUWELOP;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((x < 0.0), param, "x(%lg) < 0",x);
	SASFIT_CHECK_COND1((LAMBDA <= 0.0), param, "lambda(%lg) <= 0",LAMBDA); // modify condition to your needs
	SASFIT_CHECK_COND1((THETA <= 0.0), param, "theta(%lg) <= 0",THETA); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA <= 0.0), param, "alpha(%lg) <= 0",ALPHA); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA <= 0.0), param, "beta(%lg) <= 0",BETA); // modify condition to your needs
	SASFIT_CHECK_COND1((GAMMA <= 0.0), param, "gamma(%lg) <= 0",GAMMA); // modify condition to your needs

	// insert your code here
	if (x<=0) return 0;
	pGUWELOP = (ALPHA*BETA*exp(-(BETA/pow(-1 + exp(pow(x/LAMBDA,ALPHA)),1/GAMMA)) +
       THETA - THETA/exp(BETA/pow(-1 + exp(pow(x/LAMBDA,ALPHA)),1/GAMMA)) +
       pow(x/LAMBDA,ALPHA))*THETA*pow(x/LAMBDA,ALPHA)) /
       ((-1 + exp(THETA))*pow(-1 + exp(pow(x/LAMBDA,ALPHA)),(1 + GAMMA)/GAMMA)*GAMMA*x);
	if (gsl_isnan(pGUWELOP)) {
        return 0.0;
	} else {
	    return N*pGUWELOP;
	}
}

scalar sasfit_sd_p_guwelop_f(scalar x, sasfit_param * param)
{
    scalar FGUWELOP;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	if (x<=0) return 0;
	FGUWELOP = 1 - (expm1((-expm1(-(BETA/pow(expm1(pow(x/LAMBDA,ALPHA)),1/GAMMA))))*THETA))/(expm1(THETA));
	if (gsl_isnan(FGUWELOP)) {
        return 1;
	} else {
	    return FGUWELOP;
	}
}

scalar sasfit_sd_p_guwelop_v(scalar y, sasfit_param * param, int dist)
{
    scalar QGUWELOP;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here

	// case 0: quantile distribution function
	// case 1: not yet defined (mode?, median?, ....)
	switch (dist) {
        case 0: if (y<=0) return 0;
                if (y>=1) y = 1-sasfit_eps_get_nriq();
                QGUWELOP = LAMBDA*pow(log(1 + pow(-(log(-((-THETA +
                            log((1 - exp(THETA))*(-1 + 1/(1 - exp(THETA)) + y)))/THETA))/
                    BETA),-GAMMA)),1/ALPHA);
                if (gsl_isnan(QGUWELOP)) {
                    return gsl_posinf();
                } else {
                    return QGUWELOP;
                }
        default: sasfit_err("parameter distr=%d not defined",dist);
    }

    return 0;
}

