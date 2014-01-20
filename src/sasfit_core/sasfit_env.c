/*
 * src/sasfit_core/sasfit_env.c
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
 *   Ingo Bressler (ingo@cs.tu-berlin.de)
 */

#include "include/sasfit_core.h"

// contains a pointer to the Tcl Interpreter by which we were called
sasfit_env_t sasfit_env = { NULL };

void sasfit_env_set_interp(Tcl_Interp * interp)
{
	SASFIT_ASSERT_VOID(interp);
	sasfit_env.interp = interp;
}

Tcl_Interp * sasfit_env_get_interp(void)
{
	return sasfit_env.interp;
}

