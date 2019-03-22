/*
 * src/sasfit_common/sasfit_utils.c
 *
 * Copyright (c) 2008-2009, Paul Scherrer Institute (PSI)
 *
 * This file is part of SASfit.
 *
 * SASfit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SASfit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SASfit.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   Ingo Bressler (ingo@cs.tu-berlin.de)
 */

#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>
#include "include/sasfit_common.h"


/* Function to calculate inverse error function.  Rational approximation
is used to generate an initial approximation, which is then improved to
full accuracy by two steps of Newton's method.  Code is a direct
translation of the erfinv m file in matlab version 2.0.
Author:  Gary L. Pavlis, Indiana University
Date:  February 1996
*/
#include <stdlib.h>
#include <math.h>
#include <float.h>
#define MAXDOUBLE DBL_MAX

#define CENTRAL_RANGE 0.7
scalar sasfit_erfinv(scalar y)
{
        double x,z,num,dem; /*working variables */
        /* coefficients in rational expansion */
        double a[4]={ 0.886226899, -1.645349621,  0.914624893, -0.140543331};
        double b[4]={-2.118377725,  1.442710462, -0.329097515,  0.012229801};
        double c[4]={-1.970840454, -1.624906493,  3.429567803,  1.641345311};
        double d[2]={ 3.543889200,  1.637067800};
        if(fabs(y) > 1.0) return (atof("NaN"));  /* This needs IEEE constant*/
        if(fabs(y) == 1.0) return((copysign(1.0,y))*MAXDOUBLE);
        if( fabs(y) <= CENTRAL_RANGE )
        {
                z = y*y;
                num = (((a[3]*z + a[2])*z + a[1])*z + a[0]);
                dem = ((((b[3]*z + b[2])*z + b[1])*z +b[0])*z + 1.0);
                x = y*num/dem;
        }
        else if( (fabs(y) > CENTRAL_RANGE) && (fabs(y) < 1.0) )
        {
                z = sqrt(-log((1.0-fabs(y))/2.0));
                num = ((c[3]*z + c[2])*z + c[1])*z + c[0];
                dem = (d[1]*z + d[0])*z + 1.0;
                x = (copysign(1.0,y))*num/dem;
        }
        /* Two steps of Newton-Raphson correction */
        x = x - (erf(x) - y)/( (2.0/sqrt(M_PI))*exp(-x*x));
        x = x - (erf(x) - y)/( (2.0/sqrt(M_PI))*exp(-x*x));

        return(x);
}

scalar chepolsum(int n, scalar x, scalar a[])
{
	int k;
	scalar h,r,s,tx;

	if (n == 0) return (a[0]);
	if (n == 1) return (a[0]+a[1]*x);
	tx=x+x;
	r=a[n];
	h=a[n-1]+r*tx;
	for (k=n-2; k>=1; k--) {
		s=r;
		r=h;
		h=a[k]+r*tx-s;
	}
	return (a[0]-r+h*x);
}

scalar sasfit_log1p (const scalar x)
{
	volatile scalar y;
	y = 1 + x;
	return log(y) - ((y-1)-x)/y ;  /* cancels errors with IEEE arithmetic */
}

#ifdef _MSC_VER
double log1p (double x)
{
	volatile scalar y;
	y = 1 + x;
	return log(y) - ((y-1)-x)/y ;  /* cancels errors with IEEE arithmetic */
}
#endif

void sincosfg(scalar x, scalar * f, scalar *g)
{
        scalar absx;
        scalar a[24];

        absx=fabs(x);
        a[0] =  9.6578828035185e-1;  a[1]  = -4.3060837778597e-2;
        a[2] = -7.3143711748104e-3;  a[3]  =  1.4705235789868e-3;
        a[4] = -9.8657685732702e-5;  a[5]  = -2.2743202204655e-5;
        a[6] =  9.8240257322526e-6;  a[7]  = -1.8973430148713e-6;
        a[8] =  1.0063435941558e-7;  a[9]  =  8.0819364822241e-8;
        a[10]= -3.8976282875288e-8;  a[11] =  1.0335650325497e-8;
        a[12]= -1.4104344875897e-9;  a[13] = -2.5232078399683e-10;
        a[14]=  2.5699831325961e-10; a[15] = -1.0597889253948e-10;
        a[16]=  2.8970031570214e-11; a[17] = -4.1023142563083e-12;
        a[18]= -1.0437693730018e-12; a[19] =  1.0994184520547e-12;
        a[20]= -5.2214239401679e-13; a[21] =  1.7469920787829e-13;
        a[22]= -3.8470012979279e-14;
        *f = chepolsum(22,8.0/absx-1.0,a)/x;
        a[0] =  2.2801220638241e-1;  a[1]  = -2.6869727411097e-2;
        a[2] = -3.5107157280958e-3;  a[3]  =  1.2398008635186e-3;
        a[4] = -1.5672945116862e-4;  a[5]  = -1.0664141798094e-5;
        a[6] =  1.1170629343574e-5;  a[7]  = -3.1754011655614e-6;
        a[8] =  4.4317473520398e-7;  a[9]  =  5.5108696874463e-8;
        a[10]= -5.9243078711743e-8;  a[11] =  2.2102573381555e-8;
        a[12]= -5.0256827540623e-9;  a[13] =  3.1519168259424e-10;
        a[14]=  3.6306990848979e-10; a[15] = -2.2974764234591e-10;
        a[16]=  8.5530309424048e-11; a[17] = -2.1183067724443e-11;
        a[18]=  1.7133662645092e-12; a[19] =  1.7238877517248e-12;
        a[20]= -1.2930281366811e-12; a[21] =  5.7472339223731e-13;
        a[22]= -1.8415468268314e-13; a[23] =  3.5937256571434e-14;
        *g = 4.0*chepolsum(23,8.0/absx-1.0,a)/absx/absx;
}

scalar sasfit_si(scalar x)
{
        scalar absx,z,f,g;

        absx=fabs(x);
        if (absx <= 4.0) {
                scalar a[11],z2;
                a[0] =2.7368706803630e0;   a[1] = -1.1106314107894e0;
                a[2] =1.4176562194666e-1;  a[3] = -1.0252652579174e-2;
                a[4] =4.6494615619880e-4;  a[5] = -1.4361730896642e-5;
                a[6] =3.2093684948229e-7;  a[7] = -5.4251990770162e-9;
                a[8] =7.1776288639895e-11; a[9] = -7.6335493723482e-13;
                a[10]=6.6679958346983e-15;
                z=x/4.0;
                z2=z*z;
                g=z2+z2-1.0;
                return z*chepolsum(10,g,a);
        } else {
                scalar cx,sx;
                scalar ssi;
                sincosfg(x,&f,&g);
                cx=cos(x);
                sx=sin(x);
                ssi = 1.570796326794897;
                if (x < 0.0) ssi = -(ssi);
                ssi -= f*cx+g*sx;
                return ssi;
        }
}

scalar sasfit_erf_pure(scalar x)
{
//	float gammp();
//	return x < 0.0 ? -gammp(interp,0.5,x*x,error) : gammp(interp,0.5,x*x,error);
	return gsl_sf_erf(x);
}

scalar sasfit_j1f(scalar x)
{
        if (x == 0) return 0;
        return (sin(x)-x*cos(x))/x/x;
}

scalar sasfit_bessj1(scalar x)
{
	scalar ax,z;
	scalar xx,y,ans,ans1,ans2;

	if ((ax=fabs(x)) < 8.0)
	{
		y=x*x;
		ans1=x*(72362614232.0+y*(-7895059235.0+y*(242396853.1
			+y*(-2972611.439+y*(15704.48260+y*(-30.16036606))))));
		ans2=144725228442.0+y*(2300535178.0+y*(18583304.74
			+y*(99447.43394+y*(376.9991397+y*1.0))));
		ans=ans1/ans2;
	} else {
		z=8.0/ax;
		y=z*z;
		xx=ax-2.356194491;
		ans1=1.0+y*(0.183105e-2+y*(-0.3516396496e-4
			+y*(0.2457520174e-5+y*(-0.240337019e-6))));
		ans2=0.04687499995+y*(-0.2002690873e-3
			+y*(0.8449199096e-5+y*(-0.88228987e-6
			+y*0.105787412e-6)));
		ans=sqrt(0.636619772/ax)*(cos(xx)*ans1-z*sin(xx)*ans2);
		if (x < 0.0) ans = -ans;
	}
	return ans;
}

scalar sasfit_rwbrush_w(scalar Q, scalar R)
{
    scalar u2;
    scalar u = Q*Q * R*R;
    u2=u*u;

    if ( fabs(u) <= 1.0e-4 )
    {
            return (1 - u/10. + u2/60.)/(1. + (2.*u)/5 + u2/20.);
    }

	return (1 - exp(-u)) / u;
}

scalar sasfit_jinc(scalar x) {
    scalar x2, x4;
    x2=x*x;
    x4=x2*x2;
//PadeApproximant {3,4}
/*
    if (fabs(x) <= 1e-6) {
		return (0.5-23*x2/480.+11*x4/11520.)
		       /(1+7/240.*x2+x4/2880.);
	} else {
		return gsl_sf_bessel_J1(x)/x;
	}
*/
// Taylor Series
    if (fabs(x) <= 1e-6) {
		return 0.5-gsl_pow_2(x)/16.0
		          +gsl_pow_4(x)/384.0
		          -gsl_pow_6(x)/18432.0
		          +gsl_pow_8(x)/1474560.0;
	} else {
		return gsl_sf_bessel_J1(x)/x;
	}
}

scalar sasfit_rod_fc(scalar Q, scalar R)
{
	scalar u  = Q * R;
	return 2.0 * sasfit_jinc(u);
}

scalar sasfit_sphere_fc(scalar Q, scalar R)
{
    scalar u2,u4;
	scalar u  = Q * R;
    u2=u*u;
    u4=u2*u2;

	if (fabs(u) <= 1.0e-4)
	{
		return   (1. - (211.*u2)/2860. + (2647.*u4)/2162160.)
                /(1. + (15. *u2)/572.  + (17.  *u4)/61776.);
	}

	return 3.0* (sin(u)-u*cos(u))/(u*u*u);
}

scalar sasfit_gauss_fc(scalar Q, scalar R)
{
	scalar u,res,u2,u3,u4;
	u = Q*Q * R*R;
    u2=u*u;
    u3=u2*u;
    u4=u2*u2;
	if (fabs(u) <= 1.0e-4)
	{
		res = 1.0-u/3.+u2/12.-u3/60.+u4/360.;
	} else {
        res = 2 * (exp(-u) - 1 + u) / (u*u);
	}
	return res;
}

scalar sasfit_g(scalar fp, scalar A)
{
        scalar gamma, alpha, beta;

        alpha = pow(1.0+2.0*fp,2.0)/pow(1.0-fp,4.0);
        beta  = -6.0*fp*pow(1.0+fp/2.0,2.0)/pow(1.0-fp,4.0);
        gamma = fp*alpha/2.0;
        return  alpha*(sin(A)-A*cos(A))/pow(A,2.0)  +
                beta*(2.0*A*sin(A)+(2.0-pow(A,2.0)) * cos(A)-2.0)/pow(A,3.0)  +
                gamma * (-pow(A,4.0) * cos(A) + 4.0*((3.0*pow(A,2.0)-6.0)*cos(A)+(pow(A,3.0)-6.0*A)*sin(A)+6.0))/pow(A,5.0);
}

const char * sasfit_get_lib_prefix()
{
	return SASFIT_LIB_PREFIX;
}

const char * sasfit_get_lib_suffix()
{
	return SASFIT_LIB_SUFFIX;
}


scalar find_LogNorm_int_range(scalar dim, scalar x0, scalar sigma, scalar *Xstart, scalar *Xend, sasfit_param *param)
{
	scalar a1,a2,a3,a4, X_0, X_50, X_n, nperc;
	nperc = 0.1;
	a3 = 1.0;
	a4 = fabs(x0);
	a2 = fabs(sigma);
	X_0  = a4*exp(-a2*a2*(a3-2.0*dim));
	X_n  = a4*exp(-a2*a2*(a3-2.0*dim)+sqrt(2.0*a2*a2*log(100.0/nperc)));
	X_50 = a4*exp(-a2*a2*(a3-2.0*dim)+sqrt(2.0*a2*a2*log(100.0/50.0)));

	SASFIT_CHECK_COND((X_50 == 0.0), param, "Can't guess good integration interval");

	*Xstart = 0.0;
	*Xend = X_n;

	if ((X_n-X_0) < X_0)
	{
		*Xend = X_n;
		*Xstart = 2.0*X_0-X_n;
	}
	return 0;
}
