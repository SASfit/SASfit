/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */


#include "include/private.h"
#include <sasfit_error_ff.h>


scalar n_plus(scalar p_pol, scalar epsilon) 
{
	return epsilon*(1.0-p_pol)/2.0 + (1.0-epsilon)*(1.0+p_pol)/2.0;
}

scalar n_minus(scalar p_pol, scalar epsilon) 
{
	return epsilon*(1.0+p_pol)/2.0 + (1.0-epsilon)*(1.0-p_pol)/2.0;
}

scalar n(scalar x, scalar p_pol, scalar epsilon) 
{
	if (x > 0) return n_plus(p_pol,epsilon);
	return n_minus(p_pol,epsilon);
}
scalar A(scalar x, sasfit_param *param) 
{
	if (x > 0) return T_PLUS;
	return T_MINUS;
}

scalar T_out(scalar x, scalar p_out, sasfit_param *param) {
	if (x > 0) {
		return A(p_out, param);
	} else {
		return A(-p_out, param);
	}
}

scalar A_PLUS( sasfit_param *param, scalar *rad, scalar *sad)
{
	return rad[0]*A(sad[0],param)+rad[1]*A(sad[1],param);
}

scalar A_MINUS( sasfit_param *param, scalar *rad, scalar *sad)
{
	return rad[0]*A(-sad[0],param)+rad[1]*A(-sad[1],param);
}

scalar A_plus( sasfit_param *param, scalar *rad, scalar *sad) 
{
	return   ((1.0+sad[0])/2.0*rad[0]+(1.0+sad[1])/2.0*rad[1])*T_PLUS
			+((1.0-sad[0])/2.0*rad[0]+(1.0-sad[1])/2.0*rad[1])*T_MINUS;
}

scalar A_minus( sasfit_param *param, scalar *rad, scalar *sad) 
{
	return   ((1.0+sad[1])/2.0*rad[0]+(1.0+sad[0])/2.0*rad[1])*T_MINUS
			+((1.0-sad[1])/2.0*rad[0]+(1.0-sad[0])/2.0*rad[1])*T_PLUS;
}

void r_adiabatic(scalar a_ad, scalar sign_adiab, scalar sign_nonadiab, scalar *rad, scalar *sad) 
{
//	rad[0] = a_ad*GSL_SIGN(sign_adiab);
//	rad[1] = (1.0-a_ad)*GSL_SIGN(sign_nonadiab);
	
	rad[0] = a_ad;
	rad[1] = (1.0-a_ad);
	
	sad[0] = GSL_SIGN(sign_adiab);
	sad[1] = GSL_SIGN(sign_nonadiab);
}

scalar phi(scalar t, sasfit_param *param) 
{
	return 2.0*M_PI*NU*(t-SD*LAMBDA/3956.034613) + RHO_0;
}

scalar b(scalar t, sasfit_param *param)
{
	return B1-B0*cos(phi(t,param));
}

scalar alpha(scalar t, scalar phase, sasfit_param *param) 
{
	scalar kB=1.3806504E-23;
	if (STATIC == 1.0) {
		return DAMP_ALPHA*(B_STATIC)* MU/(kB*TEMPERATURE);
	} else {
		return (B1-DAMP_ALPHA*(B0*cos(phi(t,param)-phase)))* MU/(kB*TEMPERATURE);
	}
}

scalar langevin(scalar x) 
{
	if ( fabs(x) < 1.0e-5 ) {
		return 1.0/3.0 * x;
	} else {
		return 1.0/tanh(x) - 1.0/x; 
	}
}

scalar lalpha_alpha(scalar x) 
{
	if ( fabs(x) < 1.0e-5 ) {
		return 1.0/3.0;
	} else {
		return langevin(x)/x;
	}
}

scalar l_tilde(scalar x) 
{
	return gsl_pow_2(langevin(x))-1.0+3.0*lalpha_alpha(x);
}

scalar sq(scalar t, sasfit_param *param) 
{
	return (SQ_AMPL-1.0)*((1-SQ_STATIC)*gsl_pow_2(langevin(alpha(t,SQ_PHASE,param)))+SQ_STATIC)+1.0;
}

scalar avg_f_pp(sasfit_param *param) 
{
	return FNUC-FMAG*S1*gsl_pow_2(sin(PSI));
}

scalar avg_f_mm(sasfit_param *param) 
{
	return FNUC+FMAG*S1*gsl_pow_2(sin(PSI));
}

scalar avg_f_pm(sasfit_param *param) 
{
	return FMAG*S1*cos(PSI)*sin(PSI);
}

scalar avg_f_mp(sasfit_param *param) 
{
	return avg_f_pm(param);
}

scalar avg_f2_pp(sasfit_param *param) 
{
	scalar sin2, sin4;
	sin2 = gsl_pow_2(sin(PSI));
	sin4 = gsl_pow_2(sin2);
	return FNUC*FNUC
		  +FMAG*FMAG*(S2*sin4+(1-S2)/3.0*sin2)
		  -2.0*FNUC*FMAG*S1*sin2;
}

scalar avg_f2_mm(sasfit_param *param) 
{
	scalar sin2, sin4;
	sin2 = gsl_pow_2(sin(PSI));
	sin4 = gsl_pow_2(sin2);
	return FNUC*FNUC
		  +FMAG*FMAG*(S2*sin4+(1-S2)/3.0*sin2)
		  +2.0*FNUC*FMAG*S1*sin2;
}

scalar avg_f2_pm(sasfit_param *param) 
{
	scalar sin2, sin4;
	sin2 = gsl_pow_2(sin(PSI));
	sin4 = gsl_pow_2(sin2);
	return FMAG*FMAG*(  2.0/3.0*(1.0-S2)
                      - S2*sin4
					  +(4.0*S2-1.0)/3.0*sin2
		             );
}

scalar avg_f2_mp(sasfit_param *param) 
{
	return avg_f2_pm(param);
}

scalar i_pp(sasfit_param *param) 
{
	return  avg_f2_pp(param)+gsl_pow_2(avg_f_pp(param))*(SQ_TIME-1);
}

scalar i_mm(sasfit_param *param) 
{
	return  avg_f2_mm(param)+gsl_pow_2(avg_f_mm(param))*(SQ_TIME-1);
}

scalar i_pm(sasfit_param *param) 
{
	return  avg_f2_pm(param)+gsl_pow_2(avg_f_pm(param))*(SQ_TIME-1);
}

scalar i_mp (sasfit_param *param) 
{
	return i_pm(param);
}

scalar triangle(scalar x, scalar center, scalar width)
{
	scalar xmin, xmax;

	xmin = center - width;
	xmax = center + width;

	if ((x<xmin) || (x>xmax)) return 0;
	if (x==center) return 1.0/width;
	if (x==xmin) return 0.0;
	if (x==xmax) return 0.0;
	if (x<center) return (x-xmin)/(center-xmin)/width;
	if (x>center) return (x-xmax)/(center-xmax)/width;

	return 0.0;
}

scalar isosceles_trapezoid(scalar x, scalar Base_Bottom, scalar Base_Top, scalar Center)
{
	scalar N, y0;
	scalar X_min, X_max;


//	SASFIT_CHECK_COND2((Base_Bottom <= Base_Top), param, "Base_Bottom(%lg) <= Base_Top(%lg)",Base_Bottom,Base_Top);
//	SASFIT_CHECK_COND1((Base_Bottom <= 0), param, "Base_Bottom(%lg) <= 0",Base_Bottom);
//	SASFIT_CHECK_COND1((Base_Top < 0), param, "Base_Top(%lg) < 0",Base_Top);

	N = 1.0;

	y0 = 2.*N/(Base_Bottom+Base_Top);

	X_min = Center-Base_Bottom/2.;
	X_max = Center+Base_Bottom/2.;

	if ((x<X_min) || (x>X_max)) return 0.0;
	if (fabs(Center-x)<=Base_Top/2.) return y0;

	if (x<(Center-Base_Top/2.)) return (x-X_min)*y0*2.0/(Base_Bottom-Base_Top);
	if (x>(Center+Base_Top/2.)) return (x-X_max)*y0*2.0/(Base_Top-Base_Bottom);

	return 0.0;
}
scalar i_meas_kern(scalar x, sasfit_param *param)
{	
	scalar res,p_out;
	scalar r_ad[2];
	scalar s_ad[2];

	if 	(LAM_CENTER != 0.0  || D_LAM_LAM != 0.0) {	
		LAMBDA = x;
	} else {
		LAMBDA = 0.0;
		TIME = x;
	}

	AD_FUNC = b(TIME, param)*alpha(TIME,ALPHA_PHASE,param);
	NONAD_FUNC = b(TIME, param);

//	AD_FUNC = b(TIME, param);
//	NONAD_FUNC = 1;

	SQ_TIME = sq(TIME,param);

	r_adiabatic(AD, AD_FUNC, NONAD_FUNC, r_ad, s_ad);

//
// the sign of alpha is treated in the calculation of the cross section below
//
	S1 = langevin(fabs(alpha(TIME,ALPHA_PHASE,param)));
	S2 = 1.0-3.0*lalpha_alpha(fabs(alpha(TIME,ALPHA_PHASE,param)));

	res = 0.0;
	
	p_out = 1;
	res = res + r_ad[0]*n_plus(P_POL,EPSILON_FLIPPER)* (s_ad[0]>0?i_pp(param):i_mm(param)) *
									(r_ad[0]*T_out(1,p_out,param)
									+0*r_ad[1]*T_out(-s_ad[0],p_out,param))
			  + 0*r_ad[1]*n_plus(P_POL,EPSILON_FLIPPER)* i_pp(param) *
									(r_ad[0]*T_out(s_ad[0],p_out,param)
									+0*r_ad[1]*T_out(1,p_out,param));
	p_out = -1;
	res = res + r_ad[0]*n_plus(P_POL,EPSILON_FLIPPER)* (s_ad[0]>0?i_pm(param):i_mp(param)) *
									(r_ad[0]*T_out(1,p_out,param)
									+0*r_ad[1]*T_out(-s_ad[0],p_out,param))
			  + 0*r_ad[1]*n_plus(P_POL,EPSILON_FLIPPER)* i_pm(param) *
									(r_ad[0]*T_out(1,p_out,param)
									+0*r_ad[1]*T_out(1,p_out,param));
	p_out = 1;
	res = res + r_ad[0]*n_minus(P_POL,EPSILON_FLIPPER)* (s_ad[0]>0?i_mp(param):i_pm(param)) *
									(r_ad[0]*T_out(1,p_out,param)
									+0*r_ad[1]*T_out(-s_ad[0],p_out,param))
			  + 0*r_ad[1]*n_minus(P_POL,EPSILON_FLIPPER)* i_mp(param) *
									(r_ad[0]*T_out(1,p_out,param)
									+r_ad[1]*T_out(1,p_out,param));
	p_out = -1;
	res = res + r_ad[0]*n_minus(P_POL,EPSILON_FLIPPER)* (s_ad[0]>0?i_mm(param):i_pp(param)) *
									(r_ad[0]*T_out(1,p_out,param)
									+0*r_ad[1]*T_out(-s_ad[0],p_out,param))
			  + 0*r_ad[1]*n_minus(P_POL,EPSILON_FLIPPER)* i_mm(param) *
									(r_ad[0]*T_out(1,p_out,param)
									+0*r_ad[1]*T_out(1,p_out,param));


	if 	(LAM_CENTER != 0.0  || D_LAM_LAM != 0.0) {	
		return triangle(LAMBDA,LAM_CENTER,LAM_CENTER*D_LAM_LAM)*res;
	} else {
		return isosceles_trapezoid(TIME,DELTA_T_BOTTOM,DELTA_T_TOP,SD)*res;
	}
}

scalar i_measured(scalar t, sasfit_param *param) 
{
	scalar kB=1.3806504E-23;

	MU = MU_KT*kB*TEMPERATURE*gsl_pow_3(R/R_AV);
	FNUC = 1.0 * gsl_pow_6(R/R_AV);
	FMAG = FNUC/FNUC_FMAG;
	TIME = t;

	if (D_LAM_LAM == 0) {
		return i_meas_kern(t,param);
	} else {
		return sasfit_integrate(LAM_CENTER*(1.0-D_LAM_LAM),LAM_CENTER*(1.0+D_LAM_LAM),&i_meas_kern,param);
	}
}


scalar i_measured_tisane(scalar t, sasfit_param *param) 
{
	scalar kB=1.3806504E-23;

	MU = MU_KT*kB*TEMPERATURE*gsl_pow_3(R/R_AV);
	FNUC = 1.0 * gsl_pow_6(R/R_AV);
	FMAG = FNUC/FNUC_FMAG;


	if (DELTA_T_BOTTOM == 0.0 && DELTA_T_TOP == 0.0) {
		return i_meas_kern(t,param);
	} else {
		return sasfit_integrate(t-DELTA_T_BOTTOM/2.,t+DELTA_T_BOTTOM/2.,&i_meas_kern,param);
	}
}
