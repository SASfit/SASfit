/*
 * src/sasfit_ff/sasfit_ff_Sphere_RWbrush.c
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

#include <gsl/gsl_math.h>
#include "include/sasfit_ff_utils.h"

/*
float P_Sphere_RWbrush(Tcl_Interp *interp, 
			float q,
			float Rc,		// core radius
			float n_agg,		// aggregation number of chains per surface area
			float V_brush,	// volume of a single chain in brush
			float eta_core,	// scattering length density of non-swollen core
			float eta_brush,	// scattering length of a single chain in brush
			float eta_solv,	// scattering length density of core
			float xsolv_core,// amount of solvent in core
			float Rg,		// gyration radius of chain
			float d,			// correction factor should be chosen close to 1
			bool *error)
*/
scalar sasfit_ff_Sphere_RWbrush(scalar q, sasfit_param * param)
{
	scalar rc, rs, R, Nagg, S, V, res, V_core;
	scalar Rc, n_agg, V_brush, eta_core, eta_brush, eta_solv, xsolv_core, Rg, d;
	sasfit_param subParam;

        SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 9, EMPTY, EMPTY, &V_brush, &eta_core, &eta_brush, &eta_solv, &xsolv_core, &Rg, &d);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((V_brush < 0.0), param, "V_brush(%lg) < 0",V_brush);
	SASFIT_CHECK_COND1((Rg < 0.0), param, "Rg(%lg) < 0",Rg);
	SASFIT_CHECK_COND((xsolv_core == 1.0), param, "xsolv_core == 1");

	switch( param->kernelSelector )
	{
		case SPHERE_RWBRUSH:
			Rc		= param->p[0];
			n_agg		= param->p[1];

			V = 4./3. * M_PI * pow(Rc,3.);
			S = 4.*M_PI*Rc*Rc;
			R = Rc;

			Nagg = n_agg*S;

			if (Rc == 0) return 0;
			if (Nagg == 0.0) return pow(V*(1.-xsolv_core)*(eta_core-eta_solv)*3.*(sin(q*R)-q*R*cos(q*R))/pow(q*R,3.),2.);

			rs = V/Nagg * (eta_core  - eta_solv)*(1.-xsolv_core);
			break;
		case SPHERE_RWBRUSH_RC:
			Rc		= param->p[0];
			V_core		= param->p[1];

			R = Rc;
			SASFIT_CHECK_COND1((V_core < 0.0), param, "V_core(%lg) < 0",V_core);
			Nagg = 4./3.*M_PI*R*R*R*(1-xsolv_core)/V_core;

			rs = V_core * (eta_core  - eta_solv);
			break;
		case SPHERE_RWBRUSH_NAGG:
			Nagg		= param->p[0];
			V_core		= param->p[1];

			SASFIT_CHECK_COND1((V_core < 0.0), param, "V_core(%lg) < 0",V_core);
			R = pow(fabs(Nagg*(V_core/(1.-xsolv_core))*3./4./M_PI),1./3.);

			rs = V_core * (eta_core - eta_solv);
			break;
		default:
			SASFIT_ERR_UNKNOWN_KERNEL(param, "kernel");
			break;
	}

	rc = V_brush * (eta_brush - eta_solv);

	sasfit_init_param( &subParam );
	subParam.p[0] = R; 
	subParam.p[1] = Rg;
	subParam.p[2] = d;
	subParam.p[3] = Nagg;
	subParam.p[4] = rc;
	subParam.p[5] = rs;

//	return SphereWithGaussChains(interp,q,R,Rg,d,Nagg,rc,rs,error);
	res = sasfit_ff_SphereWithGaussChains(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return res;
}

