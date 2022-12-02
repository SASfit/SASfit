/*
 * src/sasfit_ff/sasfit_ff_spherical_shell.c
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

#include <gsl/gsl_math.h>
#include "include/sasfit_ff_utils.h"

scalar sasfit_ff_spherical_shell(scalar q, sasfit_param * param)
{
	scalar res;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	sasfit_init_param( &subParam );

	switch( param->kernelSelector )
	{
		case SPH_SHELL1:
			subParam.p[0] = param->p[0];
			subParam.p[1] = param->p[1];
			subParam.p[2] = param->p[3];	// == eta
			subParam.p[3] = param->p[2]*param->p[3];
			break;
		case SPH_SHELL2:
			subParam.p[0] = param->p[0];
			subParam.p[1] = param->p[0] * param->p[1];
			subParam.p[2] = param->p[3];	// == eta
			subParam.p[3] = param->p[2]*param->p[3];
			break;
		case SPH_SHELL3:
			subParam.p[0] = param->p[0] + param->p[1];
			subParam.p[1] = param->p[0];
			subParam.p[2] = param->p[3];	// == eta
			subParam.p[3] = param->p[2];
			break;
		default:
			SASFIT_ERR_UNKNOWN_KERNEL(param, "kernel");
			break;
	}

	res = sasfit_ff_Ksh(q, &subParam);

	SASFIT_CHECK_SUB_ERR(param, subParam);

	return res*res;
}

scalar sasfit_ff_spherical_shell_v(scalar x, sasfit_param * param, int distr)
{
	SASFIT_ASSERT_PTR(param);

	if ( param->kernelSelector == SPH_SHELL3 ||
	     param->kernelSelector == KSHLIN1 )
	{
		     if ( distr == 0 ) x =           x + param->p[1];
		else if ( distr == 1 ) x = param->p[0] + x;
		else                   x = param->p[0] + param->p[1];
		
		distr = 0;
	}
	return sasfit_ff_sphere_v(x, param, distr);
}

