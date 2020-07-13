/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define N	param->p[0]
#define D	param->p[1]
#define DELTA	param->p[2]
#define NU	param->p[3]

scalar sasfit_sq_thermaldisorder(scalar q, sasfit_param * param)
{
    scalar DWf,qd,qD2,w,SQ1,SQ2;
    long NN,k;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
//	SASFIT_CHECK_COND1((N < 0.0), param, "N(%lg) < 0",N); // modify condition to your needs
	SASFIT_CHECK_COND1((D < 0.0), param, "d(%lg) < 0",D); // modify condition to your needs
	SASFIT_CHECK_COND1((DELTA < 0.0), param, "Delta(%lg) < 0",DELTA); // modify condition to your needs
	SASFIT_CHECK_COND1((NU < 0.0), param, "Nu(%lg) < 0",NU); // modify condition to your needs

	// insert your code here
    qd=q*D;
    qD2 = gsl_pow_2(q*DELTA);
    NN = lround(floor(abs(N)));
	w = N-floor(N);

	DWf = exp(-gsl_pow_2(q*DELTA)/2.);
	return N*(1-DWf)+DWf*(sasfit_sq_perfect_ordered(q,param)-NU)+NU;
}

scalar sasfit_sq_thermaldisorder_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_thermaldisorder_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

