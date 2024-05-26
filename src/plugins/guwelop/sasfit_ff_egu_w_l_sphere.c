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
#define Q       param->p[MAXPAR-1]

scalar egu_w_l_sphere(scalar x, sasfit_param *param) {
	scalar fsp,eta;
	eta = 1-sasfit_sd_egu_w_l_f(x, param);
	fsp = N*eta*4*M_PI*x*x*gsl_sf_bessel_j0(Q*x);
	return fsp;
}

scalar sasfit_ff_egu_w_l_sphere(scalar q, sasfit_param * param)
{
    scalar pegu_w_l,z,t;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((LAMBDA <= 0.0), param, "lambda(%lg) <= 0",LAMBDA); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA <= 0.0), param, "alpha(%lg) <= 0",ALPHA); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA <= 0.0), param, "beta(%lg) <= 0",BETA); // modify condition to your needs
	SASFIT_CHECK_COND1((GAMMA <= 0.0), param, "gamma(%lg) <= 0",GAMMA); // modify condition to your needs

	// insert your code here
	return gsl_pow_2(sasfit_ff_egu_w_l_sphere_f(q,param));
}

scalar sasfit_ff_egu_w_l_sphere_f(scalar q, sasfit_param * param)
{
    scalar xstart, xend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code herehere
	Q=q;
	xstart=0;
	xend=sasfit_sd_egu_w_l_v(1-sasfit_eps_get_nriq(),param,0);
	return sasfit_integrate(xstart,xend,&egu_w_l_sphere,param);
}

scalar sasfit_ff_egu_w_l_sphere_v(scalar y, sasfit_param * param, int dist)
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

