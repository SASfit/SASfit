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

scalar sasfit_ff_flatcylinder(scalar q, sasfit_param * param)
{
    scalar mu,sigma,G1mu,G1sig,xi,I1sig,I0sig,I1mu,I0mu, Omega, Sum,V;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L); // modify condition to your needs

	SASFIT_CHECK_COND1((ETA < 0.0), param, "eta(%lg) < 0",ETA); // modify condition to your needs

	// insert your code here
    V = M_PI*R*R*L;
	if (V == 0.0) return 0.0;

	sigma = L*q;
	mu = 2.0*R*q;
	if (q==0) return V*V*ETA*ETA;

	G1sig = 2.0/sigma * gsl_sf_bessel_J1(sigma);
	G1mu  = 2.0/mu    * gsl_sf_bessel_J1(mu);

// Expand[Integrate[2*BesselJ[1, x]/x, {x, 0, t}, Assumptions -> t > 0]]
// 2 t BesselJ[0, t] -  2 BesselJ[1, t] + \[Pi] t BesselJ[1, t] StruveH[0, t] - \[Pi] t BesselJ[0, t] StruveH[1, t]

	I1mu  = 2.0*mu*gsl_sf_bessel_J0(mu)-2.0*gsl_sf_bessel_J1(mu)
	       - M_PI*mu*(  gsl_sf_bessel_J0(mu)*sasfit_struve_h1(&mu)
			      - gsl_sf_bessel_J1(mu)*sasfit_struve_h0(&mu)
					 );
	I1sig = 2.0*sigma*gsl_sf_bessel_J0(sigma)-2.0*gsl_sf_bessel_J1(sigma)
	       - M_PI*sigma*(  gsl_sf_bessel_J0(sigma)*sasfit_struve_h1(&sigma)
				 - gsl_sf_bessel_J1(sigma)*sasfit_struve_h0(&sigma)
						);
	I0sig = 0.5 * (I1sig + sigma * G1sig);
	I0mu  = 0.5 * (I1mu  + mu    * G1mu);

	Omega = 2.0/sigma*(I0sig-2.0*gsl_sf_bessel_J1(sigma));
	xi = pow(sin(sigma*0.5)/(0.5*sigma),2.0);

	Sum = 8.0/mu/mu*(xi+I1mu*Omega/mu-G1mu);
	return ETA*ETA *V*V* Sum;
}

scalar sasfit_ff_flatcylinder_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return M_PI*gsl_pow_2(R)*L;
}

scalar sasfit_ff_flatcylinder_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

