/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define K	param->p[0]
#define HI	param->p[1]
#define L_H	param->p[2]
#define R   fabs(param->p[3])
#define lR  param->p[MAXPAR-1]

scalar Cr_kernel(scalar q, sasfit_param * param) {
    return K*gsl_pow_4(R)/gsl_pow_2(HI)*gsl_sf_bessel_j0(q*lR)*gsl_pow_2(gsl_sf_bessel_j1(q*fabs(R)))/gsl_pow_2(1.0+gsl_pow_2(L_H*q));
}

scalar sasfit_ff_c_r__for_spin_misalignment(scalar r, sasfit_param * param)
{
    scalar lh,Cr, Cr0,C2RLr, C2RGr, C2REr, euler;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((r < 0.0), param, "r(%lg) < 0",r);
	SASFIT_CHECK_COND1((K < 0.0), param, "K(%lg) <= 0",K); // modify condition to your needs
	SASFIT_CHECK_COND1((HI < 0.0), param, "Hi(%lg) <= 0",HI); // modify condition to your needs
//	SASFIT_CHECK_COND1((L_H < 0.0), param, "l_H(%lg) < 0",L_H); // modify condition to your needs
//	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) <= 0",R); // modify condition to your needs

	// insert your code here
	lR = r;
	lh = L_H;
if (R==0) return 0;
//if (R>0) {
euler = M_E;
if (fabs(r)<=1e-5) {
//	   Cr = exp(-2/lh*R)*pow(R,-4)*(exp(2/lh
//*R)*(4*M_PI*pow(R,3)-9*lh*M_PI*pow(R,2)+15*pow(lh,3)*M_PI)-6*M_PI
//*pow(R,3)-21*lh*M_PI*pow(R,2)-30*pow(lh,2)*M_PI*R-15*pow(lh,3)*M_PI)/24.0;
Cr = exp(-2*pow(lh,-1)*R)*pow(lh,-4)*pow(R,-4)*(exp(2*pow(lh,-1)*R)
*(160*pow(lh,4)*M_PI*pow(R,3)+(lh*pow(r,4)*M_PI-
20*pow(lh,3)*pow(r,2)*M_PI-360*pow(lh,5)*M_PI)*pow(R,2)+pow(lh,3)
*pow(r,4)*M_PI+60*pow(lh,5)*pow(r,2)*M_PI+600*pow(lh,7)*M_PI)+(-2
*pow(r,4)*M_PI-40*pow(lh,2)*pow(r,2)*M_PI-240*pow(lh,4)*M_PI)*pow(R,3)
+(-3*lh*pow(r,4)*M_PI-100*pow(lh,3)*pow(r,2)*M_PI-840*pow(lh,5)
*M_PI)*pow(R,2)+(-2*pow(lh,2)*pow(r,4)*M_PI-120*pow(lh,4)*pow(r,2)
*M_PI-1200*pow(lh,6)*M_PI)*R-pow(lh,3)*pow(r,4)*M_PI-60*pow(lh,5)
*pow(r,2)*M_PI-600*pow(lh,7)*M_PI)/960.0;
} else if (2*R > r) {Cr = exp(-2/lh*R-r/lh)*pow(r,-1)*pow(R,-4)
    *(exp(2/lh*R)*(16*exp(r/lh)*r*M_PI*
pow(R,3)+(exp(r/lh)*(-12*pow(r,2)*M_PI-48*pow(lh,2)
*M_PI)+12*lh*r*M_PI+48*pow(lh,2)*M_PI)*pow(R,2)+exp(pow(lh,
-1)*r)*(pow(r,4)*M_PI+24*pow(lh,2)*pow(r,2)*M_PI+72*pow(lh,4)*M_PI)
-12*pow(lh,3)*r*M_PI-72*pow(lh,4)*M_PI)+(12*lh*M_PI-12*exp(
2/lh*r)*lh*M_PI)*pow(R,3)+(exp(2/lh*r)*(6
*lh*r*M_PI-48*pow(lh,2)*M_PI)+6*lh*r*M_PI+48*pow(lh,2)*M_PI)*pow(
R,2)+(exp(2/lh*r)*(12*pow(lh,2)*r*M_PI-72*pow(lh,3)
*M_PI)+12*pow(lh,2)*r*M_PI+72*pow(lh,3)*M_PI)*R+exp(2/lh*r)
*(6*pow(lh,3)*r*M_PI-36*pow(lh,4)*M_PI)+6*pow(lh,3)*r*M_PI
+36*pow(lh,4)*M_PI)/96.0;}
else if (2*R < r) {Cr = -exp(-2/lh*R-r/lh)*pow(r,-1)*pow(R,
-4)*(exp(4/lh*R)*(2*lh*M_PI*pow(R,3)+(-lh*r*M_PI-8*
pow(lh,2)*M_PI)*pow(R,2)+(2*pow(lh,2)*r*M_PI+12*pow(lh,3)*M_PI)*R
-pow(lh,3)*r*M_PI-6*pow(lh,4)*M_PI)-2*lh*M_PI*pow(R,3)+exp(
2/lh*R)*((-2*lh*r*M_PI-8*pow(lh,2)*M_PI)*pow(R,2)+2*pow(lh,
3)*r*M_PI+12*pow(lh,4)*M_PI)+(-lh*r*M_PI-8*pow(lh,2)*M_PI)*pow(
R,2)+(-2*pow(lh,2)*r*M_PI-12*pow(lh,3)*M_PI)*R-pow(lh,3)*r*M_PI-6
*pow(lh,4)*M_PI)/16.0;}
else if (2*R == r) {Cr = exp(-2/lh*r)*pow(r,-5)*(exp(r/lh)
*(lh*pow(r,3)*M_PI+4*pow(lh,2)*pow(r,2)*M_PI-4*pow(lh,3)*r*M_PI
-24*pow(lh,4)*M_PI)+exp(2/lh*r)*(2*pow(lh,2)*pow(r,2)
*M_PI-10*pow(lh,3)*r*M_PI+12*pow(lh,4)*M_PI)+lh*pow(r,3)*M_PI
+6*pow(lh,2)*pow(r,2)*M_PI+14*pow(lh,3)*r*M_PI+12*pow(lh,4)*M_PI)
/2.0;} else {Cr = 0;}

return K*gsl_pow_4(R)/gsl_pow_2(HI)*Cr;
//} else {
//
//	return sasfit_integrate(0.0,GSL_POSINF,&Cr_kernel, param);
//}
}

scalar sasfit_ff_c_r__for_spin_misalignment_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_c_r__for_spin_misalignment_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

