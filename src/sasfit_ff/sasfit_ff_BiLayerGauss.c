/*
 * src/sasfit_ff/sasfit_ff_BiLayerGauss.c
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
#include "include/sasfit_ff_utils.h"

/*
float BiLayerGauss(Tcl_Interp *interp,
			float q,
				float sigma_out,
			float b_out,
				float sigma_core,
			float b_core,
			float t,
			float D,
				bool  *error)
 */

scalar sasfit_ff_BiLayerGauss(scalar q, sasfit_param * param)
{
	scalar u_out, u_core, M, Pprime, R, Fout, Fcore;
	scalar sigma_out, b_out, sigma_core, b_core, t, D;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 6, &sigma_out, &b_out, &sigma_core, &b_core, &t, &D);

	u_out  = q*q*sigma_out *sigma_out ;
	u_core = q*q*sigma_core*sigma_core;

	R = 0.5*D;

	if (q == 0.0) 
	{
		Pprime=1.;
	} 
	else if (D == 0.0) 
	{
        	Pprime = 0.0;
	} 
	else 
	{
		Pprime = (M_PI*R*R)*(M_PI*R*R)*2.0/(q*q*R*R)*(1.0-gsl_sf_bessel_J1(D*q)/(q*R));
	}

	M = 2.0*sqrt(2.*M_PI)*sigma_out *b_out+sqrt(2.*M_PI)*sigma_core*b_core;


	if (M == 0.0) 
	{
		M = 1.0;
                sasfit_param_set_err(param, DBGINFO("BiLayerGauss is divergent"));
	}


	Fout  = sqrt(2.*M_PI)*sigma_out *b_out  *exp(-0.5*u_out) *cos(q*0.5*t);
	Fcore = sqrt(2.*M_PI)*sigma_core*b_core *exp(-0.5*u_core);

	return Pprime*(Fcore+2.0*Fout) * (Fcore+2.0*Fout);

}
