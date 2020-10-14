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
#define BETA    ((3+ALPHA)*(4+ALPHA))
scalar sasfit_ff_gen__exp__sphere(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA <= -3.0), param, "alpha(%lg) <= -3",ALPHA); // modify condition to your needs

    return gsl_pow_2(sasfit_ff_gen__exp__sphere_f(q,param));
}

scalar sasfit_ff_gen__exp__sphere_f(scalar q, sasfit_param * param)
{
    scalar V;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here

	V = 4*gsl_pow_3(RG)*M_PI;
    if (q*RG<1e-6) return (V*DETA)*(1-gsl_pow_2(RG*q)/6.);

	if (ALPHA == -2) {
       return (V*DETA)*M_SQRT2*atan(RG*q/M_SQRT2)/(RG*q);
	} else {
	    return DETA*V*(
                  exp(
                    log(BETA)*((3 + ALPHA)/2.)+
                    log(12 + ALPHA*(7 + ALPHA) + q*q*RG*RG)*(-1 - ALPHA/2.)
                     )*
                sin((2 + ALPHA)*atan((q*RG)/sqrt(BETA))))/((2 + ALPHA)*q*RG);
	}
}

scalar sasfit_ff_gen__exp__sphere_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 4*gsl_pow_3(RG)*M_PI;

}

