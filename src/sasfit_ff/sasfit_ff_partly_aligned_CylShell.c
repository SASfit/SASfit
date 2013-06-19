/*
 * src/sasfit_ff/sasfit_ff_partly_aligned_CylShell.c
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

#define R		param->p[0]
#define DR		param->p[1]
#define L		param->p[2]
#define ETA_C	param->p[3]
#define ETA_SH	param->p[4]
#define ETA_SOL	param->p[5]
#define PSI_DEG	param->p[6]

#define P2		param->p[7]
#define P4		param->p[8]
#define P6		param->p[9]

#define PHI		param->p[MAXPAR-1]
#define Q		param->p[MAXPAR-2]
#define THETA		param->p[MAXPAR-3]

scalar sasfit_ff_partly_aligned_CylShell_THETA(scalar x, sasfit_param * param)
{
	scalar res,p,P_l[7],u,norm;
	int l;
	sasfit_param subparam;

	SASFIT_ASSERT_PTR(param);
	sasfit_copy_param(&subparam,param->p);
	THETA = x;
	subparam.p[6] = PSI_DEG+90.0;
	subparam.p[7] = THETA*180./M_PI;
	subparam.p[8] = PHI*180./M_PI;

// size distribution simulated up to the 6th even order parameter
	P_l[0]=1.0;
	P_l[1]=0.0;
	P_l[2]=P2;
	P_l[3]=0.0;
	P_l[4]=P4;
	P_l[5]=0.0;
	P_l[6]=P6;

	p=0.0;
/*
	for (l=0;l<=6;l++) {
		p = p+(2.*l+1.)/2.*gsl_sf_legendre_Pl(l,cos(THETA))*P_l[l];
	}
*/
// /*

// Maier-Saupe orientation distribution
	if (P2 < 0) {
		u = sqrt(-P2);
		norm = sqrt(M_PI)*gsl_sf_erf(u)/u;
	}
	else if (P2 == 0.0) {
		norm = 2;
	}
	else {
		u = sqrt(P2);
		norm = 2.0*exp(P2)*gsl_sf_dawson(u)/u; // maple: `assuming`([convert(int(exp(P2*cos(THETA)^2)*sin(THETA), theta = 0 .. Pi), dawson)], [kappa > 0])
	}
    p = exp(P2*gsl_pow_2(cos(THETA)))/norm;
// */
	res = sin(THETA)*p*sasfit_ff_alignedCylShell(Q,&subparam);

	return res;
}

scalar sasfit_ff_partly_aligned_CylShell_intPHI(scalar x, sasfit_param * param)
{
	scalar res;
	SASFIT_ASSERT_PTR(param);

	PHI = x;
	res = sasfit_integrate(0,M_PI,&sasfit_ff_partly_aligned_CylShell_THETA,param);
	if (param->errStatus) {
		sasfit_out("Q=%lf, THETA=%lf, PHI=%lf\n",Q,THETA,PHI);
	}
	return res;
}

scalar sasfit_ff_partly_aligned_CylShell(scalar q, sasfit_param * param)
{
	scalar res;

    SASFIT_ASSERT_PTR(param);

	Q=q;

	SASFIT_CHECK_COND1((q < 0.0),      param, "q(%lg) < 0", q);
	SASFIT_CHECK_COND1((R < 0.0),      param, "R(%lg) < 0", R);
	SASFIT_CHECK_COND1((DR < 0.0),     param, "DR(%lg) < 0", DR);
	SASFIT_CHECK_COND1((L < 0.0),      param, "L(%lg) < 0", L);

	res = sasfit_integrate(0.0,2.0*M_PI,&sasfit_ff_partly_aligned_CylShell_intPHI,param);
	return res;
}

