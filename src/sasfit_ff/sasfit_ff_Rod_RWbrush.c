/*
 * src/sasfit_ff/sasfit_ff_Rod_RWbrush.c
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

/*
float P_Rod_RWbrush(Tcl_Interp *interp, 
			float q,
			float Rc,		// core radius
			float n_agg,		// aggregation number of chains per surfacearea
			float V_brush,	// volume of a single chain in brush
			float eta_core,	// scattering length density of non-swollen core
			float eta_brush,	// scattering length of a single chain in brush
			float eta_solv,	// scattering length density of core
			float xsolv_core,// amount of solvent in core
			float Rg,		// gyration radius of chain
			float d,			// correction factor should be chosen close to 1
			float H	,		// length of cylinder
			bool *error)
*/
scalar sasfit_ff_Rod_RWbrush(scalar q, sasfit_param * param)
{
	scalar r_core, r_chain, R, Nagg, S, V, V_core, res;
	scalar Rc, n_agg, V_brush, eta_core, eta_brush, eta_solv, xsolv_core, Rg, d, H;
	sasfit_param subParam;

        SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 10, EMPTY, EMPTY, &V_brush, &eta_core, &eta_brush, &eta_solv, &xsolv_core, &Rg, &d, &H);

	SASFIT_CHECK_COND1((q < 0.0), 		param, "q(%lg) < 0", q);
	SASFIT_CHECK_COND1((V_brush < 0.0), 	param, "V_brush(%lg) < 0", V_brush);
	SASFIT_CHECK_COND1((Rg < 0.0), 		param, "Rg(%lg) < 0", Rg);
	SASFIT_CHECK_COND1((xsolv_core == 1.0),	param, "xsolv_core(%lg) == 1",xsolv_core);
	SASFIT_CHECK_COND1((H < 0.0), 		param, "H(%lg) < 0",H);

	switch( param->kernelSelector )
	{
		case ROD_RWBRUSH:
			Rc		= param->p[0];
			n_agg		= param->p[1];
			R = Rc;
			V = M_PI * pow(R,2.)*H;
			S = 2.*M_PI*R*H;
			Nagg = n_agg*S;
			if (Nagg != 0) {
				V_core = V*(1.-xsolv_core)/Nagg;
			} else {
				V_core = 0;
			}
			break;
		case ROD_RWBRUSH_RC:
			Rc		= param->p[0];
			V_core		= param->p[1];
			SASFIT_CHECK_COND1((V_core < 0.0),      param, "V_core(%lg) < 0", V_core);
			R = Rc;
			Nagg = M_PI * pow(Rc,2.)*H* (1.-xsolv_core) / V_core;
			break;
		case ROD_RWBRUSH_NAGG:
			n_agg		= param->p[0];
			V_core		= param->p[1];
			SASFIT_CHECK_COND1((V_core < 0.0),      param, "V_core(%lg) < 0", V_core);
			if ( n_agg == 0.0 ) return 0.0;
			R  = 2.*n_agg*V_core/(1-xsolv_core);
			S = 2.*M_PI*R*H;
			Nagg = n_agg*S;
			break;
		default:
			SASFIT_ERR_UNKNOWN_KERNEL(param, "kernel");
			break;
	}


        r_core  = Nagg*V_core  * (eta_core  - eta_solv);
        r_chain =      V_brush * (eta_brush - eta_solv);

	sasfit_init_param( &subParam );
	subParam.p[0] = R;
	subParam.p[1] = Rg;
	subParam.p[2] = d;
	subParam.p[3] = Nagg;
	subParam.p[4] = H;
	subParam.p[5] = r_core;
	subParam.p[6] = r_chain;

	res = sasfit_ff_very_long_cyl_w_gauss_chains(q, &subParam);

	SASFIT_CHECK_SUB_ERR(param, subParam);

	return res;
}

