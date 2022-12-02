/*
 * src/sasfit_ff/sasfit_ff_BiLayeredVesicle.c
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

#include "include/sasfit_ff_utils.h"

/*
float BiLayeredVesicle(Tcl_Interp *interp,
			float q,
			float R_c,
			float t_h,
			float t_t,
			float eta_sol,
			float eta_h,
			float eta_t,
			bool  *error)
*/

/**
		 R_c: radius of vesicle core
		t_h: thickness of headgroups of bilayer
		t_t: thickness of tailgroup of bilayer
		eta_sol: scattering length density of solvent
		eta_h: scattering length density of bilayer head group
		eta_t: scattering length density of bilayer tail
 */
scalar sasfit_ff_BiLayeredVesicle(scalar q, sasfit_param * param)
{
	scalar FF, R_c, t_h, t_t, eta_sol, eta_h, eta_t;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 6, &R_c, &t_h, &t_t, &eta_sol, &eta_h, &eta_t);

	sasfit_init_param( &subParam );
	subParam.p[0] = R_c+t_h+t_t+t_h;
	subParam.p[3] = eta_h-eta_sol;
	FF = sasfit_ff_sphere_f(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	subParam.p[0] = R_c+t_h+t_t;
	subParam.p[3] = eta_t-eta_h;
	FF = FF + sasfit_ff_sphere_f(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	subParam.p[0] = R_c+t_h;
	subParam.p[3] = eta_h-eta_t;
	FF = FF + sasfit_ff_sphere_f(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	subParam.p[0] = R_c;
	subParam.p[3] = eta_sol-eta_h;
	FF = FF + sasfit_ff_sphere_f(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);
/*
	FF =	  K(interp,q,R_c+t_h+t_t+t_h,eta_h-eta_sol,error);
	FF = FF + K(interp,q,R_c+t_h+t_t    ,eta_t-eta_h  ,error);
	FF = FF + K(interp,q,R_c+t_h	    ,eta_h-eta_t  ,error);
	FF = FF + K(interp,q,R_c	    ,eta_sol-eta_h,error);
*/
	return FF*FF;
}

scalar sasfit_ff_BiLayeredVesicle_v(scalar q, sasfit_param * param, int distr)
{
	SASFIT_ASSERT_PTR(param);

	     if ( distr == 0 ) q = q           + 2.0*param->p[1] + param->p[2];
	else if ( distr == 1 ) q = param->p[0] + 2.0*q           + param->p[2];
	else if ( distr == 2 ) q = param->p[0] + 2.0*param->p[1] + q;
	else                   q = param->p[0] + 2.0*param->p[1] + param->p[2];

	return sasfit_ff_sphere_v(q, param, 0);
}

