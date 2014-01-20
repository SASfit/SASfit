/*
 * src/sasfit_ff/sasfit_ff_Langevin.c
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


/*
 * Magnetization curve of superparamagnetic particles following the Labgevin
 * statistics. Magnetic particles consists of spheres with a radius R,
 * magnetization Ms at temperature T.
 * [x] = 1 Gauss = 1 Oe, [R] = 1 nm, [Ms] = 1 A/m, [T] = 1 K, [M0] = 1 emu
 */
scalar sasfit_ff_LangevinMH(scalar x, sasfit_param * param)
{
	scalar R, Ms, T, M0, alpha, k;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 4, &R, &Ms, &T, &M0);

	k = 1.3806505*1e-23;
	alpha = x/1.0e4*Ms*4.0/3.0*M_PI * R*R*R * 1e-27/(k*T);

	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R);
	SASFIT_CHECK_COND((T == 0.0), param, "T == 0");

	if (alpha == 0.0) 
	{
		return 0.0;
	}

	switch( param->kernelSelector )
	{
		case LANGEVIN1:
			return M0*(4.0/3.0*M_PI*R*R*R)*(1.0/tanh(alpha) - 1.0/alpha);
			break;
		case LANGEVIN2:
			return M0*(1.0/tanh(alpha) - 1.0/alpha);
			break;
		default:
			SASFIT_ERR_UNKNOWN_KERNEL(param, "kernel");
			break;
	}
}

