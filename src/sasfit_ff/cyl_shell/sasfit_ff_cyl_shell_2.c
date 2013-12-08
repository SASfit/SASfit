/*
 * src/sasfit_ff/cyl_shell/sasfit_ff_cyl_shell_2.c
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

#include "include/private.h"

scalar sasfit_ff_cyl_shell_2(scalar q, sasfit_param * param)
{
	CYL_SHELL_INT_BODY(sasfit_ff_cyl_shell_2_core, 2.0);
}

scalar sasfit_ff_cyl_shell_2_f(scalar q, sasfit_param * param)
{
	CYL_SHELL_INT_BODY(sasfit_ff_cyl_shell_2_core, 1.0);
}

scalar sasfit_ff_cyl_shell_2_core(scalar x, sasfit_param * param)
{
	CYL_SHELL_HEADER;

	else if (SASFIT_EQUAL(V[L]+V[DR], 0.0)) return 0.0;

	else if (SASFIT_EQUAL(x, 0.0))
	{
		base = 2.0*gsl_sf_bessel_J1(V[Q]*RDR)/V[Q]* (V[L]+2.0*V[DR]) *RDR*M_PI*SH_SO; //

		if (! SASFIT_EQUAL(V[R], 0.0))
			base = base + 2.0*gsl_sf_bessel_J1(V[Q]*V[R])/V[Q]*V[L]*V[R]*M_PI*CO_SH;
	} 
	else if (SASFIT_EQUAL(x, 1.0))
	{
		base = 2.0/V[Q]*RDR*RDR*sin(V[Q]* (V[L]+2.0*V[DR]) *0.5)*SH_SO*M_PI;	//

		if (! SASFIT_EQUAL(V[R], 0.0))
			base = base + 2.0/V[Q]*R*R    *sin(V[Q]*V[L]*0.5)*CO_SH*M_PI;
	} else 
	{
		base = 4.0*SH_SO*M_PI*RDR*gsl_sf_bessel_J1(V[Q]*RDR*sqrt(1.0-x*x))*sin(V[Q]* (V[L]+V[DR]) *x/2.0)*pow(V[Q],-2.0) 
			/ (sqrt(1.0-x*x)*x); //

		if (! SASFIT_EQUAL(V[R], 0.0))
		{
			// "(V[L]+2.0*V[DR])" correct here ?? (as in original, but inconsistent)
			// supposed to be (V[L]+V[DR]), see above
			base = 4.0*SH_SO*M_PI*RDR*gsl_sf_bessel_J1(V[Q]*RDR*sqrt(1.0-x*x))*sin(V[Q]* (V[L]+2.0*V[DR]) *x/2.0)*pow(V[Q],-2.0) 
				/ (sqrt(1.0-x*x)*x); //
			base = base + 4.0*CO_SH*M_PI*V[R]*gsl_sf_bessel_J1(V[Q]*V[R]*sqrt(1.0-x*x))*sin(V[Q]*V[L]*x/2.0)*pow(V[Q],-2.0) 
				/ (sqrt(1.0-x*x)*x);
		}
	}
	return pow(base, V[EXP]);
}

