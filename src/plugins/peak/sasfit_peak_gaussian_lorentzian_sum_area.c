/*
 * src/sasfit_peaks/sasfit_peak_GaussianLorentzianSumArea.c
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
#define WIDTH	fabs(param->p[2])
#define SHAPE	param->p[3]
#define DUMMY   param->p[4]
#define BACKGR	param->p[5]

scalar sasfit_peak_gaussian_lorentzian_sum_area(scalar x, sasfit_param * param)
{
	scalar z;
	scalar u,v;

	SASFIT_ASSERT_PTR( param );

	SASFIT_CHECK_COND1((WIDTH == 0), param, "width(%lg) == 0",WIDTH);
	SASFIT_CHECK_COND1((SHAPE <  0), param, "shape(%lg) < 0",SHAPE);
	SASFIT_CHECK_COND1((SHAPE >  1), param, "shape(%lg) > 1",SHAPE);

	u = SHAPE*sqrt(log(2.0))/(WIDTH*sqrt(M_PI));
	v = (1.-SHAPE)/(M_PI*WIDTH);
	z = (x-CENTER)/WIDTH;
	return 2*AREA*
			( (u*exp(-4.*log(2)*z*z)+v/(1.+4.*z*z)))+BACKGR;
}

scalar sasfit_peak_gaussian_lorentzian_sum_area_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_peak_gaussian_lorentzian_sum_area_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}
