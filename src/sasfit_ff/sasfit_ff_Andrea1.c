/*
 * src/sasfit_ff/sasfit_ff_Andrea1.c
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

#include "include/sasfit_ff_utils.h"

/*
float Andrea1(Tcl_Interp *interp,
			 float q, 
			 float R, 
			 float DR, 
			 float c, 
			 float eta_core,
		  float dR2,
		  float shell1,
		  float shell2,
		  float c2,
			 bool  *error)
*/

/**
 * Formfactor for Andrea's nanocapsules
 * R:		  radius of core
 * DR:		 shell thickness
 * c:		  amount of H2O in solvent
 * eta_core: scattering length density of miglyol core
 */
scalar sasfit_ff_Andrea1(scalar q, sasfit_param * param)
{
	scalar R1, R2, R3, etaShell, etaSolvent, etaD2O, etaH2O, eta1, eta2, eta3, etaPolaxamer, etaoutsh, coutsh;
	scalar R, DR, c, eta_core, dR2, shell1, shell2, c2;
	scalar k1, k2, k3, k4;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 8, &R, &DR, &c, &eta_core, &dR2, &shell1, &shell2, &c2);

	R2 = R; 	/* R2 = R-DR;	*/
	R1 = R+DR;	/* R1 = R;	*/
	etaShell = 1.5423e10;
	etaShell = shell1;
	etaD2O = 6.3159e10;
	etaH2O = -5.6267e9;
	etaPolaxamer = 0.638e10;
	etaPolaxamer = shell2;
	coutsh = 0.8;
	coutsh = c2;
	etaSolvent = etaD2O*(1.0-c) + c*etaH2O;
	eta2 = eta_core - etaSolvent;
	eta1 = etaShell - etaSolvent;
	etaoutsh = (1.0-coutsh)*etaPolaxamer + coutsh*etaSolvent;
	eta3 = etaoutsh - etaSolvent;
	R3 = R1+dR2;

	sasfit_init_param( &subParam );
	subParam.p[0] = R1;
	subParam.p[3] = eta1;
	k1 = sasfit_ff_sphere_f(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	subParam.p[0] = R2;
	subParam.p[3] = eta1 - eta2;
	k2 = sasfit_ff_sphere_f(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	subParam.p[0] = R3;
	subParam.p[3] = eta3;
	k3 = sasfit_ff_sphere_f(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	subParam.p[0] = R1;
	subParam.p[3] = eta3;
	k4 = sasfit_ff_sphere_f(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	/*
	return K(interp,q,R1,eta1,error) 
	     - K(interp,q,R2,eta1-eta2,error) 
	     + K(interp,q,R3,eta3,error)
	     - K(interp,q,R1,eta3,error);
	     */
	return k1 - k2 + k3 - k4;
}
