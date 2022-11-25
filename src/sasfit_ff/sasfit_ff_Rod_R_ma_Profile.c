/*
 * src/sasfit_ff/sasfit_ff_Rod_R_ma_Profile.c
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

scalar sasfit_ff_Rod_R_ma_ave_core(scalar x, sasfit_param * param)
{
        scalar Q, u, alpha;

        SASFIT_ASSERT_PTR(param);

        Q 	= param->p[MAXPAR-1];
        alpha 	= param->p[7];

        u = Q*x;

        if (u==0.0) return pow(x,-alpha)* 2.0 * M_PI * x;

        return pow(x,-alpha)* 2.0 * M_PI * x* gsl_sf_bessel_J0(u);
}


scalar sasfit_ff_Rod_R_ma_Profile(scalar q, sasfit_param * param)
{
	scalar Psh, Fsh, res1, res2, Fc, Pc, Vc, V, S, Nagg, b_c, b_sh, Pcs, Pp;
	scalar Plocal, Rc, n_agg, Vsh, rho_c, rho_sh, rho_solv, xsolv_core, alpha, t, H;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 10, EMPTY, EMPTY, &Vsh, &rho_c, &rho_sh, &rho_solv, &xsolv_core, &alpha, &t, &H);

	SASFIT_CHECK_COND1((t < 0.0), param, "t(%lg) <= 0",t);

	switch( param->kernelSelector )
	{
		case ROD_RMA:
			Rc		= param->p[0];
			n_agg		= param->p[1];
			if (Rc == 0.0) return 0.0;
			S = 2. * M_PI *Rc*H;
			Nagg = n_agg*S;
			V = M_PI *Rc*Rc * H;
			Vc = V * (1.-xsolv_core) / Nagg;
			break;
		case ROD_RMA_NAGG:
			n_agg		= param->p[0];
			Vc		= param->p[1];
			SASFIT_CHECK_COND1((n_agg < 0.0), param, "n_agg(%lg) < 0",n_agg);
			if ( n_agg == 0.0 ) return 0.0;
//	wrong equation		Rc = sqrt(n_agg*Vc/(1.-xsolv_core)/M_PI);
//  Solve[{Vc/(1 - xsolv)*nagg*2*Pi*Rc*H == Pi*Rc^2*H}, {Rc}]
//  here the total volume of the rod is given by V == Vc/(1 - xsolv)*nagg*2*Pi*Rc*H == Pi*Rc^2*H
//  {{Rc -> 0}, {Rc -> -((2 nagg Vc)/(-1 + xsolv))}}
			Rc = 2.0*n_agg*Vc/(1.0-xsolv_core);
			if (Rc == 0.0) return 0.0;
			S = 2. * M_PI *Rc*H;
			Nagg = n_agg*S;
			break;
		case ROD_RMA_RC:
			Rc		= param->p[0];
			Vc		= param->p[1];
			SASFIT_CHECK_COND1((Vc <= 0.0), param, "Vc(%lg) <= 0",Vc);
			if (Rc == 0.0) return 0.0;
			V = M_PI *Rc*Rc * H;
			Nagg = V * (1.-xsolv_core)/ Vc;
			break;
		default:
			SASFIT_ERR_UNKNOWN_KERNEL(param, "kernel");
			break;
	}

	if (Nagg == 0.0) return 0;

        b_c  = Vc    * (rho_c  - rho_solv);
        b_sh = Vsh   * (rho_sh - rho_solv);

	Fc = sasfit_rod_fc(q, Rc);
	Pc = Fc * Fc;

        if ((t == 0.0) || (b_sh==0.0)) 
	{
                Fsh = Psh = 0.0;
        } else 
	{
		SASFIT_CHECK_COND((alpha == 2 && Rc == 0), param, "alpha==2 and Rc==0");

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

        if (alpha==1) 
	{
                Plocal = atan(q*t)/(q*t);
        } else 
	{
                Plocal = 1/(q*t)
                   * gsl_sf_gamma(2.0*(1.0-alpha))
                   * (sin(2.*(1.-alpha)*atan(q*t)))
                   / pow(1.0+q*q*t*t,0.5*2.*(1.-alpha));
        }

        Pcs =   Nagg*Nagg       * b_c  * b_c  * Pc +
                    Nagg*(Nagg-1.0) * b_sh * b_sh * Psh * ((Nagg < 1) ?  0 : 1) +
//                  Nagg*Nagg           * b_sh * b_sh * Psh +
                        2.0*Nagg*Nagg   * b_c  * b_sh * Fc * Fsh;

	Pp =(2.0*gsl_sf_Si(q*H)/(q*H)-pow(sin(q*H*0.5)/(q*H*0.5),2.0));

	if ( param->kernelSelector == ROD_RMA_NAGG )
	{
        	return Pp*Pcs;
	}
	return Pp*Pcs + Nagg*b_sh*b_sh*Plocal;
}

scalar sasfit_ff_Rod_R_ma_Profile_v(scalar q, sasfit_param * param, int distr)
{
	scalar postfix = q;
        SASFIT_ASSERT_PTR(param);

	if ( distr != 9 ) postfix = param->p[9];

	if ( param->kernelSelector == ROD_RMA_NAGG ) 
	{
		     if (distr == 1) q = q          /(1.0-param->p[6]) + param->p[8];
		else if (distr == 6) q = param->p[1]/(1.0-q)           + param->p[8];
		else if (distr == 8) q = param->p[1]/(1.0-param->p[6]) + q;
		else                 q = param->p[1]/(1.0-param->p[6]) + param->p[8];
		q = 2.0 * param->p[0] * q;
	} else
	{
		     if (distr == 0) q = q           + param->p[8];
		else if (distr == 8) q = param->p[0] + q;
		else                 q = param->p[0] + param->p[8];
	}
	return M_PI * pow(q, 2.0) * postfix;
}

