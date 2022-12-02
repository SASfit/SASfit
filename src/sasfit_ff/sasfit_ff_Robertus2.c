/*
 * src/sasfit_ff/sasfit_ff_Robertus2.c
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
#include "include/sasfit_ff_utils.h"
#include <sasfit_utils_f2c.h>

static logical c_false = FALSE_;

/*
double Robertus2(Tcl_Interp *interp,
					double Q,
					double Rmean,
					double dR,
					double sigma, 
					double btau, 
					double P,
					double fp,
						  double eta_core, 
						  double T,
						double x_solvent,
						double eta_solvent,
					bool  *error)
*/
scalar sasfit_ff_Robertus2(scalar q, sasfit_param * param)
{
	static doublereal Rmean_old=-1.0;
	static doublereal sigma_old=-1.0;
	static doublereal btau_old=-1.0;
	static doublereal P_old=-1.0;
	static doublereal fp_old=-1.0;
	static doublereal eta_core_old=-1.0;
	static doublereal eta_shell_old=-1.0;
	static doublereal x_solvent_old=-1.0;
	static doublereal eta_solvent_old=-1.0;
	static doublereal rm, phi, sig;

	static integer i__;
	static doublereal ai;
	static doublereal tmp;
	static doublereal aint0,x0;


	double res, eta_shell, kappa_T, rho_ref, T_ref;
	doublereal  lRmean, lDR, leta_core, lfp, leta_sh, lx_sol, leta_sol, lcore, lshell, lsolvent, 
		kappa_core, kappa_solvent, kappa_shell;
	scalar Rmean, dR, sigma, btau, P, fp, eta_core, T, x_solvent, eta_solvent;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 10, &Rmean, &dR, &sigma, &btau, &P, &fp, &eta_core, &T, &x_solvent, &eta_solvent);
	
	eta_shell = -2.34e9;
	rho_ref = 0.87153; // rho(15C)
	T_ref	= 15.;
	kappa_T = (0.88542-0.87153)/20.0 / rho_ref; //(rho(0C)-rho(20C))/20C/rho(15)
	res = 1.0;

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((Rmean < 0.0), param, "Rmean(%lg) < 0",Rmean);
	SASFIT_CHECK_COND1((sigma < 0.0), param, "sigma(%lg) < 0",sigma);
	SASFIT_CHECK_COND1((sigma > 0.3), param, "sigma(%lg) > 0.3",sigma);
	SASFIT_CHECK_COND1((fabs(btau) <= 0.0), param, "fabs(btau)(%lg) <= 0",fabs(btau));
	SASFIT_CHECK_COND1((fp <= 0.0 || fp >= 1.0), param, "fp(%lg) out of range (0,1)",fp);

	if (	(Rmean_old != Rmean) || 
		(sigma_old != sigma) || 
		(btau_old  != btau)  || 
		(eta_core_old != eta_core) || 
		(eta_shell_old != eta_shell) || 
		(x_solvent_old != x_solvent) || 
		(eta_solvent_old != eta_solvent) || 
		(fp_old != fp) || 
		(P_old	!= P) ) 
	{
		Rmean_old		= Rmean;
		sigma_old		= sigma;
		btau_old		= btau;
		eta_core_old		= eta_core;
		eta_shell_old		= eta_shell;
		x_solvent_old		= x_solvent;
		eta_solvent_old		= eta_solvent;
		fp_old			= fp;
		P_old			= P;

		kappa_core = 4.46849e-6;
		kappa_shell = 9.53147e-5;
		kappa_solvent = 5.5e-5;

		lcore	 = pow(1./(1.-P*kappa_core),1./3.);
		lshell	= pow(1./(1.-P*kappa_shell),1./3.);
		lsolvent = pow(1./(1.-P*kappa_solvent),1./3.);

		leta_core = eta_core/(1.-P*kappa_core);
		lx_sol  = 1.-(1.-x_solvent)/pow(lshell,2.);
		leta_sh = eta_shell/(1.-P*kappa_shell);
		lDR = dR/lshell;
		lRmean = (Rmean-dR)/lcore+lDR;
		leta_sol = eta_solvent/(1.-P*kappa_solvent)/(1.+kappa_T*(T-T_ref));
		lfp = fp/(1.-P*kappa_solvent)/(1.+kappa_T*(T-T_ref));

	//	leta_sol = eta_solvent*(1.0018 +P*7.4638E-5-P*P*1.0969E-8);
		leta_sol = eta_solvent*(0.873 +P*5.65E-5-P*P*5.272E-9)/0.873/(1.+kappa_T*(T-T_ref));
		lfp = fp*(0.873 +P*5.65E-5-P*P*5.272E-9)/0.873/(1.+kappa_T*(T-T_ref));

		rm  = 2.*(doublereal) lRmean;
		sig = (doublereal) sigma;
		phi = (doublereal) lfp;
		_BLNK__1.idistr = 1;
		deeltje_1.rho[0] =  leta_core - leta_sol;
		if ((dR == 0.) || ((leta_sh	- leta_sol)==0.0)) {
			deeltje_1.iform = 1;
			deeltje_1.rho[1] = 0.;
			deeltje_1.rho[2] = 0.;
			deeltje_1.dlayer[0] = 0.;
			deeltje_1.nlayer = 0;
			deeltje_1.nrho = 2;
		} else {
			deeltje_1.iform = -1;
			deeltje_1.nlayer = 1;
			deeltje_1.dlayer[0] = lDR;
			deeltje_1.nrho = 2;
			deeltje_1.rho[1] = (1.0- lx_sol)*(leta_sh-leta_sol);
		}
		_BLNK__1.pi = atan(1.) * 4.;
		_BLNK__1.p = sasfit_eps_get_robertus_p();
		_BLNK__1.atau = (doublereal) 0;
		_BLNK__1.btau = (doublereal) fabs(btau);
/* L5: */
		for (i__ = 1; i__ <= 100 ;++i__) 
		{
			ve_1.x[i__ - 1] = 0.;
		}
		wl_1.li = 1;
		wl_1.lj = 1;
		ve_1.v[0] = 0;
/* L6: */
		for (i__ = 1; i__ <= 19 ;++i__) 
		{
			ve_1.v[i__] = ve_1.v[i__ - 1] + i__;
		}
// the subroutine calc_ does some initializations and solves the nonlinear equation
// solution is needed for calculating the partial structure factors
		sasfit_robertus_calc(param, &rm, &sig, &phi);
		sasfit_robertus_deal(&rm, &sig, &phi, &c_false);
	}
	tmp = (doublereal ) q;
	sasfit_robertus_subint(param, &tmp, &ai, &aint0);
	x0 = 6.*fp/M_PI/pow(rm,3.)/(1+sig*sig)/(1+sig);
//	res = ai/x0;
	res = ai;
//	if (dummy < 0) res = ai/aint0;
	if (sasfit_eps_get_sq_or_iq() < 0) res = ai/aint0;
	return res;
}

