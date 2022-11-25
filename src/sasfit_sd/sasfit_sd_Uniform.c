/*
 * src/sasfit_sd/sasfit_sd_Uniform.c
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

#define N param->p[0]
#define A param->p[1]
#define B param->p[2]

scalar sasfit_sd_Uniform(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR( param );

	SASFIT_CHECK_COND2((A >= B), param, "A(%lg) >= B(%lg)",A,B);
	SASFIT_CHECK_COND1((A < 0), param, "A(%lg) < 0",A);

	if ((x < A) || (x > B) ) return 0;
	return N/(B-A);
}
