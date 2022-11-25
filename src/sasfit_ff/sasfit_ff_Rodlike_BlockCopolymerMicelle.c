/*
 * src/sasfit_ff/sasfit_ff_Rodlike_BlockCopolymerMicelle.c
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
#include "include/sasfit_ff_utils.h"


scalar sasfit_ff_Rodlike_BlockCopolymerMicelle(scalar q, sasfit_param * param)
{
	scalar R, r_core, r_chain, Nagg,S, res;
	scalar Vpolym_c, Vpolym_sh, eta_poly_c, eta_poly_sh, eta_solv, xsolv_c, Rg, d, n_agg, H;
	sasfit_param subParam;

        SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 10, &Vpolym_c, &Vpolym_sh, &eta_poly_c, &eta_poly_sh, &eta_solv, &xsolv_c, &Rg, &d, &n_agg, &H);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((H < 0.0), param, "H(%lg) < 0",H);
	SASFIT_CHECK_COND1((Rg < 0.0), param, "Rg(%lg) < 0",Rg);
	SASFIT_CHECK_COND1((xsolv_c < 0.0), param, "xsolv_c(%lg) < 0",xsolv_c);
	SASFIT_CHECK_COND1((xsolv_c >= 1.0), param, "xsolv_c(%lg) >= 1",xsolv_c);

        if (H==0) return 0;

        R  = 2.*n_agg*Vpolym_c/(1.-xsolv_c);
        S = 2.*M_PI*R*H;
        Nagg = n_agg*S;

        r_core  = Nagg*Vpolym_c*(eta_poly_c-eta_solv);
        r_chain = Vpolym_sh*(eta_poly_sh-eta_solv);

	sasfit_init_param( &subParam );
	subParam.p[0] = R;
	subParam.p[1] = Rg;
	subParam.p[2] = d;
	subParam.p[3] = Nagg;
	subParam.p[4] = H;
	subParam.p[5] = r_core;
	subParam.p[6] = r_chain;

	res = sasfit_ff_very_long_cyl_w_gauss_chains(q, &subParam);

	SASFIT_CHECK_SUB_ERR(param, subParam);

	return res;
}
