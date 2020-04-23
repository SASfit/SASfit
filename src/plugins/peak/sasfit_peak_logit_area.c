/*
 * src/sasfit_peaks/sasfit_peak_LogisticArea.c
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

#include "include/private.h"
#include <sasfit_error_ff.h>

#define AREA	param->p[0]
#define XMIN	param->p[1]
#define XMAX	param->p[2]
// #define MODE  	param->p[3]
#define MU  	param->p[3]
#define SIGMA	fabs(param->p[4])
#define BACKGR	param->p[5]
//#define MU      param->p[MAXPAR-1]

scalar sasfit_peak_logit_area(scalar x, sasfit_param * param)
{
	scalar z, zMODE, xmin, xmax;

	SASFIT_ASSERT_PTR( param );


	if (XMIN>XMAX) {
		xmin = XMAX;
		xmax = XMIN;
	} else {
		xmin = XMIN;
		xmax = XMAX;
	}
	SASFIT_CHECK_COND2((xmin == xmax), param, "xmin(%lg) == xmax(%lg)",xmin,xmax);
	SASFIT_CHECK_COND1((SIGMA == 0), param, "sigma(%lg) == 0",SIGMA);
//	SASFIT_CHECK_COND2((MODE <= xmin), param, "mode(%lg) <= xmin",MODE,xmin);
//	SASFIT_CHECK_COND2((MODE >= xmax), param, "mode(%lg) >= xmax",MODE,xmax);


	if (x<=xmin) return BACKGR;
	if (x>=xmax) return BACKGR;

	z = (x-xmin)/(xmax-xmin);
//	zMODE = (MODE-xmin)/(xmax-xmin);
//    MU = log(zMODE/(1-zMODE))-SIGMA*SIGMA*(2*zMODE-1);
	return BACKGR
			+ AREA/(xmax-xmin) / (SIGMA*M_SQRT2*M_SQRTPI * z*(1-z)) * exp(-gsl_pow_2((log(z/(1-z))-MU)/SIGMA)/2.);
}

scalar sasfit_peak_logit_area_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_peak_logit_area_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}
