/*
 * src/plugins/rod/sasfit_ff_rod.c
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

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define LENGTH	param->p[0]
#define ETA	param->p[3]

scalar sasfit_ff_rod(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((LENGTH < 0.0), param, "LENGTH(%lg) < 0",LENGTH);

	if (q == 0.0) return ETA*ETA*LENGTH*LENGTH;
	if (LENGTH == 0.0) return 0.0;

	return ETA*ETA * LENGTH*LENGTH * 
		(2.0*gsl_sf_Si(q*LENGTH)/(q*LENGTH)-pow(sin(q*LENGTH*0.5)/(q*LENGTH*0.5),2.0));
}

scalar sasfit_ff_rod_f(scalar q, sasfit_param * param)
{
	// insert your code here
	return 0.0;
}

scalar sasfit_ff_rod_v(scalar q, sasfit_param * param, int dist)
{
	// insert your code here
	return 0.0;
}

