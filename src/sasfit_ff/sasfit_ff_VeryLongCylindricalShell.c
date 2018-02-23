/*
 * src/sasfit_ff/sasfit_ff_VeryLongCylindricalShell.c
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
float VeryLongCylindricalShell(Tcl_Interp *interp,
			float Q,
			float R,
			float DR,
			  float L,
			  float eta_core,
			float eta_shell,
			float eta_solv,
			  bool  *error)
*/
scalar sasfit_ff_VeryLongCylindricalShell(scalar q, sasfit_param * param)
{
	scalar Pp, Pcs, R, DR, L, eta_core, eta_shell, eta_solv;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 6, &R, &DR, &L, &eta_core, &eta_shell, &eta_solv);

	SASFIT_CHECK_COND1((q < 0), param, "q(%lg) < 0", q);
	SASFIT_CHECK_COND1((L < 0), param, "L(%lg) < 0", L);
	SASFIT_CHECK_COND1((R < 0), param, "R(%lg) < 0", R);
	SASFIT_CHECK_COND1((DR < 0), param, "DR(%lg) < 0", DR);

	if (q == 0.0) return gsl_pow_2((eta_core-eta_shell)*R*R*L*M_PI+(eta_shell-eta_solv)*(R+DR)*(R+DR)*L*M_PI);
	if (L == 0.0) return 0.0;
	if (R+DR == 0.0) return 0.0;

	Pp =(2.0*gsl_sf_Si(q*L)/(q*L)-gsl_pow_2(sin(q*L*0.5)/(q*L*0.5)));

	if (R==0) 
	{
		Pcs = gsl_pow_2(2*gsl_sf_bessel_J1(q*(R+DR))/(q*(R+DR))*(eta_shell-eta_solv)*(R+DR)*(R+DR)*L*M_PI);
	} else 
	{
		Pcs = gsl_pow_2(2*gsl_sf_bessel_J1(q*R)     /(q*R)     *(eta_core-eta_shell)*R*R*L*M_PI + 
			2*gsl_sf_bessel_J1(q*(R+DR))/(q*(R+DR))*(eta_shell-eta_solv)*(R+DR)*(R+DR)*L*M_PI);
	}

	return Pp*Pcs;
}

