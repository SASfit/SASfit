/*
 * src/sasfit_ff/superparamagnetic_ff_psi/include/private.h
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

#ifndef SUPERPARAMAGNETIC_FF_PSI_PRIVATE_H
#define SUPERPARAMAGNETIC_FF_PSI_PRIVATE_H

#include <gsl/gsl_math.h>
#include <sasfit_sd.h>
#include "../../include/sasfit_ff_utils.h"

#include "calc_sections.h"

/*
 * An enumeration of all variable names which will be stored in a
 * sasfit_param structure. This includes all variables which are
 * required to let several functions know about. For communication
 * of variable values across function boundaries. They can be regarded
 * as global variables in the scope of the 'toplevel' form factor
 * function which gets called from the outside.
 */
typedef enum // maximum number == MAXPAR !!! 
// see sasfit_common/include/sasfit_constants.h
{
	T,
	NDL,
	LAMBDA,
	D_LAMBDA,
	R,
	R_AV,
	FREQ,
	MU_KT,
	PSI,
	SD,
	B0,
	B1,
	PHI0DEG,
	PHI0,
	POL,
	KNUC,
	KMAG,
	KNUC_KMAG,
	SQ,
	SQ_TMP,
	LAMB,
	BT,
	L,
	LA,
	GAMMA,
	PHI0SQDEG,
	PHI0SQ,
	B_SQ,
	BS,
	D_PHI,
	BT_SIGN,
	ALPHA,
	BEFF,
	D_T,
	NUC_SH,
	NUC_MAT,
	D_R
} variable_names;

/*
 * Shortcut for accessing the parameter array.
 * e.g. V[ALPHA] -> accessing parameter variable alpha
 */
#define V	subParam.p

/*
 * defines and initalizes a secondary parameter structure where all parameters
 * are copied to
 * -> it is required to have a clear defined (and fixed) association of a variable name
 *    to its array position (in FF order and meaning of parameters change)
 * -> also, we want to be allowed to overwrite them with temporary values
 */
#define INIT_SUBPARAM \
\
	sasfit_param subParam; \
	sasfit_init_param( &subParam ); \
	V[T]		= t

/*
 * some common initialization at the beginning of each form factor. All of them
 * have this part in common. Avoids a couple of lines duplicated over and over.
 */
#define SET_COMMON_VARS \
\
	V[NDL]		= (scalar)55; \
	V[B1]		= 0.0; \
	V[SQ]		= 1.0; \
	V[SD]		= 4.0; \
	V[POL]		= 0.0; \
	V[KNUC]		= 1.0; \
	V[R_AV]		= 1.0; \
	V[R]		= 1.0; \
	V[LAMBDA]	= 6.0; \
	V[D_LAMBDA]	= 0.1; \
	V[BT_SIGN]	= 1.0; \
	V[GAMMA]	= 0.0;

/*
 * Default parameters for evaluation of the size distribution during 
 * calculation of the form factor (inner loop).
 * see CALC_MAIN_BODY
 */
#define TRIANGULAR_PARAM_DEFAULT \
                subParam2.p[0] = 1.0; \
                subParam2.p[1] = V[LAMBDA] * (1.0 - V[D_LAMBDA]); \
                subParam2.p[2] = V[LAMBDA]; \
                subParam2.p[3] = V[LAMBDA] * (1.0 + V[D_LAMBDA]); \
                subParam2.p[4] = 1.0

/*
 * Parameters for evaluation of the size distribution during 
 * calculation of the superparamagnetic_ff_psi_tisane form factor 
 * (inner loop).
 * see CALC_MAIN_BODY
 */
#define TRIANGULAR_PARAM_TISANE \
                subParam2.p[0] = 1.0; \
                subParam2.p[1] = V[T]-V[D_T]; \
                subParam2.p[2] = V[T]; \
                subParam2.p[3] = V[T]+V[D_T]; \
                subParam2.p[4] = 1.0

#endif

