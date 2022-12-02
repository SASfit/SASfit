/*
 * src/sasfit_ff/ellip_rwbrush/sasfit_ff_ellip_rwbrush.c
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
 *   Ingo Bressler (ingo.bressler@bam.de)
 */

#include "include/private.h"

scalar sasfit_ff_ellip_rwbrush(scalar q, sasfit_param * param)
{
	scalar s, ae, vol;
	ELLIP_RWBRUSH_HEADER;

	V[RC]		= V[PARAM0];
	V[N_AGG]	= V[PARAM1];
	vol = 4./3. * M_PI * pow(V[RC],3.)*V[EPSILON];

	if (V[EPSILON] < 1.0)
	{
		ae = acos(V[EPSILON]);
		s = 2.*M_PI*V[RC]*V[RC]*(1.+V[EPSILON]*gsl_atanh(sin(ae))/sin(ae));
	} 
	else if (V[EPSILON] > 1.0)
	{
		ae = acos(1.0/V[EPSILON]);
		s = 2.*M_PI*V[RC]*V[RC]*(1.0+V[EPSILON]*ae/tan(ae));
	} else 
	{
		s = 4.*M_PI*V[RC]*V[RC];
	}

	V[R] = V[RC];

	V[NAGG] = V[N_AGG]*s;
	if (SASFIT_EQUAL(V[NAGG], 0.0))
	{
		V[V_CORE] = 0.0;
	}
	else
	{
		V[V_CORE] = vol * (1.0 - V[XSOLV_CORE]) / V[NAGG];
	}

	ELLIP_RWBRUSH_BODY;
}

