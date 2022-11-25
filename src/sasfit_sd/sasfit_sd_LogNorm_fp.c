/*
 * src/sasfit_sd/sasfit_sd_LogNorm_fp.c
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
float LogNorm_fp(Tcl_Interp *interp,
			  float fp,
              float s,
              float p,
              float m,
              float q,
              bool  *error)
	      */
/**
 * Calculates a log normal distribution.
 */
scalar sasfit_sd_LogNorm_fp(scalar q, sasfit_param * param)
{
	scalar fp, s, p, m, c, N, LNf;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 4, &fp, &s, &p, &m);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0", q);
	SASFIT_CHECK_COND1((m <= 0.0), param, "m(%lg) <= 0", m);

	N=3./4.*fp/M_PI * pow(m,-3.)/exp(-3./2.*s*s*(-5.+2.*p))/1E21;

	if (q == 0.0) return 0.0;

	c = 1.0/sqrt(M_PI*2.0)/s/pow(m,1.0-p) / exp(0.5*pow(s*(1.0-p),2.0));
	LNf = N*c* pow(q,-p) * exp(-0.5*pow(log(q/m)/s,2.0));

	return LNf;
}

