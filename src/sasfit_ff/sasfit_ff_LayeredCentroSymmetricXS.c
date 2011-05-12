/*
 * src/sasfit_ff/sasfit_ff_LayeredCentroSymmetricXS.c
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
float LayeredCentroSymmetricXS(Tcl_Interp *interp,
			float Q,
				float t_out,
			float t_core,
				float b_out,
			float b_core,
			float D,
				bool  *error)
 */
scalar sasfit_ff_LayeredCentroSymmetricXS(scalar q, sasfit_param * param)
{
	scalar u_out, u_core, M, sinc_core, sinc_out, Pprime, R, L_out, L_core;
	scalar t_out, t_core, b_out, b_core, D;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 5, &t_out, &t_core, &b_out, &b_core, &D);

	L_core = t_core;
	L_out = t_core+t_out;
	u_out = q*L_out*0.5;
	u_core = q*L_core*0.5;
	R = 0.5*D;

	if (q == 0.0) 
	{
		Pprime=1.0;
	} 
	else if (D == 0.0) 
	{
		Pprime = 0.0;
	} else 
	{
		Pprime = (M_PI*R*R)*(M_PI*R*R)*2.0/(q*q*R*R)*(1.0-gsl_sf_bessel_J1(D*q)/(q*R));
	}

	M = b_out*L_out-L_core*(b_out-b_core);

	if (u_out == 0) 
	{
		sinc_out = b_out*L_out;
	} else 
	{
		sinc_out = b_out*L_out*sin(u_out)/u_out;
	}

	if (u_core == 0) 
	{
		sinc_core = (b_out-b_core)*L_core;
	} 
	else {
		sinc_core = (b_out-b_core)*L_core*sin(u_core)/u_core;
	}

	return Pprime*pow(sinc_out-sinc_core, 2.0);
}

