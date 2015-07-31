/*
 * src/sasfit_ff/sasfit_ff_DaisyLikeRing.c
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


scalar sasfit_ff_DaisyLikeRing(scalar q, sasfit_param * param)
{
	scalar u, P1r, Pmr, Rg, m, I0;
	int k;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 4, &Rg, &m, EMPTY, &I0);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((Rg < 0.0), param, "Rg(%lg) < 0",Rg);
	SASFIT_CHECK_COND1((m < 1.0), param, "m(%lg) < 1",m);

	Pmr = 0.0;
	k = (int) m;
	u = q*q * Rg*Rg;
	if (u  == 0.0) 
	{
		P1r = I0;
	} else {
		P1r = I0*sqrt(2./(u*u))*gsl_sf_dawson(sqrt(u*u/2.));
	}
	Pmr = 1./k * (P1r+(k-1.)*P1r*P1r);
	return I0*Pmr;
}

