/*
 * src/sasfit_ff/cyl_shell/include/private.h
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

#ifndef CYL_SHELL_PRIVATE_H
#define CYL_SHELL_PRIVATE_H

#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>
#include "../../include/sasfit_ff_utils.h"

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
	EXP,
	Q
} variable_names;

#define CYL_SHELL_INT_BODY(core_fct, exp) \
\
	scalar res; \
\
	SASFIT_ASSERT_PTR(param); \
	V[Q] = q; \
	V[EXP] = (exp); \
\
	SASFIT_CHECK_COND1((V[Q] < 0.0), param, "q(%lg) < 0",V[Q]); \
	SASFIT_CHECK_COND1((V[L] < 0.0), param, "L(%lg) < 0",V[L]); \
	SASFIT_CHECK_COND1((V[R] < 0.0), param, "R(%lg) < 0",V[R]); \
	SASFIT_CHECK_COND1((V[DR] < 0.0), param, "DR(%lg) < 0",V[DR]); \
\
	res = sasfit_integrate(0.0, 1.0, (core_fct), param); \
\
	return res

#define CYL_SHELL_HEADER \
\
	scalar base = 0.0; \
	SASFIT_ASSERT_PTR(param); \
\
        if (SASFIT_EQUAL(V[Q], 0.0)) \
		base = CO_SH * V[R]*V[R] * V[L]*M_PI +  \
			SH_SO * RDR*RDR * V[L]*M_PI; \
\
	else if (SASFIT_EQUAL(RDR, 0.0)) return 0.0

// local shortcuts
#define RDR	(V[R] + V[DR])
#define SH_SO	(V[ETA_SHELL]-V[ETA_SOLV])
#define CO_SH	(V[ETA_CORE]-V[ETA_SHELL])

#define V	param->p

scalar sasfit_ff_cyl_shell_1_core(scalar x, sasfit_param * param);

scalar sasfit_ff_cyl_shell_2_core(scalar x, sasfit_param * param);

#endif

