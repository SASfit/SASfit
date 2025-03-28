/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_deriv.h>

// define shortcuts for local parameters/variables

scalar sasfit_sq_p__q___worm_ps3_PRISM3(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((LB < 0.0), param, "lb(%lg) < 0",LB); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L); // modify condition to your needs

	// insert your code here

// J. S. Pedersen and P. Schurtenberger,
// Scattering Functions of Semiflexible Polymers with and without Excluded Volume Effects,
// Macromolecules 1996, 29, 7602-7612
// DOI: 10.1021/ma9607630
// here method 3 of the above paper is implemented.

	scalar Swc,S0,cq,u;
	S0=L*L*2;
	if (MONO_SD>0) {
        Swc=PS3_EXP_SD(q,param);
	} else {
        Swc=sasfit_sq_p__q___worm_ps3_(q,param);
	}
	u=q*2*RP;
	if (abs(u)<1e-6) {
        cq = 1-gsl_pow_2(u)/10. + gsl_pow_4(u)/280.;
	} else {
	    cq = 3*gsl_sf_bessel_j1(u)/u;
	}
	return Swc/(1+BETA*cq*Swc/S0);
}

scalar sasfit_sq_p__q___worm_ps3_PRISM3_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_p__q___worm_ps3_PRISM3_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}
