/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

double ThetaK(scalar r, sasfit_param * param) {
    scalar sigma2, mu2, mu3,mu2_r,logfp;
    sigma2=SIGMA*SIGMA;
    mu2=MU*MU;
    mu3=mu2*MU;
    mu2_r=2*MU/r;
    logfp=log(FP);
    if (r/MU < 1e-10) return -logfp + (3*r*logfp)/(4.*exp((5*sigma2)/2.)*MU);

    return -((gsl_pow_3(r)*(1 + gsl_sf_erf(log(mu2_r)/(sqrt(2)*SIGMA))) -
        12*exp(2*sigma2)*mu2*r*(1 + gsl_sf_erf((2*sigma2 + log(mu2_r))/(sqrt(2)*SIGMA))) +
        16*exp((9*sigma2)/2.)*mu3*(1 + gsl_sf_erf((3*sigma2 + log(mu2_r))/(sqrt(2)*SIGMA))))*
      logfp)/(32.*exp((9*sigma2)/2.)*mu3);
}

scalar gammaBoolean(scalar r, void *pam){
    sasfit_param * param;
	param = (sasfit_param *) pam;
    return 4*M_PI*r*r*gsl_sf_bessel_j0(Q*r)*(exp(ThetaK(r,param)-1.0)/(1.0/FP-1.0));
}

scalar gammaDeadLeaves(scalar r, void *pam){
    sasfit_param * param;
    scalar Kr, K0;
	param = (sasfit_param *) pam;
    FP=0.5;
	Kr = ThetaK(r,param);
	K0 = ThetaK(0,param);
    return 4*M_PI*r*r*gsl_sf_bessel_j0(Q*r)*Kr/(2*K0-Kr);
}
