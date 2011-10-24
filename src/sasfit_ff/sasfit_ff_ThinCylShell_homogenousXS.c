/*
 * src/sasfit_ff/sasfit_ff_ThinCylShell_homogenousXS.c
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
#include <sasfit_sd.h>
#include "include/sasfit_ff_utils.h"

#define T0			param->p[0]
#define SIGMA_T		param->p[1]
#define R0			param->p[2]
#define L0			param->p[3]
#define SIGMA_RL	param->p[4]

#define ETA		param->p[6]

#define Q		param->p[MAXPAR-1]
#define	R		param->p[MAXPAR-2]
#define	H		param->p[MAXPAR-3]
#define ALPHA	param->p[MAXPAR-4]


scalar sasfit_XI_CylShell(scalar x, sasfit_param * param)
{
	scalar xR, xH, XIres;

	SASFIT_ASSERT_PTR(param);

	xR = x*R*sin(ALPHA);
	xH = x*H*cos(ALPHA)/2.;

	if ((R+H) == 0) return 0;

	if (xR == 0) 
	{
		XIres = R/(R+H) * cos(xH);
	} else 
	{
		XIres = R/(R+H) * 2.*gsl_sf_bessel_J1(xR)/xR * cos(xH);
	}

	if (xH == 0) 
	{
		XIres = XIres + H/(R+H) * gsl_sf_bessel_J0(xR);
	} else 
	{
		XIres = XIres + H/(R+H) * gsl_sf_bessel_J0(xR)*sin(xH)/xH;
	}
	return (2.* M_PI*R*R + 2.0*M_PI*R*H)*XIres;
}


scalar sasfit_thinCyl_P_core(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param);

	ALPHA = x;

	return  gsl_pow_2(sasfit_XI_CylShell(Q, param)) * sin(ALPHA);
}

scalar sasfit_thinCyl_P(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param);

	return sasfit_integrate(0.0, M_PI/2.0, sasfit_thinCyl_P_core, param);
}



scalar sasfit_ff_ThinCylShell_H_core(scalar l, sasfit_param * param)
{
	scalar P, LNdistr;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	H = l;

	P = sasfit_thinCyl_P(Q,param);

	sasfit_init_param( &subParam );
	
	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_RL;
	subParam.p[2] = 1.0;
	subParam.p[3] = L0;

	LNdistr = sasfit_sd_LogNorm(l, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return LNdistr*P;
}

scalar sasfit_ff_ThinCylShell_H(scalar q,sasfit_param * param)
{
	scalar a2,a3,a4, L_0, L_n, L_50;
	scalar Lstart = 0.0, Lend = 0.0;

	static scalar Q_old = -1.;
	static scalar R0_old = -1.;
	static scalar sigma_RL_old = -1.;
	static scalar H_old = -1.;
	static scalar Pprime = 1.;

	SASFIT_ASSERT_PTR(param);

	if ((q != Q_old) || (R0 != R0_old) || (SIGMA_RL != sigma_RL_old) || (H !=H_old)) 
	{
		Pprime = sasfit_thinCyl_P(Q, param);
		goto out;
		if (SIGMA_RL == 0.0) 
		{
			H = L0;
			Pprime = sasfit_thinCyl_P(Q, param);
		} else 
		{
			a3 = 1.0;
			a4 = L0;
			a2 = SIGMA_RL;
			L_0  = a4*exp(-a2*a2*(a3-2.0));
			L_n  = a4*exp(-a2*a2*(a3-2.0)+sqrt(2.0*a2*a2*log(100.0/0.1)));
			L_50 = a4*exp(-a2*a2*(a3-2.0)+sqrt(2.0*a2*a2*log(100.0/50.0)));

			SASFIT_CHECK_COND((L_50 == 0.0), param, "Can't guess good integration interval");

			Lstart = 0.0;
			Lend = L_n;
			if ((L_n-L_0) < L_0)
			{
				Lend = L_n;
				Lstart = 2.0*L_0-L_n;
			}
			Pprime 	= sasfit_integrate(Lstart, Lend, sasfit_ff_ThinCylShell_H_core, param);
		}

out:
		Q_old	= q;
		R0_old	= R0;
		sigma_RL_old = SIGMA_RL;
		H_old	= L0;
	}
	return Pprime;
}

scalar sasfit_ff_ThinCylShell_core(scalar x, sasfit_param * param)
{
	scalar P, LNdistr;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	R = x*R0;
	H = x*L0;
	P = sasfit_ff_ThinCylShell_H(Q,param);

	sasfit_init_param( &subParam );
	
	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_RL;
	subParam.p[2] = 1.0;
//	subParam.p[3] = R0;
	subParam.p[3] = 1.0;

	LNdistr = sasfit_sd_LogNorm(x, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return LNdistr*P;
}

scalar sasfit_ff_ThinCylShell(scalar q, sasfit_param * param)
{
	scalar a2,a3,a4, x_0, x_n, x_50;
	scalar xstart = 0.0, xend = 0.0;

	static scalar Q_old = -1.;
	static scalar R0_old = -1.;
	static scalar sigma_RL_old = -1.;
	static scalar H_old = -1.;
	static scalar Pprime = 1.;

	SASFIT_ASSERT_PTR(param);

	Q = q;

	if ((q != Q_old) || (R0 != R0_old) || (SIGMA_RL != sigma_RL_old) || (H !=H_old) ) 
	{
		if (SIGMA_RL == 0.0) 
		{
			R = R0;
			H = L0;
			Pprime = sasfit_ff_ThinCylShell_H(Q,param);;
		} else 
		{
			a3 = 1.0;
			a4 = R0;
			a2 = SIGMA_RL;
			x_0  = a4*exp(-a2*a2*(a3-4.0));
			x_n  = a4*exp(-a2*a2*(a3-4.0)+sqrt(2.0*a2*a2*log(100.0/0.1)));
			x_50 = a4*exp(-a2*a2*(a3-4.0)+sqrt(2.0*a2*a2*log(100.0/50.0)));

			SASFIT_CHECK_COND((x_50 == 0.0), param, "Can't guess good integration interval");

			xstart = 0.0;
			xend = x_n;
			if ((x_n-x_0) < x_0)
			{
				xend = x_n;
				xstart = 2.0*x_0-x_n;
			}
			
			Pprime 	= sasfit_integrate(xstart, xend, sasfit_ff_ThinCylShell_core, param);
		}

		Q_old	= q;
		R0_old	= R0;
		H_old	= L0;
		sigma_RL_old = SIGMA_RL;
	}

	return Pprime;
}

scalar sasfit_ff_CylhomogeneousXS_core(scalar x, sasfit_param * param)
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


scalar sasfit_CylhomogeneousXS(scalar q, sasfit_param * param)
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
			Pcs 	= sasfit_integrate(tstart, tend, sasfit_ff_CylhomogeneousXS_core, param);
		}
		Q_old	= Q;
		T0_old	= T0;
		sigma_T_old = SIGMA_T;
	}

	return Pcs;
}


scalar sasfit_ff_ThinCylShell_homogenousXS(scalar q, sasfit_param * param)
{
	scalar Pcs, Pprime;

	SASFIT_ASSERT_PTR(param);

	Pprime = sasfit_ff_ThinCylShell(q, param);
    Pcs    = sasfit_CylhomogeneousXS(q,param);

	return Pcs*Pprime;
}

