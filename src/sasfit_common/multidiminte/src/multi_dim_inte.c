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

/*
 * This is the file which does the integration.
 *
 * The integration form is as following:
 *
 * \int_{x^{(0)}_0} ^{x^{(0)}_1} f_0(x^{(0)}) dx^{(0)} \int_{x^{(1)}_0} ^{x^{(1)}_1} f_1(x^{(0)}, x^{(1)})\cdots \int_{x^{(n-1)}_0} ^{x^{(n-1)}_1} f_{n-1} dx^{(n-1)}
 *
 * A recursive technique is used. big_g is always be the integration kernel. call_integration_func will set the limit
 * and call the integration function to integrate big_g. big_g will call call_integration_func for the inner level of
 * the integration. big_g will stop calling call_integration_func once the most inner level of integration is reached.
 * The recursive procedure is:
 *
 * call_integration_func --> integrate big_g --> call_integration_func in big_g --> ...
 *
 * For example, for the zeroth dimension, big_g is
 *
 * f_0(x^{(0)}) dx^{(0)} \int_{x^{(1)}_0} ^{x^{(1)}_1} f_1(x^{(0)}, x^{(1)})\cdots \int_{x^{(n-1)}_0} ^{x^{(n-1)}_1} f_{n-1} dx^{(n-1)}.
 *
 * which is integrated by the first call of call_integration_func. In this big_g, it first calls call_integration_func
 * to evaluate
 *
 * \int_{x^{(1)}_0} ^{x^{(1)}_1} f_1(x^{(0)}, x^{(1)})\cdots \int_{x^{(n-1)}_0} ^{x^{(n-1)}_1} f_{n-1} dx^{(n-1)}.
 *
 * Remember call_integration_func evaluates recursively. Then it evaluates f_0, and multiply them.
 */

//#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gsl/gsl_errno.h>
#include <stddef.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_qrng.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_monte.h>
#include <gsl/gsl_monte_plain.h>
#include <gsl/gsl_monte_miser.h>
#include <gsl/gsl_monte_vegas.h>
#include <gsl/gsl_integration.h>
#include "tanhsinh/tanhsinh.h"
#include <sys/time.h>
#include "../../include/sasfit_common.h"
#include "../../include/lobatto.h"
#include "../../quasimontecarlo/quasimontecarlo.h"
#include "../../quasimontecarlo/Burley2020Scrambling/genpoints.h"
#include "gmdi.h"
#include "gmdi_structs.h"
#include "sasfit.h"
#include "smolpack.h"
#include "sparse_grid_hw.h"
#include "sparse_grid_cc.h"
#include "smolpack.h"
#include "Frolov.h"
#define CALL_GSL_FUNCTION(gf, x)                    gf.function((x), gf.params)
#define CALL_GMDI_MULTI_VAR_FUNCTION(gmvf, x)       gmvf.function((x), gmvf.n, gmvf.params)

/*
 * Evaluate gmdi_function_or_constant according to their type.
 */
static double call_gmdi_function_or_constant(gmdi_function_or_constant * gfc, const double * x)
{
    switch (gfc->type)
    {
    case GMDI_FUNCTION_OR_CONSTANT_TYPE_CONSTANT:
        return gfc->content.c;
    case GMDI_FUNCTION_OR_CONSTANT_TYPE_FUNCTION:
        return CALL_GSL_FUNCTION(gfc->content.gf, *x);
    case GMDI_FUNCTION_OR_CONSTANT_TYPE_MULTI_VAR_FUNCTION:
        return CALL_GMDI_MULTI_VAR_FUNCTION(gfc->content.mf, x);
    }
}

static double big_g(double y, void* p);

/*
 * call the integration function according to the parameters.
 */
static int call_integration_func(gmdi_multi_dim_inte_param* params)
{
    gmdi_one_inte_param *               oip = params->oip + params->intern.dim;

    int                                 ret;
    int                                 n =15;

    double                              inte_limit_low  = call_gmdi_function_or_constant(&oip->x0, params->intern.x);
    double                              inte_limit_high = call_gmdi_function_or_constant(&oip->x1, params->intern.x);
    unsigned num_eval;

    gsl_function                        gf;

    gf.function = big_g;
    gf.params = params;

    switch (oip->inte_func)
    {
    case GMDI_INTE_FUNCTIONS_QNG:
        ret = gsl_integration_qng(&gf,
                inte_limit_low,
                inte_limit_high,
                oip->epsabs,
                oip->epsrel,
                params->intern.results + params->intern.dim,
                params->intern.abserrs + params->intern.dim,
                &params->neval);

        break;
    case GMDI_INTE_FUNCTIONS_QAG:
        ret = gsl_integration_qag(&gf,
                inte_limit_low,
                inte_limit_high,
                oip->epsabs,
                oip->epsrel,
                oip->limit,
                oip->key,
                oip->intern.giw,
                params->intern.results + params->intern.dim,
                params->intern.abserrs + params->intern.dim);
        break;
    case GMDI_INTE_FUNCTIONS_QAGS:
        ret = gsl_integration_qags(&gf,
                inte_limit_low,
                inte_limit_high,
                oip->epsabs,
                oip->epsrel,
                oip->limit,
                oip->intern.giw,
                params->intern.results + params->intern.dim,
                params->intern.abserrs + params->intern.dim);
        break;
    case GDMI_INTE_FUNCTIONS_OOURA_DE:
 //       sasfit_out("start OOURA DE integration dim:%d\n",params->intern.dim);
        sasfit_intde(big_g,
                inte_limit_low,
                inte_limit_high,
                oip->intern.aw,
                params->intern.results + params->intern.dim,
                params->intern.abserrs + params->intern.dim,
                params);
        ret=GSL_SUCCESS;
        break;
    case GDMI_INTE_FUNCTIONS_OOURA_CC:
//        sasfit_out("start OOURA CC integration dim:%d\n",params->intern.dim);
        sasfit_intcc(big_g,
                inte_limit_low,
                inte_limit_high,
                oip->epsrel,
                oip->intern.lenaw,
                oip->intern.aw,
                params->intern.results + params->intern.dim,
                params->intern.abserrs + params->intern.dim,
                params);
        ret=GSL_SUCCESS;
        break;
    case GDMI_INTE_FUNCTIONS_TANHSINH_1:
        *(params->intern.results + params->intern.dim) = TanhSinhQuad(&big_g, params,
            inte_limit_low,
            inte_limit_high,
            n,
            oip->epsrel,
            params->intern.abserrs + params->intern.dim);
        ret=GSL_SUCCESS;
        break;
    case GDMI_INTE_FUNCTIONS_TANHSINH_2:
        *(params->intern.results + params->intern.dim) = qthsh(&big_g, params,
            inte_limit_low,
            inte_limit_high,
            n,
            oip->epsrel,
            params->intern.abserrs + params->intern.dim);
        ret=GSL_SUCCESS;
        break;
    case GDMI_INTE_FUNCTIONS_CQUAD:
         ret = gsl_integration_cquad(&gf,
                 inte_limit_low,
                 inte_limit_high,
                 oip->epsabs, oip->epsrel,
                 oip->intern.gcw,
                 params->intern.results + params->intern.dim,
                 params->intern.abserrs + params->intern.dim,
                 &params->neval);
        break;
    case GDMI_INTE_FUNCTIONS_GAUSSLEGENDRE:
        *(params->intern.results + params->intern.dim) = gsl_integration_glfixed(&gf, inte_limit_low, inte_limit_high, oip->intern.glft);
        ret = GSL_SUCCESS;
        break;
    case GDMI_INTE_FUNCTIONS_CHEBYSHEV1:
        ret = gsl_integration_fixed(&gf, params->intern.results + params->intern.dim, oip->intern.gfw);
        break;
    case GDMI_INTE_FUNCTIONS_CHEBYSHEV2:
        ret = gsl_integration_fixed(&gf, params->intern.results + params->intern.dim, oip->intern.gfw);
        break;
    case GDMI_INTE_FUNCTIONS_GEGENBAUER:
        ret = gsl_integration_fixed(&gf, params->intern.results + params->intern.dim, oip->intern.gfw);
        break;
    case GDMI_INTE_FUNCTIONS_EXPONENTIAL:
        ret = gsl_integration_fixed(&gf, params->intern.results + params->intern.dim, oip->intern.gfw);
        break;
    case GDMI_INTE_FUNCTIONS_LAGUERRE:
        ret = gsl_integration_fixed(&gf, params->intern.results + params->intern.dim, oip->intern.gfw);
        break;
    case GDMI_INTE_FUNCTIONS_GSL_JACOBI:
        ret = gsl_integration_fixed(&gf, params->intern.results + params->intern.dim, oip->intern.gfw);
        break;
    case GDMI_INTE_FUNCTIONS_LOBATTO:
        *(params->intern.results + params->intern.dim) = sasfit_GaussLobattoInt(big_g,
                            inte_limit_low,inte_limit_low,oip->epsabs, oip->epsrel, 4000,params->intern.abserrs + params->intern.dim,params);
        //ret = gsl_integration_fixed(&gf, params->intern.results + params->intern.dim, oip->intern.gfw);
        break;
    default: /* No proper inte_func is found */
        GSL_ERROR("Invalid inte_func is specified", GSL_EINVAL);
    }
    return ret;
}

/*
 * The big_g function. See the comments at the top.
 */
static double big_g(double x, void * p)
{
    double                              result = 1.0;
    gmdi_multi_dim_inte_param *         params = (gmdi_multi_dim_inte_param *) p;
    gmdi_one_inte_param *               oip;

    oip = params->oip + params->intern.dim;

    params->intern.x[params->intern.dim] = x;

    if (params->intern.dim < params->n - 1)
    {
        ++ params->intern.dim;

        call_integration_func(params);
        result = params->intern.results[params->intern.dim];

        -- params->intern.dim;
    }

    if (oip->f.function)
        result *= CALL_GMDI_MULTI_VAR_FUNCTION(oip->f, params->intern.x);

    return result;
}

/*
 * Start the integration.
 */

 void alloc_ws_4_gmdi (gmdi_inte_handle handle)
 {
    int                                 ret, i;
    gmdi_multi_dim_inte_param *         params = (gmdi_multi_dim_inte_param *) handle;
    gmdi_one_inte_param *               oip = params->oip + params->intern.dim;
    double                              inte_limit_low  = call_gmdi_function_or_constant(&oip->x0, params->intern.x);
    double                              inte_limit_high = call_gmdi_function_or_constant(&oip->x1, params->intern.x);

    for (i = 0; i < params->n; ++ i)
    {
//        sasfit_out("alloc:dim:%d int_type:%d\n",i,params->oip[i].inte_func);
        switch( params->oip[i].inte_func) {
            case GDMI_INTE_FUNCTIONS_OOURA_DE :
                params->oip[i].intern.lenaw=4000;
                params->oip[i].intern.aw = (scalar *)malloc((params->oip[i].intern.lenaw)*sizeof(scalar));
                sasfit_intdeini(params->oip[i].intern.lenaw, GSL_DBL_MIN, params->oip[i].epsrel, params->oip[i].intern.aw);
                params->oip[i].intern.gcw=NULL;
                params->oip[i].intern.giw=NULL;
                params->oip[i].intern.glft=NULL;
                params->oip[i].intern.gfw=NULL;
                break;
            case GDMI_INTE_FUNCTIONS_OOURA_CC :
                params->oip[i].intern.lenaw=4000;
                params->oip[i].intern.aw = (scalar *)malloc((params->oip[i].intern.lenaw+1)*sizeof(scalar));
                sasfit_intccini(params->oip[i].intern.lenaw, params->oip[i].intern.aw);
                params->oip[i].intern.gcw=NULL;
                params->oip[i].intern.giw=NULL;
                params->oip[i].intern.glft=NULL;
                params->oip[i].intern.gfw=NULL;
                break;
            case GDMI_INTE_FUNCTIONS_CQUAD :
                params->oip[i].intern.aw=NULL;
                params->oip[i].intern.gcw=gsl_integration_cquad_workspace_alloc(params->oip[i].limit);
                params->oip[i].intern.giw=NULL;
                params->oip[i].intern.glft=NULL;
                params->oip[i].intern.gfw=NULL;
                break;
            case GMDI_INTE_FUNCTIONS_QAG :
                params->oip[i].intern.aw=NULL;
                params->oip[i].intern.gcw=NULL;
                params->oip[i].intern.giw=gsl_integration_workspace_alloc(params->oip[i].limit);
                params->oip[i].intern.glft=NULL;
                params->oip[i].intern.gfw=NULL;
                break;
            case GMDI_INTE_FUNCTIONS_QAGS :
                params->oip[i].intern.aw=NULL;
                params->oip[i].intern.gcw=NULL;
                params->oip[i].intern.giw=gsl_integration_workspace_alloc(params->oip[i].limit);
                params->oip[i].intern.glft=NULL;
                params->oip[i].intern.gfw=NULL;
                break;
            case GMDI_INTE_FUNCTIONS_QNG :
                params->oip[i].intern.aw=NULL;
                params->oip[i].intern.gcw=NULL;
                params->oip[i].intern.giw=gsl_integration_workspace_alloc(params->oip[i].limit);
                params->oip[i].intern.glft=NULL;
                params->oip[i].intern.gfw=NULL;
                break;
            case GDMI_INTE_FUNCTIONS_GAUSSLEGENDRE :
                params->oip[i].intern.aw=NULL;
                params->oip[i].intern.gcw=NULL;
                params->oip[i].intern.giw=NULL;
                params->oip[i].intern.glft = gsl_integration_glfixed_table_alloc(sasfit_eps_get_gausslegendre());
                params->oip[i].intern.gfw=NULL;
                break;
            case GDMI_INTE_FUNCTIONS_CHEBYSHEV1 :
                params->oip[i].intern.aw=NULL;
                params->oip[i].intern.gcw=NULL;
                params->oip[i].intern.giw=NULL;
                params->oip[i].intern.glft=NULL;
                params->oip[i].intern.gfw = gsl_integration_fixed_alloc(gsl_integration_fixed_chebyshev,sasfit_eps_get_chebyshev1(),inte_limit_low,inte_limit_high,sasfit_eps_get_alpha(), sasfit_eps_get_beta());
                break;
            case GDMI_INTE_FUNCTIONS_CHEBYSHEV2 :
                params->oip[i].intern.aw=NULL;
                params->oip[i].intern.gcw=NULL;
                params->oip[i].intern.giw=NULL;
                params->oip[i].intern.glft=NULL;
                params->oip[i].intern.gfw = gsl_integration_fixed_alloc(gsl_integration_fixed_chebyshev2,sasfit_eps_get_chebyshev2(),inte_limit_low,inte_limit_high,sasfit_eps_get_alpha(), sasfit_eps_get_beta());
                break;
            case GDMI_INTE_FUNCTIONS_GEGENBAUER :
                params->oip[i].intern.aw=NULL;
                params->oip[i].intern.gcw=NULL;
                params->oip[i].intern.giw=NULL;
                params->oip[i].intern.glft=NULL;
                params->oip[i].intern.gfw = gsl_integration_fixed_alloc(gsl_integration_fixed_gegenbauer,sasfit_eps_get_gegenbauer(),inte_limit_low,inte_limit_high,sasfit_eps_get_alpha(), sasfit_eps_get_beta());
                break;
            case GDMI_INTE_FUNCTIONS_EXPONENTIAL :
                params->oip[i].intern.aw=NULL;
                params->oip[i].intern.gcw=NULL;
                params->oip[i].intern.giw=NULL;
                params->oip[i].intern.glft=NULL;
                params->oip[i].intern.gfw = gsl_integration_fixed_alloc(gsl_integration_fixed_exponential,sasfit_eps_get_exponential(),inte_limit_low,inte_limit_high,sasfit_eps_get_alpha(), sasfit_eps_get_beta());
                break;
            case GDMI_INTE_FUNCTIONS_LAGUERRE :
                params->oip[i].intern.aw=NULL;
                params->oip[i].intern.gcw=NULL;
                params->oip[i].intern.giw=NULL;
                params->oip[i].intern.glft=NULL;
                params->oip[i].intern.gfw = gsl_integration_fixed_alloc(gsl_integration_fixed_laguerre,sasfit_eps_get_laguerre(),inte_limit_low,inte_limit_high,sasfit_eps_get_alpha(), sasfit_eps_get_beta());
                break;
            case GDMI_INTE_FUNCTIONS_GSL_JACOBI :
                params->oip[i].intern.aw=NULL;
                params->oip[i].intern.gcw=NULL;
                params->oip[i].intern.giw=NULL;
                params->oip[i].intern.glft=NULL;
                params->oip[i].intern.gfw = gsl_integration_fixed_alloc(gsl_integration_fixed_jacobi,sasfit_eps_get_jacobi(),inte_limit_low,inte_limit_high,sasfit_eps_get_alpha(), sasfit_eps_get_beta());
                break;
            default :
                params->oip[i].intern.aw=NULL;
                params->oip[i].intern.gcw=gsl_integration_cquad_workspace_alloc(params->oip[i].limit);
                params->oip[i].intern.giw=NULL;
                params->oip[i].intern.glft=NULL;
                params->oip[i].intern.gfw=NULL;
                break;
        }
    }
}

void free_ws_4_gmdi (gmdi_inte_handle handle)
 {
    int                                 ret, i;
    gmdi_multi_dim_inte_param *         params = (gmdi_multi_dim_inte_param *) handle;
    gmdi_one_inte_param *               oip = params->oip + params->intern.dim;

    for (i = 0; i < params->n; ++ i)
    {
//        sasfit_out("free:dim:%d int_type:%d\n",i,params->oip[i].inte_func);
        if (params->oip[i].intern.gcw!=NULL) gsl_integration_cquad_workspace_free(params->oip[i].intern.gcw);
        if (params->oip[i].intern.aw!=NULL) free(params->oip[i].intern.aw);
        if (params->oip[i].intern.giw!=NULL) gsl_integration_workspace_free(params->oip[i].intern.giw);
        if (params->oip[i].intern.glft!=NULL) gsl_integration_glfixed_table_free(params->oip[i].intern.glft);
        if (params->oip[i].intern.gfw!=NULL) gsl_integration_fixed_free(params->oip[i].intern.gfw);
    }
}

int gmdi_multi_dimensional_integration(gmdi_inte_handle handle)
{
    int                                         ret, i;
    gmdi_multi_dim_inte_param *                 params = (gmdi_multi_dim_inte_param *) handle;

    params->intern.x = calloc(params->n, sizeof(double));
    params->intern.results = calloc(params->n, sizeof(double));
    params->intern.abserrs = calloc(params->n, sizeof(double));
    params->intern.dim = 0;
    alloc_ws_4_gmdi(params);
    for (i = 0; i < params->n; ++ i)
    {
        params->oip[i].f.n = i + 1;

        if (params->oip[i].x0.type == GMDI_FUNCTION_OR_CONSTANT_TYPE_MULTI_VAR_FUNCTION)
            params->oip[i].x0.content.mf.n = i;
        if (params->oip[i].x1.type == GMDI_FUNCTION_OR_CONSTANT_TYPE_MULTI_VAR_FUNCTION)
            params->oip[i].x1.content.mf.n = i;
    }

    ret = call_integration_func(params);

    params->result = params->intern.results[0];
    params->abserr = params->intern.abserrs[0];


    free_ws_4_gmdi(params);

    free(params->intern.x);
    free(params->intern.results);
    free(params->intern.abserrs);

    return ret;
}

/*
 * Get the version number of the library.
 */
/*
void gmdi_get_version(int * major, int * minor, int * subminor)
{
    if (major != NULL)
        *major = MULTIDIMINTE_VERSION_MAJOR;
    if (minor != NULL)
        *minor = MULTIDIMINTE_VERSION_MINOR;
    if (major != NULL)
        *subminor = MULTIDIMINTE_VERSION_SUBMINOR;
}
*/
int Kernel_cub_nD(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
	multint_cub *cub;
	cub = (multint_cub *) pam;
	fval[0]=(*cub->KernelnD_fct)(x,ndim,cub->param);
	return 0;
}

scalar Kernel_MCnD(scalar *x, size_t dim, void *pam) {
	multint_cub *cub;
	cub = (multint_cub *) pam;
	return (*cub->KernelnD_fct)(x,dim,cub->param);
}

int sasfit_cubature(size_t ndim,
			scalar *int_start,
			scalar *int_end,
			sasfit_func_ndim_t *intKern_fct,
			void * param,
			scalar epsrel,
			scalar *result,
			scalar *error) {
    gmdi_inte_handle            handle = gmdi_create_inte_handle(ndim);
    gmdi_function_or_constant   fc;
    gmdi_multi_var_function     mvf;
    int                         ret = 0;
    GMDI_INTE_FUNCTIONS         integration_strategy;
    int done;
    size_t limit = 4000;
    ((gmdi_multi_dim_inte_param *) handle)->neval=limit;
    scalar epsabs = 0;
	int i,method, key;
	scalar Iavg;

    int  k, j;

    double *x=NULL, *w=NULL, *x_ab=NULL;
    double wt, volume_ndim;
    int n , n2;
    static cub_grid_struct *cg=NULL;
    static cub_grid_struct cg_static;

	scalar fval[1], ferr[1];
    gsl_integration_glfixed_table * wglfixed;
    multint_cub cubstruct;
    const gsl_rng_type *T;
    gsl_rng *r;
    gsl_qrng* qrng;
    unsigned int calls;
    gsl_monte_function GMC;
    cubstruct.KernelnD_fct=intKern_fct;
    cubstruct.param=param;
    GMC.f = &Kernel_MCnD;
    GMC.params=&cubstruct;
    GMC.dim=ndim;
    struct timeval tv; // Seed generation based on time
    gettimeofday(&tv,0);
    unsigned long mySeed = tv.tv_sec + tv.tv_usec;
    volume_ndim = 1;
    for (j=0;j<ndim;j++) {
        volume_ndim *= (int_end[j]-int_start[j]);
    }
    done=0;
    *error=0;
	switch (sasfit_get_int_strategy()) {
        case H_CUBATURE :
                hcubature(1, &Kernel_cub_nD,&cubstruct,ndim, int_start, int_end,
                        ndim*limit, epsabs, epsrel, ERROR_PAIRED,
                        fval, ferr);
                *result = fval[0];
                *error = ferr[0];
                done=1;
                break;
        case P_CUBATURE :
                pcubature(1, &Kernel_cub_nD,&cubstruct,ndim, int_start, int_end,
                        ndim*limit, epsabs, epsrel, ERROR_PAIRED,
                        fval, ferr);
                *result = fval[0];
                *error = ferr[0];
                done=1;
                break;
        case GSL_QMC_NIEDERREITER_2 :
                calls = gsl_max(sasfit_eps_get_iter_4_mc(),20);
                qrng = gsl_qrng_alloc(gsl_qrng_niederreiter_2, ndim);
                quasi_monte_state* s_niederreiter_2 = quasi_monte_alloc(ndim);
                quasi_monte_integrate(&GMC, int_start, int_end, ndim, calls, epsrel, epsabs, qrng, s_niederreiter_2,
                                result, error);
                quasi_monte_free(s_niederreiter_2);
                gsl_qrng_free(qrng);
                done=1;
                break;
        case GSL_QMC_SOBOL :
                calls = gsl_max(sasfit_eps_get_iter_4_mc(),20);
                qrng = gsl_qrng_alloc(gsl_qrng_sobol, ndim);
                quasi_monte_state* s_sobol = quasi_monte_alloc(ndim);
                quasi_monte_integrate(&GMC, int_start, int_end, ndim, calls, epsrel, epsabs, qrng, s_sobol,
                                result, error);
                quasi_monte_free(s_sobol);
                gsl_qrng_free(qrng);
                done=1;
                break;
        case GSL_QMC_HALTON :
                calls = gsl_max(sasfit_eps_get_iter_4_mc(),20);
                qrng = gsl_qrng_alloc(gsl_qrng_halton, ndim);
                quasi_monte_state* s_halton = quasi_monte_alloc(ndim);
                quasi_monte_integrate(&GMC, int_start, int_end, ndim, calls, epsrel, epsabs, qrng, s_halton,
                                result, error);
                quasi_monte_free(s_halton);
                gsl_qrng_free(qrng);
                done=1;
                break;
        case GSL_QMC_REVERSEHALTON :
                calls = gsl_max(sasfit_eps_get_iter_4_mc(),20);
                qrng = gsl_qrng_alloc(gsl_qrng_reversehalton, ndim);
                quasi_monte_state* s_r_halton = quasi_monte_alloc(ndim);
                quasi_monte_integrate(&GMC, int_start, int_end, ndim, calls, epsrel, epsabs, qrng, s_r_halton,
                                result, error);
                quasi_monte_free(s_r_halton);
                gsl_qrng_free(qrng);
                done=1;
                break;
        case GSL_RQMC_SOBOL_RDS :
                calls = gsl_max(sasfit_eps_get_iter_4_mc(),20);
                quasi_monte_state* s_sobol_rds = quasi_monte_alloc(ndim);
                randomized_quasi_monte_integrate(&GMC, int_start, int_end, ndim, calls, epsrel, epsabs, &genpoint_sobol_rds, s_sobol_rds,
                                result, error);
                quasi_monte_free(s_sobol_rds);
                done=1;
                break;
        case GSL_RQMC_SOBOL_OWEN :
                calls = gsl_max(sasfit_eps_get_iter_4_mc(),20);
                quasi_monte_state* s_sobol_owen = quasi_monte_alloc(ndim);
                randomized_quasi_monte_integrate(&GMC, int_start, int_end, ndim, calls, epsrel, epsabs, &genpoint_sobol_owen, s_sobol_owen,
                                result, error);
                quasi_monte_free(s_sobol_owen);
                done=1;
                break;
        case GSL_RQMC_FAURE05_OWEN :
                calls = gsl_max(sasfit_eps_get_iter_4_mc(),20);
                quasi_monte_state* s_faure05_owen = quasi_monte_alloc(ndim);
                randomized_quasi_monte_integrate(&GMC, int_start, int_end, ndim, calls, epsrel, epsabs, &genpoint_faure05_owen, s_faure05_owen,
                                result, error);
                quasi_monte_free(s_faure05_owen);
                done=1;
                break;
        case GSL_RQMC_LAINE_KARRAS :
                calls = gsl_max(sasfit_eps_get_iter_4_mc(),20);
                quasi_monte_state* s_laine_karras = quasi_monte_alloc(ndim);
                randomized_quasi_monte_integrate(&GMC, int_start, int_end, ndim, calls, epsrel, epsabs, &genpoint_laine_karras, s_laine_karras,
                                result, error);
                quasi_monte_free(s_laine_karras);
                done=1;
                break;
        case GSL_MC_PLAIN :
                calls = gsl_max(sasfit_eps_get_iter_4_mc(),50);
                gsl_rng_env_setup ();
                T = gsl_rng_default;
                r = gsl_rng_alloc (T);
                gsl_rng_set(r, mySeed);
                gsl_monte_plain_state *s_plain = gsl_monte_plain_alloc(ndim);
                gsl_monte_plain_integrate (&GMC, int_start, int_end, ndim, calls, r, s_plain,
                                result, error);
                gsl_monte_plain_free (s_plain);
                gsl_rng_free(r);
                done=1;
                break;
        case GSL_MC_VEGAS :
                calls = gsl_max(sasfit_eps_get_iter_4_mc(),50);
                gsl_rng_env_setup ();
                T = gsl_rng_default;
                r = gsl_rng_alloc (T);
                gsl_rng_set(r, mySeed);
                gsl_monte_vegas_state *s_vegas = gsl_monte_vegas_alloc(ndim);
                gsl_monte_vegas_integrate (&GMC, int_start, int_end, ndim, calls/10, r, s_vegas,
                                result, error);
                i=1;
                do
                {
                    gsl_monte_vegas_integrate (&GMC, int_start, int_end, ndim, calls/10, r, s_vegas,
                                    result, error);
                    i++;
                } while (i<10 && ((fabs (gsl_monte_vegas_chisq (s_vegas) - 1.0) > 0.5) || (*error)/(*result)>epsrel));
//                sasfit_out("VEGAS: number of calls:%d\t ferr/fval=%lf\n",i*calls/10,ferr[0]/fval[0]);
                gsl_monte_vegas_free (s_vegas);
                gsl_rng_free(r);
                done=1;
                break;
        case GSL_MC_MISER :
                calls = gsl_max(sasfit_eps_get_iter_4_mc(),50);
                gsl_rng_env_setup ();
                T = gsl_rng_default;
                r = gsl_rng_alloc (T);
                gsl_rng_set(r, mySeed);
                gsl_monte_miser_state *s_miser = gsl_monte_miser_alloc(ndim);
                gsl_monte_miser_integrate (&GMC, int_start, int_end, ndim, calls, r, s_miser,
                                result, error);
                gsl_monte_miser_free (s_miser);
                gsl_rng_free(r);
                done=1;
                break;
        case SG_SMOLYAK_CLENSHAW_CURTIS: // sparse_grid_cc   [-1;1]
                if (cg == NULL) {
                    cg = &cg_static;
                    cg->int_strategy=sasfit_get_int_strategy();
                    cg->dim = ndim;
                    cg->k = GSL_MAX(1,sasfit_get_sg_level());
                    cg->n = sparse_grid_cfn_size ( cg->dim, cg->k );
                    cg->n2=cg->n;
                    cg->volume_ndim=volume_ndim;
                    cg->x = ( double * ) malloc ( cg->dim * cg->n * sizeof ( double ) );
                    cg->w = ( double * ) malloc ( cg->n * sizeof ( double ) );
                    cg->x_ab = ( double * ) malloc ( cg->dim * sizeof ( double ) );
                    sparse_grid_cc ( cg->dim, cg->k, cg->n, cg->w, cg->x );
                    sasfit_out("first call to generate grid\n");
                    sasfit_out("dim=%d, n=%d, k=%d, int_strategy:%d,%d\n",cg->dim, cg->n,cg->k,cg->int_strategy,sasfit_get_int_strategy());
                    fflush(stdout);
                } else {
                    k=GSL_MAX(1,sasfit_get_sg_level());
                    if (!(ndim==cg->dim &&
                        cg->k==k &&
                        cg->int_strategy==sasfit_get_int_strategy()))
                    {
                        sasfit_out("generate new sparse-grid\n");
                        free(cg->x);
                        free(cg->x_ab);
                        free(cg->w);

                        cg->int_strategy=sasfit_get_int_strategy();
                        cg->dim = ndim;
                        cg->k = k;
                        cg->n = sparse_grid_cfn_size ( cg->dim, cg->k );
                        cg->n2=cg->n;
                        cg->volume_ndim=volume_ndim;
                        cg->x = ( double * ) malloc ( cg->dim * cg->n * sizeof ( double ) );
                        cg->w = ( double * ) malloc ( cg->n * sizeof ( double ) );
                        cg->x_ab = ( double * ) malloc ( cg->dim * sizeof ( double ) );
                        sparse_grid_cc ( cg->dim, cg->k, cg->n, cg->w, cg->x );
                        sasfit_out("dim=%d, n=%d, k=%d, int_strategy:%d,%d\n",cg->dim, cg->n,k,cg->int_strategy,sasfit_get_int_strategy());
                        fflush(stdout);
                        sasfit_out("generated new one .... done\n");
                    }
                }
                *result = 0;
                *error = 0;
                cg->wt=0;
                for (i=0;i<cg->n;i++) {
                    for (j=0;j<cg->dim;j++) {
                        cg->x_ab[j]=int_start[j] + (1+cg->x[j+i*cg->dim])*(int_end[j]-int_start[j])/2.;
                    }
                    cg->wt += cg->w[i];
                    *result = (*result) + 0.5*Kernel_MCnD(cg->x_ab,cg->dim,&cubstruct) *cg->w[i];
                }
                // sasfit_out("volume:%lf\t sum w[i]=%lf\n",volume_ndim,cg->wt);
                *result *= volume_ndim;
                done=1;
                break;
        case SG_CLENSHAW_CURTIS_LINEAR: // ccl_order [0,1]
                if (cg == NULL) {
                    cg = &cg_static;
                    cg->int_strategy=sasfit_get_int_strategy();
                    cg->dim = ndim;
                    cg->k = GSL_MAX(1,sasfit_get_sg_level());
                    cg->n = nwspgr_size ( ccl_order, cg->dim, cg->k );
                    cg->n2=cg->n;
                    cg->volume_ndim=volume_ndim;
                    cg->x = ( double * ) malloc ( cg->dim * cg->n * sizeof ( double ) );
                    cg->w = ( double * ) malloc ( cg->n * sizeof ( double ) );
                    cg->x_ab = ( double * ) malloc ( cg->dim * sizeof ( double ) );
                    nwspgr ( cc, ccl_order, cg->dim, cg->k, cg->n, &cg->n2, cg->x, cg->w );
                    sasfit_out("first call to generate grid\n");
                    sasfit_out("dim=%d, n=%d, k=%d, int_strategy:%d,%d\n",cg->dim, cg->n,cg->k,cg->int_strategy,sasfit_get_int_strategy());
                    fflush(stdout);
                } else {
                    k=GSL_MAX(1,sasfit_get_sg_level());
                    if (!(ndim==cg->dim &&
                        cg->k==k &&
                        cg->int_strategy==sasfit_get_int_strategy()))
                    {
                        sasfit_out("generate new sparse-grid\n");

                        free(cg->x);
                        free(cg->x_ab);
                        free(cg->w);

                        cg->int_strategy=sasfit_get_int_strategy();
                        cg->dim = ndim;
                        cg->k = k;
                        cg->n = nwspgr_size ( ccl_order, cg->dim, cg->k );
                        cg->n2=cg->n;
                        cg->volume_ndim=volume_ndim;
                        cg->x = ( double * ) malloc ( cg->dim * cg->n * sizeof ( double ) );
                        cg->w = ( double * ) malloc ( cg->n * sizeof ( double ) );
                        cg->x_ab = ( double * ) malloc ( cg->dim * sizeof ( double ) );
                        nwspgr ( cc, ccl_order, cg->dim, cg->k, cg->n, &cg->n2, cg->x, cg->w );
                        sasfit_out("dim=%d, n=%d, k=%d, int_strategy:%d,%d\n",cg->dim, cg->n,k,cg->int_strategy,sasfit_get_int_strategy());
                        fflush(stdout);
                        sasfit_out("generated new one .... done\n");
                        fflush(stdout);
                    }
                }
//                sasfit_out("number of points in sparse grid (SG_CLENSHAW_CURTIS_LINEAR): %d, %d\n",n,n2);
                *result = 0;
                *error = 0;
                cg->wt = 0;
                for (i=0;i<cg->n2;i++) {
                    for (j=0;j<cg->dim;j++) {
                        cg->x_ab[j]=int_start[j] + cg->x[j+i*cg->dim]*(int_end[j]-int_start[j]);
                    }
                    cg->wt += cg->w[i];
                    *result = (*result) + Kernel_MCnD(cg->x_ab,cg->dim,&cubstruct) *cg->w[i];
                }
                // sasfit_out("volume:%lf\t sum w[i]=%lf\n",volume_ndim,wt);
                *result *= volume_ndim;
                done=1;
                break;
        case SG_CLENSHAW_CURTIS_SLOW: // ccs_order  [0,1]
                if (cg == NULL) {
                    cg = &cg_static;
                    cg->int_strategy=SG_CLENSHAW_CURTIS_SLOW;
                    cg->dim = ndim;
                    cg->k = GSL_MAX(1,sasfit_get_sg_level());
                    cg->n = nwspgr_size ( ccs_order, cg->dim, cg->k );
                    cg->n2=cg->n;
                    cg->volume_ndim=volume_ndim;
                    cg->x = ( double * ) malloc ( cg->dim * cg->n * sizeof ( double ) );
                    cg->w = ( double * ) malloc ( cg->n * sizeof ( double ) );
                    cg->x_ab = ( double * ) malloc ( cg->dim * sizeof ( double ) );
                    nwspgr ( cc, ccs_order, cg->dim, cg->k, cg->n, &cg->n2, cg->x, cg->w );
                    sasfit_out("first call to generate grid\n");
                    sasfit_out("dim=%d, n=%d, k=%d, int_strategy:%d,%d\n",cg->dim, cg->n,cg->k,cg->int_strategy,sasfit_get_int_strategy());
                    fflush(stdout);
                } else {
                    k=GSL_MAX(1,sasfit_get_sg_level());
                    if (!(ndim==cg->dim &&
                        cg->k==k &&
                        cg->int_strategy==SG_CLENSHAW_CURTIS_SLOW))
                    {
                        sasfit_out("generate new sparse-grid\n");

                        free(cg->x);
                        free(cg->x_ab);
                        free(cg->w);

                        cg->int_strategy=SG_CLENSHAW_CURTIS_SLOW;
                        cg->dim = ndim;
                        cg->k = GSL_MAX(1,sasfit_get_sg_level());
                        cg->n = nwspgr_size ( ccs_order, cg->dim, cg->k );
                        cg->n2=cg->n;
                        cg->volume_ndim=volume_ndim;
                        cg->x = ( double * ) malloc ( cg->dim * cg->n * sizeof ( double ) );
                        cg->w = ( double * ) malloc ( cg->n * sizeof ( double ) );
                        cg->x_ab = ( double * ) malloc ( cg->dim * sizeof ( double ) );
                        nwspgr ( cc, ccs_order, cg->dim, cg->k, cg->n, &cg->n2, cg->x, cg->w );
                        sasfit_out("dim=%d, n=%d, k=%d, int_strategy:%d,%d\n",cg->dim, cg->n,k,cg->int_strategy,sasfit_get_int_strategy());
                        fflush(stdout);
                        sasfit_out("generated new one .... done\n");
                        fflush(stdout);
                    }
                }
//                sasfit_out("number of points in sparse grid (SG_CLENSHAW_CURTIS_SLOW): %d, %d\n",n,n2);
                *result = 0;
                *error = 0;
                cg->wt = 0;
                for (i=0;i<cg->n2;i++) {
                    for (j=0;j<cg->dim;j++) {
                        cg->x_ab[j]=int_start[j] + cg->x[j+i*cg->dim]*(int_end[j]-int_start[j]);
                    }
                    cg->wt += cg->w[i];
                    *result = (*result) + Kernel_MCnD(cg->x_ab,cg->dim,&cubstruct) * cg->w[i];
                }
                // sasfit_out("volume:%lf\t sum w[i]=%lf\n",volume_ndim,wt);
                *result *= volume_ndim;
                done=1;
                break;
        case SG_CLENSHAW_CURTIS_EXP: // cce_order  [0,1]
                if (cg == NULL) {
                    cg = &cg_static;
                    cg->int_strategy=SG_CLENSHAW_CURTIS_EXP;
                    cg->dim = ndim;
                    cg->k = GSL_MAX(1,sasfit_get_sg_level());
                    cg->n = nwspgr_size ( cce_order, cg->dim, cg->k );
                    cg->n2=cg->n;
                    cg->volume_ndim=volume_ndim;
                    cg->x = ( double * ) malloc ( cg->dim * cg->n * sizeof ( double ) );
                    cg->w = ( double * ) malloc ( cg->n * sizeof ( double ) );
                    cg->x_ab = ( double * ) malloc ( cg->dim * sizeof ( double ) );
                    nwspgr ( cc, cce_order, cg->dim, cg->k, cg->n, &cg->n2, cg->x, cg->w );
                    sasfit_out("first call to generate grid\n");
                    sasfit_out("dim=%d, n=%d, k=%d, int_strategy:%d,%d\n",cg->dim, cg->n,cg->k,cg->int_strategy,sasfit_get_int_strategy());
                    fflush(stdout);
                } else {
                    k=GSL_MAX(1,sasfit_get_sg_level());
                    if (!(ndim==cg->dim &&
                        cg->k==k &&
                        cg->int_strategy==SG_CLENSHAW_CURTIS_EXP))
                    {
                        sasfit_out("generate new sparse-grid\n");

                        free(cg->x);
                        free(cg->x_ab);
                        free(cg->w);

                        cg->int_strategy=SG_CLENSHAW_CURTIS_EXP;
                        cg->dim = ndim;
                        cg->k = GSL_MAX(1,sasfit_get_sg_level());
                        cg->n = nwspgr_size ( cce_order, cg->dim, cg->k );
                        cg->n2=cg->n;
                        cg->volume_ndim=volume_ndim;
                        cg->x = ( double * ) malloc ( cg->dim * cg->n * sizeof ( double ) );
                        cg->w = ( double * ) malloc ( cg->n * sizeof ( double ) );
                        cg->x_ab = ( double * ) malloc ( cg->dim * sizeof ( double ) );
                        nwspgr ( cc, cce_order, cg->dim, cg->k, cg->n, &cg->n2, cg->x, cg->w );
                        sasfit_out("dim=%d, n=%d, k=%d, int_strategy:%d,%d\n",cg->dim, cg->n,k,cg->int_strategy,sasfit_get_int_strategy());
                        fflush(stdout);
                        sasfit_out("generated new one .... done\n");
                        fflush(stdout);
                    }
                }
//                sasfit_out("number of points in sparse grid (SG_CLENSHAW_CURTIS_EXP): %d, %d\n",n,n2);
                *result = 0;
                *error = 0;
                cg->wt = 0;
                for (i=0;i<cg->n2;i++) {
                    for (j=0;j<cg->dim;j++) {
                        cg->x_ab[j]=int_start[j] + cg->x[j+i*cg->dim]*(int_end[j]-int_start[j]);
                    }
                    cg->wt += cg->w[i];
                    *result = (*result) + Kernel_MCnD(cg->x_ab,cg->dim,&cubstruct) * cg->w[i];
                }
                // sasfit_out("volume:%lf\t sum w[i]=%lf\n",volume_ndim,wt);
                *result *= volume_ndim;
                done=1;
                break;
        case SG_GAUSS_LEGENDRE: // gqu  [0,1]
                if (cg == NULL) {
                    cg = &cg_static;
                    cg->int_strategy=SG_GAUSS_LEGENDRE;
                    cg->dim = ndim;
                    cg->k = GSL_MIN(GSL_MAX(1,sasfit_get_sg_level()),25);
                    cg->n = nwspgr_size ( gqu_order, cg->dim, cg->k );
                    cg->n2=cg->n;
                    cg->volume_ndim=volume_ndim;
                    cg->x = ( double * ) malloc ( cg->dim * cg->n * sizeof ( double ) );
                    cg->w = ( double * ) malloc ( cg->n * sizeof ( double ) );
                    cg->x_ab = ( double * ) malloc ( cg->dim * sizeof ( double ) );
                    nwspgr ( gqu, gqu_order, cg->dim, cg->k, cg->n, &cg->n2, cg->x, cg->w );
                    sasfit_out("first call to generate grid\n");
                    sasfit_out("dim=%d, n=%d, k=%d, int_strategy:%d,%d\n",cg->dim, cg->n,cg->k,cg->int_strategy,sasfit_get_int_strategy());
                    fflush(stdout);
                } else {
                    k=GSL_MIN(GSL_MAX(1,sasfit_get_sg_level()),25);
                    if (!(ndim==cg->dim &&
                        cg->k==k &&
                        cg->int_strategy==SG_GAUSS_LEGENDRE))
                    {
                        sasfit_out("generate new sparse-grid\n");

                        free(cg->x);
                        free(cg->x_ab);
                        free(cg->w);

                        cg->int_strategy=SG_GAUSS_LEGENDRE;
                        cg->dim = ndim;
                        cg->k = GSL_MIN(GSL_MAX(1,sasfit_get_sg_level()),25);
                        cg->n = nwspgr_size ( gqu_order, cg->dim, cg->k );
                        cg->n2=cg->n;
                        cg->volume_ndim=volume_ndim;
                        cg->x = ( double * ) malloc ( cg->dim * cg->n * sizeof ( double ) );
                        cg->w = ( double * ) malloc ( cg->n * sizeof ( double ) );
                        cg->x_ab = ( double * ) malloc ( cg->dim * sizeof ( double ) );
                        nwspgr ( gqu, gqu_order, cg->dim, cg->k, cg->n, &cg->n2, cg->x, cg->w );
                        sasfit_out("dim=%d, n=%d, k=%d, int_strategy:%d,%d\n",cg->dim, cg->n,k,cg->int_strategy,sasfit_get_int_strategy());
                        fflush(stdout);
                        sasfit_out("generated new one .... done\n");
                        fflush(stdout);
                    }
                }

//                sasfit_out("number of points in sparse grid (SG_GAUSS_LEGENDRE): %d, %d\n",n,n2);
                *result = 0;
                *error = 0;
                cg->wt = 0;
                for (i=0;i<cg->n2;i++) {
                    for (j=0;j<cg->dim;j++) {
                        cg->x_ab[j]=int_start[j] + cg->x[j+i*cg->dim]*(int_end[j]-int_start[j]);
                    }
                    cg->wt += cg->w[i];
                    *result = (*result) + Kernel_MCnD(cg->x_ab,cg->dim,&cubstruct) *cg->w[i];
                }
                // sasfit_out("volume:%lf\t sum w[i]=%lf\n",volume_ndim,wt);
                *result *= volume_ndim;
                done=1;
                break;
        case SG_KONROD_PATTERSON: //kpu  [0,1]
                if (cg == NULL) {
                    cg = &cg_static;
                    cg->int_strategy=SG_KONROD_PATTERSON;
                    cg->dim = ndim;
                    cg->k = GSL_MIN(GSL_MAX(1,sasfit_get_sg_level()),25);
                    cg->n = nwspgr_size ( kpu_order, cg->dim, cg->k );
                    cg->n2=cg->n;
                    cg->volume_ndim=volume_ndim;
                    cg->x = ( double * ) malloc ( cg->dim * cg->n * sizeof ( double ) );
                    cg->w = ( double * ) malloc ( cg->n * sizeof ( double ) );
                    cg->x_ab = ( double * ) malloc ( cg->dim * sizeof ( double ) );
                    nwspgr ( kpu, kpu_order, cg->dim, cg->k, cg->n, &cg->n2, cg->x, cg->w );
                    sasfit_out("first call to generate grid\n");
                    sasfit_out("dim=%d, n=%d, k=%d, int_strategy:%d,%d\n",cg->dim, cg->n,cg->k,cg->int_strategy,sasfit_get_int_strategy());
                    fflush(stdout);
                } else {
                    k=GSL_MIN(GSL_MAX(1,sasfit_get_sg_level()),25);
                    if (!(ndim==cg->dim &&
                        cg->k==k &&
                        cg->int_strategy==SG_KONROD_PATTERSON))
                    {
                        sasfit_out("generate new sparse-grid\n");

                        free(cg->x);
                        free(cg->x_ab);
                        free(cg->w);

                        cg->int_strategy=SG_KONROD_PATTERSON;
                        cg->dim = ndim;
                        cg->k = GSL_MIN(GSL_MAX(1,sasfit_get_sg_level()),25);
                        cg->n = nwspgr_size ( kpu_order, cg->dim, cg->k );
                        cg->n2=cg->n;
                        cg->volume_ndim=volume_ndim;
                        cg->x = ( double * ) malloc ( cg->dim * cg->n * sizeof ( double ) );
                        cg->w = ( double * ) malloc ( cg->n * sizeof ( double ) );
                        cg->x_ab = ( double * ) malloc ( cg->dim * sizeof ( double ) );
                        nwspgr ( kpu, kpu_order, cg->dim, cg->k, cg->n, &cg->n2, cg->x, cg->w );
                        sasfit_out("dim=%d, n=%d, k=%d, int_strategy:%d,%d\n",cg->dim, cg->n,k,cg->int_strategy,sasfit_get_int_strategy());
                        fflush(stdout);
                        sasfit_out("generated new one .... done\n");
                        fflush(stdout);
                    }
                }
//                sasfit_out("number of points in sparse grid (SG_KONROD_PATTERSON): %d, %d\n",n,n2);
                *result = 0;
                *error = 0;
                cg->wt = 0;
                for (i=0;i<cg->n2;i++) {
                    for (j=0;j<cg->dim;j++) {
                        cg->x_ab[j]=int_start[j] + cg->x[j+i*cg->dim]*(int_end[j]-int_start[j]);
                    }
                    cg->wt += cg->w[i];
                    *result = (*result) + Kernel_MCnD(cg->x_ab,cg->dim,&cubstruct) *cg->w[i];
                }
                // sasfit_out("volume:%lf\t sum w[i]=%lf\n",volume_ndim,wt);
                *result *= volume_ndim;
                done=1;
                break;
        case SG_FROLOV:
                if (ndim<2 || ndim>10) {
                    sasfit_err("Frolov sparse grids are only available for dimensions 2 to 10 in SASfit so far\n");
                    break;
                }
                if (cg == NULL) {
                    cg = &cg_static;
                    cg->int_strategy=SG_FROLOV;
                    cg->dim = ndim;
                    cg->k = GSL_MIN(GSL_MAX(1,sasfit_get_sg_level()),14);
                    cg->x_ab = ( double * ) malloc ( cg->dim * sizeof ( double ) );
                    cg->w = ( double * ) malloc (  sizeof ( double ) );

                    cg->x=Frolov(cg->dim,cg->k,&cg->w[0],&cg->n,&i);
                    sasfit_out("Frolov status:%d\n",i);
                    fflush(stdout);
                    if (cg->x == NULL) {
                        sasfit_err("could not allocate memory for Frolov sparse grid (k*dim)-points=%d, weight:%lf\n",ndim*cg->k,cg->w[0]);
                        fflush(stdout);
                        free(cg->w);
                        free(cg->x_ab);
                        cg=NULL;
                        break;
                    };
                    cg->n2=cg->n;
                    cg->volume_ndim=volume_ndim;
                    sasfit_out("first call to generate grid\n");
                    sasfit_out("dim=%d, n=%d, k=%d, int_strategy:%d,%d\n",cg->dim, cg->n,cg->k,cg->int_strategy,sasfit_get_int_strategy());
                    fflush(stdout);
                } else {
                    k=GSL_MIN(GSL_MAX(1,sasfit_get_sg_level()),14);
                    if (!(ndim==cg->dim &&
                        cg->k==k &&
                        cg->int_strategy==SG_FROLOV))
                    {
                        sasfit_out("generate new sparse-grid\n");

                        free(cg->x);
                        free(cg->w);
                        free(cg->x_ab);

                        cg->int_strategy=SG_FROLOV;
                        cg->dim = ndim;
                        cg->k = GSL_MIN(GSL_MAX(1,sasfit_get_sg_level()),14);
                        cg->x_ab = ( double * ) malloc ( cg->dim * sizeof ( double ) );
                        cg->w = ( double * ) malloc (  sizeof ( double ) );
                        cg->x=Frolov(cg->dim,cg->k,&cg->w[0],&cg->n,&i);
                        sasfit_out("Frolov status:%d\n",i);
                        fflush(stdout);
                        if (cg->x == NULL) {
                            sasfit_err("could not allocate memory for Frolov sparse grid (k*dim)-points=%d, weight:%lf\n",ndim*cg->k,cg->w[0]);
                            fflush(stdout);
                            free(cg->w);
                            free(cg->x_ab);
                            cg=NULL;
                            break;
                        };
                        cg->n2=cg->n;
                        cg->volume_ndim=volume_ndim;
                        sasfit_out("dim=%d, n=%d, k=%d, int_strategy:%d,%d, weight:%lf\n",cg->dim, cg->n,k,cg->int_strategy,sasfit_get_int_strategy(),cg->w[0]);
                        fflush(stdout);
                        sasfit_out("generated new one .... done\n");
                        fflush(stdout);
                    }
                }
//                sasfit_out("number of points in sparse grid (SG_FROLOV): %d, %d\n",n,n2);
                *result = 0;
                *error = 0;
                cg->wt = 0;
                for (i=0;i<cg->n2;i++) {
                    for (j=0;j<cg->dim;j++) {
                        cg->x_ab[j]=int_start[j] + cg->x[j+i*cg->dim]*(int_end[j]-int_start[j]);
                    }
                    cg->wt += cg->w[0];
                    *result = (*result) + Kernel_MCnD(cg->x_ab,cg->dim,&cubstruct) * cg->w[0];
                }
                // sasfit_out("volume:%lf\t sum w[i]=%lf\n",volume_ndim,wt);
                *result *= volume_ndim;
                done=1;
                break;
        case SG_SMOLYAK :
                *result = int_smolyak (ndim, GSL_MAX(3,GSL_MIN(abs(sasfit_get_sg_level()),47+ndim)) , int_start, int_end,
                                       &Kernel_MCnD, &cubstruct, 0 );
                *error = 0;
                *error = 0;
                *result *= volume_ndim;
                done=1;
                break;
        case SG_CC_SMOLYAK :
                *result = cc_int_smolyak (ndim, GSL_MAX(3,GSL_MIN(abs(sasfit_get_sg_level()),11+ndim)) , int_start, int_end,
                                       &Kernel_MCnD, &cubstruct, 0 );
                *error = 0;
                *result *= volume_ndim;
                done=1;
                break;
        case OOURA_DOUBLE_EXP_QUADRATURE :
                integration_strategy = GDMI_INTE_FUNCTIONS_OOURA_DE;
                break;
        case TANHSINH_1 :
                integration_strategy = GDMI_INTE_FUNCTIONS_TANHSINH_1;
                break;
        case TANHSINH_2 :
                integration_strategy = GDMI_INTE_FUNCTIONS_TANHSINH_2;
                break;
        case OOURA_CLENSHAW_CURTIS_QUADRATURE :
                integration_strategy = GDMI_INTE_FUNCTIONS_OOURA_CC;
                break;
        case LOBATTO:
                integration_strategy = GDMI_INTE_FUNCTIONS_LOBATTO;
                break;
        case GSL_CQUAD :
                integration_strategy = GDMI_INTE_FUNCTIONS_CQUAD;
                break;
        case GSL_QAG :
                integration_strategy = GMDI_INTE_FUNCTIONS_QAG;
                break;
        case GSL_QAGS :
                integration_strategy = GMDI_INTE_FUNCTIONS_QAGS;
                break;
        case GSL_QNG :
                integration_strategy = GMDI_INTE_FUNCTIONS_QNG;
                break;
        case GSL_GAUSSLEGENDRE :
                integration_strategy = GDMI_INTE_FUNCTIONS_GAUSSLEGENDRE;
                break;
        case GSL_CHEBYSHEV1 :
                integration_strategy = GDMI_INTE_FUNCTIONS_CHEBYSHEV1;
                break;
        case GSL_CHEBYSHEV2 :
                integration_strategy = GDMI_INTE_FUNCTIONS_CHEBYSHEV2;
                break;
        case GSL_GEGENBAUER :
                integration_strategy = GDMI_INTE_FUNCTIONS_GEGENBAUER;
                break;
        case GSL_EXPONENTIAL :
                integration_strategy = GDMI_INTE_FUNCTIONS_EXPONENTIAL;
                break;
        case GSL_LAGUERRE :
                integration_strategy = GDMI_INTE_FUNCTIONS_LAGUERRE;
                break;
        case GSL_JACOBI :
                integration_strategy = GDMI_INTE_FUNCTIONS_GSL_JACOBI;
                break;
        default :
                integration_strategy = GDMI_INTE_FUNCTIONS_CQUAD;
                break;
	}
	if (done) {
        gmdi_free_inte_handle(handle);
        if ((*error)/(*result) >= epsrel) {
            ret = 1;
            // sasfit_err("integration (sasfit_cubature) failed with the required precision, err/res=%le>eps=%le\n",(*error)/(*result),epsrel);
            return SASFIT_RETURNVAL_ON_ERROR;
        }
        return ret;
	}


    fc.type = GMDI_FUNCTION_OR_CONSTANT_TYPE_CONSTANT;
	for (i=0;i<ndim;i++) {
		gmdi_handle_set_key(handle, GSL_INTEG_GAUSS61, i);
		gmdi_handle_set_epsabs(handle, epsabs, i);
		gmdi_handle_set_epsrel(handle, epsrel, i);
		gmdi_handle_set_limit(handle, limit, i);
		gmdi_handle_set_inte_func(handle, integration_strategy, i);
		fc.content.c=int_start[i];
		gmdi_handle_set_x0(handle, &fc, i);
		fc.content.c=int_end[i];
		gmdi_handle_set_x1(handle, &fc, i);
	}

    mvf.n = ndim;
    mvf.function = *intKern_fct;
    mvf.params = param;
    gmdi_handle_set_kernel(handle, &mvf);

    gmdi_multi_dimensional_integration(handle);

    *result = gmdi_handle_get_result(handle);
    *error = gmdi_handle_get_abserr(handle);

    gmdi_free_inte_handle(handle);

    if ((*error)/(*result) >= epsrel)
    {
        ret = 1;
        sasfit_err("integration (sasfit_cubature) failed with the required precision, err/res=%le>eps=%le\n",(*error)/(*result),epsrel);
        return SASFIT_RETURNVAL_ON_ERROR;
    }

    return ret;
}
