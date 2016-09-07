/*
 * src/sasfit_peaks/sasfit_peak_PearsonIVAmplitude.c
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

#define AMPL	param->p[0]
#define CENTER	param->p[1]
#define WIDTH	param->p[2]
#define SHAPE1	param->p[3]
#define SHAPE2	param->p[4]
#define BACKGR	param->p[5]

scalar sasfit_peak_pearsonIV_amplitude(scalar x, sasfit_param * param)
{
	scalar z,u;
	scalar a0,a1,a2,a3,a4;
	scalar a_temp, l_temp, m_temp, nu_temp;

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

	a0 = AMPL*exp(-a4*atan(u))/pow(1+u*u,-a3);
    
// mode: lambda-a*nu/(2*m)  => lambda = mode + a*nu/(2*m) 
// l_temp=lambda
// center = mode
// m = a3
// nu = a4
// a = a2
// mean == E[X] lambda-a*nu/(2*(m-1)) for m>1
// variance = (standard deviation)^2 = E[(X-mean)^2]  = E[X^2]-(E[X])^2 = a^2*(r^2+nu^2) / (r^2*(r-1)) with r=2*(m-1)  for m>3/2

// ref: http://www-cdf.fnal.gov/physics/statistics/notes/cdf6820_pearson4.pdf

	return BACKGR+a0*pow(1+z*z,-a3)*exp(-a4*atan(z));
}

scalar sasfit_peak_pearsonIV_amplitude_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_peak_pearsonIV_amplitude_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}