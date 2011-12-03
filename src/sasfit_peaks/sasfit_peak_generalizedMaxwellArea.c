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

#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>
#include "include/sasfit_peaks_utils.h"


scalar sasfit_peak_generalizedMaxwellArea(scalar x, sasfit_param * param)
{
	scalar bckgr, c,area, mode, offset,m,n, width;;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 6, &area, &width,&offset, &m,&n,&bckgr);

	SASFIT_CHECK_COND1((width == 0), param, "width(%lg) == 0",width);
	SASFIT_CHECK_COND1(((1+m)/n <= 0), param, "(1+m)/n (%lg) <= 0",(1+m)/n);
	
	mode=pow(2.0*m/n,1.0/n)*fabs(width)+offset;

    c = pow(2,(1.0+m)/n)*pow(fabs(width),1+m)*gsl_sf_gamma((1+m)/n)/fabs(n);
	if (x <= offset)  return bckgr;
	return bckgr+area/c*pow(x-offset,m)*exp(-0.5*pow((x-offset)/fabs(width),n));
}