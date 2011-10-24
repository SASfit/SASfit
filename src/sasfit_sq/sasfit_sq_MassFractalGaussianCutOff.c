/*
 * src/sasfit_sq/sasfit_sq_MassFractalGaussianCutOff.c
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
 *   Ingo Bressler (ingo@cs.tu-berlin.de)
 */

#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_errno.h>
#include "include/sasfit_sq_utils.h"


/**
 * form factor of a mass fractal consisting of spheres with a radius R, a
 * fractal dimesnion of D, a cut-off length of xi and a scattering length
 * density eta
 */
scalar sasfit_sq_MassFractalGaussianCutOff(scalar q, sasfit_param * param)
{
	scalar P16, r0, xi, D;
	int status;

	gsl_sf_result pFq_1F1;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 3, &r0, &xi, &D);

	gsl_set_error_handler_off();

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((r0 <= 0.0), param, "r0(%lg) <= 0",r0);
	SASFIT_CHECK_COND2((xi < r0), param, "xi(%lg) < r0(%lg)",xi,r0);
	SASFIT_CHECK_COND1((D <= 1.0), param, "D(%lg) <= 1",D);

	if ((xi == 0) || (r0 == 0)) 
	{
		return 1.0;
	}
	P16 = gsl_sf_gamma(D/2.)*D/2.;
	P16 = P16*pow(xi/r0,D);

	status = gsl_sf_hyperg_1F1_e(D/2.,1.5,-0.25*pow(q*xi,2.),&pFq_1F1);

	if (status && (q*xi >= 10)) 
	{
		pFq_1F1.val = (sqrt(M_PI)*(pow(2.,D)/(pow(q,D)*pow(pow(xi,2),D/2.)*gsl_sf_gamma(1.5 - D/2.)) + 
						(pow(4,1.5 - D/2.)*pow(q,-3 + D)*pow(-pow(xi,2),-1.5 + D/2.))/
						(exp((pow(q,2)*pow(xi,2))/4.)*gsl_sf_gamma(D/2.))))/2. ;
// gsl_sf_gamma(1.5)/gsl_sf_gamma(1.5-D/2.0)*pow(0.25*pow(q*xi,2.),D/2.);
	} 
	else if (status && (q*xi < 10)) 
	{
		sasfit_param_set_err(param, DBGINFO("%s,q=%lf"), gsl_strerror(status), q);
		return SASFIT_RETURNVAL_ON_ERROR;
	} else {
		return 1.0+P16*pFq_1F1.val;
	}
	return P16;
}
