/*
 * src/sasfit_ff/sasfit_ff_Sphere_SAWbrush.c
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
float P_Sphere_SAWbrush(Tcl_Interp *interp, 
			float q,
			float Rc,		// radius of spherical core
			float n_agg,		// aggregation number density of chains on sphere surface
			float V_brush,	// volume of a single chain in brush
			float eta_core,	// scattering length density of non-swollen core
			float eta_brush,	// scattering length of a single chain in brush
			float eta_solv,	// scattering length density of core
			float xsolv_core,// amount of solvent in core
			float Rg,		// gyration radius of chain
			float L,			// contour length
			float b,			// Kuhn length
			bool *error)
*/
scalar sasfit_ff_Sphere_SAWbrush(scalar q, sasfit_param * param)
{
	scalar s, R, Mthirtynine, sigma, nu, Nc, rhos, rhoc, S, V, b, V_core, Nagg, res;
	scalar Rc, n_agg, V_brush, eta_core, eta_brush, eta_solv, xsolv_core, Rg, L;
	sasfit_param subParam;

        SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 10, EMPTY, EMPTY, &V_brush, &eta_core, &eta_brush, &eta_solv, &xsolv_core, &Rg, &L, &b);

	SASFIT_CHECK_COND1((L <= 0.0), param, "L(%lg) <= 0",L);
	SASFIT_CHECK_COND1((b < 0.0), param, "b(%lg) < 0",b);

	switch( param->kernelSelector )
	{
		case SPHERE_SAW:
			Rc		= param->p[0];
			n_agg		= param->p[1];

			SASFIT_CHECK_COND((xsolv_core == 1.0), param, "xsolv_core == 1");
			V = 4./3. * M_PI * pow(Rc,3.);
			S = 4.*M_PI*Rc*Rc;
			Nc = n_agg*S;
			rhos = V/(1.-xsolv_core) * (eta_core  - eta_solv);
			break;
		case SPHERE_SAW_RC:
			Rc		= param->p[0];
			V_core		= param->p[1];

			V = 4./3. * M_PI * pow(Rc,3.);
			Nc = V*(1.-xsolv_core)/V_core;
			rhos = Nc*V_core  * (eta_core  - eta_solv);
			break;
		case SPHERE_SAW_NAGG:
			Nagg		= param->p[0];
			V_core		= param->p[1];

			V = (Nagg*V_core)/(1.-xsolv_core);
			Rc = pow(V*3./4./M_PI,1./3.);
			Nc = Nagg;
			rhos = Nc*V_core  * (eta_core  - eta_solv);
			break;
		default:
			SASFIT_ERR_UNKNOWN_KERNEL(param, "kernel");
			break;
	}

	if (Nc == 0.0) return 0.0;

	rhoc = V_brush           *(eta_brush - eta_solv);

	s = Rg;
	R = Rc;
	Mthirtynine = rhos + Nc*rhoc;
/* ?? not used here ...
	if (Rg == 0.0)
	{
		sigma = Nc/4.;
	}
*/
	sigma = Nc*pow(Rg,2.)/(4.*pow(R+Rg,2.));
	nu = 1.4*pow(sigma,1.04);

	//return Pthirtynine(interp,q, Nc, Rg, R, Rc, s, nu, Mthirtynine, rhos, rhoc, b, L,error);
	sasfit_init_param( &subParam );
	subParam.p[0] = Nc;
	subParam.p[1] = Rg;
	subParam.p[2] = R;
	subParam.p[3] = Rc;
	subParam.p[4] = s;
	subParam.p[5] = nu;
	subParam.p[6] = Mthirtynine;
	subParam.p[7] = rhos;
	subParam.p[8] = rhoc;
	subParam.p[9] = b;
	subParam.p[10]= L;

//	return SphereWithGaussChains(interp,q,R,Rg,d,Nagg,rc,rs,error);
	res = sasfit_Pthirtynine(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return res;
}

