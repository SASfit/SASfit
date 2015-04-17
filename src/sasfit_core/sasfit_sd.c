/*
 * src/sasfit_core/sasfit_sd.c
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
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   Ingo Bressler (ingo@cs.tu-berlin.de)
 */

#include <string.h>
#include <sasfit_sd.h>
#include "include/sasfit_core.h"
#include <sasfit_plugin.h>
#include "include/sasfit_plugin_backend.h"

scalar sasfit_sd(scalar x,
                 scalar * a,
                 sasfit_function * sd,
                 int * dparam,
                 int * err)
{
	scalar res;
	int dpar = 0;

        if( !sasfit_valid_fct(sd) )
	{
		sasfit_err("SD is not valid !\n");
		if(err) *err = TCL_ERROR;
		return 0.0;
	}

	// copy the supplied parameters
	sasfit_copy_param( &(sd->params), a);

	if( sd->params.errStatus != TCL_OK )
	{
		sasfit_err("Supplied parameters are erroneous !\n");
		if(err) *err = TCL_ERROR;
		return 0.0;
	}

	if (dparam) dpar = dparam[SDfct];
//	sasfit_out("SD: %g %g %g %g %d",sd->params.p[0],sd->params.p[1],sd->params.p[2],sd->params.p[3],dpar);

	res = sasfit_part_diff_fct(x, sd, dpar);
//	sasfit_out(" %lg\n", res);

	if ( sd->params.errStatus != TCL_OK )
	{
		sasfit_err("#size_distrib: can not calculate \n"
		           "value of %s diff %d\n%s\n",sd->typestr,
		           dpar, sd->params.errStr);
		if(err) *err = TCL_ERROR;
		return 0.0;
	}
	return res;
}

int sasfit_sd_init(sasfit_analytpar * ap)
{
//	int funcid;
//	const sasfit_plugin_func_t * func_descr = NULL;

	sasfit_function* sd = 0;

	SASFIT_ASSERT_VAL(ap, FALSE);

	// assuming we get an already initialized (valid) sasfit_function
	// (initialized during init of according AP, earlier,
	// see SASFIT_x_tcl.c and sasfit_init_fct() in sasfit_common)
	sd = &(ap->SD);	// get size distrib function pointer

	// try to determine the function by specified plugin function ID
	if ( sasfit_function_select_id(sd) == TCL_OK )
	{
		return TCL_OK;
	}

	// select the functions

	if ( strcmp(sd->typestr,"LogNorm") == 0 )
	{
		sd->fct = sasfit_sd_LogNorm;
		sd->F_identifier = SD_id_LogNorm;
	} else
	if ( strcmp(sd->typestr,"BiLogNorm") == 0 )
	{
		sd->fct = sasfit_sd_BiLogNorm;
	} else
	if ( strcmp(sd->typestr,"Gaussian") == 0 )
	{
		sd->fct = sasfit_sd_GaussDistribution;
	} else
	if ( strcmp(sd->typestr,"Maxwell") == 0 )
	{
		sd->fct = sasfit_sd_Maxwell;
	} else
	if ( strcmp(sd->typestr,"Schulz-Zimm") == 0 )
	{
		sd->fct = sasfit_sd_Schulz_Zimm;
	} else
	if ( strcmp(sd->typestr,"Weibull") == 0 )
	{
		sd->fct = sasfit_sd_Weibull;
	} else
	if ( strcmp(sd->typestr,"GEX") == 0 )
	{
		sd->fct = sasfit_sd_GEX;
	} else
	if ( strcmp(sd->typestr,"GEV") == 0 )
	{
		sd->fct = sasfit_sd_GEV;
	} else
	if ( strcmp(sd->typestr,"Triangular") == 0 )
	{
		// sd->fct = sasfit_sd_Rn_Triangular;
		sd->fct = sasfit_sd_Triangular;
	} else
	if ( strcmp(sd->typestr,"fractalSD") == 0 )
	{
		sd->fct = sasfit_sd_fractalSD;
	} else
	if ( strcmp(sd->typestr,"gammaSD") == 0 )
	{
		sd->fct = sasfit_sd_gammaSD;
	} else
	if ( strcmp(sd->typestr,"Uniform") == 0 )
	{
		sd->fct = sasfit_sd_Uniform;
	} else
	if ( strcmp(sd->typestr,"Beta") == 0 )
	{
		sd->fct = sasfit_sd_Beta;
	} else
	if ( strcmp(sd->typestr,"Monodisperse") == 0 )
	{
		sd->fct = sasfit_sd_Monodisperse;
		sd->F_identifier = SD_id_Monodisperse;
	} else
	if ( strcmp(sd->typestr,"Delta") == 0 )
	{
		sd->fct = sasfit_sd_Delta;
		sd->F_identifier = SD_id_Delta;
	} else
	if ( strcmp(sd->typestr,"A_div_B") == 0 )
	{
		sd->fct = sasfit_sd_AdivB;
		sd->F_identifier = SD_id_AdivB;
	} else
	{
		// unknown size distribution
		sasfit_param_set_err(&sd->params, "Unknown size distribution !");
		return TCL_ERROR;
	}
	return TCL_OK;
}

