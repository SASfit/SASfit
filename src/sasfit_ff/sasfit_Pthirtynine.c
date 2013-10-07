/*
 * src/sasfit_ff/sasfit_Pthirtynine.c
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

scalar Fone(scalar q, scalar R)
{
	scalar F;
	if( (q*R) == 0.0 ) 
	{
		F = 1.0;
	} else {
		F = 3.0*(sin(q*R) - q*R*cos(q*R))/pow(q*R, 3.0);
	}
	return F;
}

scalar V(scalar R)
{
	return 4.0*M_PI*pow(R,3.0)/3.0;
}

scalar Sg(scalar q, scalar Rc, scalar s)
{
	scalar S;
	if(q == 0.0) 
	{
		S = 1.0;
	} 
	else if (Rc == 0.0)
	{
		S = exp(-pow(q*s,2.0)/2.0);
	} 
	else if ((s == 0.0) && (q*Rc != 0.0)) 
	{
		S = sin(q*Rc)/(q*Rc);
	} else 
	{
		S = pow(q*Rc*(4.*Rc*s+sqrt(2.*M_PI)*(pow(Rc,2.)+pow(s,2.))),-1.) * 
			( 2.*Rc*s*sin(q*Rc)  +  sqrt(2*M_PI)*exp(-pow(q*s,2.)/2.) * 
			  (q*Rc*pow(s,2.)*cos(q*Rc)+pow(Rc,2.)*sin(q*Rc)) + 
			   2.*sqrt(2.)*gsl_sf_dawson(q*s/sqrt(2.)) * 
			   (pow(Rc,2.)*cos(q*Rc)-q*Rc*pow(s,2.)*sin(q*Rc))
			);
	}
	return S;
}

scalar Ac(scalar q, scalar Rc, scalar R, scalar s)
{
	scalar Vzero, A;
	Vzero = 2.*M_PI*(4.*Rc*pow(s,2.)+sqrt(2.*M_PI)*(pow(Rc,2.)+pow(s,2.))*s);
	A = (Vzero*Sg(q, Rc,s)+V(Rc)*Fone(q,Rc)-V(R)*Fone(q,R))/(Vzero+V(Rc)-V(R));
	return A;
}

scalar Scc(scalar q, scalar Rc, scalar R, scalar s) 
{
	scalar S;
	S = pow(Ac(q, Rc, R, s),2);
	return S;
}

scalar Ssc(scalar q, scalar Rc, scalar R, scalar s)
{
	scalar S = Fone(q,R) * Ac(q, Rc, R, s);
	return S;
}

scalar Ptwentyone(scalar x) // is identical to "F_Debye"
{
	scalar P;
	if(x == 0.0) 
	{
		P = 1.0;
	} else {
		P = 2*(x-1+exp(-x))/pow(x,2);
	}
	return P;
}

/*
double Pthirtynine(Tcl_Interp *interp, 
				   double q, 
				   double Nc, 
				   double Rg, 
				   double R, 
				   double Rc,
				   double s,
				   double nu, 
				   double Mthirtynine,
				   double rhos,
				   double rhoc,
				   double b,
				   double L,
				   bool *error)
*/
scalar sasfit_Pthirtynine(scalar q, sasfit_param * param)
{
	scalar P, P_ch, P0_ch;
	scalar Nc, Rg, R, Rc, s, nu, Mthirtynine, rhos, rhoc, b, L;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 11, &Nc, &Rg, &R, &Rc, &s, &nu, &Mthirtynine, &rhos, &rhoc, &b, &L);

	sasfit_init_param( &subParam );
	subParam.p[0] = 1.0;
	subParam.p[0] = L;
	subParam.p[0] = b;
	subParam.p[0] = 0.0;

	//P_ch  = WormLikeChainEXV(interp,q,1.0,L,b,0.0,error)/(1.+nu*Ptwentyone(pow(q*Rg,2.)));
	P_ch  = sasfit_ff_WormLikeChainEXV(q, &subParam)/(1.+nu*Ptwentyone(pow(q*Rg,2.)));

	SASFIT_CHECK_SUB_ERR(param, subParam);

	P0_ch = 1./(1.+nu);

	P =    pow(rhos,2)*pow(Fone(q,R),2)
		+  Nc*pow(rhoc,2)*P_ch  
		+  Nc*(Nc-P0_ch)*pow(rhoc,2)*Scc(q, Rc, R, s)  
		+  2*Nc*rhos*rhoc*Ssc(q, Rc, R, s)  ;
// The factor 1/pow(Mthirtynine,2) is skipped here compared to the paper from J.S. Pedersen as P39 shall be in absolute units

	return P;
}

