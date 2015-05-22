/*
 * src/sasfit_ff/gauss/sasfit_ff_gauss_1.c
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

scalar sasfit_ff_gauss_1(scalar q, sasfit_param * param)
{
	GAUSS_HEADER;

	V[I0] = V[PARAM3];
	if (SASFIT_EQUAL(V[Q], 0.0)) return V[I0];
	if (SASFIT_EQUAL(V[RG], 0.0)) return 0.0;
	return V[I0] * 2.0 * (u-1.0+exp(-u))/(u*u);
}

