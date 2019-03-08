/*
 * Copyright (C) 2013 Hong Xu
 * All rights reserved.
 * 
 * This file is part of MultiDimInte.
 *
 * MultiDimInte is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later
 * version.
 * 
 * MultiDimInte is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with MultiDimInte.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <string.h>
#include "gmdi.h"
#include "gmdi_structs.h"

/*
 * Initialize gmdi_inte_handle
 */
gmdi_inte_handle gmdi_create_inte_handle(size_t n)
{
    gmdi_multi_dim_inte_param * ret;

    ret = (gmdi_multi_dim_inte_param * ) malloc(sizeof(gmdi_multi_dim_inte_param));
    memset(ret, 0, sizeof(gmdi_multi_dim_inte_param));
    
    ret->n = n;
    ret->oip = (gmdi_one_inte_param *) calloc(n, sizeof(gmdi_one_inte_param));

    return ret;
}

/*
 * Destroy gmdi_inte_handle
 */
void gmdi_free_inte_handle(gmdi_inte_handle * handle)
{
    free(((gmdi_multi_dim_inte_param * )handle)->oip);

    free(handle);
}

void gmdi_handle_set_key(gmdi_inte_handle handle, int key, size_t n)
{
    ((gmdi_multi_dim_inte_param *) handle)->oip[n].key = key;
}

void gmdi_handle_set_epsabs(gmdi_inte_handle handle, double epsabs, size_t n)
{
    ((gmdi_multi_dim_inte_param *) handle)->oip[n].epsabs = epsabs;
}

void gmdi_handle_set_epsrel(gmdi_inte_handle handle, double epsrel, size_t n)
{
    ((gmdi_multi_dim_inte_param *) handle)->oip[n].epsrel = epsrel;
}

void gmdi_handle_set_limit(gmdi_inte_handle handle, double limit, size_t n)
{
    ((gmdi_multi_dim_inte_param *) handle)->oip[n].limit = limit;
}

void gmdi_handle_set_inte_func(gmdi_inte_handle handle, GMDI_INTE_FUNCTIONS inte_func, size_t n)
{
    ((gmdi_multi_dim_inte_param *) handle)->oip[n].inte_func = inte_func;
}

void gmdi_handle_set_x0(gmdi_inte_handle handle, const gmdi_function_or_constant * x0, size_t n)
{
    ((gmdi_multi_dim_inte_param *) handle)->oip[n].x0 = *x0;
}

void gmdi_handle_set_x0_constant(gmdi_inte_handle handle, double x0, size_t n)
{
    gmdi_function_or_constant gfc;
    gfc.type = GMDI_FUNCTION_OR_CONSTANT_TYPE_CONSTANT;
    gfc.content.c = x0;

    gmdi_handle_set_x0(handle, &gfc, n);
}

void gmdi_handle_set_x1(gmdi_inte_handle handle, const gmdi_function_or_constant * x1, size_t n)
{
    ((gmdi_multi_dim_inte_param *) handle)->oip[n].x1 = *x1;
}

void gmdi_handle_set_x1_constant(gmdi_inte_handle handle, double x1, size_t n)
{
    gmdi_function_or_constant gfc;
    gfc.type = GMDI_FUNCTION_OR_CONSTANT_TYPE_CONSTANT;
    gfc.content.c = x1;

    gmdi_handle_set_x1(handle, &gfc, n);
}

void gmdi_handle_set_f(gmdi_inte_handle handle, const gmdi_multi_var_function * f, size_t n)
{
    ((gmdi_multi_dim_inte_param *) handle)->oip[n].f = *f;
}

void gmdi_handle_set_kernel(gmdi_inte_handle handle, const gmdi_multi_var_function * f)
{
    gmdi_multi_dim_inte_param * params = (gmdi_multi_dim_inte_param *) handle;
    params->oip[params->n - 1].f = *f;
}

double gmdi_handle_get_result(gmdi_inte_handle handle)
{
    return ((gmdi_multi_dim_inte_param *) handle)->result;
}

double gmdi_handle_get_abserr(gmdi_inte_handle handle)
{
    return ((gmdi_multi_dim_inte_param *) handle)->abserr;
}
