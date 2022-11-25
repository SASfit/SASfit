/*
 * src/sasfit_ff/cyl_rwbrush/include/private.h
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

#ifndef CYL_RWBRUSH_PRIVATE_H
#define CYL_RWBRUSH_PRIVATE_H

#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>
#include "../../include/sasfit_ff_utils.h"
#include "utils.h"

/*
float P_Cylinder_RWbrush(Tcl_Interp *interp, 
			float Q,
			float Rc,		// core radius
			float n_agg,		// aggregation number of chains per surfacearea
			float V_brush,	// volume of a single chain in brush
			float eta_core,	// scattering length density of non-swollen core
			float eta_brush,	// scattering length of a single chain in brush
			float eta_solv,	// scattering length density of core
			float xsolv_core,// amount of solvent in core
			float Rg,		// gyration radius of chain
			float d,			// correction factor should be chosen close to 1
			float H,			// height of cylinder
			bool *error)
*/


typedef enum
{
	PARAM0,
	PARAM1,
	V_BRUSH,
	ETA_CORE,
	ETA_BRUSH,
	ETA_SOLV,
	XSOLV_CORE,
	RG,
	D,
	H,
	NAGG,
	N_AGG,
	V_CORE,
	R,
	Q
} variable_names;

#define CYL_RWBRUSH_TESTS \
	SASFIT_CHECK_COND1((V[Q] < 0.0),           param, "q(%lg) < 0",V[Q]); \
	SASFIT_CHECK_COND1((V[RG] < 0.0),          param, "Rg(%lg) < 0",V[RG]); \
	SASFIT_CHECK_COND1((V[V_BRUSH] < 0.0),     param, "V_brush(%lg) < 0",V[V_BRUSH]); \
	SASFIT_CHECK_COND1((V[XSOLV_CORE] == 1.0), param, "xsolv_core(%lg) == 1",V[XSOLV_CORE])

#define CYL_RWBRUSH_HEADER \
\
	CYL_RWBRUSH_BODY_VARS; \
	SASFIT_ASSERT_PTR(param); \
	V[Q] = q

#define V       param->p


#endif

