/*
 * src/sasfit_ff/gauss/sasfit_ff_gauss_poly.c
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

scalar sasfit_ff_gauss_poly(scalar q, sasfit_param * param)
{
	scalar na, uu, k;
	GAUSS_HEADER;

	sasfit_get_param(param, 4, EMPTY, &V[MW], &V[MN], &V[I0]);

	SASFIT_CHECK_COND1((V[MW] <= 0.0), param, "Mw(%lg) <= 0",V[MW]);
	SASFIT_CHECK_COND1((V[MN] <= 0.0), param, "Mn(%lg) <= 0",V[MN]);
	SASFIT_CHECK_COND2((V[MW] <= V[MN]), param, "Mw(%lg) <= Mn(%lg)",V[MW],V[MN]);

	na = 6.0221415e23;
	uu = V[MW]/V[MN]-1.0;
	k = u/(1.0 + 2.0*uu);

	if (SASFIT_EQUAL(k, 0.0)) return V[I0];
	return V[I0]*2.0*(pow(1+uu*k,-1.0/uu)+k-1.0)/((1+uu)*k*k);
}

