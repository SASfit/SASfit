/*
 * src/sasfit_core/sasfit_plugin_backend.c
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

int sasfit_plugin_search(sasfit_plugin_info_t * requested_set)
{
	int i = 0;
	const sasfit_plugin_func_t * rec_db = 0;
	sasfit_plugin_func_t * rec_req = 0;

	if ( !requested_set ) return 0;

	// for each requested function, search its record in the database
	for(i=0; i < requested_set->num ;i++)
	{
		rec_req = &((requested_set->functions)[i]);
		if ( !rec_req ) continue;

		rec_db = sasfit_plugin_db_get_by_name(rec_req->name, rec_req->len);
		if ( !rec_db || !(rec_db->func) )
		{
			// dependency not found, trying again later (in tcl code)
//			sasfit_out("Could not resolve '%s' while loading plugin!\n",rec_req->name);
			return SASFIT_PLUGIN_DEP_ERR;
		}
		sasfit_plugin_func_copy(rec_req, rec_db);		
	}
	return TRUE;
}

int sasfit_function_select_id(sasfit_function * sf)
{
	int funcid;
	const sasfit_plugin_func_t * func_descr = NULL;

	SASFIT_ASSERT_VAL(sf, TCL_ERROR);
	SASFIT_ASSERT_VAL(sf->typestr, TCL_ERROR);

	if ( Tcl_GetInt(sasfit_env_get_interp(), sf->typestr, &funcid) 
	      != TCL_OK ) 
	{
		return TCL_ERROR;
	}

	func_descr = sasfit_plugin_db_get_by_id(funcid);
	SASFIT_ASSERT_VAL(func_descr, TCL_ERROR);

	sf->fct   = func_descr->func;
	sf->fct_f = func_descr->func_f;
	sf->fct_v = func_descr->func_v;

	return TCL_OK;
}

#define DEP_ERROR_MSG "dependency_error"
#define ASSERT_APPEND_RESULT(result) \
do { \
	if ( result != TCL_OK ) { \
		sasfit_err("Could not append to the result list!\n"); \
		return TCL_ERROR; \
	} \
} while(0)

int add_functions_to_db_and_list(Tcl_Interp * interp, 
		sasfit_plugin_api_t * plugin_api, 
		Tcl_Obj * list, 
		int func_count)
{
	int i = 0, list_len = 0, new_id = 0, res = 0;
	const sasfit_plugin_func_t * plugin_func = 0;
	const sasfit_plugin_info_t * plugin_exp = 0;

	if ( ! interp || ! plugin_api || ! list ) return TCL_ERROR;

	// unload the plugin/library if no valid plugin api was supplied
	if ( ! sasfit_plugin_api_is_valid(plugin_api) )
	{
		sasfit_err("Could not get a valid set of api functions!");
		return TCL_ERROR;
	}

	// init the plugin
	res = plugin_api->do_init_func(&plugin_exp, sasfit_common_stubs_ptr(), &sasfit_plugin_search);
	if ( !res )
	{
		sasfit_err("Could not initialize the plugin!");
		return TCL_ERROR;
	} 
	else if ( res == SASFIT_PLUGIN_DEP_ERR )
	{
		res = Tcl_ListObjAppendElement(interp, list, 
			Tcl_NewStringObj(DEP_ERROR_MSG, sizeof(DEP_ERROR_MSG)-1));
		ASSERT_APPEND_RESULT(res);
		return TCL_OK; // preserves the result list
	}
	else if ( !plugin_exp )
	{
		sasfit_err("Could not initialize the plugin, nothing exported!");
		return TCL_ERROR;
	} 
	
	// add the plugin functions to the database
	for(i=0; i < plugin_exp->num ;i++)
	{
		plugin_func = &(plugin_exp->functions[i]);
		new_id = sasfit_plugin_db_add( plugin_func );
		if ( new_id < 0 )
		{
			sasfit_err("Could not add the %d. function!\n", i+1);
			return TCL_ERROR;
		} else {
			// add function basename and ID to tcl result
			res = Tcl_ListObjAppendElement(interp, list, 
				Tcl_NewStringObj(plugin_func->name, plugin_func->len));
			ASSERT_APPEND_RESULT(res);
			res = Tcl_ListObjAppendElement(interp, list, Tcl_NewIntObj(new_id));
			ASSERT_APPEND_RESULT(res);
		}
	}
	// check if we got the declared number of functions 
	// (determined from header file by tcl code)
	if ( Tcl_ListObjLength(interp, list, &list_len) != TCL_OK ||
		(list_len/2) != func_count )
	{
		sasfit_err("Number of functions in plugin (%d) "
			"don't match those in header file (%d)!\n",(list_len/2), func_count);
		return TCL_ERROR;
	}
	return TCL_OK;
}

int sasfit_load_plugin(Tcl_Interp * interp, 
			Tcl_Obj * filename_obj, 
			int func_count)
{
	int retcode = 0, i = 0;
	const char * filename = 0;
	Tcl_Obj * retlist = 0;
	Tcl_PackageInitProc * proc = 0;
	Tcl_FSUnloadFileProc * unload_proc = 0;
	Tcl_LoadHandle load_handle;
	sasfit_plugin_api_t plugin_api;

	if ( !interp || !filename_obj )
	{
		sasfit_err("One of the supplied arguments is NULL!\n");
		return TCL_ERROR;
	}

	filename = Tcl_GetStringFromObj(filename_obj, 0);

	sasfit_plugin_api_reset(&plugin_api);

	for(i=0; i < SASFIT_PLUGIN_API_NUM ;i++)
	{
		proc = 0;
		retcode = Tcl_FSLoadFile(interp, filename_obj, 
					plugin_api.names[i], 0, &proc, 0, 
					&load_handle, &unload_proc);

		// check for error while loading
		if ( retcode != TCL_OK || !proc ) 
		{
			sasfit_err("Could not load function '%s'!\n%s\n", 
					plugin_api.names[i], 
					Tcl_GetStringResult(interp));

			if ( !proc && unload_proc ) (*unload_proc)(load_handle);
			return TCL_ERROR;
		}
		sasfit_plugin_api_set(&plugin_api, i, (void *) proc);
//		sasfit_plugin_api_print(&plugin_api);
	}

	retlist = Tcl_NewListObj(0, 0);

	if ( add_functions_to_db_and_list(interp, &plugin_api, retlist, func_count) == TCL_ERROR )
	{
		if ( unload_proc ) (*unload_proc)(load_handle);
		Tcl_DecrRefCount(retlist);
		return TCL_ERROR;
	}

	Tcl_SetObjResult(interp, retlist);

	return TCL_OK;
}

int sasfit_load_plugin_cmd(ClientData clientData,
			    Tcl_Interp * interp, 
			    int objc,
			    Tcl_Obj * CONST objv[])
{
	int func_count = 0;
	Tcl_Obj * filename_obj = 0;

	if ( objc < 3 ) return TCL_OK;

	filename_obj = objv[1];
	if ( Tcl_GetIntFromObj(interp, objv[2], &func_count) != TCL_OK )
	{
		sasfit_err("Could not read expected number of functions!\n");
		return TCL_OK;
	}

	// test if file exists, return -1 on error
	if ( Tcl_FSAccess(filename_obj, 0) != 0 )
	{
		sasfit_err("File does not exist: '%s'\n", Tcl_GetStringFromObj(filename_obj, 0));
		return TCL_OK;
	}

	if ( sasfit_load_plugin(interp, filename_obj, func_count) == TCL_ERROR )
	{
		Tcl_ResetResult(interp);
	}

	return TCL_OK;
}

