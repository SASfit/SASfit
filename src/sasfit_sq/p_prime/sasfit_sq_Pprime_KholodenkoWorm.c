/*
 * src/sasfit_ff/sasfit_sq_Pprime_KholodenkoWorm.c
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
 */

#include <gsl/gsl_math.h>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_errno.h>
#include "../include/sasfit_sq_utils.h"

#define intabseps 0.0
#define intreleps 1.e-7

#define	LB	param->p[0]
#define	L	param->p[1]
#define	N	param->p[MAXPAR-1]
#define	Q	param->p[MAXPAR-2]

scalar sasfit_sq_Pprime_KholodenkoWorm_core(scalar x, sasfit_param * param)
{
	scalar E, F, fy;

	SASFIT_ASSERT_PTR(param);

        E = 1.0;
        F = 1.0;

        if (x == 0.0) return 1.0;

        if (Q == 3.0/LB) 
	{
                fy = x/sinh(x);
        } 
	else  if (Q < 3.0/LB) 
	{
                E = sqrt(1.0-pow(Q*LB/3.0, 2.0));
                fy = sinh(E*x)/(E*sinh(x));
        } else {
                F = sqrt(pow(Q*LB/3.0, 2.0)-1.0);
                fy = sin(F*x)/(F*sinh(x));
        }

        if (N == 0.0) return 1.0;
        return 2.0/(3.0*N) * (1.0-x/(3.0*N)) * fy;
}


scalar sasfit_sq_Pprime_KholodenkoWorm(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param);

	N = L/LB;
	if (N == 0.0)
	{
		return 1.0;
	}

	Q = q;
//	return L*L*sasfit_integrate_ctm(0.0, 3.0*N, sasfit_sq_Pprime_KholodenkoWorm_core, param, 10000, intabseps, intreleps);
	if (N > 1) {
		if (q > 10.*M_PI/LB) {
			return L*L*M_PI/(L*q);
		} else {
			return L*L*sasfit_integrate_ctm(0.0, 3.0*N, sasfit_sq_Pprime_KholodenkoWorm_core, param, 10000, intabseps, intreleps);
		}
	} else {
		if (q > 10.*M_PI/L) {
			return L*L*M_PI/(L*q);
		} else {
			return L*L*sasfit_integrate_ctm(0.0, 3.0*N, sasfit_sq_Pprime_KholodenkoWorm_core, param, 10000, intabseps, intreleps);
		}
	}
}
