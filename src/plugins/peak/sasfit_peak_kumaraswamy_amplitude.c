/*
 * src/sasfit_peaks/sasfit_peak_KumaraswamyAmplitude.c
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
#define XMIN	param->p[1]
#define XMAX	param->p[2]
#define ALPHA	param->p[3]
#define BETA	param->p[4]
#define BCKGR	param->p[5]

scalar sasfit_peak_kumaraswamy_help(scalar x, sasfit_param * param)
{
	scalar z, xmin, xmax;
	
	SASFIT_ASSERT_PTR( param );

	if (XMIN>XMAX) {
		xmin = XMAX;
		xmax = XMIN;
	} else {
		xmin = XMIN;
		xmax = XMAX;
	}

	if (x<xmin) return 0.0;
	if (x>xmax) return 0.0;

	z = (x-xmin)/(xmax-xmin);

	return pow(z,ALPHA-1.0)*pow(1.0-pow(z,ALPHA),BETA-1.0);
}

scalar sasfit_peak_kumaraswamy_mode(sasfit_param * param)
{
	scalar xmin,xmax;
	if (XMIN>XMAX) {
		xmin = XMAX;
		xmax = XMIN;
	} else {
		xmin = XMIN;
		xmax = XMAX;
	}
	return 	pow((ALPHA-1.0)/(ALPHA*BETA-1.),1.0/ALPHA)*(xmax-xmin)+xmin;
}

scalar sasfit_peak_kumaraswamy_amplitude(scalar x, sasfit_param * param)
{
	scalar mode, a0;
	
	SASFIT_ASSERT_PTR( param );

	SASFIT_CHECK_COND2((XMIN == XMAX), param, "xmin(%lg) == xmax(%lg)",XMIN,XMAX);
	SASFIT_CHECK_COND1((ALPHA <= 1.0), param, "alpha(%lg) <= 1",ALPHA);
	SASFIT_CHECK_COND1((BETA  <= 1.0), param, "beta(%lg) <= 1",BETA);
	
    mode = sasfit_peak_kumaraswamy_mode(param);
	a0 = AMPL/sasfit_peak_kumaraswamy_help(mode,param);
	return BCKGR+a0*sasfit_peak_kumaraswamy_help(x,param);
}


scalar sasfit_peak_kumaraswamy_amplitude_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_peak_kumaraswamy_amplitude_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}