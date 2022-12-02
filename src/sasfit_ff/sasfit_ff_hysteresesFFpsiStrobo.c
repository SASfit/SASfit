/*
 * src/sasfit_ff/sasfit_ff_hysteresesFFpsiStrobo.c
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
#include <sasfit_sd.h>
#include "include/sasfit_ff_utils.h"

/*
float I_hysteresesFFpsiStrobo(Tcl_Interp *interp,
		float t, 
		  float phi0deg,
		float Knuc_Kmag,
		float Dl_l,
		float Bc,
		  float S,
		  float frequency,
		  float B0,
		  float Lambda,
		  float SD,
		float psi,
		  bool  *error)
*/

scalar sasfit_ff_hysteresesFFpsiStrobo(scalar q, sasfit_param * param)
{
	scalar Bt,gamma,L,R_av,R,pol,phi0,Knuc,Kmag,DeltaLambda_Lambda,lamb,lamb_min;
	scalar La,res,res1,res2,Ipp,Imm,Ipm,Imp,PPsi,pm,SQ,alpha,alpha_guess,alpha_min,alpha_max;
	scalar A,dB,MB,MBinf;
	scalar sin2, sin4;
	int i,nDl;
	scalar phi0deg, Knuc_Kmag, Dl_l, Bc, S, frequency, B0, Lambda, SD, psi;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 10, &phi0deg, &Knuc_Kmag, &Dl_l, &Bc, &S, &frequency, &B0, &Lambda, &SD, &psi);

	nDl = 15;
	DeltaLambda_Lambda = Dl_l;
	pol = 0.;
	R_av = 1.;
	R = 1.;
	Knuc = 1.;
	Kmag = 1./Knuc_Kmag;
	phi0 = M_PI*phi0deg/180.;
	res = 0.0;
	res1 = 0.0;
	res2 = 0.0;
	lamb_min = Lambda*(1.-DeltaLambda_Lambda);

	for (i=0; i <= 2*nDl ;i++) 
	{
		lamb = lamb_min + i*Lambda*DeltaLambda_Lambda/(1.*nDl);
		A = 2.*M_PI*frequency*(q-SD*lamb/3956.034613)+phi0;
		Bt = B0*cos(A);
		if (fabs(fmod(A,M_PI*2)) <= M_PI) 
		{
			dB = -1.;
		} else 
		{
			dB = 1.;
		}
		if (A<0) dB=-dB;
		MB = 2.0/M_PI*atan((Bt-dB*Bc)*tan(0.5)/S)
				 +dB/M_PI*(atan((B0+Bc)*tan(0.5)/S)
							-atan((B0-Bc)*tan(0.5)/S));
		MBinf = 1.-1./M_PI*(atan((B0+Bc)*tan(0.5)/S)
							 -atan((B0-Bc)*tan(0.5)/S));
		alpha_min = -1000.;
		alpha_max = 1000.;
		//	  sasfit_err("Superparamagnet: MB/MBinf=%lg\n",MB/MBinf);
		if (Bt == 0) 
		{
			alpha = 0.;
		} else 
		{
		//root finding of L(alpha) = MB/MBinf
			while (fabs(alpha_max-alpha_min)>1e-5) 
			{
				alpha_guess = (alpha_min+alpha_max)*0.5;
			 	if (fabs(alpha_guess) > 1e-5) 
				{
					L=1./tanh(alpha_guess)-1./alpha_guess;
			 	} else 
				{
					L=0.0;
				}
				if (L<MB) //(L<(MB/MBinf)) 
				{
					alpha_min = alpha_guess;
				} else 
				{
					alpha_max = alpha_guess;
				}
			}
			alpha = alpha_guess;
		//	 sasfit_err("Superparamagnet: MB=%lf, MBinf=%lg alpha=%lg Bt=%lg, A=%lf\n",MB,MBinf,alpha,Bt,A);
		}
		if ( fabs(alpha)<1.0e-5 )
		{
			La = 1.0/3.0;
			L = 0.0;
		//	  } else if (fabs(alpha)<=1.0e-3) {
		//			L = 1./3.* alpha;
		//		  La = 1.0/3.0;
		} else 
		{
			L = 1.0/tanh(alpha)-1.0/alpha;
			La = L/alpha;
		}
		gamma = 0.0;
		SQ = 1.0;
		PPsi = psi/180.0*M_PI;
		sin2 = pow(sin(PPsi),2.0);
		sin4 = pow(sin(PPsi),4.0);
		pm  = -1.0;
		Ipp = pow(Kmag*(L-gamma)*sin2+pm*Knuc,2.0)*SQ
			 +Kmag*Kmag*(La*sin2-(L*L-1.0+3.0*La)*sin4);
		pm  = +1.0;
		Imm = pow(Kmag*(L-gamma)*sin2+pm*Knuc,2.0)*SQ
			 +Kmag*Kmag*(La*sin2-(L*L-1.0+3.0*La)*sin4);
		Imp = (sin2-sin4)*pow(L-gamma,2.0)*Kmag*Kmag*SQ
			+ Kmag*Kmag*((sin4-sin2)*(L*L-1.0+3.0*La)+(2.0-sin2)*La);
		Ipm = Imp;
		res1 = res2;
		res2 = (1.-pol)/2. * (Imm+Imp) + (1.+pol)/2. * (Ipp+Ipm);

		sasfit_init_param( &subParam );
		subParam.p[0] = 1.0;
		subParam.p[1] = Lambda*(1.-DeltaLambda_Lambda);
		subParam.p[2] = Lambda;
		subParam.p[3] = Lambda*(1.+DeltaLambda_Lambda);
		subParam.p[4] = 1.0;

		// res2 = res2*Triangular(interp, 1., Lambda*(1.-DeltaLambda_Lambda), Lambda, Lambda*(1.+DeltaLambda_Lambda), 1., lamb, error);
		res2 = res2*sasfit_sd_Triangular(lamb, &subParam);
		if ( subParam.errStatus != FALSE ) return res;

		res = res+(res1+res2)*.5*Lambda*DeltaLambda_Lambda/(1.*nDl)*pow(R/R_av,6.);;
	} 

	return res;
}

