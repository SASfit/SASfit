/*
 * src/sasfit_peaks/sasfit_peak_HaarhoffVanderLindeArea.c
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
#include <float.h>
#include "include/sasfit_peaks_utils.h"

#define DBL_MAX_10_EXP 999
scalar sasfit_peak_HaarhoffVanderLindeArea(scalar x, sasfit_param * param)
{
	scalar y, bckgr, area, center, width, distortion;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 5, &area, &center, &width, &distortion, &bckgr);

	SASFIT_CHECK_COND1((center <= 0), param, "center(%lg) <= 0 ",center);
	SASFIT_CHECK_COND1((width <= 0), param, "width(%lg) <= 0 ",width);
	SASFIT_CHECK_COND1((distortion == 0), param, "distortion(%lg) == 0 ",distortion);

	if (center*distortion/(width*width) > DBL_MAX_10_EXP) {
		y = bckgr+area*width/(center*distortion*sqrt(2.*M_PI)) *
			exp(-0.5*pow((x-center)/width,2.)) /
			(
				0. +
//				(1.+gsl_sf_erf((x-center)/(sqrt(2.)*width)))/2.0
				(gsl_sf_erfc(-(x-center)/(sqrt(2.)*width)))/2.0
			);
	} else {
		y = bckgr+area*width/(center*distortion*sqrt(2.*M_PI)) *
			exp(-0.5*pow((x-center)/width,2.)) /
			(
				1./(exp(center*distortion/(width*width))-1.) +
//				(1.+gsl_sf_erf((x-center)/(sqrt(2.)*width)))/2.0
				(gsl_sf_erfc(-(x-center)/(sqrt(2.)*width)))/2.0
			);
	}
	return y;
}

