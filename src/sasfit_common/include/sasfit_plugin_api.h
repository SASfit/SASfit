/*
 * src/sasfit_common/include/sasfit_plugin_api.h
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

#ifndef SASFIT_PLUGIN_API_H
#define SASFIT_PLUGIN_API_H

#include "sasfit_common_shared_exports.h"

/**
 * \file sasfit_plugin_api.h
 * \ingroup sasfit_plugin_api
 */

/**
 * \defgroup sasfit_plugin_api Plugin Framework API
 * \ingroup sasfit_common
 * A set of plugin interface functions and definitions for accessing SASfit plugins,
 * also for automated processing of plugin-API functions.
 */
/*@{*/

/**
 * Plugin interface function to return the version number of the plugin 
 * system a plugin was build with.
 */
typedef int (sasfit_plugin_api_get_ver_t) (void);

/**
 * Plugin interface Function to call for initialization of a plugin.
 * - It expects a storage location for a pointer to a set of model functions
 *   this plugin provides (sasfit_plugin_info_t structure).
 * - It expects a pointer to the lookup table with all functions provided by
 *   \ref sasfit_common (sasfit_common_stubs_t structure).
 * - It gets a pointer to a function (sasfit_plugin_search_t) in \ref sasfit_core
 *   which is used to get all required model functions of other plugins from
 *   the internal plugin database in \ref sasfit_core.
 */
typedef int (sasfit_plugin_api_do_init_t) (const sasfit_plugin_info_t **,
                                           const sasfit_common_stubs_t *,
                                           sasfit_plugin_search_t *);

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
sasfit_common_DLLEXP
void sasfit_plugin_api_reset(sasfit_plugin_api_t * plugin_api);

/**
 * Sets the pointer to an API function loaded from a plugin.
 * \param plugin_api Pointer to a sasfit plugin API record.
 * \param idx Index of the the API function to set.
 * \param func_ptr Pointer to the API function of the plugin.
 * \sa sasfit_plugin_api_t
 */
sasfit_common_DLLEXP
void sasfit_plugin_api_set(sasfit_plugin_api_t * plugin_api, int idx, void * func_ptr);

/**
 * Checks a sasfit plugin API record for completeness after loading all API
 * functions from a plugin.
 * \param plugin_api Contains pointer to the API functions of the currently
 *                   loaded plugin.
 * \returns True, if all plugin API function were loaded successfully and the
 *          plugin system version match with that in the plugin.
 */
sasfit_common_DLLEXP
int sasfit_plugin_api_is_valid(sasfit_plugin_api_t * plugin_api);

/**
 * Prints the contents of a plugin API record.
 * Pointers are printed as hexadecimal numbers.
 * \param plugin_api API record to print.
 */
sasfit_common_DLLEXP
void sasfit_plugin_api_print(sasfit_plugin_api_t * plugin_api);

/*@}*/
#endif // file

