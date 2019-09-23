/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define T	param->p[1]
#define L	param->p[2]
#define ETA_CORE	param->p[3]
#define ETA_SHELL	param->p[4]
#define ETA_SOLV	param->p[5]

scalar sasfit_ff_longcylshell(scalar q, sasfit_param * param)
{
	scalar Pcs,Pprime;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L); // modify condition to your needs

	// insert your code here

    sasfit_init_param( &subParam );
	subParam.p[0] = R;
    subParam.p[1] = 0; //SIGMA_R0;
    subParam.p[2] =	1.0; // EPSILON;
    subParam.p[3] = T;
    subParam.p[4] = 0; //SIGMA_T0;

    subParam.p[7] = ETA_CORE;
    subParam.p[8] = ETA_SHELL;
    subParam.p[9] = ETA_SOLV;

	Pcs=sasfit_ff_pcs_ellcylsh(q,&subParam);

	subParam.p[0] = L;
	subParam.p[1] = 0.0;
	subParam.p[2] = 0; //SIGMA_L;
	Pprime = sasfit_sq_p__q___rod(q,&subParam);

	return Pcs*Pprime;
}

scalar sasfit_ff_longcylshell_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_longcylshell_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

