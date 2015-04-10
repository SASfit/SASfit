/*
 * src/sasfit_ff/sasfit_ff_SphellWithGaussChainsSimon1.c
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
float SphellWithGaussChainsSimon1(Tcl_Interp *interp,
		float Q, 
		float Nagg,
		float Rg,
		float d,
		float rho_solv,
		float rho_core,
		float b_A_h,
		float V_A_h,
		float b_A_l,
		float V_A_l,
		float rho_B,
//		float R,	  // radius of the spherical core
//		float Rg,	 // radius of gyration of polymer chains in shell
//		float d,	  // correction factor should be chosen close to 1
//		float Nagg,  // aggregation number
//		float rc,	 // excess scattering of chain (corona)
//		float rs,	 // excess scattering length of the part in the spherical core 
		  bool  *error)
*/
/**
 * form factor of a spherical shell with gaussian chains attached
 */
scalar sasfit_ff_SphellWithGaussChainsSimon1(scalar q, sasfit_param * param)
{
	scalar PHI,PHI1,PHI2,Fs,Fc,Scc,Ssc,I,w,sinc_x,rc,rs, R,Rt,t,VB,Vsh,Vcore, xsolutol,xcapriol,xmiglyol, rho_sh;
	scalar Nagg, Rg, d, rho_solv, rho_core, b_A_h, V_A_h, b_A_l, V_A_l, rho_B;

        SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 10, &Nagg, &Rg, &d, &rho_solv, &rho_core, &b_A_h, &V_A_h, &b_A_l, &V_A_l, &rho_B);

	xsolutol = 0.5755;
	xcapriol = 0.2127;
	xmiglyol = 0.2118;

	if (Nagg == 0) return 0;
	rc = b_A_h-V_A_h*rho_solv;
	
	VB = (V_A_h+V_A_l)*xcapriol/xsolutol;

	Vsh = Nagg * (V_A_l+VB);							

	rho_sh = (b_A_l/V_A_l * V_A_l/(V_A_h+V_A_l)*xsolutol + rho_B*xcapriol) / (xsolutol*V_A_l/(V_A_h+V_A_l)+xcapriol);

	Vcore = Nagg * VB*xmiglyol/xcapriol;

	R  = pow(3./(4.*M_PI)* Vcore	  ,1./3.);
	Rt = pow(3./(4.*M_PI)*(Vcore+Vsh),1./3.);
	t = Rt-R;

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0", q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R);
	SASFIT_CHECK_COND1((Rg < 0.0), param, "Rg(%lg) < 0",Rg);

	if (q * R == 0) 
	{
		PHI1 = 1;
	} else 
	{
		PHI1 = 3 * ((sin(q * R) - q * R * cos(q * R)) / pow(q * R,3));
	}
	if (q * Rt == 0) 
	{
		PHI2 = 1;
	} else 
	{
		PHI2 = 3 * ((sin(q * Rt) - q * Rt * cos(q * Rt)) / pow(q * Rt,3));
	}

	rs =   (Vcore+Vsh)*(rho_sh-rho_solv)
	+ Vcore*(rho_core-rho_sh) ;

	PHI = (Vcore+Vsh)*(rho_sh-rho_solv)*PHI2
	+ Vcore*(rho_core-rho_sh) * PHI1;

	PHI = PHI/rs;
	Fs = PHI*PHI;
	Fc = sasfit_gauss_fc(q, Rg);
	w = sasfit_rwbrush_w(q, Rg);
	if (q * (Rt + d * Rg) == 0) 
	{
		sinc_x = 1.0;
	} else 
	{
		sinc_x = sin(q * (Rt + d * Rg)) / (q * (Rt + d * Rg));
	}

	Scc = pow(w*sinc_x,2);
	Ssc = PHI * w * sinc_x;
	I = Nagg*Nagg*rs*rs*Fs + Nagg*rc*rc*Fc + Nagg*(Nagg-1)*((Nagg < 1) ?  0 : 1)*rc*rc*Scc + 2*Nagg*Nagg*rs*rc*Ssc;

	return I;
}

