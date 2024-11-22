/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define LAMBDA	param->p[0]
#define T0SAMPLE	param->p[1]
#define T0FLAT	param->p[2]
#define DSAMPLE param->p[3]
#define DFLAT param->p[4]

#define ldouble scalar
#define Power(x, y)	    (pow((ldouble)(x), (ldouble)(y)))
#define Sqrt(x)		    (sqrt((ldouble)(x)))
#define Sqrtl(x)        (sqrt((ldouble)(x)))

#define Abs(x)		    (fabs((ldouble)(x)))

#define Exp(x)		    (exp((ldouble)(x)))
#define Log(x)		    (log((ldouble)(x)))

#define Sin(x)		    (sin((ldouble)(x)))
#define Cos(x)		    (cos((ldouble)(x)))
#define Tan(x)		    (tan((ldouble)(x)))

#define ArcSin(x)       (asin((ldouble)(x)))
#define ArcCos(x)       (acos((ldouble)(x)))
#define ArcTan(x)       (atan((ldouble)(x)))

#define Sinh(x)         (sinh((ldouble)(x)))
#define Cosh(x)         (cosh((ldouble)(x)))
#define Tanh(x)         (tanh((ldouble)(x)))

#define Cot(x)          (1./tan((ldouble)(x)))
#define Csc(x)          (1./sin((ldouble)(x)))
#define Sec(x)          (1./cos((ldouble)(x)))

#define ExpIntegralEi(x)    (gsl_sf_expint_Ei(x))
#define EulerGamma          M_EULER
#define LogIntegral(x)      (gsl_sf_expint_Ei(log(x)))

scalar FlatMultiInc(scalar DTheta, scalar T, scalar t, sasfit_param * param)
{   scalar A, B, res;
    A = (t*(T*(-3 - 6*EulerGamma + 3*Power(T,2) - 6*ExpIntegralEi(2*Log(T))*(1 + Log(T)) + Log(64/(Power(T,6)*Power(Log(T),6))) + Log(T)*(6*EulerGamma + 12*t + Log(64*Power(Log(T),6)))) + 6*(1 + Power(T,2))*LogIntegral(T)))/(12.*Log(T));
    if (DTheta < 1e-2) {
        B= (t*(T*(325 + 720*EulerGamma - 450*Power(T,2) + Log(4096) - 360*ExpIntegralEi(2*Log(T))*(-2 + Power(Log(T),2)) + Log(Power(Log(T),-12)) + 366*Log(Power(Log(T),2)/4.) +
            180*Log(T)*(4 - 4*EulerGamma + Power(T,2) + Log(Power(Log(T),-4)) + Log(T)*(-3 + 2*EulerGamma + 4*t + Log(4*Power(Log(T),2))))) + 720*(-1 - Power(T,2) + Power(T,2)*Log(T))*LogIntegral(T)))/(2880.*Log(T));
        return 1+B/A*Power(DTheta,2);
    } else {
        res = (-(t*(T*(EulerGamma + 2*t + Log(-Log(T)) + Cos(DTheta)*(-ExpIntegralEi(Log(T)*(1 + Sec(DTheta))) + Log(1 + Sec(DTheta)))) + (-1 + Cos(DTheta))*LogIntegral(T))) +
            t*Power(T,Sec(DTheta))*(2*t - ExpIntegralEi(2*Log(T)) + Cos(DTheta)*ExpIntegralEi(-(Log(T)*(-1 + Sec(DTheta)))) + Log(2) + T*LogIntegral(T) - Cos(DTheta)*(Log(-1 + Sec(DTheta)) + T*LogIntegral(T))))/(2.*Log(T)*(-1 + Sec(DTheta)));
        return res/A;
    }
}

scalar sasfit_sq_flat_panel_correction(scalar q, sasfit_param * param)
{
	scalar TwoTheta, T0, corr;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((LAMBDA < 0.0), param, "lambda(%lg) < 0",LAMBDA); // modify condition to your needs
	SASFIT_CHECK_COND1((T0SAMPLE < 0.0), param, "T0sample(%lg) < 0",T0SAMPLE); // modify condition to your needs
	SASFIT_CHECK_COND1((T0FLAT < 0.0), param, "T0flat(%lg) < 0",T0FLAT); // modify condition to your needs

	// insert your code here
	TwoTheta = 2*asin(q*LAMBDA/(4*M_PI));
	return FlatMultiInc(TwoTheta,T0SAMPLE,DSAMPLE,param)/FlatMultiInc(TwoTheta,T0FLAT,DFLAT,param);
	T0 = T0SAMPLE;
	corr  = ((T0*log(T0)*(-1 + 1./cos(TwoTheta)))/(-T0 + pow(T0,1./cos(TwoTheta))));
	T0=T0FLAT;
    corr = corr / ((T0*log(T0)*(-1 + 1./cos(TwoTheta)))/(-T0 + pow(T0,1./cos(TwoTheta))));
    return corr;
}

scalar sasfit_sq_flat_panel_correction_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_flat_panel_correction_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

