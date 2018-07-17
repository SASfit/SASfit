/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define T	param->p[0]
#define SIGMA_T	fabs(param->p[1])
#define R0	param->p[2]
#define SIGMA_R0	fabs(param->p[3])
#define H0	param->p[4]
#define SIGMA_H0	fabs(param->p[5])

#define SIGMA_OUT	fabs(param->p[6])
#define B_OUT	param->p[7]
#define SIGMA_CORE	fabs(param->p[8])
#define B_CORE	param->p[9]

scalar sasfit_ff_cylsh_sd_bilayergauss(scalar q, sasfit_param * param)
{
	scalar Pcs, Pprime;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T); // modify condition to your needs
	SASFIT_CHECK_COND1((R0 < 0.0), param, "R0(%lg) < 0",R0); // modify condition to your needs
	SASFIT_CHECK_COND1((H0 < 0.0), param, "H0(%lg) < 0",H0); // modify condition to your needs

	// insert your code here
	sasfit_init_param( &subParam );
	subParam.p[0] =	SIGMA_CORE;
	subParam.p[1] =	B_CORE;
	subParam.p[2] =	SIGMA_OUT;
	subParam.p[3] =	B_OUT;
	subParam.p[4] =	T;
	subParam.p[5] =	SIGMA_T;
    Q=q;
    P=2;
	Pcs = sasfit_ff_pcs_bilayergauss(q,&subParam);

	sasfit_init_param( &subParam );
	subParam.p[0] = R0;
	subParam.p[1] = H0;
	subParam.p[2] = SIGMA_R0;
	subParam.p[3] = SIGMA_H0;

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

