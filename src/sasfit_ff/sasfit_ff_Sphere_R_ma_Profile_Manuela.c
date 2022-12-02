/*
 * src/sasfit_ff/sasfit_ff_Sphere_R_ma_Profile_Manuela.c
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
#include <gsl/gsl_sf.h>
#include "include/sasfit_ff.h"
#include "include/sasfit_ff_utils.h"

scalar getBetaDistr(scalar a, scalar b, scalar x1)
{
	scalar xa, xb;

	if (a == 0) 
	{
		xa = 1.;
	} else 
	{
		xa = pow(x1,a-1.);
	}
	if (b == 0) 
	{
		xb = 1.;
	} else 
	{
                xb = pow(1.-x1,b-1.);
	}
        return xa*xb/gsl_sf_beta(a,b);
}

scalar sasfit_ff_Sphere_R_ma_x_beta_ave_core(scalar x, sasfit_param * param)
{
        scalar Q, u, alpha, Rc, Rm, a, b, x1, betadistr;

	Q	= param->p[MAXPAR-1];
	Rc	= param->p[MAXPAR-2];
	Rm	= param->p[MAXPAR-3];
	alpha	= param->p[6];
	Rc	= param->p[0];
	a	= param->p[8];
	b	= param->p[9];

	x1 = (x-Rc)/(Rm-Rc);
        betadistr = getBetaDistr(a, b, x1);

	u = Q*x;
        if (u == 0.0)
	{
		return pow(x,-alpha)*betadistr* 4.0 * M_PI * x*x;
	}
        return pow(x,-alpha)*betadistr * 4.0 * M_PI * x*x * sin(u)/u;
}


scalar sasfit_ff_Sphere_R_ma_x_beta_V_core(scalar x, sasfit_param * param)
{
        scalar Q,alpha, Rc, Rm, a, b, x1, betadistr;

	Q	= param->p[MAXPAR-1];
	Rc	= param->p[MAXPAR-2];
	Rm	= param->p[MAXPAR-3];
	alpha	= param->p[6];
	Rc	= param->p[0];
	a	= param->p[8];
	b	= param->p[9];

	x1 = (x-Rc)/(Rm-Rc);
        betadistr = getBetaDistr(a, b, x1);

        return pow(x,-alpha)*betadistr* 4.0 * M_PI * x*x;
}


/*
float P_Sphere_R_ma_Profile_Manuela(Tcl_Interp *interp,
				float Q,
				  float Rc,
				float Vc,
				float Vsh,
				float D_rho_c,
			  float D_rho_sh,
			  float r_SiOx,
				float alpha,
				float t,
				float a,
				float b,
				  bool  *error)
*/
scalar sasfit_ff_Sphere_R_ma_Profile_Manuela(scalar q, sasfit_param * param)
{
	scalar Plocal, Psh, Fsh, Fc, Pc, V, Nagg, b_c, b_sh, res, res1, res2;
	scalar Rc, Vc, Vsh, D_rho_c, D_rho_sh, r_SiOx, alpha, t, a, b, Rm;
	scalar FSiOx = 0.0; // was uninitialized, see line 179
	sasfit_param subParam;

        SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 10, &Rc, &Vc, &Vsh, &D_rho_c, &D_rho_sh, &r_SiOx, &alpha, &t, &a, &b);

	switch( param->kernelSelector )
	{
		case SPHERE_RMA_MANU1:
		case SPHERE_RMA_MANU2:
			SASFIT_CHECK_COND1((Vc <= 0.0), param, "Vc(%lg) < 0",Vc);
			if (Rc == 0.0) return 0.0;
			V = 4./3. * M_PI * pow(Rc,3.);
			Nagg = V / Vc;
			break;
		default:
			SASFIT_ERR_UNKNOWN_KERNEL(param, "kernel");
			break;
	}
	b_c  = Vc	* D_rho_c;	// Vc  * Drho_c
	b_sh = Vsh	* D_rho_sh;	// Vsh * Drho_sh

	Fc = sasfit_sphere_fc(q, Rc);
	Pc = Fc*Fc;

	if ((t == 0.0) || (b_sh == 0.0)) 
	{
		Fsh = Psh = 0.0;
	} else 
	{
		param->p[MAXPAR-1] = q;
		param->p[MAXPAR-2] = Rc;	// integration start
		param->p[MAXPAR-3] = Rm = Rc+t;	// integration end
		//Fsh = SPHERE_R_ma_ave(interp,q,Rc,Rc+t,alpha,error);
		res1 = sasfit_integrate(Rc, Rm, sasfit_ff_Sphere_R_ma_Profile_ave_core, param);
		if (alpha == 3.0) 
		{
			res2 = 4.*M_PI*(log(Rm/Rc));
		} else 
		{
			res2 = 4./(3.-alpha)*M_PI*(pow(Rm,3.-alpha)-pow(Rc,3.-alpha));
		}
		Fsh = res1/res2;

		res1 = sasfit_integrate(Rc, Rc+t, sasfit_ff_Sphere_R_ma_x_beta_ave_core, param);
		res2 = sasfit_integrate(Rc, Rc+t, sasfit_ff_Sphere_R_ma_x_beta_V_core, param);
		FSiOx = res1/res2;

		Psh = pow(Fsh+r_SiOx*FSiOx,2.0);
	}
	sasfit_init_param( &subParam );
	subParam.p[0] = 1.0;
	subParam.p[1] = t;
	subParam.p[2] = t;
	subParam.p[3] = 0.0;
	Plocal = sasfit_ff_WormLikeChainEXV(q, &subParam);

	SASFIT_CHECK_SUB_ERR(param, subParam);

        res =   Nagg*Nagg       * b_c  * b_c  * Pc +
//              Nagg*(Nagg-1.0) * b_sh * b_sh * Psh * ((Nagg < 1.) ?  0. : 1.) +
                Nagg*Nagg       * b_sh * b_sh * Psh +
            2.0*Nagg*Nagg       * b_c  * b_sh * Fc * (Fsh+r_SiOx*FSiOx) +
                0;
//              Nagg            * b_sh * b_sh * Plocal;

	if ( param->kernelSelector == SPHERE_RMA_MANU2 )
	{
		return Nagg*res;
	}
	return res;
}
