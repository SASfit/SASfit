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
#define ALPHA   param->p[MAXPAR-1]

scalar sasfit_ff_bottlebrush_profile_emp(scalar r, sasfit_param * param)
{
	scalar rho;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((RC <= 0.0), param, "Rc(%lg) <= 0",RC); // modify condition to your needs
	SASFIT_CHECK_COND1((K <= 0.0), param, "k(%lg) <= 0",K); // modify condition to your needs
	SASFIT_CHECK_COND1((RS <= 0.0), param, "Rs(%lg) <= 0",RS); // modify condition to your needs
	SASFIT_CHECK_COND2((RS <= RC), param, "Rs(%lg) <= RC(%lg)",RS,RC); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA_S <= 0.0), param, "sigma_s(%lg) <= 0",SIGMA_S); // modify condition to your needs

	// insert your code here
	ALPHA = (1+exp((RC-RS)/SIGMA_S))*pow(RC,K);
	if (fabs(r)<=RC) {
        rho = ETA;
	} else {
	    rho = ETA*ALPHA*pow(fabs(r),-K)/(1+exp((fabs(r)-RS)/SIGMA_S));
	}
	return rho;
}

scalar sasfit_ff_bottlebrush_profile_emp_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0;
}

scalar sasfit_ff_bottlebrush_profile_emp_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

