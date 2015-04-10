/*
 * src/sasfit_ff/sasfit_ff_CoPolymerMicelleSphericalProfile.c
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
float CoPolymerMicelleSphericalProfile (Tcl_Interp *interp,
				float q,
				  float Rc,
				float Vc,
				float Vsh,
				float rho_c,
			  float rho_sh,
			  float rho_solv,
				float alpha,
				float t,
				float L,
				float b,
				  bool  *error)
*/
scalar sasfit_ff_CoPolymerMicelleSphericalProfile(scalar q, sasfit_param * param)
{

	scalar Plocal, Psh, Fsh, Fc, Pc, Nagg, b_c, b_sh, Vshtot, tmin, alpha_min,a2,c0,tmp,w2;
	scalar Rc, Vc, Vsh, rho_c, rho_sh, rho_solv, alpha, t, L, b, res1, res2, Rm;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 10, &Rc, &Vc, &Vsh, &rho_c, &rho_sh, &rho_solv, &alpha, &t, &L, &b);

	SASFIT_CHECK_COND((Rc == 0.0), param, "Rc == 0");

	Nagg = 4./3. * M_PI * Rc*Rc*Rc / Vc;
	b_c  = Vc	* (rho_c  - rho_solv);
	b_sh = Vsh	* (rho_sh - rho_solv);

	Fc = sasfit_sphere_fc(q, Rc);

	// calculate minimum neccassary tmin for fully collapsed corona 
	tmin = pow(Rc*Rc*Rc+3.0/4.0/M_PI*Vsh*Nagg,1./3.)-Rc;

	if (tmin >= t) 
	{
		// if requested thickness is choosen to small it will be expanded to that one of a 
		alpha_min = 0.0;
		sasfit_param_set_err(param, DBGINFO("Compact shell is required: thickness will be set to t=%lf and alpha_min=0.0 (q=%lf)"),tmin,q);
	} else {
		Vshtot = 4.0/(3.0-alpha)*M_PI*pow(Rc+t,3.0-alpha)-4.0/(3.-alpha)*M_PI*pow(Rc,3.0-alpha)/pow(Rc,-alpha);
		if ( Vshtot < (Nagg*Vsh) ) 
		{
			c0 = 4.0*M_PI*Rc*Rc*Rc/(Nagg*Vsh);
			tmp  = c0*pow(Rc/(Rc+t),c0) * log(Rc/(Rc+t));
//			w1  = gsl_sf_lambert_W0(tmp);
//			a1 = 3.0+c0-w1/log(Rc/(Rc+t));
			w2  = gsl_sf_lambert_Wm1(tmp);
			a2 = 3.0+c0-w2/log(Rc/(Rc+t));
//			alpha_min = ((a1>a2)?a2:a1);
			alpha_min = a2;
			tmin = t;
			sasfit_param_set_err(param, DBGINFO("Corona potential decay need to be reduced: thickness t=%lf and alpha_min=%lf (q=%lf)"), tmin, alpha_min, q);
		} else {
			alpha_min = alpha;
			tmin = t;
		}
	}
	// the above calculations are needed to get around some unphysical constrains

	Pc = Fc*Fc;
	if ((tmin == 0.0) || (b_sh==0.0)) 
	{
		Fsh = Psh = 0.0;
	} 
	else 
	{
		param->p[MAXPAR-1] = q;
		param->p[6] = alpha_min; // temporarily set
		Rm = Rc+tmin;
		res1 = sasfit_integrate(Rc, Rm, sasfit_ff_Sphere_R_ma_Profile_ave_core, param);
		param->p[6] = alpha;
		if (alpha == 3.0)
		{
			res2 = 4.*M_PI*(log(Rm/Rc));
		} else
		{
			res2 = 4./(3.-alpha)*M_PI*(pow(Rm,3.-alpha)-pow(Rc,3.-alpha));
		}
		Fsh = (res1/res2);
		//Fsh = SPHERE_R_ma_ave(interp,q,Rc,Rc+tmin,alpha_min,error);
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

	return  Nagg*Nagg	* b_c  * b_c  * Pc +
		Nagg*(Nagg-1.0) * b_sh * b_sh * Psh * ((Nagg < 1) ?  0 : 1) +
		2.0*Nagg*Nagg	* b_c  * b_sh * Fc * Fsh +
		Nagg		* b_sh * b_sh * Plocal;
}
