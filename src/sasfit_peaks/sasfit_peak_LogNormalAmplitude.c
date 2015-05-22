/*
 * src/sasfit_peaks/sasfit_peak_LogNormalAmplitude.c
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
 */

#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>
#include "include/sasfit_peaks_utils.h"

#define AMPL		param->p[0]
#define MU			param->p[1]
#define SIGMA		param->p[2]
#define LOCATION	param->p[3]
#define BACKGR		param->p[4]
#define P			param->p[MAXPAR-1]

scalar sasfit_peak_LogNormalAmplitude(scalar x, sasfit_param * param)
{
	scalar c, z, mode;

	SASFIT_ASSERT_PTR( param );

	SASFIT_CHECK_COND1((MU <= 0.0), param, "mu(%lg) <= 0", MU);
	SASFIT_CHECK_COND1((SIGMA == 0.0), param, "sigma(%lg) == 0", SIGMA);

	P=1;
	z = (x-LOCATION);
	if (z <= 0.0) return BACKGR;

	mode = exp(-P*SIGMA*SIGMA)*MU;
// c= sqrt(2)*csgn(1/sigma)*sigma*sqrt(Pi)*exp((1/2*(-1+p))*(p*sigma^2-2*ln(mu)-sigma^2))
// Rn = exp((1/2)*n*(n*sigma^2-2*p*sigma^2+2*sigma^2+2*ln(mu)))


	c =   pow(mode, P) 
		* exp(0.5*pow(-P*SIGMA, 2.0));
	return AMPL*c* pow(z,-P) * exp(-0.5*pow(log(z/MU)/SIGMA,2.0)) + BACKGR;
}

