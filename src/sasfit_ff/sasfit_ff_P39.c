/*
 * src/sasfit_ff/sasfit_ff_P39.c
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
#include "include/sasfit_ff_utils.h"

/*
float P39(Tcl_Interp *interp,
			float q,
			float Nc,	// aggregation number
			float Rg,	// gyration radius of chains
			float Rc,	// radius of core
			float rhos, // excess scattering length of spherical core
			float rhoc, // excess scattering length of chains
			float L,	 // contour length
			float b,	 // Kuhn length
			bool *error)
*/
scalar sasfit_ff_P39(scalar q, sasfit_param * param)
{
	scalar s, R, Mthirtynine, sigma, nu, res;
	scalar Nc, Rg, Rc, rhos, rhoc, L, b;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 7, &Nc, &Rg, &Rc, &rhos, &rhoc, &L, &b);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((L <= 0.0), param, "L(%lg) <= 0",L);
	SASFIT_CHECK_COND1((b < 0.0), param, "b(%lg) < 0",b);

	s = Rg;
	R = Rc;
	Mthirtynine = rhos + Nc*rhoc;

	if (Rc == 0.0)
	{
		sigma = Nc/4.;
	}

	sigma = Nc*pow(Rg,2.0)/(4.0*pow(R+Rg,2.0));
	nu = 1.4*pow(sigma,1.04);

	sasfit_init_param( &subParam );
	subParam.p[0] = Nc;
	subParam.p[1] = Rg;
	subParam.p[2] = R;
	subParam.p[3] = Rc;
	subParam.p[4] = s;
	subParam.p[5] = nu;
	subParam.p[6] = Mthirtynine;
	subParam.p[7] = rhos;
	subParam.p[8] = rhoc;
	subParam.p[9] = b;
	subParam.p[10] = L;

	// return sasfit_Pthirtynine(interp, q, Nc, Rg, R, Rc, s, nu, Mthirtynine, rhos, rhoc, b, L, error);
	res = sasfit_Pthirtynine(q, subparam);

	SASFIT_CHECK_SUB_ERR(param, subParam);

	return res;
}


