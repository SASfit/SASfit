/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define RHS	param->p[0]
#define FP	param->p[1]
#define EPSI_KT	param->p[2]
#define DELTA	param->p[3]

scalar sph_jinc(scalar x) {
    if (fabs(x)<1e-4) {
        return 1/3-gsl_pow_2(x)/30.+gsl_pow_4(x)/840.-gsl_pow_6(x)/45360.;
    } else {
        return gsl_sf_bessel_j1(x)/x;
    }
}

scalar sasfit_sq_square_well_potential_2(scalar q, sasfit_param * param)
{
	scalar sigma,sigma3,kappa,kappa3,kb,rho,epsilon;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RHS < 0.0), param, "RHS(%lg) < 0",RHS); // modify condition to your needs
	SASFIT_CHECK_COND1((FP < 0.0), param, "fp(%lg) < 0",FP); // modify condition to your needs
	SASFIT_CHECK_COND1((EPSI_KT < 0.0), param, "epsi/kT(%lg) < 0",EPSI_KT); // modify condition to your needs
	SASFIT_CHECK_COND1((DELTA < 0.0), param, "Delta(%lg) < 0",DELTA); // modify condition to your needs

	// insert your code here
    epsilon = EPSI_KT;
	sigma = 2.*RHS;
	sigma3 = sigma*sigma*sigma;
	rho = FP*6./M_PI/sigma3;
	kappa = sigma*q;
	kappa3=kappa*kappa*kappa;
	return 1.-4.*M_PI*rho*sigma3*sph_jinc(kappa)
		  +4.*M_PI*rho*sigma*sigma*(exp(epsilon)-1.)*gsl_sf_bessel_j0(kappa)*DELTA;
}

scalar sasfit_sq_square_well_potential_2_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_square_well_potential_2_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

