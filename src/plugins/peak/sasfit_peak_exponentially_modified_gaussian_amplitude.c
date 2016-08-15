/*
 * src/sasfit_peaks/sasfit_peak_ExponentiallyModifiedGaussianAmplitude.c
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
// #include <gsl/gsl_errno.h>
// #include <gsl/gsl_min.h>

#define AMPLITUDE   param->p[0]
#define CENTER param->p[1]
#define DUMMY param->p[1]
#define WIDTH  fabs(param->p[3])
#define DISTORTION  param->p[4]
#define BACKGR param->p[5]

scalar df_emg(scalar x, sasfit_param *param)
{

	return -pow(DISTORTION, -0.2e1) * exp(WIDTH * WIDTH * pow(DISTORTION, -0.2e1) / 0.2e1 + (CENTER - x) / DISTORTION) * 
		(scalar) (gsl_sf_erf((x - CENTER) * sqrt(0.2e1) / WIDTH / 0.2e1 - WIDTH * sqrt(0.2e1) / DISTORTION / 0.2e1) + DISTORTION/fabs(DISTORTION)) / 0.2e1 + 1. / DISTORTION * exp(WIDTH * WIDTH * pow(DISTORTION, -0.2e1) / 0.2e1 + (CENTER - x) / DISTORTION) * pow(0.3141592654e1, -0.1e1 / 0.2e1) * exp(-pow((x - CENTER) * sqrt(0.2e1) / WIDTH / 0.2e1 - WIDTH * sqrt(0.2e1) / DISTORTION / 0.2e1, 0.2e1)) * sqrt(0.2e1) / WIDTH / 0.2e1;
}

scalar sasfit_peak_ExponentiallyModifiedGaussianMode(sasfit_param * param)
{	
	scalar a0,Atmp,xl,xr,m;
	int iter;


	Atmp=AMPLITUDE;
	AMPLITUDE=1.0;
// searching for interval

	xr = CENTER;
	a0 = sasfit_peak_exponentially_modified_gaussian_area(CENTER,param);
	do {
		xr = xr+fabs(WIDTH);
	} while (a0 < sasfit_peak_exponentially_modified_gaussian_area(xr,param));

	xl = CENTER;
	do {
		xl = xl-fabs(WIDTH);
	} while (a0 < sasfit_peak_exponentially_modified_gaussian_area(xl,param));

//	sasfit_out("center:%lg l:%lg r:%lg\n",CENTER,xl,xr);

	iter = 0;
	do {
		m = (xl+xr)/2.;
		iter=iter+1;
		if (df_emg(m,param) > 0.0) {
			xl=m;
		} else {
			xr=m;
		}
	} while (fabs((xr-xl)/WIDTH) > 1e-8);
	param->p[0]=Atmp;
// 	sasfit_out("iter:%d center:%lg l:%lg r:%lg m:%lg\n",iter,center,xl,xr,m);

	return m;
}

scalar sasfit_peak_exponentially_modified_gaussian_amplitude(scalar x, sasfit_param * param)
{   
    scalar a0, xa;

	SASFIT_ASSERT_PTR( param );


	SASFIT_CHECK_COND1((WIDTH <= 0), param, "width(%lg) <= 0 ",WIDTH);
	SASFIT_CHECK_COND1((DISTORTION == 0), param, "distortion(%lg) = 0 ",DISTORTION);

//	a0 = 1./(.753*2*width*sqrt(-2.*log(1./4.)));

	xa = sasfit_peak_ExponentiallyModifiedGaussianMode(param);
	a0 = 1./(2*DISTORTION) *
		exp(pow(WIDTH/DISTORTION,2)/2.+(CENTER-xa)/DISTORTION) *
		(gsl_sf_erf((xa-CENTER)/(sqrt(2)*WIDTH)-WIDTH/(sqrt(2)*DISTORTION))+DISTORTION/fabs(DISTORTION));

	xa = x;
	return BACKGR+AMPLITUDE/a0/(2*DISTORTION) *
		exp(pow(WIDTH/DISTORTION,2)/2.+(CENTER-xa)/DISTORTION) *
		(gsl_sf_erf((xa-CENTER)/(sqrt(2)*WIDTH)-WIDTH/(sqrt(2)*DISTORTION))+DISTORTION/fabs(DISTORTION));
}

scalar sasfit_peak_exponentially_modified_gaussian_amplitude_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_peak_exponentially_modified_gaussian_amplitude_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

