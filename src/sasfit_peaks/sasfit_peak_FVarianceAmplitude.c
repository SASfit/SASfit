/*
 * src/sasfit_peaks/sasfit_peak_FVarianceAmplitude.c
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
#include "include/sasfit_peaks_utils.h"


scalar sasfit_peak_FVarianceAmplitude(scalar x, sasfit_param * param)
{
	scalar z,t,u,v,w;
	scalar bckgr, a0, amplitude, center, width, shape1, shape2;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 6, &amplitude, &center, &width, &shape1, &shape2, &bckgr);

	SASFIT_CHECK_COND1((width <= 0), param, "width(%lg) <= 0",width);
	SASFIT_CHECK_COND1((shape1 <= 2), param, "shape1(%lg) <= 2",shape1);
	SASFIT_CHECK_COND1((shape2 <= 2), param, "shape2(%lg) <= 2",shape2);
	
	u = (shape1-2.)/(shape2+2.);
	v = shape2/shape1;
	w = shape1/2.-1.;
	t = (shape1+shape2)/2.0;
	a0 = amplitude*pow(1+u,t)/pow(v*u,w);
	z = (x-center)/width;

	if ((z+v*u) < 0) {
		return bckgr;
	} else {
		return bckgr+a0*pow(z+v*u,w)/pow(1.0+(z+v*u)/v,t);
	}
}

