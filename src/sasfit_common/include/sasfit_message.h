/*
 * src/sasfit_common/include/sasfit_message.h
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

#ifndef SASFIT_MESSAGE_H 
#define SASFIT_MESSAGE_H 

#include "sasfit_common_shared_exports.h"

/**
 * \file sasfit_message.h
 * \ingroup sasfit_message
 */

/**
 * \defgroup sasfit_message Message Handling
 * \ingroup sasfit_common
 * Common utilities for message output.
 */
/*@{*/

/**
 * \def SASFIT_ERR_PREFIX
 * Default Error message prefix, designed to override before calling 
 * SASFIT_CHECK_* macros.
 */
#ifndef SASFIT_ERR_PREFIX
#define SASFIT_ERR_PREFIX "Error: "
#endif

/**
 * \def DBGINFO
 * Adds debugging information (filename and line number) to the 
 * specified \e printf() formatted string.
 */
#define DBGINFO(text) "%s:%d: " text ,__FILE__,__LINE__

/**
 * \def sasfit_err
 * Outputs a \e printf() formatted message to the error channel (for
 * convenience).
 */
#define sasfit_err(format, ...) sasfit_msg(CHAN_ERR, DBGINFO(format), ##__VA_ARGS__)

/**
 * \def sasfit_out
 * Outputs a \e printf() formatted message to the info channel (for
 * convenience).
 */
#define sasfit_out(format, ...) sasfit_msg(CHAN_INFO, format, ##__VA_ARGS__)
//#define sasfit_err(format, ...) sasfit_error(format, ##__VA_ARGS__)

/**
 * Available channels for output messages.
 */
typedef enum
{
	CHAN_ERR=0,     //!< Equivalent to stderr.
	CHAN_INFO=1,    //!< Equivalent to stdout.
	MAX_MSG_TYPES=2	//!< Number of available channels.
} sasfit_msg_type_e;

/**
 * Stores function references for alternate output (printf-like).
 */
typedef struct
{
	int                 pass_std; //!< If true, pass messages also to std channels (stdout,stderr).
	sasfit_func_msg_t * msg_func; //!< Custom function to pass output messages to.
} sasfit_msg_chan_t;


// hide direct functions from plugins
#ifndef MAKE_SASFIT_PLUGIN

/**
 * Configures the output behaviour of a given channel.
 * \param type     Output channel to configure.
 * \param pass_std If true, pass messages also to std channels 
 *                 (e.g. stdout or stderr).
 * \param msg_func Custom function to pass messages of this channel to.
 * \sa sasfit_alt_msg_t
 */
sasfit_common_DLLEXP 
void sasfit_set_msg_handler(sasfit_msg_type_e   type,
                            int                 pass_std,
                            sasfit_func_msg_t * msg_func);

/**
 * Prints an informational message.
 * If set, it passes the message also to the function specified with 
 * sasfit_msg_handler() for the given type. Printing to the info channel is 
 * supposed to be faster than the error channel because no update of the TCL 
 * result occurs.
 * \param type   The channel type to print the message to
 * \param format Format string of ouput message in default \e printf() syntax.
 */
sasfit_common_DLLEXP void sasfit_msg(sasfit_msg_type_e type, 
                                     const char * format, ...);

/** \ingroup sasfit_common_int
 * Forwards output messages of the GSL library into the SASfit output channel.
 * Feed to GSL by sasfit_set_msg_handler().
 * \param label  Message prefix.
 * \param file   Source code filename.
 * \param line   Line number in source code file.
 * \param reason Message string.
 */
void sasfit_gsl_stream_handler(const char * label, const char * file, 
                               int line, const char * reason);

/*@}*/
#endif // MAKE_SASFIT_PLUGIN
#endif // file

