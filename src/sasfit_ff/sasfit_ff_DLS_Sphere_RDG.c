/*
 * src/sasfit_ff/sasfit_ff_DLS_Sphere_RDG.c
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

/*
float DLS_Sphere_RDG(Tcl_Interp *interp,
					 float t,
							float R,
							float viscosity,
					 float T,
							float Q,
							bool  *error)
*/
/**
 * Dynamic light scattering g2(t) of sperical particles in Raylay-Debye-Gans
 * approximation
 * R:radius eta:viscosity T:temperature Q:scattering vector
 * [t] = 1 s, [R] = 1 nm, [eta] = 1 ??, [T] = 1 K, [Q] = 1 nm-1
 */
scalar sasfit_ff_DLS_Sphere_RDG(scalar q, sasfit_param * param)
{
	scalar D, k, Gamma_t, K_val;
	scalar R, viscosity, T, Q;
	sasfit_param subParam;
        
	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 4, &R, &viscosity, &T, &Q);

	k = 1.3806505e-23;
	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((viscosity <= 0.0), param, "viscosity(%lg) <= 0",viscosity);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R);
	SASFIT_CHECK_COND((T == 0.0), param, "T == 0");

	if (R == 0) return 0.0;
	D = k*T/(6.0*M_PI*viscosity*1.0e-3*R*1.0e-9);
	Gamma_t = D*Q*Q*1e18*q;

	sasfit_init_param( &subParam );
	subParam.p[0] = R;
	subParam.p[3] = 1.0;
	K_val = sasfit_ff_sphere_f(Q, &subParam);

	return K_val*K_val * exp(-Gamma_t);
	// return K(interp,Q,R,1.0,error) * K(interp, Q, R, 1.0, error) * exp(-Gamma_t);
	/* the factor 2.0 in exponent is needed as (g2(tau)-1) = g1(tau)^2 is calculated here */
}
