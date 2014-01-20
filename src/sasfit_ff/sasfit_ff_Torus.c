/*
 * src/sasfit_ff/sasfit_ff_Torus.c
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
#include "include/sasfit_ff_utils.h"

#define R		param->p[0]
#define A		param->p[1]
#define DA		param->p[2]
#define NU		param->p[3]
#define ETAC	param->p[4]
#define ETASH	param->p[5]
#define ETASOL	param->p[6]
#define Q		param->p[MAXPAR-1]
#define THETA	param->p[MAXPAR-2]
#define	ETA		param->p[MAXPAR-3]
#define ATOT	param->p[MAXPAR-4]

scalar sasfit_Torus_core_int_r(scalar r, sasfit_param * param)
{
	scalar gamma, KTcore, s;

	SASFIT_ASSERT_PTR(param);

	SASFIT_CHECK_COND((Q == 0.0), param, "Q == 0");
	SASFIT_CHECK_COND2((ATOT > R), param, "a(%lg) > R(%lg)",ATOT,R);

	s = Q/(2.0*M_PI);
	gamma = NU*sqrt(ATOT*ATOT - (r-R)*(r-R));

	if (THETA == 0.0) 
	{
		return ETA*4.*r*sin(M_PI*s*gamma)*cos(M_PI*s*gamma)/s;
	}
	if (THETA == M_PI_2) 
	{
		return ETA*4.*gsl_sf_bessel_J0(2.*M_PI*s*r)*M_PI*r*gamma;
	}
	KTcore = ETA*4.*M_PI*r
		* gsl_sf_bessel_J0(2.*M_PI*s*r*sin(THETA)) 
		* sin(2.*M_PI*s*gamma*cos(THETA))
		/ (2.0*M_PI*s*cos(THETA));

	return KTcore;
}


scalar sasfit_Torus_core_int_theta(scalar theta, sasfit_param * param)
{
	scalar TorusShell, int_r, int_r_delta;

	SASFIT_ASSERT_PTR(param);

	THETA = theta;
	ETA = 1.0; // eta

	ATOT = A;
	int_r	= sasfit_integrate(R-ATOT, R+ATOT, sasfit_Torus_core_int_r, param);
	
	if (DA == 0.0 || ETASH == ETASOL)
	{
		return gsl_pow_2((ETAC-ETASOL)*int_r) * sin(theta);
	}

	ATOT = A+DA;
	int_r_delta	= sasfit_integrate(R-ATOT, R+ATOT, sasfit_Torus_core_int_r, param);

	if (ETASH == ETAC) 
	{
		return gsl_pow_2((ETASH-ETASOL)*int_r_delta) * sin(theta);
	}

	TorusShell = (ETASH-ETASOL)*int_r_delta + (ETAC-ETASH)*int_r;

	return TorusShell*TorusShell * sin(theta);
}


/*
        R: radius of torus
		a: inner radius of toris cross section
		Delta_a: shell thickness of torus cross section
		nu: if not equal one it describes scaling factor in case elliptical cross section
		eta_c: scattering length density of core
		eta_sh: scattering length density of shell
		eta_sol: scattering length density of solvent
 */
scalar sasfit_ff_Torus(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param);

	Q = q;
	return sasfit_integrate(0.0, M_PI_2, sasfit_Torus_core_int_theta, param);
}

