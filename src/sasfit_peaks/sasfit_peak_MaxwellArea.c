/*
 * src/sasfit_peaks/sasfit_peak_MaxwellArea.c
 *
 * Copyright (c) 2008-2011, Paul Scherrer Institute (PSI)
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


scalar sasfit_peak_MaxwellArea(scalar x, sasfit_param * param)
{
	scalar bckgr, area, offset, mode, width;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 4, &area, &width, &offset, &bckgr);

	SASFIT_CHECK_COND1((width == 0), param, "width(%lg) == 0",width);
		
	if (x <= offset)  return bckgr;
	return bckgr+area*sqrt(2/M_PI)/gsl_pow_3(fabs(width))*gsl_pow_2((x-offset))*exp(-0.5*gsl_pow_2((x-offset)/width));
}