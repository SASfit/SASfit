/*
 * src/sasfit_peaks/sasfit_peak_PearsonVIIArea.c
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


scalar sasfit_peak_PearsonVIIArea(scalar x, sasfit_param * param)
{
	scalar z;
	scalar bckgr, a0, area, center, width, shape;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 5, &area, &center, &width, &shape, &bckgr);

	SASFIT_CHECK_COND1((width <=  0), param, "width(%lg) <= 0",width);
	SASFIT_CHECK_COND1((shape <= .5), param, "shape(%lg) <= 1/2",shape);

	z = (x-center)/width;
	a0 = area*shape*gsl_sf_gamma(shape)*sqrt((pow(2.,1./shape)-1.)/shape) /
			(width*sqrt(M_PI*shape)*gsl_sf_gamma(shape-0.5));
	return bckgr+a0/pow(1.+4.*z*z*(pow(2,1./shape)-1),shape);
}

