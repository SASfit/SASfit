/*
 * src/sasfit_ff/magnetic_shell/include/private.h
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

#ifndef MAGNETIC_SHELL_PRIVATE_H
#define MAGNETIC_SHELL_PRIVATE_H

#include <gsl/gsl_math.h>
#include "../../include/sasfit_ff_utils.h"

/*
float InucmagShellAnisotropic(Tcl_Interp *interp,
		float q, 
		  float R,
		float dR,
		  float nuc_c,
		  float nuc_sh,
		  float nuc_matrix,
		  float mag_c,
		  float mag_sh,
		  float mag_matrix,
		float pol,
		  bool  *error)
*/

typedef enum
{
	R,
	D_R,
	NUC_C,
	NUC_SH,
	NUC_MATRIX,
	MAG_C,
	MAG_SH,
	MAG_MATRIX,
	PSI,
	POL,
	Q,
	PSI2D
} variable_names;

#define V       param->p

#define MAGNETIC_SHELL_HEADER \
\
	scalar knuc, kmag, res, sin2 = 1.0; \
	sasfit_param subParam; \
\
	SASFIT_ASSERT_PTR(param); \
\
	V[Q] = q; \
\
	SASFIT_CHECK_COND1((V[Q] < 0.0), param, "q(%lg) < 0",V[Q]); \
	SASFIT_CHECK_COND1((V[R] < 0.0), param, "R(%lg) < 0",V[R]); \
	SASFIT_CHECK_COND1((fabs(V[POL]) > 1.0), param, "fabs(pol)(%lg) > 1",fabs(V[POL])); \
\
	if (SASFIT_EQUAL(V[R], 0.0)) return 0.0; \
\
	V[PSI2D] = sasfit_param_override_get_psi ( V[PSI]/180.0*M_PI ); \
\
	sasfit_init_param( &subParam )


#define MAGNETIC_SHELL_KNUC_KMAG \
\
			sin2 = pow(sin(V[PSI2D]),2.0); \
\
			subParam.p[0] = V[R] + V[D_R]; \
			subParam.p[3] = V[NUC_SH] - V[NUC_MATRIX]; \
			knuc = sasfit_ff_sphere_f(V[Q], &subParam ); \
			SASFIT_CHECK_SUB_ERR(param, subParam); \
\
			subParam.p[0] = V[R]; \
			subParam.p[3] = V[NUC_C] - V[NUC_SH]; \
			knuc = knuc + sasfit_ff_sphere_f(V[Q], &subParam ); \
			SASFIT_CHECK_SUB_ERR(param, subParam); \
\
			subParam.p[0] = V[R] + V[D_R]; \
			subParam.p[3] = V[MAG_SH] - V[MAG_MATRIX]; \
			kmag = sasfit_ff_sphere_f(V[Q], &subParam ); \
			SASFIT_CHECK_SUB_ERR(param, subParam); \
\
			subParam.p[0] = V[R]; \
			subParam.p[3] = V[MAG_C] - V[MAG_SH]; \
			kmag = kmag + sasfit_ff_sphere_f(V[Q], &subParam ); \
			SASFIT_CHECK_SUB_ERR(param, subParam)

#endif
