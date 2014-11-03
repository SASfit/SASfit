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

#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_errno.h>
#include "include/sasfit_peaks_utils.h"

scalar df_giddings(scalar x, sasfit_param *param)
{
	scalar bckgr, amplitude, center, width;
	gsl_sf_result besselI1, besselI0;
	int status;

	sasfit_get_param(param, 4, &amplitude, &center, &width, &bckgr);

	gsl_set_error_handler_off();

	status = gsl_sf_bessel_In_e(1, 0.2e1 * sqrt(center * x) / width,&besselI1);
	if (status) {
		SASFIT_CHECK_COND1((status != 0), param, "%s",gsl_strerror(status));
		return 0;
	}
	status = gsl_sf_bessel_In_e(0, 0.2e1 * sqrt(center * x) / width,&besselI0);
	if (status) {
		SASFIT_CHECK_COND1((status != 0), param, "%s",gsl_strerror(status));
		return 0;
	}
	return -exp(-(x + center) / width) * center * pow(x, -0.2e1) * 
		(- besselI0.val * sqrt(center * x) 
		 + width * besselI1.val 
		 + besselI1.val * x) 
		* pow(width, -0.2e1) * pow(center / x, -0.1e1 / 0.2e1);
}

scalar sasfit_peak_GiddingsMode(sasfit_param * param)
{	
	scalar bckgr, a0,a_tmp, amplitude, center, width,xl,xr,m;
	int iter;

	sasfit_get_param(param, 4, &amplitude, &center, &width, &bckgr);

	a_tmp=param->p[0];
	param->p[0]=1.0;
// searching for interval

	xr = center;
	a0 = sasfit_peak_GiddingsArea(center,param);
	do {
		xr = xr+fabs(width);
	} while (a0 < sasfit_peak_GiddingsArea(xr,param));

	xl = center;
	do {
		xl = xl-fabs(width);
		if (xl < 0) xl = 0;
	} while (a0 < sasfit_peak_GiddingsArea(xl,param) && xl>0);

//	sasfit_out("center:%lg l:%lg r:%lg\n",center,xl,xr);

	iter = 0;
	do {
		m = (xl+xr)/2.;
		iter=iter+1;
		if (df_giddings(m,param) > 0.0) {
			xl=m;
		} else {
			xr=m;
		}
	} while (fabs((xr-xl)/width) > 1e-8);
	param->p[0]=a_tmp;
// 	sasfit_out("iter:%d center:%lg l:%lg r:%lg m:%lg\n",iter,center,xl,xr,m);

	return m;
}

scalar sasfit_peak_GiddingsAmplitude(scalar x, sasfit_param * param)
{
	scalar y, bckgr, a_tmp, amplitude, center, width, y0, x0;
	gsl_sf_result besselI1;
	int status;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 4, &amplitude, &center, &width, &bckgr);

	SASFIT_CHECK_COND1((width <= 0), param, "width(%lg) <= 0 ",width);
	SASFIT_CHECK_COND1((center <= 0), param, "center(%lg) <= 0 ",center);

	gsl_set_error_handler_off();

	if (x<0) return bckgr;

	a_tmp = amplitude;
	param->p[0]=1.0;
	x0=x;
	x = sasfit_peak_GiddingsMode(param);
	param->p[0]=a_tmp;
	if (x0==0.0) {
		y0 = exp(-center/width)*center/gsl_pow_2(width);
	} else {
		status = gsl_sf_bessel_I1_e(2*sqrt(center*x)/width,&besselI1);
		if (status) {
//			SASFIT_CHECK_COND1((status != 0), param, "%s",gsl_strerror(status));
//			return 0;
			besselI1.val = 0;
		}
		y0 = 1.0/width * sqrt(center/x) *
			besselI1.val*
			exp((-x-center)/width);
	}

	x = x0;
	if (x==0.0) {
		y = bckgr +
			amplitude/y0;
	} else {
		status = gsl_sf_bessel_I1_e(2*sqrt(center*x)/width,&besselI1);
		if (status) {
//			SASFIT_CHECK_COND1((status != 0), param, "%s",gsl_strerror(status));
//			return 0;
			besselI1.val = 0;
		}
		y = bckgr +
			amplitude/y0/width * sqrt(center/x) *
			besselI1.val*
			exp((-x-center)/width);
	}
	return y;
}

