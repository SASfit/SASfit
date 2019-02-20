/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
scalar g0(scalar s, scalar eta0) {
    return (1+0.5*eta0)/gsl_pow_2(1-eta0) - 4.5*eta0*(1+eta0)/gsl_pow_3(1-eta0)*(s-1);
}

scalar sasfit_sq_hard_sphere__gh_(scalar q, sasfit_param * param)
{
    scalar sigma, sigma0, eta0, etas, Rs, C,m,kappa,hkcorr;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA < 0.0), param, "eta(%lg) < 0",ETA); // modify condition to your needs

	// insert your code here
	sigma = 2*R;
	Rs=R;
	etas=ETA;
	sigma0=pow(1-ETA/16.,1./3.)*sigma;
	eta0 = ETA*(1.-ETA/16.);
	C=sigma*((2-ETA)/(2*gsl_pow_3(1-ETA))-g0(sigma/sigma0,eta0));
	m =   gsl_pow_4(1-ETA)/(1+4*ETA+4*ETA*ETA-4*gsl_pow_3(ETA)+gsl_pow_4(ETA))
	    - gsl_pow_4(1-eta0)/gsl_pow_2(1+2*eta0)
	    - 24*eta0*
	    (8 + 5*eta0*(1 + eta0) + (gsl_pow_3(sigma)*(-8 + eta0*(1 + eta0)*(-32 + (27*sigma)/sigma0)))/
      gsl_pow_3(sigma0))/(24.*gsl_pow_3(-1 + eta0));
	m=12*ETA*C/gsl_pow_2(sigma0) / m;
	kappa=q*sigma0;
	hkcorr=-4*M_PI*gsl_pow_3(sigma0)/kappa*
            ((-2*gsl_pow_2(kappa) + eta0*(1 + eta0)*(-18 + gsl_pow_2(kappa)))*cos(kappa) +
     ((2*gsl_pow_2(kappa)*sigma)/sigma0 +
        eta0*(1 + eta0)*(18 + (gsl_pow_2(kappa)*sigma*(-9*sigma + 8*sigma0))/gsl_pow_2(sigma0)))
       *cos((kappa*sigma)/sigma0) - 2*(-1 + 5*eta0*(1 + eta0))*kappa*sin(kappa) +
     2*kappa*(-1 + eta0*(1 + eta0)*(-4 + (9*sigma)/sigma0))*sin((kappa*sigma)/sigma0))/
   (2.*gsl_pow_3(-1 + eta0)*gsl_pow_3(kappa))
   +2*M_PI*gsl_pow_3(sigma)/(q*sigma)*C/sigma*
       ( cos(q*sigma)/sigma*((q+m)/(m*m+gsl_pow_2(q+m))+(q-m)/(m*m+gsl_pow_2(q-m)))
        +sin(q*sigma)/sigma*(  (m)/(m*m+gsl_pow_2(q+m))+  (m)/(m*m+gsl_pow_2(q-m)))
        );
    R=sigma0/2.;
    ETA=eta0;
    hkcorr = ETA*6/(M_PI*gsl_pow_3(sigma))*hkcorr+sasfit_sq_hard_sphere__py_(q,param);
    R=Rs;
    ETA=etas;
	return hkcorr;
}

scalar sasfit_sq_hard_sphere__gh__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_hard_sphere__gh__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

