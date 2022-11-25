/*
 * src/sasfit_sq/sasfit_sq_RandomDistributionModel.c
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
 */
scalar sasfit_sq_RandomDistributionModel(scalar q, sasfit_param * param)
{
	scalar Phi,x;
	scalar R, fp, epsilon;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 3, &R, &fp, &epsilon);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R <= 0.0), param, "R(%lg) <= 0",R);
	SASFIT_CHECK_COND1((fp <= 0.0), param, "fp(%lg) <= 0",fp);

	x = 2.0*q*R;
	Phi = 3.0*(sin(q)-q*cos(q))/pow(q,3.0);
	return 1.0/(1.0+8.0*fp*epsilon*Phi);
}
