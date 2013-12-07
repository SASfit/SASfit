/*
 * src/sasfit_ff/dozier_star/include/private.h
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

#ifndef DOZIER_STAR_PRIVATE_H
#define DOZIER_STAR_PRIVATE_H

#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>
#include "../../include/sasfit_ff_utils.h"

/*
float DozierStar(Tcl_Interp *interp,
			float q,
			float I0,
				float Rg,
			float alpha,
				float xi,
			float nu,
				bool  *error)
*/
/**
 * form factor of a star polymer with Gaussian statistics according to Dozier
 * scattering length density:  eta
 */

typedef enum 
{
	I0,
	RG,
	PARAM2,
	XI,
	NU,
	ALPHA,
	NAGG,
	MU,
	Q
} variable_names;

#define V       param->p

#define DOZIER_STAR_HEADER \
\
	SASFIT_ASSERT_PTR(param); \
\
	V[Q] = q; \
\
	SASFIT_CHECK_COND1((V[Q] < 0.0), param, "q(%lg) < 0",V[Q]); \
	SASFIT_CHECK_COND1((V[XI] < 0.0), param, "xi(%lg) < 0",V[XI]); \
	SASFIT_CHECK_COND1((V[NU] <= 0.0), param, "nu(%lg) <= 0",V[NU]); \
	SASFIT_CHECK_COND1((V[NU] > 1.0), param, "nu(%lg) > 1",V[NU]); \
\
	V[MU] = 1.0/V[NU]-1.0


#endif

