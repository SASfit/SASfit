/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define NU	param->p[0]
#define R_EQUATORIAL	param->p[1]
#define DUMMY2	param->p[2]
#define DUMMY3	param->p[3]
#define K	param->p[4]
#define ETA_CORE	param->p[5]
#define DUMMY6	param->p[6]
#define ETA_SOLV	param->p[7]

#define Q   param->p[MAXPAR-1]
#define A   param->p[MAXPAR-2]
#define B   param->p[MAXPAR-3]
#define T   param->p[MAXPAR-4]
#define THETA param->p[MAXPAR-5]

scalar sasfit_ff_spheroid_w_sz_distr_f_core(scalar y, void * pam) {
    scalar qttheta,QT, V0;
	sasfit_param *param;

	param = (sasfit_param *) pam;

    T = sqrt(fabs(1.0 + (NU*NU-1.0)*y*y));
    QT=Q*T;
    qttheta = QT*THETA;
    A = 1 + gsl_pow_2(2*qttheta);
    B = atan(qttheta);
    V0=gsl_pow_2(4./3.*M_PI*NU);
    if (fabs(qttheta*K)<=5e-2) {
        return V0*(K*(1 + K)*(2 + K)*gsl_pow_int(THETA,3)*(15120 + (3 + K)*(4 + K)*gsl_pow_int(qttheta,2)*
        (-1512 - (5 + K)*(6 + K)*gsl_pow_int(qttheta,2)*(-54 + (7 + K)*(8 + K)*gsl_pow_int(qttheta,2)))))/15120.;
    } else {
        return V0*(3*pow(A,-0.5 - K/2.)*gsl_pow_int(THETA,3)*(-(K*qttheta*cos(B*(1 + K))) + sqrt(A)*sin(B*K)))/gsl_pow_int(qttheta,3);
    }
}

scalar sasfit_ff_spheroid_w_sz_distr_core(scalar y, void * pam) {
    scalar qttheta,QT, V02;
	sasfit_param *param;

	param = (sasfit_param *) pam;
    T = sqrt(fabs(1.0 + (NU*NU-1.0)*y*y));
    QT=Q*T;
    qttheta = QT*THETA;
    A = 1 + gsl_pow_2(2*qttheta);
    B = atan(2*qttheta);
    V02=gsl_pow_2(4./3.*M_PI*NU);
    if (fabs(qttheta*K)<=5e-2) {
        return -V02*(K*(120 + 274*K + 225*gsl_pow_int(K,2) + 85*gsl_pow_int(K,3) + 15*gsl_pow_int(K,4) + gsl_pow_int(K,5))*gsl_pow_int(THETA,6)*
      (-4725 + 945*(42 + 13*K + gsl_pow_int(K,2))*gsl_pow_int(qttheta,2) -
        81*(3024 + 1650*K + 335*gsl_pow_int(K,2) + 30*gsl_pow_int(K,3) + gsl_pow_int(K,4))*gsl_pow_int(qttheta,4) +
        4*(332640 + 245004*K + 74524*gsl_pow_int(K,2) + 11985*gsl_pow_int(K,3) + 1075*gsl_pow_int(K,4) + 51*gsl_pow_int(K,5) + gsl_pow_int(K,6))*gsl_pow_int(qttheta,6)))/4725.;

//        return -(K*(120 + 274*K + 225*gsl_pow_2(K) + 85*gsl_pow_int(K,3) + 15*gsl_pow_int(K,4) + gsl_pow_int(K,5))*gsl_pow_int(THETA,6)*
//      (-5 + (42 + 13*K + gsl_pow_2(K))*gsl_pow_2(qttheta)))/5.;
    }
    return V02*(9*(1 - cos(K*atan(2*qttheta))/pow(1 + 4*gsl_pow_int(qttheta,2),K/2.) +
       K*(1 + K)*gsl_pow_int(qttheta,2)*(1 + pow(1 + 4*gsl_pow_int(qttheta,2),-1 - K/2.)*cos((2 + K)*atan(2*qttheta))) -
       2*K*qttheta*pow(1 + 4*gsl_pow_int(qttheta,2),-0.5 - K/2.)*sin((1 + K)*atan(2*qttheta))))/(2.*gsl_pow_int(Q*T,6));

//    return (9*(1 + K*(1 + K)*gsl_pow_int(qttheta,2) + pow(A,-1 - K/2.)*
//            (-(A*cos(B*K)) + K*qttheta*((1 + K)*qttheta*cos(B*(2 + K)) - 2*sqrt(A)*sin(B*(1 + K))))))/(2.*gsl_pow_int(Q*T,6));
}

scalar sasfit_ff_spheroid_w_sz_distr_f_sasfitint(scalar y, sasfit_param *pam) {
    return sasfit_ff_spheroid_w_sz_distr_f_core(y, pam);
}
scalar sasfit_ff_spheroid_w_sz_distr_sasfitint(scalar y, sasfit_param *pam) {
    return sasfit_ff_spheroid_w_sz_distr_core(y, pam);
}

scalar sasfit_ff_spheroid_w__sz_distr(scalar q, sasfit_param * param)
{
	scalar sum;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R_EQUATORIAL <= 0.0), param, "R_equatorial(%lg) <= 0",R_EQUATORIAL); // modify condition to your needs
	SASFIT_CHECK_COND1((K <= 1.0), param, "k(%lg) <= 1",K); // modify condition to your needs
	SASFIT_CHECK_COND1((NU <= 0.0), param, "nu(%lg) <= 0",NU); // modify condition to your needs

    THETA = R_EQUATORIAL/(K-1);

    Q=q;
	sum=sasfit_integrate(0.0, 1.0, &sasfit_ff_spheroid_w_sz_distr_sasfitint, param);
	return gsl_pow_2(ETA_CORE-ETA_SOLV)*sum;

	return gsl_pow_2(ETA_CORE-ETA_SOLV)*sasfit_integrate(0,1,&sasfit_ff_spheroid_w_sz_distr_sasfitint,param);
}

scalar sasfit_ff_spheroid_w__sz_distr_f(scalar q, sasfit_param * param)
{
	scalar sum;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
    THETA = R_EQUATORIAL/(K-1);
    Q=q;
	sum=sasfit_integrate(0.0, 1.0, &sasfit_ff_spheroid_w_sz_distr_f_sasfitint, param);
	return (ETA_CORE-ETA_SOLV)*sum;

}

scalar sasfit_ff_spheroid_w__sz_distr_v(scalar r, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 4./3.*M_PI*NU*r*r*r;
}

