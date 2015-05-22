/*
 * src/plugins/baba_ahmed/sasfit_sq_baba_ahmed_2.c
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
#define R_EFF	param->p[0]
#define Z_P	param->p[1]
#define EPSILON_R	param->p[2]
#define MB	param->p[3]
#define ZB	param->p[4]
#define ETA	param->p[5]
#define T	param->p[6]

scalar sasfit_sq_baba_ahmed_2(scalar q, sasfit_param * param)
{
	scalar gamm, beta, s, n, c0, nc0, ncq, a, b, vp, lb;
	scalar kd, d_eff, im, epsilon, psi_0_eff;
	const scalar kb = 1.3806504e-23;				//  kg m^2 / K s^2  = J/K 
	const scalar e0 = 1.602176487e-19;			//  C
	const scalar epsilon_0 = 8.854187817e-12;	//  F/m
	const scalar na = 6.02214179e23;				//  1/mol

	SASFIT_ASSERT_PTR( param );

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
//	SASFIT_CHECK_COND1((R_EFF < 0.0), param, "r_eff(%lg) < 0",R_EFF); // modify condition to your needs
//	SASFIT_CHECK_COND1((Z_P < 0.0), param, "z_p(%lg) < 0",Z_P); // modify condition to your needs
//	SASFIT_CHECK_COND1((EPSILON_R < 0.0), param, "epsilon_r(%lg) < 0",EPSILON_R); // modify condition to your needs
//	SASFIT_CHECK_COND1((MB < 0.0), param, "mB(%lg) < 0",MB); // modify condition to your needs
//	SASFIT_CHECK_COND1((ZB < 0.0), param, "zB(%lg) < 0",ZB); // modify condition to your needs
//	SASFIT_CHECK_COND1((ETA < 0.0), param, "eta(%lg) < 0",ETA); // modify condition to your needs
//	SASFIT_CHECK_COND1((T < 0.0), param, "T(%lg) < 0",T); // modify condition to your needs


	d_eff = 2.0*R_EFF;
	vp = 4./3. * M_PI*pow(R_EFF*1e-9, 3.);
	n = ETA/(M_PI*pow(d_eff*1e-9,3)/6.0);

	epsilon = EPSILON_R*epsilon_0;

	lb = e0*e0/(4*M_PI*epsilon*kb*T);

	im = 0.5 * (2.*MB*1e3*ZB*ZB+Z_P*ETA/(vp*na));

//	cs=csalt*6.0221415E23*1E3;	//# salt molecules/m^3
//	IonStr = 0.5 * pow(Elcharge,2.)*(zz*VolFrac/Vp+2.*cs)
//	Kappa=sqrt(2.*Beta*IonSt/Perm);	  //Kappa calc from Ionic strength
//	Kappa=2/SIdiam						// Use to compare with HP paper
//	gMSAWave[5]=Beta*charge*charge/(M_PI*Perm*SIdiam*pow(2.+Kappa*SIdiam,2.));

	beta = 1./(kb*T);
	kd = sqrt(2.*na*pow(e0,2.)*im*beta/epsilon);
	s = kd*d_eff*1e-9;
	psi_0_eff = pow(Z_P*e0,1)/(M_PI*epsilon*d_eff*1e-9*(2.+s));
	gamm = beta*M_PI*EPSILON_R*d_eff*1e-9*psi_0_eff*psi_0_eff;

	q = q*d_eff;
	a = -24*ETA*pow(1.+2.*ETA,2)/pow(1.-ETA,4);
	b = 36*ETA*ETA*pow(2.+ETA,2)/pow(1.-ETA,4);
	c0 = ( a*(sin(q)-q*cos(q))
		+  b*((2./pow(q,2)-1.)*q*cos(q)+2.*sin(q)-2./q)
		- ETA*a/2.* (  24./pow(q,3)+4.*(1.-6./pow(q,2))*sin(q)
						 - (1.-12./pow(q,2)+24./pow(q,4))*q*cos(q)
					 )
		)/pow(q,3);

	nc0 = c0;

	ncq = nc0-24.*ETA*gamm*(q*cos(q)+s*sin(q))/(q*(q*q+s*s));
	ncq = nc0-24.*M_PI*ETA*epsilon*psi_0_eff*psi_0_eff*d_eff*1e-9*(q*cos(q)+s*sin(q))/(q*(q*q+s*s)*kb*T);
	return 1./(1.-ncq);
}

scalar sasfit_sq_baba_ahmed_2_f(scalar q, sasfit_param * param)
{
	// insert your code here
	return 0.0;
}

scalar sasfit_sq_baba_ahmed_2_v(scalar q, sasfit_param * param, int dist)
{
	// insert your code here
	return 0.0;
}

