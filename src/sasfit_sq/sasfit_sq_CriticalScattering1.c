/*
 * src/sasfit_sq/sasfit_sq_CriticalScattering1.c
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

#include "include/sasfit_sq_utils.h"

/*
float S_CriticalScattering1(Tcl_Interp *interp,
				 float Q,
				float kappa,
				float zeta,
				bool  *error)
*/

/**
 * critical scattering structure factor
 * zeta correlation length
 */
scalar sasfit_sq_CriticalScattering1(scalar q, sasfit_param * param)
{
	scalar kappa, zeta;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 2, &kappa, &zeta);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((kappa < 0.0), param, "kappa(%lg) < 0",kappa);
	SASFIT_CHECK_COND1((zeta < 0.0), param, "zeta(%lg) < 0",zeta);

	return 1+kappa/(1+zeta*zeta*q*q);
}
