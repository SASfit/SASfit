/*
 * src/sasfit_common/sasfit_mem.c
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

#include <stdlib.h>
#include "include/sasfit_common.h"

// what is nl and nh for ? i.e. their meaning ?

void sasfit_init_vector(scalar ** vec, int size)
{
	if ( vec == 0 )
	{
		sasfit_err("Supplied vector pointer is NULL !");
		return;
	}
	if ( size < 0 )
	{
		sasfit_err("Supplied size (%f) has to be positive !", size);
		return;
	}
	if ( *vec != 0 )
	{
		sasfit_free_vector(vec);
	}
	else
	{
		if ( size == 0 )
		{
			*vec = 0;
			return;
		}
		*vec = malloc( size * sizeof(scalar) );
	}
}

void sasfit_free_vector(scalar ** vec)
{
	if ( vec == 0 )
	{
		sasfit_err("Supplied vector pointer is NULL !");
		return;
	}
	free( *vec );
	*vec = 0;
}

