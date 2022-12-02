/*
 * src/sasfit_ff/sasfit_ff_MLVesicleFrielinghaus.c
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
#include <sasfit_utils_f2c.h>


/*
float MLVesicleFrielinghaus(Tcl_Interp *interp, 
							float Q,
							float n,
							float nw,
							float rho_sol,
							float rho_core,
							float rho_shell,
							float sig_core,
							float sig_shell,
							float Rmain,
							float zz,
							float Rshift,
							bool *error)
*/
scalar sasfit_ff_MLVesicleFrielinghaus(scalar q, sasfit_param * param)
{
	static integer npar;
	static char thnam[8];
	static char parnam[8*20];
	static integer ini;
	static real mlv;
	real Q = (real)q;

	SASFIT_ASSERT_PTR(param);
/*
	pa[1] = 1.0e20; // phi = 1;
	pa[2] = n;
	pa[3] = nw;
	pa[4] = rho_sol;
	pa[5] = rho_core;
	pa[6] = rho_shell;
	pa[7] = sig_core;
	pa[8] = sig_shell;
	pa[9] = Rmain;
	pa[10] = zz;
	pa[11] = Rshift;
	pa[12] = 0.0; // bckgr
*/

	param->p[MAXPAR-2] = 1.0e20;	// phi = 1
	param->p[MAXPAR-1] = 0.0;	// background

	ini = 0;
	mlv = (real)sasfit_th36(&Q, param, thnam, parnam, &npar, &ini, 8L, 8L);
	ini = 1;

	npar = 12;					
/*
		  parnam(1) = 'phi'	! Concentration Polymer
		  parnam(2) = 'n'	  ! Number of layers
		  parnam(3) = 'nw'	 ! width of n-distribution
		  parnam(4) = 'rhoW'  ! Scattering length density water
		  parnam(5) = 'rho1'  ! Scattering length density 'core'
		  parnam(6) = 'rho2'  ! Scattering length density 'shell'
		  parnam(7) = 'sig1'  ! Thickness 'core'
		  parnam(8) = 'sig2'  ! Thickness 'shell' 
		  parnam(9) = 'Rmain' ! Main Radius (distance between layers)
		  parnam(10)= 'zz'	 ! Schultz Z
		  parnam(11)= 'Rshift'! Maximum possible shift
		  parnam(12)= 'bckgr' ! background
 */
	mlv = (real)sasfit_th36(&Q, param, thnam, parnam, &npar, &ini, 8L, 8L);

	return mlv;
}

