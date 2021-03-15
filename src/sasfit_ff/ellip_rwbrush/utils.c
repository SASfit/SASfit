/*
 * src/sasfit_ff/ellip_rwbrush/utils.c
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

#include "include/private.h"

scalar f1(scalar q, scalar r)
{
	scalar u = q*r;
	if (SASFIT_EQUAL(u, 0.0)) return 1.0;
	return 3*(sin(u)-u*cos(u))/(u*u*u);
}

scalar r_ell(scalar r, scalar epsilon, scalar alpha)
{
	return r*gsl_hypot(sin(alpha),epsilon*cos(alpha));
}

scalar sasfit_ff_ellip_rwbrush_ssc_core(scalar alpha, sasfit_param *param)
{
    scalar w, u;

    SASFIT_ASSERT_PTR(param);

    w       = sasfit_rwbrush_w(V[Q], V[RG]);
	u 	= V[Q] * (r_ell(V[R],V[EPSILON],alpha)+V[D]*V[RG]);

	return w*f1(V[Q],r_ell(V[R],V[EPSILON],alpha))*gsl_sf_bessel_j0(u)*sin(alpha);
}

scalar sasfit_ff_ellip_rwbrush_scc_core(scalar alpha, sasfit_param *param)
{
    scalar w, u;

    SASFIT_ASSERT_PTR(param);

    w       = sasfit_rwbrush_w(V[Q], V[RG]);
	u 	= V[Q] * (r_ell(V[R],V[EPSILON],alpha)+V[D]*V[RG]);
	return gsl_pow_2(w*gsl_sf_bessel_j0(u)) * sin(alpha);
}

scalar sasfit_ff_ellip_rwbrush_fs_core(scalar alpha, sasfit_param *param)
{
    SASFIT_ASSERT_PTR(param);

	return gsl_pow_2(f1(V[Q],r_ell(V[R],V[EPSILON],alpha)))*sin(alpha);
}

