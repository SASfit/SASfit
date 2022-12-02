/*
 * src/sasfit_ff/sasfit_ff_two_attached_spheres.c
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
#include "include/sasfit_ff_utils.h"

/*
float two_attached_spheres(Tcl_Interp *interp,
			 float Q, 
			 float R1, 
			 float R2,
		  float eta0,
			 float eta1, 
			 float eta2, 
			 bool  *error)
*/
scalar sasfit_ff_two_attached_spheres(scalar q, sasfit_param * param)
{
	scalar f1, f2, R12, R1, R2, eta0, eta1, eta2;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 5, &R1, &R2, &eta0, &eta1, &eta2);

	R12 = R1+R2;

	// f1 = K(interp,q,R1,eta1-eta0,error);
	sasfit_init_param( &subParam );
	subParam.p[0] = R1;
	subParam.p[3] = eta1-eta0;
	f1 = sasfit_ff_sphere_f(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	// f2 = K(interp,q,R2,eta2-eta0,error);
	subParam.p[0] = R2;
	subParam.p[3] = eta2-eta0;
	f2 = sasfit_ff_sphere_f(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	if (R12 == 0) return f1*f1 + f2*f2; 
	return f1*f1 + f2*f2 + 2.*f1*f2*sin(q*R12)/(q*R12);
}

/*
float K_two_attached_spheres(Tcl_Interp *interp,
			 float q, 
			 float R1, 
			 float R2,
		  float eta0,
			 float eta1, 
			 float eta2, 
			 bool  *error)
*/
scalar sasfit_ff_two_attached_spheres_f(scalar q, sasfit_param * param)
{
	scalar f1, f2, R0, Vt, R1, R2, eta0, eta1, eta2;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 5, &R1, &R2, &eta0, &eta1, &eta2);

	Vt = (eta1-eta0)*R1*R1*R1 + (eta2-eta0)*R2*R2*R2;
	if (Vt == 0) return 0;

	R0 = (-(eta1-eta0)*R1*R1*R1*R1 + (eta2-eta0)*R2*R2*R2*R2)/Vt;

	// f1 = K(interp,q,R1,eta1-eta0,error);
	sasfit_init_param( &subParam );
	subParam.p[0] = R1;
	subParam.p[3] = eta1-eta0;
	f1 = sasfit_ff_sphere_f(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	// f2 = K(interp,q,R2,eta2-eta0,error);
	subParam.p[0] = R2;
	subParam.p[3] = eta2-eta0;
	f2 = sasfit_ff_sphere_f(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	if (R0 == 0) return f1+f2; 
	return f1*(((q*(R1+R0))==0)?1.0:sin(q*(R1+R0))/(q*(R1+R0))) + f2*(((q*(R2-R0))==0)?1.0:sin(q*(R2-R0))/(q*(R2-R0)));
}
