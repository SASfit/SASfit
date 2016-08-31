/*
 * src/sasfit_peaks/sasfit_peak_generalizedMaxwellArea.c
 *
 * Copyright (c) 2008-2011, Paul Scherrer Institute (PSI)
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
#define OFFSET	param->p[1]
#define WIDTH   param->p[2]
#define MSHAPE	param->p[3]
#define NSHAPE	param->p[4]
#define BCKGR	param->p[5]
#define MODE	param->p[6]

scalar sasfit_peak_generalized_maxwell_area(scalar x, sasfit_param * param)
{
	scalar c;

	SASFIT_ASSERT_PTR( param );

	SASFIT_CHECK_COND1((WIDTH == 0), param, "width(%lg) == 0",WIDTH);
	SASFIT_CHECK_COND1((NSHAPE*MSHAPE <= 0), param, "n*m(%lg) <= 0, both values need to have the same sign",NSHAPE*MSHAPE);

	MODE=pow(2.0*MSHAPE/NSHAPE,1.0/NSHAPE)*fabs(WIDTH)+OFFSET;
	
	if (x <= OFFSET)  return BCKGR;
    c = pow(2,(1.0+MSHAPE)/NSHAPE)*pow(fabs(WIDTH),1+MSHAPE)*gsl_sf_gamma((1+MSHAPE)/NSHAPE)/fabs(NSHAPE);
	return BCKGR+AREA/c*pow(x-OFFSET,MSHAPE)*exp(-0.5*pow((x-OFFSET)/fabs(WIDTH),NSHAPE));
}

scalar sasfit_peak_generalized_maxwell_area_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_peak_generalized_maxwell_area_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

