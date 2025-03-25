/*
 * Author(s) of this file:
 *   <Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>
#include <float.h>

// define shortcuts for local parameters/variables
#define ETA	param->p[0]
#define RC	param->p[1]
#define K	fabs(param->p[2])
#define RS	param->p[3]
#define SIGMA_S	fabs(param->p[4])
#define Q       param->p[MAXPAR-1]
#define ALPHA   param->p[MAXPAR-2]

scalar bbe_profile_PcsCyl(scalar t, sasfit_param *param) {
    scalar fcyl,rho,u,r,JD;
    r = t/(1-t);
    JD=1/(1-t)+t/gsl_pow_2(1-t);
	u=(Q*r);
	if (r<=RC) {
        rho = JD*ETA;
	} else {
	    rho = JD*ETA*ALPHA*pow(r,-K)/(1+exp((r-RS)/SIGMA_S));
	}
	fcyl = rho*2*M_PI*r*gsl_sf_bessel_J0(u);
	return fcyl;
}

scalar sasfit_ff_bottlebrush_emp(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RC <= 0.0), param, "Rc(%lg) <= 0",RC); // modify condition to your needs
	SASFIT_CHECK_COND1((K <= 0.0), param, "k(%lg) <= 0",K); // modify condition to your needs
	SASFIT_CHECK_COND1((RS <= 0.0), param, "Rs(%lg) <= 0",RS); // modify condition to your needs
	SASFIT_CHECK_COND2((RS <= RC), param, "Rs(%lg) <= RC(%lg)",RS,RC); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA_S <= 0.0), param, "sigma_s(%lg) <= 0",SIGMA_S); // modify condition to your needs

	// insert your code here
	return gsl_pow_2(sasfit_ff_bottlebrush__empirical__f(q,param));
}

scalar sasfit_ff_bottlebrush_emp_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	Q=q;
	ALPHA = (1+exp((RC-RS)/SIGMA_S))*pow(RC,K);
	//return sasfit_integrate(0,RS+5*SIGMA_S,&bbe_profile_PcsCyl,param);
	return sasfit_integrate(0,1-10*DBL_EPSILON,&bbe_profile_PcsCyl,param);
}

scalar sasfit_ff_bottlebrush_emp_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

