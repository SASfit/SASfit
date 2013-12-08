/*
 * src/sasfit_ff/sasfit_ff_Cylinder.c
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

// define shortcuts for local parameters/variables
#define R			param->p[0]
#define EPSILON		param->p[1]
#define L			param->p[2]
#define T			param->p[3]
#define ETA_CORE	param->p[4]
#define ETA_SH		param->p[5]
#define ETA_SOL		param->p[6]
#define TYPE_SHELL	param->p[MAXPAR-4]
#define Q			param->p[MAXPAR-3]
#define ALPHA		param->p[MAXPAR-2]
#define THETA		param->p[MAXPAR-1]


scalar ell_rad(scalar radius, scalar epsilo, scalar t, scalar theta)
{
	return sqrt((radius+t)*(radius+t)*sin(theta)*sin(theta)+(epsilo*radius+t)*(epsilo*radius+t)*cos(theta)*cos(theta));
}

scalar AcylSHell(sasfit_param *param)
{
	scalar Ain, Aout,A,u1,u2,v1,v2,b1,b2, J1;

	u1 = Q*ell_rad(R,EPSILON,0,THETA)*sin(ALPHA);
	v1 = Q*L*0.5*cos(ALPHA);
	u2 = Q*ell_rad(R,EPSILON,T,THETA)*sin(ALPHA);
	v2 = Q*(L*0.5+T*TYPE_SHELL)*cos(ALPHA);
	b1 = M_PI*R*R*EPSILON*L*(ETA_CORE-ETA_SH);
	b2 = M_PI*(R+T)*(EPSILON*R+T)*(L+2.0*T*TYPE_SHELL)*(ETA_SH-ETA_SOL);
	if (v1==0.0) {
		Ain = 1.0;
	} else {
		Ain = sin(v1)/v1;
	}
	if (u1!=0.0) {
//		J1 = sasfit_bessj1(u1);
		J1 = gsl_sf_bessel_J1(u1);
		Ain = Ain*2.0*J1/u1;
	}
	
	if (v2==0.0) {
		Aout = 1.0;
	} else {
		Aout = sin(v2)/v2;
	}
	if (u2!=0.0) {
//		J1 = sasfit_bessj1(u2);
		J1 = gsl_sf_bessel_J1(u2);
		Aout = Aout*2.0*J1/u2;
	} 


	A = b1*Ain+b2*Aout;
	return A;
}

scalar PcylSHell(sasfit_param *param)
{
	scalar Ain, Aout,A,u1,u2,v1,v2,b1,b2, J1;

	u1 = Q*ell_rad(R,EPSILON,0,THETA)*sin(ALPHA);
	v1 = Q*L*0.5*cos(ALPHA);
	u2 = Q*ell_rad(R,EPSILON,T,THETA)*sin(ALPHA);
	v2 = Q*(L*0.5+T*TYPE_SHELL)*cos(ALPHA);
	b1 = M_PI*R*R*EPSILON*L*(ETA_CORE-ETA_SH);
	b2 = M_PI*(R+T)*(EPSILON*R+T)*(L+2.0*T*TYPE_SHELL)*(ETA_SH-ETA_SOL);
	if (v1==0.0) {
		Ain = 1.0;
	} else {
		Ain = sin(v1)/v1;
	}
	if (u1!=0.0) {
//		J1 = sasfit_bessj1(u1);
		J1 = gsl_sf_bessel_J1(u1);
		Ain = Ain*2.0*J1/u1;
	}
	
	if (v2==0.0) {
		Aout = 1.0;
	} else {
		Aout = sin(v2)/v2;
	}
	if (u2!=0.0) {
//		J1 = sasfit_bessj1(u2);
		J1 = gsl_sf_bessel_J1(u2);
		Aout = Aout*2.0*J1/u2;
	} 


	A = b1*Ain+b2*Aout;
	return A*A;
}


scalar P_theta(scalar x,sasfit_param * param)
{
	THETA = x;
	return PcylSHell(param);
} 

scalar P_alpha(scalar x,sasfit_param * param)
{
	ALPHA = x;
	
	if (EPSILON == 1.0) {
		return P_theta(1.0,param)*sin(ALPHA);
	} else {
		return sasfit_integrate(0.0,M_PI/2.0,&P_theta,param)*2.0/M_PI*sin(ALPHA);
	}
}

scalar A_theta(scalar x,sasfit_param * param)
{
	THETA = x;
	return AcylSHell(param);
} 

scalar A_alpha(scalar x,sasfit_param * param)
{
	ALPHA = x;
	
	if (EPSILON == 1.0) {
		return A_theta(1.0,param)*sin(ALPHA);
	} else {
		return sasfit_integrate(0.0,M_PI/2.0,&A_theta,param)*2.0/M_PI*sin(ALPHA);
	}
}

scalar sasfit_ff_ellCylShell1(scalar q, sasfit_param * param)
{
	scalar P_RODSH;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((EPSILON < 0.0), param, "epsilon(%lg) < 0",EPSILON); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L); // modify condition to your needs
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T); // modify condition to your needs

	Q = q;
	TYPE_SHELL = 0.0;
	P_RODSH = sasfit_integrate(0.0,M_PI/2.0,&P_alpha,param);
	return P_RODSH;
}

scalar sasfit_ff_ellCylShell2(scalar q, sasfit_param * param)
{
	scalar P_RODSH;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((EPSILON < 0.0), param, "epsilon(%lg) < 0",EPSILON); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L); // modify condition to your needs
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T); // modify condition to your needs

	Q = q;
	TYPE_SHELL = 1.0;
	P_RODSH = sasfit_integrate(0.0,M_PI/2.0,&P_alpha,param);
	return P_RODSH;
}

scalar sasfit_ff_ellCylShell1_f(scalar q, sasfit_param * param)
{
	scalar A_RODSH;
	
	Q = q;
	TYPE_SHELL = 0.0;
	A_RODSH = sasfit_integrate(0.0,M_PI/2.0,&A_alpha,param);

	return A_RODSH;
}

scalar sasfit_ff_ellCylShell2_f(scalar q, sasfit_param * param)
{
	scalar A_RODSH;

	Q = q;
	TYPE_SHELL = 1.0;
	A_RODSH = sasfit_integrate(0.0,M_PI/2.0,&A_alpha,param);

	return A_RODSH;
}
scalar sasfit_ff_ellCylShell1_v(scalar q, sasfit_param * param, int dist)
{
	// insert your code here
	return M_PI*(R+T)*(R*EPSILON+T)*L;
}


scalar sasfit_ff_ellCylShell2_v(scalar q, sasfit_param * param, int dist)
{
	// insert your code here
	return M_PI*(R+T)*(R*EPSILON+T)*(L+2*T);
}
