/*
 * src/sasfit_sd/sasfit_sd_SchulzZimm.c
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
float Schultz_Zimm(Tcl_Interp *interp,
				   float N,
                   float Ra,
                   float sigma,
                   float q,
                   bool  *error)
		   */
/**
 * Calculates a Schultz-Zimm distribution function.
 */
scalar sasfit_sd_Schultz_Zimm(scalar x, sasfit_param * param)
{
	scalar N, Ra, sigma, c, k,SZ;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 3, &N, &Ra, &sigma);

	SASFIT_CHECK_COND((sigma == 0.0), param, "sigma == 0");
        
	k = 1.0/sigma/sigma;

	SASFIT_CHECK_COND1((k <= 0.0), param, "k(%lg) <= 0",k);
	SASFIT_CHECK_COND1((x < 0.0), param, "x(%lg) < 0",x);

        if ((x == 0.0) && (k > 1)) 
	{
           return 0.0;
        }
        if (Ra == 0.0) 
	{
           return 0.0;
        }
        if ((x == 0.0) && (k == 1)) 
	{
           return N/Ra;
        }

	SASFIT_CHECK_COND1((x == 0.0 && k < 1.0), param, "x == 0 && k(%lg) < 1",k);

        c = k*log(k) - sasfit_gammaln(k) - k*x/Ra;
        SZ = N/Ra*pow(x/Ra,k-1.0);

        return SZ * exp(c);
}

