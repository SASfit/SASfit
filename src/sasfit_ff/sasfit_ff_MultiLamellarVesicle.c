/*
 * src/sasfit_ff/sasfit_ff_MultiLamellarVesicle.c
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

/**
 //
 // MultiLamellarVesicle: 
 //	 l[0]: R_c: core radius, core contains solvent
 //	 l[1]: t_sh: thickness of surfactant layer thickness 
 //	 l[2]: t_sol: thickness of solvent layer
 //	 l[3]: eta_sh: scattering length density of surfactant shell 
 //	 l[4]: eta_sol: scattering length density of solvent as well as core
 //	 l[5]: n: number of layers
 //
 */

#define R_C		param->p[0]
#define T_SH	param->p[1]
#define T_SOL	param->p[2]
#define ETA_SH	param->p[3]
#define ETA_SOL	param->p[4]
#define N		param->p[5]

scalar sasfit_ff_MultiLamellarVesicle(scalar q, sasfit_param * param)
{
	scalar sum;
	int i, nlayers;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R_C < 0.0), param, "R_C(%lg) < 0",R_C);
	SASFIT_CHECK_COND1((T_SOL < 0.0), param, "T_SOL(%lg) < 0",T_SOL);
	SASFIT_CHECK_COND1((T_SH < 0.0), param, "T_SH(%lg) < 0",T_SH);
	SASFIT_CHECK_COND1((N < 1.0), param, "N(%lg) < 1",N);

	sasfit_init_param( &subParam );

	nlayers = (int) N;
	sum = 0.;
	for (i=0; i < nlayers ;i++) 
	{
		subParam.p[0] = R_C + i*T_SH + i*T_SOL;
		subParam.p[3] = -(ETA_SH-ETA_SOL);
		sum += sasfit_ff_sphere_f(q, &subParam);

		subParam.p[0] = R_C + (i+1)*T_SH + i*T_SOL;
		subParam.p[3] = ETA_SH-ETA_SOL;
		sum += sasfit_ff_sphere_f(q, &subParam);
	}
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return sum*sum;
}

