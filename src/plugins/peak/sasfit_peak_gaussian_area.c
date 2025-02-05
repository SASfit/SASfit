/*
 * src/sasfit_peaks/sasfit_peak_GaussianArea.c
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
#define DUMMY3	param->p[3]
#define DUMMY4	param->p[4]
#define BCKGR	param->p[5]

scalar sasfit_peak_gaussian_area(scalar x, sasfit_param * param)
{
	scalar z, a0;

	SASFIT_ASSERT_PTR( param );

	SASFIT_CHECK_COND1((WIDTH <= 0.0), param, "width(%lg) <= 0",WIDTH);
	
	a0 = AREA/(sqrt(2.*M_PI)*WIDTH);
	z = (x-CENTER)/WIDTH;
	return BCKGR+a0*exp(-z*z/2.);
}

scalar sasfit_peak_gaussian_area_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_peak_gaussian_area_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}
