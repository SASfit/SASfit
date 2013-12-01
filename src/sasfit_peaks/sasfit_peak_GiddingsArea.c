/*
 * src/sasfit_peaks/sasfit_peak_GiddingsArea.c
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
#include <gsl/gsl_errno.h>
#include "include/sasfit_peaks_utils.h"

scalar sasfit_peak_GiddingsArea(scalar x, sasfit_param * param)
{
	scalar y, bckgr, area, center, width;
	gsl_sf_result besselI1;
	int status;

	gsl_set_error_handler_off();
	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 4, &area, &center, &width, &bckgr);

	SASFIT_CHECK_COND1((width <= 0), param, "width(%lg) <= 0 ",width);
	SASFIT_CHECK_COND1((center <= 0), param, "center(%lg) <= 0 ",center);

	area = area*exp(center/width)/(exp(center/width)-1);

	if (x<0) return bckgr;
	if (x==0.0) {
		y = bckgr +
			area*exp(-center/width)*center/gsl_pow_2(width);
	} else {
		status = gsl_sf_bessel_I1_e(2*sqrt(center*x)/width,&besselI1);
		if (status) {
//			SASFIT_CHECK_COND1((status != 0), param, "%s",gsl_strerror(status));
//			return 0;
			besselI1.val = 0;
		}
		y = bckgr +
			area/width * sqrt(center/x) *
			besselI1.val*
			exp((-x-center)/width);
	}
	return y;
}

