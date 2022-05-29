/*
 * src/sasfit_core/sasfit_analytPar_utils.c
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

#include <gsl/gsl_math.h>
#include <gsl/gsl_vector.h>
#include "string.h"
#include "tcl.h"
#include "include/sasfit_core.h"


void sasfit_ap2paramlist(int              ** idx_list,
                         int               * max_param_idx,
                         int               * fit_idx,
                         scalar           ** param_list,
                         sasfit_analytpar  * ap,
                         sasfit_commonpar  * gcp,
                         int                 contrib_num)
{
	int i,j,k,idx;
	int nofit_idx;

	SASFIT_ASSERT_VOID(idx_list && max_param_idx && fit_idx && param_list && ap);

	nofit_idx = 1;
	(*fit_idx) = 0;
	(*max_param_idx) = contrib_num*(3*MAXPAR);
	(*idx_list) = ivector(0,(*max_param_idx)-1);
	(*param_list) = dvector(0,(*max_param_idx)-1);

	for (i=0; i < contrib_num; i++)
	{
		k = 0;
		for (j=0; j < MAXPAR; j++)
		{
			idx = i*(3*MAXPAR)+k;
			if ((ap[i].SD_active[j] == 1) &&
			     ap[i].fitSDFF && ap[i].calcSDFF)
			{
				if (gcp && ap[i].SD_index[j] != 0) {
					if ( gcp->P_common_index[ap[i].SD_index[j]-1] < 0) {
						gcp->P_common_index[ap[i].SD_index[j]-1] = idx;
						(*idx_list)[(*fit_idx)] = idx;
						(*fit_idx)++;
					} else {
						(*idx_list)[(*max_param_idx)-nofit_idx] = idx;
						nofit_idx++;
					}
				} else {
					(*idx_list)[(*fit_idx)] = idx;
					(*fit_idx)++;
				}
			} else {
				(*idx_list)[(*max_param_idx)-nofit_idx] = idx;
				nofit_idx++;
			}
			if (gcp && ap[i].SD_index[j] != 0) {
				(*param_list)[idx] = gcp->P_common[ap[i].SD_index[j]-1]*ap[i].SD_factor[j];
			} else {
				(*param_list)[idx] = ap[i].SD_a[j];
			}
			k++;
		}
		for (j=0; j < MAXPAR; j++)
		{
			idx = i*(3*MAXPAR)+k;
			if ((ap[i].FF_active[j] == 1) &&
			     ap[i].fitSDFF && ap[i].calcSDFF)
			{
				if (gcp && ap[i].FF_index[j] != 0) {
					if ( gcp->P_common_index[ap[i].FF_index[j]-1] < 0) {
						gcp->P_common_index[ap[i].FF_index[j]-1] = idx;
						(*idx_list)[(*fit_idx)] = idx;
						(*fit_idx)++;
					} else {
						(*idx_list)[(*max_param_idx)-nofit_idx] = idx;
						nofit_idx++;
					}
				} else {
					(*idx_list)[(*fit_idx)] = idx;
					(*fit_idx)++;
				}
			} else {
				(*idx_list)[(*max_param_idx)-nofit_idx] = idx;
				nofit_idx++;
			}
			if (gcp && ap[i].FF_index[j] != 0) {
				(*param_list)[idx] = gcp->P_common[ap[i].FF_index[j]-1]*ap[i].FF_factor[j];
			} else {
				(*param_list)[idx] = ap[i].FF_l[j];
			}
			k++;
		}
		for (j=0; j < MAXPAR; j++)
		{
			idx = i*(3*MAXPAR)+k;
			if ((ap[i].SQ_active[j] == 1) &&
			     ap[i].fitSDFF && ap[i].calcSDFF)
			{
				if (gcp && ap[i].SQ_index[j] != 0) {
					if ( gcp->P_common_index[ap[i].SQ_index[j]-1] < 0) {
						gcp->P_common_index[ap[i].SQ_index[j]-1] = idx;
						(*idx_list)[(*fit_idx)] = idx;
						(*fit_idx)++;
					} else {
						(*idx_list)[(*max_param_idx)-nofit_idx] = idx;
						nofit_idx++;
					}
				} else {
					(*idx_list)[(*fit_idx)] = idx;
					(*fit_idx)++;
				}
			} else {
				(*idx_list)[(*max_param_idx)-nofit_idx] = idx;
				nofit_idx++;
			}
			if (gcp && ap[i].SQ_index[j] != 0) {
				(*param_list)[idx] = gcp->P_common[ap[i].SQ_index[j]-1]*ap[i].SQ_factor[j];
			} else {
				(*param_list)[idx] = ap[i].SQ_s[j];
			}
			k++;
		}
	} // for all parameter
}

void sasfit_paramlist2ap(scalar            * param_list,
                         sasfit_analytpar  * ap,
                         sasfit_commonpar  * gcp,
                         int                 contrib_num)
{
	int i, j, k, idx;

	SASFIT_ASSERT_VOID(param_list && ap);

	for (i=0; i < contrib_num; i++)
	{
		k = 0;
		for (j=0; j < MAXPAR; j++)
		{
			idx = i*(3*MAXPAR)+k;
			ap[i].SD_a[j] = param_list[idx];
			if (gcp && ap[i].SD_index[j] > 0) {
				ap[i].SD_a[j] = ap[i].SD_factor[j]*gcp->P_common[ap[i].SD_index[j]-1];
			}
			k++;
		}
		for (j=0; j < MAXPAR; j++)
		{
			idx = i*(3*MAXPAR)+k;
			ap[i].FF_l[j] = param_list[idx];
			if (gcp && ap[i].FF_index[j] > 0) {
				ap[i].FF_l[j] = ap[i].FF_factor[j]*gcp->P_common[ap[i].FF_index[j]-1];
			}
			k++;
		}
		for (j=0; j < MAXPAR; j++)
		{
			idx = i*(3*MAXPAR)+k;
			ap[i].SQ_s[j] = param_list[idx];
			if (gcp && ap[i].SQ_index[j] > 0) {
				ap[i].SQ_s[j] = ap[i].SQ_factor[j]*gcp->P_common[ap[i].SQ_index[j]-1];
			}
			k++;
		}
	}
}

void sasfit_covar2ap(scalar ** covar,
                     sasfit_analytpar * ap,
                     sasfit_commonpar * gcp,
                     int contrib_num)
{
	int i, j, fit_idx, nofit_idx;
	int *P_common_index = NULL; // use only if (gcp) !

	SASFIT_ASSERT_VOID(covar && ap);

	if (gcp) {
		P_common_index = ivector(0,(gcp->common_i)-1);
		for (i=0; i < gcp->common_i; i++) {
			P_common_index[i] = gcp->P_common_index[i];
		}
	}

	fit_idx = 0;
	nofit_idx = 0;
	for (i=0; i < contrib_num; i++)
	{
		for (j=0; j < MAXPAR; j++) {
			ap[i].SD_err[j] = 0.0;
		}
		for (j=0; j < MAXPAR; j++) {
	   		ap[i].FF_err[j] = 0.0;
		}
		for (j=0; j < MAXPAR; j++) {
			ap[i].SQ_err[j] = 0.0;
		}
	}

	for (i=0; i < contrib_num; i++)
	{
		for (j=0; j < MAXPAR; j++)
		{
			if ((ap[i].SD_active[j] == 1) &&
			     ap[i].fitSDFF && ap[i].calcSDFF)
			{
				if (gcp && ap[i].SD_index[j] != 0) {
					ap[i].SD_err[j] = sqrt(fabs(covar[fit_idx][fit_idx]));
					gcp->P_common_err[ap[i].SD_index[j]-1] = ap[i].SD_err[j]/ap[i].SD_factor[j];
					if (P_common_index[ap[i].SD_index[j]-1] >= 0) {
						P_common_index[ap[i].SD_index[j]-1] = -1;
						fit_idx++;
					} else {
						nofit_idx++;
					}
				} else {
					ap[i].SD_err[j] = sqrt(fabs(covar[fit_idx][fit_idx]));
					fit_idx++;
				}
			}
		}
		for (j=0; j < MAXPAR; j++)
		{
			if ((ap[i].FF_active[j] == 1) &&
			     ap[i].fitSDFF && ap[i].calcSDFF)
			{
				if (gcp && ap[i].FF_index[j] != 0) {
					ap[i].FF_err[j] = sqrt(fabs(covar[fit_idx][fit_idx]));
					gcp->P_common_err[ap[i].FF_index[j]-1] = ap[i].FF_err[j]/ap[i].FF_factor[j];
					if (P_common_index[ap[i].FF_index[j]-1] >= 0) {
						P_common_index[ap[i].FF_index[j]-1] = -1;
						fit_idx++;
					} else {
						nofit_idx++;
					}
				} else {
					ap[i].FF_err[j] = sqrt(fabs(covar[fit_idx][fit_idx]));
					fit_idx++;
				}
			}
		}

		for (j=0; j < MAXPAR; j++)
		{
			if ((ap[i].SQ_active[j] == 1) &&
			     ap[i].fitSDFF && ap[i].calcSDFF)
			{
				if (gcp && ap[i].SQ_index[j] != 0) {
					ap[i].SQ_err[j] = sqrt(fabs(covar[fit_idx][fit_idx]));
					gcp->P_common_err[ap[i].SQ_index[j]-1] = ap[i].FF_err[j]/ap[i].SQ_factor[j];
					if (P_common_index[ap[i].SQ_index[j]-1] >= 0) {
						P_common_index[ap[i].SQ_index[j]-1] = -1;
						fit_idx++;
					} else {
						nofit_idx++;
					}
				} else {
					ap[i].SQ_err[j] = sqrt(fabs(covar[fit_idx][fit_idx]));
					fit_idx++;
				}
			}
		}
	} // for all contributions
	if (gcp) {
		free_ivector(P_common_index,0,(gcp->common_i)-1);
	}
}

void sasfit_covsrt2ap(scalar           ** covar,
                      sasfit_analytpar  * ap,
                      sasfit_commonpar  * gcp,
                      int                 contrib_num)
{
	int i, j, k, idx;
	int *P_common_index = NULL; // use only if (gcp) !

	SASFIT_ASSERT_VOID(covar && ap);

	if (gcp) {
		P_common_index = ivector(0,(gcp->common_i)-1);
		for (i=0; i < gcp->common_i; i++) P_common_index[i] = gcp->P_common_index[i];
	}

	for (i=0; i < contrib_num; i++) {
		for (j=0; j < MAXPAR; j++) {
			ap[i].SD_err[j] = 0.0;
		}
		for (j=0; j < MAXPAR; j++) {
			ap[i].FF_err[j] = 0.0;
		}
		for (j=0; j < MAXPAR; j++) {
			ap[i].SQ_err[j] = 0.0;
		}
	}

	for (i=0; i < contrib_num; i++) {
		k = 0;
		for (j=0; j < MAXPAR; j++)
		{
			idx = i*(3*MAXPAR)+k;
			if (gcp && ap[i].SD_index[j] != 0) {
				ap[i].SD_err[j] = sqrt(fabs(covar[idx][idx]));
				if (P_common_index[ap[i].SD_index[j]-1] > 0) {
					P_common_index[ap[i].SD_index[j]-1] = -1;
					gcp->P_common_err[ap[i].SD_index[j]-1] = ap[i].SD_err[j]/ap[i].SD_factor[j];
				}
			} else {
				ap[i].SD_err[j] = sqrt(fabs(covar[idx][idx]));
			}
			k++;
		}

		for (j=0; j < MAXPAR; j++)
		{
			idx = i*(3*MAXPAR)+k;
			if (gcp && ap[i].FF_index[j] != 0) {
				ap[i].FF_err[j] = sqrt(fabs(covar[idx][idx]));
				if (P_common_index[ap[i].FF_index[j]-1] > 0) {
					P_common_index[ap[i].FF_index[j]-1] = -1;
					gcp->P_common_err[ap[i].FF_index[j]-1] = ap[i].FF_err[j]/ap[i].FF_factor[j];
				}
			} else {
				ap[i].FF_err[j] = sqrt(fabs(covar[idx][idx]));
			}
			k++;
		}

		for (j=0; j < MAXPAR; j++)
		{
			idx = i*(3*MAXPAR)+k;
			if (gcp && ap[i].SQ_index[j] != 0) {
				ap[i].SQ_err[j] = sqrt(fabs(covar[idx][idx]));
				if (P_common_index[ap[i].SQ_index[j]-1] > 0) {
					P_common_index[ap[i].SQ_index[j]-1] = -1;
					gcp->P_common_err[ap[i].SQ_index[j]-1] = ap[i].SQ_err[j]/ap[i].SQ_factor[j];
				}
			} else {
				ap[i].SQ_err[j] = sqrt(fabs(covar[idx][idx]));
			}
			k++;
		}
	}
	if (gcp) {
		free_ivector(P_common_index,0,(gcp->common_i)-1);
	}
}

void SASFIT_gsl_vector_x2GlobalAP_GCP(gsl_vector * x, void * params)
{
	int i,j,k,index;

	for (j=0;j<=((sasfit_gparam*) params) -> mfit-1;j++)
	{
		i = (int)floor(((sasfit_gparam*) params) -> lista[j]/(3*MAXPAR));
		k = ((sasfit_gparam*) params) -> lista[j]-(int)floor(((sasfit_gparam*) params) -> lista[j]/(3*MAXPAR))*(3*MAXPAR);
		if ((int)floor(k/MAXPAR) == 0)
		{
	  		index = ((sasfit_gparam*) params) -> GlobalAP[i].SD_index[k-(int)floor(k/MAXPAR)*MAXPAR];
	   	} else if ((int)floor(k/MAXPAR) == 1) {
	  		index = ((sasfit_gparam*) params) -> GlobalAP[i].FF_index[k-(int)floor(k/MAXPAR)*MAXPAR];
	   	} else {
	  		index = ((sasfit_gparam*) params) -> GlobalAP[i].SQ_index[k-(int)floor(k/MAXPAR)*MAXPAR];
	   	}


		if (index > 0) {
			((sasfit_gparam*) params) -> GCP.P_common[index-1] = gsl_vector_get(x,j);
	   	} else {
	  		if ((int)floor(k/MAXPAR) == 0) {
				((sasfit_gparam*) params) -> GlobalAP[i].SD_a[k-(int)floor(k/MAXPAR)*MAXPAR]
			     		= gsl_vector_get(x,j);
		  	} else if ((int)floor(k/MAXPAR) == 1) {
		     		((sasfit_gparam*) params) -> GlobalAP[i].FF_l[k-(int)floor(k/MAXPAR)*MAXPAR]
					= gsl_vector_get(x,j);
		  	} else {
		     		((sasfit_gparam*) params) -> GlobalAP[i].SQ_s[k-(int)floor(k/MAXPAR)*MAXPAR]
					= gsl_vector_get(x,j);
		  	}
	   	}
	}
}


int initTypeStr(sasfit_analytpar       * ap,
                 int                     num_ap,
                 const char           ** name,
                 sasfit_function_type    fctType)
{
	int i;
	sasfit_function *fct = 0;
	int (*initfct) (sasfit_analytpar *) = 0;

	SASFIT_ASSERT_VAL(ap && num_ap > 0 && name, TCL_ERROR);

	for (i=0; i < num_ap; i++)
	{
		SASFIT_ASSERT_VAL(name[i], TCL_ERROR);

		// an array of sasfit_function in sasfit_analytpar would avoid such switches (todo later)
		switch(fctType)
		{
			case SDfct:	fct = &(ap[i].SD);
					initfct = sasfit_sd_init;
					break;
			case SQfct:	fct = &(ap[i].SQ);
					initfct = sasfit_sq_init;
					break;
			case FFfct:	fct = &(ap[i].FF);
					initfct = sasfit_ff_init;
					break;
			default:	sasfit_err("sasfit_function_type not found !\n");
					return TCL_ERROR;
		}

		strcpy( fct->typestr, name[i]);

		if ( initfct(&ap[i]) != TCL_OK )
		{
			sasfit_err("could not find function type: %s !\n", fct->typestr);
			return TCL_ERROR;
		}
	}
	return TCL_OK;
}

Tcl_Obj * sasfit_tcl_get_obj(Tcl_Interp * interp,
                             const char * name1,
                             const char * name2)
{
	Tcl_Obj * name1_obj = 0;
	Tcl_Obj * name2_obj = 0;
	Tcl_Obj * val_obj = 0;

	if (!name1) goto exit;
	name1_obj = Tcl_NewStringObj(name1, -1);
	if (!name1_obj) goto exit;
	if (name2) {
		name2_obj = Tcl_NewStringObj(name2, -1);
		if (!name2_obj) goto exit;
	}
	val_obj = Tcl_ObjGetVar2(interp, name1_obj, name2_obj, 0);
//	Tcl_IncrRefCount(val_obj);
exit:
	// name1_obj, name2_obj is cleaned by tcl-interpreter (hopefully)
	return val_obj;
}

// to use this, all commands have to be rewritten for Tcl_CreateObjCommand
int sasfit_tcl_get_arr_int(Tcl_Interp * interp,
                              int ** arr,
			      int * length,
                              const char * name1,
                              const char * name2)
{
	Tcl_Obj * list_obj = 0;
	Tcl_Obj ** objvPtr = 0;
	int i, result = TCL_ERROR;

	if (!arr || !length) goto exit;
	list_obj = sasfit_tcl_get_obj(interp, name1, name2);
	if (!list_obj) goto exit;
	result = Tcl_ListObjGetElements(interp, list_obj, length, &objvPtr);
	if (result != TCL_OK || *length <= 0 || !objvPtr) goto exit;
	// get memory for target array
	result = TCL_ERROR; // reset error status
	if (*arr) {
		free_ivector(*arr, 0, 0);
		*arr = 0;
	}
	*arr = ivector(0, *length - 1); //##
	if (!*arr) goto exit;
	for (i=0; i < *length; i++) {
		if (!objvPtr[i]) goto on_error; // corrupted result
		result = (Tcl_GetIntFromObj)(interp, objvPtr[i], &(*arr)[i]); //##
		if (result != TCL_OK) goto on_error;
	}
	result = TCL_OK;
	goto exit;
on_error:
	free_ivector(*arr, 0, 0); //##
exit:
	return result;
}


#define GET_SINGLE_VAL(type) \
Tcl_Obj * val_obj = 0; \
int result; \
do { \
	if (!val) return TCL_ERROR; \
	val_obj = sasfit_tcl_get_obj(interp, name1, name2); \
	if (!val_obj) return TCL_ERROR; \
   	result = (Tcl_Get ##type ##FromObj)(interp, val_obj, val); \
	if (result != TCL_OK) return TCL_ERROR; \
	return TCL_OK; \
} while(0)

int sasfit_tcl_get_int(Tcl_Interp * interp, int * val, const char * name1, const char * name2)
{
	GET_SINGLE_VAL(Int);
}

int sasfit_tcl_get_double(Tcl_Interp * interp, double * val, const char * name1, const char * name2)
{
	GET_SINGLE_VAL(Double);
}

int sasfit_tcl_get_bool(Tcl_Interp * interp, int * val, const char * name1, const char * name2)
{
	GET_SINGLE_VAL(Boolean);
}

#define SET_SINGLE_VAL(type) \
Tcl_Obj * name1_obj = 0; \
Tcl_Obj * name2_obj = 0; \
Tcl_Obj * val_obj = 0; \
do { \
	if (!name1) return TCL_ERROR; \
	name1_obj = Tcl_NewStringObj(name1, -1); \
	if (!name1_obj) return TCL_ERROR; \
	if (name2) { \
		name2_obj = Tcl_NewStringObj(name2, -1); \
		if (!name2_obj) return TCL_ERROR; \
	} \
	val_obj = (Tcl_New ##type ##Obj)(val); \
	if (!val_obj) return TCL_ERROR; \
	val_obj = Tcl_ObjSetVar2(interp, name1_obj, name2_obj, val_obj, 0); \
	if (!val_obj) return TCL_ERROR; \
	return TCL_OK; \
} while(0)

int sasfit_tcl_set_int(Tcl_Interp * interp, const char * name1, const char * name2, int val)
{
	SET_SINGLE_VAL(Int);
}

int sasfit_tcl_set_double(Tcl_Interp * interp, const char * name1, const char * name2, double val)
{
	SET_SINGLE_VAL(Double);
}

int sasfit_tcl_set_bool(Tcl_Interp * interp, const char * name1, const char * name2, int val)
{
	SET_SINGLE_VAL(Boolean);
}


int sasfit_eps_get_from_tcl(Tcl_Interp * interp, const char * argv[])
{
	int tcl_int, tcl_bool, tcl_result;
	double tcl_double;

	sasfit_eps_set_comp(1e-50); // c/SASFIT_x_tcl.c, l.575: somewhere between 1e-70 and 1e-100

	SF_TCL_GET_F(double, "::FitPrecision", "epsNRIQ", sasfit_eps_set_nriq);
	SF_TCL_GET_F(double, "::FitPrecision", "h", sasfit_eps_set_h);
	SF_TCL_GET_F(double, "::FitPrecision", "epsanisotropic", sasfit_eps_set_aniso);
	SF_TCL_GET_F(double, "::FitPrecision", "epsresolution", sasfit_eps_set_res);
	SF_TCL_GET_F(int,    "::FitPrecision", "JMAXNRIQ", sasfit_eps_set_jmax_nriq);
	SF_TCL_GET_F(int,    "::FitPrecision", "iter_4_MC", sasfit_eps_set_iter_4_mc);
	SF_TCL_GET_F(int,    "::FitPrecision", "JMAXanisotropic", sasfit_eps_set_jmax_aniso);
	SF_TCL_GET_F(int,    "::FitPrecision", "JMAXresolution", sasfit_eps_set_jmax_res);
	SF_TCL_GET_F(int,    "::FitPrecision", "SQ_or_IQ", sasfit_eps_set_sq_or_iq);
	SF_TCL_GET_F(int,    "::FitPrecision", "IQorGz_int", sasfit_set_iq_or_gz);
	SF_TCL_GET_F(int,    "::FitPrecision", "IntStrategy_int", sasfit_set_int_strategy);
	SF_TCL_GET_F(int,    "::FitPrecision", "SphAvgStrategy_int", sasfit_set_sphavg_strategy);
	SF_TCL_GET_F(int,    "::FitPrecision", "Robertus_p", sasfit_eps_set_robertus_p);
	SF_TCL_GET_F(int,    "::FitPrecision", "GSL_GAUSSLEGENDRE", sasfit_eps_set_gausslegendre);
	SF_TCL_GET_F(int,    "::FitPrecision", "GSL_CHEBYSHEV1", sasfit_eps_set_chebyshev1);
	SF_TCL_GET_F(int,    "::FitPrecision", "GSL_CHEBYSHEV2", sasfit_eps_set_chebyshev2);
	SF_TCL_GET_F(int,    "::FitPrecision", "GSL_GEGENBAUER", sasfit_eps_set_gegenbauer);
	SF_TCL_GET_F(int,    "::FitPrecision", "GSL_EXPONENTIAL", sasfit_eps_set_exponential);
	SF_TCL_GET_F(int,    "::FitPrecision", "GSL_LAGUERRE", sasfit_eps_set_laguerre);
	SF_TCL_GET_F(int,    "::FitPrecision", "GSL_JACOBI", sasfit_eps_set_jacobi);
	SF_TCL_GET_F(int,    "::FitPrecision", "Lebedev", sasfit_eps_set_lebedev);
	SF_TCL_GET_F(int,    "::FitPrecision", "spherical_t_design", sasfit_eps_set_spherical_t_design);
	SF_TCL_GET_F(int,    "::FitPrecision", "FIBONACCI", sasfit_eps_set_fibonacci);
	SF_TCL_GET_F(double, "::FitPrecision", "GSL_ALPHA", sasfit_eps_set_alpha);
	SF_TCL_GET_F(double, "::FitPrecision", "GSL_BETA", sasfit_eps_set_beta);
	SF_TCL_GET_F(bool, argv[1], "fit", sasfit_eps_set_fitorsim);
	if (sasfit_eps_get_alpha() <= -1) {
        sasfit_err("alpha=&lg<=-1, will be set to -0.9\n",sasfit_eps_get_alpha());
        sasfit_eps_set_alpha(-0.9);
	}
	if (sasfit_eps_get_beta() <= -1) {
        sasfit_err("beta=&lg<=-1, will be set to -0.9\n",sasfit_eps_get_beta());
        sasfit_eps_set_beta(-0.9);
	}
	return TCL_OK;
}


// has to be rewritten and optimized, but not now ;)
/*#########################################################################*/
/*#                                                                       #*/
/*# get_AP --                                                             #*/
/*#                                                                       #*/
/*#      This function reads the value of the tcl variable AP             #*/
/*#      in the C-structure AnalyticPar                                   #*/
/*#                                                                       #*/
/*# Results: TCL_OK, TCL_ERROR                                            #*/
/*#                                                                       #*/
/*#########################################################################*/
int get_AP(Tcl_Interp        *interp,
           const char        **argv,
           sasfit_analytpar **AnalyticPar,
           int               *mmax_SD,
           scalar            *aalambda,
           int               *error_type,
           scalar           **Q,
           scalar           **IQ,
           scalar           **DIQ,
           scalar           **Qres,
           int               *n_data)
{
	sasfit_analytpar *AP = 0;
	int              i,j, Splitcode, Splitargc, tcode, targc;
	const char       **Splitargv = 0, **targv = 0;
	int              max_SD,ndata;
	scalar           *h = 0, *Ih = 0, *DIh = 0, *res = 0;
	const char       *xyer_data = 0;

	int *param_count_sd = 0, *param_count_sq = 0, *param_count_ff = 0;
	int tcl_result, tcl_int, len;
	double tcl_double;

	if (sasfit_eps_get_from_tcl(interp, argv) == TCL_ERROR) {
		return TCL_ERROR;
	}

	// read the number alambda
	SF_TCL_GET(double, argv[1], "alambda", *aalambda);
	// read the error type
	SF_TCL_GET(int, argv[1], "error", *error_type);

/*
 * read scattering curve h, Ih and DIh
 */
    xyer_data   = argv[2];
    Splitcode = Tcl_SplitList(interp,xyer_data,&Splitargc,&Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_err("scattering data {h Ih DIh res} is not a list\n");
       return TCL_ERROR;
    }
    if ((Splitargc < 3) || (Splitargc > 4)) {
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in {h Ih DIh <res>}\n");
       return TCL_ERROR;
    }
    Splitcode = Tcl_SplitList(interp,Splitargv[0],&targc,&targv);
    if (Splitcode == TCL_ERROR) {
       Tcl_Free((char *) Splitargv);
       Tcl_Free((char *) targv);
       sasfit_err( "could not read h\n");
       return TCL_ERROR;
    }
    ndata = targc;
    (*n_data) = ndata;
    h   = dvector(0,ndata-1);
    res = dvector(0,ndata-1);
    Ih  = dvector(0,ndata-1);
    DIh = dvector(0,ndata-1);
    for (j=0; j < targc ;j++) {
       tcode = Tcl_GetDoubleD(interp,targv[j],&h[j]);
       if (tcode == TCL_ERROR) {
          free_dvector(h,0,ndata-1);
          free_dvector(res,0,ndata-1);
          free_dvector(Ih,0,ndata-1);
          free_dvector(DIh,0,ndata-1);
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("could not convert h[j]) to double\n");
          return TCL_ERROR;
       }
    }
    Splitcode = Tcl_SplitList(interp,Splitargv[1],&targc,&targv);
    if (Splitcode == TCL_ERROR) {
       free_dvector(h,0,ndata-1);
       free_dvector(res,0,ndata-1);
       free_dvector(Ih,0,ndata-1);
       free_dvector(DIh,0,ndata-1);
       Tcl_Free((char *) Splitargv);
       Tcl_Free((char *) targv);
       sasfit_err( "could not read Ih\n");
       return TCL_ERROR;
    }
    if (targc != ndata) {
       free_dvector(h,0,ndata-1);
       free_dvector(res,0,ndata-1);
       free_dvector(Ih,0,ndata-1);
       free_dvector(DIh,0,ndata-1);
       Tcl_Free((char *) Splitargv);
       Tcl_Free((char *) targv);
       sasfit_err("could not convert Ih[j]) to double\n");
       return TCL_ERROR;
    }
    for (j=0; j < targc ;j++) {
       tcode = Tcl_GetDoubleD(interp,targv[j],&Ih[j]);
       if (tcode == TCL_ERROR) {
          free_dvector(h,0,ndata-1);
          free_dvector(res,0,ndata-1);
          free_dvector(Ih,0,ndata-1);
          free_dvector(DIh,0,ndata-1);
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("could not convert Ih[j]) to double\n");
          return TCL_ERROR;
       }
    }
    Splitcode = Tcl_SplitList(interp,Splitargv[2],&targc,&targv);
    if (Splitcode == TCL_ERROR) {
       free_dvector(h,0,ndata-1);
       free_dvector(res,0,ndata-1);
       free_dvector(Ih,0,ndata-1);
       free_dvector(DIh,0,ndata-1);
       Tcl_Free((char *) Splitargv);
       Tcl_Free((char *) targv);
       sasfit_err("could not read DIh\n");
       return TCL_ERROR;
    }
    if (targc != ndata) {
       free_dvector(h,0,ndata-1);
       free_dvector(res,0,ndata-1);
       free_dvector(Ih,0,ndata-1);
       free_dvector(DIh,0,ndata-1);
       Tcl_Free((char *) Splitargv);
       Tcl_Free((char *) targv);
       sasfit_err("could not convert DIh[j]) to double\n");
       return TCL_ERROR;
    }
    for (j=0; j < targc ;j++) {
       tcode = Tcl_GetDoubleD(interp,targv[j],&DIh[j]);
       if (tcode == TCL_ERROR) {
          free_dvector(h,0,ndata-1);
          free_dvector(res,0,ndata-1);
          free_dvector(Ih,0,ndata-1);
          free_dvector(DIh,0,ndata-1);
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("could not convert DIh[j]) to double\n");
          return TCL_ERROR;
       }
    }

	if (Splitargc == 4) {
	   Splitcode = Tcl_SplitList(interp,Splitargv[3],&targc,&targv);
       if (Splitcode == TCL_ERROR) {
          free_dvector(h,0,ndata-1);
          free_dvector(res,0,ndata-1);
          free_dvector(Ih,0,ndata-1);
          free_dvector(DIh,0,ndata-1);
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("could not read res\n");
          return TCL_ERROR;
	   }
       if (targc != ndata) {
          free_dvector(h,0,ndata-1);
          free_dvector(res,0,ndata-1);
          free_dvector(Ih,0,ndata-1);
          free_dvector(DIh,0,ndata-1);
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("could not convert res[j]) to double\n");
          return TCL_ERROR;
	   }
       for (j=0; j < targc ;j++) {
          tcode = Tcl_GetDoubleD(interp,targv[j],&res[j]);
          if (tcode == TCL_ERROR) {
             free_dvector(h,0,ndata-1);
             free_dvector(res,0,ndata-1);
             free_dvector(Ih,0,ndata-1);
             free_dvector(DIh,0,ndata-1);
             Tcl_Free((char *) Splitargv);
             Tcl_Free((char *) targv);
             sasfit_err("could not convert res[j]) to double\n");
             return TCL_ERROR;
		  }
	   }
    } else {
       for (i=0;i<ndata;i++) res[i] = 0.0;
	}
    switch (*error_type) {
       case 0 : {break;}
       case 1 : { for (i=0;i<ndata;i++) DIh[i] = log(fabs(Ih[i])); break; }
       case 2 : { for (i=0;i<ndata;i++) DIh[i] = sqrt(fabs(Ih[i])); break; }
       case 3 : {for (i=0;i<ndata;i++) DIh[i] = 1.0; break;}
      default : { sasfit_err("#get_AP: unknown error_type %d\n",*error_type);
                  for (i=0;i<ndata;i++) DIh[i] = 1.0;
		break;
                }
    }
    (*Q)    = h;
    (*IQ)   = Ih;
    (*DIQ)  = DIh;
    (*Qres) = res;
/*
 * read the number of size distributions max_SD
 */
    if (TCL_ERROR == Tcl_GetInt(interp,
                             Tcl_GetVar2(interp,argv[1],"max_SD",0),
                             &max_SD) ) {
       sasfit_err("could not read max_SD\n");
       return TCL_ERROR;
    }
    (*mmax_SD) = max_SD;
    // AP = (sasfit_analytpar*) Tcl_Alloc(sizeof(sasfit_analytpar) * max_SD );
    sasfit_init_AP(&AP, max_SD);
/*
 * read the number of parameters for each contribution to get only values
 * which are actually in use
 */
	tcl_result = sasfit_tcl_get_arr_int(interp, &param_count_ff,
	                    &len, argv[1],"FF,param_count");
	if (tcl_result != TCL_OK) {
		sasfit_free_AP(&AP);
		return TCL_ERROR;
	}
/*
 * read parameter l[MAXPAR] of the "max_SD" form factors
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"FF,l",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &AP );
       sasfit_err("Problem with parameters of the form factor\nCorrespond this error message to me\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &AP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong number of list elements in the form factor parameter list\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &AP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
	  sasfit_err("problem to interprete parameters of form factor for scattering contribution %d \n",i+1);
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetDoubleD(interp,targv[j],&AP[i].FF_l[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &AP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
	      sasfit_err("problem to interprete parameter no. %d of the form factor in scattering contribution no. %d\n", j+1, i+1);
              return TCL_ERROR;
           }
       }
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter err of the "max_SD" form factors
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"FF,err",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &AP );
       sasfit_err("AP(FF,err) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &AP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in AP(FF,err)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &AP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in AP[i](FF,err)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetDoubleD(interp,targv[j],&AP[i].FF_err[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &AP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert AP[i](FF,err[j]) to double\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter min of the "max_SD" form factors
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"FF,min",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &AP );
       sasfit_err("AP(FF,min) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &AP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in AP(FF,min)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &AP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in AP[i](FF,min)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetDoubleD(interp,targv[j],&AP[i].FF_min[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &AP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert AP[i](FF,min[j]) to double\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter max of the "max_SD" form factors
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"FF,max",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &AP );
       sasfit_err("AP(FF,max) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &AP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in AP(FF,max)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &AP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in AP[i](FF,max)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetDoubleD(interp,targv[j],&AP[i].FF_max[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &AP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert AP[i](FF,max[j]) to double\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter limits of the "max_SD" form factors
 */
  Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"FF,limits",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &AP );
       sasfit_err("AP(FF,limits) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &AP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in AP(FF,limits)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++)
    {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &AP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in AP[i](FF,limits)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++)
       {
           tcode = Tcl_GetBoolean(interp,targv[j],&AP[i].FF_limits[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &AP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert AP[%d](FF,limits[%d]) to integer\n",i,j);
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter active of the "max_SD" form factors
 */
  Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"FF,active",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &AP );
       sasfit_err("AP(FF,active) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &AP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in AP(FF,active)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &AP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in AP[i](FF,active)\n");
          return TCL_ERROR;
       }
       for (j=0; j < MAXPAR ;j++) AP[i].FF_active[j] = 0;
       for (j=0; j < param_count_ff[i] ;j++) {
           tcode = Tcl_GetBoolean(interp,targv[j],&AP[i].FF_active[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &AP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert AP[i](FF,active[j]) to integer\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter distr of the "max_SD" form factors
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"FF,distr",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &AP );
       sasfit_err("AP(FF,distr) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_err("wrong # of list elements in AP[i](FF,distr)\n");
       sasfit_free_AP( &AP );
       Tcl_Free((char *) Splitargv);
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_err("wrong # of list elements in AP[i](FF,distr[j])\n");
          sasfit_free_AP( &AP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetBoolean(interp,targv[j],&AP[i].FF_distr[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &AP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert AP[i](FF,distr[j]) to integer\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter typestr of the "max_SD" form factors
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"FF,typestr",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &AP );
       sasfit_err("could not convert list AP[i](FF,typestr)\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_err("wrong # of list elements in AP[i](FF,typestr)\n");
       sasfit_free_AP( &AP );
       Tcl_Free((char *) Splitargv);
       return TCL_ERROR;
    }
    if ( initTypeStr(AP, max_SD, Splitargv, FFfct) != TCL_OK ) return TCL_ERROR;
    Tcl_Free((char *) Splitargv);

/*
 * read the number of parameters for each contribution to get only values
 * which are actually in use
 */
	tcl_result = sasfit_tcl_get_arr_int(interp, &param_count_sd,
	                    &len, argv[1],"SD,param_count");
	if (tcl_result != TCL_OK) {
		sasfit_free_AP(&AP);
		return TCL_ERROR;
	}
/*
 * read parameter a of the "max_SD" size distribution
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SD,a",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &AP );
       sasfit_err("could not read list AP[i](SD,a)\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &AP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in AP(SD,a)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &AP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in AP[i](SD,a)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetDoubleD(interp,targv[j],&AP[i].SD_a[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &AP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
  	      sasfit_err("problem to interprete parameter no. %d of the size distribution in scattering contribution no. %d\n",j+1,i+1);
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter err of the "max_SD" size distribution
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SD,err",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &AP );
       sasfit_err("could not read list AP[i](SD,err)\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &AP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in AP(SD,err)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &AP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in AP[i](SD,err)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetDoubleD(interp,targv[j],&AP[i].SD_err[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &AP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert AP[i](SD,err[j]) to double\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter min of the "max_SD" size distribution
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SD,min",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &AP );
       sasfit_err("could not read list AP(SD,min)\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &AP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in AP(SD,min)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &AP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in AP[i](SD,min)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetDoubleD(interp,targv[j],&AP[i].SD_min[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &AP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert AP[i](SD,min[j]) to double\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter max of the "max_SD" size distribution
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SD,max",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &AP );
       sasfit_err("could not read list AP(SD,max)\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &AP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in AP(SD,max)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &AP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in AP[i](SD,max)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetDoubleD(interp,targv[j],&AP[i].SD_max[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &AP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert AP[i](SD,max[j]) to double\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter limits of the "max_SD" size distribution
 */
  Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SD,limits",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &AP );
       sasfit_err("AP(SD,limits) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &AP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in AP[i](SD,limits)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &AP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in AP[i](SD,limits)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetBoolean(interp,targv[j],&AP[i].SD_limits[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &AP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert AP[i](SD,limits[j]) to integer\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter active of the "max_SD" size distribution
 */
  Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SD,active",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &AP );
       sasfit_err("AP(SD,active) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &AP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in AP[i](SD,active)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &AP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in AP[i](SD,active)\n");
          return TCL_ERROR;
       }
       for (j=0; j < MAXPAR ;j++) AP[i].SD_active[j] = 0;
       for (j=0; j < param_count_sd[i] ;j++) {
           tcode = Tcl_GetBoolean(interp,targv[j],&AP[i].SD_active[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &AP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert AP[i](SD,active[j]) to integer\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter typestr of the "max_SD" size distribution
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SD,typestr",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &AP );
       sasfit_err("AP(SD,typestr) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &AP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in AP[i](SD,typestr)\n");
       return TCL_ERROR;
    }
    if ( initTypeStr(AP, max_SD, Splitargv, SDfct) != TCL_OK ) return TCL_ERROR;
    Tcl_Free((char *) Splitargv);

/*
 * read the number of parameters for each contribution to get only values
 * which are actually in use
 */
	tcl_result = sasfit_tcl_get_arr_int(interp, &param_count_sq,
	                    &len, argv[1],"SQ,param_count");
	if (tcl_result != TCL_OK) {
		sasfit_free_AP(&AP);
		return TCL_ERROR;
	}
/*
 * read parameter s of the "max_SD" size distribution
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SQ,s",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &AP );
       sasfit_err("could not read list AP[i](SQ,s)\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &AP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in AP(SQ,s)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &AP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in AP[i](SQ,s)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetDoubleD(interp,targv[j],&AP[i].SQ_s[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &AP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
    		sasfit_err("problem to interprete parameter no. %d of structure factor for scattering contribution %d \n", j+1, i+1);
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter err of the "max_SD" size distribution
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SQ,err",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &AP );
       sasfit_err("could not read list AP[i](SQ,err)\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &AP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in AP(SQ,err)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &AP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in AP[i](SQ,err)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetDoubleD(interp,targv[j],&AP[i].SQ_err[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &AP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert AP[i](SQ,err[j]) to double\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter min of the "max_SD" size distribution
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SQ,min",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &AP );
       sasfit_err("could not read list AP(SQ,min)\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &AP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in AP(SQ,min)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &AP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in AP[i](SQ,min)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetDoubleD(interp,targv[j],&AP[i].SQ_min[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &AP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert AP[i](SQ,min[j]) to double\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter max of the "max_SD" size distribution
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SQ,max",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &AP );
       sasfit_err("could not read list AP(SQ,max)\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &AP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in AP(SQ,max)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &AP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in AP[i](SQ,max)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetDoubleD(interp,targv[j],&AP[i].SQ_max[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &AP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert AP[i](SQ,max[j]) to double\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter limits of the "max_SD" size distribution
 */
  Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SQ,limits",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &AP );
       sasfit_err("AP(SQ,limits) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &AP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in AP[i](SQ,limits)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &AP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in AP[i](SQ,limits)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetBoolean(interp,targv[j],&AP[i].SQ_limits[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &AP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert AP[i](SQ,limits[j]) to integer\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter active of the "max_SD" size distribution
 */
  Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SQ,active",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &AP );
       sasfit_err("AP(SQ,active) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &AP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in AP[i](SQ,active)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &AP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in AP[i](SQ,active)\n");
          return TCL_ERROR;
       }
       for (j=0; j < MAXPAR ;j++) AP[i].SQ_active[j] = 0;
       for (j=0; j < param_count_sq[i] ;j++) {
           tcode = Tcl_GetBoolean(interp,targv[j],&AP[i].SQ_active[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &AP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert AP[i](SQ,active[j]) to integer\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter typestr of the "max_SD" size distribution
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SQ,typestr",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &AP );
       sasfit_err("AP(SQ,typestr) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &AP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in AP[i](SQ,typestr)\n");
       return TCL_ERROR;
    }
    if ( initTypeStr(AP, max_SD, Splitargv, SQfct) != TCL_OK ) return TCL_ERROR;
    Tcl_Free((char *) Splitargv);

/*
 * read parameter how of the "max_SD" structure factor
 */

  Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SQ,how",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &AP );
       sasfit_err("AP(SQ,how) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &AP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in AP[i](SQ_how)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
        tcode = Tcl_GetInt(interp,Splitargv[i],&AP[i].SQ_how);
        if (tcode == TCL_ERROR) {
           sasfit_free_AP( &AP );
           Tcl_Free((char *) Splitargv);
           sasfit_err("could not convert AP[i](SQ_how) to integer\n");
           return TCL_ERROR;
        }
    }
    Tcl_Free((char *) Splitargv);




/*
 * read parameter Par(fit)
 */


/*
 * read parameter substrSDFF of the "max_SD" size distribution
 */

  Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"substrSDFF",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &AP );
       sasfit_err("AP(substrSDFF) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &AP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in AP[i](substrSDFF)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
        tcode = Tcl_GetBoolean(interp,Splitargv[i],&AP[i].substrSDFF);
        if (tcode == TCL_ERROR) {
           sasfit_free_AP( &AP );
           Tcl_Free((char *) Splitargv);
           sasfit_err("could not convert AP[i](substrSDFF) to boolean\n");
           return TCL_ERROR;
        }
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter calcSDFF of the "max_SD" size distribution
 */

  Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"calcSDFF",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &AP );
       sasfit_err("AP(calcSDFF) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &AP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in AP[i](calcSDFF)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
        tcode = Tcl_GetBoolean(interp,Splitargv[i],&AP[i].calcSDFF);
        if (tcode == TCL_ERROR) {
           sasfit_free_AP( &AP );
           Tcl_Free((char *) Splitargv);
           sasfit_err("could not convert AP[i](calcSDFF) to boolean\n");
           return TCL_ERROR;
        }
    }
    Tcl_Free((char *) Splitargv);


/*
 * read parameter fitSDFF of the "max_SD" size distribution
 */
  Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"fitSDFF",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &AP );
       sasfit_err("AP(fitSDFF) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &AP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in AP[i](fitSDFF)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
        tcode = Tcl_GetBoolean(interp,Splitargv[i],&AP[i].fitSDFF);
        if (tcode == TCL_ERROR) {
           sasfit_free_AP( &AP );
           Tcl_Free((char *) Splitargv);
           sasfit_err("could not convert AP[i](fitSDFF) to boolean\n");
           return TCL_ERROR;
        }
    }
    Tcl_Free((char *) Splitargv);

    (*AnalyticPar) = AP;
    (*mmax_SD) = max_SD;

    if (param_count_ff) free_ivector(param_count_ff,0,0);
    if (param_count_sq) free_ivector(param_count_sq,0,0);
    if (param_count_sd) free_ivector(param_count_sd,0,0);

    return TCL_OK;
}

