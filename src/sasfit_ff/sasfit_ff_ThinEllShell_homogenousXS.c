/*
 * src/sasfit_ff/sasfit_ff_ThinEllShell_homogenousXS.c
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
#include <sasfit_sd.h>
#include "include/sasfit_ff_utils.h"

#define T0		param->p[0]
#define SIGMA_T	param->p[1]

#define R0		param->p[2]
#define SIGMA_R param->p[3]

#define S0		param->p[2]
#define SIGMA_S param->p[3]

#define EPSILON param->p[4]
#define ETA		param->p[5]

#define Q		param->p[MAXPAR-1]
#define	R		param->p[MAXPAR-2]

scalar thinF1(scalar q, scalar r, scalar epsilon, scalar alpha)
{
    scalar u, Rtmp;

	Rtmp = r*sqrt(pow(sin(alpha),2.)+pow(epsilon*cos(alpha),2.));
	       
	u = q * Rtmp;

        if (u == 0.0) 
	{
		return 1.0;
	}
        return sin(u)/u;
}

scalar sasfit_thinEll_P_core(scalar alpha, sasfit_param * param)
{
	scalar S, ae;

	SASFIT_ASSERT_PTR(param);

	if (EPSILON < 1.) 
	{
		ae = acos(EPSILON);
		S = 2.*M_PI*R*R*(1.+EPSILON*gsl_atanh(sin(ae))/sin(ae));
	} 
	else if (EPSILON > 1.) 
	{
		ae = acos(1./EPSILON);
		S = 2.*M_PI*R*R*(1.+EPSILON*ae/tan(ae));
	} else 
	{
		S = 4.*M_PI*R*R;
	}
	return  pow(S * thinF1(Q, R, EPSILON, alpha),2.0)*sin(alpha);
}


scalar sasfit_thinEll_P(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param);

	return sasfit_integrate(0.0, M_PI/2.0, sasfit_thinEll_P_core, param);
}

scalar sasfit_ff_ThinEllShell_core(scalar r, sasfit_param * param)
{
	scalar P, LNdistr;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	R = r;

	if (EPSILON != 1.0) {
		P = sasfit_thinEll_P(Q, param);
	} else {
		P = sasfit_thinEll_P_core(0.0,param);
	}
	sasfit_init_param( &subParam );
	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_R;
	subParam.p[2] = 1.0;
	subParam.p[3] = R0;

	LNdistr = sasfit_sd_LogNorm(r, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return LNdistr*P;
}


scalar sasfit_r0(scalar epsilon, scalar s0)
{
	scalar r0, ae;
	if (epsilon < 1.) 
	{
		ae = acos(epsilon);
		r0 = sqrt(s0/(1.+epsilon*gsl_atanh(sin(ae))/sin(ae)) / (2.*M_PI));
	} 
	else if (epsilon > 1.) 
	{
		ae = acos(1./epsilon);
		r0 = sqrt(s0/(1.+epsilon*ae/tan(ae)) / (2.*M_PI));
	} else 
	{
		r0 = sqrt(s0 / 4./M_PI);
	}
	return r0;
}

scalar sasfit_ff_ThinEllShell_S_core(scalar S, sasfit_param * param)
{
	scalar P, LNdistr;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	R = sasfit_r0(EPSILON, S);

	P = sasfit_thinEll_P(Q, param);

	sasfit_init_param( &subParam );
	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_S;
	subParam.p[2] = 1.0;
	subParam.p[3] = S0;

	LNdistr = sasfit_sd_LogNorm(S, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return LNdistr*P;
}


scalar sasfit_ThinEllShell(scalar q, sasfit_param * param)
{
	scalar a2,a3,a4, R_0, R_n, R_50;
	scalar Rstart = 0.0, Rend = 0.0; // was uninitialized, see line 216
	
	static scalar Q_old = -1.;
	static scalar R0_old = -1.;
	static scalar sigma_old = -1.;
	static scalar epsilon_old = -1.;
	static scalar Pprime = 1.;

	SASFIT_ASSERT_PTR(param);

	Q = q;

	if ((Q != Q_old) || (R0 != R0_old) || (SIGMA_R != sigma_old) || (EPSILON != epsilon_old)) 
	{
		R = R0;
		if (SIGMA_R == 0.0) 
		{
			Pprime = sasfit_thinEll_P(q, param);
		} else 
		{
			a3 = 1.0;
			a4 = R0;
			a2 = SIGMA_R;
			R_0  = a4*exp(-a2*a2*(a3-4.0));
			R_n  = a4*exp(-a2*a2*(a3-4.0)+sqrt(2.0*a2*a2*log(100.0/0.1)));
			R_50 = a4*exp(-a2*a2*(a3-4.0)+sqrt(2.0*a2*a2*log(100.0/50.0)));

			SASFIT_CHECK_COND((R_50 == 0.0), param, "Can't guess good integration interval");

			Rstart = 0.0;
			Rend = R_n;
			if ((R_n-R_0) < R_0)
			{
				Rend = R_n;
				Rstart = 2.0*R_0-R_n;
			}
			Pprime 	= sasfit_integrate(Rstart, Rend, sasfit_ff_ThinEllShell_core, param);
		}

		Q_old		= Q;
		R0_old		= R0;
		sigma_old 	= SIGMA_R;
		epsilon_old	= EPSILON;
	}

	return Pprime;
}

scalar sasfit_ThinEllShell_S(scalar q, sasfit_param * param)
{
	scalar S,S_0,S_n,S_50,a2,a3,a4;
	scalar Sstart = 0.0, Send = 0.0; // was uninitialized, see line 272
	static scalar Q_old = -1.;
	static scalar S0_old = -1.;
	static scalar sigma_old = -1.;
	static scalar epsilon_old = -1.;
	static scalar Pprime = 1.;

	SASFIT_ASSERT_PTR(param);

	Q = q;

	if ((q != Q_old) || (S0 != S0_old) || (SIGMA_S != sigma_old) || (EPSILON != epsilon_old)) 
	{
		S = S0;
		if (SIGMA_S == 0) 
		{
			param->p[MAXPAR-3] = sasfit_r0(EPSILON,S0);
			Pprime = sasfit_thinEll_P(q, param);
		} else 
		{
			a3 = 1.0;
			a4 = S0;
			a2 = SIGMA_S;
			S_0  = a4*exp(-a2*a2*(a3-2.0));
			S_n  = a4*exp(-a2*a2*(a3-2.0)+sqrt(2.0*a2*a2*log(100.0/0.1)));
			S_50 = a4*exp(-a2*a2*(a3-2.0)+sqrt(2.0*a2*a2*log(100.0/50.0)));

			SASFIT_CHECK_COND((S_50 == 0.0), param, "Can't guess good integration interval");

			Sstart = 0.0;
			Send = S_n;
			if ((S_n-S_0) < S_0)
			{
				Send = S_n;
				Sstart = 2.0*S_0-S_n;
			}
			Pprime 	= sasfit_integrate(Sstart, Send, sasfit_ff_ThinEllShell_S_core, param);
		}

		Q_old		= q;
		S0_old		= S0;
		sigma_old 	= SIGMA_S;
		epsilon_old	= EPSILON;
	}

	return Pprime;
}

scalar sasfit_ff_EllhomogeneousXS_core(scalar x, sasfit_param * param)
{
	scalar Pcs, u, LNdistr;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);


	u = Q*x*0.5;
	if (u  == 0.0) 
	{
		Pcs = gsl_pow_2(x*ETA);
	} else {
		Pcs = gsl_pow_2(x*ETA*sin(u)/u);
	}

	sasfit_init_param( &subParam );
	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_T;
	subParam.p[2] = 1.0;
	subParam.p[3] = T0;

	LNdistr = sasfit_sd_LogNorm(x, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return LNdistr*Pcs;
}


scalar sasfit_EllhomogeneousXS(scalar q, sasfit_param * param)
{
	scalar a2,a3,a4,t_n,t_0,t_50,u;
	scalar tstart = 0.0, tend = 0.0; // was uninitialized, see line 125
	static scalar Q_old = -1.;
	static scalar T0_old = -1.;
	static scalar sigma_T_old = -1.;
	static scalar Pcs = 1.;

	SASFIT_ASSERT_PTR(param);

	Q = q;

	if ((Q != Q_old) || (T0 != T0_old) || (SIGMA_T != sigma_T_old)) 
	{
		if (SIGMA_T == 0.0) 
		{
			u = q*T0*0.5;
			if (u  == 0.0) 
			{
				Pcs = gsl_pow_2(T0*ETA);
			} else {
				Pcs = gsl_pow_2(T0*ETA*sin(u)/u);
			}
		} else 
		{
			a3 = 1.0;
			a4 = T0;
			a2 = SIGMA_T;
			t_0  = a4*exp(-a2*a2*(a3-2.0));
			t_n  = a4*exp(-a2*a2*(a3-2.0)+sqrt(2.0*a2*a2*log(100.0/0.1)));
			t_50 = a4*exp(-a2*a2*(a3-2.0)+sqrt(2.0*a2*a2*log(100.0/50.0)));

			SASFIT_CHECK_COND((t_50 == 0.0), param, "Can't guess good integration interval");

			tstart = 0.0;
			tend = t_n;
			if ((t_n-t_0) < t_0)
			{
				tend = t_n;
				tstart = 2.0*t_0-t_n;
			}
			Pcs 	= sasfit_integrate(tstart, tend, sasfit_ff_EllhomogeneousXS_core, param);
		}
		Q_old	= Q;
		T0_old	= T0;
		sigma_T_old = SIGMA_T;
	}

	return Pcs;
}


scalar sasfit_ff_ThinEllShell_homogenousXS(scalar q, sasfit_param * param)
{
	scalar Pcs, Pprime;

	SASFIT_ASSERT_PTR(param);

	Pprime = sasfit_ThinEllShell(q, param);
    Pcs    = sasfit_EllhomogeneousXS(q,param);

	return Pcs*Pprime;
}

scalar sasfit_ff_ThinEllShell_S_homogenousXS(scalar q, sasfit_param * param)
{
	scalar Pcs, Pprime;

	SASFIT_ASSERT_PTR(param);

	Pprime = sasfit_ThinEllShell_S(q, param);
    Pcs    = sasfit_EllhomogeneousXS(q,param);

	return Pcs*Pprime;
}

