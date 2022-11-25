/*
 * src/sasfit_ff/sasfit_ff_StickyHardSphere.c
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
#include <sasfit_sq.h>
#include "include/sasfit_ff_utils.h"

/*
double StickyHardSphere(Tcl_Interp *interp,
					double Q,
					double R,
					double RHS,
					double tau,
					double fp,
					double eta,
					bool  *error)
*/
scalar sasfit_ff_StickyHardSphere(scalar q, sasfit_param * param)
{
	scalar res;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	sasfit_init_param( &subParam );
	subParam.p[0] = param->p[0];
	subParam.p[3] = param->p[4];
	res = sasfit_ff_sphere_f(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	subParam.p[0] = param->p[1];
	subParam.p[1] = param->p[2];
	subParam.p[2] = param->p[3];
	res = res*res * sasfit_sq_StickyHardSphere(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	// return S_StickyHardSphere(interp,q,RHS,tau,fp,error)*K(interp,q,R,eta,error)*K(interp,q,R,eta,error);
	return res;
}

