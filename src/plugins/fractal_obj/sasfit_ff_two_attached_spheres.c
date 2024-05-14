/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R1	param->p[0]
#define R2	param->p[2]
#define ETA0	param->p[4]
#define ETA1	param->p[5]
#define ETA2	param->p[6]

scalar sasfit_ff_two_attached_spheres(scalar q, sasfit_param * param)
{
    scalar f1, f2, R12;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R1 < 0.0), param, "R1(%lg) < 0",R1); // modify condition to your needs
	SASFIT_CHECK_COND1((R2 < 0.0), param, "R2(%lg) < 0",R2); // modify condition to your needs

    R12 = R1+R2;
	f1 = sphere_f(q,R1,ETA1-ETA0);;
	f2 = sphere_f(q,R2,ETA2-ETA0);
	// insert your code here
	return f1*f1 + f2*f2 + 2.*f1*f2*gsl_sf_bessel_j0(q*R12);
}

scalar sasfit_ff_two_attached_spheres_f(scalar q, sasfit_param * param)
{
    scalar f1, f2, R0, Vt;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	Vt = (ETA1-ETA0)*R1*R1*R1 + (ETA2-ETA0)*R2*R2*R2;
	if (Vt == 0) return 0;

	R0 = (-(ETA1-ETA0)*gsl_pow_4(R1) + (ETA2-ETA0)*gsl_pow_4(R2))/Vt;
	f1 = sphere_f(q,R1,ETA1-ETA0);
	f2 = sphere_f(q,R2,ETA2-ETA0);
	return f1*gsl_sf_bessel_j0(q*(R1+R0)) + f2*gsl_sf_bessel_j0(q*(R2-R0));
}

scalar sasfit_ff_two_attached_spheres_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 4/.3*M_PI*(gsl_pow_3(R1) + gsl_pow_3(R2));
}

