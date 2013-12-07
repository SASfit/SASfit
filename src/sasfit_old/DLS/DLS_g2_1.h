/*
 * src/sasfit_old/DLS/DLS_g2_1.h
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
 */

#ifndef _DLS_
#define _DLS_ 1

#include <tcl.h>
#include "../include/sasfit.h"
#include "../include/SASFIT_nr.h"

/* enum bool { FALSE = 0, TRUE = 1 }; */

/* #define PI M_PI */
#define MAXDLSPAR 7

struct DLSPar {
	double Par[7];
    double Err[7];
	double iter;
	double iter_d;
	int    ndata;
	char   DLSmodel[132];
	char   error_type[132];
	int    active[7];
	double chisqr;
	double *t;
    double *y;
	double *yth;
	double *sigma;
	double *w;
	size_t n;
};

int get_DLSpar(ClientData,Tcl_Interp *,char **,struct DLSPar *,double **,double **,double **);
int save_DLSpar(ClientData,Tcl_Interp *,char **,struct DLSPar *);

void  DLS(Tcl_Interp *,double,struct DLSPar,double *,double *,bool *);

int DLS_fdf (const gsl_vector *, void *,gsl_vector *, gsl_matrix *);
int DLS_df (const gsl_vector *, void *, gsl_matrix *);
int DLS_f (const gsl_vector *, void *,gsl_vector *);
#endif

