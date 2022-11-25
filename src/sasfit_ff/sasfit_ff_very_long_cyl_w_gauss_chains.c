/*
 * src/sasfit_ff/sasfit_ff_very_long_cyl_w_gauss_chains.c
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


scalar sasfit_ff_very_long_cyl_w_gauss_chains(scalar q, sasfit_param * param)
{
	scalar Pp, Pcs, Fc, w, Scc, Ssc, J1x_x;
	scalar R, Rg, d, Nagg, H, r_core, r_chains;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 7, &R, &Rg, &d, &Nagg, &H, &r_core, &r_chains);

	SASFIT_CHECK_COND1((H <= 0.0), param, "H(%lg) < 0", H);

	if (q == 0.0) return Pp = 1.0;

	Pp =(2.0*gsl_sf_Si(q*H)/(q*H)-pow(sin(q*H*0.5)/(q*H*0.5),2.0));

	w = sasfit_rwbrush_w(q, Rg);
	Fc = sasfit_gauss_fc(q, Rg);

	Scc = w*w*pow(gsl_sf_bessel_J0(q*(R+d*Rg)),2.);
	if (q*R == 0)
	{
		J1x_x = 0.5;
	} else
	{
		J1x_x = gsl_sf_bessel_J1(q*R)/(q*R);
	}
	Ssc = w*2.*J1x_x*gsl_sf_bessel_J0(q*(R+d*Rg));

	Pcs = pow(r_core*2.*J1x_x,2.)
		+ Nagg*(Nagg-1.)*pow(r_chains,2.0)*Scc*((Nagg < 1) ?  0 : 1)
		+ 2.*Nagg*r_chains*r_core*Ssc;
//  sasfit_out("Cyl_Fc:%lf \n",pow(r_chains,2.0)*Nagg*Fc);
	return Pp*Pcs
		+ pow(r_chains,2.0)*Nagg*Fc;
}
