/*
 * src/sasfit_core/sasfit_plugin_api.c
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

#include <stdlib.h>
#include <string.h>
#include <sasfit_common.h>
#include <sasfit_plugin.h>
#include "include/sasfit_core.h"
#include "include/sasfit_plugin_backend.h"

// clean function pointer/references
void sasfit_plugin_api_reset(sasfit_plugin_api_t * plugin_api)
{
	int i=0;
	if ( !plugin_api ) return;

	plugin_api->names[i++] = "get_ver";
	plugin_api->names[i++] = "do_init";
	plugin_api->get_ver_func = 0;
	plugin_api->do_init_func = 0;
}

void sasfit_plugin_api_set(sasfit_plugin_api_t * plugin_api, int idx, void * func_ptr)
{
	if ( !plugin_api || 
		idx < 0 || 
		idx > SASFIT_PLUGIN_API_NUM-1 )
	{
		return;
	}
	switch(idx)
	{
		case 0:	plugin_api->get_ver_func = 
				(sasfit_plugin_api_get_ver_t*) func_ptr;
			break;
		case 1:	plugin_api->do_init_func = 
				(sasfit_plugin_api_do_init_t*) func_ptr;
			break;
		default:break;
	}
}

int sasfit_plugin_api_is_valid(sasfit_plugin_api_t * plugin_api)
{
	int version = 0;

	if ( !plugin_api ) return 0;
	if ( (version = plugin_api->get_ver_func()) != SASFIT_PLUGIN_VERSION )
	{
		sasfit_err("Wrong version (%d), expected '%d'!\n",version,SASFIT_PLUGIN_VERSION);
		return 0;
	}
	return (plugin_api->do_init_func != 0);
}

void sasfit_plugin_api_print(sasfit_plugin_api_t * plugin_api)
{
	int i=0;
	if ( !plugin_api ) return;
	for(; i < SASFIT_PLUGIN_API_NUM ;i++)
	{
		sasfit_out("%s [%x]\n", plugin_api->names[i], ((int**)plugin_api)[i] );
	}
}

