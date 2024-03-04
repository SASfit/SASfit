/*
 * src/sasfit_ff/sasfit_ff_SphereWithGaussChains.c
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
#include <gsl/gsl_sf.h>
#include "include/sasfit_ff_utils.h"

/*
float SphereWithGaussChains(Tcl_Interp *interp,
		float Q,
		  float R,	  // radius of the spherical core
		float Rg,	 // radius of gyration of polymer chains in shell
		float d,	  // correction factor should be chosen close to 1
		float Nagg,  // aggregation number
		float rc,	 // excess scattering of chain (corona)
		float rs,	 // excess scattering length of the part in the spherical core
		  bool  *error)
*/
/**
 * form factor of a sphere with radius R and and gaussian chains attached
 */
scalar sasfit_ff_SphereWithGaussChains(scalar q, sasfit_param * param)
{
	scalar PHI,Fs,Fc,Scc,Ssc,I,w,sinc_x;
	scalar R, Rg, d, Nagg, rc, rs;
	scalar u;

        SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 6, &R, &Rg, &d, &Nagg, &rc, &rs);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R);
	SASFIT_CHECK_COND1((Rg < 0.0), param, "Rg(%lg) < 0",Rg);


	u = q*R;
    if (fabs(u)<1e-4) {
        PHI = (1 - gsl_pow_2(u)/10. + gsl_pow_4(u)/280. - gsl_pow_6(u)/15120.);
    } else {
        PHI = 3*(sin(u)-u*cos(u))/gsl_pow_3(u);
    }

	Fs = PHI*PHI;
	w = sasfit_rwbrush_w(q, Rg);
	Fc = sasfit_gauss_fc(q, Rg);

	sinc_x=gsl_sf_bessel_j0(q * (R + d * Rg));
	Scc = gsl_pow_2(w*sinc_x);
	Ssc = PHI * w * sinc_x;

	I = Nagg*Nagg*rs*rs*Fs
		+ Nagg*rc*rc*Fc
		+ Nagg*(Nagg-1)*((Nagg < 1) ?  0 : 1)*rc*rc*Scc
		+ 2*Nagg*Nagg*rs*rc*Ssc;
	return I;
}

