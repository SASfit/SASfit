/*
 * src/sasfit_ff/ellip_core_shell/include/private.h
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

#ifndef ELLIP_CORE_SHELL_INTERNAL_H
#define ELLIP_CORE_SHELL_INTERNAL_H

#include <gsl/gsl_math.h>
#include "../../include/sasfit_ff_utils.h"

// a == R1, b == R2 == R3

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


#define CALC_RADIUS(r, mu, a, b, eta1, eta2) \
	do { \
		scalar v, nu, x; \
\
		v = 4.0*M_PI*(a)*(b)*(b)/3.0; \
\
		nu = (a)/(b); \
		x = V[Q] * (b) * sqrt((nu*nu-1.0)*(mu)*(mu) + 1.0); \
\
		if (x <= 0.0 || SASFIT_EQUAL((b), 0.0)) \
		{ \
			r = ((eta1)-(eta2))*v; \
		} else { \
			r = ((eta1)-(eta2))*v*3.0*sasfit_j1f(x)/x; \
		} \
	} while(0)


#define CALC_RADII \
	scalar r1 = 0.0, r2 = 0.0; \
\
	SASFIT_ASSERT_PTR(param); \
\
	CALC_RADIUS(r1, mu,  V[A],        V[B],       V[ETA_C],     V[ETA_SHELL]); \
	CALC_RADIUS(r2, mu, (V[A]+V[T]), (V[B]+V[T]), V[ETA_SHELL], V[ETA_SOL])


#define V	param->p


#define ELLIP_CORE_SHELL_INTEGRATE(core_fct) \
\
	scalar sum; \
\
	SASFIT_ASSERT_PTR(param); \
\
	V[Q] = q; \
\
	SASFIT_CHECK_COND1((V[Q] < 0.0), param, "q(%lg) < 0",V[Q]); \
	SASFIT_CHECK_COND1((V[B] < 0.0), param, "b(%lg) < 0",V[B]); \
	SASFIT_CHECK_COND1((V[A] < 0.0), param, "a(%lg) < 0",V[A]); \
	SASFIT_CHECK_COND1((V[T] < 0.0), param, "t(%lg) < 0",V[T]); \
\
	sum = sasfit_integrate(0.0, 1.0, (core_fct), param); \
\
	return sum


scalar ellip_core_shell_core(scalar mu, sasfit_param * param);

scalar ellip_core_shell_f_core(scalar mu, sasfit_param * param);

#endif

