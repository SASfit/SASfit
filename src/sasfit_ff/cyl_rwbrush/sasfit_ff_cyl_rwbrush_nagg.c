/*
 * src/sasfit_ff/cyl_rwbrush/sasfit_ff_cyl_rwbrush_nagg.c
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

scalar sasfit_ff_cyl_rwbrush_nagg(scalar q, sasfit_param * param)
{
	CYL_RWBRUSH_HEADER;

	V[NAGG] = V[PARAM0];
	V[V_CORE] = V[PARAM1];

	CYL_RWBRUSH_TESTS;
	SASFIT_CHECK_COND1((V[V_CORE] < 0.0), param, "V_core(%lg) < 0",V[V_CORE]);
	SASFIT_CHECK_COND((V[NAGG] == 0.0), param, "Nagg == 0");

// Solve[{Vc/(1 - xsolv)*Nagg == Pi*Rc^2*H}, {Rc}]
// {{Rc -> (
//	Sqrt[Nagg] Sqrt[Vc])/(Sqrt[H] Sqrt[\[Pi]] Sqrt[1 - xsolv])}}

	V[R] = sqrt( fabs(V[NAGG]*(V[V_CORE]/(1.0-V[XSOLV_CORE]))/M_PI/V[H]));

	CYL_RWBRUSH_BODY;
	return pcyl;
}


