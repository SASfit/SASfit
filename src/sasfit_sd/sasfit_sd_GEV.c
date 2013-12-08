/*
 * src/sasfit_sd/sasfit_sd_GEV.c
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
float GEV(Tcl_Interp *interp,
				   float N,
                   float xi,
                   float sigma,
				   float mu,
				   float x,
                   bool  *error)
		*/
/**
 * Calculates a generalized extreme value distribution function.
 */
scalar sasfit_sd_GEV(scalar x, sasfit_param * param)
{
	scalar N, xi, sigma, mu, z, c;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 4, &N, &xi, &sigma, &mu);

//	SASFIT_CHECK_COND1((mu < 0.0), param, "mu(%lg) < 0", mu);
	SASFIT_CHECK_COND1((sigma <= 0.0), param, "sigma(%lg) <= 0", sigma);
	SASFIT_CHECK_COND((xi == 0.0), param, "xi == 0");
//	SASFIT_CHECK_COND2((x <= mu-sigma/xi), param, "x(%lg) <= mu-sigma/xi (%lg)", x, mu-sigma/xi);

	z = (x-mu)/sigma;
	if (xi > 0.0) {
		c = 1;
		if (x > mu-sigma/xi){
			return N/c*1.0/sigma*pow(1.+xi*z,-1.-1./xi)*exp(-pow(1.+xi*z,-1./xi));
		}
	} else if (xi < 0.0) {
		c = 1-exp(-pow(1-xi*mu/sigma,-1/xi));
		if (x<mu-sigma/xi){
			return N/c*1.0/sigma*pow(1.+xi*z,-1.-1./xi)*exp(-pow(1.+xi*z,-1./xi));
		}
	}
	return 0.0;
}
