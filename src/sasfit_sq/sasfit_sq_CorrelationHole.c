/*
 * src/sasfit_sq/sasfit_sq_CorrelationHole.c
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
#include "include/sasfit_sq_utils.h"

/**
 *   this structure factor has been removed from the menu interface of SASfit
 *   there are much better theoretically justified models
**/

/*
 * correlation hole structure factor
 * @param eta hole volume fraction
 * @param h hole radius
 */
scalar sasfit_sq_CorrelationHole(scalar q, sasfit_param * param)
{
	scalar h, eta;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 2, &h, &eta);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((eta < 0.0), param, "eta(%lg) < 0",eta);
	SASFIT_CHECK_COND1((h < 0.0), param, "h(%lg) < 0",h);

	q = q*h;
	return (1.0+eta*3*(q*cos(q)-sin(q))/pow(q,3.0));
}
