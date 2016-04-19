/*
 * src/sasfit_old/DLS/DLS_models.c
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

/*#########################################################################*/
/*#                                                                       #*/
/*#                                                                       #*/
/*#                                                                       #*/
/*#                                                                       #*/
/*#                                                                       #*/
/*# Written by Joachim Kohlbrecher                                        #*/
/*#########################################################################*/
/*#     27.09.2016    Joachim Kohlbrecher                                 #*/
/*#########################################################################*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <tcl.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>
#include "DLS_g2_1.h"
#include "../include/sasfit.h"

void DoubleCumulantNew(double,double*,double*, double*, bool*);
void CumulantNew(double,double*,double*, double*, bool*);
void CumulantTraditional(double,double*,double*, double*, bool*);


int
DLS_f (const gsl_vector * x, void *params, 
        gsl_vector * f)
{
  size_t n =      ((struct DLSPar *) params)->n;
  double *t =     ((struct DLSPar *) params)->t;
  double *y =     ((struct DLSPar *) params)->y;
  double *yth =   ((struct DLSPar *) params)->yth;
  double *sigma = ((struct DLSPar *) params)->sigma;
  double *w =     ((struct DLSPar *) params)->w;
  int *active =   ((struct DLSPar *) params)->active;
  char   *DLSmodel =   ((struct DLSPar *) params)->DLSmodel; 
//  double argexp;
  double Yi,Par[MAXDLSPAR],dydpar[MAXDLSPAR];
  int    ma,j;
  void CumulantNew(), DoubleStretchedExpDecay();
  bool error;
  size_t i;

  ma = 0;
  for (j=0;j<MAXDLSPAR; j++) {
	  if (active[j] == 1) {
         Par[j] = gsl_vector_get (x, ma);
		 ma++;
	  } else {
		 Par[j] = ((struct DLSPar *) params)->Par[j];
	  }
  }


  for (i = 0; i < n; i++)
    {
	if ( strcmp(DLSmodel,"cumulant analysis (recommended)") == 0 ) {
		CumulantNew(t[i],Par,&Yi, dydpar, &error);
	}
   	if ( strcmp(DLSmodel,"cumulant analysis (traditional)") == 0 ) {
		CumulantTraditional(t[i],Par,&Yi, dydpar, &error);
	}
	if ( strcmp(DLSmodel,"double decay cumulant analysis") == 0 ) {
		DoubleCumulantNew(t[i],Par,&Yi, dydpar, &error);
	}
	if ( strcmp(DLSmodel,"double stretched exponential") == 0 ) {
		DoubleStretchedExpDecay(t[i],Par,&Yi, dydpar, &error);
	}

	  yth[i]=Yi;
      gsl_vector_set (f, i, (Yi - y[i])*sqrt(fabs(w[i]))/sigma[i]);
    }

  return GSL_SUCCESS;

}

int DLS_df (const gsl_vector * x, void *params, gsl_matrix * J)
{
  size_t n =      ((struct DLSPar *) params)->n;
  double *t =     ((struct DLSPar *) params)->t;
  double *sigma = ((struct DLSPar *) params)->sigma;
  double *w =     ((struct DLSPar *) params)->w;
  int *active =   ((struct DLSPar *) params)->active;
  char   *DLSmodel =   ((struct DLSPar *) params)->DLSmodel; 
//  double argexp;
  double Yi;
  double Par[MAXDLSPAR],dPar[MAXDLSPAR],dydpar[MAXDLSPAR];
  bool error;
  int    j,ma;
  void Cumulant(), DoubleStretchedExpDecay();
  size_t i;

  ma = 0;
  for (j=0;j<MAXDLSPAR; j++) {
	  if (active[j] == 1) {
         Par[j] = gsl_vector_get (x, ma);
		 ma++;
	  } else {
		 Par[j] = ((struct DLSPar *) params)->Par[j];
	  }
  }


  for (i = 0; i < n; i++) {
	if ( strcmp(DLSmodel,"cumulant analysis (recommended)") == 0 ) {
		CumulantNew(t[i],Par,&Yi, dydpar, &error);
	}
	if ( strcmp(DLSmodel,"cumulant analysis (traditional)") == 0 ) {
		CumulantTraditional(t[i],Par,&Yi, dydpar, &error);
	}
	if ( strcmp(DLSmodel,"double stretched exponential") == 0 ) {
		DoubleStretchedExpDecay(t[i],Par,&Yi, dydpar, &error);
	}
	if ( strcmp(DLSmodel,"double decay cumulant analysis") == 0 ) {
		DoubleCumulantNew(t[i],Par,&Yi, dydpar, &error);
	}

	for (j=0; j<MAXDLSPAR; j++) {
		dPar[j] = dydpar[j]*sqrt(fabs(w[i]))/sigma[i];
	}

	  ma = 0;
      for (j=0; j<MAXDLSPAR; j++) {
	    if (active[j] == 1) {
		    gsl_matrix_set (J, i, ma,dPar[j]);
		    ma++;
		} else {
		    Par[j] = ((struct DLSPar *) params)->Par[j];
		}
	  }
  }

  return GSL_SUCCESS;

}

int
DLS_fdf (const gsl_vector * x, void *params,
          gsl_vector * f, gsl_matrix * J)
{
  DLS_f (x, params, f);
  DLS_df (x, params, J);

  return GSL_SUCCESS;
}

void CumulantTraditional(double x,double *par,double *y, double *dydpar, bool *error)
{
	double argexp;
	argexp = -2.0*par[1]*x
		     +    par[2]*x*x
		   	 -    par[3]*x*x*x/3.0;

	*y = par[0]*exp(argexp);
     dydpar[0] = exp(argexp);
	 dydpar[1] = -2.0*x*(*y);
     dydpar[2] = x*x*(*y);
     dydpar[3] = -1.0/3.0*x*x*x*(*y);
     dydpar[4] = 1.0;
	 dydpar[5] = 0.0;
	 dydpar[6] = 0.0;
	 *y = *y + par[4];
	return;
}

void DoubleCumulantTraditional(double x,double *par,double *y, double *dydpar, bool *error)
{
	double argexp1,argexp2,p,A,B;
	A = par[0];
	p = par[1];
	B = par[6];

	argexp1 = -2.0*par[2]*x
		      +    par[3]*x*x;

	argexp2 = -2.0*par[4]*x
		      +    par[5]*x*x;

	*y = A*(p*exp(argexp1)+(1.-p)*exp(argexp2)) + B;

    dydpar[0] = (   p    * exp(argexp1)
		        + (1.-p) * exp(argexp2) );

	dydpar[1] = A*( exp(argexp1)
		           -exp(argexp2) );

	dydpar[2] = -2.0*x* A*p*exp(argexp1);
    dydpar[3] =    x*x* A*p*exp(argexp1);

    dydpar[4] = -2.0*x* A*(1.-p)*exp(argexp2);
	dydpar[5] =    x*x* A*(1.-p)*exp(argexp2);

    dydpar[6] = 1.0;
	return;
}


void CumulantNew(double x,double *par,double *y, double *dydpar, bool *error)
{
	double argexp,factor;
	argexp = -2.0*par[1]*x;
    factor = 1 + par[2]*x*x
               - par[3]*x*x*x/3.0;


     dydpar[0] = exp(argexp)*factor*factor;
	 *y = par[0]*dydpar[0];
	 dydpar[1] = -2.0*x*(*y);
     dydpar[2] = 2*par[0]*x*x*exp(argexp)*factor;
     dydpar[3] = -par[0]*2.0/3.0*x*x*x*exp(argexp)*factor;
     dydpar[4] = 1.0;
	 dydpar[5] = 0.0;
	 dydpar[6] = 0.0;
	 *y = *y + par[4];
	return;
}

void DoubleCumulantNew(double x,double *par,double *y, double *dydpar, bool *error)
{
	double argexp1,argexp2,p,A,B,fac1, fac2;
	A = par[0];
	p = par[1];
	B = par[6];

	argexp1 = -2.0*par[2]*x;
	fac1  = 1.+ par[3]*x*x;

	argexp2 = -2.0*par[4]*x;
    fac2 = 1.+ par[5]*x*x;

	*y = A*(p*exp(argexp1)*fac1*fac1+(1.-p)*exp(argexp2)*fac2*fac2) + B;

    dydpar[0] = (p*exp(argexp1)*fac1*fac1+(1.-p)*exp(argexp2)*fac2*fac2);

	dydpar[1] = A*( exp(argexp1)*fac1*fac1
		           -exp(argexp2)*fac2*fac2) ;

	dydpar[2] = -2.0*x* A*p*exp(argexp1)*fac1*fac1;
    dydpar[3] =    2*x*x* A*p*exp(argexp1)*fac1;

    dydpar[4] = -2.0*x* A*(1.-p)*exp(argexp2)*fac2*fac2;
	dydpar[5] =    2*x*x* A*(1.-p)*exp(argexp2)*fac2;

    dydpar[6] = 1.0;
	return;
}

void DoubleStretchedExpDecay(double x,double *par,double *y, double *dydpar, bool *error)
{
	double A,B,p,tau1,tau2,gamma1,gamma2;
	A = par[0];
	B = par[1];
	p = par[2];
	tau1 = par[3];
	gamma1 = par[4];
	tau2 = par[5];
	gamma2 = par[6];
	if (tau1==0) {
	   *y = 0.0;
	   dydpar[0] = 0;
	   dydpar[2] = 0;
	   dydpar[3] = 0.0;
	   dydpar[4] = 0.0;
	} else {
  	   *y = A*p*exp(-pow(x/tau1,gamma1));
	   dydpar[0] = p*exp(-pow(x/tau1,gamma1));
	   dydpar[2] = A*exp(-pow(x/tau1,gamma1)); 
       dydpar[3] =  A*p*exp(-pow(x/tau1,gamma1))*pow(x/tau1,gamma1)*gamma1/tau1;
       dydpar[4] = -A*p*exp(-pow(x/tau1,gamma1))*pow(x/tau1,gamma1)*log(x/tau1);
	}
	if (tau2 == 0) {
		dydpar[5] = 0.0;
		dydpar[6] = 0.0;
	} else {
  	    *y = (*y)+A*(1.-p)*exp(-pow(x/tau2,gamma2));
	    dydpar[0] = dydpar[0]+(1.-p)*exp(-pow(x/tau2,gamma2));
	    dydpar[2] = dydpar[2]-A*exp(-pow(x/tau2,gamma2));
	    dydpar[5] =  A*(1.-p)*exp(-pow(x/tau2,gamma2))*pow(x/tau2,gamma2)*gamma2/tau2;
        dydpar[6] = -A*(1.-p)*exp(-pow(x/tau2,gamma2))*pow(x/tau2,gamma2)*log(x/tau2);
	}

	*y = (*y)+B;
	dydpar[1] = 1.;

	return;
}


void DLS(interp,x,DLSParData,yth,dydpar,error)
Tcl_Interp *interp;
double x;
struct DLSPar DLSParData;
double *yth;
double *dydpar;
bool *error;
{
	if ( strcmp(DLSParData.DLSmodel,"cumulant analysis (recommended)") == 0 ) {
		CumulantNew(x,DLSParData.Par,yth,dydpar,error);
	} else if ( strcmp(DLSParData.DLSmodel,"cumulant analysis (traditional)") == 0 ) {
		CumulantTraditional(x,DLSParData.Par,yth,dydpar,error);
	} else if ( strcmp(DLSParData.DLSmodel,"double decay cumulant analysis") == 0 ) {
		DoubleCumulantNew(x,DLSParData.Par,yth,dydpar,error);
	} else if ( strcmp(DLSParData.DLSmodel,"double stretched exponential") == 0 ) {
		DoubleStretchedExpDecay(x,DLSParData.Par,yth,dydpar,error);
	} else {
        sasfit_err("Unknown function\nContact developer\n");
    }
}
