/*
 * src/sasfit_old/SASFIT_gammaq.c
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
 */

#include <tcl.h>
#include "include/SASFIT_nr.h"

float gammq(interp,a,x,error)
Tcl_Interp *interp;
bool *error;
float a,x;
{

	float gamser,gammcf,gln;
	void gcf(),gser();
	*error = FALSE;
	if (x < 0.0 || a <= 0.0) {
		sasfit_err("Invalid arguments in routine GAMMQ\n");
		*error = TRUE;
    	return 1.0;
	}
	if (x < (a+1.0)) {
		gser(interp,&gamser,a,x,&gln,error);
		return 1.0-gamser;
	} else {
		gcf(interp,&gammcf,a,x,&gln,error);
		return gammcf;
	}
}
