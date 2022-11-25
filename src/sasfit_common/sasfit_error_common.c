/*
 * src/sasfit_common/sasfit_error_common.c
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

void sasfit_param_set_err(sasfit_param * param, const char* format, ...)
{
	va_list argptr;

	SASFIT_ASSERT_VOID(param);
	SASFIT_ASSERT_VOID(format);

	param->errStatus = TRUE;
   	va_start(argptr, format);
	vsprintf(param->errStr, format, argptr);
	va_end(argptr);
	sasfit_err("%s", param->errStr);
}

void sasfit_gsl_error_handler(const char * reason, const char * file, 
                              int line, int gsl_errno)
{
	sasfit_msg(CHAN_ERR,"gsl: %s:%d: %s: %s (%d:%s)\n",file, line, 
	           "ERROR", reason, gsl_errno, gsl_strerror(gsl_errno));
}
