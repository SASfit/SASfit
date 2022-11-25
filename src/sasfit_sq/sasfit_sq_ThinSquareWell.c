/*
 * src/sasfit_sq/sasfit_sq_ThinSquareWell.c
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
#include "include/sasfit_sq_utils.h"


#define RHS    param->p[0]
#define PHI    param->p[1]
#define E_KT   param->p[2]
#define EPSILON param->p[3]

scalar f1_TSW(scalar x) {
	return (1.0-cos(x))/gsl_pow_2(x);
}

scalar f2_TSW(scalar x) {
	return (sin(x)-x*cos(x))/gsl_pow_3(x);
}
scalar f3_TSW(scalar x) {
	return (2.*x*sin(x)-(x*x-2.0)*cos(x)-2.0)/gsl_pow_4(x);
}
scalar f5_TSW(scalar x) {
	return ((4.*x*x*x-24.*x)*sin(x) -
		    (x*x*x*x-12.*x*x+24.)*cos(x) +
			24.
			)/gsl_pow_6(x);
}

scalar sasfit_sq_ThinSquareWell(scalar q, sasfit_param * param)
{
	scalar sigma, kappa,  alpha, beta, mu, Gamma, Delta, tau,
		lambda, Ck;

	SASFIT_ASSERT_PTR( param );

	sigma = 2.0*RHS/(1.0-EPSILON);
	kappa = q*sigma;

	Gamma = PHI*(1.0+PHI/2.0)/(3.0*gsl_pow_2(1.0-PHI));
	tau = exp(-E_KT)/(12.0*EPSILON);
	Delta = tau + PHI/(1.0-PHI);
	lambda = 6.0*(Delta-sqrt(Delta*Delta-Gamma))/PHI;
	mu = lambda*PHI*(1.0-PHI);

	alpha = gsl_pow_2(1.0+2.0*PHI-mu)/gsl_pow_4(1.0-PHI);
	beta  = -(3.0*PHI*gsl_pow_2(2.0+PHI)
		    -2.0*mu*(1.0+7.0*PHI+PHI*PHI)
			+mu*mu*(2.0+PHI))
		/(2.0*gsl_pow_4(1.0-PHI));
	
	Ck = 24.0*PHI*(  alpha*f2_TSW(kappa)
		           + beta*f3_TSW(kappa)
				   + 0.5*PHI*alpha*f5_TSW(kappa))
		+gsl_pow_2(2.0*PHI*lambda*EPSILON)*(    f2_TSW(EPSILON*kappa)
		                                   -0.5*f3_TSW(EPSILON*kappa))
		+2.0*gsl_pow_2(PHI*lambda)*(f1_TSW(kappa)-
		                            gsl_pow_2(EPSILON)*f1_TSW(EPSILON*kappa))
		-2.0*PHI*lambda/EPSILON*
			( f1_TSW(kappa)
			 -gsl_pow_2(1.0-EPSILON)*f1_TSW((1.0-EPSILON)*kappa)
			)
		-24.0*PHI*
			( f2_TSW(kappa)
			 -gsl_pow_3(1.0-EPSILON)*f2_TSW((1.0-EPSILON)*kappa)
			);
	return 1.0/(1.0+Ck);
}
