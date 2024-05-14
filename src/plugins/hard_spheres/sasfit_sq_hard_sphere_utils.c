/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>

// define shortcuts for local parameters/variables
void set_params(sasfit_param * param) {
    scalar ETA2;
    ETA2=ETA*ETA;
    ZPY=(1+2*ETA+3*ETA*ETA)/gsl_pow_2(1-ETA);
    CHIPY=gsl_pow_4(1-ETA)/gsl_pow_2(1+2*ETA);
    S4 = (1-ETA)/(36*ETA*(ZHS-1./3.))
         *(1-sqrt(1+(ZHS-1./3.)/(ZHS-ZPY)*(CHI/CHIPY-1.)));
    L2 = -3*(ZHS-1)*S4;
    S3 = 1./12. * (2*ETA-ETA2+12*ETA2*L2-12*ETA*L2-1-72*ETA2*S4)/((2*ETA+1)*ETA);
    S2 = -0.5*(-ETA+8*ETA*L2+1-2*L2-24*ETA*S4)/(2*ETA+1);
    S1 = 1.5*ETA*(-1+4*L2-8*S4)/(2*ETA+1);
    L1 = 0.5*(ETA+12*ETA*L2+2-24*ETA*S4)/(2*ETA+1);
}

gsl_complex PHI(gsl_complex t,sasfit_param * param) {
    gsl_complex t2, t3, t4, res, resD, resN;
    t2 = gsl_complex_pow_real(t,2.);
    t3 = gsl_complex_pow_real(t,3.);
    t4 = gsl_complex_pow_real(t,4.);
    resD = gsl_complex_rect(1,0);
    resN= gsl_complex_rect(1,0);
    res=gsl_complex_mul_real(t,S1);
    resN=gsl_complex_add(resN,res);
    res=gsl_complex_mul_real(t2,S2);
    resN=gsl_complex_add(resN,res);
    res=gsl_complex_mul_real(t3,S3);
    resN=gsl_complex_add(resN,res);
    res=gsl_complex_mul_real(t4,S4);
    resN=gsl_complex_add(resN,res);

    res=gsl_complex_mul_real(t,L1);
    resD=gsl_complex_add(resD,res);
    res=gsl_complex_mul_real(t2,L2);
    resD=gsl_complex_add(resD,res);

    return gsl_complex_div(resN,resD);
}

gsl_complex G(gsl_complex t,sasfit_param * param) {
    gsl_complex res;
    res=gsl_complex_mul(gsl_complex_exp(t),PHI(t,param));
    res=gsl_complex_add_real(gsl_complex_negative(res),1);
    res=gsl_complex_div(t,res);
    return gsl_complex_div_real(res,12.0*ETA);
}

scalar ReGfunc(scalar q, sasfit_param * param){
    gsl_complex t,t2,t3, res;
    t=gsl_complex_rect(0,q*2*R);
    t2 = gsl_complex_pow_real(t,2.);
    t3 = gsl_complex_pow_real(t,3.);
    res=gsl_complex_mul(t2,G(t,param));
    res=gsl_complex_add_real(res,-1);
    res=gsl_complex_div(res,t3);
    return GSL_REAL(res);
}
