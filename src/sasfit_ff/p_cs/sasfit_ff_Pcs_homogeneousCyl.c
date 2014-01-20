/*
 * src/sasfit_ff/sasfit_ff_Pcs_homogeneousCyl.c
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
#include "../include/sasfit_ff_utils.h"

#define R		param->p[0]
#define ETA_C	param->p[1]
#define ETA_SOL	param->p[2]

scalar sasfit_ff_Pcs_homogeneousCyl(scalar q, sasfit_param * param)
{
	scalar res,u;

	SASFIT_ASSERT_PTR(param);

	SASFIT_CHECK_COND1((q < 0.0),      param, "q(%lg) < 0", q);
	SASFIT_CHECK_COND1((R < 0.0),      param, "R(%lg) < 0", R);
	
	u = q*R;
	if (u==0) {
		res = M_PI*R*R*(ETA_C-ETA_SOL)*2;
	} else {
		res = M_PI*R*R*(ETA_C-ETA_SOL)*2*gsl_sf_bessel_J1(u)/u;
	}
	return res*res;
}

