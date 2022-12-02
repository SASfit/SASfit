/*
 * src/sasfit_common/sasfit_function.c
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
 *   Ingo Bressler (ingo.bressler@bam.de)
 */

#include <stdarg.h>
#include "include/sasfit_common.h"

void sasfit_init_fct(sasfit_function * f)
{
	int i;

	SASFIT_ASSERT_VOID(f);

	// reset function pointer
	f->fct = 0;
	f->fct_f = 0;
	f->fct_v = 0;
	// default value for function identifier
	f->F_identifier = -1;
	f->V_identifier = -1;
	f->compute_f = FALSE;

	// initialize error message and typestr
	for(i=0; i < STRLEN ;i++)
	{
		f->typestr[i] = '\0';
	}
	f->typeLen = 0;

	// reset parameter
	sasfit_init_param( &(f->params) );
}

void sasfit_init_param(sasfit_param * param)
{
	int i;

	SASFIT_ASSERT_VOID(param);

	// reset parameter array
	for(i=0; i < MAXPAR ;i++)
	{
		param->p[i] = 0.0;
	}
	// initialize error message and typestr
	for(i=0; i < STRLEN ;i++)
	{
		param->errStr[i] = '\0';
	}
	param->errLen = 0;
	// reset error status
	param->errStatus = FALSE;
}

void sasfit_get_param(sasfit_param * param, int num, ...)
{
	va_list argp;
	scalar *ptr = 0;
	int i = 0;

	SASFIT_ASSERT_VOID(param);

	if ( num <= 0 || num > MAXPAR )
	{
		sasfit_err("Number of arguments out of range (0,%d), doing nothing !\n", MAXPAR);
		return;
	}

	va_start(argp, num);

	for (i=0; i < num ;i++)
	{
		ptr = va_arg(argp, scalar*);
		if ( ptr != 0 )
		{
			*ptr = param->p[i];
		}
	}

	va_end(argp);
}

void sasfit_copy_param(sasfit_param * param, scalar * a)
{
	int i;

	SASFIT_ASSERT_VOID(param);
	SASFIT_ASSERT_VOID(a);

	for(i=0; i < MAXPAR ;i++)
	{
		param->p[i] = a[i];
	}
}

void sasfit_print_param(sasfit_param * param)
{
	int i;
	SASFIT_ASSERT_VOID(param);
	for(i=0; i < MAXPAR ;i++)
	{
		sasfit_err("param %2d = %lg\n", i, param->p[i]);
	}
}

int sasfit_valid_fct(sasfit_function * f)
{
	if ( 		       f != 0 &&
			  f->fct != 0 &&
		      f->typestr != 0 &&
		f->params.errStr != 0 )
	{
		return 1;
	}
	sasfit_err("f: %x, f->fct: %x, f->typestr: %x, f->params.errStr: %x\n",
			  f,	 f->fct,     f->typestr,     f->params.errStr);
	return 0;
}

scalar sasfit_part_diff_fct(scalar x, sasfit_function * f, int dparam)
{
	scalar h = sasfit_eps_get_h();
	scalar param_neg, param_pos, param_len, param_orig, result;
	scalar  (* fct) (scalar x, sasfit_param* params) = 0;
	scalar* param = 0;

	SASFIT_ASSERT_PTR( f );

	fct = f->fct;
	if ( f->compute_f == TRUE ) // select the non-squared function if required
	{
		fct = f->fct_f;
	}

	SASFIT_CHECK_COND((!fct), &(f->params), "Selected Function is not available!\n");

	if( dparam < 1 )
	{
		result = fct(x, &f->params);
	}
	else	// partially differentiate (this goes into a function if it works correctly)
	{
		param = &f->params.p[dparam-1];

		if( *param == 0.0 )
		{
			param_pos =  h;
			param_neg = -h;
/*
			sasfit_param_set_err(&f->params, "%g %g %g %g %d\n",
							f->params.p[0],
							f->params.p[1],
							f->params.p[2],
							f->params.p[3], dparam);
*/
		}
		else
		{
			param_pos = (*param)*(1.0 + h);
			param_neg = (*param)*(1.0 - h);
		}
		param_len = param_pos-param_neg;
		param_orig = *param;	// save the original value, we need to overwrite it

		*param = param_pos;
		result = fct(x, &f->params);

		*param = param_neg;
		result = result - fct(x, &f->params);

		*param = param_len;
		result = result / param_len;

		*param = param_orig;	// restore the original value
	}

	//sasfit_out("returning %lg\n",result);
	return result;
}

int sasfit_get_maxpar()
{
	return MAXPAR;
}



sasfit_param_override_t	sasfit_param_override_psi;

void sasfit_param_override_init (void)
{
	sasfit_param_override_psi.override  = FALSE;
	sasfit_param_override_psi.value     = 0.0;
	sasfit_param_override_psi.value2    = 0.0;

}

void sasfit_param_override_set_psi2 (scalar new_value)
{
	sasfit_param_override_psi.override = TRUE;
	sasfit_param_override_psi.value2    = new_value;
}

scalar sasfit_param_override_get_psi2 (scalar default_val)
{
	if (sasfit_param_override_psi.override)
		return sasfit_param_override_psi.value2;
	else
		return default_val;
}

void sasfit_param_override_set_psi (scalar new_value)
{
	sasfit_param_override_psi.override = TRUE;
	sasfit_param_override_psi.value    = new_value;
}

scalar sasfit_param_override_get_psi (scalar default_val)
{
	if (sasfit_param_override_psi.override)
		return sasfit_param_override_psi.value;
	else
		return default_val;
}

void sasfit_param_override_set_2theta (scalar new_value)
{
	sasfit_param_override_psi.override = TRUE;
	sasfit_param_override_psi.value3    = new_value;
}

scalar sasfit_param_override_get_2theta (scalar default_val)
{
	if (sasfit_param_override_psi.override)
		return sasfit_param_override_psi.value3;
	else
		return default_val;
}


void sasfit_param_override_set_lambda (scalar new_value)
{
	sasfit_param_override_psi.override = TRUE;
	sasfit_param_override_psi.value4    = new_value;
}

scalar sasfit_param_override_get_lambda (scalar default_val)
{
	if (sasfit_param_override_psi.override)
		return sasfit_param_override_psi.value4;
	else
		return default_val;
}
