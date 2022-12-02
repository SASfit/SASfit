/*
 * src/sasfit_sd/sasfit_sd_LogNorm.c
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
#include "include/sasfit_sd_utils.h"

#define AREA	param->p[0]
#define SIGMA	param->p[1]
#define P	param->p[2]
#define R	param->p[3]

scalar sasfit_sd_LogNorm(scalar x, sasfit_param * param)
{
	scalar c, LNf;

	SASFIT_ASSERT_PTR( param );

	if (x <= 0.0) return 0.0;

	SASFIT_CHECK_COND1((R <= 0.0), param, "R(%lg) <= 0", R);
	SASFIT_CHECK_COND1((SIGMA == 0.0), param, "sigma(%lg) == 0", SIGMA);

// mode = exp(-p*sigma^2)*mu
// c= sqrt(2)*csgn(1/sigma)*sigma*sqrt(Pi)*exp((1/2*(-1+p))*(p*sigma^2-2*ln(mu)-sigma^2))
// Rn = exp((1/2)*n*(n*sigma^2-2*p*sigma^2+2*sigma^2+2*ln(mu)))

	c = 1.0/sqrt(M_PI*2.0)/fabs(SIGMA)/pow(R,1.0-P) / exp(0.5*pow(SIGMA*(1.0-P),2.0));
	LNf = AREA*c* pow(x,-P) * exp(-0.5*pow(log(x/R)/SIGMA,2.0));

	return LNf;
}
