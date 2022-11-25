/*
 * src/sasfit_ff/sasfit_ff_ellip_core_shell_old.c
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

typedef enum 
{
	A,
	B,
	T,
	ETA_C,
	ETA_SHELL,
	ETA_SOL,
	Q
} variable_names;


#define V	param->p

// a == R1, b == R2 == R3

#define CALC_RADIUS(r, mu, a, b, eta1, eta2) \
	do { \
		scalar vc, nu, xc; \
\
		vc = 4.0*M_PI*a*b*b/3.0; \
\
		nu = a/b; \
		xc = V[Q] * b * sqrt((nu*nu-1.0)*mu*mu + 1.0); \
\
		if (xc <= 0.0 || SASFIT_EQUAL(b, 0.0)) \
		{ \
			r = (eta1-eta2)*vc; \
		} else { \
			r = (eta1-eta2)*vc*3.0*sasfit_j1f(xc)/xc; \
		} \
	} while(0)


scalar sasfit_ff_ellip_core_shell_core(scalar mu, sasfit_param * param)
{
	scalar r1, r2;

	SASFIT_ASSERT_PTR(param);

	CALC_RADIUS(r1, mu, V[A], V[B], V[ETA_C], V[ETA_SHELL]);
	CALC_RADIUS(r2, mu, V[A]+V[T], V[B]+V[T], V[ETA_SHELL], V[ETA_SOL]);

	return (r1+r2)*(r1+r2);
}

scalar sasfit_ff_ellip_core_shell_f_core(scalar mu, sasfit_param * param)
{
	scalar r1, r2;

	SASFIT_ASSERT_PTR(param);

	CALC_RADIUS(r1, mu, V[A], V[B], V[ETA_C], V[ETA_SHELL]);
	CALC_RADIUS(r2, mu, V[A]+V[T], V[B]+V[T], V[ETA_SHELL], V[ETA_SOL]);

	return (r1+r2);
}

scalar sasfit_ff_ellip_core_shell_int(scalar q, sasfit_param *param, sasfit_func_one core_fct)
{
	scalar sum;

	SASFIT_ASSERT_PTR(param);

	V[Q] = q;

	SASFIT_CHECK_COND1((V[Q] < 0.0), param, "q(%lg) < 0",V[Q]);
	SASFIT_CHECK_COND1((V[B] < 0.0), param, "b(%lg) < 0",V[B]);
	SASFIT_CHECK_COND1((V[A] < 0.0), param, "a(%lg) < 0",V[A]);
	SASFIT_CHECK_COND1((V[T] < 0.0), param, "t(%lg) < 0",V[T]);

	sum = sasfit_integrate(0.0, 1.0, core_fct, param);

	return sum;
}

// below, functions available externally

scalar sasfit_ff_ellip_core_shell(scalar q, sasfit_param * param)
{
	return sasfit_ff_ellip_core_shell_int(q, param, sasfit_ff_ellip_core_shell_core);
}

scalar sasfit_ff_ellip_core_shell_f(scalar q, sasfit_param * param)
{
	return sasfit_ff_ellip_core_shell_int(q, param, sasfit_ff_ellip_core_shell_f_core);
}

scalar sasfit_ff_ellip_core_shell_v(scalar q, sasfit_param * param, int distr)
{
	scalar factor;

	SASFIT_ASSERT_PTR(param);

	switch ( distr )
	{
		case 0:
			factor = q    + V[T];
			q      = V[B] + V[T];
			break;
		case 1:
			factor = V[A] + V[T];
			q      = q    + V[T];
			break;
		case 2:
			factor = V[A] + q;
			q      = V[B] + q;
			break;
		default:
			factor = V[A] + V[T];
			q      = V[B] + V[T];
			break;
	}

	return 4.0/3.0 * M_PI * factor * pow(q, 2.0);
}

