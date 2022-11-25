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


#define PARAM0      param->p[0]
#define PARAM1      param->p[1]
#define V_BRUSH     param->p[2]
#define ETA_CORE    param->p[3]
#define ETA_BRUSH   param->p[4]
#define ETA_SOLV    param->p[5]
#define XSOLV_CORE  param->p[6]
#define RG          param->p[7]
#define D           param->p[8]
#define DIA         param->p[9]
#define NAGG        param->p[MAXPAR-6]
#define N_AGG       param->p[MAXPAR-5]
#define V_CORE      param->p[MAXPAR-4]
#define R           param->p[MAXPAR-3]
#define	LC          param->p[MAXPAR-2]
#define Q           param->p[MAXPAR-1]
#define V param->p

#define DISC_RWBRUSH_HEADER \
\
	scalar rc, rs, pcs, pprime; \
	sasfit_param subParam; \
\
	SASFIT_ASSERT_PTR(param); \
\
	Q = q; \
\
	SASFIT_CHECK_COND1((Q < 0.0), param, "q(%lg) < 0",Q); \
	SASFIT_CHECK_COND1((V_BRUSH < 0.0), param, "V_brush(%lg) < 0",V_BRUSH); \
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); \
	SASFIT_CHECK_COND1((DIA < 0.0), param, "D(%lg) < 0",DIA); \
	SASFIT_CHECK_COND( SASFIT_EQUAL(XSOLV_CORE, 1.0), param, "xsolv_core == 1"); \
\
	R = 0.5*DIA; \
\
	if ((Q == 0.0) && (DIA != 0)) \
	{ \
		pprime = 1.0; \
	} else if (DIA == 0.0) \
	{ \
		pprime = 0.0; \
	} else { \
		pprime = 2.0/(Q*Q * R*R) * (1.0-gsl_sf_bessel_J1( DIA*Q ) / (Q*R)); \
	}

#define DISC_RWBRUSH_BODY \
\
	NAGG = (2.0*M_PI*DIA*DIA/4.0 + 0.0*2.0*M_PI*R*LC) * N_AGG; \
\
	rs = V_CORE * (ETA_CORE - ETA_SOLV); \
	rc = V_BRUSH * (ETA_BRUSH - ETA_SOLV); \
\
	sasfit_init_param( &subParam ); \
	subParam.p[0] = LC; \
	subParam.p[1] = RG; \
	subParam.p[2] = D; \
	subParam.p[3] = NAGG; \
	subParam.p[4] = rc; \
	subParam.p[5] = rs; \
	subParam.p[6] = pprime;\
\
	return sasfit_planar_gauss_chains(Q, &subParam);

#endif

