/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define RG	param->p[0]
#define DUMMY	param->p[1]
#define B_P	param->p[2]
#define MW	param->p[3]
#define RHO_P	param->p[4]
#define ETA_S	param->p[5]

scalar sasfit_ff_gauss3(scalar q, sasfit_param * param)
{
    scalar u,I0, na, beta, Vol;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((DUMMY < 0.0), param, "dummy(%lg) < 0",DUMMY); // modify condition to your needs
	SASFIT_CHECK_COND1((MW < 0.0), param, "Mw(%lg) < 0",MW); // modify condition to your needs
	SASFIT_CHECK_COND1((RHO_P < 0.0), param, "rho_p(%lg) < 0",RHO_P); // modify condition to your needs

	// insert your code here
	na 	= 6.0221415e23;
	Vol	= MW/RHO_P/na;
	beta	= B_P - Vol*ETA_S;
	I0=beta*beta;
	u = gsl_pow_2(q*RG);
	if (u < 1e-6) {
        return I0*(1-u/3.+gsl_pow_2(u)/12.-gsl_pow_3(u)/60.+gsl_pow_4(u)/360.);
    } else {
        return I0*2.0 * (u-1.0+exp(-u))/(u*u);
    }
}

scalar sasfit_ff_gauss3_f(scalar q, sasfit_param * param)
{
    scalar u,I0, na, beta, Vol;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	na 	= 6.0221415e23;
	Vol	= MW/RHO_P/na;
	beta	= B_P - Vol*ETA_S;
	I0=beta*beta;
	u = gsl_pow_2(q*RG);
	// insert your code here
	if (u < 1e-6) {
        return sqrt(I0)*(1-u/2.+u*u/6.-u*u*u/24.+u*u*u*u/120.);
    } else {
        return sqrt(I0)*(1-exp(u))/u;
    }
}

scalar sasfit_ff_gauss3_v(scalar q, sasfit_param * param, int dist)
{
    scalar rho;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	rho=8/3/M_SQRTPI;
	return 4/3.*M_PI*gsl_pow_3(RG/rho);
}

