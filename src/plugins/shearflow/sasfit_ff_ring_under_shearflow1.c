/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define RG	param->p[0]
#define DUMMY	param->p[1]
#define PSI_DEG	param->p[2]
#define XI	param->p[3]
#define BX	param->p[4]
#define BZ	param->p[5]
#define Q   param->p[MAXPAR-1]
#define PSI	param->p[MAXPAR-2]
#define NN  param->p[MAXPAR-3]
#define MM  param->p[MAXPAR-4]

scalar r1expQRn2(scalar x,void *pam){
	sasfit_param * param;
	param = (sasfit_param *) pam;
	scalar mu;
	mu = fabs(x);
	return (mu+pow(mu,XI)*(gsl_pow_2(BX*cos(PSI))-gsl_pow_2(BZ*sin(PSI))));
}

scalar gsl_r1expQRn2(scalar x,sasfit_param * param){
	return (1-x)*exp(-gsl_pow_2(Q*RG)*r1expQRn2(x,param)*r1expQRn2(1-x,param));
}

scalar sasfit_ff_ring_under_shearflow1(scalar q, sasfit_param * param)
{	scalar *aw, res,err,sum;
    scalar cubxmin[2], cubxmax[2], fval[1], ferr[1];
    int intstrategy, lenaw=4000;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs

	// insert your code here

	PSI = sasfit_param_override_get_psi(PSI_DEG * M_PI/180.);
	Q = q;

	sum = sasfit_integrate(0,1,&gsl_r1expQRn2,param);
    return sum;
}

scalar sasfit_ff_ring_under_shearflow1_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_ring_under_shearflow1_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

