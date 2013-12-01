/*
 * src/sasfit_old/bessj1.c
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
 */

#include <math.h>
#include "include/SASFIT_nr.h"

/*
float tbessj0(float x)
{
	if (x == 0.0) return 1.0;
	if (fabs(x) < 8.0) {
		int i;
		float z,z2,b0,b1,b2;
		static float ar[15]={-0.75885e-15, 0.4125321e-13,
			-0.194383469e-11, 0.7848696314e-10, -0.267925353056e-8,
			0.7608163592419e-7, -0.176194690776215e-5,
			0.324603288210051e-4, -0.46062616620628e-3,
			0.48191800694676e-2, -0.34893769411409e-1,
			0.158067102332097, -0.37009499387265, 0.265178613203337,
			-0.872344235285222e-2};
		x /= 8.0;
		z=2.0*x*x-1.0;
		z2=z+z;
		b1=b2=0.0;
		for (i=0; i<=14; i++) {
			b0=z2*b1-b2+ar[i];
			b2=b1;
			b1=b0;
		}
		return z*b1-b2+0.15772797147489;
	} else {
		void besspq0(float, float *, float *);
		float c,cosx,sinx,p0,q0;
		x=fabs(x);
		c=0.797884560802865/sqrt(x);
		cosx=cos(x-0.706858347057703e1);
		sinx=sin(x-0.706858347057703e1);
		besspq0(x,&p0,&q0);
		return c*(p0*cosx-q0*sinx);
	}
}


void bessy01(float x, float *y0, float *y1)
{
	if (x < 8.0) {
		int i;
		float tbessj0(float);
		float bessj1(float);
		float z,z2,c,lnx,b0,b1,b2;
		static float ar1[15]={0.164349e-14, -0.8747341e-13,
			0.402633082e-11, -0.15837552542e-9, 0.524879478733e-8,
			-0.14407233274019e-6, 0.32065325376548e-5,
			-0.563207914105699e-4, 0.753113593257774e-3,
			-0.72879624795521e-2, 0.471966895957634e-1,
			-0.177302012781143, 0.261567346255047,
			0.179034314077182, -0.274474305529745};
		static float ar2[15]={0.42773e-15, -0.2440949e-13,
			0.121143321e-11, -0.5172121473e-10, 0.187547032473e-8,
			-0.5688440039919e-7, 0.141662436449235e-5,
			-0.283046401495148e-4, 0.440478629867099e-3,
			-0.51316411610611e-2, 0.423191803533369e-1,
			-0.226624991556755, 0.675615780772188,
			-0.767296362886646, -0.128697384381350};
		c=0.636619772367581;
		lnx=c*log(x);
		c /= x;
		x /= 8.0;
		z=2.0*x*x-1.0;
		z2=z+z;
		b1=b2=0.0;
		for (i=0; i<=14; i++) {
			b0=z2*b1-b2+ar1[i];
			b2=b1;
			b1=b0;
		}
		*y0 = lnx*tbessj0(8.0*x)+z*b1-b2-0.33146113203285e-1;
		b1=b2=0.0;
		for (i=0; i<=14; i++) {
			b0=z2*b1-b2+ar2[i];
			b2=b1;
			b1=b0;
		}
		*y1 = lnx*bessj1(8.0*x)-c+x*(z*b1-b2+0.2030410588593425e-1);
	} else {
		void besspq0(float, float *, float *);
		void besspq1(float, float *, float *);
		float c,cosx,sinx,p0,q0,p1,q1;
		c=0.797884560802865/sqrt(x);
		besspq0(x,&p0,&q0);
		besspq1(x,&p1,&q1);
		x -= 0.706858347057703e1;
		cosx=cos(x);
		sinx=sin(x);
		*y0 = c*(p0*sinx+q0*cosx);
		*y1 = c*(q1*sinx-p1*cosx);
	}
}


void besspq0(float x, float *p0, float *q0)
{
	if (x < 8.0) {
		float tbessj0(float);
		void bessy01(float, float *, float *);
		float b,cosx,sinx,j0x,y0;
		b=sqrt(x)*1.25331413731550;
		bessy01(x,&y0,&j0x);
		j0x=tbessj0(x);
		x -= 0.785398163397448;
		cosx=cos(x);
		sinx=sin(x);
		*p0 = b*(y0*sinx+j0x*cosx);
		*q0 = b*(y0*cosx-j0x*sinx);
	} else {
		int i;
		float x2,b0,b1,b2,y;
		static float ar1[11]={-0.10012e-15, 0.67481e-15, -0.506903e-14,
			0.4326596e-13, -0.43045789e-12, 0.516826239e-11,
			-0.7864091377e-10, 0.163064646352e-8, -0.5170594537606e-7,
			0.30751847875195e-5, -0.536522046813212e-3};
		static float ar2[10]={-0.60999e-15, 0.425523e-14,
			-0.3336328e-13, 0.30061451e-12, -0.320674742e-11,
			0.4220121905e-10, -0.72719159369e-9, 0.1797245724797e-7,
			-0.74144984110606e-6, 0.683851994261165e-4};
		y=8.0/x;
		x=2.0*y*y-1.0;
		x2=x+x;
		b1=b2=0.0;
		for (i=0; i<=10; i++) {
			b0=x2*b1-b2+ar1[i];
			b2=b1;
			b1=b0;
		}
		*p0 = x*b1-b2+0.99946034934752;
		b1=b2=0.0;
		for (i=0; i<=9; i++) {
			b0=x2*b1-b2+ar2[i];
			b2=b1;
			b1=b0;
		}
		*q0 = (x*b1-b2-0.015555854605337)*y;
	}
}

void besspq1(float x, float *p1, float *q1)
{
	if (x < 8.0) {
		float bessj1(float);
		void bessy01(float, float *, float *);
		float b,cosx,sinx,j1x,y1;
		b=sqrt(x)*1.25331413731550;
		bessy01(x,&j1x,&y1);
		j1x=bessj1(x);
		x -= 0.785398163397448;
		cosx=cos(x);
		sinx=sin(x);
		*p1 = b*(j1x*sinx-y1*cosx);
		*q1 = b*(j1x*cosx+y1*sinx);
	} else {
		int i;
		float x2,b0,b1,b2,y;
		static float ar1[11]={0.10668e-15, -0.72212e-15, 0.545267e-14,
			-0.4684224e-13, 0.46991955e-12, -0.570486364e-11,
			0.881689866e-10, -0.187189074911e-8, 0.6177633960644e-7,
			-0.39872843004889e-5, 0.89898983308594e-3};
		static float ar2[11]={-0.10269e-15, 0.65083e-15, -0.456125e-14,
			0.3596777e-13, -0.32643157e-12, 0.351521879e-11,
			-0.4686363688e-10, 0.82291933277e-9, -0.2095978138408e-7,
			0.91386152579555e-6, -0.96277235491571e-4};
		y=8.0/x;
		x=2.0*y*y-1.0;
		x2=x+x;
		b1=b2=0.0;
		for (i=0; i<=10; i++) {
			b0=x2*b1-b2+ar1[i];
			b2=b1;
			b1=b0;
		}
		*p1 = x*b1-b2+1.0009030408600137;
		b1=b2=0.0;
		for (i=0; i<=10; i++) {
			b0=x2*b1-b2+ar2[i];
			b2=b1;
			b1=b0;
		}
		*q1 = (x*b1-b2+0.46777787069535e-1)*y;
	}
}


float bessj1(float x)
{
	if (x == 0.0) return 1.0;
	if (fabs(x) < 8.0) {
		int i;
		float z,z2,b0,b1,b2;
		static float ar[15]={-0.19554e-15, 0.1138572e-13,
			-0.57774042e-12, 0.2528123664e-10, -0.94242129816e-9,
			0.2949707007278e-7, -0.76175878054003e-6,
			0.158870192399321e-4, -0.260444389348581e-3,
			0.324027018268386e-2, -0.291755248061542e-1,
			0.177709117239728e0, -0.661443934134543e0,
			0.128799409885768e1, -0.119180116054122e1};
		x /= 8.0;
		z=2.0*x*x-1.0;
		z2=z+z;
		b1=b2=0.0;
		for (i=0; i<=14; i++) {
			b0=z2*b1-b2+ar[i];
			b2=b1;
			b1=b0;
		}
		return x*(z*b1-b2+0.648358770605265);
	} else {
		void besspq1(float, float *, float *);
		int sgnx;
		float c,cosx,sinx,p1,q1;
		sgnx = (x > 0.0) ? 1 : -1;
		x=fabs(x);
		c=0.797884560802865/sqrt(x);
		cosx=cos(x-0.706858347057703e1);
		sinx=sin(x-0.706858347057703e1);
		besspq1(x,&p1,&q1);
		return sgnx*c*(p1*sinx+q1*cosx);
	}
}

*/

float bessj1(x)
float x;
{
	float ax,z;
	double xx,y,ans,ans1,ans2;

	if ((ax=fabs(x)) < 8.0) {
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

