/*
 * src/sasfit_ff/sasfit_ff_Rod.c
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
#include <gsl/gsl_sf.h>
#include "include/sasfit_ff_utils.h"

/**
 * form factor of a rod with length L and scattering length desity eta
 * @param q The scattering vector.
 * @param param Pointer to a structure containing an array of parameters.
 */
scalar sasfit_ff_Rod(scalar q, sasfit_param * param)
{
	scalar L, eta;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 4, &L, EMPTY, EMPTY, &eta);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L);

	if (q == 0.0) return eta*eta*L*L;
	if (L == 0.0) return 0.0;

	return eta*eta * L*L * (2.0*gsl_sf_Si(q*L)/(q*L)-pow(sin(q*L*0.5)/(q*L*0.5),2.0));
}

