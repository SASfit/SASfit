/*
 * src/sasfit_ff/sasfit_ff_long_cyl.c
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

/*
float I_long_Cylinder(Tcl_Interp *interp,
				 float q,
					  float R,
					  float L,
					  float eta,
					  bool  *error)
*/
/*
 * form factor of a spherical Cylinder with radius R, height L and scattering
 * length density eta
 */
scalar sasfit_ff_long_cyl(scalar q, sasfit_param * param)
{
	scalar mu, sigma, pi_mu, G1, G2, I_sp, Sum, V, omega;
	scalar R, L, eta;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 4, &R, &L, EMPTY, &eta);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R);
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L);

	if (R == 0.0) return 0.0;
	if (L == 0.0) return 0.0;

	mu = L*q;
	sigma = 2.0*R*q;
	V = M_PI*R*R*L;

	if (R==0 || L==0) return 0;
	if (q==0) return V*V*eta*eta;

	pi_mu = gsl_sf_Si(mu)+cos(mu)/mu+sin(mu)/mu/mu;
	G1 = 2.0/(0.5*sigma) *  gsl_sf_bessel_J1(0.5*sigma);
	G2 = 8.0/sigma/sigma * gsl_sf_bessel_Jn(2,sigma);
//	I_sp = 3.0 * (sin(sigma*0.5)-0.5*sigma*cos(0.5*sigma)) / pow(sigma/2.0,3);
//	I_sp = I_sp*I_sp;
	omega = 8/gsl_pow_2(sigma)*(3*gsl_sf_bessel_Jn(2,sigma)+gsl_sf_bessel_J0(sigma)-1);

//	Sum = 2.0/mu * (pi_mu*G1*G1 - 1.0/mu*(2.0*G2-I_sp) - sin(mu)/mu/mu);
	Sum = 2.0/mu * (pi_mu*G1*G1 - omega/mu - sin(mu)/mu/mu);

	return eta*eta *V*V* Sum;
}

