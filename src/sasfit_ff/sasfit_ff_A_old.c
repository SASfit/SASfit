/*
 * src/sasfit_ff/sasfit_ff_A_old.c
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

#include "../../superparamagnetic_ff_psi/include/calc_sections.h"

typedef enum 
{
	B,
	KNUC_IN,
	KMAG_IN,
	M_TESLA,
	R_NM,
	T,
	T_FLUC,
	B_REMANENT,
	PSI,
	L,
	LA,
	ALPHA,
	GAMMA,
	SQ_TMP,
	POL,
	BT_SIGN,
	KNUC,
	KMAG
} variables_names;

#define V	param->p

#define CALC_PSI_VARS \
\
	SOME_NAME_FCT_VARS; \
	scalar kb, mu0, mu, vol; \
\
	SASFIT_ASSERT_PTR( param )

// the result is stored in variable res2
#define CALC_PSI(psi) \
\
	mu  = 4.0*M_PI/3.0 * pow(V[R_NM]*1.0e-9,3.)*V[M_TESLA]; \
	mu0 = 4.0*M_PI*1e-7; \
	kb  = 1.38065034E-23; \
	V[GAMMA] = 0.0; \
	V[POL] = 0.0; \
	V[BT_SIGN] = 1.0; \
	V[PSI] = psi; \
\
	vol = 4.0*M_PI/3.0 * pow(V[R_NM],3.); \
\
	V[ALPHA] = mu/mu0/(kb*(V[T]+V[T_FLUC]))*(q+V[B_REMANENT]); \
\
	SET_L_LA; \
	SET_SQ_ONE; \
\
	V[KNUC] = V[KNUC_IN] * vol; \
	V[KMAG] = V[KMAG_IN] * vol; \
\
	SOME_NAME_FCT



scalar sasfit_ff_A(scalar q, sasfit_param * param)
{
	CALC_PSI_VARS;

	CALC_PSI(0.0);

	return res2;
}

scalar sasfit_ff_B(scalar q, sasfit_param * param)
{
	scalar res;
	CALC_PSI_VARS;

	CALC_PSI(0.0);
	res = res2;
	CALC_PSI(90.0);

	return res2 - res;
}

