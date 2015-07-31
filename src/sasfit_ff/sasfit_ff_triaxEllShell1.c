/*
 * src/sasfit_ff/sasfit_ff_triaxEllShell1.c
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


scalar sasfit_ff_triaxEllShell1_core(scalar y, sasfit_param * param)
{
	scalar Q, a, b, c, t, eta_core, eta_shell, eta_solv, p, x, u_c, u_sh;
	scalar F_c, F_sh;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 7, &a, &b, &c, &t, &eta_core, &eta_shell, &eta_solv);
	Q		= param->p[MAXPAR-1];
	p		= param->p[MAXPAR-2]; // pow
	x		= param->p[MAXPAR-3]; // param from outer integration

	if (fabs(a*b*c) + fabs(t) == 0.0) return 0.0;

	u_c = Q*sqrt(( pow(a*cos(x*M_PI/2.),2.) + pow(b*sin(x*M_PI/2.),2.) )*(1.-y*y) + pow(c*y,2.));
	u_sh = Q*sqrt(( pow((a+t)*cos(x*M_PI/2.),2.) + pow((b+t)*sin(x*M_PI/2.),2.) ) * (1.-y*y) + pow((c+t)*y,2.));

	F_c  = 4./3.*M_PI*a*b*c*(eta_core-eta_shell);
	if (u_c != 0.0)
	{
		F_c  = F_c * 3*(sin(u_c) -u_c *cos(u_c) )/pow(u_c ,3);
	}

	F_sh  = 4./3.*M_PI*(a+t)*(b+t)*(c+t)*(eta_shell-eta_solv);
	if (u_sh != 0.0) 
	{
		F_sh  = F_sh * 3*(sin(u_sh)-u_sh*cos(u_sh))/pow(u_sh,3);
	}

	return pow(F_sh+F_c, p);
}


scalar sasfit_ff_triaxEllShell1_core_x(scalar x, sasfit_param * param)
{
	scalar res;

	SASFIT_ASSERT_PTR(param);

	param->p[MAXPAR-3] = x;

	res = sasfit_integrate(0.0, 1.0, sasfit_ff_triaxEllShell1_core, param);

	return res;
}

/*
float I_triaxEllShell1(Tcl_Interp *interp,
				 float Q,
					  float a,
				 float b,
				 float c,
				 float t,
				 float eta_core,
				 float eta_shell,
				 float eta_solv,
				 bool*error)
*/
scalar sasfit_ff_triaxEllShell1(scalar q, sasfit_param * param)
{
	scalar res, b, c, t;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 4, EMPTY, &b, &c, &t);

	SASFIT_CHECK_COND1((q < 0.0), param, "Q(%lg) < 0",q);
	SASFIT_CHECK_COND1((b < 0.0), param, "b(%lg) < 0",b);
	SASFIT_CHECK_COND1((c < 0.0), param, "c(%lg) < 0",c);
	SASFIT_CHECK_COND1((t < 0.0), param, "t(%lg) < 0",t);

	param->p[MAXPAR-1] = q;		// Q
	param->p[MAXPAR-2] = 2.0;	// pow

	res = sasfit_integrate(0.0, 1.0, sasfit_ff_triaxEllShell1_core_x, param);

	return res;
}

scalar sasfit_ff_triaxEllShell1_v(scalar q, sasfit_param * param, int distr)
{
	SASFIT_ASSERT_PTR(param);

	switch ( distr )
	{
		case 0:
			q = (q          +param->p[3]) * (param->p[1]+param->p[3]) * (param->p[2]+param->p[3]);
			break;
		case 1:
			q = (param->p[0]+param->p[3]) * (q          +param->p[3]) * (param->p[2]+param->p[3]);
			break;
		case 2:
			q = (param->p[0]+param->p[3]) * (param->p[1]+param->p[3]) * (q          +param->p[3]);
			break;
		case 3:
			q = (param->p[0]+ q)          * (param->p[1]+ q)          * (param->p[2]+ q);
			break;
		default:
			q = (param->p[0]+param->p[3]) * (param->p[1]+param->p[3]) * (param->p[2]+param->p[3]);
			break;
	}
	return 4.0/3.0 * M_PI * q;
}

