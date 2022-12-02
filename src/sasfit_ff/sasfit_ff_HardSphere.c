/*
 * src/sasfit_ff/sasfit_ff_HardSphere.c
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
#include <sasfit_sq.h>
#include "include/sasfit_ff_utils.h"

/*
float HardSphere(Tcl_Interp *interp,
			 float q, 
			 float R, 
			 float RHS, 
			 float fp, 
			 float eta, 
			 bool  *error)
			 */
/**
 * form factor of a sphere with hard sphere interaction potential
 * radius R of sphere
 * eta scattering length density
 * fp volume fraction
 * RHS radius of hard sphere repulsion
 */
scalar sasfit_ff_HardSphere(scalar q, sasfit_param * param)
{
	scalar hard_sph, K;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	sasfit_init_param( &subParam );

	subParam.p[0] = param->p[1]; //RHS
	subParam.p[1] = param->p[2]; //fp
	hard_sph = sasfit_sq_HardSphere(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	K = sasfit_ff_sphere_f(q, param);

	//return S_HardSphere(interp,q,RHS,fp,error)*K(interp,q,R,eta,error)*K(interp,q,R,eta,error);
	return hard_sph * K*K;
}

