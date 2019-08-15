/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define SIGMA_A	fabs(param->p[1])
#define ALPHA	param->p[2]
#define BETA	param->p[3]
#define GAMA	param->p[4]
#define NU	param->p[5]
#define ETA	param->p[6]

#define Q   	param->p[MAXPAR-1]

scalar sasfit_ff_generalized_form_factor(scalar q, sasfit_param * param)
{
    scalar i0, m2;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA <= 0.0), param, "alpha(%lg) < 0",ALPHA); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA < 0.0), param, "beta(%lg) < 0",BETA); // modify condition to your needs
	SASFIT_CHECK_COND1((NU < 0.0), param, "nu(%lg) < 0",NU); // modify condition to your needs

	// insert your code here
	Q=q;
	if(A==0) {
        return 0.0;
    } else {
        m2= pow(A,2*ALPHA);
    }
    if (SIGMA_A==0) {
        i0=pow(A*q,2*NU-BETA)*pow(4,-NU)*gsl_pow_2(gsl_sf_hyperg_0F1(NU+1,-Q*A*Q*A/4.)/gsl_sf_gamma(NU+1));
    } else {
    }

	return i0*m2*ETA*ETA;
}

scalar sasfit_ff_generalized_form_factor_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_generalized_form_factor_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

