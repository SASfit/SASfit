/*
 * src/sasfit_peaks/sasfit_peak_GaussianLorentzianCrossProductAmplitude.c
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

scalar sasfit_peak_GaussianLorentzianCrossProductAmplitude(scalar x, sasfit_param * param)
{
	scalar z;
	scalar bckgr, amplitude, center, width, shape;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 4, &amplitude, &center, &width, &shape, &bckgr);

	SASFIT_CHECK_COND1((width <= 0), param, "width(%lg) <= 0",width);
	SASFIT_CHECK_COND1((shape <  0), param, "shape(%lg) < 0",shape);
	SASFIT_CHECK_COND1((shape >  1), param, "shape(%lg) > 1",shape);

	
	z = (x-center)/width;
	return amplitude/
		((1.0+shape*pow(z,2.0))*exp((1.0-shape)*0.5*z*z));
}

