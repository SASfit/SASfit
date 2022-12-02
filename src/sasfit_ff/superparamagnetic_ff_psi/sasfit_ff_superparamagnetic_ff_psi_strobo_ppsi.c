/*
 * src/sasfit_ff/superparamagnetic_ff_psi/sasfit_ff_superparamagnetic_ff_psi_strobo_ppsi.c
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
 *   Ingo Bressler (ingo.bressler@bam.de)
 */

#include "include/private.h"

#define SET_BT			SET_BT_DEFAULT
#define SET_ALPHA 		SET_ALPHA_DEFAULT
#define SET_SQ			SET_SQ_PPSI
#define TRIANGULAR_PARAM        TRIANGULAR_PARAM_DEFAULT
scalar sasfit_ff_superparamagnetic_ff_psi_strobo_ppsi(scalar t, sasfit_param * param)
{
	HEADER_FF_PSI_STROBO;

	// get supplied parameters (probably from GUI)
	sasfit_get_param(param, 11, EMPTY, &V[PHI0SQDEG], &V[FREQ], &V[B0], &V[B_SQ], &V[SQ], &V[BS], &V[KNUC_KMAG], &V[MU_KT], &V[SD], &V[PSI]);

	// sets kmag, phi0 which require knowledge of supplied parameters (above)
	SET_VARS_PRIOR;

	// customization
	V[PHI0SQ] = M_PI*V[PHI0SQDEG]/180.0;

	CALC_LOOP;
	
	// let the toplevel parameter structure know about possible errors
	SASFIT_CHECK_SUB_ERR(param, subParam);
	
	return res;
}

