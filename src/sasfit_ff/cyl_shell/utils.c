/*
 * src/sasfit_ff/cyl_shell/utils.c
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

scalar sasfit_ff_cyl_shell_v(scalar q, sasfit_param * param, int distr)
{	
	scalar v;
	SASFIT_ASSERT_PTR(param);

	     if ( distr == 0 ) v = q    + V[DR];
	else if ( distr == 1 ) v = V[R] + q;
	else                   v = V[R] + V[DR];
	
	if ( distr != 2 ) q = V[L];

	return M_PI * q * pow(v, 2.0);
}

