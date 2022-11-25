/*
 * src/sasfit_core/sasfit_analytpar.c
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

#include "include/sasfit_core.h"

void sasfit_init_AP(sasfit_analytpar ** AParr, int num)
{
	int i;

	if( num < 1 )
	{
		sasfit_err("number of AParrs is less than 1 !");
		return;
	}
	if( AParr == 0 )
	{
		sasfit_err("Target pointer is 0 !");
		return;
	}
	if( *AParr != 0 )
	{
		sasfit_free_AP(AParr);
	}
	*AParr = (sasfit_analytpar *) Tcl_Alloc(sizeof(sasfit_analytpar) * num );

	if( *AParr == 0 )
	{
		sasfit_err("Allocation of AParrs failed !");
		return;
	}

	// initialize sasfit_function, fill them with well defined values
	for(i=0; i < num ;i++)
	{
		sasfit_init_fct( &((*AParr)[i].FF) );
		sasfit_init_fct( &((*AParr)[i].SD) );
		sasfit_init_fct( &((*AParr)[i].SQ) );
	}
}

void sasfit_free_AP(sasfit_analytpar ** AParr)
{
	if( AParr == 0 )
	{
		sasfit_err("Target pointer is 0 !");
		return;
	}
	if( *AParr == 0 )
	{
		sasfit_err("supplied AParr is already 0 !");
		return;
	}

	Tcl_Free((char*) *AParr);

	*AParr = 0;
}

