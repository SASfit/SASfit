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

#ifndef GMDI_STRUCTS_H_

#define GMDI_STRUCTS_H_

#include <gsl/gsl_integration.h>
#include "gmdi.h"

/* The integration form is as following:
 *
 * \int_{x^{(0)}_0} ^{x^{(0)}_1} f_0(x^{(0)}) dx^{(0)} \int_{x^{(1)}_0} ^{x^{(1)}_1} f_1(x^{(0)}, x^{(1)})\cdots \int_{x^{(n-1)}_0} ^{x^{(n-1)}_1} f_{n-1} dx^{(n-1)}
 *
 * gmdi_multi_dim_inte_param::oip[i] corresponds to the dimension of x^{(i)}.
 */

/* The param of one integral */
typedef struct struct_gmdi_one_inte_param
{
    gmdi_function_or_constant           x0;     /* Integration low limit */
    gmdi_function_or_constant           x1;     /* Integration high limit */
    double                              limit;
    double                              epsrel; /* relative tolerance */
    double                              epsabs; /* absolute tolerance */
    int                                 key;
    GMDI_INTE_FUNCTIONS                 inte_func;  /* Which GSL integration function to use? */
    gmdi_multi_var_function             f;          /* function to be integrated */

    /* used internally. */
    struct
    {
        gsl_integration_workspace *         giw;    /* integration workspace for this dimension */
    } intern;

} gmdi_one_inte_param;

typedef struct struct_gmdi_multi_dim_inte_param
{
    size_t                              n; /* integral dimension */

    gmdi_one_inte_param *               oip;    /* the array of the parameters for each dimension. */

    /* output result */
    double                              result;
    double                              abserr;
    size_t                              neval;

    /* used internally */
    struct
    {
        size_t                              dim;            /* which dim are we working on now? */
        double*                             x;              /* Value of the high dimensional x */
        double*                             results;        /* Results of each dimension. It's not the final output. */
        double*                             abserrs;        /* Abserrs of each dimension. It's not the final output. */
    } intern;
} gmdi_multi_dim_inte_param;


#endif /* GMDI_STRUCTS_H_ */
