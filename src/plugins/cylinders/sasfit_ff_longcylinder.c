/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]

#define L	param->p[2]
#define ETA	param->p[3]

scalar sasfit_ff_longcylinder(scalar q, sasfit_param * param)
{
    scalar mu, sigma, pi_mu, G1, G2, I_sp, Sum, V, omega;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L); // modify condition to your needs

	// insert your code here
    if (R == 0.0) return 0.0;
	if (L == 0.0) return 0.0;

	mu = L*q;
	sigma = 2.0*R*q;
	V = M_PI*R*R*L;

	if (R==0 || L==0) return 0;
	if (q==0) return V*V*ETA*ETA;

	pi_mu = gsl_sf_Si(mu)+cos(mu)/mu+sin(mu)/mu/mu;
	G1 = 2.0/(0.5*sigma) *  gsl_sf_bessel_J1(0.5*sigma);
	G2 = 8.0/sigma/sigma * gsl_sf_bessel_Jn(2,sigma);
//	I_sp = 3.0 * (sin(sigma*0.5)-0.5*sigma*cos(0.5*sigma)) / pow(sigma/2.0,3);
//	I_sp = I_sp*I_sp;
	omega = 8/gsl_pow_2(sigma)*(3*gsl_sf_bessel_Jn(2,sigma)+gsl_sf_bessel_J0(sigma)-1);

//	Sum = 2.0/mu * (pi_mu*G1*G1 - 1.0/mu*(2.0*G2-I_sp) - sin(mu)/mu/mu);
	Sum = 2.0/mu * (pi_mu*G1*G1 - omega/mu - sin(mu)/mu/mu);

	return ETA*ETA *V*V* Sum;
}

scalar sasfit_ff_longcylinder_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_longcylinder_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return M_PI*gsl_pow_2(R)*L;
}

