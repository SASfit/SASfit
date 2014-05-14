/*
 * src/sasfit_sq/sasfit_sq_TD.c
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

/**
 * Structure factor of weakly orderd membrane stacks
 * weakly orderd due to thermal disorder
 * Q:  scattering vector
 * N:  total umber of layers in a single domain
 * d:  lattice constant
 * Nu: scaling constant for an additional diffuse scattering term
 * Delta: Debye-Waller temperature factor Delta = <(d_k-d)^2>
 */
scalar sasfit_sq_TD(scalar q, sasfit_param * param)
{
	scalar P1, P2, PN, SQ_TD, sigma, nsf, xi, NormPD, PD, Dd;
	int m, k,Ni,NN,dnsf;
	scalar N, d, Delta, Nu, sigma_d;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 5, &N, &d, &Delta, &Nu, &sigma_d);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((N < 2.0), param, "N(%lg) < 2",N);
	SASFIT_CHECK_COND1((d <= 0.0), param, "d(%lg) <= 0",d);
	SASFIT_CHECK_COND1((Delta < 0.0), param, "Delta(%lg) < 0",Delta);
	SASFIT_CHECK_COND1((Nu < 0.0), param, "Nu(%lg) < 0",Nu);

	NN = (int) N;
	if (N >= 5) 
	{
		sigma = sqrt(N);
	} else {
		sigma = 0.5*(N-1.0);
	}
	nsf =  (int) ceil(30.0/log(N));
	dnsf = (int) ceil(4.0*sigma/nsf);

	P2 = 2.0*exp(-q*q*Delta*Delta/2.0);
	SQ_TD = 0.0;
	PN = 0.0;

	for (Ni=(int)(N-2.0*sigma); Ni <= (int)(N+2.0*sigma) ;Ni++) 
	{
		P1 = Ni;
		for (k=1; k < Ni ;k++)
		{
			PD = 0.0;
			if (sigma_d == 0)
			{
				PD = P2*(Ni-k)*cos(k*q*d);
		  	} else {
				NormPD = 0.0;
				for (m=0; m < 30 ;m++)
				{
					Dd = d-2*sigma_d+4.0*sigma_d/30.0*m;
					NormPD = NormPD+exp(-pow(d-Dd,2.0)/(2.0*pow(sigma_d,2.0)));
					PD = PD + P2*(Ni-k)*cos(k*q*Dd)
						* exp(-pow(d-Dd,2.0)/(2.0*pow(sigma_d,2.0)));
				}
				PD = PD/NormPD;
			}
			P1 = P1+PD;
		}
		xi = exp(-pow(Ni-N,2.0)/pow(2.0*sigma,2.0))/(sigma*sqrt(2.0*M_PI));
		PN = PN+xi;
		SQ_TD = SQ_TD + P1*xi;
	}
	return SQ_TD/PN+Nu;
}

