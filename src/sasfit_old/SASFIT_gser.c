/*
 * src/sasfit_old/SASFIT_gser.c
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

#include <math.h>
#include <tcl.h>
#include "include/SASFIT_nr.h"

#define ITMAX 100
#define EPS 3.0e-7

void gser(interp,gamser,a,x,gln,error)
Tcl_Interp *interp;
bool *error;
float a,x,*gamser,*gln;
{
	int n;
	float sum,del,ap;

	*gln=sasfit_gammaln(a);
	if (x <= 0.0) {
		if (x < 0.0) {
			sasfit_err("x less than 0 in routine GSER\n");
			*error = TRUE;
	    	*gamser=0.0;
		    return;
		}
	} else {
		ap=a;
		del=sum=1.0/a;
		for (n=1;n<=ITMAX;n++) {
			ap += 1.0;
			del *= x/ap;
			sum += del;
			if (fabs(del) < fabs(sum)*EPS) {
				*gamser=sum*exp(-x+a*log(x)-(*gln));
				return;
			}
		}
		sasfit_err("a too large, ITMAX too small in routine GSER\n");
		*error = TRUE;
		return;
	}
}

#undef ITMAX
#undef EPS
