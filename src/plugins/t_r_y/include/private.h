#ifndef HEADER_D74FAD662AB17C8
#define HEADER_D74FAD662AB17C8

/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef T_R_Y_PRIVATE_H
#define T_R_Y_PRIVATE_H

/*
 * Header file for the implementation of the form factor itself.
 */

// optional, depends on form factor implementation
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>

// mandatory, no adjustments necessary
#include <sasfit_common.h>

// mandatory, no adjustments necessary
#ifdef MAKE_SASFIT_PLUGIN
  // mandatory, no adjustments necessary
  #include <sasfit_plugin.h>

  SASFIT_PLUGIN_INFO_DECL;

#else


#endif

// adjust according to the plugins name
#include "sasfit_t_r_y.h"

//
// add local defines here:
// #define P0 param->p[0]
//

#define N	param->p[0]

#define BL	GSL_MIN(param->p[1],param->p[2])
#define BU	GSL_MAX(param->p[1],param->p[2])

#define DIM	    param->p[3]
#define RSCALE	param->p[3]

#define KAPPA	fabs(param->p[4])
#define THETA	param->p[4]
#define S	    param->p[4]
#define W	    param->p[4]
#define LAMBDA2	param->p[4]

#define ALPHA	param->p[5]
#define C	    param->p[5]
#define LAMBDA3	param->p[5]

#define BETA	param->p[6]
#define LAMBDA4	param->p[6]

#define GAMMA	param->p[7]
#define MU		param->p[7]

#define DELTA	param->p[8]
#define SIGMA	param->p[8]
#define LAMBDA 	param->p[8]




#define Q       param->p[MAXPAR-1]
#define YSTART  param->p[MAXPAR-2]
#define P       param->p[MAXPAR-3]


#define Power(x, y)	(pow((scalar)(x), (scalar)(y)))
#define Sqrt(x)		(sqrt((scalar)(x)))
#define Sqrtl(x)        (sqrt((scalar)(x)))

#define Abs(x)		(fabs((scalar)(x)))

#define Exp(x)		(exp((scalar)(x)))
#define Log(x)		(log((scalar)(x)))

#define Sin(x)		(sin((scalar)(x)))
#define Cos(x)		(cos((scalar)(x)))
#define Tan(x)		(tan((scalar)(x)))

#define ArcSin(x)       (asin((scalar)(x)))
#define ArcCos(x)       (acos((scalar)(x)))
#define ArcTan(x)       (atan((scalar)(x)))
#define ArcCot(x)       (atan((scalar)(1/x)))

#define Sinh(x)          (sinh((scalar)(x)))
#define Cosh(x)          (cosh((scalar)(x)))
#define Tanh(x)          (tanh((scalar)(x)))

#define Cot(x)          (1./tan((scalar)(x)))
#define Csc(x)          (1./sin((scalar)(x)))
#define Sec(x)          (1./cos((scalar)(x)))

#define E       2.71828182845904523536029
#define Pi      3.14159265358979323846264
#define Degree      0.01745329251994329576924

#define GSLPOWN(x,y)    (gsl_pow_int(x,y))
#define InverseErfc(x)  (sasfit_erfcinv(x))
#define ProductLog(x)   (gsl_sf_lambert_Wm1(x))


enum TRY_QY_fY_type {
    Y_UNIFORM,
    Y_FRECHET,
    Y_LOGISTIC,
    Y_LOGLOGISTIC,
    Y_PARETO,
    Y_RAYLEIGH,
    Y_EXTREMEVALUE,
    Y_CAUCHY,
    Y_EXPONENTIAL,
    Y_WEIBULL,
    Y_GAMMA
};

scalar KWm1(scalar, scalar);
scalar Lindley_f_R(scalar, scalar);
scalar Lindley_F_R(scalar, scalar);
scalar f_Gamma(scalar, scalar, scalar);
scalar F_Gamma(scalar, scalar, scalar);
#endif // end of file

#endif // header guard

