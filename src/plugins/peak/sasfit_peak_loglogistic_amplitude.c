/*
 * src/sasfit_peaks/sasfit_peak_LogLogisticAmplitude.c
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

#define AMPL		param->p[0]
#define LOCATION	param->p[1]
#define MU			param->p[2]
#define SIGMA		param->p[3]
#define DUMMY2		param->p[4]
#define BCKGR		param->p[5]
#define MODE		param->p[MAXPAR-1]

#include "include/private.h"
#include <sasfit_error_ff.h>


scalar sasfit_peak_loglogistic_amplitude(scalar x, sasfit_param * param)
{
	scalar a0,z;
	SASFIT_ASSERT_PTR( param );

	SASFIT_CHECK_COND1((SIGMA <= 0), param,  "sigma(%lg) <= 0 ",SIGMA);
	SASFIT_CHECK_COND1((SIGMA >= 1), param,  "sigma(%lg) >= 1 ",SIGMA);

	if ((x<=LOCATION) || fabs(gsl_sf_log((x-LOCATION)/(MU)))/SIGMA > 100) return BCKGR;

	
	MODE = (MU+LOCATION)*pow((1.0-SIGMA)/(1.0+SIGMA),SIGMA);
	SASFIT_CHECK_COND2((MODE <= LOCATION), param,  "mode(%lg) <= location(%lg) ",MODE,LOCATION);

	z = gsl_sf_log((MODE-LOCATION)/(MU))/SIGMA;
	a0 = exp(-z)/pow(1.0+exp(-z),2.0) / (MODE-LOCATION);

	z = gsl_sf_log((x-LOCATION)/(MU))/SIGMA;
	return BCKGR
			+ AMPL/a0 * exp(-z)/pow(1.0+exp(-z),2.0) / (x-LOCATION);
}

scalar sasfit_peak_loglogistic_amplitude_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_peak_loglogistic_amplitude_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}