/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define RG	param->p[0]
#define ALPHA	param->p[1]
#define DETA	param->p[2]
#define BETA    (ALPHA/2.-3)
scalar sasfit_ff_gen__exp__sphere(scalar q, sasfit_param * param)
{
    scalar V;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA <= 0.0), param, "alpha(%lg) <= 0",ALPHA); // modify condition to your needs

    return gsl_pow_2(sasfit_ff_gen__exp__sphere_f(q,param));
}

scalar sasfit_ff_gen__exp__sphere_f(scalar q, sasfit_param * param)
{
    scalar V;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here

	V = 4*gsl_pow_3(RG)*M_PI;
    if (q*RG<1e-6) return (V*DETA)*(1-gsl_pow_2(RG*q)/6.);

	if (ALPHA==2) {
       return (V*DETA)*M_SQRT2*atan(RG*q/M_SQRT2)/(RG*q);
	} else {
	    return (V*DETA)*(pow((3 + BETA)*(4 + BETA),(3 + BETA)/2.)*pow(12 + BETA*(7 + BETA) + gsl_pow_2(RG*q),-1 - BETA/2.)*
                sin((2 + BETA)*atan((RG*q)/sqrt((3 + BETA)*(4 + BETA)))))/(RG*(2 + BETA)*q);
	}
}

scalar sasfit_ff_gen__exp__sphere_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 4*gsl_pow_3(RG)*M_PI;

}

