/*
 * src/sasfit_sq/sasfit_sq_SquareWell.c
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


//	  SUBROUTINE SQWELL: 
//          CALCULATES THE STRUCTURE FACTOR FOR A
//			DISPERSION OF MONODISPERSE HARD SPHERES
//	        IN THE Mean Spherical APPROXIMATION ASSUMING 
//          THE SPHERES INTERACT THROUGH A SQUARE WELL POTENTIAL.
//** not the best choice of closure ** see note below
//	  REFS:  SHARMA,SHARMA, PHYSICA 89A,(1977),212
//
//	  
 
// NOTE - depths >1.5kT and volume fractions > 0.08 give UNPHYSICAL RESULTS
// when compared to Monte Carlo simulations

// Input variables are:
//	  [0] radius (req)
//	  [1] volume fraction (phis)
//	  [2] well depth e/kT, dimensionless, +ve depths are attractive (edinkb)
//	  [3] well width, multiples of diameter (lambda)	

#define RHS    param->p[0]
#define ETA    param->p[1]
#define E_KT   param->p[2]
#define LAMBDA param->p[3]

scalar sasfit_sq_SquareWell(scalar q, sasfit_param * param)
{
	scalar sigma, kappa, lkappa, alpha, beta, gamma,Ck;

	SASFIT_ASSERT_PTR( param );

	sigma = 2*RHS;
	alpha = (gsl_pow_2(1.0+2.0*ETA)+gsl_pow_3(ETA)*(ETA-4.0))/gsl_pow_4(1-ETA);
	beta  = -ETA/3.0*(18.0+20.0*ETA-12.0*gsl_pow_2(ETA)+gsl_pow_4(ETA))/gsl_pow_4(1-ETA);
	gamma = ETA*0.5*alpha;
	
	kappa = q*sigma;
	lkappa = kappa*LAMBDA;

	Ck = -24.0*ETA/gsl_pow_6(kappa) *(
			alpha*gsl_pow_3(kappa)*(sin(kappa)-kappa*cos(kappa))
		   +beta*gsl_pow_2(kappa)*(2.0*kappa*sin(kappa)-(gsl_pow_2(kappa)-2.0)*cos(kappa)-2.0)
		   +gamma*(  (4*gsl_pow_3(kappa)-24.0*kappa)*sin(kappa)
		            -(gsl_pow_4(kappa)-12.0*gsl_pow_2(kappa)+24.0)*cos(kappa)+24.0)
		   -E_KT*gsl_pow_3(kappa)*(sin(lkappa)-lkappa*cos(lkappa)
		                  +kappa*cos(kappa)-sin(kappa))
		);
	return 1.0/(1.0-Ck);
}
