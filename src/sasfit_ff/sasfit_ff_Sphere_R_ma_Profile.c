/*
 * src/sasfit_ff/sasfit_ff_Sphere_R_ma_Profile.c
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

scalar sasfit_ff_Sphere_R_ma_Profile_ave_core(scalar x, sasfit_param * param)
{
	scalar Q,u,alpha;

        SASFIT_ASSERT_PTR(param);

        Q 	= param->p[MAXPAR-1];
        alpha 	= param->p[6];

        u = Q*x;

        if (u==0.0) return pow(x,-alpha)* 4.0 * M_PI * x*x;

        return pow(x,-alpha)* 4.0 * M_PI * x*x * sin(u)/u;
}


scalar sasfit_ff_Sphere_R_ma_Profile(scalar q, sasfit_param * param)
{
	scalar Plocal, Psh, Fsh, Fc, Pc, Vc, V, S, Nagg, b_c, b_sh;
	scalar smoothie = 1.0;
	scalar Rc, n_agg, Vsh, rho_c, rho_sh, rho_solv, alpha, t, Rm, L, b, res1, res2;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 10, EMPTY, EMPTY, &Vsh, &rho_c, &rho_sh, &rho_solv, &alpha, &t, &L, &b);

	switch( param->kernelSelector )
	{
		case SPHERE_RMA:
			Rc	= param->p[0];
			n_agg	= param->p[1];
			if (Rc == 0.0) return 0.0;
			Fc = sasfit_sphere_fc(q, Rc) * smoothie;
			Pc = Fc*Fc;
			V = 4./3. * M_PI * pow(Rc,3.);
			S = 4.*M_PI*Rc*Rc;
			Nagg = n_agg*S;
			if (Nagg == 0.0) return Pc;
			Vc = V / Nagg;
			break;
		case SPHERE_RMA_RC:
		case SPHERE_RMA_RC_SMOOTH:
			Rc	= param->p[0];
			Vc	= param->p[1];
			SASFIT_CHECK_COND1((Vc <= 0.0), param, "Vc(%lg) < 0",Vc);
			if (Rc == 0.0) return 0.0;
			Fc = sasfit_sphere_fc(q, Rc) * smoothie;
			Pc = Fc*Fc;
			V = 4./3. * M_PI * pow(Rc,3.);
			Nagg = V / Vc;
			break;
		case SPHERE_RMA_NAGG:
		case SPHERE_RMA_NAGG_SMOOTH:
			Nagg	= param->p[0];
			Vsh     = fabs(Vsh);
//			Vc	= param->p[1] - Vsh; // this was wrong Vc should be param->p[1]
			Vc	= fabs(param->p[1]);
			// I am using here the abs-value to avoid eventually negative value during a fit,
			// which can happen if the scattering curve is weakly dependent on this parameter or
			// if this function is not the right choice to describe the scattering curve
			SASFIT_CHECK_COND1((Nagg < 0.0), param, "Nagg(%lg) < 0",Nagg);
			V = Nagg*Vc;
			Rc = pow(V*3./4./M_PI,1./3.);
			Fc = sasfit_sphere_fc(q, Rc) * smoothie;
			Pc = Fc*Fc;
			break;
		default:
			SASFIT_ERR_UNKNOWN_KERNEL(param, "kernel");
			break;
	}
	if ( param->kernelSelector == SPHERE_RMA_NAGG_SMOOTH ||
	     param->kernelSelector == SPHERE_RMA_RC_SMOOTH )
	{
		b_c  = Vc	* rho_c;	// Vc  * Drho_c
		b_sh = fabs(Vsh)	* rho_sh;	// Vsh * Drho_sh
		smoothie = exp(-0.5 * q*q * rho_solv*rho_solv);	// rho_solv == sigma
	} else
	{
		b_c  = Vc	* (rho_c  - rho_solv);
		b_sh = fabs(Vsh)	* (rho_sh - rho_solv);
	}

	if ((t == 0.0) || (b_sh == 0.0))
	{
		Fsh = Psh = 0.0;
	} else
	{
		SASFIT_CHECK_COND(((alpha == 3) && (Rc == 0)), param, "alpha==3 and Rc==0");

		param->p[MAXPAR-1] = q;
		Rm = Rc+t;
		//Fsh = SPHERE_R_ma_ave(interp,q,Rc,Rc+t,alpha,error);
		res1 = sasfit_integrate(Rc, Rc+t, sasfit_ff_Sphere_R_ma_Profile_ave_core, param);
		if (alpha == 3.0)
		{
			res2 = 4.*M_PI*(log(Rm/Rc));
		} else
		{
			res2 = 4./(3.-alpha)*M_PI*(pow(Rm,3.-alpha)-pow(Rc,3.-alpha));
		}
		Fsh = (res1/res2) * smoothie;
		Psh = Fsh*Fsh;
	}
	sasfit_init_param( &subParam );
	subParam.p[0] = 1.0;
	subParam.p[1] = L;
	subParam.p[2] = b;
	subParam.p[3] = 0.0;
	//Plocal = WormLikeChainEXV(interp,q,1.0,L,b,0.0,error);
	Plocal = sasfit_ff_WormLikeChainEXV(q, &subParam);

	SASFIT_CHECK_SUB_ERR(param, subParam);

	if (Vsh<0) return Nagg * b_sh * b_sh * Plocal;
	return  Nagg*Nagg	* b_c  * b_c  * Pc +
		Nagg*(Nagg-1.0) * b_sh * b_sh * Psh * ((Nagg < 1) ?  0 : 1) +
		2.0*Nagg*Nagg	* b_c  * b_sh * Fc * Fsh +
		Nagg * b_sh * b_sh * Plocal;
}

scalar sasfit_ff_Sphere_R_ma_Profile_v(scalar q, sasfit_param * param, int distr)
{
	SASFIT_ASSERT_PTR(param);

	if ( param->kernelSelector == SPHERE_RMA_NAGG )
	{
		     if (distr == 0) return           q * param->p[1];
		else if (distr == 1) return param->p[0] * q;
		else                 return param->p[0] * param->p[1];
	} else
	{
		     if ( distr == 0 ) q =           q + param->p[7];
		else if ( distr == 7 ) q = param->p[0] + q;
		else                   q = param->p[0] + param->p[7];

		return sasfit_ff_sphere_v(q, param, 0);
	}
}

