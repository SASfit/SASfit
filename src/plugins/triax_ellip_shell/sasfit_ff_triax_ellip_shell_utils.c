/*
 * src/plugins/triax_ellip_shell/sasfit_ff_triax_ellip_shell.c
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
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

scalar triax_ellip_shell_cubature(const scalar *x, size_t ndim, void *pam) {
	sasfit_param subParam;
	sasfit_init_param( &subParam );
	sasfit_param *param;
	cubature_param *cparam;
	cparam = (cubature_param *) pam;
	param = cparam->param;

	if ((ndim < 2) || (fdim < 1)) {
		sasfit_err("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return -1;
	}
	if ((ndim < 3) || (SIGMA==0)) {
		LNDISTR = 1;
		NU = 1;
	} else {
		subParam.p[0] = 1.0;
		subParam.p[1] = SIGMA;
		subParam.p[2] = 1.0;
		subParam.p[3] = 1.0;
		NU=x[2];
		LNDISTR = sasfit_sd_LogNorm(NU, &subParam);
		SASFIT_CHECK_SUB_ERR(param, subParam);
		if ( subParam.errStatus != FALSE ) {
			sasfit_err("LogNormError: SIGMA:%lf\n",SIGMA);
			return -1;
		}
	}
	X = x[0];
	Y = x[1];
    return (cparam->func)(param);
}
