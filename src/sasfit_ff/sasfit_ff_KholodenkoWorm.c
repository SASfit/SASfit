/*
 * src/sasfit_ff/sasfit_ff_KholodenkoWorm.c
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
#include <gsl/gsl_integration.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_sf.h>
#include "include/sasfit_ff_utils.h"

#define intabseps 0.0
#define intreleps 1.e-7

#define R	param->p[0]
#define B	param->p[1]
#define L	param->p[2]
#define Q	param->p[MAXPAR-1]
#define NP	param->p[MAXPAR-2]

scalar sasfit_ff_KholodenkoWorm_core(scalar x, sasfit_param * param)
{
	scalar E, F, fy;

	SASFIT_ASSERT_PTR(param);

        E = 1.0;
        F = 1.0;

        if (x == 0.0) {
                return 1.0;
        }
        if (Q == 3.0/(B)) {
                fy = x/sinh(x);
        } else  if (Q < 3.0/B) {
                E = sqrt(1.0-pow(Q*B/3.0, 2.0));
                fy = sinh(E*x)/(E*sinh(x));
        } else {
                F = sqrt(pow(Q*B/3.0, 2.0)-1.0);
                fy = sin(F*x)/(F*sinh(x));
        }

        if (NP == 0.0) return 1.0;
        return 2.0/(3.0*NP) * (1.0-x/(3.0*NP)) * fy;
}

scalar sasfit_ff_KholodenkoWorm(scalar q, sasfit_param * param)
{
	scalar res, u, fxs;

	SASFIT_ASSERT_PTR(param);

	NP = L/B;
	if (NP == 0.0)
	{
		return 1.0;
	}

	u = q*R;

	if (u == 0.0) 
	{
		fxs = 1.0;
	} else 
	{
		fxs = pow((2.0*gsl_sf_bessel_J1(u)/u),2.0);
	}

	Q = q;

	if (Q==0) return fxs;

	res = sasfit_integrate_ctm(0.0, 3.0*NP, sasfit_ff_KholodenkoWorm_core, param, 10000, intabseps, intreleps);

	return fxs * res;
}
