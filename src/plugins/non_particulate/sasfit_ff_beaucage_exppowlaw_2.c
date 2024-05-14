/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define G	param->p[0]
#define B	param->p[1]
#define R_I	param->p[2]
#define R_Ip1	param->p[3]
#define K	param->p[4]
#define P	param->p[5]

scalar sasfit_ff_beaucage_exppowlaw_2(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((G < 0.0), param, "G(%lg) < 0",G); // modify condition to your needs
	SASFIT_CHECK_COND1((B < 0.0), param, "B(%lg) < 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((R_I < 0.0), param, "R_i(%lg) < 0",R_I); // modify condition to your needs
	SASFIT_CHECK_COND1((R_Ip1 < 0.0), param, "R_i+1(%lg) < 0",R_Ip1); // modify condition to your needs
	SASFIT_CHECK_COND1((K < 0.0), param, "k(%lg) < 0",K); // modify condition to your needs
	SASFIT_CHECK_COND1((P < 0.0), param, "P(%lg) < 0",P); // modify condition to your needs

	// insert your code here
    scalar x, xs, tmp;


	x = q*R_I;
	xs = q*R_Ip1;

	tmp = gsl_pow_3( gsl_sf_erf(x *K /sqrt(6.)) ) / q;

	return G*exp(-x*x/3.0) + B*exp(-xs*xs/3.)*pow(tmp,P);
}

scalar sasfit_ff_beaucage_exppowlaw_2_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_beaucage_exppowlaw_2_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

