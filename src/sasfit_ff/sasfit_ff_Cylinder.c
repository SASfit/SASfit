/*
 * src/sasfit_ff/sasfit_ff_Cylinder.c
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
 * gsl_sf_bessel_J1_e has to be used for errorhandling
 */
scalar sasfit_ff_Cylinder_core(scalar q, sasfit_param * param)
{
	scalar Q, R, L;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 2, &R, &L);
	Q	= param->p[MAXPAR-1];	// non-std args begin at the end

	if ((R == 0.0) || (L == 0.0))
	{
		return 0.0;
	}
	else if (Q == 0.0)
	{
		return gsl_pow_2(M_PI*R*R*L);
	}
	else if (q == 0)
	{
		// return 4.0*pow(bessj1(Q*R)*L*PI*R/Q,2.0);
		return 4.0 * gsl_pow_2(gsl_sf_bessel_J1(Q*R) * L * M_PI * R/Q);
	}
	else if (q == 1.0)
	{
		return 4.0 * gsl_pow_2( sin(0.5*Q*L) * M_PI * R * R/Q);
	}
	else
	{
		return 16.0 * gsl_pow_2(M_PI*R*R*L)
			    * gsl_pow_2( gsl_sf_bessel_J1(Q*R*sqrt(1.0-q*q))
				   * sin(Q*L*q/2.0)
				   / (Q*Q*R*sqrt(1.0-q*q)*L*q) );
	}
}


/**
 * form factor of a spherical Cylinder with radius R, height L and scattering
 * length density eta
 */
scalar sasfit_ff_Cylinder(scalar q, sasfit_param * param)
{
	scalar Sum, R, L, eta;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 4, &R, &L, EMPTY, &eta);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R);
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L);

	if (R == 0.0) return 0.0;
	if (L == 0.0) return 0.0;

	param->p[MAXPAR-1] = q;	// additional value at the end

	Sum = 0.0;
	Sum = sasfit_qromb(0.0, 1.0, sasfit_ff_Cylinder_core, param);

	return eta*eta * Sum;
}
