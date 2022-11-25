/*
 * src/sasfit_sq/sasfit_sq_BabaAhmed2.c
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
#include "include/sasfit_sq_utils.h"

/*
float S_BabaAhmed2(Tcl_Interp *interp,
		 float Q,
		float R_eff,		// effective radius
		 float z_p,			// charge per colloid
		 float epsilon_r,	// relative permittivity
		 float mB,			// molarity of ion B	mol/l
		 float zB,			// charge number of the ion B
		 float eta,			// volume fraction of colloids
		 float T,			// sample temperature in Kelvin
		bool  *error)
*/

scalar sasfit_sq_BabaAhmed2(scalar q, sasfit_param * param)
{
	scalar R_eff, z_p, epsilon_r, mB, zB, eta, T;
	scalar gamma,beta,s,n,C0,nC0,nCQ,A,B,Vp, LB;
	const scalar kB = 1.3806504e-23;				//  kg m^2 / K s^2  = J/K 
	const scalar e0 = 1.602176487e-19;			//  C
	const scalar epsilon_0 = 8.854187817e-12;	//  F/m
	const scalar Na = 6.02214179e23;				//  1/mol
	scalar kD,Deff,Im, epsilon, Psi_0_eff;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 7, &R_eff, &z_p, &epsilon_r, &mB, &zB, &eta, &T);

	Deff = 2*R_eff;
	Vp = 4./3. * M_PI*pow(R_eff*1e-9,3.);
	n = eta/(M_PI*pow(Deff*1e-9,3)/6.0);

	epsilon = epsilon_r*epsilon_0;

	LB = e0*e0/(4*M_PI*epsilon*kB*T);

	Im = 0.5 * (2.*mB*1e3*zB*zB+z_p*eta/(Vp*Na));

//	cs=csalt*6.0221415E23*1E3;	//# salt molecules/m^3
//	IonStr = 0.5 * pow(Elcharge,2.)*(zz*VolFrac/Vp+2.*cs)
//	Kappa=sqrt(2.*Beta*IonSt/Perm);	  //Kappa calc from Ionic strength
//	Kappa=2/SIdiam						// Use to compare with HP paper
//	gMSAWave[5]=Beta*charge*charge/(M_PI*Perm*SIdiam*pow(2.+Kappa*SIdiam,2.));

	beta = 1./(kB*T);
	kD = sqrt(2.*Na*pow(e0,2.)*Im*beta/epsilon);
	s = kD*Deff*1e-9;
	Psi_0_eff = pow(z_p*e0,1)/(M_PI*epsilon*Deff*1e-9*(2.+s));
	gamma = beta*M_PI*epsilon_r*Deff*1e-9*Psi_0_eff*Psi_0_eff;

	q = q*Deff;
	A = -24*eta*pow(1.+2.*eta,2)/pow(1.-eta,4);
	B = 36*eta*eta*pow(2.+eta,2)/pow(1.-eta,4);
	C0 = ( A*(sin(q)-q*cos(q))
		+  B*((2./pow(q,2)-1.)*q*cos(q)+2.*sin(q)-2./q)
		- eta*A/2.* (  24./pow(q,3)+4.*(1.-6./pow(q,2))*sin(q)
						 - (1.-12./pow(q,2)+24./pow(q,4))*q*cos(q)
					 )
		)/pow(q,3);

	nC0 = C0;

	nCQ = nC0-24.*eta*gamma*(q*cos(q)+s*sin(q))/(q*(q*q+s*s));
	nCQ = nC0-24.*M_PI*eta*epsilon*Psi_0_eff*Psi_0_eff*Deff*1e-9*(q*cos(q)+s*sin(q))/(q*(q*q+s*s)*kB*T);
	return 1./(1.-nCQ);
}

