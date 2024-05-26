/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables
#define N	param->p[0]
#define LAMBDA	fabs(param->p[1])
#define K	param->p[2]
#define ALPHA	fabs(param->p[3])
#define BETA	fabs(param->p[4])
#define GAMMA	fabs(param->p[5])

scalar sasfit_sd_egu_w_l(scalar x, sasfit_param * param)
{
    scalar pegu_w_l,z,t;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((x < 0.0), param, "x(%lg) < 0",x);
	SASFIT_CHECK_COND1((LAMBDA <= 0.0), param, "lambda(%lg) <= 0",LAMBDA); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA <= 0.0), param, "alpha(%lg) <= 0",ALPHA); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA <= 0.0), param, "beta(%lg) <= 0",BETA); // modify condition to your needs
	SASFIT_CHECK_COND1((GAMMA <= 0.0), param, "gamma(%lg) <= 0",GAMMA); // modify condition to your needs

	// insert your code here
	if (x<=0) return 0;
	z= pow(x/LAMBDA,ALPHA);
	pegu_w_l = (ALPHA*BETA*exp(-(exp(K/GAMMA)/pow(expm1(z),1/GAMMA)) + K/GAMMA + z)*
                pow(-expm1(-(exp(K/GAMMA)/pow(expm1(z),1/GAMMA))),-1 + BETA)*z)/
                (pow(expm1(z),(1 + GAMMA)/GAMMA)*GAMMA*x);
    if (gsl_isnan(pegu_w_l)) {
        t = -(exp(K/GAMMA)/pow(expm1(z),1/GAMMA)) + K/GAMMA + z;
        if (gsl_isnan(t)) {
            t = z;
        }
        pegu_w_l = (ALPHA*BETA*exp(t)*
                pow(-expm1(-(exp(K/GAMMA)/pow(expm1(z),1/GAMMA))),-1 + BETA)*z)/
                (pow(expm1(z),(1 + GAMMA)/GAMMA)*GAMMA*x);
        if (gsl_isnan(pegu_w_l)) {
            return 0;
        } else {
            return N*pegu_w_l;
        }
	} else {
	    return N*pegu_w_l;
	}

	t = exp(-exp(K/GAMMA)*pow(expm1(z),-1/GAMMA));
	pegu_w_l = ALPHA*BETA*exp(K/GAMMA)/(LAMBDA*GAMMA)*pow(x/LAMBDA,ALPHA-1)
			* exp(z)*pow(expm1(z),-1-1/GAMMA)*t*pow(1-t,BETA-1);
	if (gsl_isnan(pegu_w_l)) {
        return 0.0;
	} else {
	    return N*pegu_w_l;
	}
}

scalar sasfit_sd_egu_w_l_f(scalar x, sasfit_param * param)
{
    scalar Fegu_w_l,z, t;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	if (x<=0) return 0;
	z= pow(x/LAMBDA,ALPHA);
	t = exp(-exp(K/GAMMA)*pow(expm1(z),-1/GAMMA));
	Fegu_w_l = 1-pow(1 - t,BETA);
	if (gsl_isnan(Fegu_w_l)) {
        return 1;
	} else {
	    return Fegu_w_l;
	}
}

scalar sasfit_sd_egu_w_l_v(scalar y, sasfit_param * param, int dist)
{
    scalar Qegu_w_l;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here

	// case 0: quantile distribution function
	// case 1: not yet defined (mode?, median?, ....)
	switch (dist) {
        case 0: if (y<=0) return 0;
                if (y>=1) y = 1-sasfit_eps_get_nriq();
                Qegu_w_l = LAMBDA*pow(log(pow(-exp(-K/GAMMA)*log(1-pow(1-y,1/BETA)),-GAMMA)+1),1/ALPHA);
                if (gsl_isnan(Qegu_w_l)) {
                    return gsl_posinf();
                } else {
                    return Qegu_w_l;
                }
        default: sasfit_err("parameter distr=%d not defined",dist);
    }

    return 0;
}

