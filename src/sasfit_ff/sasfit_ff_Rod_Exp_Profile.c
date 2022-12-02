/*
 * src/sasfit_ff/sasfit_ff_Rod_Exp_Profile.c
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
#include "include/sasfit_ff_utils.h"


scalar sasfit_Rod_Exp_ave_core(scalar x, sasfit_param * param)
{
	scalar Q, u, Rc, alpha, etashin, etashout, etaexp, t, arg;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 4, &Rc, &etashout, &t, &alpha);
	Q		= param->p[MAXPAR-1];

	u=Q*x;
	etashin = 1.0;
	arg = (x-Rc)/t;

	if (x<Rc)	return 0.0;
	if (x>Rc+t)	return 0.0;

	if (alpha < 0) 
	{
		etaexp = etashin+(etashout-etashin)*arg*exp((1-arg)*alpha);
	} else 
	{
		etaexp = etashout+(etashin-etashout)*(1-arg)*exp(-arg*alpha);
	}

	if (u==0.0) return etaexp * 2.0 * M_PI * x;

	return etaexp * 2.0 * M_PI * x* gsl_sf_bessel_J0(u);
}


scalar sasfit_ff_Rod_Exp_Profile(scalar q, sasfit_param * param)
{
	scalar Psh, Fsh, Fsh1, Fsh2, Fc, Pc, Vc, V, S, Nagg, b_c, b_sh, Pcs, Pp, Plocal;
	scalar eta_out_in;
	scalar Rc, n_agg, Vsh, rho_c, rho_sh, rho_solv, xsolv_core, t, alpha, H;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 10, EMPTY, EMPTY, &Vsh, &rho_c, &rho_sh, &rho_solv, EMPTY, &t, &alpha, &H);

	switch( param->kernelSelector )
	{
		case ROD_EXP:
			Rc		= param->p[0];
			n_agg		= param->p[1];
			xsolv_core	= param->p[6];
			eta_out_in	= 0.0;
			if (Rc == 0.0) return 0.0;
			V = M_PI *Rc*Rc * H;
			S = 2. * M_PI *Rc*H;
			Nagg = n_agg*S;
			if (Nagg == 0.0) return 0;
			Vc = V * (1.-xsolv_core) / Nagg;
			break;
		case ROD_EXP_RC:
			Rc		= param->p[0];
			Vc		= param->p[1];
			eta_out_in	= param->p[6];
			xsolv_core	= 0.0;
			if (Rc == 0.0) return 0.0;
			SASFIT_CHECK_COND1((Vc <= 0.0), param, "Vcore(%lg) <= 0", Vc);
			V = M_PI *Rc*Rc * H;
			S = 2. * M_PI *Rc*H;
			Nagg = V * (1.-xsolv_core)/ Vc;
			break;
		case ROD_EXP_NAGG:
			n_agg		= param->p[0];
			Vc		= param->p[1];
			eta_out_in	= param->p[6];
			xsolv_core	= 0.0;
			SASFIT_CHECK_COND1((n_agg < 0.0), param, "n_agg(%lg) < 0", n_agg);
			if ( n_agg==0.0 ) return 0.0;
//	wrong equation		Rc = sqrt(n_agg*Vc/(1.-xsolv_core)/M_PI);
//  Solve[{Vc/(1 - xsolv)*nagg*2*Pi*Rc*H == Pi*Rc^2*H}, {Rc}]
//  here the total volume of the rod is given by V == Vc/(1 - xsolv)*nagg*2*Pi*Rc*H == Pi*Rc^2*H
//  {{Rc -> 0}, {Rc -> -((2 nagg Vc)/(-1 + xsolv))}}

			Rc = 2.0*n_agg*Vc/(1.0-xsolv_core);
			S = 2. * M_PI *Rc*H;
			Nagg = n_agg*H;
			if (Nagg == 0.0) return 0;
			break;
                default:
			SASFIT_ERR_UNKNOWN_KERNEL(param, "kernel");
			break;
	}

	Fc = sasfit_rod_fc(q, Rc);
	Pc = Fc * Fc;

	b_c  = Vc  * (rho_c  - rho_solv);
	b_sh = Vsh * (rho_sh - rho_solv);

	if (b_sh==0.0) 
	{
		Fsh = Psh = 0.0;
	} else 
	{
		//Fsh = ROD_Exp_ave(interp,q,Rc,eta_out_in,t,alpha,error);
		sasfit_init_param( &subParam );
		subParam.p[0] = Rc;
		subParam.p[1] = eta_out_in;
		subParam.p[2] = t;
		subParam.p[3] = alpha;

		subParam.p[MAXPAR-1] = q;
		Fsh1 = sasfit_integrate(Rc, Rc+t, sasfit_Rod_Exp_ave_core, &subParam);
		subParam.p[MAXPAR-1] = 0.0;
		Fsh2 = sasfit_integrate(Rc, Rc+t, sasfit_Rod_Exp_ave_core, &subParam);

		SASFIT_CHECK_SUB_ERR(param, subParam);

		Fsh = Fsh1/Fsh2;
		Psh = Fsh*Fsh;
	}
	Pcs = Nagg*Nagg	* b_c*b_c * Pc +
			 Nagg*(Nagg-1.0) * b_sh * b_sh * Psh * ((Nagg < 1) ?  0 : 1) +
//			 Nagg*Nagg		* b_sh * b_sh * Psh +
			2.0*Nagg*Nagg	* b_c  * b_sh * Fc * Fsh;

	Pp =(2.0*gsl_sf_Si(q*H)/(q*H)-pow(sin(q*H*0.5)/(q*H*0.5),2.0));

	Plocal =(2.0*gsl_sf_Si(q*t)/(q*t)-pow(sin(q*t*0.5)/(q*t*0.5),2.0));

	return Pp*Pcs + Nagg*b_sh*b_sh*Plocal;
}

