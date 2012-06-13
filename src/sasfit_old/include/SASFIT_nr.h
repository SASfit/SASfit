/*
 * src/sasfit_old/include/SASFIT_nr.h
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

/*
 * original nr.h changed
 * changes #include <bool.h>
 *          definition of mrqcof()
 *          definition of mrqmin()
 *          definition of new function SASFIT_lista()
 * Joachim Kohlbrecher
 * 04.07.2004
 */

#ifndef __SASFIT_NR
#define __SASFIT_NR

#include <stdio.h>
#include <sasfit_core.h>

#define float double
typedef struct FCOMPLEX {float r,i;} fcomplex;
typedef struct IMMENSE {unsigned long l,r;} immense;
typedef struct GREAT {unsigned short l,c,r;} great;

struct sasfit_gsl_multifit_CData {
   float **covar;
   float **alpha;
   int   ma;
   int   mfit;
   int   common_i;
   int   max_SD;
   float chisq;
   float *a;
   float *da;
   float *atry;
   float *P_common;
   int   *lista;
   float *beta;
   float **oneda;
};


	void float_to_string(char *, float);
	float **matrix(int, int, int, int);
	float **convert_matrix(float *, int, int, int, int);
	double **dmatrix(int, int, int, int);
	int **imatrix(int, int, int, int);
	float **submatrix(float **, int, int, int, int, int, int);
	void free_matrix(float **, int, int, int, int);
	void free_dmatrix(double **, int, int, int, int);
	void free_imatrix(int **, int, int, int, int);
	void free_submatrix(float **, int, int, int, int);
	void free_convert_matrix(float **, int, int, int, int);

	void fit(Tcl_Interp *, float *, float *, int, float *, int, float *, 
	         float *, float *, float *, float *, float *, bool *);
	float gammq(Tcl_Interp *, float, float, bool *);
	void gcf(Tcl_Interp *, float *, float, float, float *, bool *);
	void gser(Tcl_Interp *, float *, float, float, float *, bool *);
	float bessj1(float);
	float bessj0(float);
	float bessi0(float);
	float sasfit_erf(Tcl_Interp *interp, float x, bool *error);
	float gammp(Tcl_Interp *,float, float,bool *);

	void SASFITindexx(int, float *, int *);
	void SASFIT2indexx(int, float *, int *);

	float SASFITtrapzdNR_V_dR(Tcl_Interp *,float *,sasfit_function* ,sasfit_function* ,float,float,int, bool *);
	float SASFITqrombNR_V_dR(Tcl_Interp *,float *,sasfit_function* ,sasfit_function* ,float,float,bool *);

	float SASFITtrapzdIQdR(Tcl_Interp *, int *,float *,float *,float,float *,sasfit_function* ,sasfit_function* ,sasfit_function* ,int,float,float,int,bool *);
	float SASFITqrombIQdR(Tcl_Interp *, int *,float *,float *,float,float *,sasfit_function* ,sasfit_function* ,sasfit_function* ,int,float,float,bool *);

	float SASFITtrapzdIQSQdR(Tcl_Interp *, int *,float *,float *,float,float *,sasfit_function* ,sasfit_function* ,sasfit_function* ,int,float,float,int,bool *);
	float SASFITqrombIQSQdR(Tcl_Interp *, int *,float *,float *,float,float *,sasfit_function* ,sasfit_function* ,sasfit_function* ,int,float,float,bool *);

	float SASFITtrapzdIQSQijdRj(Tcl_Interp *, int *, float *, float *,float,float *,sasfit_function* ,sasfit_function* ,sasfit_function* ,int,float,float,int,bool *);
	float SASFITqrombIQSQijdRj(Tcl_Interp *, int *,float *,float *,float,float *,sasfit_function* ,sasfit_function* ,sasfit_function* ,int,float,float,bool *);

	float SASFITtrapzdIQSQijdRi(Tcl_Interp *, int *,float[], float, float[],float,float[],sasfit_function* ,sasfit_function* ,sasfit_function* ,int,float,float,int,bool *);
	float SASFITqrombIQSQijdRi(Tcl_Interp *, int *,float *, float, float *,float,float *,sasfit_function* ,sasfit_function* ,sasfit_function* ,int,float,float,bool *);

	float SASFITtrapzdSA_IQSQijdRj(Tcl_Interp *, int *,float *, float, float *,float,float *,sasfit_function* ,sasfit_function* ,sasfit_function* ,int,float,float,int,bool *);
	float SASFITqrombSA_IQSQijdRj(Tcl_Interp *, int *,float *, float, float *,float,float *,sasfit_function* ,sasfit_function* ,sasfit_function* ,int,float,float,bool *);

	float SASFITtrapzdSA_IQSQijdRi(Tcl_Interp *, int *,float *, float, float, float *,float,float *,sasfit_function* ,sasfit_function* ,sasfit_function* ,int,float,float,int,bool *);
	float SASFITqrombSA_IQSQijdRi(Tcl_Interp *, int *,float *, float, float, float *,float,float *,sasfit_function* ,sasfit_function* ,sasfit_function* ,int,float,float,bool *);

	float SASFITtrapzdIQ(Tcl_Interp	*, float, float, float, float, float *, float *, float *, float *, int, sasfit_analytpar *, int, int, bool *);
	float SASFITtrapzdIQglobal(Tcl_Interp	*, float, float, float, float, float *, float *, float *, float *, int, sasfit_analytpar *, sasfit_commonpar *, int, int, bool *);
	float SASFITtrapzdIQ_gsl_global(Tcl_Interp	*, float, float, float, float, float *, float *, int, sasfit_analytpar *, sasfit_commonpar *, int, int, bool *);
    
	void SASFITpolint(Tcl_Interp *, float *, float *, int, float, float *, float *, bool *);

	float SASFITqrombIQ(Tcl_Interp	*, float, float, float, float, float *, float *, float *, float *, int, sasfit_analytpar *, int, bool *);
	float SASFITqrombIQglobal(Tcl_Interp	*, float, float, float, float, float *, float *, float *, float *, int, sasfit_analytpar *, sasfit_commonpar *, int, bool *);
	float SASFITqrombIQ_gsl_global(Tcl_Interp	*, float, float, float, float, float *, float *, int, sasfit_analytpar *, sasfit_commonpar *, int, bool *);

	void SASFITcovsrt(Tcl_Interp *, float **, int, int *, int, bool *);
	void SASFITgaussj(Tcl_Interp *, float **, int, float **, int, bool *);

	void SASFITmrqcof(Tcl_Interp *, float *, float *, float *, float *, float *, float *, int, 
		float *, int *, int, int, sasfit_analytpar *, int, int, 
		float **, float *, float *, 
		void (*)(), bool *);

	void SASFITmrqcofGlobal(Tcl_Interp *, float **, float **, float **, float **, float **, float **, int *, 
		float *, int *, int, int, sasfit_analytpar *, sasfit_commonpar *, int, int, 
		float **, float *, float *, 
		void (*)(), bool *);

	void SASFIT2mrqcof(Tcl_Interp *, float *, float *, float *, int, 
		float *, int, int *, int, float **, float *, float *, void (*)(), bool *);
	void SASFIT2mrqmin(Tcl_Interp *, float *, float *, float *, int, 
		float *, int, int *, int, float **, float **, float *, 
		void (*)(), float *, bool *);
	void SASFITmrqminGlobal(Tcl_Interp *, float **, float **, float **, float **, float **, float **,
		int *, int, sasfit_analytpar *, sasfit_commonpar *, int, struct sasfit_CData *, 
		float *, void (*)(), float *, bool *);
	void SASFITmrqmin(Tcl_Interp *, float *, float *, float *, float *, float *, float *,
		int, int, sasfit_analytpar *, int, struct sasfit_CData *, 
		float *, void (*)(), float *, bool *);
	bool FFtypestr_equal(Tcl_Interp *, char *, char *);

#endif
