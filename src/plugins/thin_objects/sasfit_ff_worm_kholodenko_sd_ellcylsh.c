/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R0	param->p[0]
#define SIGMA_R0	fabs(param->p[1])
#define EPSILON	param->p[2]
#define T	param->p[3]
#define SIGMA_T	param->p[4]
#define LLB		param->p[5]
#define LL		param->p[6]
#define LEXV	param->p[7]
#define ETA_CORE	param->p[8]
#define ETA_SHELL	param->p[9]
#define ETA_SOLV	param->p[10]
#define R	param->p[MAXPAR-2]
#define Q	param->p[MAXPAR-1]



scalar sasfit_ff_worm_kholodenko_sd_ellcylsh(scalar q, sasfit_param * param)
{
	scalar Pcs,Pprime;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R0 < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((LL <= 0.0), param, "L(%lg) <= 0",LL); // modify condition to your needs
	SASFIT_CHECK_COND1((LLB <=0.0), param, "lb(%lg) <= 0",LLB); // modify condition to your needs
	SASFIT_CHECK_COND1((EPSILON <= 0.0), param, "epsilon(%lg) <= 0",EPSILON); // modify condition to your needs
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T); // modify condition to your needs

	// insert your code here

	Pcs=sasfit_ff_pcs_ellcylsh(q,param);

	sasfit_init_param( &subParam );
	subParam.p[0] = LLB;
	subParam.p[1] = LL;
	Pprime = sasfit_sq_p__q___kholodenko_worm(q,&subParam);

	return Pcs*Pprime;
}

scalar sasfit_ff_worm_kholodenko_sd_ellcylsh_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_worm_kholodenko_sd_ellcylsh_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

