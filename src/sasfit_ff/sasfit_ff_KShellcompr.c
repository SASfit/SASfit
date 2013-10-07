/*
 * src/sasfit_ff/sasfit_ff_KShellcompr.c
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

scalar sasfit_ff_KShellcompr(scalar q, sasfit_param * param)
{
	scalar lRtot, lDR, leta_core, leta_sh, lx_sol, leta_sol, lcore, lshell, lsolvent;
	scalar Rtot, DR, eta_core, eta_shell, x_sol, eta_solvent, kappa_core, kappa_shell, kappa_solvent, P;
	sasfit_param subParam;
	scalar res;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 10, &Rtot, &DR, &eta_core, &eta_shell, &x_sol, &eta_solvent, &kappa_core, &kappa_shell, &kappa_solvent, &P);

	lcore	 = pow(1./(1.-P*kappa_core),1./3.);
	lshell	= pow(1./(1.-P*kappa_shell),1./3.);
	lsolvent = pow(1./(1.-P*kappa_solvent),1./3.);

	leta_core = eta_core/(1.-P*kappa_core);

	lx_sol  = 1.-(1.-x_sol)/pow(lshell,2.);
	leta_sh = eta_shell/(1.-P*kappa_shell);
	lDR = DR/lshell;
	lRtot = (Rtot-DR)/lcore+lDR;
	leta_sol = eta_solvent/(1.-P*kappa_solvent);

//	leta_sol = eta_solvent*(1.0018 +P*7.4638E-5-P*P*1.0969E-8);
	leta_sol = eta_solvent*(0.873 +P*5.65E-5-P*P*5.272E-9)/0.873;

	sasfit_init_param( &subParam );
	subParam.p[0] = lRtot;
	subParam.p[1] = lDR;
	subParam.p[2] = leta_core;
	subParam.p[3] = leta_sh;
	subParam.p[4] = lx_sol;
	subParam.p[5] = lx_sol;
	subParam.p[6] = leta_sol;
	subParam.kernelSelector = KSHLIN2;

	res = sasfit_ff_Kshlin(q, &subParam);

	SASFIT_CHECK_SUB_ERR(param, subParam);

	return res;
}
