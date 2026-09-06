/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>
#include <math.h>
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_sf_hyperg.h>

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

double erf1(double x)
{
    double g = gsl_sf_gamma(0.25);

    double norm = 8.0 * sqrt(M_PI) / (g * g);

    double F = gsl_sf_hyperg_2F1(
                    -0.5,   /* a */
                     0.25,  /* b */
                     1.25,  /* c */
                    -pow(x,4));

    return norm * (x * F - x*x*x/3.0);
}

scalar erf_kappa(scalar x, sasfit_param * param) {
    if (KAPPA == 0) return gsl_sf_erf(x);
    if (KAPPA > 0 && KAPPA < 1)
        return (2+KAPPA)*sqrt(2*KAPPA/M_PI)*exp(gsl_sf_lngamma(0.5/KAPPA+0.25)-gsl_sf_lngamma(0.5/KAPPA-0.25))
                * sasfit_integrate(0,x,&erf_kernel,param);
    if (KAPPA==1) return erf1(x);
    // for kappa >=1 and kappa<2 there is a mathematical solution available but should not be relevant here.
    // Therefore NAN is returned.
    return NAN;
}

scalar mode_kappa(sasfit_param * param) {
    if ((1-gsl_pow_2(KAPPA*(NU-1./ALPHA))) < 0) return 2*XI;
    return XI+SIGMA*pow((NU-1./ALPHA)/(BETA*sqrt(1-gsl_pow_2(KAPPA*(NU-1./ALPHA)))),1./ALPHA);
}

scalar Z_kappa(sasfit_param * param) {
    return sqrt(2*BETA*KAPPA/M_PI)*(1+0.5*KAPPA)*exp(gsl_sf_lngamma(0.5/KAPPA+0.25)-gsl_sf_lngamma(0.5/KAPPA-0.25));
}

