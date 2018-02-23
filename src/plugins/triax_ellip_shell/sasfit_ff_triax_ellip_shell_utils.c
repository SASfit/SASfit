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
 *   Ingo Bressler (ingo@cs.tu-berlin.de)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

int triax_ellip_shell_cubature(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
	sasfit_param subParam;
	sasfit_init_param( &subParam );
	sasfit_param *param;
	cubature_param *cparam;
	cparam = (cubature_param *) pam;
	param = cparam->param;
	
	fval[0] = 0;
	if ((ndim < 2) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
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
			sasfit_out("LogNormError: SIGMA:%lf\n",SIGMA);
			return 1;
		}
	}
	X = x[0];
	Y = x[1];
    fval[0] = (cparam->func)(param);
    return 0;
}


scalar Kernel_P_OOURA3(scalar x, void * pam) {
	sasfit_param subParam;
	sasfit_init_param( &subParam );
	sasfit_param *param;
	cubature_param *cparam;
	cparam = (cubature_param *) pam;
	param = cparam->param;
	
	if (SIGMA==0) {
		LNDISTR = 1;
		NU = 1;
	} else {
		subParam.p[0] = 1.0;
		subParam.p[1] = SIGMA;
		subParam.p[2] = 1.0;
		subParam.p[3] = 1.0;
		LNDISTR = sasfit_sd_LogNorm(x, &subParam);
		NU=x;
		SASFIT_CHECK_SUB_ERR(param, subParam);
		if ( subParam.errStatus != FALSE ) {
			sasfit_out("LogNormError: SIGMA:%lf\n",SIGMA);
			return 1;
		}
	}
	return  (cparam->func)(param);
} 
scalar Kernel_P_OOURA2(scalar x, void * pam) {
	sasfit_param *param;
	cubature_param *cparam;
	cparam = (cubature_param *) pam;
	param = cparam->param;
	scalar sum, err, *aw;
	int lenaw;
	
	lenaw=10000;
	Y = x;
	switch(sasfit_get_int_strategy()) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
			aw = (scalar *)malloc((lenaw)*sizeof(scalar));
			sasfit_intdeini(lenaw,GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
			sasfit_intde(&Kernel_P_OOURA3, cparam->cubxmin[2], cparam->cubxmax[2], aw, &sum, &err,pam);
			free(aw);
            break;
            } 
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
			aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
			sasfit_intccini(lenaw, aw);
			sasfit_intcc(&Kernel_P_OOURA3, cparam->cubxmin[2], cparam->cubxmax[2], sasfit_eps_get_nriq(), lenaw, aw, &sum, &err,pam);
			free(aw);
            break;
            }
	}
	return sum;
}

scalar Kernel_P_OOURA1(scalar x, void * pam) {
	sasfit_param *param;
	cubature_param *cparam;
	cparam = (cubature_param *) pam;
	param = cparam->param;
	scalar sum, err, *aw;
	int lenaw;
	
	lenaw=10000;
	X = x;
	switch(sasfit_get_int_strategy()) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
			aw = (scalar *)malloc((lenaw)*sizeof(scalar));
			sasfit_intdeini(lenaw,GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
			sasfit_intde(&Kernel_P_OOURA2, cparam->cubxmin[1], cparam->cubxmax[1], aw, &sum, &err,pam);
			free(aw);
            break;
            } 
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
			aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
			sasfit_intccini(lenaw, aw);
			sasfit_intcc(&Kernel_P_OOURA2, cparam->cubxmin[1], cparam->cubxmax[1], sasfit_eps_get_nriq(), lenaw, aw, &sum, &err,pam);
			free(aw);
            break;
            }
	}
	return sum;
}