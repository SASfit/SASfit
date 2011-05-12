/*
 * src/sasfit_ff/sasfit_sq_Pprime_rod.c
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
 */

#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>
#include "../include/sasfit_sq_utils.h"

#define L		param->p[0]


scalar sasfit_sq_Pprime_rod(scalar q, sasfit_param *param)
{
	scalar u,u2;
	u = q*L;
	u2 = u/2.0;
	if (u==0.0) return L*L;
	return L*L*(2.0*gsl_sf_Si(u)/u-gsl_pow_2(sin(u2)/u2) );
}


