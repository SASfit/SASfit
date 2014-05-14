/*
 * src/sasfit_common/include/sasfit_plugin.h
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

#ifndef SASFIT_PLUGIN_H
#define SASFIT_PLUGIN_H

#include "sasfit_common_shared_exports.h"

/**
 * \file sasfit_plugin.h
 * \ingroup sasfit_plugin
 */

/**
 * \defgroup sasfit_plugin Plugin Framework
 * \ingroup sasfit_common
 * A framework to help implementing a SASfit compatible plugin.
 */
/*@{*/

/*
 * Macros for plugin implementation below
 */

// DRAFT:
// do_init() gets two arguments function addresses to use for searching 
// other plugin functions, e.g. sasfit_plugin_get_by_name
// first arg: name ptr, 2nd: length of name, 3rd: search in plugins or api
// do_init() then runs sasfit_plugin_get_by_name for each function it requires
// if a name is not found, do_init() returns error -> plugin loading is
// postponed, tried later again when all plugins without dependencies were
// loaded
// sasfit_plugin_get_by_name returns sasfit_plugin_func_t pointer
// plugin stores external pointers in fixed array
// #define REQUIRE(name) { name, NULL, NULL, NULL }
// #define sasfit_ff_sphere   array[3].func
// #define sasfit_ff_sphere_f array[3].func_f
// #define sasfit_ff_sphere_v array[3].func_v
//
// api functions: nonsense, compile time dependent
// enum (or define):
//  F1
//  F2
//  F3
//  ...
//
// array[F1] = ptr
// array[F2] = ptr
// array[F3] = ptr
// ...

/** \def SASFIT_PLUGIN_VERSION
 * Version of this plugin system to ensure compatibility of plugins.
 */
#define SASFIT_PLUGIN_VERSION	101

//
// for export below
//

/** \def SASFIT_PLUGIN_INFO_DECL
 * Declares necessary interface functions and datastructures required 
 * by each plugin to work properly.
 */
#define SASFIT_PLUGIN_INFO_DECL \
/* data structures */ \
const sasfit_common_stubs_t * sasfit_common_stubs_ptr; \
sasfit_plugin_info_t * imp_ptr; \
/* internal functions */ \
void do_at_init(void); \
/* These functions never get imported, they are used for dynamic loading only */ \
SASFIT_LIB_EXPORT sasfit_plugin_api_get_ver_t	get_ver; \
SASFIT_LIB_EXPORT sasfit_plugin_api_do_init_t	do_init

/*
 * for export 
 */

/** \def SASFIT_PLUGIN_EXP_BEGIN
 * Opens a section to mark (model) functions for export.
 * \param count Number of (model) functions to export.
 *              Has to match the number of \ref SASFIT_PLUGIN_EXP_ADD
 *              statements following.
 * \sa \ref SASFIT_PLUGIN_EXP_END, \ref SASFIT_PLUGIN_EXP_ADD
 */
#define SASFIT_PLUGIN_EXP_BEGIN(count) \
static const sasfit_plugin_info_t plugin_export = { (count), {

/** \def SASFIT_PLUGIN_EXP_ADD
 * Marks a (model) function for export.
 * Each (model) function consists of 3 functions in C-code. Assuming
 * <tt>\<basename\></tt> for the basename of all 3 functions, the
 * following C-functions have to be defined (but may have an empty body
 * returning 0.0): \n
 * \code
 * scalar sasfit_<basename>(scalar value, sasfit_param * p);
 * scalar sasfit_<basename>_f(scalar value, sasfit_param * p);
 * scalar sasfit_<basename>_v(scalar value, sasfit_param * p, int distr);
 * \endcode
 * \param name Basename of the (model) function to mark for export.
 * \sa sasfit_plugin_func_t, \ref SASFIT_PLUGIN_EXP_BEGIN, \ref SASFIT_PLUGIN_EXP_END
 */
#define SASFIT_PLUGIN_EXP_ADD(name) \
{ sizeof(#name)-1, #name, &sasfit_ ##name, &sasfit_ ##name ##_f, &sasfit_ ##name ##_v },

/** \def SASFIT_PLUGIN_EXP_END
 * Closes a section for marking (model) functions for export.
 * \sa \ref SASFIT_PLUGIN_EXP_ADD, \ref SASFIT_PLUGIN_EXP_BEGIN
 */
#define SASFIT_PLUGIN_EXP_END \
}};

/** \def SASFIT_PLUGIN_EXP_NONE
 * Disables export of functions.
 * Instead of \ref SASFIT_PLUGIN_EXP_BEGIN , use this if there are no 
 * functions for export.
 * \sa \ref SASFIT_PLUGIN_EXP_ADD, \ref SASFIT_PLUGIN_EXP_END
 */
#define SASFIT_PLUGIN_EXP_NONE \
static sasfit_plugin_info_t plugin_export = { 0 };

/*
 * for import
 */

/** \def SASFIT_PLUGIN_IMP_BEGIN
 * Opens a section to mark (model) functions for import (dependency).
 * \param count Number of (model) functions to import.
 *              Has to match the number of \ref SASFIT_PLUGIN_IMP_ADD
 *              statements following.
 * \sa \ref SASFIT_PLUGIN_IMP_END, \ref SASFIT_PLUGIN_IMP_ADD
 */
#define SASFIT_PLUGIN_IMP_BEGIN(count) \
static sasfit_plugin_info_t plugin_import = { count, {

/** \def SASFIT_PLUGIN_IMP_ADD
 * Marks a (model) function for import (a single dependency).
 * \param name Basename of the (model) function to mark for import.
 * \sa sasfit_plugin_func_t, \ref SASFIT_PLUGIN_IMP_BEGIN, 
 *     \ref SASFIT_PLUGIN_IMP_END, \ref SASFIT_PLUGIN_EXP_ADD
 */
#define SASFIT_PLUGIN_IMP_ADD(name) \
{ sizeof(#name)-1, #name, NULL, NULL, NULL },

/** \def SASFIT_PLUGIN_IMP_END
 * Closes a section for marking (model) functions for import.
 * \sa \ref SASFIT_PLUGIN_IMP_ADD, \ref SASFIT_PLUGIN_IMP_BEGIN
 */
#define SASFIT_PLUGIN_IMP_END \
}};

/** \def SASFIT_PLUGIN_IMP_NONE
 * Disables import of functions of other plugins.
 * Instead of \ref SASFIT_PLUGIN_IMP_BEGIN , use this if there are no 
 * external plugin functions required.
 * \sa \ref SASFIT_PLUGIN_IMP_ADD, \ref SASFIT_PLUGIN_IMP_END
 */
#define SASFIT_PLUGIN_IMP_NONE \
static sasfit_plugin_info_t plugin_import = { 0 };

// interface

/** \def SASFIT_PLUGIN_INTERFACE
 * Inserts interface definition code to a plugins <tt>interface.c</tt> file.
 */
#define SASFIT_PLUGIN_INTERFACE \
const sasfit_common_stubs_t * sasfit_common_stubs_ptr = 0; \
int get_ver(void) \
{ \
	return SASFIT_PLUGIN_VERSION; \
} \
\
int do_init(const sasfit_plugin_info_t ** exp_ptr, \
	sasfit_common_stubs_t * ptr, \
	sasfit_plugin_search_t * searchf) \
{ \
	if ( !exp_ptr || !ptr || !searchf ) return 0; \
	sasfit_common_stubs_ptr = ptr; \
	*exp_ptr = &plugin_export; \
	do_at_init(); \
	imp_ptr = &plugin_import; \
	/* get pointer for model functions we depend on */ \
	/* also decides if we can get loaded successfully */ \
	return searchf(imp_ptr); \
} \


/*
 * Plugin related types
 */

/**
 * Information about a single model function. 
 * Contains information about all provided supplementary C-functions.
 */
typedef struct
{
	int                 len;    //!< Length of the function base name.
	const char *        name;   //!< (Base-)Name of the function.
	sasfit_func_one_t * func;   //!< Pointer to the main function (mandatory).
	sasfit_func_one_t * func_f; //!< Pointer to \<F\> function (non-squared, optional).
	sasfit_func_vol_t * func_v; //!< Pointer to volume function (optional).
} sasfit_plugin_func_t;

/**
 * Information about a plugin.
 * Contains information about all provided model functions.
 */
typedef struct
{
	/// Number of model functions in the plugin.
	int			num;
	/// Array of model function descriptions. \see sasfit_plugin_func_t
	sasfit_plugin_func_t	functions[];
} sasfit_plugin_info_t;

/*
 * plugin utility functions below
 */

/**
 * Set in \ref sasfit_core.
 * \see sasfit_plugin_search
 */
typedef int (sasfit_plugin_search_t) (sasfit_plugin_info_t * requested_set);

/**
 * Copies model function information structure.
 * Does nothing if one of the supplied pointers is \e NULL.
 * \param dst Pointer to a structure sasfit_plugin_func_t where the contents 
 *            of \e src will be written to.
 * \param src Pointer to a structure sasfit_plugin_func_t used as source.
 */
void sasfit_plugin_func_copy(sasfit_plugin_func_t * dst, const sasfit_plugin_func_t * src);

/*
 * plugin interface (API) functions below
 */

/**
 * Plugin interface function to return the version number of the plugin 
 * system a plugin was build with.
 */
typedef int (sasfit_plugin_api_get_ver_t) (void);

/**
 * Plugin interface Function to call for initialization of a plugin.
 * - It passes a pointer to a set of model functions this plugin provides 
 *   (sasfit_plugin_info_t structure).
 * - It gets a pointer to the lookup table with all functions provided by 
 *   \ref sasfit_common (sasfit_common_stubs_t structure).
 * - It gets a pointer to a function (sasfit_plugin_search_t) in \ref sasfit_core 
 *   which is used to get all required model functions of other plugins from 
 *   the internal plugin database in \ref sasfit_core.
 */
typedef int (sasfit_plugin_api_do_init_t) (const sasfit_plugin_info_t **, sasfit_common_stubs_t *, sasfit_plugin_search_t *);

/*@}*/
#endif // file

