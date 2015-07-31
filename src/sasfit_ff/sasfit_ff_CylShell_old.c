/*
 * src/sasfit_ff/sasfit_ff_CylShell_old.c
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
float I_CylShell1(Tcl_Interp *interp,
				 float Q,
					  float R,
				 float DR,
				 float L,
				 float eta_core,
				 float eta_shell,
				 float eta_solv,
				 bool*error)
*/

typedef enum
{
	R,
	DR,
	L,
	ETA_CORE,
	ETA_SHELL,
	ETA_SOLV,
	POW,
	Q
} variable_names;

#define CYL_SHELL_BODY(core_fct) \
\
	scalar res; \
\
	SASFIT_ASSERT_PTR(param); \
	V[Q] = q; \
	V[POW] = 2.0; \
\
	SASFIT_CHECK_COND1((V[Q] < 0.0), param, "q(%lg) < 0",V[Q]); \
	SASFIT_CHECK_COND1((V[L] < 0.0), param, "L(%lg) < 0",V[L]); \
	SASFIT_CHECK_COND1((V[R] < 0.0), param, "R(%lg) < 0",V[R]); \
	SASFIT_CHECK_COND1((V[DR] < 0.0), param, "DR(%lg) < 0",V[DR]); \
\
	res = sasfit_integrate(0.0, 1.0, core_fct, param); \
\
	return res

#define V	param->p

// local shortcuts
#define RDR	(V[R] + V[DR])
#define SH_SO	(V[ETA_SHELL]-V[ETA_SOLV])
#define CO_SH	(V[ETA_CORE]-V[ETA_SHELL])

scalar sasfit_ff_cyl_shell_1_core(scalar x, sasfit_param * param)
{
	scalar base = 0.0;
	SASFIT_ASSERT_PTR(param);

        if (SASFIT_EQUAL(V[Q], 0.0))
		base = CO_SH * V[R]*V[R] * V[L]*M_PI + 
			SH_SO * RDR*RDR * L*M_PI;

	else if (SASFIT_EQUAL(RDR, 0.0)) return 0.0;

	else if (SASFIT_EQUAL(V[L], 0.0)) return 0.0;

	else if (SASFIT_EQUAL(x, 0.0))
	{
		base = 2.0*gsl_sf_bessel_J1(V[Q]*RDR)/V[Q]*L*RDR*M_PI*SH_SO;

		if (! SASFIT_EQUAL(V[R], 0.0))
			base = base + 2.0*gsl_sf_bessel_J1(V[Q]*V[R])/V[Q]*L*V[R]*M_PI*CO_SH;
	} 
	else if (SASFIT_EQUAL(x, 1.0))
	{
		base = 2.0/V[Q]*RDR*RDR*sin(V[Q]*V[L]*0.5)*SH_SO*M_PI;

		if (! SASFIT_EQUAL(V[R], 0.0))
			base = base + 2.0/V[Q]*R*R    *sin(V[Q]*V[L]*0.5)*CO_SH*M_PI;
	} else 
	{
		base = 4.0*SH_SO*M_PI*RDR*gsl_sf_bessel_J1(V[Q]*RDR*sqrt(1.0-x*x))*sin(V[Q]*V[L]*x/2.0)*pow(V[Q],-2.0) 
			/ (sqrt(1.0-x*x)*x);

		if (! SASFIT_EQUAL(V[R], 0.0))
			base = base + 4.0*CO_SH*M_PI*V[R]*gsl_sf_bessel_J1(V[Q]*V[R]*sqrt(1.0-x*x))*sin(V[Q]*V[L]*x/2.0)*pow(V[Q],-2.0) 
				/ (sqrt(1.0-x*x)*x);
	}
	return pow(base, V[POW]);
}

scalar sasfit_ff_cyl_shell_2_core(scalar x, sasfit_param * param)
{
	scalar base = 0.0;
	SASFIT_ASSERT_PTR(param);

        if (SASFIT_EQUAL(V[Q], 0.0))
		base = CO_SH * V[R]*V[R] * V[L]*M_PI + 
			SH_SO * RDR*RDR * L*M_PI;

	else if (SASFIT_EQUAL(RDR, 0.0)) return 0.0;

	else if (SASFIT_EQUAL(V[L]+V[DR], 0.0)) return 0.0;

	else if (SASFIT_EQUAL(x, 0.0))
	{
		base = 2.0*gsl_sf_bessel_J1(V[Q]*RDR)/V[Q]* (V[L]+2.0*V[DR]) *RDR*M_PI*SH_SO; //

		if (! SASFIT_EQUAL(V[R], 0.0))
			base = base + 2.0*gsl_sf_bessel_J1(V[Q]*V[R])/V[Q]*L*V[R]*M_PI*CO_SH;

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
			base = base + 4.0*CO_SH*M_PI*V[R]*gsl_sf_bessel_J1(V[Q]*V[R]*sqrt(1.0-x*x))*sin(V[Q]*V[L]*x/2.0)*pow(V[Q],-2.0) 
				/ (sqrt(1.0-x*x)*x);
	}
	return pow(base, V[POW]);
}

scalar sasfit_ff_cyl_shell_1(scalar q, sasfit_param * param)
{
	CYL_SHELL_BODY(sasfit_ff_cyl_shell_1_core);
}

scalar sasfit_ff_cyl_shell_2(scalar q, sasfit_param * param)
{
	CYL_SHELL_BODY(sasfit_ff_cyl_shell_2_core);
}

scalar sasfit_ff_cyl_shell_v(scalar q, sasfit_param * param, int distr)
{	
	scalar v;
	SASFIT_ASSERT_PTR(param);

	     if ( distr == 0 ) v = q    + V[DR];
	else if ( distr == 1 ) v = V[R] + q;
	else                   v = V[R] + V[DR];
	
	if ( distr != 2 ) q = V[L];

	return M_PI * q * pow(v, 2.0);
}

