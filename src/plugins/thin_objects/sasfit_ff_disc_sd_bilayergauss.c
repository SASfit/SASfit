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


#define SIGMA_OUT	param->p[6]
#define B_OUT	param->p[7]
#define SIGMA_CORE	param->p[8]
#define B_CORE	param->p[9]

scalar sasfit_ff_disc_sd_bilayergauss(scalar q, sasfit_param * param)
{
	scalar Pcs, Pprime;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T); // modify condition to your needs
	SASFIT_CHECK_COND1((R < 0.0), param, "R0(%lg) < 0",R); // modify condition to your needs

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
	subParam.p[0] = R;
	subParam.p[1] = 0;
	subParam.p[2] = fabs(SIGMA_R);

	Pprime = sasfit_sq_p__q___thin_disc(q,&subParam);
	return Pcs*Pprime;
}

scalar sasfit_ff_disc_sd_bilayergauss_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_disc_sd_bilayergauss_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

