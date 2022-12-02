/*
 * src/sasfit_ff/sasfit_ff_francois.c
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
#include "../sasfit_sq/include/sasfit_sq.h"


scalar sasfit_pave_core(scalar x, sasfit_param * param);
scalar sasfit_pave_core_norm(scalar x, sasfit_param * param);


scalar sasfit_ff_francois(scalar q, sasfit_param * param)
{
	scalar Plocal, Pa, res1, res2;
	scalar Rc, Rm, nu, L, b, Nagg, Drho, K, EXVOL;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 9, &Rc, &Rm, &nu, &L, &b, &Nagg, &Drho, &K, &EXVOL);

	// need to fix sasfit_ff_KholodenkoWorm, not required here - when WormLikeChainEXV is done
	sasfit_init_param( &subParam );



	param->p[MAXPAR-1] = q;
	if (EXVOL <= 0) {
		subParam.p[0] = b;
		subParam.p[1] = L;
//		Plocal  = sasfit_sq_Pprime_WormPS2(q, &subParam)/L/L;
		exit(999);
				Plocal  = sasfit_ff_KholodenkoWorm(q, &subParam)/L/L;
	} else {
		subParam.p[0] = 1.0;
		subParam.p[1] = L;
		subParam.p[2] = b;
		subParam.p[3] = 0.0;
		Plocal 	= sasfit_ff_WormLikeChainEXV(q, &subParam);
	}
	res1 = sasfit_integrate(Rc, Rm, sasfit_pave_core, param);
	res2 = sasfit_integrate(Rc, Rm, sasfit_pave_core_norm, param);
	Pa = (res1*res1)/(res2*res2);
	/*
	Plocal 	= WormLikeChainEXV(interp,q,1,L,b,0,error);
	Pa	= Pave(interp,q,Rc,Rm,nu,error);
	*/

	return Drho*Drho*K*((Nagg-1)*Pa+Plocal);
}

scalar sasfit_pave_core(scalar x, sasfit_param * param)
{
	scalar Q,u,nu;

	SASFIT_ASSERT_PTR(param);

	Q	= param->p[MAXPAR-1];
	nu	= param->p[2];

	u = Q*x;

	if (u == 0) return pow(x,(1-nu)/nu)*4.0*M_PI;

	return pow(x,(1-nu)/nu)*4.0*M_PI*sin(u)/u;
}

scalar sasfit_pave_core_norm(scalar x, sasfit_param * param)
{
	scalar Q,nu;

	SASFIT_ASSERT_PTR(param);

	Q	= param->p[MAXPAR-1];
	nu	= param->p[2];

	return pow(x,(1-nu)/nu)*4*M_PI;
}

