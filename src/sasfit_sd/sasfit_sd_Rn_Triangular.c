/*
 * src/sasfit_sd/sasfit_sd_Rn_Triangular.c
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
#include "include/sasfit_sd_utils.h"

/*
float Rn_Triangular(Tcl_Interp *interp,
                   float N,
                   float Rmin,
				   float R0,
                   float Rmax,
				   float n,
				   float q,
                   bool  *error)
		   */
/**
 * calculates a Triangular distribution function
 */
scalar sasfit_sd_Rn_Triangular(scalar x, sasfit_param * param)
{
	scalar N, Rmin, R0, Rmax, n, y0;
	scalar t1,t2,t4,t5,t6,t9,t14,R1, cg;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 5, &N, &Rmin, &R0, &Rmax, &n);

	SASFIT_CHECK_COND2((Rmin > Rmax), param, "Rmin(%lg) > Rmax(%lg)",Rmin,Rmax);
	SASFIT_CHECK_COND1((Rmin < 0.0), param, "Rmin(%lg) < 0",Rmin);
	SASFIT_CHECK_COND2((Rmin > R0), param, "Rmin(%lg) > R0(%lg)",Rmin,R0);
	SASFIT_CHECK_COND2((Rmax < R0), param, "Rmax(%lg) < R0(%lg)",Rmax,R0);
	if ((x<Rmin) || (x>Rmax)) return 0;

//	SASFIT_CHECK_COND2((q < Rmin), param, "x(%lg) < Rmin(%lg)",x,Rmin);
//	SASFIT_CHECK_COND2((q > Rmax), param, "x(%lg) > Rmax(%lg)",x,Rmax);

	cg = n;

	if (cg == 1.) 
	{
		y0 = N*2./(Rmax-Rmin);
	} 
	else 
	{
		R1 = Rmax;
	   
		t1 = cg + 1;
		t2 = pow(R1,t1);
		t4 = pow(R0,t1);
		t5 = pow(R1,cg);
		t6 = R0 * t5;
		t9 = pow(R0,cg);
		t14 = 0.1e1 / (-t9 * cg + t5 * cg - t9 + t5) * (-cg * t2 - t4 + t6 + cg * t6);

		y0 = t14;

		R1 = Rmin;

		t1 = cg + 1;
		t2 = pow(R1,t1);
		t4 = pow(R0,t1);
		t5 = pow(R1,cg);
		t6 = R0 * t5;
		t9 = pow(R0,cg);
		t14 = 0.1e1 / (-t9 * cg + t5 * cg - t9 + t5) * (-cg * t2 - t4 + t6 + cg * t6);

		y0 = N/(fabs(y0)+fabs(t14));
	}
	if (x==R0) return y0;
	if (x==Rmin) return 0.;
	if (x==Rmax) return 0.;
	if (x<R0) return (pow(x,cg)-pow(Rmin,cg))*y0/(pow(R0,cg)-pow(Rmin,cg));
	if (x>R0) return (pow(x,cg)-pow(Rmax,cg))*y0/(pow(R0,cg)-pow(Rmax,cg));

	return 0.0;
}

