/*
 * src/sasfit_common/sasfit_integrate.c
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

#include <gsl/gsl_integration.h>
#include <gsl/gsl_errno.h>
#include "include/sasfit_common.h"

sasfit_int_workspace_t	sasfit_int_workspace = {0, NULL};
sasfit_int_ws_all_t	sasfit_int_ws_all = { -1 };

// not used yet
void sasfit_int_ws_all_free(void)
{
	int i=0;
	for(i=SASFIT_MAX_WS-1; i >= 0 ;i--)
	{
		sasfit_int_free(i);
		sasfit_int_release();
	}
}

void sasfit_int_free(int idx)
{
	if ( idx >= SASFIT_MAX_WS || idx < 0 ) 
	{
		sasfit_err("Invalid number of integration workspaces !\n");
		return;
	}
	if ( sasfit_int_ws_all.ws[idx].ptr != NULL )
	{
		gsl_integration_workspace_free(sasfit_int_ws_all.ws[idx].ptr);
	}
	sasfit_int_ws_all.ws[idx].ptr = NULL;
	sasfit_int_ws_all.ws[idx].size = 0;
}

void sasfit_int_occupy(int size)
{
	int occ = sasfit_int_ws_all.last+1;

	if ( size <= 0 ) return;
	if ( occ >= SASFIT_MAX_WS || occ < 0 ) 
	{
		sasfit_err("Can't occupy more integration workspace !\n");
		return;
	}
	// enlarge the gsl workspace if required
	if ( size > sasfit_int_ws_all.ws[occ].size )
	{
		sasfit_int_free(occ);
		sasfit_int_ws_all.ws[occ].ptr = gsl_integration_workspace_alloc(size);
		sasfit_int_ws_all.ws[occ].size = size;
	}
	sasfit_int_ws_all.last = occ; // finally occupy it
//sasfit_out("occupied: %d\n",sasfit_int_ws_all.last);
}

void sasfit_int_release(void)
{
	sasfit_int_ws_all.last--;
//sasfit_out("released: %d\n",sasfit_int_ws_all.last);
}

gsl_integration_workspace * sasfit_int_mem(void)
{
	if ( sasfit_int_ws_all.last >= SASFIT_MAX_WS || 
	     sasfit_int_ws_all.last < 0 ) return NULL;
	return sasfit_int_ws_all.ws[sasfit_int_ws_all.last].ptr;
}

scalar sasfit_integrate_ctm(scalar int_start,
			scalar int_end,
			sasfit_func_one_t intKern_fct, 
			sasfit_param * param,
			int limit,
			scalar epsabs,
			scalar epsrel)
{
	scalar res, errabs;
	int err;
	gsl_function F;

	SASFIT_ASSERT_PTR(param);
	SASFIT_ASSERT_PTR(intKern_fct);

	if ( gsl_finite(int_start) && gsl_finite(int_end) && 
	     (int_end - int_start) == 0.0 )
	// nothing to integrate, test for an eps instead of 0 ? (which?)
	{
		return 0.0;
	}

	sasfit_int_occupy(limit);
	F.params = param;
	F.function = (double (*) (double, void*)) intKern_fct;

	gsl_set_error_handler_off();

	if ( gsl_isinf(int_start) && gsl_finite(int_end) ) 	// adaptive integration on infinite intervals (-\infty,b)
	{
        	err = gsl_integration_qagil(&F, int_end, epsabs, epsrel, limit, sasfit_int_mem(), &res, &errabs);
	}
	else if ( gsl_finite(int_start) && gsl_isinf(int_end) )	// adaptive integration on infinite intervals (a,+\infty)
	{
        	err = gsl_integration_qagiu(&F, int_start, epsabs, epsrel, limit, sasfit_int_mem(), &res, &errabs);
	}
	else if ( gsl_isinf(int_start) && gsl_isinf(int_end) ) 	// adaptive integration on infinite intervals (-\infty,+\infty)
	{
        	err = gsl_integration_qagi(&F, epsabs, epsrel, limit, sasfit_int_mem(), &res, &errabs);
	} 
	else if ( gsl_finite(int_start) && gsl_finite(int_end) ) // adaptive integration with singularities 
	                                                         // on well defined intervall (a,b)
	{
        	err = gsl_integration_qags(&F, int_start, int_end, epsabs, epsrel, limit, sasfit_int_mem(), &res, &errabs);
	} else {
		sasfit_err("Erroneous specification of integration intervals!\n");
		err = 1; // error
	}

	sasfit_int_release();

	if (err) 
	{
                sasfit_param_set_err(param,
                    DBGINFO("Could not properly perform integration of %x "
                            "over [%f, %f]: %s\n"), intKern_fct,
                    int_start, int_end, gsl_strerror(err));
                return SASFIT_RETURNVAL_ON_ERROR;
	}

	return res;
}

