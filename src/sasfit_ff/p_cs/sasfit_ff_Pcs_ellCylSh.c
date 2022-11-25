/*
 * src/sasfit_ff/sasfit_ff_Pcs_ellCylSh.c
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
#include "../include/sasfit_ff_utils.h"

#define R		param->p[0]
#define EPSILON	param->p[1]
#define T		param->p[2]
#define DUMMY1	param->p[3]
#define DUMMY2	param->p[4]
#define DUMMY3	param->p[5]
#define ETA_C	param->p[6]
#define ETA_SH	param->p[7]
#define ETA_SOL	param->p[8]

#define Q		param->p[MAXPAR-1]


scalar ell_r(scalar radius, scalar epsilo, scalar t, scalar theta)
{
	return sqrt(gsl_pow_2((radius+t)*sin(theta))+gsl_pow_2((radius*epsilo+t)*cos(theta)));
}

scalar sasfit_ff_Pcs_ellCylSh_core(scalar x, sasfit_param *param)
{
	scalar Ain, Aout,A,u1,u2,b1,b2;
	u1 = Q*ell_r(R,EPSILON,0,x);
	u2 = Q*ell_r(R,EPSILON,T,x);
	b1 = M_PI*R*R*EPSILON*(ETA_C-ETA_SH);
	b2 = M_PI*(R+T)*(R*EPSILON+T)*(ETA_SH-ETA_SOL);

	if (u1==0) {
		Ain = 1.0;
	} else {
		Ain = 2.0*gsl_sf_bessel_J1(u1)/u1;
	}
	
	if (u2==0) {
		Aout = 1.0;
	} else {
		Aout = 2.0*gsl_sf_bessel_J1(u2)/u2;
	}

	A = b1*Ain+b2*Aout;
	return A*A;
}


scalar sasfit_ff_Pcs_ellCylSh(scalar q, sasfit_param * param)
{
	scalar res;

    SASFIT_ASSERT_PTR(param);

	SASFIT_CHECK_COND1((q < 0.0),      param, "q(%lg) < 0", q);
	SASFIT_CHECK_COND1((R < 0.0),      param, "R(%lg) < 0", R);
	SASFIT_CHECK_COND1((T < 0.0),      param, "t(%lg) < 0", T);
	SASFIT_CHECK_COND1((EPSILON <= 0.0),param, "epsilon(%lg) <= 0", EPSILON);
	
	Q = q;
	if (EPSILON != 1) {
		res = 2.0/M_PI*sasfit_integrate(0.0,M_PI/2.0,&sasfit_ff_Pcs_ellCylSh_core,param);
	} else {
		res = sasfit_ff_Pcs_ellCylSh_core(M_PI/2.0,param);
	}
	return res;
}

