/*
 * src/sasfit_ff/sasfit_ff_Disc.c
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

scalar sasfit_ff_Disc(scalar q, sasfit_param * param)
{
	scalar R, u, eta;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 4, &R, EMPTY, EMPTY, &eta);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R);

	u = R*R*q*q;
	if (fabs(q*R) <= 1.0e-6) return gsl_pow_2(eta*R*M_PI)*(R*R)*(1-u/6.+u*u/72.-u*u*u/1440.+gsl_pow_4(u)/43200.);
	//   return 2.0*eta*eta*M_PI*M_PI*R*R/(Q*Q)*(1.0-bessj1(2.0*Q*R)/(Q*R));
	return gsl_pow_2(eta*R*M_PI)*2.0/(q*q)*(1.0-gsl_sf_bessel_J1(2.0*q*R)/(q*R));
}

scalar sasfit_ff_Disc_f(scalar q, sasfit_param * param)
{
	scalar R, eta;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 4, &R, EMPTY, EMPTY, &eta);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R);

	if (q*R == 0.0) return eta*R*R*M_PI;;
	return eta*M_PI*R*R*2*(cos(q*R)-1.0)/(q*q*R*R);
}
