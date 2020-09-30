/*
 * src/sasfit_ff/sasfit_ff_RNDMultiLamellarVesicle2.c
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
#include <gsl/gsl_randist.h>
#include "include/sasfit_ff_utils.h"

#define NMAX 200

scalar sasfit_ff_RNDMultiLamellarVesicle2(scalar q, sasfit_param * param)
{
	scalar sum, sumt;
	scalar Delta_s, DRi, Fi, Fj, rij;
	scalar xc, yc, zc;
	scalar t_sh, s_tsh, R_c, s_Rc, n, s_n, t_sol, s_tsol, Dt_sol, Delta_eta;
	int i, j, N, p, Nav = 200;
	static int idum = -1;
	static gsl_rng * r;
	static const gsl_rng_type * T;
//	static float o_R_c=-1., o_t_sh=-1., o_t_sol=-1., o_Dt_sol=-1.,o_n=-1.;
	static scalar r_i[NMAX][3], R_i[NMAX], tsh_i[NMAX];
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 10, &t_sh, &s_tsh, &R_c, &s_Rc, &n, &s_n, &t_sol, &s_tsol, &Dt_sol, &Delta_eta);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R_c <= 0.0), param, "R_c(%lg) <= 0",R_c);
	SASFIT_CHECK_COND1((s_Rc < 0.0), param, "s_Rc(%lg) < 0",s_Rc);
	SASFIT_CHECK_COND1((t_sol < 0.0), param, "t_sol(%lg) < 0",t_sol);
	SASFIT_CHECK_COND1((s_tsol < 0.0), param, "s_tsol(%lg) < 0",s_tsol);
	SASFIT_CHECK_COND1((t_sh < 0.0), param, "t_sh(%lg) < 0",t_sh);
	SASFIT_CHECK_COND1((s_tsh < 0.0), param, "s_tsh(%lg) < 0",s_tsh);
	SASFIT_CHECK_COND1((n < 1.0), param, "n(%lg) < 1",n);
	SASFIT_CHECK_COND1((n > 100), param, "n(%lg) > 100",n);
	SASFIT_CHECK_COND1((s_n < 0.0), param, "s_n(%lg) < 0",s_n);

	if (idum < 0)
	{
		idum = 1;
		gsl_rng_env_setup();
		T = gsl_rng_default;
		r = gsl_rng_alloc(T);
	}
	if (20 < sasfit_eps_get_iter_4_mc())
	{
		Nav = sasfit_eps_get_iter_4_mc();
	} else {
		Nav =20;
	}

	sasfit_init_param( &subParam );
	sumt = 0.0;

	for (p=0; p < Nav ;p++)
	{
		//  do {
		//	  N = (int) (gsl_ran_gaussian (r,s_n)+n);
		//  } while ((N>n+3*s_n) || (N<1));
		if (s_n== 0.0) {
			N = (int) n;
		} else {
			do {
				N = (int) gsl_ran_lognormal(r,log(n),s_n);
			}
			while ((N>NMAX) || (N<1));
		}
		r_i[0][0] = 0.0;
		r_i[0][1] = 0.0;
		r_i[0][2] = 0.0;

		for (i=0; i < N ;i++)
		{
			if (s_tsh == 0.0) {
				tsh_i[i] = t_sh;
			} else {
				do {
					tsh_i[i] = gsl_ran_gaussian(r,s_tsh)+t_sh;
				}
				while (tsh_i[i] <= 0);
			}
		}

		//	  do {
		//		  R_i[0] = gsl_ran_gaussian(r,s_Rc)+R_c;
		//	  } while (R_i[0] <= 0);
		if (s_Rc == 0.0) {
			R_i[0] = R_c;
		} else {
			do {
				R_i[0] = gsl_ran_lognormal(r,log(R_c),s_Rc);
			}
			while (R_i[0] <= 0);
		}
		for (i=1; i < N ;i++)
		{
			gsl_ran_dir_3d(r,&xc,&yc,&zc);
			if (s_tsol == 0.0) {
				DRi = t_sol;
			} else {
				do {
					DRi = gsl_ran_gaussian(r,s_tsol)+t_sol;
				}
				while (DRi < 0);
			}
			R_i[i] = R_i[i-1]+tsh_i[i-1]+DRi;
			if (Dt_sol <= 0) {
				Delta_s = Dt_sol*DRi;
			} else {
				do {
					Delta_s = Dt_sol*DRi*gsl_rng_uniform(r);
				}
				while (Delta_s < 0);
			}
			r_i[i][0] = r_i[i-1][0] + Delta_s*xc;
			r_i[i][1] = r_i[i-1][1] + Delta_s*yc;
			r_i[i][2] = r_i[i-1][2] + Delta_s*zc;
		}
		sum=0.0;
		for (i=0; i < N ;i++)
		{
			/*
			Fi =  K(interp,q,R_i[i]			,-Delta_eta,error)
				+ K(interp,q,R_i[i]+tsh_i[i], Delta_eta,error);
				*/
			subParam.p[0] = R_i[i];
			subParam.p[3] = -Delta_eta;
			Fi = sasfit_ff_sphere_f(q, &subParam);

			subParam.p[0] = R_i[i] + tsh_i[i];
			subParam.p[3] = Delta_eta;
			Fi += sasfit_ff_sphere_f(q, &subParam);

			sum = sum + Fi*Fi;
			for (j=i+1; j < N ;j++)
			{
				/*
				Fj =  K(interp,q,R_i[j]			,-Delta_eta,error)
					+ K(interp,q,R_i[j]+tsh_i[j], Delta_eta,error);
				*/
				subParam.p[0] = R_i[j];
				subParam.p[3] = -Delta_eta;
				Fj = sasfit_ff_sphere_f(q, &subParam);

				subParam.p[0] = R_i[j] + tsh_i[j];
				subParam.p[3] = Delta_eta;
				Fj += sasfit_ff_sphere_f(q, &subParam);

				rij = sqrt( pow(r_i[i][0]-r_i[j][0],2)
					       +pow(r_i[i][1]-r_i[j][1],2)
					       +pow(r_i[i][2]-r_i[j][2],2)
						  );
				if (rij == 0)
				{
					sum = sum + 2.0*Fi*Fj;
				} else
				{
					sum = sum + 2.0*Fi*Fj*sin(q*rij)/(q*rij);
				}
			}
		}
		sumt = sumt+sum;
	}
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return sumt/Nav;
}

