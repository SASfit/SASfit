/*
 * src/sasfit_core/sasfit_plugin_db.c
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
 *   Ingo Bressler (ingo.bressler@bam.de)
 */

#include <stdlib.h>
#include <string.h>
#include <sasfit_common.h>
#include <sasfit_plugin.h>
#include "include/sasfit_core.h"
#include "include/sasfit_plugin_backend.h"

// head structure of central plugin DB
static sasfit_plugin_db_t	sasfit_plugin_db = {0, -1, NULL};

int sasfit_plugin_db_is_valid(void)
{
	return ( sasfit_plugin_db.size > 0 &&
		sasfit_plugin_db.last >= 0 &&
		sasfit_plugin_db.last < sasfit_plugin_db.size &&
		sasfit_plugin_db.ptr != NULL );
}

const sasfit_plugin_func_t * sasfit_plugin_db_get_by_id(int id)
{
	if ( !sasfit_plugin_db_is_valid() ||
	     id < 0 ||
	     id > sasfit_plugin_db.last )	return NULL;

	return &( (sasfit_plugin_db.ptr)[id] );
}

const sasfit_plugin_func_t * sasfit_plugin_db_get_by_name(const char * name, int len)
{
	int i;
	sasfit_plugin_func_t * cur_func = NULL;

	if ( !sasfit_plugin_db_is_valid() ||
	     !name )	return NULL;

	// do linear search to find the requested function
	for(i=0; i <= sasfit_plugin_db.last ;i++)
	{
		cur_func = &((sasfit_plugin_db.ptr)[i]);
		if ( cur_func &&
		     cur_func->len == len &&
		     strncmp(cur_func->name, name, len) == 0 )
		{
			break;
		}
		cur_func = NULL;
	}

	return cur_func;
}

int sasfit_plugin_db_add(const sasfit_plugin_func_t * source)
{
	void * ptr = 0;
	int new_size = 0;
	int id = 0;
	if ( ! source ) return -1;

	id = sasfit_plugin_db.last + 1;

	// database is full, increase its size
	if ( id == sasfit_plugin_db.size )
	{
		// is empty
		if ( sasfit_plugin_db.size == 0 && id == 0)
		{
			new_size = 20; // some kind of init size
		} else {
			new_size = sasfit_plugin_db.size*2; // double the size
		}
		// try to allocate more memory
		ptr = realloc(sasfit_plugin_db.ptr,
				sizeof(sasfit_plugin_func_t) * new_size);
		// allocation failed ?
		if ( ! ptr ) return -1;

		sasfit_plugin_db.ptr = (sasfit_plugin_func_t *) ptr;
		sasfit_plugin_db.size = new_size;
	}

	// quick check of validity and non-empty name
	if ( ! source->func ||
	       source->len <= 0 ||
	     ! source->name ||
	     (source->name)[0] == '\0') return -1;

	sasfit_plugin_func_copy(&(sasfit_plugin_db.ptr[id]), source);
	sasfit_plugin_db.last = id;

	return id;
}

void sasfit_plugin_db_delete(void)
{
	if ( sasfit_plugin_db.ptr != 0 )
	{
		free( sasfit_plugin_db.ptr );
	}
	sasfit_plugin_db.size = 0;
	sasfit_plugin_db.last = -1;
	sasfit_plugin_db.ptr = 0;
}

