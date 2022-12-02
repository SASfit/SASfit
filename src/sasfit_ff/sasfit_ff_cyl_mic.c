/*
 * src/sasfit_ff/sasfit_ff_cyl_mic.c
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

#include <gsl/gsl_math.h>
#include "include/sasfit_ff_utils.h"


scalar sasfit_ff_cyl_mic(scalar q, sasfit_param * param)
{
	scalar R, r_core,r_chain, Pcyl;
	scalar Vpolym_c, Vpolym_sh, eta_poly_c, eta_poly_sh, eta_solv, xsolv_c, Rg, d, Nagg, H;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 10, &Vpolym_c, &Vpolym_sh, &eta_poly_c, &eta_poly_sh, &eta_solv, &xsolv_c, &Rg, &d, &Nagg, &H);

	SASFIT_CHECK_COND((H < 0.0), param, "H == 0");

	R  = sqrt(fabs(Nagg*(Vpolym_c/(1.-xsolv_c))/M_PI/H));
	r_core  = Nagg*Vpolym_c*(eta_poly_c-eta_solv);
	r_chain = Vpolym_sh*(eta_poly_sh-eta_solv);

        Pcyl =        pow(r_core,2.0) * sasfit_integrate(0.0, M_PI_2, sasfit_ff_cyl_rwbrush_fs_core, param);
        Pcyl = Pcyl + Nagg*r_chain*r_chain * sasfit_ff_P18(q,Rg);
        Pcyl = Pcyl + Nagg*(Nagg-1.)*r_chain*r_chain * sasfit_integrate(0.0, M_PI_2, sasfit_ff_cyl_rwbrush_scc_core, param)*((Nagg < 1) ?  0 : 1);
        Pcyl = Pcyl + 2.*Nagg*r_core*r_chain * sasfit_integrate(0.0, M_PI_2, sasfit_ff_cyl_rwbrush_ssc_core, param);

	return Pcyl;
}
