/*
 * src/sasfit_peaks/sasfit_peak_InvertedGammaArea.c
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
#include "include/sasfit_peaks_utils.h"


scalar sasfit_peak_InvertedGammaArea(scalar x, sasfit_param * param)
{
	scalar bckgr, ampl, center, width, shape;
	scalar z,u;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 5, &ampl, &center, &width, &shape, &bckgr);

	SASFIT_CHECK_COND1((width <= 0), param, "width(%lg) <= 0",width);
	SASFIT_CHECK_COND1((shape <= 0), param, "shape(%lg) <= 0",shape);

	u = shape*x+x+width-center*shape-center;
	z = x-center;
	if (u <= 0) return bckgr;

	return bckgr+ampl*(shape+1.)*exp(-width*(shape+1.)/u)*pow(width*(shape+1.)/u,shape)/u/gsl_sf_gamma(shape);
}

