/*
 * src/sasfit_ff/ellip/sasfit_ff_ellip_2.c
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

scalar sasfit_ff_ellip_2(scalar q, sasfit_param * param)
{
	ELLIP_HEADER;

	V[NU]	= V[PARAM1];

	SASFIT_CHECK_COND1((V[NU] < 0.0), param, "nu(%lg) < 0",V[NU]);

	if (SASFIT_EQUAL(V[NU], 0.0)) return 0.0;
	sum = sasfit_integrate(0.0, M_PI_2, sasfit_ff_ellip_2_core, param);

	ELLIP_BODY;
}

