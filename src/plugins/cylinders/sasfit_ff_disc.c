/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]

#define ETA	param->p[3]

scalar sasfit_ff_disc(scalar q, sasfit_param * param)
{
    scalar u;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs

	// insert your code here
    u = R*R*q*q;
	if (fabs(q*R) <= 1.0e-6) return gsl_pow_2(ETA*R*M_PI)*(R*R)*(1-u/6.+u*u/72.-u*u*u/1440.+gsl_pow_4(u)/43200.);
	return gsl_pow_2(ETA*R*M_PI)*2.0/(q*q)*(1.0-gsl_sf_bessel_J1(2.0*q*R)/(q*R));
}

scalar sasfit_ff_disc_f(scalar q, sasfit_param * param)
{   scalar u,u2;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	u=q*R;
	u2=u*u;
    if (fabs(u) <= 1.0e-6) return ETA*R*R*M_PI*(1-u2/12.+u2*u2/360.-gsl_pow_3(u2)/20160.);
	return ETA*M_PI*R*R*2*(1-cos(q*R))/(q*q*R*R);
}

scalar sasfit_ff_disc_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return M_PI*gsl_pow_2(R);
}

