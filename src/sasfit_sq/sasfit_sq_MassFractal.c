/*
 * src/sasfit_sq/sasfit_sq_MassFractal.c
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
#include "include/sasfit_sq_utils.h"


/**
 * form factor of a mass fractal consisting of spheres with a radius R, a
 * fractal dimesnion of D, a cut-off length of xi and a scattering length
 * density eta
 */
scalar sasfit_sq_MassFractal(scalar q, sasfit_param * param)
{
	scalar P16, R, xi, D;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 3, &R, &xi, &D);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R <= 0.0), param, "R(%lg) <= 0",R);
	SASFIT_CHECK_COND2((xi < R), param, "xi(%lg) < R(%lg)",xi,R);
	SASFIT_CHECK_COND1((D <= 1.0), param, "D(%lg) <= 1",D);

	if ((xi == 0) || (R == 0)) return 1.0;

	P16 = 1.0/pow(q*R,D);
	P16 = P16*D*gsl_sf_gamma(D-1.0)/pow(1.0+1.0/(q*q*xi*xi),(D-1.0)/2.0);
	P16 = P16*sin((D-1.0)*atan(q*xi));
	P16 = (1.0+P16);
	return P16;
}
