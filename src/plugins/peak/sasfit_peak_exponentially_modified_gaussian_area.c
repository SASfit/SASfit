/*
 * src/sasfit_peaks/sasfit_peak_ExponentiallyModifiedGaussianArea.c
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

#define AREA   param->p[0]
#define CENTER param->p[1]
#define DUMMY param->p[1]
#define WIDTH  fabs(param->p[3])
#define DISTORTION  param->p[4]
#define BACKGR param->p[5]

scalar sasfit_peak_exponentially_modified_gaussian_area(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR( param );

	SASFIT_CHECK_COND1((WIDTH <= 0), param, "width(%lg) <= 0 ",WIDTH);
	SASFIT_CHECK_COND1((DISTORTION == 0), param, "distortion(%lg) == 0 ",DISTORTION);

	return BACKGR+AREA/(2*DISTORTION) *
		exp(pow(WIDTH/DISTORTION,2)/2.+(CENTER-x)/DISTORTION) *
		(gsl_sf_erf((x-CENTER)/(sqrt(2)*WIDTH)-WIDTH/(sqrt(2)*DISTORTION))+DISTORTION/fabs(DISTORTION));
}

scalar sasfit_peak_exponentially_modified_gaussian_area_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_peak_exponentially_modified_gaussian_area_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}
