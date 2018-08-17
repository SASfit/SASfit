/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define I0	param->p[0]
#define RG	param->p[1]
#define ALPHA	param->p[2]
#define NU	param->p[3]
#define F	param->p[4]

scalar sasfit_ff_dozierstar(scalar q, sasfit_param * param)
{
    scalar mu,xi;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((I0 < 0.0), param, "I0(%lg) < 0",I0); // modify condition to your needs
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA < 0.0), param, "alpha(%lg) < 0",ALPHA); // modify condition to your needs
	SASFIT_CHECK_COND1((F < 1.0), param, "f(%lg) < 0",F); // modify condition to your needs
	SASFIT_CHECK_COND1((NU <= 0.0), param, "nu(%lg) <= 0",NU); // modify condition to your needs

	// insert your code here
	mu = 1.0/NU-1.;
	xi = 2.0*RG/sqrt(F);

    if (q==0.0)
	{
		if (mu==0)
		{
			return I0 + 4*M_PI*ALPHA;
		} else {
			return I0 + (4.0*M_PI*ALPHA) * mu * gsl_sf_gamma(mu);
		}
	} else { // q != 0
		if (mu==0)
		{
			return I0 + 4*M_PI*ALPHA * atan(q*xi)/(q*xi);
		} else {
			return I0*exp(-RG*RG*q*q/3.0)
				+ (4.0*M_PI*ALPHA) / (q*xi)
				* (sin(mu*atan(q*xi)))
				/ pow(1.0+q*q*xi*xi, 0.5*mu)
				* gsl_sf_gamma(mu);
		}
	}
}

scalar sasfit_ff_dozierstar_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_dozierstar_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

