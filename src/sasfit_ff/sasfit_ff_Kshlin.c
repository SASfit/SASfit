/*
 * src/sasfit_ff/sasfit_ff_Kshlin.c
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
#include "include/sasfit_ff_utils.h"

/*
float Kshlin(Tcl_Interp *interp,
			 float Q, 
			 float R1, 
			 float DR, 
			 float eta_core, 
			 float eta_shell,
		  float x_in_solvent,
		  float x_out_solvent,
		  float eta_solvent,
			 bool  *error)
*/
/*
 * form factor of a spherical shell with radii R1 and DR
 * contrast profile in shell:
 *		eta_sh = m*R+b
 *		  m = (eta_sh_out-eta_sh_in)/DR
 *	  b = -m*R1+eta_sh_in-eta_solvent
 *		  y-y1 = m*(q-x1)
 *	eta_sh_in : scattering length density at R1
 *  eta_sh_out : scattering length density at R1+DR
 * eta_solvent : scattering length density of solvent
 *x_in_solvent : amount of solvent at R1
 *  x_out_solvent : amount of solvent at R1+DR
 *	eta_shell : scattering length density of pure shell material
 */
scalar sasfit_ff_Kshlin(scalar q, sasfit_param * param)
{
	scalar A, t1, t2, shlin, m, b, eta_sh_in, eta_sh_out;
	scalar R1, Rtot, DR, eta_core, eta_shell, x_in_solvent, x_out_solvent, eta_solvent;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 7, EMPTY, &DR, &eta_core, &eta_shell, &x_in_solvent, &x_out_solvent, &eta_solvent);

	switch( param->kernelSelector )
	{
		case KSHLIN1:
			R1		= param->p[0];
			break;
		case KSHLIN2:
			Rtot		= param->p[0];
			R1 = Rtot-DR;
			break;
		default:
			SASFIT_ERR_UNKNOWN_KERNEL(param, "kernel");
			break;
	}

	eta_sh_in  = (1.0- x_in_solvent)*eta_shell +  x_in_solvent*eta_solvent-eta_solvent;
	eta_sh_out = (1.0-x_out_solvent)*eta_shell + x_out_solvent*eta_solvent-eta_solvent;
		
	A = q*R1;
	shlin = 4.0*M_PI*(eta_core-eta_solvent)*(sin(A)-A*cos(A))/pow(q,3.0);

	if (DR > 0) 
	{
		m = (eta_sh_out-eta_sh_in)/DR;
		b = -m*R1+eta_sh_in;

		t1 = 4.0*M_PI*m*(2.0*cos(A)+2.0*A*sin(A)-A*A*cos(A))/pow(q,4);
		t2 = 4.0*M_PI*b*(sin(A)-A*cos(A))/pow(q,3.0);
		shlin = shlin-t1-t2;

		A = q*(R1+DR);
		t1 = 4.0*M_PI*m*(2.0*cos(A)+2.0*A*sin(A)-A*A*cos(A))/pow(q,4);
		t2 = 4.0*M_PI*b*(sin(A)-A*cos(A))/pow(q,3.0);
		shlin = shlin+t1+t2;
	}

	return shlin;
}

