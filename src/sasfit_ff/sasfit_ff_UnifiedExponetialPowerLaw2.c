/*
 * src/sasfit_ff/sasfit_ff_UnifiedExponetialPowerLaw2.c
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

/*
J. Appl. Cryst. (1996). 29, 134-146
Small-Angle Scattering from Polymeric Mass Fractals of Arbitrary Mass-Fractal Dimension
G. BEAUCAGE
*/

scalar sasfit_ff_UnifiedExponetialPowerLaw2(scalar q, sasfit_param * param)
{
	scalar x, xs, tmp, G, B, Rg_i, Rg_ip1, k, P;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 6, &G, &B, &Rg_i, &Rg_ip1, &k, &P);

	SASFIT_CHECK_COND1((q < 0), param, "q(%lg) < 0", q);

	x = q*Rg_i;
	xs = q*Rg_ip1;

	tmp = gsl_pow_3( gsl_sf_erf(x *k /sqrt(6.)) ) / q;

	return G*exp(-x*x/3.0) + B*exp(-xs*xs/3.)*pow(tmp,P);
}


