/*
 * src/sasfit_sd/sasfit_sd_BiLogNorm.c
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
#include "include/sasfit_sd_utils.h"

/*
float BiLogNorm(Tcl_Interp *interp,
			  float N1,
              float s1,
              float p1,
              float m1,
			  float N2,
              float s2,
              float p2,
              float m2,
              float R,
              bool  *error)
	      */
/**
 * calculates a bi-modal log normal distribution
 */
scalar sasfit_sd_BiLogNorm(scalar x, sasfit_param * param)
{
	scalar LNf;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR( param );

	sasfit_init_param( &subParam );
	subParam.p[0] = param->p[4];	// N2
	subParam.p[1] = param->p[5];	// s2
	subParam.p[2] = param->p[6];	// p2
	subParam.p[3] = param->p[7];	// m2

	LNf = sasfit_sd_LogNorm(x, &subParam);

	SASFIT_CHECK_SUB_ERR(param, subParam);

	LNf = LNf + sasfit_sd_LogNorm(x, param);

	return LNf;
}

