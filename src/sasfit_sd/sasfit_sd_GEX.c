/*
 * src/sasfit_sd/sasfit_sd_GEX.c
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

/*
float GEX(Tcl_Interp *interp,
				   float N,
                   float g,
                   float b,
                   float l,
				   float R0,
				   float q,
                   bool  *error)
		   */
/**
 * Calculates a generalized exponential distribution function
 */
scalar sasfit_sd_GEX(scalar x, sasfit_param * param)
{
	scalar N, g, b, l, R0, c;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 5, &N, &g, &b, &l, &R0);

	SASFIT_CHECK_COND1((R0 < 0), param, "R0(%lg) < 0", R0);
	SASFIT_CHECK_COND((l <= 0), param, "l <= 0");
	SASFIT_CHECK_COND((b <= 0), param, "b <= 0");
	SASFIT_CHECK_COND1((g <= 0), param, "g(%lg) <= 0", g);
//	SASFIT_CHECK_COND2((x <= R0), param, "x(%lg) <= R0(%lg)", x, R0);

	if ((x-R0) < 0.0) return 0.0;

	c=log(b/g)-sasfit_gammaln((l+2.0)/b)+(l+1.0)*log((x-R0)/g)-pow((x-R0)/g,b);
	return N*exp(c);
}
