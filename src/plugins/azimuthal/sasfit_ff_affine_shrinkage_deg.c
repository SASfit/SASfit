/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define B	param->p[1]
#define KAPPA	param->p[2]
#define DELTA	(param->p[3]*M_PI/180.)


scalar sasfit_ff_maiersaupe__deg_(scalar psi, sasfit_param * param)
{
    scalar norm,u;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	if (KAPPA < 0) {
		u = sqrt(-KAPPA);
		norm = sqrt(M_PI)*gsl_sf_erf(u)/u;
	}
	else if (KAPPA == 0.0) {
		norm = 2;
	}
	else {
		u = sqrt(KAPPA);
		norm = 2.0*exp(KAPPA)*gsl_sf_dawson(u)/u; // maple: `assuming`([convert(int(exp(ksppa*cos(psi)^2)*sin(psi), psi = 0 .. Pi), dawson)], [kappa > 0])
	}
	norm = exp(KAPPA/2.)*gsl_sf_bessel_I0(KAPPA/2.)/norm;
	return A+B*MaierSaupeODF(psi*M_PI/180-DELTA,KAPPA)/norm;
}

scalar sasfit_ff_maiersaupe__deg__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_maiersaupe__deg__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

