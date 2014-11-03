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

#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_min.h>
#include "include/sasfit_peaks_utils.h"

scalar df_emg(scalar x, sasfit_param *param)
{
	scalar bckgr, amplitude, center, width, distortion;
	sasfit_get_param(param, 5, &amplitude, &center, &width, &distortion, &bckgr);

	return -pow(distortion, -0.2e1) * exp(width * width * pow(distortion, -0.2e1) / 0.2e1 + (center - x) / distortion) * 
		(scalar) (gsl_sf_erf((x - center) * sqrt(0.2e1) / width / 0.2e1 - width * sqrt(0.2e1) / distortion / 0.2e1) + distortion/fabs(distortion)) / 0.2e1 + 1. / distortion * exp(width * width * pow(distortion, -0.2e1) / 0.2e1 + (center - x) / distortion) * pow(0.3141592654e1, -0.1e1 / 0.2e1) * exp(-pow((x - center) * sqrt(0.2e1) / width / 0.2e1 - width * sqrt(0.2e1) / distortion / 0.2e1, 0.2e1)) * sqrt(0.2e1) / width / 0.2e1;
}

scalar sasfit_peak_ExponentiallyModifiedGaussianMode(sasfit_param * param)
{	
	scalar bckgr, a0,Atmp, amplitude, center, width, distortion,xl,xr,m;
	int iter;

	sasfit_get_param(param, 5, &amplitude, &center, &width, &distortion, &bckgr);

	Atmp=param->p[0];
	param->p[0]=1.0;
// searching for interval

	xr = center;
	a0 = sasfit_peak_ExponentiallyModifiedGaussianArea(center,param);
	do {
		xr = xr+fabs(width);
	} while (a0 < sasfit_peak_ExponentiallyModifiedGaussianArea(xr,param));

	xl = center;
	do {
		xl = xl-fabs(width);
	} while (a0 < sasfit_peak_ExponentiallyModifiedGaussianArea(xl,param));

//	sasfit_out("center:%lg l:%lg r:%lg\n",center,xl,xr);

	iter = 0;
	do {
		m = (xl+xr)/2.;
		iter=iter+1;
		if (df_emg(m,param) > 0.0) {
			xl=m;
		} else {
			xr=m;
		}
	} while (fabs((xr-xl)/width) > 1e-8);
	param->p[0]=Atmp;
// 	sasfit_out("iter:%d center:%lg l:%lg r:%lg m:%lg\n",iter,center,xl,xr,m);

	return m;
}
scalar sasfit_peak_ExponentiallyModifiedGaussianAmplitude(scalar x, sasfit_param * param)
{
	scalar y, bckgr, a0, amplitude, center, xa, width, distortion;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 5, &amplitude, &center, &width, &distortion, &bckgr);

	SASFIT_CHECK_COND1((width <= 0), param, "width(%lg) <= 0 ",width);
	SASFIT_CHECK_COND1((distortion == 0), param, "distortion(%lg) = 0 ",distortion);

//	a0 = 1./(.753*2*width*sqrt(-2.*log(1./4.)));

	xa = sasfit_peak_ExponentiallyModifiedGaussianMode(param);
	a0 = 1./(2*distortion) *
		exp(pow(width/distortion,2)/2.+(center-xa)/distortion) *
		(gsl_sf_erf((xa-center)/(sqrt(2)*width)-width/(sqrt(2)*distortion))+distortion/fabs(distortion));

	xa = x;
	y = bckgr+amplitude/a0/(2*distortion) *
		exp(pow(width/distortion,2)/2.+(center-xa)/distortion) *
		(gsl_sf_erf((xa-center)/(sqrt(2)*width)-width/(sqrt(2)*distortion))+distortion/fabs(distortion));
	return y;
}

