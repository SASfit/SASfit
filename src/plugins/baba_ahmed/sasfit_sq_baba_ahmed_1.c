/*
 * src/plugins/baba_ahmed/sasfit_sq_baba_ahmed_1.c
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
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define D_EFF	param->p[0]
#define PSI0_EFF	param->p[1]
#define EPSILON	param->p[2]
#define ETA	param->p[3]
#define KD	param->p[4]
#define T	param->p[5]

scalar sasfit_sq_baba_ahmed_1(scalar q, sasfit_param * param)
{
	scalar gamm, beta, s, n, c0, nc0, ncq, a, b;
	const scalar kb = 1.3806503e-23;  /*  kg m^2 / K s^2 */

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
//	SASFIT_CHECK_COND1((D_EFF < 0.0), param, "D_eff(%lg) < 0",D_EFF); // modify condition to your needs
//	SASFIT_CHECK_COND1((PSI0_EFF < 0.0), param, "psi0_eff(%lg) < 0",PSI0_EFF); // modify condition to your needs
//	SASFIT_CHECK_COND1((EPSILON < 0.0), param, "epsilon(%lg) < 0",EPSILON); // modify condition to your needs
//	SASFIT_CHECK_COND1((ETA < 0.0), param, "eta(%lg) < 0",ETA); // modify condition to your needs
//	SASFIT_CHECK_COND1((KD < 0.0), param, "kD(%lg) < 0",KD); // modify condition to your needs
//	SASFIT_CHECK_COND1((T < 0.0), param, "T(%lg) < 0",T); // modify condition to your needs

	beta = 1./(kb*T);
	s = KD*1e9*D_EFF*1e-9;
//	gamm = beta*M_PI*EPSILON*D_EFF*PSI0_EFF*PSI0_EFF*exp(s);
	gamm = beta*M_PI*EPSILON*D_EFF*1e-9*PSI0_EFF*PSI0_EFF;

	n = ETA/(M_PI*pow(D_EFF*1e-9, 3)/6.0);
	n = 1.0;

	q = q*D_EFF;
	a = -24.*ETA*pow(1.+2.*ETA, 2.)/pow(1.-ETA, 4.);
	b = 36.*ETA*ETA*pow(2.+ETA, 2.)/pow(1.-ETA, 4.);
	c0 = ( a*(sin(q) - q*cos(q))
		+  b*((2./pow(q,2)-1.)*q*cos(q)+2.*sin(q)-2./q)
		- ETA*a/2.*(24./pow(q,3)+4.*(1.-6./pow(q,2.))*sin(q)-(1.-12./pow(q,2.)+24./pow(q,4.))*q*cos(q))
		)/pow(q,3.);
//	nc0 = -24.0*ETA*G(ETA,D_EFF*q)/(D_EFF*q);
	nc0 = n*c0;
//	ncq = nc0-24.*ETA*gamm*exp(-s)*(q*cos(q)+s*sin(q))/(q*(q*q+s*s));
	ncq = nc0-24.*ETA*gamm		  *(q*cos(q)+s*sin(q))/(q*(q*q+s*s));
	return 1./(1.-ncq);
}

scalar sasfit_sq_baba_ahmed_1_f(scalar q, sasfit_param * param)
{
	// insert your code here
	return 0.0;
}

scalar sasfit_sq_baba_ahmed_1_v(scalar q, sasfit_param * param, int dist)
{
	// insert your code here
	return 0.0;
}

