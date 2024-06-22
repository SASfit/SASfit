/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   Ingo Bressler (ingo.bressler@bam.de)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

scalar QY_Uniform(scalar p) {
    return p;
}

scalar QY_Cauchy(scalar p) {
    return tan(M_PI*(p-0.5));
}

scalar QY_Weibull(scalar p, scalar alpha, scalar beta) {
    return beta*pow(-log(1-p),1/alpha);
}

scalar QY_Exponential(scalar p, scalar b) {
    // b > 0
    return -b*log(1-p);
}

scalar QY_LogLogistic(scalar p, scalar a, scalar b) {
    // a,b > 0
    return a*pow(p/(1-p),1./b);
}

scalar QY_Logistic(scalar p, scalar a, scalar b) {
    // a,b > 0
    return a+b*log(p/(1-p));
}

scalar QY_ExtremeValue(scalar p) {
    return log(-log(1-p));
}

scalar QY_Frechet(scalar p) {
    return -1/log(1-p);
}

scalar KWm1(scalar Z, scalar theta) {
    return -(1+theta)/theta-1/theta*gsl_sf_lambert_Wm1(-Z*(theta+1)*exp(-(theta+1)));
}

scalar Qx (scalar y, sasfit_func_one_t *Fy, sasfit_func_one_t *QT, sasfit_param *param) {
    return KWm1(1-(*Fy)((*QT)(y,param),param),THETA);
}

scalar QxWLE(scalar y, sasfit_param *param) {
    return -(1+THETA)/THETA-1/THETA*gsl_sf_lambert_Wm1(-(THETA+1)*exp(-((THETA+1)+pow(-log(1-y),1/ALPHA)/BETA)));
}

scalar Lindley_f_R(scalar x, scalar theta) {
	if (x<0) return 0;
	return theta*theta/(1+theta)*(1+x)*exp(-theta*x);
}

scalar Lindley_F_R(scalar x, scalar theta) {
	if (x<=0) return 0;
	return 1-(1+theta*x/(theta+1))*exp(-theta*x);
}
