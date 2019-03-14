/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables




scalar sasfit_ff_boolean_union_model(scalar q, sasfit_param * param)
{
    scalar *aw, res,err,xstart,xend;
    int intstrategy, lenaw=4000;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((SCALE < 0.0), param, "Scale(%lg) < 0",SCALE); // modify condition to your needs
	SASFIT_CHECK_COND1((FP < 0.0), param, "phi_1(%lg) < 0",FP); // modify condition to your needs
	SASFIT_CHECK_COND1((MU < 0.0), param, "mu(%lg) < 0",MU); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA < 0.0), param, "sigma(%lg) < 0",SIGMA); // modify condition to your needs

	// insert your code here
	Q=q;
    aw = (scalar *)malloc((lenaw)*sizeof(scalar));
//    if (Q*MU*exp(-SIGMA*SIGMA) < 1e-1) {
//        find_LogNorm_int_range(6,MU,SIGMA,&xstart,&xend,param);
//        sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
//        sasfit_intde(&gammaBoolean,0, sasfit_eps_get_jmax_aniso()*xend,aw, &res, &err, param);
//    } else {
        sasfit_intdeoini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
        sasfit_intdeo(&gammaBoolean,0,q, aw, &res, &err, param);
//    }
    free(aw);
    return SCALE*res;
}

scalar sasfit_ff_boolean_union_model_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_boolean_union_model_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

