/*
 * src/sasfit_core/include/sasfit_analytpar.h
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

#ifndef SASFIT_ANALYTPAR_H
#define SASFIT_ANALYTPAR_H

#include <gsl/gsl_vector.h>
#include "sasfit_core_shared_exports.h"

// for debugging
//#define Tcl_GetDoubleD(a,b,c) Tcl_GetDoubleE(a,b,c,"Tcl_GetDouble %d: %x %x %x\n",__LINE__)
#define Tcl_GetDoubleD(a,b,c) Tcl_GetDouble((a),(b),(c))

typedef struct
{
	Tcl_Interp *interp;
    int   *dF_dpar;
    scalar *l;
    scalar *s;
    scalar *a;
    scalar Q;
    scalar z;
    sasfit_function*  SD;
    sasfit_function*  FF;
    sasfit_function*  SQ;
    int   distr;
    int   SQ_how;
    scalar Rstart;
    scalar Rend;
    int   nintervals;
    bool  error;
    bool withSQ;
    scalar (* function) (scalar x, void * params);
} param4int;
typedef param4int sasfit_param4int;

typedef struct
{
	int		fit;
	int		calcSDFF;
	int		fitSDFF;
	int		substrSDFF;
	int		dodydpar;
	scalar		FF_l[MAXPAR];
	scalar		FF_err[MAXPAR];
	scalar		FF_min[MAXPAR];
	scalar		FF_max[MAXPAR];
	int		FF_limits[MAXPAR];
	int		FF_active[MAXPAR];
	int		FF_distr[MAXPAR];
	sasfit_function FF;
	scalar		FF_factor[MAXPAR];
	char		FF_common[MAXPAR][32];
	int		FF_index[MAXPAR];
	char		Nth_datalabel[132];
	int		Nth_dataset;
	scalar		SD_a[MAXPAR];
	scalar		SD_err[MAXPAR];
	scalar		SD_min[MAXPAR];
	scalar		SD_max[MAXPAR];
	int		SD_limits[MAXPAR];
	int		SD_active[MAXPAR];
	sasfit_function SD;
	scalar		SD_factor[MAXPAR];
	char		SD_common[MAXPAR][32];
	int		SD_index[MAXPAR];
	scalar		R_moments[MAXPAR];
	scalar		fp;
	scalar		SQ_s[MAXPAR];
	scalar		SQ_err[MAXPAR];
	scalar		SQ_min[MAXPAR];
	scalar		SQ_max[MAXPAR];
	int		SQ_limits[MAXPAR];
	int		SQ_active[MAXPAR];
	int		SQ_distr[MAXPAR];
	sasfit_function SQ;
	scalar		SQ_factor[MAXPAR];
	char		SQ_common[MAXPAR][32];
	int		SQ_index[MAXPAR];
	int		SQ_how;
} sasfit_analytpar;

typedef struct
{
	int	nmultset;
	scalar	* P_common;
	scalar	* P_common_err;
	int	* P_common_index;
	int	common_i;
	char	* common_names[MAXPAR];
} sasfit_commonpar;

typedef struct
{
	sasfit_analytpar	* GlobalAP;
	sasfit_commonpar	GCP;
	Tcl_Interp		* interp;
	int			ma;
	int			mfit;
	int			* ndata;
	int			max_SD;
	int			error_type;
	scalar			** h;
	scalar			** Ih;
	scalar			** DIh;
	scalar			** Ith;
	scalar			** res;
	scalar			* a;
	int			* lista;
	bool			error;
} Gparam;
typedef Gparam sasfit_gparam;

struct sasfit_CData {
	scalar ** covar;
	scalar ** alpha;
	int       ma;
	int       mfit;
	int       common_i;
	int       max_SD;
	scalar    chisq;
	scalar  * a;
	scalar  * da;
	scalar  * atry;
	scalar  * yfit;
	scalar  * P_common;
	int     * lista;
	scalar  * beta;
	scalar ** oneda;
};
typedef struct sasfit_CData sasfit_cdata;


struct GzIntStruct {
        Tcl_Interp *interp;
	    scalar z;
        scalar *par;
        scalar *Ifit;
		scalar *Isub;
        scalar *dydpar;
        scalar Qres;
        int   max_SD;
        sasfit_analytpar *AP;
        sasfit_analytpar *GAP;
        sasfit_commonpar *GCP;
        int   error_type;
        bool  *error;
};
typedef struct GzIntStruct sasfit_GzIntStruct;


/**
 * Allocates and initializes an array of sasfit_analytpar structures.
 * @param AParr Pointer to an array of sasfit_analytpar, will be freed if not 0.
 * @param num Size of the resulting array.
 * @sa sasfit_free_AP, sasfit_analytpar
 */
sasfit_core_DLLEXP void sasfit_init_AP(sasfit_analytpar ** AParr, int num);

/**
 * Frees an array of sasfit_analytpar structures.
 * @param AParr Pointer to an array of sasfit_analytpar (one or more, aka. array).
 * @sa sasfit_init_AP, sasfit_analytpar
 */
sasfit_core_DLLEXP void sasfit_free_AP(sasfit_analytpar ** AParr);

sasfit_core_DLLEXP void sasfit_ap2paramlist(int ** idx_list, int * max_param_idx, int * fit_idx, scalar ** param_list, sasfit_analytpar * ap, sasfit_commonpar * gcp, int contrib_num);
sasfit_core_DLLEXP void sasfit_paramlist2ap(scalar * param_list, sasfit_analytpar * ap, sasfit_commonpar * gcp, int contrib_num);
sasfit_core_DLLEXP void sasfit_covar2ap(scalar ** covar, sasfit_analytpar * ap, sasfit_commonpar * gcp, int contrib_num);

sasfit_core_DLLEXP void sasfit_covsrt2ap(scalar ** covar, sasfit_analytpar * ap, sasfit_commonpar * gcp, int contrib_num);
sasfit_core_DLLEXP void SASFIT_gsl_vector_x2GlobalAP_GCP(gsl_vector * x, void * params);

int initTypeStr(sasfit_analytpar     *  ap,
                int                     num_ap,
                const char           ** name,
                sasfit_function_type    fctType);

int get_AP(Tcl_Interp       *interp,
	const char         **argv,
	sasfit_analytpar **AnalyticPar,
	int              *mmax_SD,
	scalar           *aalambda,
	int              *error_type,
	scalar           **Q,
	scalar           **IQ,
	scalar           **DIQ,
	scalar           **Qres,
	int              *n_data);

/**
 * \ingroup sasfit_core_int
 * Gets some TCL variables from the interpreter regarding precision of
 * (non-GSL) integration and numerical limits.
 * \param interp Current interpreter.
 * \param argv TCL-supplied list of variable names.
 */
int sasfit_eps_get_from_tcl(Tcl_Interp * interp, const char * argv[]);

Tcl_Obj * sasfit_tcl_get_obj(Tcl_Interp * interp,
                             const char * name1,
                             const char * name2);

/**
 * Gets a single value of type \e int from Tcl environment.
 * \param interp Current Tcl interpreter.
 * \param val Address to store the value.
 * \param name1 Name of the variable in Tcl code.
 * \param name2 Name of the entry if \e name1 is an array
 *              (optional, NULL otherwise).
 */
int sasfit_tcl_get_int(Tcl_Interp * interp, int * val,
                       const char * name1, const char * name2);

/**
 * Gets a single value of type \e double from Tcl environment.
 * \sa sasfit_tcl_get_int
 */
int sasfit_tcl_get_double(Tcl_Interp * interp, double * val,
                          const char * name1, const char * name2);

/**
 * Gets a single value of type \e boolean from Tcl environment.
 * \sa sasfit_tcl_get_int
 */
int sasfit_tcl_get_bool(Tcl_Interp * interp, int * val,
                        const char * name1, const char * name2);

/**
 * Sets a Tcl variable to a single value of type \e int.
 * \param interp Current Tcl interpreter.
 * \param name1 Name of the variable in Tcl code.
 * \param name2 Name of the entry if \e name1 is an array
 *              (optional, NULL otherwise).
 * \param val Value to store in the Tcl variable.
 */
int sasfit_tcl_set_int(Tcl_Interp * interp,
                       const char * name1, const char * name2,
                       int val);

/**
 * Sets a Tcl variable to a single value of type \e double.
 * \sa sasfit_tcl_set_int
 */
int sasfit_tcl_set_double(Tcl_Interp * interp,
                          const char * name1, const char * name2,
                          double val);

/**
 * Sets a Tcl variable to a single value of type \e boolean.
 * \sa sasfit_tcl_set_int
 */
int sasfit_tcl_set_bool(Tcl_Interp * interp,
                        const char * name1, const char * name2,
                        int val);

int sasfit_tcl_get_arr_int(Tcl_Interp * interp,
                              int ** arr,
                              int * length,
                              const char * name1,
                              const char * name2);
#endif

