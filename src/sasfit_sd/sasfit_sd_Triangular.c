/*
 * src/sasfit_sd/sasfit_sd_Triangular.c
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
 *   Ingo Bressler (ingo@cs.tu-berlin.de)
 */

#include <gsl/gsl_math.h>
#include "include/sasfit_sd_utils.h"


/**
 * Calculates a Triangular distribution function.
 */
scalar sasfit_sd_Triangular(scalar x, sasfit_param * param)
{
	scalar N, Rmin, R0, Rmax, res;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 4, &N, &Rmin, &R0, &Rmax);

	SASFIT_CHECK_COND2((Rmin > Rmax), param, "Rmin(%lg) > Rmax(%lg)",Rmin,Rmax);
	SASFIT_CHECK_COND2((Rmin > R0), param, "Rmin(%lg) > R0(%lg)",Rmin,R0);
	SASFIT_CHECK_COND2((Rmax < R0), param, "Rmax(%lg) < R0(%lg)",Rmax,R0);

	res = 0.0;
	if ( x > Rmin && x < Rmax )
	{
		res = 2.0 * N / (Rmax-Rmin);
		if ( x < R0 ) // x > Rmin
		{
			res = res * (x-Rmin) / (R0-Rmin);
		} else if ( x > R0 ) { // x < Rmax
			res = res * (x-Rmax) / (R0-Rmax);
		}
	}

	return res;
}

