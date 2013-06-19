/*
 * src/sasfit_core/sasfit_error_core.c
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

#include <string.h>
#include "include/sasfit_core.h"

#define MAX_LEN 1024
#define SASFIT_TCL_PUTS_ERR_CMD "puts -nonewline stderr \"error in: "
#define SASFIT_TCL_PUTS_OUT_CMD "puts -nonewline stdout \""
#define ERROR_RESULT_HEAD "SASFIT run-time error...\n"

#define SASFIT_TCL_PUTS(tclcmd, setresult) \
	Tcl_Obj * res_obj = 0; \
	long unsigned int ret = 0; \
	long unsigned int prefix = strlen((tclcmd)); \
	char error_text[MAX_LEN]; \
\
    do { \
	if ( !sasfit_env_get_interp() ) return; \
\
	strcpy(error_text, (tclcmd)); \
	ret = vsprintf( error_text + prefix, format, arglist ); \
\
	if ( prefix+ret+2 >= MAX_LEN ) return; \
	error_text[prefix+ret] = '\"'; \
	error_text[prefix+ret+1] = '\0'; \
\
	Tcl_Eval(sasfit_env_get_interp(), error_text); \
\
	if (setresult) \
	{ \
		res_obj = Tcl_NewStringObj(ERROR_RESULT_HEAD,strlen(ERROR_RESULT_HEAD)); \
		if ( !res_obj ) return; \
		Tcl_AppendToObj(res_obj, error_text + prefix, ret); \
		Tcl_ResetResult(sasfit_env_get_interp()); \
		Tcl_SetObjResult(sasfit_env_get_interp(), res_obj); \
	} \
    } while(0)

void sasfit_tcl_err(const char * format, va_list arglist)
{
	SASFIT_TCL_PUTS(SASFIT_TCL_PUTS_ERR_CMD, 1);
}

void sasfit_tcl_out(const char * format, va_list arglist)
{
	SASFIT_TCL_PUTS(SASFIT_TCL_PUTS_OUT_CMD, 0);
}

#undef SASFIT_TCL_PUTS_ERR_CMD
#undef SASFIT_TCL_PUTS_OUT_CMD
#undef SASFIT_TCL_PUTS
#undef MAX_LEN

