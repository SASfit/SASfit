/*
 * src/sasfit_ff/gauss/sasfit_ff_gauss_2.c
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

scalar sasfit_ff_gauss_2(scalar q, sasfit_param * param)
{
	scalar beta;
	GAUSS_HEADER;

	sasfit_get_param(param, 4, EMPTY, &V[B_P], &V[VOL], &V[ETA_S]);

	SASFIT_CHECK_COND1((V[VOL] < 0.0), param, "V(%lg) < 0",V[VOL]);

	beta = V[B_P] - V[VOL]*V[ETA_S];
	if (SASFIT_EQUAL(V[Q], 0.0)) return beta*beta;
	return beta*beta*2.0*(u-1.0+exp(-u))/(u*u);
}

