/*
 * src/sasfit_ff/sasfit_ff_ThinSphShell_homogenousXS.c
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


#define T0		param->p[0]
#define SIGMA_T	param->p[1]
#define R0		param->p[2]
#define SIGMA_R param->p[3]
#define ETA		param->p[4]

#define Q		param->p[MAXPAR-1]

scalar sasfit_ff_ThinSphShell_core(scalar r, sasfit_param * param)
{
	scalar Ptsh, LNdistr;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);


	if (Q == 0.0)
	{
		Ptsh = pow(4.0*M_PI*r*r,2.0);
	} else
	{
		Ptsh = pow(4.0*M_PI*r*sin(Q*r)/(Q),2.0);
	}

	sasfit_init_param( &subParam );
	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_R;
	subParam.p[2] = 1.0;
	subParam.p[3] = R0;

	LNdistr = sasfit_sd_LogNorm(r, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return LNdistr*Ptsh;
}


scalar sasfit_ThinSphShell(scalar q, sasfit_param * param)
{
	scalar a2,a3,a4, R_0, R_n, R_50;
	scalar Rstart = 0.0, Rend = 0.0; // was uninitialized, see line 125
	static scalar Q_old = -1.;
	static scalar R0_old = -1.;
	static scalar sigma_old = -1.;
	static scalar Pprime = 1.;

	SASFIT_ASSERT_PTR(param);

	// R0 == mean
	Q=q;

	if ((Q != Q_old) || (R0 != R0_old) || (SIGMA_R != sigma_old)) 
	{
		if (SIGMA_R == 0) 
		{
			if ((Q == 0.0) && (R0 != 0)) 
			{
				Pprime = (4.0*M_PI*R0*R0) * (4.0*M_PI*R0*R0);
			} 
			else if (R0 == 0.0) 
			{
				Pprime = 0.0;
			} else 
			{
				Pprime = gsl_pow_2( 4.0*M_PI*R0*R0 * sin(Q*R0)/(Q*R0));
			}
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
			Pprime 	= sasfit_integrate(Rstart, Rend, sasfit_ff_ThinSphShell_core, param);
		}

		Q_old	= Q;
		R0_old	= R0;
		sigma_old = SIGMA_R;
	}

	return Pprime;
}


scalar sasfit_ff_homogeneousXS_core(scalar x, sasfit_param * param)
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


scalar sasfit_homogeneousXS(scalar q, sasfit_param * param)
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
			Pcs 	= sasfit_integrate(tstart, tend, sasfit_ff_homogeneousXS_core, param);
		}
		Q_old	= Q;
		T0_old	= T0;
		sigma_T_old = SIGMA_T;
	}

	return Pcs;
}



scalar sasfit_ff_ThinSphShell_homogenousXS(scalar q, sasfit_param * param)
{
	scalar Pcs, Pprime;

	SASFIT_ASSERT_PTR(param);

	Pprime = sasfit_ThinSphShell(q, param);
    Pcs    = sasfit_homogeneousXS(q,param);
	return Pcs*Pprime;
}

