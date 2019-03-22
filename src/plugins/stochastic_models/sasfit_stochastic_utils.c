/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

scalar gy1(scalar r, sasfit_param *param) {
    scalar r_xi;
    r_xi = r/XI;
    if (r==0) return 1;
    if (fabs(r_xi) > 300) {
        return exp(-gsl_sf_lncosh(r_xi))*gsl_sf_bessel_j0(2*M_PI*r/DD);
    }
    return 1.0/cosh(r_xi)*gsl_sf_bessel_j0(2*M_PI*r/DD);
}

scalar gy2(scalar r, sasfit_param *param) {
    if (r==0) return 1;
    return (exp(-r/XI) - RM/XI*exp(-r/RM))/(1 - RM/XI)*gsl_sf_bessel_j0(2*M_PI*r/DD);
}

scalar gy3(scalar r, sasfit_param *param) {
    scalar a;
    a=XI/RM;
    if (r==0) return 1;
    return a*(a + 1)/gsl_pow_2(a - 1)* (exp(-r/XI) + exp(-a*r/XI)/a -
                                        4*XI/(r*(a*a - 1))*
                                           (exp(-r/XI) - exp(-a*r/XI)))
                            *gsl_sf_bessel_j0(2*M_PI*r/DD);
}

scalar gy4(scalar r, sasfit_param *param) {
    scalar a2, b2, c2,c4, abc;
    a2=AA*AA;
    b2=BB*BB;
    c2=CC*CC;
    c4=c2*c2;
    abc = a2+c2-b2;
    if (r==0.0) return 1;
//    return 1./abc*(abc*gsl_sf_bessel_j0(AA*r)*exp(-BB*r)+2*BB/r*(exp(-CC*r)-exp(-BB*r)*cos(AA*r)));
    return 4*BB*CC*gsl_pow_2(a2+gsl_pow_2(BB+CC))/((BB+CC)*r)*
            (exp(-CC*r)*((a2-b2+c2)/gsl_pow_2(4*a2*b2+gsl_pow_2(a2-b2+c2))
                         +r/(4*CC*(gsl_pow_2(a2+b2)+2*(a2-b2)*c2+c4))
                        ) +
             exp(-BB*r)/(AA*BB)*( (-8*a2*b2+gsl_pow_2(a2+b2)+2*(a2-b2)*c2+c4)/(4*gsl_pow_2(4*a2*b2+gsl_pow_2(a2-b2+c2)))
                                 *sin(AA*r)-(AA*BB*(a2-b2+c2))/(gsl_pow_2(4*a2*b2+gsl_pow_2(a2-b2+c2)))*cos(AA*r)
                                 )
             );
}

scalar gy5(scalar r, sasfit_param *param) {
    scalar a2, b2, c2,c4, abc;
    a2=AA*AA;
    b2=BB*BB;
    c2=CC*CC;
    c4=c2*c2;
    abc = a2+c2-b2;
    if (r==0.0) return 1;
    return 1./(a2+gsl_pow_2(CC-BB))*(abc*gsl_sf_bessel_j0(AA*r)*exp(-BB*r)+2*BB/r*(exp(-CC*r)-exp(-BB*r)*cos(AA*r)));
}

scalar C11kernel(scalar x,sasfit_param *param) {
    return exp(-gsl_pow_2(ALPHA)/(1+sin(x)));
}

scalar gamaY1(scalar r, void *pam) {
    scalar gy;
    sasfit_param * param;
	param = (sasfit_param *) pam;
	gy = gy1(r,param);
	gy = (gy>1?1:gy);
	gy = (gy<-1?-1:gy);
    ASIN_GR=asin(gy);
    return gsl_sf_bessel_j0(Q*r)*4*M_PI*gsl_pow_2(r)*(sasfit_integrate(0,ASIN_GR,&C11kernel,param)/(2*M_PI)-gsl_pow_2(FP))/(FP*(1-FP));
}

scalar gamaY2(scalar r, void *pam) {
    scalar gy;
    sasfit_param * param;
	param = (sasfit_param *) pam;
	gy = gy2(r,param);
	gy = (gy>1?1:gy);
	gy = (gy<-1?-1:gy);
    ASIN_GR=asin(gy);
    return gsl_sf_bessel_j0(Q*r)*4*M_PI*gsl_pow_2(r)*(sasfit_integrate(0,ASIN_GR,&C11kernel,param)/(2*M_PI)-gsl_pow_2(FP))/(FP*(1-FP));
}

scalar gamaY3(scalar r, void *pam) {
    scalar gy;
    sasfit_param * param;
	param = (sasfit_param *) pam;
	gy = gy3(r,param);
	gy = (gy>1?1:gy);
	gy = (gy<-1?-1:gy);
    ASIN_GR=asin(gy);
    return gsl_sf_bessel_j0(Q*r)*4*M_PI*gsl_pow_2(r)*(sasfit_integrate(0,ASIN_GR,&C11kernel,param)/(2*M_PI)-gsl_pow_2(FP))/(FP*(1-FP));
}
scalar gamaY4(scalar r, void *pam) {
    scalar gy;
    sasfit_param * param;
	param = (sasfit_param *) pam;
	gy = gy4(r,param);
	gy = (gy>1?1:gy);
	gy = (gy<-1?-1:gy);
    ASIN_GR=asin(gy);
    return gsl_sf_bessel_j0(Q*r)*4*M_PI*gsl_pow_2(r)*(sasfit_integrate(0,ASIN_GR,&C11kernel,param)/(2*M_PI)-gsl_pow_2(FP))/(FP*(1-FP));
}
scalar gamaY5(scalar r, void *pam) {
    scalar gy;
    sasfit_param * param;
	param = (sasfit_param *) pam;
	gy = gy5(r,param);
	gy = (gy>1?1:gy);
	gy = (gy<-1?-1:gy);
    ASIN_GR=asin(gy);
    return gsl_sf_bessel_j0(Q*r)*4*M_PI*gsl_pow_2(r)*(sasfit_integrate(0,ASIN_GR,&C11kernel,param)/(2*M_PI)-gsl_pow_2(FP))/(FP*(1-FP));
}

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
