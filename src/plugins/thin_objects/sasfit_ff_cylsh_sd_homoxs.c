/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define T	param->p[0]
#define SIGMA_T	param->p[1]
#define R	param->p[2]
#define SIGMA_R	param->p[3]
#define H	param->p[4]
#define SIGMA_H	param->p[5]
#define ETA_L	param->p[6]
#define ETA_SOL	param->p[7]

scalar sasfit_ff_cylsh_sd_homoxs(scalar q, sasfit_param * param)
{
	scalar Pcs, Pprime;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T); // modify condition to your needs
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((H < 0.0), param, "H(%lg) < 0",H); // modify condition to your needs

	// insert your code here
    sasfit_init_param( &subParam );
	subParam.p[0] =	T;
	subParam.p[1] =	SIGMA_T;
	subParam.p[4] =	ETA_L;
	subParam.p[5] =	ETA_SOL;
	Q=q;
	P=2;

	Pcs = sasfit_ff_pcs_homogeneousplate(q,&subParam);

	sasfit_init_param( &subParam );
	subParam.p[0] = R;
	subParam.p[1] = H;
	subParam.p[2] = SIGMA_R;
	subParam.p[3] = SIGMA_H;

	Pprime = sasfit_sq_p__q___thin_hollow_cylinder(q,&subParam);
	return Pcs*Pprime;
}

scalar sasfit_ff_cylsh_sd_homoxs_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_cylsh_sd_homoxs_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

