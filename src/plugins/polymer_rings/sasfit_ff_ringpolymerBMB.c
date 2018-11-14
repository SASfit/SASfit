/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define RG	param->p[0]

#define NU	param->p[2]
#define I0	param->p[3]
#define Q   param->p[MAXPAR-1]
#define KEPS    param->p[MAXPAR-2]
#define EPS param->p[MAXPAR-3]
#define MU  param->p[MAXPAR-4]

scalar Kernel_P_OOURA_BMB(scalar x, void * pam) {
	sasfit_param *param;
	param = (sasfit_param *) pam;
	return 2*(1-x)*exp(-MU*pow(x*(1-x),1+EPS));
}

scalar sasfit_ff_ringpolymerBMB(scalar q, sasfit_param * param)
{
    scalar u;
    scalar *aw, res,err,sum;
    int intstrategy, ndim, lenaw=4000;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((NU <= 0.0), param, "NU(%lg) <= 0",NU);
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs

	// insert your code here
    EPS = 2*NU-1;
    KEPS = gsl_sf_gamma(2+EPS)*gsl_sf_gamma(3+EPS)/gsl_sf_gamma(5+2*EPS);
    Q = q;
    MU = gsl_pow_2(q*RG)/(6*KEPS);

    intstrategy = sasfit_get_int_strategy();
	intstrategy=OOURA_DOUBLE_EXP_QUADRATURE;
	switch(intstrategy) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
            aw = (scalar *)malloc((lenaw)*sizeof(scalar));
            sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
            sasfit_intde(&Kernel_P_OOURA_BMB, 0,1, aw, &res, &err, param);
			sum=res;
            free(aw);
            break;
            }
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
            aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
            sasfit_intccini(lenaw, aw);
            sasfit_intcc(&Kernel_P_OOURA_BMB, 0,1, sasfit_eps_get_nriq(), lenaw, aw, &res, &err,param);
			sum=res;
            free(aw);
            break;
            }
    default: {
//		    sasfit_out("ise default sasfit_integrate routine\n");
//            sum=sasfit_integrate(0.0, 1.0, sasfit_ff_ellip_shell_0_core_MU, param);
            break;
            }
    }
    return sum*I0;
}

scalar sasfit_ff_ringpolymerBMB_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_ringpolymerBMB_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

