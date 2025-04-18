/*
 * src/sasfit_peaks/sasfit_peak_ChiSquaredArea.c
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
 *   3rd of August 2016
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

#define AREA   param->p[0]
#define CENTER param->p[1]
#define X0	   param->p[2]
#define MODE   (CENTER+X0)
#define WIDTH  fabs(param->p[3])
#define SHAPE  param->p[4]
#define BACKGR param->p[5]

scalar sasfit_peak_chi_squared_area(scalar x, sasfit_param * param)
{
	scalar z,u,v;
	scalar A0;

	SASFIT_ASSERT_PTR( param );

	SASFIT_CHECK_COND1((WIDTH == 0), param, "width(%lg) <= 0",WIDTH);
	SASFIT_CHECK_COND1((SHAPE <= 0), param, "shape(%lg) <= 0",SHAPE);
	
	A0 = AREA/(WIDTH*pow(2.0,SHAPE*0.5)*gsl_sf_gamma(SHAPE*0.5));
	z = (x-MODE)/WIDTH;
	u = SHAPE-2.0;
	v = 0.5*SHAPE-1.0;
	if ((z+u) < 0)  return BACKGR;
	return BACKGR+A0*pow(z+u,v)*exp(-0.5*(z+u));
}

scalar sasfit_peak_chi_squared_area_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_peak_chi_squared_area_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}