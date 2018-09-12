/*
 * src/sasfit_ff/sasfit_ff_flat_cyl.c
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
#include <sasfit_utils_f2c.h>

/*
float I_flat_Cylinder(Tcl_Interp *interp,
				 float Q,
					  float R,
					  float L,
					  float eta,
					  bool  *error)
					  */
/**
 * form factor of a spherical Cylinder with radius R, height L and scattering
 * length density eta
 */
scalar sasfit_ff_flat_cyl(scalar q, sasfit_param * param)
{
	scalar mu,sigma,G1mu,G1sig,xi,I1sig,I0sig,I1mu,I0mu, Omega, Sum,V;
	scalar R, L, eta;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 4, &R, &L, EMPTY, &eta);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R);
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L);

    V = M_PI*R*R*L;
	if (V == 0.0) return 0.0;

	sigma = L*q;
	mu = 2.0*R*q;
	if (q==0) return V*V*eta*eta;

	G1sig = 2.0/sigma * gsl_sf_bessel_J1(sigma);
	G1mu  = 2.0/mu    * gsl_sf_bessel_J1(mu);

// Expand[Integrate[2*BesselJ[1, x]/x, {x, 0, t}, Assumptions -> t > 0]]
// 2 t BesselJ[0, t] -  2 BesselJ[1, t] + \[Pi] t BesselJ[1, t] StruveH[0, t] - \[Pi] t BesselJ[0, t] StruveH[1, t]

	I1mu  = 2.0*mu*gsl_sf_bessel_J0(mu)-2.0*gsl_sf_bessel_J1(mu)
	       - M_PI*mu*(  gsl_sf_bessel_J0(mu)*sasfit_struve_h1(&mu)
			      - gsl_sf_bessel_J1(mu)*sasfit_struve_h0(&mu)
					 );
	I1sig = 2.0*sigma*gsl_sf_bessel_J0(sigma)-2.0*gsl_sf_bessel_J1(sigma)
	       - M_PI*sigma*(  gsl_sf_bessel_J0(sigma)*sasfit_struve_h1(&sigma)
				 - gsl_sf_bessel_J1(sigma)*sasfit_struve_h0(&sigma)
						);
	I0sig = 0.5 * (I1sig + sigma * G1sig);
	I0mu  = 0.5 * (I1mu  + mu    * G1mu);

	Omega = 2.0/sigma*(I0sig-2.0*gsl_sf_bessel_J1(sigma));
	xi = pow(sin(sigma*0.5)/(0.5*sigma),2.0);

	Sum = 8.0/mu/mu*(xi+I1mu*Omega/mu-G1mu);
	return eta*eta *V*V* Sum;
}

