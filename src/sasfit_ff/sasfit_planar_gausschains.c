/*
 * src/sasfit_ff/sasfit_planar_gausschains.c
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
float planar_GaussChains(Tcl_Interp *interp,
	float Q,
	float L,     // thickness of central core
	float Rg,    // radius of gyration of polymer chains in shell
	float d,     // correction factor should be chosen close to 1
	float Nagg,  // aggregation number
	float rc,    // excess scattering of chain (corona)
	float rs,    // excess scattering length of the part in the spherical core
	bool  *error)
*/
/**
 * form factor of a sphere with radius R and and gaussian chains attached
 */
scalar sasfit_planar_gauss_chains(scalar q, sasfit_param * param)
{
	scalar Fs, Fc, Fcs, Scc, Ssc, I, w, L, Rg, d, Nagg, rc, rs,Pprime;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 7, &L, &Rg, &d, &Nagg, &rc, &rs,&Pprime);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L);
	SASFIT_CHECK_COND1((Rg < 0.0), param, "Rg(%lg) < 0",Rg);

	Fc = sasfit_gauss_fc(q, Rg);
	w = sasfit_rwbrush_w(q, Rg);
	if (q * L/2. == 0)
	{
		Fs = 1.0;
	} else {
		Fs = sin(q * L/2.) / (q * L/2.);
	}
	Fcs = Fs*Fs;
	Scc = pow(w*cos(q*((L/2.+d*Rg))),2.);
	Ssc = w * Fs *cos(q*((L/2.+d*Rg)));

	I =   Pprime*Nagg*Nagg*rs*rs*Fcs
		+        Nagg*rc*rc*Fc
		+ Pprime*Nagg*(Nagg-1.)*((Nagg < 1.) ?  0 : 1)*rc*rc*Scc
		+ Pprime*2.*Nagg*Nagg*rs*rc*Ssc;
	return I;
}

