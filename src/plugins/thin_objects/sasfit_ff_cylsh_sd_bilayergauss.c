/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define SIGMA_CORE fabs(param->p[0])
#define B_CORE param->p[1]
#define SIGMA_OUT fabs(param->p[2])
#define B_OUT param->p[3]
#define D	param->p[4]

#define R0	param->p[0]
#define H0	param->p[1]
#define SIGMA_R	fabs(param->p[2])
#define SIGMA_H	fabs(param->p[3])

#define	R		param->p[MAXPAR-2]
#define	H		param->p[MAXPAR-3]
scalar sasfit_ff_cylsh_sd_bilayergauss(scalar q, sasfit_param * param)
{
	scalar Pcs, Pprime;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
//	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T); // modify condition to your needs

	// insert your code here
    sasfit_init_param( &subParam );
	subParam.p[0] = R;
	subParam.p[1] = H;
	subParam.p[2] = SIGMA_R;
	subParam.p[2] = SIGMA_H;

	Pprime = sasfit_sq_p__q___thin_hollow_cylinder(q,&subParam);
	return Pcs*Pprime;
}

scalar sasfit_ff_cylsh_sd_bilayergauss_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_cylsh_sd_bilayergauss_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

