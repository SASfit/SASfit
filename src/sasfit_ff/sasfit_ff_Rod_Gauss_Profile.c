/*
 * src/sasfit_ff/sasfit_ff_Rod_Gauss_Profile.c
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


scalar sasfit_Rod_Gauss_ave_core(scalar x, sasfit_param * param)
{
        scalar Q, u, Rc, sigma, t, phi_r;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 3, &Rc, &t, &sigma);
	Q 	= param->p[MAXPAR-1];

        u = Q*x;

        if (x < Rc) 
	{
                phi_r = 0.0;
        } 
	else if (x < Rc+t) 
	{
                phi_r = 1.0;
        } else 
	{
                phi_r = exp(-pow((x-(Rc+t))/sigma,2.0)/2.);
        }
        if (u==0.0) return phi_r * 2.0 * M_PI * x;

        return phi_r * 2.0 * M_PI * x* gsl_sf_bessel_J0(u);
}



scalar sasfit_ff_Rod_Gauss_Profile(scalar q, sasfit_param * param)
{
	scalar Psh, Fsh, Fsh1, Fsh2, Fc, Pc, Vc, V, S, Nagg, b_c, b_sh, Pcs, Pp, Plocal;
	scalar Rc, n_agg, Vsh, rho_c, rho_sh, rho_solv, xsolv_core, t, sigma, H;
	sasfit_param subParam;

        SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 10, EMPTY, EMPTY, &Vsh, &rho_c, &rho_sh, &rho_solv, &xsolv_core, &t, &sigma, &H);

        switch( param->kernelSelector )
	{
		case ROD_GAUSS_RC:
			Rc	= param->p[0];
			Vc	= param->p[1];
			SASFIT_CHECK_COND1((Vc <= 0.0), param, "Vc(%lg) <= 0", Vc);
			if ( Rc == 0.0 ) return 0.0;
			V = M_PI * Rc*Rc*H;
			S = 2.0 * M_PI*Rc*H;
			Nagg = V * (1.-xsolv_core)/ Vc;
			break;
		case ROD_GAUSS_NAGG:
			n_agg	= param->p[0];
			Vc	= param->p[1];
			SASFIT_CHECK_COND1((n_agg < 0.0), param, "n_agg(%lg) < 0",n_agg);
			if ( n_agg==0.0 ) return 0.0;
//	wrong equation		Rc = sqrt(n_agg*Vc/(1.-xsolv_core)/M_PI);
//  Solve[{Vc/(1 - xsolv)*nagg*2*Pi*Rc*H == Pi*Rc^2*H}, {Rc}]
//  here the total volume of the rod is given by V == Vc/(1 - xsolv)*nagg*2*Pi*Rc*H == Pi*Rc^2*H
//  {{Rc -> 0}, {Rc -> -((2 nagg Vc)/(-1 + xsolv))}}
			Rc = 2.0*n_agg*Vc/(1.0-xsolv_core);
			S = 2.0 * M_PI*Rc*H;
			Nagg = n_agg*S;
			break;
		case ROD_GAUSS:
			Rc	= param->p[0];
			n_agg	= param->p[1];
			if (Rc == 0.0) return 0.0;
			V = M_PI *Rc*Rc * H;
			S = 2. * M_PI *Rc*H;
			Nagg = n_agg*S;
			if (Nagg == 0.0) return 0;
			Vc = V * (1.-xsolv_core) / Nagg;
			break;
		default:
			SASFIT_ERR_UNKNOWN_KERNEL(param, "kernel");
			break;
	}

	Fc = sasfit_rod_fc(q, Rc);
	Pc = Fc * Fc;

	b_c  = Vc  * (rho_c  - rho_solv);
	b_sh = Vsh * (rho_sh - rho_solv);


	if (b_sh == 0.0) 
	{
		Fsh = Psh = 0.0;
	} else 
	{
		sasfit_init_param( &subParam );
		subParam.p[0] = Rc;
		subParam.p[1] = t;
		subParam.p[2] = sigma;

		subParam.p[MAXPAR-1] = q;
		Fsh1 = sasfit_integrate(Rc, Rc+t+4.0*sigma, sasfit_Rod_Gauss_ave_core, &subParam);
		subParam.p[MAXPAR-1] = 0.0;
		Fsh2 = sasfit_integrate(Rc, Rc+t+4.0*sigma, sasfit_Rod_Gauss_ave_core, &subParam);

		SASFIT_CHECK_SUB_ERR(param, subParam);

		Fsh = Fsh1/Fsh2;
		Psh = Fsh*Fsh;
	}
	Pcs =	Nagg*Nagg		 * b_c  * b_c  * Pc +
			 Nagg*(Nagg-1.0) * b_sh * b_sh * Psh * ((Nagg < 1) ?  0 : 1) +
//			 Nagg*Nagg		* b_sh * b_sh * Psh +
			2.0*Nagg*Nagg	* b_c  * b_sh * Fc * Fsh;

	Pp =(2.0*gsl_sf_Si(q*H)/(q*H)-pow(sin(q*H*0.5)/(q*H*0.5),2.0));

	Plocal =(2.0*gsl_sf_Si(q*(t+2*sigma))/(q*(t+2*sigma))-pow(sin(q*(t+2*sigma)*0.5)/(q*(t+2*sigma)*0.5),2.0));

	return Pp*Pcs + Nagg*b_sh*b_sh*Plocal;
}
