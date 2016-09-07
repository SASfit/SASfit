/*
 * src/sasfit_peaks/sasfit_peak_HalfGaussianModifiedGaussianArea.c
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

#define AREA	param->p[0]
#define CENTER	param->p[1]
#define WIDTH	param->p[2]
#define DISTORTION	param->p[3]
#define DUMMY2	param->p[4]
#define BACKGR	param->p[5]

scalar sasfit_peak_half_gaussian_modified_gaussian_area(scalar x, sasfit_param * param)
{
	scalar y;

	SASFIT_ASSERT_PTR( param );

	SASFIT_CHECK_COND1((WIDTH <= 0), param, "width(%lg) <= 0 ",WIDTH);
	SASFIT_CHECK_COND1((DISTORTION == 0), param, "distortion(%lg) = 0 ",DISTORTION);

	y = BACKGR+AREA/(sqrt(2*M_PI)*sqrt(DISTORTION*DISTORTION+WIDTH*WIDTH)) *
		exp(-0.5*pow(x-CENTER,2)/(DISTORTION*DISTORTION+WIDTH*WIDTH)) *
		(1+gsl_sf_erf(DISTORTION*(x-CENTER)/(sqrt(2)*WIDTH*sqrt(DISTORTION*DISTORTION+WIDTH*WIDTH))));
	return y;
}

scalar sasfit_peak_half_gaussian_modified_gaussian_area_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_peak_half_gaussian_modified_gaussian_area_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}