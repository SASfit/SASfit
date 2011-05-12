/*
 * src/sasfit_peaks/sasfit_peak_VoigtPeakArea.c
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
#include <math.h>
#include "include/sasfit_peaks_utils.h"
#include <f2c.h>

#define A param->p[0]
#define MEAN param->p[1]
#define SIGMA param->p[2]
#define GAMMA param->p[3]
#define BACKGR param->p[4]


scalar sasfit_peak_VoigtPeakArea(scalar x, sasfit_param * param)
{
	scalar xactual,xi,yi,u,v;
	logical flag;
	scalar vp;

	SASFIT_ASSERT_PTR( param );

	SASFIT_CHECK_COND1((SIGMA <= 0), param, "sigma(%lg) <= 0",SIGMA);
	SASFIT_CHECK_COND1((GAMMA <  0), param, "gamma(%lg) <  0",GAMMA);

	xactual = (x-MEAN);
//	xi = xactual/(SIGMA*sqrt(2.0));
//	yi = GAMMA/(SIGMA*sqrt(2.0));
	xi = 2.* sqrt(log(2.))*xactual/SIGMA;
	yi = 2.* sqrt(log(2.))*GAMMA/SIGMA;
	flag = FALSE;
	sasfit_wofz(&xi, &yi, &u, &v, &flag);
	if (flag) {
		sasfit_param_set_err(param, "OVERFLOW WILL OCCUR in wofz (Voigt Function)");
		return 0.0;
	}
	vp = // A*u/(SIGMA*sqrt(2.0*M_PI))
		 A*(2.*sqrt(log(2)*M_PI)*u/SIGMA)
		+BACKGR;
//	sasfit_out("x:%lg vp:%lg \n",xactual,vp);
	return vp;
}

