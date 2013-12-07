/*
 * src/sasfit_ff/sasfit_ff_Fractal.c
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

/*
float Fractal(Tcl_Interp *interp,
					 float Q,
							float R,
							float xi,
					 float D,
							float eta,
							bool  *error)
*/
/**
 * form factor of a mass fractal consisting of spheres with a radius R, a
 * fractal dimesnion of D, a cut-off length of xi and a scattering length
 * density eta
 */
scalar sasfit_ff_Fractal(scalar q, sasfit_param * param)
{
	scalar P16, R, xi, D, eta, k_temp;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 4, &R, &xi, &D, &eta);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R);
	SASFIT_CHECK_COND1((xi < 0.0), param, "xi(%lg) < 0",xi);
	
	if ( xi == 0.0 || R == 0.0 )
	{
		return 0.0;
	}

	sasfit_init_param( &subParam );
	subParam.p[0] = R;
	subParam.p[3] = eta;
	k_temp = sasfit_ff_sphere_f(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	P16 = 1.0/pow(q*R,D);
	P16 = P16*D*exp(sasfit_gammaln(D-1.0))/pow(1.0+1.0/(q*q*xi*xi),(D-1.0)/2.0);
	P16 = P16*sin((D-1.0)*atan(q*xi));
	P16 = pow(xi,D)*(1.0+P16)*k_temp*k_temp;
	return P16;
}

