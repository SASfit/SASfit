/*
 * src/sasfit_peaks/sasfit_peak_VoigtPeakAmplitude.c
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
#include <sasfit_common_stubs.h>


#define A param->p[0]
#define MEAN param->p[1]
#define SIGMA param->p[2]
#define GAMMA param->p[3]
#define DUMMY param->p[4]
#define BACKGR param->p[5]

scalar sasfit_peak_voigt_amplitude(scalar x, sasfit_param * param)
{
	scalar xactual,xi,yi,u,v;
	logical flag;
	scalar vp;

	SASFIT_ASSERT_PTR( param );

	SASFIT_CHECK_COND1((SIGMA <= 0), param, "sigma(%lg) <= 0",SIGMA);
	SASFIT_CHECK_COND1((GAMMA <  0), param, "gamma(%lg) <  0",GAMMA);

// calculate amplitude
	xactual = 0.0;
	xi = xactual/(SIGMA*sqrt(2.0));
	yi = GAMMA/(SIGMA*sqrt(2.0));
	flag = FALSE;
	sasfit_wofz(&xi, &yi, &u, &v, &flag);
	if (flag) {
		sasfit_param_set_err(param, "OVERFLOW WILL OCCUR in wofz (Voigt Function)");
		return 0.0;
	}
	vp = u/(SIGMA*sqrt(2.0*M_PI));

	xactual = (x-MEAN);
	xi = xactual/(SIGMA*sqrt(2.0));
	yi = GAMMA/(SIGMA*sqrt(2.0));
	flag = FALSE;
	sasfit_wofz(&xi, &yi, &u, &v, &flag);
	if (flag) {
		sasfit_param_set_err(param, "OVERFLOW WILL OCCUR in wofz (Voigt Function)");
		return 0.0;
	}
	vp = A/vp*u/(SIGMA*sqrt(2.0*M_PI))
//		+A*(2.*sqrt(log(2)*M_PI)*u/sigma)
		+BACKGR;
	return vp;
}

scalar sasfit_peak_voigt_amplitude_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_peak_voigt_amplitude_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}