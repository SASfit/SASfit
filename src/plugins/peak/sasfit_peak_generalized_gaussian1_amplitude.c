/*
 * src/sasfit_peaks/sasfit_peak_GeneralizedGaussian1Area.c
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

#define AMPLITUDE	param->p[0]
#define LOCATION	param->p[1]
#define SCALE		param->p[2]
#define SHAPE		param->p[3]
#define DUMMY		param->p[4]
#define BCKGR		param->p[5]

scalar sasfit_peak_generalized_gaussian1_amplitude(scalar x, sasfit_param * param)
{
	scalar z, a0;

	SASFIT_ASSERT_PTR( param );

	SASFIT_CHECK_COND1((SCALE <= 0.0), param, "alpha(%lg) <= 0",SCALE);
	SASFIT_CHECK_COND1((SHAPE <= 0.0), param, "beta(%lg) <= 0",SHAPE);
	
	a0 = AMPLITUDE;
	z = (fabs(x-LOCATION)/SCALE);
	return BCKGR+a0*exp(-pow(z,SHAPE));
}

scalar sasfit_peak_generalized_gaussian1_amplitude_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_peak_generalized_gaussian1_amplitude_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}
