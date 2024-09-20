/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>


// define shortcuts for local parameters/variables


scalar guwelop_planar_Pcs(scalar x, sasfit_param *param) {
	scalar fsp,eta;
	eta = 1-sasfit_sd_p_guwelop_f(x, param);
	fsp = N*eta*2*cos(Q*x);
	return fsp;
}

scalar sasfit_ff_guwelop_planar_Pcs(scalar q, sasfit_param * param) {

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((LAMBDA < 0.0), param, "lambda(%lg) < 0",LAMBDA); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA < 0.0), param, "alpha(%lg) < 0",ALPHA); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA < 0.0), param, "beta(%lg) < 0",BETA); // modify condition to your needs
	SASFIT_CHECK_COND1((GAMMA < 0.0), param, "gamma(%lg) < 0",GAMMA); // modify condition to your needs

	// insert your code here

	return gsl_pow_2(sasfit_ff_guwelop_planar_Pcs_f(q,param));
}

scalar sasfit_ff_guwelop_planar_Pcs_f(scalar q, sasfit_param * param)
{
    scalar xstart,xend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	Q=q;
	xstart=0;
	param->p[1]=xstart;
	xend=GSL_MIN(BU,sasfit_sd_p_guwelop_v(1-sasfit_eps_get_nriq(),param,DISTRIBUTION_QUANTILE));
	return sasfit_integrate(xstart,xend,&guwelop_planar_Pcs,param);
}

scalar sasfit_ff_guwelop_planar_Pcs_v(scalar q, sasfit_param * param, int dist)
{
    scalar xstart,xend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	xstart=0;
	xend=sasfit_sd_p_guwelop_v(1-sasfit_eps_get_nriq(),param,DISTRIBUTION_QUANTILE);
	Q=0;
	return sasfit_integrate(xstart,xend,&guwelop_planar_Pcs,param);
}

