/*
 * src/sasfit_sq/sasfit_sq_BabaAhmed.c
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

#include <gsl/gsl_math.h>
#include "include/sasfit_sq_utils.h"

/*
float S_BabaAhmed(Tcl_Interp *interp,
		 float Q,
		float Deff,
		float Psi_0_eff,
		 float epsilon,
		 float eta,
		 float kD,
		 float T,
		bool  *error)
*/

scalar sasfit_sq_BabaAhmed(scalar q, sasfit_param * param)
{
	scalar gamma,beta,s,n,C0,nC0,nCQ,A,B;
	const scalar kB = 1.3806503e-23;  /*  kg m^2 / K s^2 */
	scalar Deff, Psi_0_eff, epsilon, eta, kD, T;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 6, &Deff, &Psi_0_eff, &epsilon, &eta, &kD, &T);

	beta = 1./(kB*T);
	s = kD*1e9*Deff*1e-9;
//	gamma = beta*M_PI*epsilon*Deff*Psi_0_eff*Psi_0_eff*exp(s);
	gamma = beta*M_PI*epsilon*Deff*1e-9*Psi_0_eff*Psi_0_eff;

	n = eta/(M_PI*pow(Deff*1e-9,3)/6.0);
	n=1;

	q = q*Deff;
	A = -24.*eta*pow(1.+2.*eta,2.)/pow(1.-eta,4.);
	B = 36.*eta*eta*pow(2.+eta,2.)/pow(1.-eta,4.);
	C0 = ( A*(sin(q)-q*cos(q))
		+  B*((2./pow(q,2)-1.)*q*cos(q)+2.*sin(q)-2./q)
		- eta*A/2.*(24./pow(q,3)+4.*(1.-6./pow(q,2.))*sin(q)-(1.-12./pow(q,2.)+24./pow(q,4.))*q*cos(q))
		)/pow(q,3.);
//	nC0 = -24.0*eta*G(eta,Deff*Q)/(Deff*Q); // ATTENTION: Q doesnt exist anymore
	nC0 = n*C0;
//	nCQ = nC0-24.*eta*gamma*exp(-s)*(q*cos(q)+s*sin(q))/(q*(q*q+s*s));
	nCQ = nC0-24.*eta*gamma		  *(q*cos(q)+s*sin(q))/(q*(q*q+s*s));
	return 1./(1.-nCQ);
}
