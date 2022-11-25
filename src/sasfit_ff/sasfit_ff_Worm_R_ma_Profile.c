/*
 * src/sasfit_ff/sasfit_ff_Worm_R_ma_Profile.c
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
#include <gsl/gsl_sf.h>
#include "include/sasfit_ff_utils.h"

/*
float P_WORM_R_ma_Profile(Tcl_Interp *interp,
				float q,
				  float Rc,
				float n_agg,
				float Vsh,
				float rho_c,
			  float rho_sh,
			  float rho_solv,
				float alpha,
				float t,
				float l,
				float L,
				  bool  *error)
*/
scalar sasfit_ff_Worm_R_ma_Profile(scalar q, sasfit_param * param)
{
	scalar Psh, Fsh, Fc, Pc, Vc, V, S, Nagg, b_c, b_sh, Pcs, Pp, Plocal;
	scalar Rc, n_agg, Vsh, rho_c, rho_sh, rho_solv, alpha, t, l, L, res1, res2;
	const scalar xsolv_core = 0.0;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 10, EMPTY, EMPTY, &Vsh, &rho_c, &rho_sh, &rho_solv, &alpha, &t, &l, &L);

	SASFIT_CHECK_COND1((alpha > 1.0), param, "alpha(%lg) > 1",alpha);

	switch( param->kernelSelector )
	{
		case WORM_RMA:
			Rc	= param->p[0];
			n_agg	= param->p[1];
			if (Rc == 0.0) return 0.0;

			S = 2. * M_PI *Rc*L;
			Nagg = n_agg*S;
			if (Nagg == 0.0) return 0;

			V = M_PI *Rc*Rc * L;
			Vc = V * (1.-xsolv_core) / Nagg;
			break;
		case WORM_RMA_RC:
			Rc	= param->p[0];
			Vc	= param->p[1];
			SASFIT_CHECK_COND1((Vc <= 0.0), param, "Vc(%lg) <= 0",Vc);
			SASFIT_CHECK_COND1((t < 0.0), param, "t(%lg) <= 0",t);
			if (Rc == 0.0) return 0.0;
			V = M_PI *Rc*Rc * L;
			Nagg = V * (1.-xsolv_core)/ Vc;
			break;
		case WORM_RMA_NAGG:
			n_agg	= param->p[0];
			Vc	= param->p[1];
			SASFIT_CHECK_COND1((n_agg < 0.0), param, "n_agg(%lg) < 0",n_agg);
			if ( n_agg==0.0 ) return 0.0;
			Rc = sqrt(n_agg*Vc/(1.-xsolv_core)/M_PI);

			S = 2. * M_PI *Rc*L;
			Nagg = n_agg*S;
			if (Nagg == 0.0) return 0;
			break;
		default:
			SASFIT_ERR_UNKNOWN_KERNEL(param, "kernel");
			break;
	}

	Fc = sasfit_rod_fc(q, Rc);
	Pc = Fc*Fc;

	b_c  = Vc    * (rho_c  - rho_solv);
	b_sh = Vsh   * (rho_sh - rho_solv);


	if ((t == 0.0) || (b_sh==0.0)) 
	{
		Fsh = Psh = 0.0;
	} else 
	{
		SASFIT_CHECK_COND((alpha == 2) && (Rc == 0), param, "alpha==2 and Rc==0");

		param->p[MAXPAR-1] = q;

		// Fsh = ROD_R_ma_ave(interp,q,Rc,Rc+t,alpha,error);
		res1 = sasfit_integrate(Rc, Rc+t, sasfit_ff_Rod_R_ma_ave_core, param);
		if (alpha == 2.0)
		{
			res2 = 2.*M_PI*(log((Rc+t)/Rc));
		} else
		{
			res2 = 2./(2.-alpha)*M_PI*(pow((Rc+t),2.-alpha)-pow(Rc,2.-alpha));
		}

		Fsh = res1/res2;
		Psh = Fsh*Fsh;
	}

	if (alpha == 1) 
	{
		Plocal = atan(q*t)/(q*t);
	} else 
	{
		Plocal =  1/(q*t)
		   * gsl_sf_gamma(2.0*(1.0-alpha))
		   * (sin(2.*(1.-alpha)*atan(q*t)))
		   / pow(1.0+q*q*t*t,0.5*2.*(1.-alpha));
	}

	Pcs =   Nagg*Nagg       * b_c  * b_c  * Pc +
		Nagg*(Nagg-1.0) * b_sh * b_sh * Psh * ((Nagg < 1) ?  0 : 1) +
//		  Nagg*Nagg	   * b_sh * b_sh * Psh +
		2.0*Nagg*Nagg   * b_c  * b_sh * Fc * Fsh;

//    Pp =(2.0*gsl_sf_Si(q*H)/(q*H)-pow(sin(q*H*0.5)/(q*H*0.5),2.0));

	sasfit_init_param( &subParam );
	subParam.p[0] = 0.0;
	subParam.p[1] = l;
	subParam.p[2] = L;
	Pp = sasfit_ff_KholodenkoWorm(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return Pp*Pcs + Nagg*b_sh*b_sh*Plocal;
}

