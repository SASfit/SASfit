/*
 * src/sasfit_ff/sasfit_ff_TripleLayeredHomogeneousCentroSymmetricXS.c
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
#include "include/sasfit_ff_utils.h"

/*
float TripleLayeredHomogeneousCentroSymmetricXS(Tcl_Interp *interp,
			float q,
				float nagg,
			float V_core,
				float V_middle,
			float V_out,
			float eta_core,
			float eta_middle,
			float eta_out,
			float eta_solv,
			float xsolv_middle,
			float xsolv_out,
				bool  *error)
*/
scalar sasfit_ff_TripleLayeredHomogeneousCentroSymmetricXS(scalar q, sasfit_param * param)
{
	scalar u_out, u_core, u_middle, sinc_core, sinc_middle, sinc_out, Pprime, L_out, L_core, L_middle;
	scalar nagg, V_core, V_middle, V_out, eta_core, eta_middle, eta_out, eta_solv, xsolv_middle, xsolv_out;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 10, &nagg, &V_core, &V_middle, &V_out, &eta_core, &eta_middle, &eta_out, &eta_solv, &xsolv_middle, &xsolv_out);

	SASFIT_CHECK_COND((q == 0.0), param, "q == 0");
	SASFIT_CHECK_COND((xsolv_middle == 1.0), param, "xsolv_middle == 1");
	SASFIT_CHECK_COND((xsolv_out == 1.0), param, "xsolv_out == 1");

	L_core	 = nagg * V_core;
	L_out	 = nagg * V_out	 / (1.-xsolv_out);
	L_middle = nagg * V_middle / (1.-xsolv_middle);

	u_core	 = q* L_core*0.5;
	u_middle = q*(L_core+L_middle)*0.5;
	u_out	 = q*(L_core+L_middle+L_out)*0.5;

	Pprime = 2.*M_PI/(q*q);

	if (u_out == 0) 
	{
		sinc_out = (eta_out-eta_solv)*(L_core+L_middle+L_out);
	} else 
	{
		sinc_out = (eta_out-eta_solv)*(L_core+L_middle+L_out)*sin(u_out)/u_out;
	}
	if (u_middle == 0) 
	{
		sinc_middle = (eta_middle-eta_out)*(L_core+L_middle);
	} else 
	{
		sinc_middle = (eta_middle-eta_out)*(L_core+L_middle)*sin(u_middle)/u_middle;
	}
	if (u_core == 0) 
	{
		sinc_core = (eta_core-eta_middle)*L_core;
	} else 
	{
		sinc_core = (eta_core-eta_middle)*L_core*sin(u_core)/u_core;
	}

	return Pprime*pow(sinc_out+sinc_middle+sinc_core,2.);
}

