/*
 * src/sasfit_sq/sasfit_sq_HardSphere.c
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

#include "include/sasfit_sq_utils.h"

/*
double S_HardSphere(Tcl_Interp *interp,
			double Q, 
			double RHS, 
			double fp, 
			bool  *error)
*/

/**
 * structure factor of particles interacting with the hard-sphere potential
 * RHS: hards sphere radius
 * fp: hard sphere volume fraction
 * [Percus-Yevick approximation]
 */
scalar sasfit_sq_HardSphere(scalar q, sasfit_param * param)
{
	scalar RHS, fp,A;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 2, &RHS, &fp);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RHS <= 0.0), param, "RHS(%lg) <= 0",RHS);
	SASFIT_CHECK_COND1((fp >= 1.0), param, "fp(%lg) >= 1",fp);
	SASFIT_CHECK_COND1((fp < 0.0), param, "fp(%lg) < 0",fp);

	if (fp == 0.0) return 1.0;
	
	A=2.0*RHS*q;
	if (fabs(A) > 1E-6) {
		return 1.0/(1.0+24.0*fp*sasfit_g(fp,A)/A);
	} else {
		return gsl_pow_4(fp-1.0)/gsl_pow_2(2.0*fp+1.0)
			+(gsl_pow_4(-1 + fp)*fp*(16 + fp*(-11 + 4*fp)))/(20.*gsl_pow_4(1 + 2*fp)) * gsl_pow_2(A)
			+(gsl_pow_4(-1 + fp)*fp*(-20 + fp*(386 + fp*(-627 + fp*(494 + fp*(-173 + 21*fp))))))/
			   (700.*gsl_pow_6(1 + 2*fp))* gsl_pow_4(A);
	}
}
