/*
 * src/sasfit_ff/ellip_rwbrush/sasfit_ff_ellip_rwbrush_nagg.c
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

scalar sasfit_ff_ellip_rwbrush_nagg(scalar q, sasfit_param * param)
{
	ELLIP_RWBRUSH_HEADER;

	V[NAGG]		= V[PARAM0];
	V[V_CORE]	= V[PARAM1];
	SASFIT_CHECK_COND1((V[V_CORE] < 0.0), param, "V_core(%lg) < 0",V[V_CORE]);
	V[R]  = pow(fabs(V[NAGG]*(V[V_CORE]/(1.0-V[XSOLV_CORE]))/(4/3*M_PI*V[EPSILON])), 1.0/3.0);

	ELLIP_RWBRUSH_BODY;
}

