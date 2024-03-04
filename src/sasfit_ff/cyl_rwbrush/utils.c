/*
 * src/sasfit_ff/cyl_rwbrush/utils.c
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
 *   Ingo Bressler (ingo.bressler@bam.de)
 */

#include "include/private.h"

scalar F_PSI(scalar Q, scalar R, scalar H, scalar alpha)
{
        scalar xR, xH, PSIres;

        xR = Q*R*sin(alpha);
        xH = Q*H*cos(alpha)/2.0;

        PSIres = 2.0*sasfit_jinc(xR)*gsl_sf_bessel_j0(xH);


        return PSIres;
}

scalar XI(scalar Q, scalar R, scalar H, scalar alpha)
{
        scalar xR, xH, XIres;

        xR = Q*R*sin(alpha);
        xH = Q*H*cos(alpha)/2.0;

        if ((R+H) == 0.0) return 0.0;

        XIres = R/(R+H) * 2.0*sasfit_jinc(xR) * cos(xH);

        XIres = XIres + H/(R+H) * gsl_sf_bessel_J0(xR)*gsl_sf_bessel_j0(xH);

        return XIres;
}

scalar sasfit_ff_P18(scalar Q, scalar Rg)
{
        scalar u;
        u = Q*Q * Rg*Rg;
        if ( u == 0 ) return 1.0;

        return 2*(exp(-u)+u-1.0) / (u*u);
}

scalar sasfit_ff_cyl_rwbrush_fs_core(scalar alpha, sasfit_param *param)
{
	SASFIT_ASSERT_PTR(param);

	return pow( F_PSI(V[Q],V[R],V[H],alpha), 2.0) * sin(alpha);
}

scalar sasfit_ff_cyl_rwbrush_scc_core(scalar alpha, sasfit_param *param)
{
	scalar w;

	SASFIT_ASSERT_PTR(param);

	w	= sasfit_rwbrush_w(V[Q], V[RG]);

	return pow(w*XI(V[Q], V[R]+V[D]*V[RG], V[H]+2.0*V[D]*V[RG], alpha), 2.0) * sin(alpha);
}

scalar sasfit_ff_cyl_rwbrush_ssc_core(scalar alpha, sasfit_param *param)
{
	scalar w;

	SASFIT_ASSERT_PTR(param);

	w	= sasfit_rwbrush_w(V[Q], V[RG]);

	return w * F_PSI(V[Q], V[R], V[H], alpha) * XI(V[Q], V[R]+V[D]*V[RG], V[H]+2.0*V[D]*V[RG], alpha) * sin(alpha);
}

