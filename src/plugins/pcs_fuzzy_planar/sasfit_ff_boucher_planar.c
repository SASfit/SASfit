/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define T	param->p[0]
#define T12 (T/2.)
#define SIGMA_T	fabs(param->p[1])

#define ALPHA	param->p[3]

#define DETA	param->p[7]

#define Q	param->p[MAXPAR-1]
#define P	param->p[MAXPAR-2]

scalar boucher_planar(scalar x, sasfit_param * param) {
	return gsl_pow_int(DETA*sqrt(M_PI)*x*
                    gsl_sf_gamma(ALPHA/2.)/gsl_sf_gamma((ALPHA+1)/2.)
                   *gsl_sf_hyperg_0F1((ALPHA+1)/2.,-0.25*gsl_pow_2(Q*x)),lround(P));
}

scalar boucher_planar_core(scalar x, sasfit_param *param) {
	scalar Pcs, LNdistr;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	Pcs = boucher_planar(x,param);

	sasfit_init_param( &subParam );
	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_T;
	subParam.p[2] = 1.0;
	subParam.p[3] = T12;

	LNdistr = sasfit_sd_LogNorm(x, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return LNdistr*Pcs;
}

scalar sasfit_ff_boucher_planar(scalar q, sasfit_param * param)
{
    scalar Pcs, Tstart, Tend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA <= 0.0), param, "alpha(%lg) <= 0",ALPHA); // modify condition to your needs

	// insert your code here

	Q=q;
	P=2;
    if (SIGMA_T == 0.0) {
		Pcs=boucher_planar(T12,param);
	} else {
		find_LogNorm_int_range(2,T12,SIGMA_T,&Tstart,&Tend,param);
		Pcs 	= sasfit_integrate(Tstart, Tend, &boucher_planar_core, param);
	}
	return Pcs;
}

scalar sasfit_ff_boucher_planar_f(scalar q, sasfit_param * param)
{
    scalar Pcs, Tstart, Tend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here

	Q=q;
	P=1;
    if (SIGMA_T == 0.0) {
		Pcs=boucher_planar(T,param);
	} else {
		find_LogNorm_int_range(2,T12,SIGMA_T,&Tstart,&Tend,param);
		Pcs 	= sasfit_integrate(Tstart, Tend, &boucher_planar_core, param);
	}
	return Pcs;
}

scalar sasfit_ff_boucher_planar_v(scalar t, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return t;
}

