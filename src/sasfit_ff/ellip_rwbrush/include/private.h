/*
 * src/sasfit_ff/ellip_rwbrush/include/private.h
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

#ifndef ELLIP_RWBRUSH_PRIVATE_H
#define ELLIP_RWBRUSH_PRIVATE_H

#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>
#include "../../include/sasfit_ff_utils.h"
#include "utils.h"

/*
float P_Ellipsoid_RWbrush(Tcl_Interp *interp,
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
			float epsilon,	// stretching factor of ellipsoidal micelle (R,R,epsilon*R)
			bool *error)


EllMic (Q, Vpolym_c, Vpolym_sh, eta_poly_c, eta_poly_sh, eta_solv, xsolv_c, Rg, d, Nagg, epsilon)

is equal to

P_Ellipsoid_RWbrush[ELLIP_RWBRUSH_NAGG](q, Nagg, Vpolym_c, Vpolym_sh, eta_poly_c, eta_poly_sh, eta_solv, xsolv_c, Rg, d, epsilon)
(parameter Nagg from pos 8 to pos 0 in param array)

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
	EPSILON,
	RC,
	V_CORE,
	N_AGG,
	NAGG,
	R,
	Q
} variable_names;

#define V       param->p

#define ELLIP_RWBRUSH_HEADER \
\
	scalar r_core, r_chain, p36; \
\
	SASFIT_ASSERT_PTR(param); \
\
	V[Q] = q; \
\
	SASFIT_CHECK_COND1((V[Q] < 0.0), param, "q(%lg) < 0",V[Q]); \
	SASFIT_CHECK_COND1((V[V_BRUSH] < 0.0), param, "V_brush(%lg) < 0",V[V_BRUSH]); \
	SASFIT_CHECK_COND1((V[RG] < 0.0), param, "Rg(%lg) < 0",V[RG]); \
	SASFIT_CHECK_COND1((V[XSOLV_CORE] == 1.0), param, "xsolv_core(%lg) == 1",V[XSOLV_CORE]); \
	SASFIT_CHECK_COND1((V[EPSILON] <= 0.0), param, "epsilon(%lg) <= 0",V[EPSILON])


#define ELLIP_RWBRUSH_BODY \
\
        r_core  = V[NAGG]*V[V_CORE]  * (V[ETA_CORE]  - V[ETA_SOLV]); \
        r_chain =         V[V_BRUSH] * (V[ETA_BRUSH] - V[ETA_SOLV]); \
\
        p36 =       pow(r_core,2.0)*sasfit_integrate(0.0, M_PI_2, sasfit_ff_ellip_rwbrush_fs_core, param); \
        p36 = p36 + V[NAGG]*r_chain*r_chain*sasfit_ff_P18(V[Q], V[RG]); \
        p36 = p36 + V[NAGG]*(V[NAGG]-1.0)*r_chain*r_chain*sasfit_integrate(0.0, M_PI_2, sasfit_ff_ellip_rwbrush_scc_core, param)*((V[NAGG] < 1) ?  0 : 1); \
        p36 = p36 + 2.0*V[NAGG]*r_core*r_chain*sasfit_integrate(0.0, M_PI_2, sasfit_ff_ellip_rwbrush_ssc_core, param); \
\
        return p36

#endif

