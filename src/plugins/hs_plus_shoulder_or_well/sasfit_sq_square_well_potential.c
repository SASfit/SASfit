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
#define LAMBDA	param->p[3]


scalar sasfit_sq_square_well_potential(scalar q, sasfit_param * param)
{
	scalar sigma, kappa, lkappa, alpha, beta, gamma,Ck;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RHS < 0.0), param, "RHS(%lg) < 0",RHS); // modify condition to your needs
	SASFIT_CHECK_COND1((FP < 0.0), param, "fp(%lg) < 0",FP); // modify condition to your needs
	SASFIT_CHECK_COND1((FP >= 1.0), param, "fp(%lg) >= 1",FP); // modify condition to your needs
	SASFIT_CHECK_COND1((LAMBDA <= 1.0), param, "lambda(%lg) <= 1",LAMBDA); // modify condition to your needs

	// insert your code here
	sigma = 2*RHS;
	alpha = (gsl_pow_2(1.0+2.0*FP)+gsl_pow_3(FP)*(FP-4.0))/gsl_pow_4(1-FP);
	beta  = -FP/3.0*(18.0+20.0*FP-12.0*gsl_pow_2(FP)+gsl_pow_4(FP))/gsl_pow_4(1-FP);
	gamma = FP*0.5*alpha;

	kappa = q*sigma;
	lkappa = kappa*LAMBDA;

	Ck = -24.0*FP/gsl_pow_6(kappa) *(
			alpha*gsl_pow_3(kappa)*(sin(kappa)-kappa*cos(kappa))
		   +beta*gsl_pow_2(kappa)*(2.0*kappa*sin(kappa)-(gsl_pow_2(kappa)-2.0)*cos(kappa)-2.0)
		   +gamma*(  (4*gsl_pow_3(kappa)-24.0*kappa)*sin(kappa)
		            -(gsl_pow_4(kappa)-12.0*gsl_pow_2(kappa)+24.0)*cos(kappa)+24.0)
		   -EPSI_KT*gsl_pow_3(kappa)*(sin(lkappa)-lkappa*cos(lkappa)
		                  +kappa*cos(kappa)-sin(kappa))
		);
	return 1.0/(1.0-Ck);
}

scalar sasfit_sq_square_well_potential_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_square_well_potential_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

