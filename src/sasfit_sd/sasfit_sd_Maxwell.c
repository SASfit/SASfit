/*
 * src/sasfit_sd/sasfit_sd_Maxwell.c
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
float Maxwell(Tcl_Interp *interp,
			  float N,
              float s,
              float R0,
              float q,
              bool  *error)
	      */
/**
 * Calculates a Maxwell distribution function.
 */
scalar sasfit_sd_Maxwell(scalar x, sasfit_param * param)
{
	scalar N, s, R0, c, maxw, cmax, r;

        SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 3, &N, &s, &R0);

	SASFIT_CHECK_COND((s == 0.0), param, "s == 0");

        r = x - R0;
        if (r < 0.0) return 0.0;

        if (R0 < 0)
	{
           cmax = 1.0-1.0/s*sqrt(2.0)/sqrt(M_PI)*R0/sqrt(exp(R0*R0/s/s)) + 
                  sasfit_erf_pure( R0/sqrt(2.0)/s );
        } else 
	{ 
           cmax = 1.0; 
        }

        c = 4.0 * pow(2.0*s*s,-1.5) / sqrt(M_PI);
        maxw = N*c/cmax* r*r * exp(-r*r/(2.0*s*s));

        return maxw;
}

