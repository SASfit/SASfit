/*
 * src/sasfit_ff/superparamagnetic_ff_psi/sasfit_ff_superparamagnetic_ff_psi_tisane.c
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

// shortcut for accessing the parameter structure sasfit_param
#undef V
#define V param->p

#define SET_BT			SET_BT_TISANE
#define SET_ALPHA 		SET_ALPHA_DEFAULT
#define SET_SQ			SET_SQ_ONE
#define TRIANGULAR_PARAM	TRIANGULAR_PARAM_TISANE
scalar superparamagnetic_ff_psi_core(scalar x, sasfit_param * param)
{
	MAIN_BODY_VARS;
	SET_COMMON_VARS;
	SASFIT_ASSERT_PTR( param ); // beware of erroneous pointer
	// sets kmag, phi0 which require knowledge of supplied parameters (above)
	SET_VARS_PRIOR;

	V[LAMB] = x;
	CALC_MAIN_BODY;

	return res2 * pow(V[R]/V[R_AV],6.0);
}

// redefinition of parameter access shortcut for writing to
// another (but locally) defined/created parameter structure
#undef V
#define V subParam.p

scalar sasfit_ff_superparamagnetic_ff_psi_tisane(scalar t, sasfit_param * param)
{
	scalar res;
	INIT_SUBPARAM;
	SASFIT_ASSERT_PTR( param ); // beware of erroneous pointer

	// get supplied parameters (probably from GUI)
	sasfit_get_param(param, 10, &V[PHI0DEG], &V[KNUC_KMAG], &V[D_T], &V[B1], &V[MU_KT], &V[FREQ], &V[B0], EMPTY, EMPTY, &V[PSI]);

	res = sasfit_integrate(t-V[D_T], t+V[D_T], superparamagnetic_ff_psi_core, &subParam);
	
	// let the toplevel parameter structure know about possible errors
	SASFIT_CHECK_SUB_ERR(param, subParam);
	
	return res;
}

