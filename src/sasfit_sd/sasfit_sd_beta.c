/*
 * src/sasfit_sd/sasfit_sd_beta.c
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
#include "include/sasfit_sd_utils.h"

/**
 * calculates a Beta distribution function
 */
scalar sasfit_sd_Beta(scalar x, sasfit_param * param)
{
	scalar N, Xmin, Xmax, alpha, beta,z, xa,xb;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 5, &N, &Xmin, &Xmax, &alpha, &beta);

	SASFIT_CHECK_COND2((Xmin >= Xmax), param, "Xmin(%lg) >= Xmax(%lg)",Xmin,Xmax);
	SASFIT_CHECK_COND1((Xmin < 0), param, "Xmin(%lg) < 0",Xmin);
	SASFIT_CHECK_COND1((alpha <= 0), param, "alpha(%lg) <= 0",alpha);
	SASFIT_CHECK_COND1((beta <= 0), param, "beta(%lg) <= 0",beta);

	if ((x<Xmin) || (x>Xmax)) return 0;

	z = (x-Xmin)/(Xmax-Xmin);

	if (alpha == 0) {
		xa = 1.;
	} else 	{
		xa = pow(z,alpha-1.);
	}
	if (beta == 0) 	{
		xb = 1.;
	} else 	{
        xb = pow(1.-z,beta-1.);
	}

	return N/(Xmax-Xmin)*xa*xb/gsl_sf_beta(alpha,beta);
}
