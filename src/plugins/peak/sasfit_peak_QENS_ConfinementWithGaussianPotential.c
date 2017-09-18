/*
 * src/sasfit_peaks/sasfit_peak_QENS_ConfinementWithGaussianPotential.c
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
#include <gsl/gsl_const.h>
#include <string.h>
#include <stdio.h>
#include "include/sasfit_peaks_utils.h"

#define AMPL param->p[0]
#define Q	 param->p[1]
#define E0	 param->p[2]
#define DIFF param->p[3]
#define U2	 param->p[4]
#define SIGMA0 param->p[5]
#define SIGMA1 param->p[6]
#define SIGMA2 param->p[7]
#define BACKGR param->p[8]
#define N	 param->p[9]

scalar sasfit_peak_QENS_ConfinementWithGaussianPotential(scalar energy, sasfit_param * param)
{
	scalar gamma,v,q,sigma,hbar,de,q2,a0,an, sqe;
	int i;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR( param );

	sigma = SIGMA0+Q*SIGMA1+Q*Q*SIGMA2;

	SASFIT_CHECK_COND1((sigma < 0), param, "sigma(%lg) < 0",sigma);
	SASFIT_CHECK_COND1((U2 < 0),    param, "<u^2>(%lg) < 0",U2);

	q = Q/1.e-10; // convertion from Angstrom in meter 
	q2 = q*q;
	a0 = exp(-q2*U2);
	de = energy-E0;

	if (sigma == 0.0) {
		if (de == 0.0) {
			sqe = a0;
		} else {
			sqe = 0.0;
		}
	} else {
		if (de ==0.0) {
			sqe = a0/(sqrt(2.0*M_PI)*sigma);
		} else {
			sqe = a0/(sqrt(2.0*M_PI)*sigma)*exp(-de*de/(2.0*sigma*sigma));
		}
	}

	sqe = AMPL*sqe;

//  hbar in meV
	hbar = GSL_CONST_MKSA_PLANCKS_CONSTANT_HBAR / GSL_CONST_MKSA_ELECTRON_VOLT * 1e3;

	sasfit_init_param( &subParam );

	for (i=1; i<=N; i++) {
		if (U2 == 0.0) break;
		gamma = i*hbar*DIFF/U2;
		if (q2*U2 == 0.0) {
			an=0.0;
		} else {
//			an = a0* exp(i*log(q2*u2)-gsl_sf_lngamma(i+1));
			an = a0 *pow(q2*U2,i)/gsl_sf_fact(i);
		}
		
		if (sigma == 0.0) {
			if ((gamma == 0.0) && (de == 0.0)) {
				v = 1/M_PI;
			} else {
				v = gamma/(gamma*gamma+de*de)/M_PI;
			}
		} else {
			subParam.p[0] = 1.0;
			subParam.p[1] = 0.0;
			subParam.p[2] = sigma;
			subParam.p[3] = gamma;
			subParam.p[4] = 0.0;
			v = sasfit_peak_VoigtPeakArea(de,&subParam);
			if (subParam.errStatus) {
				param->errStatus = subParam.errStatus;
				strcpy(param->errStr,subParam.errStr);
				return sqe;
			}
			if (v<=0) {
				fprintf(stderr,"energy=%lg v=%lg\ngamma=%lg sigma=%lg energy=%lg\n", energy, v, gamma, sigma, E0);
			}
		}
		sqe = sqe+AMPL*an*v;
	}
	return sqe+BACKGR;
}

