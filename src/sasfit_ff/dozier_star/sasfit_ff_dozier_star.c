/*
 * src/sasfit_ff/dozier_star/sasfit_ff_dozier_star.c
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

scalar sasfit_ff_dozier_star(scalar q, sasfit_param * param)
{
	DOZIER_STAR_HEADER;

	V[ALPHA] = V[PARAM2];
	if ( SASFIT_EQUAL(V[Q], 0.0) )
	{
		if (SASFIT_EQUAL(V[MU], 0.0)) 
		{
			return V[I0] + 4*M_PI*V[ALPHA];
		} else {
			return V[I0] + (4.0*M_PI*V[ALPHA]) * V[MU] * gsl_sf_gamma(V[MU]);
		}
	} else { // q != 0
		if (SASFIT_EQUAL(V[MU], 0.0)) 
		{
			return V[I0] + 4*M_PI*V[ALPHA] * atan(V[Q]*V[XI])/(V[Q]*V[XI]);
		} else {
			return V[I0]*exp(-V[RG]*V[RG]*V[Q]*V[Q]/3.0)
				+ (4.0*M_PI*V[ALPHA]) / (V[Q]*V[XI])
				* (sin(V[MU]*atan(V[Q]*V[XI])))
				/ pow(1.0+V[Q]*V[Q]*V[XI]*V[XI], 0.5*V[MU])
				* gsl_sf_gamma(V[MU]);
		}
	}
}

