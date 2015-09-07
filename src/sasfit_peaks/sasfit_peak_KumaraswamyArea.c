/*
 * src/sasfit_peaks/sasfit_peak_KumaraswamyArea.c
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
#include "include/sasfit_peaks_utils.h"

#define AREA	param->p[0]
#define XMIN	param->p[1]
#define XMAX	param->p[2]
#define ALPHA	param->p[3]
#define BETA	param->p[4]
#define BCKGR	param->p[5]

scalar sasfit_peak_KumaraswamyArea(scalar x, sasfit_param * param)
{
	scalar z, xmin, xmax;
	
	SASFIT_ASSERT_PTR( param );

	SASFIT_CHECK_COND2((XMIN == XMAX), param, "xmin(%lg) == xmax(%lg)",XMIN,XMAX);
	SASFIT_CHECK_COND1((ALPHA <= 0.0), param, "alpha(%lg) <= 0",ALPHA);
	SASFIT_CHECK_COND1((BETA  <= 0.0), param, "beta(%lg) <= 0",BETA);
	
	if (XMIN>XMAX) {
		xmin = XMAX;
		xmax = XMIN;
	} else {
		xmin = XMIN;
		xmax = XMAX;
	}

	if (x<xmin) return BCKGR;
	if (x>xmax) return BCKGR;

	z = (x-xmin)/(xmax-xmin);

	return BCKGR+AREA*ALPHA*BETA*pow(z,ALPHA-1.0)*pow(1.0-pow(z,ALPHA),BETA-1.0)/(xmax-xmin);
}

