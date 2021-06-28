/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define RHS	param->p[0]
#define FP	param->p[1]
#define TAU	param->p[2]

scalar sasfit_sq_sticky_hard_sphere(scalar q, sasfit_param * param)
{
	scalar SQ, CQ, kappa, gama, lamb, mu, epsi, beta, alpha;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RHS < 0.0), param, "RHS(%lg) < 0",RHS); // modify condition to your needs
	SASFIT_CHECK_COND1((FP < 0.0), param, "fp(%lg) < 0",FP); // modify condition to your needs
	SASFIT_CHECK_COND1((TAU < 0.0), param, "tau(%lg) < 0",TAU); // modify condition to your needs

	// insert your code here
	if (FP == 0.0)
	{
		return 1.0;
	}

	kappa = 2.0*q*RHS;
	epsi = TAU+(FP/(1.0-FP));
	gama = FP*(1+FP/2.0)/(3.0*pow(1.0-FP,2.0));

	SASFIT_CHECK_COND1((epsi*epsi-gama<0),param,"tau(%lg) is too small",TAU);
    if (epsi*epsi-gama>=0) {
        lamb = 6.0/FP*(epsi-sqrt(epsi*epsi-gama));
    } else {
        lamb = 6.0/FP*(epsi);
    }
	mu = lamb*FP*(1-FP);
	beta = -(3.*FP*pow(2.+FP,2.0)-2.*mu*(1.+7.*FP+FP*FP)+mu*mu*(2.+FP)) / (2.*pow(1.-FP,4.));
	alpha = pow(1.+2.*FP-mu,2.)/pow(1-FP,4.);
	CQ = -24.*FP*pow(kappa,-6.)*
			  ( alpha*pow(kappa,3.)*(sin(kappa)-kappa*cos(kappa))
			 +beta*pow(kappa,2.)*(2.*kappa*sin(kappa)-(pow(kappa,2.)-2.)*cos(kappa)-2.)
			 +0.5*FP*alpha*((4.*pow(kappa,3.)-24.*kappa)*sin(kappa)-(pow(kappa,4.)-12.*pow(kappa,2.)+24)*cos(kappa)+24)
			)
		-2*pow(FP*lamb,2.)*(1.-cos(kappa))*pow(kappa,-2.)
		+2.*FP*lamb/kappa*sin(kappa);
	SQ = 1. / (1.-CQ);

	return SQ;
}

scalar sasfit_sq_sticky_hard_sphere_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_sticky_hard_sphere_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

