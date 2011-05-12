/*
 * src/sasfit_ff/disc_rwbrush/include/private.h
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

#ifndef DISC_RWBRUSH_PRIVATE_H
#define DISC_RWBRUSH_PRIVATE_H

#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>
#include "../../include/sasfit_ff_utils.h"

/*
float P_DISC_RWbrush(Tcl_Interp *interp, 
			float q,
			float Lc,		// core thickness of the layer
			float nagg,		 // aggregation number of chains per surface area
			float V_brush,	// volume of a single chain in brush
			float eta_core,	// scattering length density of non-swollen core
			float eta_brush,	// scattering length of a single chain in brush
			float eta_solv,	// scattering length density of core
			float xsolv_core,// amount of solvent in core
			float Rg,		// gyration radius of chain
			float d,			// correction factor should be chosen close to 1
			float D,			// diameter of disc
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
        DIA,
        NAGG,
        N_AGG,
        V_CORE,
        R,
	LC,
        Q
} variable_names;

#define V       param->p

#define DISC_RWBRUSH_HEADER \
\
	scalar rc, rs, pcs, pprime; \
	sasfit_param subParam; \
\
	SASFIT_ASSERT_PTR(param); \
\
	V[Q] = q; \
\
	SASFIT_CHECK_COND1((V[Q] < 0.0), param, "q(%lg) < 0",V[Q]); \
	SASFIT_CHECK_COND1((V[V_BRUSH] < 0.0), param, "V_brush(%lg) < 0",V[V_BRUSH]); \
	SASFIT_CHECK_COND1((V[RG] < 0.0), param, "Rg(%lg) < 0",V[RG]); \
	SASFIT_CHECK_COND1((V[DIA] < 0.0), param, "D(%lg) < 0",V[DIA]); \
	SASFIT_CHECK_COND( SASFIT_EQUAL(V[XSOLV_CORE], 1.0), param, "xsolv_core == 1"); \
\
	V[R] = 0.5*V[DIA]; \
\
	if ((V[Q] == 0.0) && (V[DIA] != 0)) \
	{ \
		pprime = 1.0; \
	} else if (V[DIA] == 0.0) \
	{ \
		pprime = 0.0; \
	} else { \
		pprime = 2.0/(V[Q]*V[Q] * V[R]*V[R]) * (1.0-gsl_sf_bessel_J1( V[DIA]*V[Q] ) / (V[Q] * V[R])); \
	}

#define DISC_RWBRUSH_BODY \
\
	V[NAGG] = 1.0*M_PI*V[DIA]*V[DIA]/4.0 * V[N_AGG]; \
\
	rs = V[V_CORE] * (V[ETA_CORE] - V[ETA_SOLV]); \
	rc = V[V_BRUSH] * (V[ETA_BRUSH] - V[ETA_SOLV]); \
\
	sasfit_init_param( &subParam ); \
	subParam.p[0] = V[LC]; \
	subParam.p[1] = V[RG]; \
	subParam.p[2] = V[D]; \
	subParam.p[3] = V[NAGG]; \
	subParam.p[4] = rc; \
	subParam.p[5] = rs; \
\
	pcs = sasfit_planar_gauss_chains(V[Q], &subParam); \
\
	return pcs * pprime

#endif

