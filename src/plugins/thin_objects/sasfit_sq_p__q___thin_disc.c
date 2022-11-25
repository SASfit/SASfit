/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]

#define SIGMA	fabs(param->p[2])


scalar disc_core(scalar x, sasfit_param * param)
{
	scalar u;
	scalar IP, LNdistr;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);
	sasfit_init_param( &subParam );

	SASFIT_ASSERT_PTR(param);

	u = Q*x;
	/*
	if (u == 0.0) {
		IP = gsl_pow_4(x)*gsl_pow_2(M_PI);
	} else {
	  IP = 2.0*gsl_pow_2(M_PI*x/Q)*(1.0-2*gsl_sf_bessel_J1(2.0*u)/(2*u));
	}
	*/
	IP= 2.0*gsl_pow_2(M_PI*x/Q)*(1.0-gsl_sf_hyperg_0F1(2,-0.25*gsl_pow_2(2*u)));
	if (SIGMA == 0) return IP;

	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA;
	subParam.p[2] = 1.0;
	subParam.p[3] = R;

	LNdistr = sasfit_sd_LogNorm(x, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return LNdistr*IP;
}


scalar sasfit_sq_p__q___thin_disc(scalar q, sasfit_param * param)
{
	scalar Rstart, Rend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs

	SASFIT_CHECK_COND1((SIGMA < 0.0), param, "sigma(%lg) < 0",SIGMA); // modify condition to your needs

	// insert your code here
	Q = q;
	if (SIGMA == 0) return disc_core(R,param);
	find_LogNorm_int_range(2,R,SIGMA,&Rstart,&Rend,param);
	return sasfit_integrate(Rstart,Rend,&disc_core,param);}

scalar sasfit_sq_p__q___thin_disc_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_p__q___thin_disc_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

