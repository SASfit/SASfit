/*
 * src/sasfit_ff/sasfit_ff_porod_cyl.c
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
float I_Porod_Cylinder(Tcl_Interp *interp,
				 float q,
					  float R,
					  float L,
					  float eta,
					  bool  *error)
*/
/*
 * form factor of a spherical Cylinder with radius R, height L and scattering
 * length density eta
 */
scalar sasfit_ff_porod_cyl(scalar q, sasfit_param * param)
{
	scalar tmp, FC, LC, R, L, eta;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 4, &R, &L, EMPTY, &eta);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R);
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L);

	if (R == 0.0) return 0.0;
	if (L == 0.0) return 0.0;

	if (2.0*R/L > 1.25) 
	{
		return sasfit_ff_flat_cyl(q, param);
	} 
	else if (2.0*R/L < 0.75) 
	{
		return sasfit_ff_long_cyl(q, param);
	} else 
	{
		LC = sasfit_ff_long_cyl(q, param);
		FC = sasfit_ff_flat_cyl(q, param);
		tmp = (2.0*R/L-0.75)*2.0;
		return tmp*FC+(1.0-tmp)*LC;
	}
}

scalar sasfit_ff_porod_cyl_v(scalar R, sasfit_param * param, int distr)
{
	scalar v = R;

	SASFIT_ASSERT_PTR(param);

	if ( distr != 0 ) v = param->p[0];
	if ( distr != 1 ) R = param->p[1];

	return M_PI * R * pow(v, 2.0);
}
