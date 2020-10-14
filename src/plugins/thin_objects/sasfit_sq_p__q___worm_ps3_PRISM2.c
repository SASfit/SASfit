/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_deriv.h>

// define shortcuts for local parameters/variables


scalar sasfit_sq_p__q___worm_ps3_PRISM2(scalar q, sasfit_param * param)
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


	scalar Swc,S0,cq,u,u2;
	S0=L*L*2;
	if (MONO_SD>0) {
        Swc=PS3_EXP_SD(q,param);
	} else {
        Swc=sasfit_sq_p__q___worm_ps3_(q,param);
	}
    u = q*LP;
	u2 = u/2.0;
	if (u==0.0) {
		cq = 1;
	} else {
		cq = (2.0*gsl_sf_Si(u)/u-gsl_pow_2(sin(u2)/u2) );
	}
	return Swc/(1+BETA*cq*Swc/S0);
}

scalar sasfit_sq_p__q___worm_ps3_PRISM2_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_p__q___worm_ps3_PRISM2_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}
