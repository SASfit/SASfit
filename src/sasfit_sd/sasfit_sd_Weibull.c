/*
 * src/sasfit_sd/sasfit_sd_Weibull.c
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
float Weibull(Tcl_Interp *interp,
			  float N,
              float alpha,
              float l,
              float mu,
              float q,
              bool *error)
	      */
/**
 * calculates a Weibull distribution function
 */
scalar sasfit_sd_Weibull(scalar x, sasfit_param * param)
{
	scalar N, alpha, l, mu;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 4, &N, &alpha, &l, &mu);
	
	SASFIT_CHECK_COND1((alpha < 0.0), param, "alpha(%lg) < 0",alpha);
	SASFIT_CHECK_COND1((l < 0.0), param, "l(%lg) < 0",l);
	SASFIT_CHECK_COND1((mu < 0.0), param, "mu(%lg) < 0",mu);
//	SASFIT_CHECK_COND2((x < mu), param, "x(%lg) < mu(%lg)",x,mu);

    if (x<mu) return 0;

	return N*l/alpha * pow((x-mu)/alpha,l-1.0) * exp(-pow((x-mu)/alpha,l));
}

