/*
 * src/sasfit_sq/sasfit_sq_SquareWell2.c
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
double S_SquareWell2(Tcl_Interp *interp,
			double Q,
			double RHS,
			double eta,
			double epsi_div_kB,
			double Delta,
			double T,
			bool  *error)
*/

/**
 * structure factor of particles interacting with a square well potential
 * RHS: hards sphere radius
 * rho: colloid concentration
 * epsilon: square well depth
 * Delta: square well width
 * T: Temperature in Kelvin
 * [Percus-Yevick approximation]
 */
scalar sasfit_sq_SquareWell2(scalar q, sasfit_param * param)
{
	scalar sigma,sigma3,kappa,kappa3,kb,rho,epsilon;
	scalar RHS, eta, epsi_div_kB, Delta, T;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 5, &RHS, &eta, &epsi_div_kB, &Delta, &T);

	kb = 1.3806505E-23;
	epsilon = epsi_div_kB*kb;
	sigma = 2.*RHS;
	sigma3 = sigma*sigma*sigma;
	rho = eta*6./M_PI/sigma3;

	kappa = sigma*q;
	kappa3=kappa*kappa*kappa;
	return 1.-4.*M_PI*rho*sigma3*(sin(kappa)-kappa*cos(kappa))/kappa3
		  +4.*M_PI*rho*sigma*sigma*(exp(epsilon/(kb*T))-1.)*sin(kappa)/kappa*Delta;
}

