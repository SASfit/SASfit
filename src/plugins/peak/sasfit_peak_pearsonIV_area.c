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

#include "include/private.h"
#include <sasfit_error_ff.h>

#define AREA	param->p[0]
#define CENTER	param->p[1]
#define WIDTH	param->p[2]
#define SHAPE1	param->p[3]
#define SHAPE2	param->p[4]
#define BACKGR	param->p[5]

scalar sasfit_peak_pearsonIV_area(scalar x, sasfit_param * param)
{
	scalar z,u;
	scalar a0,a1,a2,a3,a4;
	scalar a_temp, l_temp, m_temp, nu_temp;
	gsl_sf_result lnr, carg;

	SASFIT_ASSERT_PTR( param );

	a1 = CENTER; 
	a2 = WIDTH; a_temp = WIDTH;
	a3 = SHAPE1; m_temp = SHAPE1;
	a4 = SHAPE2; nu_temp= SHAPE2;

	SASFIT_CHECK_COND1((WIDTH <= 0), param, "width(%lg) <= 0",WIDTH);
	SASFIT_CHECK_COND1((SHAPE1 <= 0.5), param, "shape1(%lg) <= 1/2",SHAPE1);

	u = a4/(2.*a3);
	l_temp = CENTER+a2*u;
	z = (x-l_temp)/WIDTH;
	
	gsl_sf_lngamma_complex_e (m_temp, 0.5*nu_temp, &lnr, &carg);

	a0 = AREA*pow(exp(lnr.val-gsl_sf_lngamma(m_temp)),2.0)/(a_temp*gsl_sf_beta(m_temp-0.5,0.5));
	return BACKGR+a0*pow(1.0+z*z,-m_temp)*exp(-nu_temp*atan(z));
}

scalar sasfit_peak_pearsonIV_area_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_peak_pearsonIV_area_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}