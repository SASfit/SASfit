/*
 * src/sasfit_ff/sasfit_ff_BlockCopolymerMicelle.c
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
float BlockCopolymerMicelle(Tcl_Interp *interp,
		float q, 
		  float Vpolym_c,	 // in nm^3 if q in nm^-1, in A^3 if q in A^-1
		float xsolv_c,	  // amount of solvent in the core
		float Vpolym_sh,	// in nm^3 if q in nm^-1, in A^3 if q in A^-1
		float eta_poly_c,  // (bulk) scattering length density of block unit in core
		float eta_poly_sh, // (bulk) scattering length density of block unit in shell
		float eta_solv,	 // scattering length density of solvent
		float Nagg,		  // aggregation number
		float Rg,			 // radius of gyration of polymer chains in shell
		float d,			  // correction factor should be chosen close to 1
		  bool  *error)
*/
scalar sasfit_ff_BlockCopolymerMicelle(scalar q, sasfit_param * param)
{
	scalar rc, rs, R;
	scalar Vpolym_c, xsolv_c, Vpolym_sh, eta_poly_c, eta_poly_sh, eta_solv, Nagg, Rg, d;
	scalar result;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 9, &Vpolym_c, &xsolv_c, &Vpolym_sh, &eta_poly_c, &eta_poly_sh, &eta_solv, &Nagg, &Rg, &d);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((Vpolym_c < 0.0), param, "Vpolym_c(%lg) < 0",Vpolym_c);
	SASFIT_CHECK_COND1((Vpolym_sh < 0.0), param, "Vpolym_sh(%lg) < 0",Vpolym_sh);
	SASFIT_CHECK_COND1((Rg < 0.0), param, "Rg(%lg) < 0",Rg);
	SASFIT_CHECK_COND((xsolv_c == 1.0), param, "xsolv_c == 1");

	R = 	pow(fabs(Nagg*(Vpolym_c/(1.-xsolv_c))*3./4./M_PI),1./3.);
	rs = 	Vpolym_c*(eta_poly_c-eta_solv);
	rc = 	Vpolym_sh*(eta_poly_sh-eta_solv);

	sasfit_init_param( &subParam );
	subParam.p[0] = R;
	subParam.p[1] = Rg;
	subParam.p[2] = d;
	subParam.p[3] = Nagg;
	subParam.p[4] = rc;
	subParam.p[5] = rs;

	// return SphereWithGaussChains(interp,q,R,Rg,d,Nagg,rc,rs,error);
	result = sasfit_ff_SphereWithGaussChains(q, &subParam);

	SASFIT_CHECK_SUB_ERR(param, subParam);

	return result;
}
