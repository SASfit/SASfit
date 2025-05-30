/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

scalar sasfit_ff_clipped_random_waves_twocut_intersection_2(scalar q, sasfit_param * param)
{
    scalar *aw, res,err;
    sm_param sm_pam;
    int intstrategy, lenaw=4000;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((SCALE < 0.0), param, "Scale(%lg) < 0",SCALE); // modify condition to your needs
	SASFIT_CHECK_COND1((FP <= 0.0), param, "fp(%lg) <= 0",FP); // modify condition to your needs
	SASFIT_CHECK_COND1((FP >= 1.0), param, "fp(%lg) >= 1",FP); // modify condition to your needs
	SASFIT_CHECK_COND1((XI <= 0.0), param, "xi(%lg) <= 0",XI); // modify condition to your needs
	SASFIT_CHECK_COND1((DD <= 0.0), param, "d(%lg) <= 0",DD); // modify condition to your needs
	SASFIT_CHECK_COND1((RM <= 0.0), param, "rc(%lg) <= 0",RM); // modify condition to your needs

	// insert your code here
	ALPHA = sasfit_erfinv(sqrt(FP))*M_SQRT2;
	Q=q;
//	return gy2(q,param);
	sm_pam.param = param;
	sm_pam.C11kernel=&C11_twocut_kernel;
	sm_pam.gy=&gy2;
	sm_pam.sm_intersect=2;
    aw = (scalar *)malloc((lenaw)*sizeof(scalar));
    sasfit_intdeoini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
    sasfit_intdeo(&gama,0,q, aw, &res, &err, &sm_pam);
    free(aw);
    return SCALE*res;

    aw = (scalar *)malloc((lenaw)*sizeof(scalar));
    sasfit_intdeoini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
    sasfit_intdeo(&gamaY2,0,q, aw, &res, &err, param);
    free(aw);
    return SCALE*res;
}

scalar sasfit_ff_clipped_random_waves_twocut_intersection_2_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_clipped_random_waves_twocut_intersection_2_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

