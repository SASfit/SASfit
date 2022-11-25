/*
 * src/sasfit_old/SASFIT_nrutil.c
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
 * original file nrutil.c

 * changes: added the function SASFIT_lista()
 * Joachim Kohlbrecher
 * 08.05.98
 */


#ifdef MACOSX
#include <sys/malloc.h>
#else
#include <malloc.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <tcl.h>
#include "include/sasfit.h"

#define float double

void float_to_string(sBuffer,value)
float value;
char *sBuffer;
{
	char *stopBuffer;
	sprintf(sBuffer,"%lg",value);
	value = strtod(sBuffer,&stopBuffer);
	sprintf(sBuffer,"%lg",value);
	return;
}

void int_to_string(sBuffer,value)
int value;
char *sBuffer;
{
	char *stopBuffer;
	sprintf(sBuffer,"%ld",value);
	value = strtol(sBuffer,&stopBuffer,10);
	sprintf(sBuffer,"%ld",value);
	return;
}


void nrerror(error_text)
char error_text[];
{
	void exit();

	fprintf(stderr,"Numerical Recipes run-time error...\n");
	fprintf(stderr,"%s\n",error_text);
	fprintf(stderr,"...now exiting to system...\n");
	exit(1);
}


/*
float *vector(nl,nh)
int nl,nh;
{
	float *v;
        int k;
nl = 0;
	v=(float *) Tcl_Alloc((unsigned) (nh-nl+1)*sizeof(float));
	if (!v) nrerror("allocation failure in vector()");
        for (k=nl;k<=nh;k++) v[k] = 0.0;
	return v-nl;
}

int *ivector(nl,nh)
int nl,nh;
{
	int *v;
nl = 0;
	v=(int *) Tcl_Alloc((unsigned) (nh-nl+1)*sizeof(int));
	if (!v) nrerror("allocation failure in ivector()");
	return (v-nl);
}

double *dvector(nl,nh)
int nl,nh;
{
	double *v;
nl = 0;
	v=(double *) Tcl_Alloc((unsigned) (nh-nl+1)*sizeof(double));
	if (!v) nrerror("allocation failure in dvector()");
	return v-nl;
}
*/


float **matrix(nrl,nrh,ncl,nch)
int nrl,nrh,ncl,nch;
{
	int i,k;
	float **m;
nrl = 0; ncl=0;
	m=(float **) Tcl_Alloc((unsigned) (nrh-nrl+1)*sizeof(float*));
	if (!m) nrerror("allocation failure 1 in matrix()");
	m -= nrl;

	for(i=nrl;i<=nrh;i++) {
		m[i]=(float *) Tcl_Alloc((unsigned) (nch-ncl+1)*sizeof(float));
		if (!m[i]) nrerror("allocation failure 2 in matrix()");
		m[i] -= ncl;
	}
        for (i=nrl;i<=nrh;i++)
            for (k=ncl;k<=nch;k++) m[i][k] = 0.0;
	return m;
}

double **dmatrix(nrl,nrh,ncl,nch)
int nrl,nrh,ncl,nch;
{
	int i;
	double **m;
nrl = 0; ncl=0;
	m=(double **) Tcl_Alloc((unsigned) (nrh-nrl+1)*sizeof(double*));
	if (!m) nrerror("allocation failure 1 in dmatrix()");
	m -= nrl;

	for(i=nrl;i<=nrh;i++) {
		m[i]=(double *) Tcl_Alloc((unsigned) (nch-ncl+1)*sizeof(double));
		if (!m[i]) nrerror("allocation failure 2 in dmatrix()");
		m[i] -= ncl;
	}
	return m;
}

int **imatrix(nrl,nrh,ncl,nch)
int nrl,nrh,ncl,nch;
{
	int i,**m;
nrl = 0; ncl=0;
	m=(int **) Tcl_Alloc((unsigned) (nrh-nrl+1)*sizeof(int*));
	if (!m) nrerror("allocation failure 1 in imatrix()");
	m -= nrl;

	for(i=nrl;i<=nrh;i++) {
		m[i]=(int *) Tcl_Alloc((unsigned) (nch-ncl+1)*sizeof(int));
		if (!m[i]) nrerror("allocation failure 2 in imatrix()");
		m[i] -= ncl;
	}
	return m;
}



float **submatrix(a,oldrl,oldrh,oldcl,oldch,newrl,newcl)
float **a;
int oldrl,oldrh,oldcl,oldch,newrl,newcl;
{
	int i,j;
	float **m;

	m=(float **) Tcl_Alloc((unsigned) (oldrh-oldrl+1)*sizeof(float*));
	if (!m) nrerror("allocation failure in submatrix()");
	m -= newrl;

	for(i=oldrl,j=newrl;i<=oldrh;i++,j++) m[j]=a[i]+oldcl-newcl;

	return m;
}


/*
void free_vector(v,nl,nh)
float *v;
int nl,nh;
{ nl=0;
	Tcl_Free((char*) (v+nl));
}

void free_ivector(v,nl,nh)
int *v,nl,nh;
{ nl=0;
	Tcl_Free((char*) (v+nl));
}

void free_dvector(v,nl,nh)
double *v;
int nl,nh;
{ nl=0;
	Tcl_Free((char*) (v+nl));
}
*/


void free_matrix(m,nrl,nrh,ncl,nch)
float **m;
int nrl,nrh,ncl,nch;
{
	int i;
nrl=0;ncl=0;
	for(i=nrh;i>=nrl;i--) Tcl_Free((char*) (m[i]+ncl));
	Tcl_Free((char*) (m+nrl));
}

void free_dmatrix(m,nrl,nrh,ncl,nch)
double **m;
int nrl,nrh,ncl,nch;
{
	int i;
nrl=0;ncl=0;
	for(i=nrh;i>=nrl;i--) Tcl_Free((char*) (m[i]+ncl));
	Tcl_Free((char*) (m+nrl));
}

void free_imatrix(m,nrl,nrh,ncl,nch)
int **m;
int nrl,nrh,ncl,nch;
{
	int i;
nrl=0;ncl=0;
	for(i=nrh;i>=nrl;i--) Tcl_Free((char*) (m[i]+ncl));
	Tcl_Free((char*) (m+nrl));
}



void free_submatrix(b,nrl,nrh,ncl,nch)
float **b;
int nrl,nrh,ncl,nch;
{
	Tcl_Free((char*) (b+nrl));
}



float **convert_matrix(a,nrl,nrh,ncl,nch)
float *a;
int nrl,nrh,ncl,nch;
{
	int i,j,nrow,ncol;
	float **m;
ncl=0;nrl=0;
	nrow=nrh-nrl+1;
	ncol=nch-ncl+1;
	m = (float **) Tcl_Alloc((unsigned) (nrow)*sizeof(float*));
	if (!m) nrerror("allocation failure in convert_matrix()");
	m -= nrl;
	for(i=0,j=nrl;i<=nrow-1;i++,j++) m[j]=a+ncol*i-ncl;
	return m;
}



void free_convert_matrix(b,nrl,nrh,ncl,nch)
float **b;
int nrl,nrh,ncl,nch;
{nrl=0;
	Tcl_Free((char*) (b+nrl));
}

/*

void SASFIT_gsl_vector_x2GlobalAP_GCP(gsl_vector *x,
                     void *params)
{
    int i,j,k,index;

    for (j=0;j<=((struct Gparam *) params) -> mfit-1;j++) {
       i = (int)floor(((struct Gparam *) params) -> lista[j]/(3*MAXPAR));
       k = ((struct Gparam *) params) -> lista[j]-(int)floor(((struct Gparam *) params) -> lista[j]/(3*MAXPAR))*(3*MAXPAR);
       if ((int)floor(k/MAXPAR) == 0) {
          index = ((struct Gparam *) params) -> GlobalAP[i].SD_index[k-(int)floor(k/MAXPAR)*MAXPAR];
	   } else if ((int)floor(k/MAXPAR) == 1) {
          index = ((struct Gparam *) params) -> GlobalAP[i].FF_index[k-(int)floor(k/MAXPAR)*MAXPAR];
	   } else {
          index = ((struct Gparam *) params) -> GlobalAP[i].SQ_index[k-(int)floor(k/MAXPAR)*MAXPAR];
	   }


       if (index > 0) {
		   ((struct Gparam *) params) -> GCP.P_common[index-1] = gsl_vector_get(x,j);
	   } else {
          if ((int)floor(k/MAXPAR) == 0) {
		     ((struct Gparam *) params) -> GlobalAP[i].SD_a[k-(int)floor(k/MAXPAR)*MAXPAR]
			     = gsl_vector_get(x,j);
		  } else if ((int)floor(k/MAXPAR) == 1) {
		     ((struct Gparam *) params) -> GlobalAP[i].FF_l[k-(int)floor(k/MAXPAR)*MAXPAR]
			     = gsl_vector_get(x,j);
		  } else {
		     ((struct Gparam *) params) -> GlobalAP[i].SQ_s[k-(int)floor(k/MAXPAR)*MAXPAR]
			     = gsl_vector_get(x,j);
		  }
	   }
	}

}
*/
bool FFtypestr_equal(Tcl_Interp *interp, char *str1, char *str2) {
	char sBuffer[256];
	int i;

	if (strcmp(str1,str2) == 0) return TRUE;

	sprintf(sBuffer,"<F> %s", str2);
	if (strcmp(str1,sBuffer) == 0) return TRUE;

	for (i=1;i<=MAXPAR;i++) {
		sprintf(sBuffer,"d %s / d l%d", str2,i);
		if (strcmp(str1,sBuffer) == 0) return TRUE;

		sprintf(sBuffer,"<F> d %s / d l%d", str2,i);
		if (strcmp(str1,sBuffer) == 0) return TRUE;
	}
	return FALSE;
}

