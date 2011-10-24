/*
 * src/sasfit_sq/sasfit_sq_MassFractExp_mpow_x_a.c
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


#define R0		param->p[0]
#define XI		param->p[1]
#define D		param->p[2]
#define ALPHA	param->p[3]
#define Q		param->p[MAXPAR-1]

scalar sasfit_sq_MassFractExp_mpow_x_a_core(scalar x, sasfit_param * param)
{
	scalar h;

	SASFIT_ASSERT_PTR(param);

	h = exp(-pow(x/XI,ALPHA));

	return D/pow(R0,D)*pow(x,D-3.)* h * x*x*sin(Q*x)/(Q*x);
}

scalar sasfit_sq_MassFractExp_mpow_x_a(scalar q, sasfit_param * param)
{
	scalar res,C;

	SASFIT_ASSERT_PTR(param);


	if (q*XI > 100.) {
		Q = 100./XI;
		res = sasfit_integrate(0.0, GSL_POSINF, sasfit_sq_MassFractExp_mpow_x_a_core, param);
		C = res*pow(Q*XI,D);
		res = C*pow(q*XI,-D);
	} else {
		param->p[MAXPAR-1] = q;
		res = sasfit_integrate(0.0, GSL_POSINF, sasfit_sq_MassFractExp_mpow_x_a_core, param);
	}

	return 1.0+res;
}

