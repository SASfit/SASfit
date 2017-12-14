/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define LL	param->p[0]
#define DUMMY	param->p[1]
#define SIGMA_L	fabs(param->p[2])

#define Q		param->p[MAXPAR-1]

scalar rod_core(scalar x, sasfit_param *param)
{
	scalar u,u2;
	scalar P, LNdistr;
	sasfit_param subParam;
	
	SASFIT_ASSERT_PTR(param);
	sasfit_init_param( &subParam );

	u = Q*x;
	u2 = u/2.0;
	if (u==0.0) {
		P = x*x;
	} else {
		P = x*x*(2.0*gsl_sf_Si(u)/u-gsl_pow_2(sin(u2)/u2) );
	}
	
	if (SIGMA_L == 0) return P;

	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_L;
	subParam.p[2] = 1.0;
	subParam.p[3] = LL;

	LNdistr = sasfit_sd_LogNorm(x, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return LNdistr*P;
}

scalar sasfit_sq_p__q___rod(scalar q, sasfit_param * param)
{
	scalar Lstart, Lend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((LL < 0.0), param, "L(%lg) < 0",LL); // modify condition to your needs
	SASFIT_CHECK_COND1((DUMMY < 0.0), param, "dummy(%lg) < 0",DUMMY); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA_L < 0.0), param, "sigma(%lg) < 0",SIGMA_L); // modify condition to your needs

	// insert your code here

	Q = q;
	if (SIGMA_L == 0) return rod_core(LL,param);
	find_LogNorm_int_range(1,LL,SIGMA_L,&Lstart,&Lend,param);
	return sasfit_integrate(Lstart,Lend,&rod_core,param);
}

scalar sasfit_sq_p__q___rod_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_p__q___rod_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return L*L;
}

