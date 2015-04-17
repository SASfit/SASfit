/*
 * src/sasfit_common/include/sasfit_common_shared_exports.h
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

#ifndef SASFIT_COMMON_SHARED_EXPORTS_H 
#define SASFIT_COMMON_SHARED_EXPORTS_H 

/**
 * \file sasfit_common_shared_exports.h
 * \ingroup sasfit_shared_exports
 */

/**
 * \defgroup sasfit_shared_exports Shared Library Import/Export Declarations
 * \ingroup sasfit_common_int
 * Defines declarations to import or export functions (symbols)
 * for building as shared library.
 */
/*@{*/

/**
 * \def SASFIT_LIB_IMPORT
 * Platform dependent import declaration for functions/symbols.
 * Set on Windows only (<tt>__declspec(dllimport)</tt>).
 */

/**
 * \def SASFIT_LIB_EXPORT
 * Platform dependent export declaration for functions/symbols.
 * Set on Windows only (<tt>__declspec(dllexport)</tt>).
 */

/**
 * \def sasfit_common_DLLEXP
 * Declaration to import or export a function of this shared library.
 * Set on Windows only.
 * Set this to \ref SASFIT_LIB_IMPORT if you want to use a function of this
 * library in your own code.
 * \sa SASFIT_LIB_IMPORT, SASFIT_LIB_EXPORT
 */

#if defined(STATIC_ENABLED) // no import/export when static
	#define SASFIT_LIB_IMPORT
	#define SASFIT_LIB_EXPORT
#elif defined(_WIN32) || defined(WIN32)
	#include <windows.h>
	#define SASFIT_LIB_IMPORT __declspec(dllimport)
	#define SASFIT_LIB_EXPORT __declspec(dllexport)
#else
	// Unix/linux needs nothing yet
	#define SASFIT_LIB_IMPORT
	#define SASFIT_LIB_EXPORT
#endif // windows

// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_EXPORTS)
	#define sasfit_common_DLLEXP SASFIT_LIB_EXPORT
#else
	#define sasfit_common_DLLEXP SASFIT_LIB_IMPORT
#endif

/*@}*/
#endif // this file
