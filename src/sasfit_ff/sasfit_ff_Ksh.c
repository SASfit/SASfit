/*
 * src/sasfit_ff/sasfit_ff_Ksh.c
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


scalar sasfit_ff_Ksh(scalar q, sasfit_param * param)
{
	scalar k1, k2;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	sasfit_init_param( &subParam );
	subParam.p[0] = param->p[0];
	subParam.p[3] = param->p[2];
	k1 = sasfit_ff_sphere_f(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	subParam.p[0] = param->p[1];
	subParam.p[3] = param->p[2] - param->p[3];
	k2 = sasfit_ff_sphere_f(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	//return K(interp,q,R1,eta1,error) - K(interp,q,R2,eta1-eta2,error);
	return k1 - k2;
}
