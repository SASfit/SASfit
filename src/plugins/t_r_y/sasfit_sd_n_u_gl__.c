/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar sasfit_sd_n_u_gl__(scalar x, sasfit_param * param)
{
    scalar z;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((N < 0.0), param, "N(%lg) < 0",N); // modify condition to your needs
	SASFIT_CHECK_COND1((BL == BU), param, "bl-bu(%lg) == 0",BL-BU); // modify condition to your needs
	SASFIT_CHECK_COND1((LAMBDA2 >= 0.0), param, "lambda2(%lg) >= 0",LAMBDA2); // modify condition to your needs
	SASFIT_CHECK_COND1((LAMBDA3 >= 0.0), param, "lambda3(%lg) >= 0",LAMBDA3); // modify condition to your needs
	SASFIT_CHECK_COND1((LAMBDA4 >= 0.0), param, "lambda4(%lg) >= 0",LAMBDA4); // modify condition to your needs

	// insert your code here
	if (x<BL) return 0;
	if (x>BU) return 0;
    z= (x-BL)/(BU-BL);
	return N/(LAMBDA2*(BU-BL)*sqrt(2*M_PI))
            *(LAMBDA3*pow(z,LAMBDA3-1)+LAMBDA4*pow(1-z,LAMBDA4-1))
            *exp(-0.5*gsl_pow_2((pow(z,LAMBDA3)-pow(1-z,LAMBDA4))/LAMBDA2));
}

scalar sasfit_sd_n_u_gl___f(scalar x, sasfit_param * param)
{
    scalar z;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
	if (x<BL) return 0;
	if (x>BU) return N;
    z= (x-BL)/(BU-BL);
	// insert your code here
	return 0.5*(1+gsl_sf_erf((pow(z,LAMBDA3)-pow(1-z,LAMBDA4))/(LAMBDA2*sqrt(2))));
}

scalar sasfit_sd_n_u_gl___v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

