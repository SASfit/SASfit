/*
 * src/sasfit_peaks/sasfit_peak_PearsonIVArea.c
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

scalar sasfit_peak_PearsonIVArea(scalar x, sasfit_param * param)
{
	scalar z,u;
	scalar bckgr, a0, area, center, width, shape1, shape2;
	scalar a1,a2,a3,a4;
	scalar a_temp, l_temp, m_temp, nu_temp;
	gsl_sf_result lnr, carg;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 6, &area, &center, &width, &shape1, &shape2, &bckgr);

//	a0 = area;
	a1 = center; 
	a2 = width; a_temp = width;
	a3 = shape1; m_temp = shape1;
	a4 = shape2; nu_temp= shape2;

	SASFIT_CHECK_COND1((width <= 0), param, "width(%lg) <= 0",width);
	SASFIT_CHECK_COND1((shape1 <= 0.5), param, "shape1(%lg) <= 1/2",shape1);

	u = a4/(2.*a3);
	l_temp = center+a2*u;
	z = (x-l_temp)/a_temp;
	
	gsl_sf_lngamma_complex_e (m_temp, 0.5*nu_temp, &lnr, &carg);

	a0 = area*pow(exp(lnr.val-gsl_sf_lngamma(m_temp)),2.0)/(a_temp*gsl_sf_beta(m_temp-0.5,0.5));
	return bckgr+a0*pow(1.0+z*z,-m_temp)*exp(-nu_temp*atan(z));
}

