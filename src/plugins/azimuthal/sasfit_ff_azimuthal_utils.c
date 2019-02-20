/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
scalar MaierSaupeODF(scalar psi, scalar kappa) {
// Maier-Saupe orientation distribution
scalar u, norm;
	if (kappa < 0) {
		u = sqrt(-kappa);
		norm = sqrt(M_PI)*gsl_sf_erf(u)/u;
	}
	else if (kappa == 0.0) {
		norm = 2;
	}
	else {
		u = sqrt(kappa);
		norm = 2.0*exp(kappa)*gsl_sf_dawson(u)/u; // maple: `assuming`([convert(int(exp(ksppa*cos(psi)^2)*sin(psi), psi = 0 .. Pi), dawson)], [kappa > 0])
	}
    return exp(kappa*gsl_pow_2(cos(psi)))/norm;
}


scalar psi_quarter_rad(scalar psi) {
    scalar tp,tf, ti;
    ti = modf(fabs(psi/M_PI),&tf)*M_PI;
    tf = tf*M_PI;
    if (tf <= M_PI_2) {
        return tf;
    } else {
        return Pi-tf;
    }
}

scalar psi_quarter_deg(scalar psi) {
    scalar tp,tf, ti;
    ti = modf(fabs(psi/180.),&tf)*180.;
    tf = tf*180.;
    if (tf <= 90.) {
        return tf;
    } else {
        return 180.0-tf;
    }
}
