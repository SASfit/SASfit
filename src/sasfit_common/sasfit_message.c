/*
 * src/sasfit_common/sasfit_message.c
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

#include <stdarg.h>
#include <stdio.h>
#include "include/sasfit_common.h"
#include <gsl/gsl_errno.h>

/// Stores all available output message channels.
static sasfit_msg_chan_t  sasfit_msg_chans[MAX_MSG_TYPES];

void sasfit_set_msg_handler(sasfit_msg_type_e   type, 
                            int                 pass_std, 
                            sasfit_func_msg_t * new_handler)
{
	if ( type < 0 || type >= MAX_MSG_TYPES ) return;

	sasfit_msg_chans[type].pass_std =  (pass_std > 0);
	sasfit_msg_chans[type].msg_func =  new_handler;

	if (type == CHAN_INFO) {
		gsl_set_stream_handler(&sasfit_gsl_stream_handler);
	} 
	else if (type == CHAN_ERR) {
		gsl_set_error_handler(&sasfit_gsl_error_handler);
	}
}

#define CALL_STD_MSG_FUNC(type) \
	do { \
		switch(type) \
		{ \
			case CHAN_ERR:	vfprintf(stderr, format, argptr);  \
					break; \
			case CHAN_INFO:	vfprintf(stdout, format, argptr);  \
					break; \
			default:	break; \
		} \
	} while(0)

void sasfit_msg(sasfit_msg_type_e type, const char * format, ...)
{
	va_list argptr, argptr2;

	if ( !format || type < 0 || type >= MAX_MSG_TYPES ) return;

	if ( sasfit_msg_chans[type].pass_std && sasfit_msg_chans[type].msg_func )
	{
		va_start(argptr, format);
#ifdef va_copy
		va_copy(argptr2, argptr);
#else
		argptr2 = argptr;
#endif
		CALL_STD_MSG_FUNC(type);
		sasfit_msg_chans[type].msg_func(format, argptr2);
		va_end(argptr2);
		va_end(argptr);
	}
	else if ( sasfit_msg_chans[type].pass_std )
	{
		va_start(argptr, format);
		CALL_STD_MSG_FUNC(type);
		va_end(argptr);
	}
	else if ( sasfit_msg_chans[type].msg_func )
	{ 
		va_start(argptr, format);
		sasfit_msg_chans[type].msg_func(format, argptr);
		va_end(argptr);
	}
}

#undef CALL_STD_MSG_FUNC

void sasfit_gsl_stream_handler(const char * label, const char * file, 
                               int line, const char * reason)
{
	sasfit_msg(CHAN_INFO, "gsl: %s:%d: %s: %s\n", file, line, label, reason);
}

