/*
 * src/sasfit_ff/superparamagnetic_ff_psi/sasfit_ff_superparamagnetic_ff_psi_cross.c
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

scalar sasfit_ff_superparamagnetic_ff_psi_cross(scalar t, sasfit_param * param)
{
	scalar result1, result2;
	HEADER_FF_PSI;

	// get supplied parameters (probably from GUI), KNUC==NUC_C, KMAG==MAG_C
	sasfit_get_param(param, 10, &V[R], &V[D_R], &V[KNUC], &V[NUC_SH], &V[NUC_MAT], &V[KMAG], &V[R_AV], &V[MU_KT], &V[PSI], &V[POL]);
	
	SUPER_PARAMAGNETIC_FF_PSI_BODY;
	result1 = res2;

	V[POL] = - V[POL];
	SUPER_PARAMAGNETIC_FF_PSI_BODY;
	result2 = res2;

	return result1 - result2;
}

