/*
 * src/sasfit_common/sasfit_plugin_func.c
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

#include "include/sasfit_common.h"
#include "include/sasfit_plugin.h"

void sasfit_plugin_func_copy(sasfit_plugin_func_t * dst, const sasfit_plugin_func_t * src)
{
	if ( !dst || !src ) return;
	dst->len = 	src->len;
	dst->name = 	src->name;
	dst->func = 	src->func;
	dst->func_f = 	src->func_f;
	dst->func_v = 	src->func_v;
}

