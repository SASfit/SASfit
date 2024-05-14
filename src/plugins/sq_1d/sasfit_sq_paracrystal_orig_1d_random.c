/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define D	param->p[0]
#define SIGMA	param->p[1]
#define NN	param->p[2]
#define THETA_DEG param->p[3]
#define PHI_DEG param->p[4]
#define PSI_DEG param->p[5]
#define COSGAMMA param->p[MAXPAR-2]
#define Q param->p[MAXPAR-1]

scalar sasfit_sq_paracrystal_orig_1d_aligned_core(scalar x,sasfit_param *param){
    COSGAMMA = x;
    return sasfit_sq_paracrystal_orig_1d_aligned_f(Q,param);
}

scalar sasfit_sq_paracrystal_orig_1d_random(scalar q, sasfit_param * param)
{
    SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((D < 0.0), param, "D(%lg) < 0",D); // modify condition to your needs
	SASFIT_CHECK_COND1((NN < 1.0), param, "N(%lg) < 1",NN); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA < 0.0), param, "sigma(%lg) < 0",SIGMA); // modify condition to your needs
    Q = q;
	return sasfit_integrate(0,1,&sasfit_sq_paracrystal_orig_1d_aligned_core,param);
}

scalar sasfit_sq_paracrystal_orig_1d_random_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return sasfit_sq_paracrystal_orig_1d_aligned_f(q,param);
}

scalar sasfit_sq_paracrystal_orig_1d_random_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

