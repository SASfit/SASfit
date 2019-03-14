/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

scalar sasfit_ff_dead_leaves_model(scalar q, sasfit_param * param)
{
    scalar *aw, res,err;
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
    sasfit_intdeoini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
    sasfit_intdeo(&gammaDeadLeaves,0,q, aw, &res, &err, param);
    return SCALE*res;
}

scalar sasfit_ff_dead_leaves_model_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_dead_leaves_model_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

