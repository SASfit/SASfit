/*
 * src/sasfit_ff/ellip/include/private.h
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

#ifndef ELLIP_PRIVATE_H
#define ELLIP_PRIVATE_H

#include <gsl/gsl_math.h>
#include "../../include/sasfit_ff.h"
#include "../../include/sasfit_ff_utils.h"
#include "utils.h"

/*
float I_Ellipsoid_1(Tcl_Interp *interp,
				 float Q,
					  float R,
					  float V,
					  float eta,
					  bool  *error)
*/
/**
 * form factor of a spherical ellipsoid with radius R, volume V and
 * scattering  length density eta
 */

typedef enum
{
	R,
	PARAM1,
	PARAM2,
	ETA,
	NU,
	VOL,
	Q
} variable_names;

#define V       param->p

#define ELLIP_HEADER \
\
	scalar sum = 0.0; \
\
	SASFIT_ASSERT_PTR(param); \
\
	V[Q] = q; \
\
	SASFIT_CHECK_COND1((V[Q] < 0.0), param, "q(%lg) < 0",V[Q]); \
	SASFIT_CHECK_COND1((V[R] < 0.0), param, "R(%lg) < 0",V[R]); \
\
	if (SASFIT_EQUAL(V[R], 0.0)) return 0.0


#define ELLIP_BODY \
\
	return V[ETA]*V[ETA]*sum


#endif

