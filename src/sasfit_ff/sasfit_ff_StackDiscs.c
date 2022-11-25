/*
 * src/sasfit_ff/sasfit_ff_StackDiscs.c
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


scalar sasfit_ff_StackDiscs_core(scalar theta, sasfit_param * param)
{
	scalar SQ, tn, V_t, V_c, f_t, f_c;
	scalar bessarg, sinarg1, sinarg2, bess, tmp;
	scalar Q, R, d, h, n, eta_c, eta_l, sigma_D, D;
	int k;

        SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 8, &R, &d, &h, &n, &eta_c, &eta_l, &sigma_D, &D);
	theta 	= theta;
	Q	= param->p[MAXPAR-1];

	tn = (int)n;
	V_t = M_PI*R*R*(d+2*h);
	V_c = M_PI*R*R*d;

	bessarg = Q*R*sin(theta);
	sinarg1 = Q*(d/2.0+h)*cos(theta);
	sinarg2 = Q*d/2.0*cos(theta);

	SQ = 1.0;
	tmp = Q*cos(theta);
	for (k=1; k < tn ;k++)
	{
		SQ = SQ+2.0/tn * (tn-k)*cos(k*D*tmp) * exp(-k*gsl_pow_2(tmp*sigma_D)/2.0);
	}
	bess=2.0*sasfit_jinc(bessarg);

	f_t = gsl_sf_bessel_j0(sinarg1)*bess;
	f_c = gsl_sf_bessel_j0(sinarg2)*bess;

	return    (eta_l * (V_t*f_t - V_c*f_c) + eta_c * V_c*f_c)
		* (eta_l * (V_t*f_t - V_c*f_c) + eta_c * V_c*f_c)
		* SQ*sin(theta);
}


/*
 * form factor of a stacked discs (tactoids)
 */
// problem ??
// #define EPS_STACKDISCS 1.0e-6
// #define JMAX 30
// #define JMAXP JMAX+1
scalar sasfit_ff_StackDiscs(scalar q, sasfit_param * param)
{
	scalar Sum;
	scalar R, d, h, n, sigma_D, D;

        SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 8, &R, &d, &h, &n, EMPTY, EMPTY, &sigma_D, &D);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R);
	SASFIT_CHECK_COND1((d < 0.0), param, "d(%lg) < 0",d);
	SASFIT_CHECK_COND1((h < 0.0), param, "h(%lg) < 0",h);
	SASFIT_CHECK_COND1((n < 1.0), param, "n(%lg) < 1",n);
	SASFIT_CHECK_COND1((sigma_D < 0.0), param, "sigma_D(%lg) < 0",sigma_D);
	SASFIT_CHECK_COND1((D < 0.0), param, "D(%lg) < 0",D);

	Sum = 0.0;
	// Sum = SASFITqrombStackDiscs(interp,0.0,PI/2.0,q,R,d,h,n,eta_c,eta_l,sigma_D,D,error);
	param->p[MAXPAR-1] = q;
	Sum = sasfit_integrate(0.0, M_PI/2.0, sasfit_ff_StackDiscs_core, param);

	return Sum;
}

