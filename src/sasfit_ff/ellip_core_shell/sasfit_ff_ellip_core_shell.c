/*
 * src/sasfit_ff/ellip_core_shell/sasfit_ff_ellip_core_shell.c
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

scalar sasfit_ff_ellip_core_shell(scalar q, sasfit_param * param)
{
	ELLIP_CORE_SHELL_INTEGRATE(ellip_core_shell_core);
}

scalar ellip_core_shell_core(scalar mu, sasfit_param * param)
{
	CALC_RADII;
	return (r1+r2)*(r1+r2);
}


scalar sasfit_ff_ellip_core_shell_f(scalar q, sasfit_param * param)
{
	ELLIP_CORE_SHELL_INTEGRATE(ellip_core_shell_f_core);
}

scalar ellip_core_shell_f_core(scalar mu, sasfit_param * param)
{
	CALC_RADII;
	return (r1+r2);
}


scalar sasfit_ff_ellip_core_shell_v(scalar q, sasfit_param * param, int distr)
{
	scalar factor;

	SASFIT_ASSERT_PTR(param);

	switch ( distr )
	{
		case 0:
			factor = q    + V[T];
			q      = V[B] + V[T];
			break;
		case 1:
			factor = V[A] + V[T];
			q      = q    + V[T];
			break;
		case 2:
			factor = V[A] + q;
			q      = V[B] + q;
			break;
		default:
			factor = V[A] + V[T];
			q      = V[B] + V[T];
			break;
	}

	return 4.0/3.0 * M_PI * factor * pow(q, 2.0);
}

