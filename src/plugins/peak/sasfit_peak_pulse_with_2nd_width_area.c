/*
 * src/sasfit_peaks/sasfit_peak_PulsePeakwith2ndWidthTermArea.c
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


scalar sasfit_peak_pulse_with_2nd_width_area(scalar x, sasfit_param * param)
{
	scalar z1, z2;
	scalar bckgr, a0, area, center, width1, width2;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 5, &area, &center, &width1, &width2, &bckgr);

	SASFIT_CHECK_COND1((width1 <= 0), param, "width1(%lg) <= 0",width1);
	SASFIT_CHECK_COND1((width2 <= 0), param, "width2(%lg) <= 0",width2);

	z1 = (x-center)/width1;
	z2 = (x-center)/width2;

	if (z1<0) return bckgr;

	a0 = area*(width1+width2)/(width2*width2);
	return bckgr+a0*exp(-z2)*(1-exp(-z1));
}

scalar sasfit_peak_pulse_with_2nd_width_area_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_peak_pulse_with_2nd_width_area_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}