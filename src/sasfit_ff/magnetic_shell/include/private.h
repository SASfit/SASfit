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
*/

#define	R       param->p[0]
#define	D_R     param->p[1]
#define	NUC_C   param->p[2]
#define	NUC_SH  param->p[3]
#define	NUC_MATRIX  param->p[4]
#define	MAG_C   param->p[5]
#define	MAG_SH  param->p[6]
#define	MAG_MATRIX  param->p[7]
#define	PSI     param->p[8]
#define	POL     param->p[9]

#define	Q       param->p[MAXPAR-1]
#define	PSI2D   param->p[MAXPAR-2]
#define V       param->p

#define MAGNETIC_SHELL_HEADER \
\
	scalar knuc, kmag, res, sin2 = 1.0; \
	sasfit_param subParam; \
\
	SASFIT_ASSERT_PTR(param); \
\
	Q = q; \
\
	SASFIT_CHECK_COND1((Q < 0.0), param, "q(%lg) < 0",Q); \
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); \
	SASFIT_CHECK_COND1((fabs(POL) > 1.0), param, "fabs(pol)(%lg) > 1",fabs(POL)); \
\
	if (R == 0.0) return 0.0; \
\
	PSI2D = sasfit_param_override_get_psi ( PSI/180.0*M_PI ); \
\
	sasfit_init_param( &subParam )


#define MAGNETIC_SHELL_KNUC_KMAG \
\
			sin2 = pow(sin(PSI2D),2.0); \
\
			subParam.p[0] = R + D_R; \
			subParam.p[3] = NUC_SH - NUC_MATRIX; \
			knuc = sasfit_ff_sphere_f(Q, &subParam ); \
			SASFIT_CHECK_SUB_ERR(param, subParam); \
\
			subParam.p[0] = R; \
			subParam.p[3] = NUC_C - NUC_SH; \
			knuc = knuc + sasfit_ff_sphere_f(Q, &subParam ); \
			SASFIT_CHECK_SUB_ERR(param, subParam); \
\
			subParam.p[0] = R + D_R; \
			subParam.p[3] = MAG_SH - MAG_MATRIX; \
			kmag = sasfit_ff_sphere_f(Q, &subParam ); \
			SASFIT_CHECK_SUB_ERR(param, subParam); \
\
			subParam.p[0] = R; \
			subParam.p[3] = MAG_C - MAG_SH; \
			kmag = kmag + sasfit_ff_sphere_f(Q, &subParam ); \
			SASFIT_CHECK_SUB_ERR(param, subParam)

#endif
