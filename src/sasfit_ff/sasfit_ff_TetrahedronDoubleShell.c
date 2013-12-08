/*
 * src/sasfit_ff/sasfit_ff_TetrahedronDoubleShell.c
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
float TetrahedronDoubleShell(Tcl_Interp *interp,
			 float Q, 
			 float Rc, 
			 float DR1,
		  float DR2,
		  float L,
		  float eta_c,
			 float eta_1, 
			 float eta_2,
		  float eta_sol,
			 float x_sol_sh2,
		  float n,
			 bool  *error)
 */
scalar sasfit_ff_TetrahedronDoubleShell(scalar q, sasfit_param * param)
{
	scalar A, R[5][5], R12, ftmp, Detac, Deta1, Deta2;
	scalar Rc, DR1, DR2, L, eta_c, eta_1, eta_2, eta_sol, x_sol_sh2, n;
	int i,j;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 10, &Rc, &DR1, &DR2, &L, &eta_c, &eta_1, &eta_2, &eta_sol, &x_sol_sh2, &n);

	R12=2.0*(Rc+DR1+DR2)+L;
	for (i=0; i < 5 ;i++) 
	{
		for (j=0; j < 5 ;j++) 
		{
			R[i][j] = R12;
		}
	}
	for (i=0; i < 5 ;i++) 
	{
		R[i][i] = 0.0;
	}
	R[3][4] = sqrt(6.0/3.0)*R12;
	R[4][3] = R[3][4];

	Deta2 = (1.0-x_sol_sh2)*(eta_2-eta_sol);
	Deta1 = (eta_1-eta_sol);
	Detac = (eta_c-eta_sol);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((fabs(x_sol_sh2-0.5) > 0.5), param, "fabs(x_sol_sh2-0.5)(%lg) > 0.5",fabs(x_sol_sh2-0.5));
	SASFIT_CHECK_COND1((n > 5.0), param, "n(%lg) > 5",n);
	SASFIT_CHECK_COND1((n <= 0.0), param, "n(%lg) <= 0",n);
	SASFIT_CHECK_COND1((R12 <= 0.0), param, "R12(%lg) <= 0",R12);
	SASFIT_CHECK_COND1((fabs(Detac)+fabs(Deta1)+fabs(Deta2) <= 0.0), param, "|Detac|+|Deta1|+|Deta2|(%lg) <= 0",fabs(Detac)+fabs(Deta1)+fabs(Deta2));

	sasfit_init_param( &subParam );
	subParam.p[0] = Rc+DR1+DR2;
	subParam.p[3] = Deta2;
	A  = sasfit_ff_sphere_f(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	subParam.p[0] = Rc+DR1;
	subParam.p[3] = Deta1-Deta2;
	A += sasfit_ff_sphere_f(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	subParam.p[0] = Rc;
	subParam.p[3] = Detac-Deta1;
	A += sasfit_ff_sphere_f(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	/*
	A =   K(interp,q,Rc+DR1+DR2,Deta2,error)
		+ K(interp,q,Rc+DR1,    Deta1-Deta2,error)
		+ K(interp,q,Rc,        Detac-Deta1,error);
	*/

	ftmp = 0.0;
	A = A*A;
	for (i=0; i < (int)n ;i++) 
	{
		for (j=0; j < (int)n ;j++) 
		{
			if (q*R[i][j] <= 0) 
			{
				ftmp = ftmp + A;
			} else 
			{
				ftmp = ftmp+A*sin(q*R[i][j])/(q*R[i][j]);
			}
		}
	}
	return ftmp;

}

