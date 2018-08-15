/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define RG	param->p[0]
#define NU	param->p[1]
#define B_P	param->p[2]
#define MW	param->p[3]
#define RHO_P	param->p[4]
#define ETA_S	param->p[5]

scalar sasfit_ff_generalized_gaussian_coil_3(scalar q, sasfit_param * param)
{
    scalar u,na,v,beta,i0;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((NU <= 0.0), param, "nu(%lg) <= 0",NU); // modify condition to your needs
	SASFIT_CHECK_COND1((MW < 0.0), param, "Mw(%lg) < 0",MW); // modify condition to your needs
	SASFIT_CHECK_COND1((RHO_P < 0.0), param, "rho_p(%lg) < 0",RHO_P); // modify condition to your needs

	// insert your code here
	u = (2.0*NU+1.0)*(2.0*NU+2.0)*q*q*RG*RG/6.0;
	na 	= 6.0221415e23;
	v	= MW/RHO_P/na;
	beta	= B_P - v*ETA_S;
	i0 = beta*beta;
	if (q*RG<0.1) return i0*exp(-q*q*RG*RG/3);
    return i0*
	       (pow(u,1/(2.*NU)) * gsl_sf_gamma(1.0/(2.*NU))
		                     - gsl_sf_gamma(1.0/NU)
		  - pow(u,1/(2.*NU)) * gsl_sf_gamma_inc(1.0/(2.*NU),u) +
                               gsl_sf_gamma_inc(1.0/NU,u)
			)/(NU*pow(u,1/NU));
}

scalar sasfit_ff_generalized_gaussian_coil_3_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_generalized_gaussian_coil_3_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

