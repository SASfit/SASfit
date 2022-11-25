/*
 * src/sasfit_sd/sasfit_sd_fractalSD.c
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
#include "include/sasfit_sd_utils.h"

/*
float fractalSD(Tcl_Interp *interp,
                   float N,
                   float Rmin,
                   float Rmax,
		   float fD,
		   float q,
                   bool  *error)
		   */
/**
 * Calculates a fractal size distribution function.
 */
scalar sasfit_sd_fractalSD(scalar x, sasfit_param * param)
{
	scalar N, Rmin, Rmax, fD;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 4, &N, &Rmin, &Rmax, &fD);

	SASFIT_CHECK_COND2((Rmin > Rmax), param, "Rmin(%lg) > Rmax(%lg)", Rmin, Rmax);
	SASFIT_CHECK_COND1((Rmin <= 0.0), param, "Rmin(%lg) <= 0", Rmin);
	SASFIT_CHECK_COND((fD == 0.0), param, "fD == 0");

	if ((x < Rmin) || (x > Rmax)) return 0.0;

//	SASFIT_CHECK_COND2((x < Rmin), param, "q(%lg) < Rmin(%lg)", x, Rmin);
//	SASFIT_CHECK_COND2((x > Rmax), param, "q(%lg) > Rmax(%lg)", x, Rmax);

	return N*fD*pow(x,-(1.0+fD))/(pow(Rmin,-fD)-pow(Rmax,-fD));
}

