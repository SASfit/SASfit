/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define I0	param->p[0]
#define RG	param->p[1]

#define F	param->p[3]
#define NU	param->p[4]

scalar sasfit_ff_dozierstar2(scalar q, sasfit_param * param)
{
    scalar mu,xi;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((I0 < 0.0), param, "I0(%lg) < 0",I0); // modify condition to your needs
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((F < 1.0), param, "f(%lg) < 1",F); // modify condition to your needs
	SASFIT_CHECK_COND1((NU < 0.0), param, "nu(%lg) < 0",NU); // modify condition to your needs

	// insert your code here
    mu = 1.0/NU-1.;
	xi = 2.0*RG/sqrt(F);

	if (q==0.0)
	{
		return I0;
	} else {
		if (mu==0.0)
		{
			return I0/F*( (F-1.0)
				* exp(-RG*RG*q*q/3.0)
				+ atan(q*xi)/(q*xi) );
		} else {
			return I0/F * ( (F-1.0)
					   * exp(-RG*RG*q*q/3.0)
					   + 1.0/(q*xi)
					   * (sin(mu*atan(q*xi)))
					   / pow(1.0+q*q*xi*xi, 0.5*mu)
					   * gsl_sf_gamma(mu) );
		}
	}
}

scalar sasfit_ff_dozierstar2_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_dozierstar2_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

