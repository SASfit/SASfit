/*
 * src/sasfit_ff/sasfit_ff_RNDMultiLamellarVesicle.c
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
#include <gsl/gsl_randist.h>
#include <gsl/gsl_specfunc.h>
#include "include/sasfit_ff_utils.h"

#define NRND 20

/*
float RNDMultiLamellarVesicle(Tcl_Interp *interp,
		float q,
		  float R_c,
		  float t_sh,
		float t_sol,
		float Dt_sol,
		  float eta_sh,
		float eta_sol,
		float n,
		float s_Rc,
		float s_tsol,
		  bool  *error)
*/
/*
 //
 // MultiLamellarVesicle:
 //	 l[0]: R_c: core radius, core contains solvent
 //	 l[1]: t_sh: thickness of surfactant layer thickness
 //	 l[2]: t_sol: thickness of solvent layer
 //	 l[3]: eta_sh: scattering length density of surfactant shell
 //	 l[4]: eta_sol: scattering length density of solvent as well as core
 //	 l[5]: n: number of layers
 //	 l[6]: s_Rc: number of layers
 //	 l[7]: s_Dtsol: distribution width of Dtsol
 //
 */
scalar sasfit_ff_RNDMultiLamellarVesicle(scalar q, sasfit_param * param)
{
	scalar sum,sumtsh, sumtsh_im1, sumt,sum_im1;
	scalar R_c, t_sh, t_sol, Dt_sol, eta_sh, eta_sol, n, s_Rc, s_tsol;
	scalar phi,theta,Delta_s, Fi, Fj,rij,t;
	scalar R = 0.0; // was uninitialized, see line 186
	scalar Dshift;
	int i,j,k,N,l,m,p, Nav = NRND;
	static int idum=-1;
	static gsl_rng * r;
	static const gsl_rng_type * T;
	static scalar o_R_c=-1., o_t_sh=-1., o_t_sol=-1., o_Dt_sol=-1.,o_n=-1.;
	static scalar r_i[1000][3][NRND];
	sasfit_param subParam;

	sasfit_get_param(param, 9, &R_c, &t_sh, &t_sol, &Dt_sol, &eta_sh, &eta_sol, &n, &s_Rc, &s_tsol);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R_c < 0.0), param, "R_c(%lg) < 0",R_c);
	SASFIT_CHECK_COND1((t_sol < 0.0), param, "t_sol(%lg) < 0",t_sol);
	SASFIT_CHECK_COND1((t_sh < 0.0), param, "t_sh(%lg) < 0",t_sh);
	SASFIT_CHECK_COND1((Dt_sol > 1.0), param, "Dt_sol(%lg) > 1",Dt_sol);
	SASFIT_CHECK_COND1((n < 1.0), param, "n(%lg) < 1",n);
	SASFIT_CHECK_COND1((n > 1000), param, "n(%lg) > 1000",n);

	if (idum < 0)
	{
		idum = -idum;
		gsl_rng_env_setup();
		T = gsl_rng_default;
		r = gsl_rng_alloc(T);
	}
	if ( 	(o_R_c		!= R_c ) ||
		(o_t_sh		!= t_sh) ||
		(o_t_sol  	!= t_sol) ||
		(o_Dt_sol	!= Dt_sol) ||
		(o_n		!= n))
	{
		o_R_c = R_c;
		o_t_sh = t_sh;
		o_t_sol = t_sol;
		o_Dt_sol = Dt_sol;
		o_n = n;
	}
	if (Nav < sasfit_eps_get_iter_4_mc())
	{
		Nav = sasfit_eps_get_iter_4_mc();
	}

	sasfit_init_param( &subParam );
	N = (int) n;
	sumtsh = 0.0;
	sumtsh_im1 = 0.0;

	for (p=0; p <= Nav ;p++)
	{
		sum = 0.;
		sum_im1 = 0.0;
		t = t_sol-2*s_tsol+p*4.*s_tsol/Nav;
		for (k=0; k <= Nav ;k++)
		{
			sumt = 0.0;
			for (i=0; i < N ;i++)
			{
				for (l=0; l < Nav ;l++)
				{
					r_i[0][0][l] = 0.0;
					r_i[0][1][l] = 0.0;
					r_i[0][2][l] = 0.0;
					for (m=1; m < N ;m++)
					{
						phi = gsl_rng_uniform(r)*2.*M_PI;
						theta = acos(1.-2.*gsl_rng_uniform(r));
						Dshift = Dt_sol*t;
						Delta_s = gsl_rng_uniform(r)*Dshift;
						r_i[m][0][l] = r_i[m-1][0][l] + Delta_s*cos(phi)*sin(theta);
						r_i[m][1][l] = r_i[m-1][1][l] + Delta_s*sin(phi)*sin(theta);
						r_i[m][2][l] = r_i[m-1][2][l] + Delta_s         *cos(theta);
					}
				}

				R = R_c-2*s_Rc+k*4.*s_Rc/Nav;
				/*
				Fi =	K(interp,q,R+i	 *t+i*t_sol,eta_sol-eta_sh,error)
					 + K(interp,q,R+(i+1)*t+i*t_sol,eta_sh-eta_sol,error);
				*/
				subParam.p[0] = R + i*t + i*t_sol;
				subParam.p[3] = eta_sol - eta_sh;
				Fi = sasfit_ff_sphere_f(q, &subParam);

				subParam.p[0] = R+ (i+1)*t + i*t_sol;
				subParam.p[3] = eta_sh - eta_sol;
				Fi += sasfit_ff_sphere_f(q, &subParam);

				for (j=0; j < N ;j++)
				{
					if (i == j)
					{
						sumt = sumt + Fi*Fi;
					} else
					{
						/*
						Fj =	K(interp,q,R+j	 *t+j*t_sol,eta_sol-eta_sh,error)
							 + K(interp,q,R+(j+1)*t+j*t_sol,eta_sh-eta_sol,error);
						*/
						subParam.p[0] = R + j*t + j*t_sol;
						subParam.p[3] = eta_sol - eta_sh;
						Fj = sasfit_ff_sphere_f(q, &subParam);

						subParam.p[0] = R + (j+1)*t + j*t_sol;
						subParam.p[3] = eta_sh - eta_sol;
						Fj += sasfit_ff_sphere_f(q, &subParam);

						rij = sqrt(pow(r_i[i][0][k]-r_i[j][0][k],2)
							+pow(r_i[i][1][k]-r_i[j][1][k],2)
							+pow(r_i[i][2][k]-r_i[j][2][k],2));
                        sumt = sumt + Fi*Fj*gsl_sf_bessel_j0(q*rij);
/*
						if (rij == 0)
						{
							sumt = sumt + Fi*Fj;
						} else
						{
							sumt = sumt + Fi*Fj*sin(q*rij)/(q*rij);
						}
*/
					}
				}
			}
			sum += (sum_im1+sumt)/2.* exp(-pow( R_c-R,2)/(2.*pow(s_Rc ,2)))/( s_Rc*sqrt(2.*M_PI))*4.*s_Rc/Nav;
			sum_im1=sumt;
		}
		sumtsh += (sumtsh_im1+sum)/2.* exp(-pow(t_sol-t,2)/(2.*pow(s_tsol,2)))/(s_tsol*sqrt(2.*M_PI))*4.*s_tsol/Nav;
		sumtsh_im1 = sum;
	}
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return sumtsh;
}

