/*
 * src/sasfit_sq/sasfit_sq_LocalOrderModel.c
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
#include "include/sasfit_sq_utils.h"

/**
 *   this structure factor has been removed from the menu interface of SASfit
 *   there are much better theoretically justified models
**/

/**
 * Random Distribution Model
 * eta hole volume fraction
 * hole radius
 */
scalar sasfit_sq_LocalOrderModel(scalar q, sasfit_param * param)
{
	scalar tmp, Phi, D, alpha, z;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 3, &D, &alpha, &z);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((D <= 0.0), param, "D(%lg) <= 0",D);
	SASFIT_CHECK_COND1((alpha <= 0.0), param, "alpha(%lg) <= 0",alpha);
	SASFIT_CHECK_COND1((z <= 0.0), param, "z(%lg) <= 0",z);

	tmp = alpha*q*D;
	if (tmp==0) Phi=0; else Phi = 3.0*(sin(tmp)-tmp*cos(tmp))/pow(tmp,3.0);

    if (q*D==0) return 1.0+4.0-z*Phi; else return 1.0+4.0*sin(q*D)/(q*D)-z*Phi;
}
