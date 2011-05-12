/*
 * src/plugins/triax_ellip_shell/sasfit_ff_triax_ellip_shell.c
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

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define B	param->p[1]
#define C	param->p[2]
#define T	param->p[3]
#define ETA_C	param->p[4]
#define ETA_SH	param->p[5]
#define ETA_SOL	param->p[6]

scalar sasfit_ff_triax_ellip_shell_core(scalar y, sasfit_param * param)
{
	scalar q, p, x, u_c, u_sh;
	scalar f_c, f_sh;

	SASFIT_ASSERT_PTR(param);

	q		= param->p[MAXPAR-1];
	p		= param->p[MAXPAR-2]; // pow
	x		= param->p[MAXPAR-3]; // param from outer integration

	if (fabs(A*B*C) + fabs(T) == 0.0) return 0.0;

	u_c = q*sqrt(( pow(A*cos(x*M_PI/2.),2.) + pow(B*sin(x*M_PI/2.),2.) )*(1.-y*y) + pow(C*y,2.));
	u_sh = q*sqrt(( pow((A+T)*cos(x*M_PI/2.),2.) + pow((B+T)*sin(x*M_PI/2.),2.) ) * (1.-y*y) + pow((C+T)*y,2.));

	f_c  = 4./3.*M_PI*A*B*C*(ETA_C-ETA_SH);
	if (u_c != 0.0)
	{
		f_c  = f_c * 3*(sin(u_c) -u_c *cos(u_c) )/pow(u_c ,3);
	}

	f_sh  = 4./3.*M_PI*(A+T)*(B+T)*(C+T)*(ETA_SH-ETA_SOL);
	if (u_sh != 0.0) 
	{
		f_sh  = f_sh * 3*(sin(u_sh)-u_sh*cos(u_sh))/pow(u_sh,3);
	}

	return pow(f_sh+f_c, p);
}

scalar sasfit_ff_triax_ellip_shell_core_x(scalar x, sasfit_param * param)
{
	scalar res;

	SASFIT_ASSERT_PTR(param);

	param->p[MAXPAR-3] = x;

	res = sasfit_integrate(0.0, 1.0, sasfit_ff_triax_ellip_shell_core, param);

	return res;
}

scalar sasfit_ff_triax_ellip_shell(scalar q, sasfit_param * param)
{
	scalar res;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A);
	SASFIT_CHECK_COND1((B < 0.0), param, "b(%lg) < 0",B);
	SASFIT_CHECK_COND1((C < 0.0), param, "c(%lg) < 0",C);
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T);
//	SASFIT_CHECK_COND1((ETA_C < 0.0), param, "eta_c(%lg) < 0",ETA_C); // modify condition to your needs
//	SASFIT_CHECK_COND1((ETA_SH < 0.0), param, "eta_sh(%lg) < 0",ETA_SH); // modify condition to your needs
//	SASFIT_CHECK_COND1((ETA_SOL < 0.0), param, "eta_sol(%lg) < 0",ETA_SOL); // modify condition to your needs

	param->p[MAXPAR-1] = q;		// Q
	param->p[MAXPAR-2] = 2.0;	// pow

	res = sasfit_integrate(0.0, 1.0, sasfit_ff_triax_ellip_shell_core_x, param);

	return res;
}

scalar sasfit_ff_triax_ellip_shell_f(scalar q, sasfit_param * param)
{
	// insert your code here
	return 0.0;
}

scalar sasfit_ff_triax_ellip_shell_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param);

	switch ( dist )
	{
		case 0:
			q = (q + T) * (B + T) * (C + T);
			break;
		case 1:
			q = (A + T) * (q + T) * (C + T);
			break;
		case 2:
			q = (A + T) * (B + T) * (q + T);
			break;
		case 3:
			q = (A + q) * (B + q) * (C + q);
			break;
		default:
			q = (A + T) * (B + T) * (C + T);
			break;
	}
	return 4.0/3.0 * M_PI * q;
}

