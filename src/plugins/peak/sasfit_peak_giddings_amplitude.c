/*
 * src/sasfit_peaks/sasfit_peak_GiddingsAmplitude.c
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

#include "include/private.h"
#include <sasfit_error_ff.h>

#define AMPL	param->p[0]
#define CENTER	param->p[1]
#define WIDTH	param->p[2]
#define DUMMY1	param->p[3]
#define DUMMY2	param->p[4]
#define BACKGR	param->p[5]


scalar df_giddings(scalar x, sasfit_param *param)
{
	gsl_sf_result besselI1, besselI0;
	int status;

	gsl_set_error_handler_off();

	status = gsl_sf_bessel_In_e(1, 0.2e1 * sqrt(CENTER * x) / WIDTH,&besselI1);
	if (status) {
		SASFIT_CHECK_COND1((status != 0), param, "%s",gsl_strerror(status));
		return 0;
	}
	status = gsl_sf_bessel_In_e(0, 0.2e1 * sqrt(CENTER * x) / WIDTH,&besselI0);
	if (status) {
		SASFIT_CHECK_COND1((status != 0), param, "%s",gsl_strerror(status));
		return 0;
	}
	return -exp(-(x + CENTER) / WIDTH) * CENTER * pow(x, -0.2e1) * 
		(- besselI0.val * sqrt(CENTER * x) 
		 + WIDTH * besselI1.val 
		 + besselI1.val * x) 
		* pow(WIDTH, -0.2e1) * pow(CENTER / x, -0.1e1 / 0.2e1);
}

scalar sasfit_peak_GiddingsMode(sasfit_param * param)
{	
	scalar a0,a_tmp,xl,xr,m;
	int iter;

	a_tmp=param->p[0];
	param->p[0]=1.0;
// searching for interval

	xr = CENTER;
	a0 = sasfit_peak_giddings_area(CENTER,param);
	do {
		xr = xr+fabs(WIDTH);
	} while (a0 < sasfit_peak_giddings_area(xr,param));

	xl = CENTER;
	do {
		xl = xl-fabs(WIDTH);
		if (xl < 0) xl = 0;
	} while (a0 < sasfit_peak_giddings_area(xl,param) && xl>0);

//	sasfit_out("center:%lg l:%lg r:%lg\n",CENTER,xl,xr);

	iter = 0;
	do {
		m = (xl+xr)/2.;
		iter=iter+1;
		if (df_giddings(m,param) > 0.0) {
			xl=m;
		} else {
			xr=m;
		}
	} while (fabs((xr-xl)/WIDTH) > 1e-8);
	param->p[0]=a_tmp;
// 	sasfit_out("iter:%d center:%lg l:%lg r:%lg m:%lg\n",iter,CENTER,xl,xr,m);

	return m;
}

scalar sasfit_peak_giddings_amplitude(scalar x, sasfit_param * param)
{
	scalar y, a_tmp, y0, x0;
	gsl_sf_result besselI1;
	int status;

	SASFIT_ASSERT_PTR( param );

	SASFIT_CHECK_COND1((WIDTH <= 0), param, "width(%lg) <= 0 ",WIDTH);
	SASFIT_CHECK_COND1((CENTER <= 0), param, "center(%lg) <= 0 ",CENTER);

	gsl_set_error_handler_off();

	if (x<0) return BACKGR;

	a_tmp = AMPL;
	param->p[0]=1.0;
	x0=x;
	x = sasfit_peak_GiddingsMode(param);
	param->p[0]=a_tmp;
	if (x0==0.0) {
		y0 = exp(-CENTER/WIDTH)*CENTER/gsl_pow_2(WIDTH);
	} else {
		status = gsl_sf_bessel_I1_e(2*sqrt(CENTER*x)/WIDTH,&besselI1);
		if (status) {
//			SASFIT_CHECK_COND1((status != 0), param, "%s",gsl_strerror(status));
//			return 0;
			besselI1.val = 0;
		}
		y0 = 1.0/WIDTH * sqrt(CENTER/x) *
			besselI1.val*
			exp((-x-CENTER)/WIDTH);
	}

	x = x0;
	if (x==0.0) {
		y = BACKGR +
			AMPL/y0;
	} else {
		status = gsl_sf_bessel_I1_e(2*sqrt(CENTER*x)/WIDTH,&besselI1);
		if (status) {
//			SASFIT_CHECK_COND1((status != 0), param, "%s",gsl_strerror(status));
//			return 0;
			besselI1.val = 0;
		}
		y = BACKGR +
			AMPL/y0/WIDTH * sqrt(CENTER/x) *
			besselI1.val*
			exp((-x-CENTER)/WIDTH);
	}
	return y;
}

scalar sasfit_peak_giddings_amplitude_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_peak_giddings_amplitude_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

