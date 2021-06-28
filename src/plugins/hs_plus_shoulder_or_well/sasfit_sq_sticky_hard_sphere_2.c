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
#define DELTA	param->p[3]

scalar sasfit_sq_sticky_hard_sphere_2(scalar q, sasfit_param * param)
{
	scalar A,B,C,
		   SQ,kappa,
		   l1,l2,ltmp,lambda,
		   alpha,beta,
		   mu,phi,sigma,
		   I0,I1,I2,
		   J0,J1,J2;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RHS < 0.0), param, "RHS(%lg) < 0",RHS); // modify condition to your needs
	SASFIT_CHECK_COND1((FP < 0.0), param, "fp(%lg) < 0",FP); // modify condition to your needs
	SASFIT_CHECK_COND1((TAU < 0.0), param, "tau(%lg) < 0",TAU); // modify condition to your needs
	SASFIT_CHECK_COND1((DELTA < 0.0), param, "Delta(%lg) < 0",DELTA); // modify condition to your needs

	// insert your code here
	if (FP == 0.0)
	{
		return 1.0;
	}

	sigma = 2.0*RHS+DELTA;
	kappa = q*sigma;
	phi = FP*pow(sigma/(2*RHS),3.0);

	ltmp = 6.0*(TAU/phi+1.0/(1.0-phi));
	SASFIT_CHECK_COND1((gsl_pow_2(ltmp) - 12.0/phi*(1.0+0.5*phi)/gsl_pow_2(1-phi) < 0),param,"tau(%lg) is too small",TAU);
if (gsl_pow_2(ltmp) - 12.0/phi*(1.0+0.5*phi)/gsl_pow_2(1-phi) >= 0) {
	l1 = ltmp + sqrt(gsl_pow_2(ltmp) - 12.0/phi*(1.0+0.5*phi)/gsl_pow_2(1-phi));
	l2 = ltmp - sqrt(gsl_pow_2(ltmp) - 12.0/phi*(1.0+0.5*phi)/gsl_pow_2(1-phi));
} else {
    l1 = ltmp;
    l2 = ltmp;
}
	if (fabs(l1) < fabs(l2))
	{
		lambda = l1;
	} else {
		lambda = l2;
	}

	mu=lambda*phi*(1.0-phi);

	A = 0.5*(1.0+2.0*phi-mu)/gsl_pow_2(1.0-phi);
	B = 0.5*sigma*(mu-3.0*phi)/gsl_pow_2(1-phi);
	C = -A*gsl_pow_2(sigma) -B*sigma+lambda*gsl_pow_2(sigma)/12.0;

	I0 = sin(kappa)/kappa;
	I1 = (cos(kappa)+kappa*sin(kappa)-1.0)/gsl_pow_2(kappa);
	I2 = (gsl_pow_2(kappa)*sin(kappa)-2.0*sin(kappa)+2.0*kappa*cos(kappa))/gsl_pow_3(kappa);

	J0 = (1-cos(kappa))/kappa;
	J1 = (sin(kappa)-kappa*cos(kappa))/gsl_pow_2(kappa);
	J2 = (2.0*sin(kappa)*kappa+2.0*cos(kappa)-gsl_pow_2(kappa)*cos(kappa)-2.0)/gsl_pow_3(kappa);
	SQ = 1.0/(A*A+B*B);

	alpha = 1.0-12.0*phi*(C/gsl_pow_2(sigma)*I0+B/sigma*I1+A*I2);
	beta  =     12.0*phi*(C/gsl_pow_2(sigma)*J0+B/sigma*J1+A*J2);

	SQ = 1.0/(alpha*alpha+beta*beta);
	return SQ;
}

scalar sasfit_sq_sticky_hard_sphere_2_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_sticky_hard_sphere_2_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

