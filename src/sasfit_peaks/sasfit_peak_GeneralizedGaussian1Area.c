/*
 * src/sasfit_peaks/sasfit_peak_GeneralizedGaussian1Amplitude.c
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
#include <gsl/gsl_sf_gamma.h>
#include "include/sasfit_peaks_utils.h"

#define AMPLITUDE	param->p[0]
#define LOCATION	param->p[1]
#define SCALE		param->p[2]
#define SHAPE		param->p[3]
#define BCKGR		param->p[4]

scalar sasfit_peak_GeneralizedGaussian1Amplitude(scalar x, sasfit_param * param)
{
	scalar z, a0;

	SASFIT_ASSERT_PTR( param );

	SASFIT_CHECK_COND1((SCALE <= 0.0), param, "alpha(%lg) <= 0",SCALE);
	SASFIT_CHECK_COND1((SHAPE <= 0.0), param, "beta(%lg) <= 0",SHAPE);
	
	a0 = AMPLITUDE;
	z = (fabs(x-LOCATION)/SCALE);
	return BCKGR+a0*exp(-pow(z,SHAPE));
}
