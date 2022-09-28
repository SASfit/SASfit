/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

scalar exp_kappa(scalar x, sasfit_param * param) {
    if (KAPPA == 0) return exp(x);
    if (KAPPA > 0 && KAPPA < 1) return pow(sqrt(1+gsl_pow_2(KAPPA*x))+KAPPA*x,1./KAPPA);
    return exp(gsl_asinh(KAPPA*x)/KAPPA);
}

scalar ln_kappa(scalar x, sasfit_param * param) {
    if (KAPPA == 0) return log(x);
    if (KAPPA > 0 && KAPPA < 1) return (pow(x,KAPPA)-pow(x,-KAPPA))/(2*KAPPA);
    return sinh(KAPPA*log(x))/KAPPA;
}

scalar erf_kernel(scalar x, sasfit_param * param) {
    return exp_kappa(-x*x,param);
}

scalar erf_kappa(scalar x, sasfit_param * param) {
    if (KAPPA == 0) return gsl_sf_erf(x);
    if (KAPPA > 0 && KAPPA < 1)
        return (2+KAPPA)*sqrt(2*KAPPA/M_PI)*exp(gsl_sf_lngamma(0.5/KAPPA+0.25)-gsl_sf_lngamma(0.5/KAPPA-0.25))
                * sasfit_integrate(0,x,&erf_kernel,param);
}

scalar mode_kappa(sasfit_param * param) {
    if ((1-gsl_pow_2(KAPPA*(NU-1./ALPHA))) < 0) return 2*XI;
    return XI+SIGMA*pow((NU-1./ALPHA)/(BETA*sqrt(1-gsl_pow_2(KAPPA*(NU-1./ALPHA)))),1./ALPHA);
}

scalar Z_kappa(sasfit_param * param) {
    return sqrt(2*BETA*KAPPA/M_PI)*(1+0.5*KAPPA)*exp(gsl_sf_lngamma(0.5/KAPPA+0.25)-gsl_sf_lngamma(0.5/KAPPA-0.25));
}

