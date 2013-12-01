/*
 * src/sasfit_peaks/sasfit_peak_LogLogisticArea.c
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

#define AREA		param->p[0]
#define LOCATION	param->p[1]
#define MU			param->p[2]
#define SIGMA		param->p[3]
#define BCKGR		param->p[4]
#define MODE		param->p[MAXPAR-1]

#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>
#include "include/sasfit_peaks_utils.h"


scalar sasfit_peak_LogLogisticArea(scalar x, sasfit_param * param)
{
	scalar z ;

	SASFIT_ASSERT_PTR( param );

	SASFIT_CHECK_COND1((SIGMA <= 0), param,  "sigma(%lg) <= 0 ",SIGMA);
	SASFIT_CHECK_COND1((SIGMA >= 1), param,  "sigma(%lg) >= 1 ",SIGMA);

	MODE = (MU+LOCATION)*pow((1.0-SIGMA)/(1.0+SIGMA),SIGMA);
	SASFIT_CHECK_COND2((MODE <= LOCATION), param,  "mode(%lg) <= location(%lg), you should increase the value for mu.",MODE,LOCATION);

	if (x<=LOCATION) return BCKGR;

	z = gsl_sf_log((x-LOCATION)/(MU))/SIGMA;
	return BCKGR
			+ AREA/SIGMA * exp(-z)/pow(1.0+exp(-z),2.0) / (x-LOCATION);
}
