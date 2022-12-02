/*
 * src/sasfit_sq/sasfit_sq_ThinDiscs.c
 *
 * Copyright (c) 2008-2012, Paul Scherrer Institute (PSI)
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
#include <gsl/gsl_sf.h>
#include <sasfit_sd.h>
#include "include/sasfit_sq_utils.h"

scalar sasfit_sq_ThinDiscs_core(scalar x, sasfit_param * param)
{
    scalar Q, P, LNdistr, R, Pprime;

	SASFIT_ASSERT_PTR(param);

    Q = param->p[MAXPAR-1];
	R = x;

	if ((Q == 0.0) && (R != 0)) 
	{
		Pprime=gsl_pow_2(M_PI*R*R);
	} 
	else if (R == 0.0) 
	{
		Pprime = 0.0;
	} else 
	{
		Pprime = gsl_pow_2(M_PI*R*R/(Q*R))*2.0*(1.0-gsl_sf_bessel_J1(2*R*Q)/(Q*R));
	}
    //LNdistr = LogNorm(x, 1, sigma, p, R0);
    LNdistr = sasfit_sd_LogNorm(R, param);

	return LNdistr*Pprime;
}


scalar sasfit_sq_ThinDiscs(scalar q, sasfit_param * param)
{
	scalar R, a2, a3, a4, R_0, R_n, R_50, Rstart, Rend, res;
	scalar R0, dummy, sigma;
	static scalar q_old	= -1.0;
	static scalar R0_old	= -1.0; 
	static scalar sigma_old	= -1.0;
	static scalar Pprime 	=  1.0;
	sasfit_param subParam;
	

	SASFIT_ASSERT_PTR(param);
	sasfit_get_param(param, 3, &R0, &dummy, &sigma);
	sasfit_init_param( &subParam );

	if ((q != q_old) || (R0 != R0_old) || (sigma != sigma_old)) 
	{
		R = R0;
		if (sigma == 0) 
		{
			if ((q == 0.0) && (R != 0)) {
				Pprime=gsl_pow_2(M_PI*R*R);
			} else if (R == 0.0) {
				Pprime = 0.0;
			} else 	{
				Pprime = gsl_pow_2(M_PI*R*R/(q*R))*2.0*(1.0-gsl_sf_bessel_J1(2*R*q)/(q*R));
			}
		} else {
			a3 = 1.0;
			a4 = R0;
			a2 = fabs(sigma);
			R_0  = a4*exp(-a2*a2*(a3-4.0));
			R_n  = a4*exp(-a2*a2*(a3-4.0)+sqrt(2.0*a2*a2*log(100.0/0.1)));
			R_50 = a4*exp(-a2*a2*(a3-4.0)+sqrt(2.0*a2*a2*log(100.0/50.0)));
	
			SASFIT_CHECK_COND((R_50 == 0.0), param, "R_50 == 0, Can't guess good integration interval");

			Rstart = 0.0;
			Rend = R_n;
			if ((R_n-R_0) < R_0) 
			{
				Rend = R_n;
				Rstart = 2.0*R_0-R_n;
			}

			// contruction of parameters for LogNorm function (inside kernel)
			subParam.p[0] = 1;
			subParam.p[1] = sigma;
			subParam.p[2] = 1;
			subParam.p[3] = R0;
			subParam.p[MAXPAR-1] = q;

			res = sasfit_integrate(Rstart, Rend, sasfit_sq_ThinDiscs_core, &subParam);
			SASFIT_CHECK_SUB_ERR(param, subParam);
			Pprime = res;
			q_old = q;
			R0_old = R0;
			sigma_old = sigma;
		}
	}

	return Pprime;
}

