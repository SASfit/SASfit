/*
 * src/sasfit_ff/sasfit_ff_ExpShell.c
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


/*
float ExpShell(Tcl_Interp *interp,
			 float Q, 
			 float R1, 
			 float DR, 
			 float eta_core, 
			 float eta_shell,
		  float x_in_solvent,
		  float x_out_solvent,
		  float alpha,
		  float eta_solvent,
			 bool  *error)
 */
scalar sasfit_ff_ExpShell(scalar q, sasfit_param * param)
{
	scalar res = sasfit_ff_KshExp(q, param);
	return res*res;
}
