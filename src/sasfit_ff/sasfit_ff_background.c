/*
 * src/sasfit_ff/sasfit_ff_background.c
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
#include "include/sasfit_ff_utils.h"

scalar sasfit_ff_background(scalar q, sasfit_param * param)
{
	scalar l0, l1, l2, l3, val;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 4, &l0, &l1, &l2, &l3);

	val = 0.0;
	if (l0 != 0.0) 
	{
		val += l0;
	}
	if (l1 != 0.0 && q != 0.0) 
	{
		val += l1*q;
	}
	if (l2 != 0.0)
	{
		val += l2*pow(q,-l3);
	}
	return val;
}

