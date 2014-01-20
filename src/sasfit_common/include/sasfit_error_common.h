/*
 * src/sasfit_common/include/sasfit_error_common.h
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

#ifndef SASFIT_ERROR_COMMON_H 
#define SASFIT_ERROR_COMMON_H 

#include "sasfit_common_shared_exports.h"

/**
 * \file sasfit_error_common.h
 * \ingroup sasfit_error
 */

/**
 * \defgroup sasfit_error Condition Checking and Error Handling
 * \ingroup sasfit_common
 * Common utilities for error handling and condition checking.
 */

/** 
 * \def SASFIT_ASSERT_VAL
 * \ingroup sasfit_error
 * Checks a single pointer, returns the specified error value on error.
 */
#define SASFIT_ASSERT_VAL(p, retval) \
	do { \
		if ( !(p) ) \
		{ \
			sasfit_err("Assertion of condition '" #p "' failed !\n"); \
			return retval; \
		} \
	} while (0)

/** 
 * \def SASFIT_ASSERT_PTR
 * \ingroup sasfit_error
 * Checks a single pointer, returns \ref SASFIT_RETURNVAL_ON_ERROR on error.
 */
#define SASFIT_ASSERT_PTR(p) SASFIT_ASSERT_VAL((p), SASFIT_RETURNVAL_ON_ERROR)

/** 
 * \def SASFIT_ASSERT_VOID
 * \ingroup sasfit_error
 * Checks a single pointer, returns nothing on error (for functions returning
 * void).
 */
#define SASFIT_ASSERT_VOID(p) SASFIT_ASSERT_VAL((p),)

/**
 * \def SASFIT_CHECK_COND
 * \ingroup sasfit_error
 * Condition verification without message variables.
 * If the supplied condition is TRUE the execution stops and returns with an 
 * error value. The provided message will be printed.
 */
#define SASFIT_CHECK_COND(cond, param, text) \
	do { \
		if ( (cond) ) \
		{ \
			sasfit_param_set_err(param, DBGINFO(SASFIT_ERR_PREFIX text " !\n")); \
			return SASFIT_RETURNVAL_ON_ERROR; \
		} \
	} while (0)

/**
 * \def SASFIT_CHECK_COND1
 * \ingroup sasfit_error
 * Condition verification with one message variables.
 * If the supplied condition is TRUE the execution stops and returns with an 
 * error value. The provided message with the supplied parameter will be 
 * printed.
 */
#define SASFIT_CHECK_COND1(cond, param, text, val) \
	do { \
		if ( (cond) ) \
		{ \
			sasfit_param_set_err(param, DBGINFO(SASFIT_ERR_PREFIX text " !\n"), (val)); \
			return SASFIT_RETURNVAL_ON_ERROR; \
		} \
	} while (0)

/**
 * \def SASFIT_CHECK_COND2
 * \ingroup sasfit_error
 * Condition verification with two message variables.
 * If the supplied condition is TRUE the execution stops and returns with an 
 * error value. The provided message with the supplied parameters will be 
 * printed.
 */
#define SASFIT_CHECK_COND2(cond, param, text, val1, val2) \
	do { \
		if ( (cond) ) \
		{ \
			sasfit_param_set_err(param, DBGINFO(SASFIT_ERR_PREFIX text " !\n"), (val1), (val2)); \
			return SASFIT_RETURNVAL_ON_ERROR; \
		} \
	} while (0)

/**
 * \def SASFIT_CHECK_SUB_ERR
 * \ingroup sasfit_error
 * Error propagation from subroutine to caller.
 * Checks the supplied parameter structure (returning from subroutine) for 
 * errors, copies the error into the second specified parameter structure 
 * and exits the current routine with an error.
 */
#define SASFIT_CHECK_SUB_ERR(param, subParam) \
	do { \
		if ( subParam.errStatus != FALSE ) \
		{ \
			sasfit_param_set_err(param, DBGINFO("Error in subroutine:\n%s\n"), subParam.errStr); \
			return SASFIT_RETURNVAL_ON_ERROR; \
		} \
	} while (0)

/** 
 * \def SASFIT_ERR_UNKNOWN_KERNEL
 * \ingroup sasfit_error
 * Only used in Juelich-Double-CoreShell, marked for removal ... 
 */
#define SASFIT_ERR_UNKNOWN_KERNEL(param, what) \
	sasfit_param_set_err(param, DBGINFO("Unknown " what " specified, set param.kernelSelector before execution !")); \
        return SASFIT_RETURNVAL_ON_ERROR

/**
 * \def SASFIT_EQUAL 
 * \ingroup sasfit_error
 * A boolean expression which decides if two floating point values are equal
 * (regarding a previously defined tolerance).
 */
#define SASFIT_EQUAL(val1, val2) (fabs((val1) - (val2)) < sasfit_eps_get_comp())

// hide direct functions from plugins
#ifndef MAKE_SASFIT_PLUGIN

/**
 * \ingroup sasfit_error
 * Sets the error status in the given parameter structure and outputs a 
 * message.
 * The supplied message is also copied into the parameter structure to enable
 * handling by the caller.
 * \param param  Valid parameter structure from sasfit_function.
 * \param format Formatted message string in \e printf() syntax.
 */
sasfit_common_DLLEXP 
void sasfit_param_set_err(sasfit_param * param, const char * format, ...);

/** 
 * \ingroup sasfit_common_int
 * Forwards GSL error messages into the SASfit error channel.
 * Feed to GSL by sasfit_set_msg_handler().
 * \param reason    Message string.
 * \param file      Source code filename.
 * \param line      Line number in source code file.
 * \param gsl_errno GSL error ID.
 */
void sasfit_gsl_error_handler(const char * reason, const char * file, 
                              int line, int gsl_errno);

#endif // MAKE_SASFIT_PLUGIN
#endif // file

