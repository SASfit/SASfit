/*
 * src/sasfit_ff/sasfit_ff_aux_gamma(r).c
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
#include "include/sasfit_ff_utils.h"

#define R param->p[0]
/**
 * 
 * 4 pi r^2 sin(q*r)/(q*r)
 * 
 */
scalar sasfit_ff_aux_gamma_r(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param);

	if (q*R == 0.0) return 4.0*M_PI*R*R; 

	return 4.0*M_PI*R*R*sin(q*R)/(q*R);
}

