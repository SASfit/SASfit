/*
 * src/sasfit_common/include/sasfit_timer.h
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

#ifndef _SASFIT_TIME_H
#define _SASFIT_TIME_H

#include <stdlib.h>
#include "sasfit_common_shared_exports.h"

/** \file sasfit_timer.h
 * \ingroup sasfit_timer
 */

#ifdef WIN32
	#include <windows.h>
	/// Unit for internal representation of time on Windows.
	#define	SASFIT_TIMEUNIT		LARGE_INTEGER
#else
	#include <sys/time.h>
	/// Unit for internal representation of time on Linux.
	#define	SASFIT_TIMEUNIT 	struct timeval
#endif    

/** 
 * \defgroup sasfit_timer Timer
 * \ingroup sasfit_common
 * Timer functionality for measuring execution time. It is supposed to be more
 * precise and reliable on different platforms than the time related functions 
 * supplied by the standard C library. 
 *
 * Example usage:
 * \code
 * #include <sasfit_common.h>
 * sasfit_timer tm = sasfit_timer_create();
 * sasfit_timer_start(tm);
 * ... <some execution> ...
 * printf("Elapsed time: %lg s\n", sasfit_timer_measure(tm));
 * sasfit_timer_destroy(&tm);
 * \endcode
 */
/*@{*/

/**
* Multi-platform timer.
* It is supposed to measure time up to a 10th of a millisecond.
*/
typedef struct
{
	SASFIT_TIMEUNIT	tstart;	//!< Starting time.
#ifdef WIN32
	LARGE_INTEGER	freq;	//!< Processor (?) frequency (Windows).
#else
	struct timezone	tz;	//!< Time-of-the-day (linux/*nix).
#endif
} sasfit_timer;


// hide direct functions from plugins
#ifndef MAKE_SASFIT_PLUGIN

/**
 * Creates this timer.
 * @returns Pointer to properly initialized timer structure.
 */
sasfit_common_DLLEXP sasfit_timer * sasfit_timer_create(void);

/**
 * Destroys this timer (releases memory).
 * @param sf_timer A sasfit_timer structure to release (will be set to null).
 */
sasfit_common_DLLEXP void sasfit_timer_destroy(sasfit_timer ** sf_timer);

/** 
* (Re-)Starts this timer.
* @param sf_timer A sasfit_timer structure.
*/
sasfit_common_DLLEXP void sasfit_timer_start(sasfit_timer * sf_timer);

/**
* Gets the time since last sasfit_timer_start().
* @param sf_timer A sasfit_timer structure.
* @returns Elapsed time in seconds.
*/
sasfit_common_DLLEXP double sasfit_timer_measure(sasfit_timer * sf_timer);

/*@}*/

#endif // MAKE_SASFIT_PLUGIN
#endif // file
