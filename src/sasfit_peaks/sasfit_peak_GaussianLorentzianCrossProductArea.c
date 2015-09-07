/*
 * src/sasfit_peaks/sasfit_peak_GaussianLorentzianCrossProductArea.c
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
#include "include/sasfit_peaks_utils.h"

#define AREA   param->p[0]
#define CENTER param->p[1]
#define WIDTH  fabs(param->p[2])
#define SHAPE  param->p[3]
#define BACKGR param->p[4]

scalar int_GLxprod_Area(sasfit_param *param) 
{	
	scalar u;

	if (SHAPE == 0.0) {
		return WIDTH*sqrt(2.0*M_PI);
	}
	
	u = 0.5*(1.0-SHAPE)/SHAPE;
	return WIDTH*M_PI/sqrt(SHAPE)*exp(u)*gsl_sf_erfc(sqrt(u));
}

scalar sasfit_peak_GaussianLorentzianCrossProductArea(scalar x, sasfit_param * param)
{
	scalar z;
//,int_GLxprod;

	SASFIT_ASSERT_PTR( param );

	SASFIT_CHECK_COND1((WIDTH == 0), param, "width(%lg) == 0",WIDTH);
	SASFIT_CHECK_COND1((SHAPE <  0), param, "shape(%lg) < 0",SHAPE);
	SASFIT_CHECK_COND1((SHAPE >  1), param, "shape(%lg) > 1",SHAPE);
	
	z = (x-CENTER)/WIDTH;

	return AREA/int_GLxprod_Area(param)/
		((1.0+SHAPE*pow(z,2.0))*exp((1.0-SHAPE)*0.5*z*z));
}
