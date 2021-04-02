/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

#define R	param->p[0]

#define L	param->p[2]
#define ETA	param->p[3]
#define Q   param->p[MAXPAR-1]

scalar cylinder_kernel_f(scalar x, sasfit_param *param) {
    return (M_PI*R*R*L*ETA
                *sasfit_jinc(Q*R*sqrt(1.0-x*x))
                *2*gsl_sf_bessel_j0(Q*L*x/2.0));
}

scalar cylinder_kernel(scalar x, sasfit_param * param) {
    return gsl_pow_2(M_PI*R*R*L*ETA
                *sasfit_jinc(Q*R*sqrt(1.0-x*x))
                *2*gsl_sf_bessel_j0(Q*L*x/2.0));
}

scalar sasfit_ff_cylinder(scalar q, sasfit_param * param)
{	scalar *aw, res,err,sum;
    scalar cubxmin[1], cubxmax[1], fval[1], ferr[1];
    int intstrategy, lenaw=4000;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L); // modify condition to your needs

	// insert your code here
    Q=q;

	return sasfit_integrate(0.0,1.0, &cylinder_kernel, param);

}

scalar sasfit_ff_cylinder_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
    return sasfit_integrate(0.0,1.0, &cylinder_kernel_f, param);
}

scalar sasfit_ff_cylinder_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return M_PI*R*R*L;
}

