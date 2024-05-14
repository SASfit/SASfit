/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define RHS	param->p[0]
#define E_KT	param->p[1]
#define PHI	param->p[2]
#define FP	param->p[2]
#define EPSILON	param->p[3]

scalar f1_TSW(scalar x) {
	return (1.0-cos(x))/gsl_pow_2(x);
}

scalar f2_TSW(scalar x) {
	return (sin(x)-x*cos(x))/gsl_pow_3(x);
}
scalar f3_TSW(scalar x) {
	return (2.*x*sin(x)-(x*x-2.0)*cos(x)-2.0)/gsl_pow_4(x);
}
scalar f5_TSW(scalar x) {
	return ((4.*x*x*x-24.*x)*sin(x) -
		    (x*x*x*x-12.*x*x+24.)*cos(x) +
			24.
			)/gsl_pow_6(x);
}

scalar sasfit_sq_thin_square_well_potential(scalar q, sasfit_param * param)
{
	scalar sigma, kappa,  alpha, beta, mu, Gamma, Delta, tau,
		lambda, Ck;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RHS < 0.0), param, "RHS(%lg) < 0",RHS); // modify condition to your needs
	SASFIT_CHECK_COND1((FP < 0.0), param, "fp(%lg) < 0",FP); // modify condition to your needs
	SASFIT_CHECK_COND1((EPSILON < 0.0), param, "epsilon(%lg) < 0",EPSILON); // modify condition to your needs

	// insert your code here
		sigma = 2.0*RHS/(1.0-EPSILON);
	kappa = q*sigma;

	Gamma = PHI*(1.0+PHI/2.0)/(3.0*gsl_pow_2(1.0-PHI));
	tau = exp(-E_KT)/(12.0*EPSILON);
	Delta = tau + PHI/(1.0-PHI);
	lambda = 6.0*(Delta-sqrt(Delta*Delta-Gamma))/PHI;
	mu = lambda*PHI*(1.0-PHI);

	alpha = gsl_pow_2(1.0+2.0*PHI-mu)/gsl_pow_4(1.0-PHI);
	beta  = -(3.0*PHI*gsl_pow_2(2.0+PHI)
		    -2.0*mu*(1.0+7.0*PHI+PHI*PHI)
			+mu*mu*(2.0+PHI))
		/(2.0*gsl_pow_4(1.0-PHI));

	Ck = 24.0*PHI*(  alpha*f2_TSW(kappa)
		           + beta*f3_TSW(kappa)
				   + 0.5*PHI*alpha*f5_TSW(kappa))
		+gsl_pow_2(2.0*PHI*lambda*EPSILON)*(    f2_TSW(EPSILON*kappa)
		                                   -0.5*f3_TSW(EPSILON*kappa))
		+2.0*gsl_pow_2(PHI*lambda)*(f1_TSW(kappa)-
		                            gsl_pow_2(EPSILON)*f1_TSW(EPSILON*kappa))
		-2.0*PHI*lambda/EPSILON*
			( f1_TSW(kappa)
			 -gsl_pow_2(1.0-EPSILON)*f1_TSW((1.0-EPSILON)*kappa)
			)
		-24.0*PHI*
			( f2_TSW(kappa)
			 -gsl_pow_3(1.0-EPSILON)*f2_TSW((1.0-EPSILON)*kappa)
			);
	return 1.0/(1.0+Ck);
}

scalar sasfit_sq_thin_square_well_potential_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_thin_square_well_potential_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

