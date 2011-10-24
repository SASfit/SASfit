/*
 * src/sasfit_sq/sasfit_sq_MCT.c
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
#include "include/sasfit_sq_utils.h"

/*
float S_MCT(Tcl_Interp *interp,
	 float Q,
	float N,
	float d,
	 float eta,
	 float Nu,
	 float sigma_d,
	bool  *error)
*/

/**
 * Structure factor fot membrane stacks descibed by modified Caill&eacute; theory
 * Q:  scattering vector
 * N:  total umber of layers in a single domain
 * d:  lattice constant
 * Nu: scaling constant for an additional diffuse scattering term
 * eta: Caill&eacute; parameter, which is a measure for the bilayer fluctuatiuons
 */
scalar sasfit_sq_MCT(scalar q, sasfit_param * param)
{
	scalar PD, NormPD, Dd, P1, PN, SQ_MCT, sigma, xi;
	int k,m,Ni,NN,nsf,dnsf;
	scalar N, d, eta, Nu, sigma_d;


	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 5, &N, &d, &eta, &Nu, &sigma_d);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((N < 2.0), param, "N(%lg) < 2",N);
	SASFIT_CHECK_COND1((d <= 0.0), param, "d(%lg) <= 0",d);
	SASFIT_CHECK_COND1((eta < 0.0), param, "eta(%lg) < 0",eta);
	SASFIT_CHECK_COND1((Nu < 0.0), param, "Nu(%lg) < 0",Nu);

	SQ_MCT = 0.0;
	PN = 0.0;
	NN = (int) N;
	if (N >= 5) 
	{
		sigma = sqrt(N);
	} else {
		sigma = 0.5*(N-1.0);
	}
	nsf =  (int) ceil(30.0/log(N));
	dnsf = (int) ceil(4.0*sigma/nsf);

	for (Ni=(int)(N-2.0*sigma); Ni <= (int)(N+2.0*sigma) ;Ni++) 
	{
		P1 = Ni;
		for (k=1;k<Ni;k++) 
		{
			NormPD = 0.0;
			PD = 0.0;
			if (sigma_d == 0) 
			{
				PD = cos(k*q*d) * exp(-pow(q*d/2/M_PI,2.0)*eta*(M_EULER+log(M_PI*k)));
		  	} else {
				for (m=0;m<30;m++) 
				{
					Dd = d-2*sigma_d+4.0*sigma_d/30.0*m;
					NormPD = NormPD+exp(-pow(d-Dd,2.0)/(2.0*pow(sigma_d,2.0)));
					PD = PD + cos(k*q*Dd) * exp(-pow(q*Dd/2/M_PI,2.0)*eta*(M_EULER+log(M_PI*k)))
						* exp(-pow(d-Dd,2.0)/(2.0*pow(sigma_d,2.0)));
			  	}
				PD = PD/NormPD;
			}
			P1 = P1+2.0*(Ni-k)*PD;
		}
		xi = exp(-pow(Ni-N,2.0)/pow(2.0*sigma,2.0))/(sigma*sqrt(2.0*M_PI));
		PN = PN+xi;
		SQ_MCT = SQ_MCT + P1*xi;
	}
	return SQ_MCT/PN+Nu;
}

