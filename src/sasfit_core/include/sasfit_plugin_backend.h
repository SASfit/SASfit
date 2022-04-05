/*
 * src/sasfit_core/include/sasfit_plugin_backend.h
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

#ifndef SASFIT_PLUGIN_BACKEND_H
#define SASFIT_PLUGIN_BACKEND_H

#include "sasfit_core_shared_exports.h"

/**
 * \file sasfit_plugin_backend.h
 * Additional datastructures and functions for loading and
 * managing plugins.
 */

/**
 * \def SASFIT_PLUGIN_API_NUM
 * The number of plugin framework functions a plugin has to provide.
 */
#define SASFIT_PLUGIN_API_NUM	2

/**
 * \def SASFIT_PLUGIN_DEP_ERR
 * Return code if a dependency for a plugin was not found while loading it.
 * Some arbitrary negative number.
 */
#define SASFIT_PLUGIN_DEP_ERR	-15

/*
 * API related, for automated processing of plugin-API functions
 */

/**
 * A set of functions and their names which are mandatory by each plugin.
 */
typedef struct
{
	/// Function for version information.
	sasfit_plugin_api_get_ver_t *	get_ver_func;
	/// Function for proper plugin initialization.
	sasfit_plugin_api_do_init_t *	do_init_func;
	/// Names of the above functions in the plugin
	/// (typically \e get_ver and \e do_init ).
	const char *	names[SASFIT_PLUGIN_API_NUM];
} sasfit_plugin_api_t;

/**
 * Resets a sasfit_plugin_api_t structure to default values.
 * \param plugin_api Pointer to a sasfit plugin API record.
 */
sasfit_core_DLLEXP
void sasfit_plugin_api_reset(sasfit_plugin_api_t * plugin_api);

/**
 * Sets the pointer to an API function loaded from a plugin.
 * \param plugin_api Pointer to a sasfit plugin API record.
 * \param idx Index of the the API function to set.
 * \param func_ptr Pointer to the API function of the plugin.
 * \sa sasfit_plugin_api_t
 */
sasfit_core_DLLEXP
void sasfit_plugin_api_set(sasfit_plugin_api_t * plugin_api, int idx, void * func_ptr);

/**
 * Checks a sasfit plugin API record for completeness after loading all API
 * functions from a plugin.
 * \param plugin_api Contains pointer to the API functions of the currently
 *                   loaded plugin.
 * \returns True, if all plugin API function were loaded successfully and the
 *          plugin system version match with that in the plugin.
 */
sasfit_core_DLLEXP
int sasfit_plugin_api_is_valid(sasfit_plugin_api_t * plugin_api);

/**
 * Prints the contents of a plugin API record.
 * Pointers are printed as hexadecimal numbers.
 * \param plugin_api API record to print.
 */
sasfit_core_DLLEXP
void sasfit_plugin_api_print(sasfit_plugin_api_t * plugin_api);

/*
 * Database (DB) of all loaded model functions.
 */

/**
 * Information about all loaded model functions (backend database).
 */
typedef struct
{
	/// Maximum number of function structures
	/// (always smaller than \e last )
	int			size;
	/// Index of the last element.
	int			last;
	/// Pointer to the head of the array of function structures.
	sasfit_plugin_func_t *	ptr;
} sasfit_plugin_db_t;

/**
 * Checks if the model function DB is properly initialized and safe for access.
 * \returns True(1), if initialized and not full. False(0) otherwise.
 */
int sasfit_plugin_db_is_valid(void);

/**
 * Retrieves a single model function record from the database by its identifier.
 * Constant execution time complexity.
 * \param id Identifier of the model function to retrieve.
 * \returns Pointer to the information record of the requested model
 * function. NULL, if the supplied ID is invalid or another error occured.
 */
const sasfit_plugin_func_t * sasfit_plugin_db_get_by_id(int id);

/**
 * Retrieves a single model function record from the DB by its basename.
 * Linear execution time complexity (over all functions in the DB).
 * \param name (Base)Name of the model function to retrieve.
 * \param len  Length of the basename (without trailing zero characters).
 * \returns Pointer to the information record of the requested model
 * function. NULL, if the supplied name was not found or another error occured.
 */
const sasfit_plugin_func_t * sasfit_plugin_db_get_by_name(const char * name, int len);

/**
 * Adds a new model function to the database.
 * \param source Valid info structure of a model function to add.
 * \returns A unique identifier of the successful added model function.<br>
 *          -1, if the function could not be added or another error occured.
 */
int sasfit_plugin_db_add(const sasfit_plugin_func_t * source);

/**
 * Deletes the function database and frees its memory.
 */
void sasfit_plugin_db_delete(void);

/*
 * interface of plugin system to other code in sasfit_core
 */

/**
 * Adds all model functions of a plugin to the database.
 * After adding a model function to the database successfully, its basename
 * and unique identifier is appended to the list object \e list.
 * \param interp Tcl Interpreter we were initially called by.
 * \param plugin_api A record, containing a pointer to API functions of the
 *                   plugin to load.
 * \param list Tcl object containing pairs of model function basename and
 *             unique identifier within the database.
 * \param func_count Number of functions to add.
 * \returns TCL_ERROR on serious error, TCL_OK otherwise.
 */
int add_functions_to_db_and_list(Tcl_Interp *          interp,
                                 sasfit_plugin_api_t * plugin_api,
                                 Tcl_Obj *             list,
                                 int                   func_count);

/**
 * Retrieves all requested model function dependecies from the internal
 * plugin database.
 * \param requested_set A set of information records of model functions
 * to retrieve from the internal database. The basename if each requested
 * function has to be set. Pointer to the according functions will be
 * overwritten.
 * \returns 0, if the \e requested_set is invalid.<br>
 *          1, if all requested functions were found.<br>
 *          \ref SASFIT_PLUGIN_DEP_ERR, if at least one requested function
 *          was not found.
 */
sasfit_core_DLLEXP
sasfit_plugin_search_t sasfit_plugin_search;

/**
 * Loads a single plugin.
 * When done a list of model function basenames and identifiers is written to
 * the Tcl Interpreters result (so it can be accessed in tcl code).
 * \param interp Tcl Interpreter we were called by initially.
 * \param filename_obj Tcl object containing the filename of the plugin to load.
 * \param func_count The exact number of model functions this plugin has to provide.
 * \returns TCL_OK on success, TCL_ERROR on error.
 */
int sasfit_load_plugin(Tcl_Interp * interp,
                       Tcl_Obj *    filename_obj,
                       int          func_count);

/**
 * Tcl command \e sasfit_load_plugin.
 * \param clientData Not used, see the Tcl documentation for \e Tcl_ObjCmdProc.
 * \param interp Tcl Interpreter we were called by initially.
 * \param objc Number of arguments supplied to this Tcl command.
 * \param objv Pointer to the list of supplied arguments (Tcl objects).
 * \sa sasfit_load_plugin
 */
int sasfit_load_plugin_cmd(ClientData   clientData,
                           Tcl_Interp * interp,
                           int          objc,
                           Tcl_Obj *    CONST objv[]);

#endif

