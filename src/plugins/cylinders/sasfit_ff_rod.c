/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define L	param->p[0]


#define ETA	param->p[3]

scalar sasfit_ff_rod(scalar q, sasfit_param * param)
{   scalar u,u2, Gmma, IP;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L); // modify condition to your needs

	SASFIT_CHECK_COND1((ETA < 0.0), param, "eta(%lg) < 0",ETA); // modify condition to your needs

	// insert your code here
    u = q*L;
	if (fabs(u<1e-6)) {
		Gmma = 1 - gsl_pow_2(u)/18. + gsl_pow_4(u)/600. - gsl_pow_6(u)/35280. + gsl_pow_8(u)/3265920.;
    } else {
		Gmma = gsl_sf_Si(u)/u;
	}
    u2 = u/2.0;
	IP = L*L*(2.0*Gmma-gsl_pow_2(gsl_sf_bessel_j0(u2)) );
	return ETA*ETA*IP;
}

scalar sasfit_ff_rod_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0;
}

scalar sasfit_ff_rod_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

