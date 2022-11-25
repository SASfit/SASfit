/*
 * src/sasfit_ff/sasfit_ff_ThinDisc_homogenousXS.c
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

scalar sasfit_ThinDisc(scalar q, sasfit_param * param)
{
	scalar R, Pprime, D;

	SASFIT_ASSERT_PTR(param);

	D = param->p[1];
	R = 0.5*D;

	if ((q == 0.0) && (D != 0)) 
	{
		Pprime=(M_PI*R*R)*(M_PI*R*R);
	} 
	else if (D == 0.0) 
	{
		Pprime = 0.0;
	} else 
	{
		Pprime = (M_PI*R*R)*(M_PI*R*R)*2.0/(q*q*R*R)*(1.0-gsl_sf_bessel_J1(D*q)/(q*R));
	}

	return Pprime;
}


scalar sasfit_ff_ThinDisc_homogenousXS(scalar q, sasfit_param * param)
{
	scalar u, Pprime, t, eta;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 3, &t, EMPTY, &eta);

	// ThinDisc(interp,q,D,error);
	Pprime = sasfit_ThinDisc(q, param);

	u = q*t*0.5;

	if (u  == 0.0) 
	{
		if (t == 0.0)
		{
			return 0.0;
		} else 
		{
			return pow(t*eta,2.0)*Pprime;
		}
	}

	return pow(t*eta*sin(u)/u,2.0)*Pprime;
}

