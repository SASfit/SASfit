/*
 * src/sasfit_peaks/sasfit_peak_BetaArea.c
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
 *   3rd of August 2016
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

#define AREA	param->p[0]
#define XMIN	param->p[1]
#define XMAX	param->p[2]
#define BALPHA	param->p[3]
#define BBETA	param->p[4]
#define BCKGR	param->p[5]


scalar sasfit_peak_beta_area(scalar x, sasfit_param * param)
{
	scalar z, xmin, xmax;
	SASFIT_CHECK_COND2((XMIN == XMAX), param, "xmin(%lg) == xmax(%lg)",XMIN,XMAX);
	SASFIT_CHECK_COND1((BALPHA <= 0.0), param, "alpha(%lg) <= 0",BALPHA);
	SASFIT_CHECK_COND1((BBETA  <= 0.0), param, "beta(%lg) <= 0",BBETA);

	if (XMIN>XMAX) {
		xmin = XMAX;
		xmax = XMIN;
	} else {
		xmin = XMIN;
		xmax = XMAX;
	}

	if (x<=xmin) return BCKGR;
	if (x>=xmax) return BCKGR;

	z = (x-xmin)/(xmax-xmin);

	return BCKGR+AREA*pow(z,BALPHA-1.0)*pow(1.0-z,BBETA-1.0)/gsl_sf_beta(BALPHA,BBETA)/(xmax-xmin);
}

scalar sasfit_peak_beta_area_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_peak_beta_area_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

