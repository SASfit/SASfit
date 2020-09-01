/*
 * Author(s) of this file
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define ALPHA	param->p[1]
#define BETA	param->p[2]
#define GAMMA	param->p[3]
#define DELTA	param->p[4]
#define C0	param->p[5]

scalar sasfit_ff_trapez__area(scalar x, sasfit_param * param)
{
    scalar u,p;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND2((ALPHA>=BETA), param, "alpha(%lg)>=beta(%lg)",ALPHA,BETA); // modify condition to your needs
	SASFIT_CHECK_COND2((BETA>=GAMMA), param, "beta(%lg)>=gamma(%lg)",BETA,GAMMA); // modify condition to your needs
	SASFIT_CHECK_COND2((GAMMA>=DELTA), param, "gammma(%lg)>=delta(%lg)",GAMMA,DELTA); // modify condition to your needs

	// insert your code
	u=2/(DELTA+GAMMA-BETA-ALPHA);

	p=0;
    if (ALPHA<=x && x<BETA) {
        p=(x-ALPHA)/(BETA-ALPHA)*u;
	} else if (BETA<=x && x<GAMMA) {
	    p=u;
	} else if (GAMMA<=x && x<DELTA) {
	    p=u*(DELTA-x)/(DELTA-GAMMA);
	}
	// insert your code here
	return A*p+C0;
}

scalar sasfit_ff_trapez__area_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_trapez__area_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

