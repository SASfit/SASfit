/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define EPSILON	param->p[1]
#define SIGMA_R	param->p[2]
#define Q	param->p[MAXPAR-1]

scalar sasfit_sq_p__q___thin_ellipsoidal_shell(scalar q, sasfit_param * param)
{
	scalar Rstart, Rend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((EPSILON <= 0.0), param, "epsilon(%lg) <= 0",EPSILON); // modify condition to your needs

	// insert your code here
	
	Q=q;
	if (SIGMA_R ==0) {
		return ThinEllShell_R_core(R,param);
	}
	find_LogNorm_int_range(2,R,SIGMA_R,&Rstart,&Rend,param);
	return sasfit_integrate(Rstart, Rend, &ThinEllShell_R_core, param);
}

scalar sasfit_sq_p__q___thin_ellipsoidal_shell_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_p__q___thin_ellipsoidal_shell_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

