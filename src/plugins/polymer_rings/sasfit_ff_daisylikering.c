/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define RG	param->p[0]
#define M	param->p[1]
#define DUMMY	param->p[2]
#define I0	param->p[3]

scalar sasfit_ff_daisylikering(scalar q, sasfit_param * param)
{
	scalar w, u, P1r, Pmr, Pmr_p1;
	long i, k;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg,1r(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((M < 0.0), param, "m(%lg) < 0",M); // modify condition to your needs
	SASFIT_CHECK_COND1((DUMMY < 0.0), param, "dummy(%lg) < 0",DUMMY); // modify condition to your needs
	SASFIT_CHECK_COND1((I0 < 0.0), param, "I0(%lg) < 0",I0); // modify condition to your needs

	// insert your code here
    Pmr = 0.0;
	k = lround(floor(M));
	w = M-floor(M);
	u = q*RG;
	if (u  == 0.0)
	{
		P1r = I0;
	} else {
		P1r = I0*sqrt(2./(u*u))*gsl_sf_dawson(sqrt(u*u/2.));
	}
	Pmr    = 1./k * (P1r+(k-1.)*P1r*P1r);
	k=k+1;
	Pmr_p1 = 1./k * (P1r+(k-1.)*P1r*P1r);
	return I0*((1-w)*Pmr+w*Pmr_p1);
}

scalar sasfit_ff_daisylikering_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_daisylikering_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

