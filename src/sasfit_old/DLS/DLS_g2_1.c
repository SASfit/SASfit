/*
 * src/sasfit_old/DLS/DLS_g2_1.c
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
/*# Fitting approximations for Q -> infinity and Q -> 0  of experimental  #*/
/*# data Q, I(Q), Delta I(Q).                                             #*/
/*#                                                                       #*/
/*# implemented approximations :                                          #*/
/*#                                                                       #*/
/*#   for Q -> infinity :                                                 #*/
/*#      Porod approx.:              I(Q) = c0 + c4 * Q^(D-4)             #*/
/*#                                                                       #*/
/*#   for Q -> 0:                                                         #*/
/*#      Guinier approx.:            I(Q)   = I0 * exp(Q^2*Rg^2/3)        #*/
/*#      Zimm approx.:               1/I(Q) = 1/I0 * (1 + Rg^2*Q^2/3)     #*/
/*#      Ornstein-Zernicke approx. : I(Q)   = I0 / (1 + xi^2*Q^2)         #*/
/*#                                                                       #*/
/*#                                                                       #*/
/*# Written by Joachim Kohlbrecher                                        #*/
/*#########################################################################*/
/*#     27.09.2016    Joachim Kohlbrecher                                 #*/
/*#########################################################################*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tcl.h>
#include "../include/sasfit.h"
#include "DLS_g2_1.h"
#include "../include/tcl_cmds.h"

int DLS_CumulantFitCmd(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int        argc;
    char       **argv;
{
   struct DLSPar DLSParData;
   double *w, *h, *Ih, *DIh, *Ith, *dydpar;
   int ndata,ma,nskip,niter;
   Tcl_DString DsBuffer;
   int i,k,stable_iter;
   bool error, stable;
   double MAD, LastMAD,stdresid, chi;
   char sBuffer[256];

   const gsl_multifit_fdfsolver_type *T;
   gsl_multifit_fdfsolver *s;
   gsl_multifit_function_fdf f;
   gsl_matrix *covar, *J;
   gsl_vector *x;
   int status;


   size_t iter;
   MAD = 0.0; LastMAD = 10.0;

   if (argc != 3) {
      sasfit_err("wrong # args: shoud be dls_CumulantFit ?DLS_par? ?xyer_data?\n");
       return TCL_ERROR;
   }
   
   if (TCL_ERROR == get_DLSpar(clientData,interp,argv,
                               &DLSParData, &h,&Ih,&DIh)) {
       return TCL_ERROR;
   }

   stable = TRUE;
   ndata = DLSParData.ndata;
   DLSParData.n = ndata;
   DLSParData.sigma = DIh;
   DLSParData.y = Ih;
   DLSParData.t = h;

   ma = 0;
   for (i=0;i<MAXDLSPAR;i++) {
	   if (DLSParData.active[i] == 1) {
		   ma++;
	   }
   }
   x = gsl_vector_alloc(ma);
   k=0;
   for (i=0;i<MAXDLSPAR;i++) {
	   if (DLSParData.active[i] == 1) {
		   gsl_vector_set(x,k,DLSParData.Par[i]);
		   k++;
	   } 
   }

   Ith = dvector(0,ndata-1);
   dydpar = dvector(0,ma-1);
   DLSParData.yth=Ith;
   w = dvector(0,ndata-1);
   for (i=0;i<ndata;i++) w[i]= 1.0;
   DLSParData.w=w;
   stable_iter = 0;
   niter = (int) DLSParData.iter;
   DLSParData.iter_d = niter;

   if (niter <1) niter = 100;
   do {
	 stable_iter ++;
     iter = 0;
     covar = gsl_matrix_alloc (ma, ma);
     J = gsl_matrix_alloc (ndata, ma);
     f.f = &DLS_f;
     f.df = &DLS_df;
     f.fdf = &DLS_fdf;
     f.n = ndata;
     f.p = ma;
     f.params = &DLSParData;
     
     T = gsl_multifit_fdfsolver_lmsder;
     s = gsl_multifit_fdfsolver_alloc (T, ndata, ma);
     gsl_multifit_fdfsolver_set (s, &f, x);
     do {
        iter++;
        status = gsl_multifit_fdfsolver_iterate (s);
        if (status) break;
        status = gsl_multifit_test_delta (s->dx, s->x,1e-6, 1e-6);

     } while (status == GSL_CONTINUE && iter < 500);
     
     gsl_multifit_fdfsolver_jac(s, J);
     gsl_multifit_covar (J, 0.0, covar);
	 chi = gsl_blas_dnrm2 (s->f)/sqrt(ndata-ma);

	 k = 0;
     DLSParData.chisqr = chi*chi;
     for (i=0;i<MAXDLSPAR;i++) {
		 if (DLSParData.active[i] == 1) {
            DLSParData.Par[i] = gsl_vector_get(gsl_multifit_fdfsolver_position(s), k);
            DLSParData.Err[i] = sqrt(fabs(gsl_matrix_get (covar,k,k)))*(0+1*chi);
			k++;
		 } else {
            DLSParData.Err[i] = 0.0;
		 }
	 }
     save_DLSpar(clientData,interp,argv,&DLSParData);
     gsl_multifit_fdfsolver_free (s);
	 gsl_matrix_free (covar);
	 gsl_matrix_free (J);

	 MAD = 0.0;
	 for (i=0;i<ndata;i++) {
		 if (DLSParData.w[i] > 1e-8) {
		    MAD = MAD + fabs(Ith[i]-Ih[i])/ndata;
		 }
	 }
	 nskip = 0;
	 for (i=0;i<ndata;i++) {
		 stdresid = fabs(Ith[i]-Ih[i])/(4.685*MAD/0.6745);
	     if (stdresid < 1.0) {
		    DLSParData.w[i] = fabs(1.0-stdresid*stdresid);
		 } else {
			 DLSParData.w[i] = 1e-8;
			 nskip ++;
		 }
	 }
	 if (fabs(MAD - LastMAD) < 1e-8) {
		 stable = TRUE;
	 } else {
		 stable = FALSE;
	 }
	 
	 sasfit_out("stable_iter: %d MAD %g Delta_MAD: %g\n ndata: %d  nskip: %d\n",
		              stable_iter,MAD,fabs(MAD - LastMAD),ndata,nskip);


	 LastMAD = MAD;
	 DLSParData.iter_d = stable_iter;

   } while (stable == FALSE && stable_iter <= niter);

   gsl_vector_free(x);

   error = FALSE;
   Tcl_DStringInit(&DsBuffer);
   Tcl_DStringStartSublist(&DsBuffer);
   for (i=0;i<ndata;i++) {
	   /*
	  DLS(interp,h[i],DLSParData,&Ith[i],dydpar,&error);
	  */
      float_to_string(sBuffer,Ith[i]);
      Tcl_DStringAppendElement(&DsBuffer,sBuffer);
      if (error==TRUE) {
         free_dvector(Ith,0,ndata-1);
         free_dvector(h,0,ndata-1);
         free_dvector(Ih,0,ndata-1);
         free_dvector(DIh,0,ndata-1);
         free_dvector(dydpar,0,ma-1);
         free_dvector(w,0,ndata-1);
		 Tcl_DStringFree(&DsBuffer);
         return TCL_ERROR;
	  }
   }
   Tcl_DStringEndSublist(&DsBuffer);

   Tcl_DStringStartSublist(&DsBuffer);
   for (i=0;i<ndata;i++) {
	  if (DLSParData.w[i] > 1e-8) {
		DIh[i] = chi/DLSParData.w[i];
	  } else {
		DIh[i] = DLSParData.Par[0];
	  }
      float_to_string(sBuffer,DIh[i]);
      Tcl_DStringAppendElement(&DsBuffer,sBuffer);
   }
   Tcl_DStringEndSublist(&DsBuffer);

   Tcl_DStringStartSublist(&DsBuffer);
   for (i=0;i<ndata;i++) {
      float_to_string(sBuffer,(Ih[i]-Ith[i]));
      Tcl_DStringAppendElement(&DsBuffer,sBuffer);
   }
   Tcl_DStringEndSublist(&DsBuffer);
   Tcl_DStringResult(interp,&DsBuffer);

   free_dvector(Ith,0,ndata-1);
   free_dvector(h,0,ndata-1);
   free_dvector(Ih,0,ndata-1);
   free_dvector(DIh,0,ndata-1);
   free_dvector(dydpar,0,ma-1);
   free_dvector(w,0,ndata-1);

   return TCL_OK;
}

/*
int DLS_ExponentialFitCmd(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int        argc;
    char       **argv;
{
return TCL_OK;
}

int DLS_StretchedExpFitCmd(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int        argc;
    char       **argv;
{
return TCL_OK;
}

*/

int DLS_CumulantCalcCmd(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int        argc;
    char       **argv;
{
   struct DLSPar DLSParData;
   double *h, *Ih, *DIh, *Ith, *dydpar;
   int ndata,ma;
   char   sBuffer[256];
   Tcl_DString DsBuffer;
   int i;
   bool error;

   if (argc != 3) {
      sasfit_err("wrong # args: shoud be dls_CumulantCalc ?DLS_par? ?xyer_data?\n");
       return TCL_ERROR;
   }
   
   if (TCL_ERROR == get_DLSpar(clientData,interp,argv,
                               &DLSParData, &h,&Ih,&DIh)) {
       return TCL_ERROR;
   }

   ma = MAXDLSPAR;
   ndata = DLSParData.ndata;
   Ith = dvector(0,ndata-1);
   dydpar = dvector(0,ma-1);
   Tcl_DStringInit(&DsBuffer);
   Tcl_DStringStartSublist(&DsBuffer);
   for (i=0;i<ndata;i++) {
	  DLS(interp,h[i],DLSParData,&Ith[i],dydpar,&error);
      float_to_string(sBuffer,Ith[i]);
      Tcl_DStringAppendElement(&DsBuffer,sBuffer);
      if (error==TRUE) {
         free_dvector(Ith,0,ndata-1);
         free_dvector(h,0,ndata-1);
         free_dvector(Ih,0,ndata-1);
         free_dvector(DIh,0,ndata-1);
         free_dvector(dydpar,0,ma-1);
	 Tcl_DStringFree(&DsBuffer);
         return TCL_ERROR;
	  }
   }
   Tcl_DStringEndSublist(&DsBuffer);

   Tcl_DStringStartSublist(&DsBuffer);
   for (i=0;i<ndata;i++) {
      float_to_string(sBuffer,DIh[i]);
      Tcl_DStringAppendElement(&DsBuffer,sBuffer);
   }
   Tcl_DStringEndSublist(&DsBuffer);

   Tcl_DStringStartSublist(&DsBuffer);
   for (i=0;i<ndata;i++) {
      float_to_string(sBuffer,(Ih[i]-Ith[i]));
      Tcl_DStringAppendElement(&DsBuffer,sBuffer);
   }
   Tcl_DStringEndSublist(&DsBuffer);
   Tcl_DStringResult(interp,&DsBuffer);
  
   free_dvector(Ith,0,ndata-1);
   free_dvector(h,0,ndata-1);
   free_dvector(Ih,0,ndata-1);
   free_dvector(DIh,0,ndata-1);
   free_dvector(dydpar,0,ma-1);

   return TCL_OK;
}


/*
int DLS_ExponentialCalcCmd(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int        argc;
    char       **argv;
{
return TCL_OK;
}

int DLS_StretchedExpCalcCmd(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int        argc;
    char       **argv;
{
return TCL_OK;
}
*/


int get_DLSpar(clientData,interp,argv,DLSParData,Q,IQ,DIQ)
    ClientData       clientData;
    Tcl_Interp       *interp;
    char             **argv;
    struct DLSPar    *DLSParData;
    double           **Q, **IQ, **DIQ;
{
    int    j;
    int    Splitcode, Splitargc, tcode, targc;
    const char   **Splitargv, **targv;
    int    ndata,itmp;
    double  *h, *Ih, *DIh;
	double dtmp;
    char   *xyer_data;
/*
 * read scattering curve h, Ih and DIh
 */
    xyer_data   = argv[2];
    Splitcode = Tcl_SplitList(interp,xyer_data,&Splitargc,&Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_err("scattering data {h Ih} is not a list\n");
       return TCL_ERROR;
    }
    if ((Splitargc < 2) || (Splitargc > 3)) {
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in {h Ih <DIh>}\n");
       return TCL_ERROR;
    }
    Splitcode = Tcl_SplitList(interp,Splitargv[0],&targc,&targv);
    if (Splitcode == TCL_ERROR) {
       Tcl_Free((char *) Splitargv);
       Tcl_Free((char *) targv);
       sasfit_err("could not read h\n");
       return TCL_ERROR;
    }

    ndata = targc;
    (*DLSParData).ndata = ndata;

    h   = dvector(0,ndata-1);
    Ih  = dvector(0,ndata-1);
    DIh = dvector(0,ndata-1);

    for (j=0;j<targc;j++) {
       tcode = Tcl_GetDouble(interp,targv[j],&h[j]);
       if (tcode == TCL_ERROR) {
          free_dvector(h,0,ndata-1);
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
       free_dvector(Ih,0,ndata-1);
       free_dvector(DIh,0,ndata-1);
       Tcl_Free((char *) Splitargv);
       Tcl_Free((char *) targv);
       sasfit_err("could not read Ih\n");
       return TCL_ERROR;
    }
    if (targc != ndata) {
       free_dvector(h,0,ndata-1);
       free_dvector(Ih,0,ndata-1);
       free_dvector(DIh,0,ndata-1);
       Tcl_Free((char *) Splitargv);
       Tcl_Free((char *) targv);
       sasfit_err("could not convert Ih[j]) to double\n");
       return TCL_ERROR;
    }
    for (j=0;j<targc;j++) {
       tcode = Tcl_GetDouble(interp,targv[j],&Ih[j]);
       if (tcode == TCL_ERROR) {
          free_dvector(h,0,ndata-1);
          free_dvector(Ih,0,ndata-1);
          free_dvector(DIh,0,ndata-1);
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("could not convert Ih[j]) to double\n");
          return TCL_ERROR;
       }
    }
	if (Splitargc != 3) {
		for (j=0;j<ndata;j++) DIh[j] = 1.0;
	} else {
       Splitcode = Tcl_SplitList(interp,Splitargv[2],&targc,&targv);
       if (Splitcode == TCL_ERROR) {
          free_dvector(h,0,ndata-1);
          free_dvector(Ih,0,ndata-1);
          free_dvector(DIh,0,ndata-1);
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("could not read DIh\n");
          return TCL_ERROR;
	   }
       if (targc != ndata) {
          free_dvector(h,0,ndata-1);
          free_dvector(Ih,0,ndata-1);
          free_dvector(DIh,0,ndata-1);
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("could not convert DIh[j]) to double\n");
          return TCL_ERROR;
	   }
       for (j=0;j<targc;j++) {
          tcode = Tcl_GetDouble(interp,targv[j],&DIh[j]);
          if (tcode == TCL_ERROR) {
             free_dvector(h,0,ndata-1);
             free_dvector(Ih,0,ndata-1);
             free_dvector(DIh,0,ndata-1);
             Tcl_Free((char *) Splitargv);
		     Tcl_Free((char *) targv);
             sasfit_err("could not convert DIh[j]) to double\n");
             return TCL_ERROR;
		  }
	   }
	}


    (*Q)    = h;
    (*IQ)   = Ih;
    (*DIQ)  = DIh;

/*
 * read parameter iter
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"iter",0),
                             &dtmp) ) {
       sasfit_err("could not read parameter iter\n");
       return TCL_ERROR;
    }
    (*DLSParData).iter = dtmp;

/*
 * read parameter Par1 .. Par7
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"Par1",0),
                             &dtmp) ) {
       sasfit_err("could not read parameter 1\n");
       return TCL_ERROR;
    }
    (*DLSParData).Par[0] = dtmp;

    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"Par2",0),
                             &dtmp) ) {
       sasfit_err("could not read parameter 2\n");
       return TCL_ERROR;
    }
    (*DLSParData).Par[1] = dtmp;

    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"Par3",0),
                             &dtmp) ) {
       sasfit_err("could not read parameter 3\n");
       return TCL_ERROR;
    }
    (*DLSParData).Par[2] = dtmp;

    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"Par4",0),
                             &dtmp) ) {
       sasfit_err("could not read parameter 4\n");
       return TCL_ERROR;
    }
    (*DLSParData).Par[3] = dtmp;


    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"Par5",0),
                             &dtmp) ) {
       sasfit_err("could not read parameter 5\n");
       return TCL_ERROR;
    }
    (*DLSParData).Par[4] = dtmp;

    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"Par6",0),
                             &dtmp) ) {
       sasfit_err("could not read parameter 6\n");
       return TCL_ERROR;
    }
    (*DLSParData).Par[5] = dtmp;

    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"Par7",0),
                             &dtmp) ) {
       sasfit_err("could not read parameter 7\n");
       return TCL_ERROR;
    }
    (*DLSParData).Par[6] = dtmp;

/*  
  read errors of parameter Par1err .. Par7err
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"Par1err",0),
                             &dtmp) ) {
       sasfit_err("could not read error for parameter 1\n");
       return TCL_ERROR;
    }
    (*DLSParData).Err[0] = dtmp;

    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"Par2err",0),
                             &dtmp) ) {
       sasfit_err("could not read error for parameter 2\n");
       return TCL_ERROR;
    }
    (*DLSParData).Err[1] = dtmp;

    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"Par3err",0),
                             &dtmp) ) {
       sasfit_err("could not read error for parameter 3\n");
       return TCL_ERROR;
    }
    (*DLSParData).Err[2] = dtmp;

    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"Par4err",0),
                             &dtmp) ) {
       sasfit_err("could not read error for parameter 4\n");
       return TCL_ERROR;
    }
    (*DLSParData).Err[3] = dtmp;


    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"Par5err",0),
                             &dtmp) ) {
       sasfit_err("could not read error for parameter 5\n");
       return TCL_ERROR;
    }
    (*DLSParData).Err[4] = dtmp;

    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"Par6err",0),
                             &dtmp) ) {
       sasfit_err("could not read error for parameter 6\n");
       return TCL_ERROR;
    }
    (*DLSParData).Err[5] = dtmp;

    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"Par7err",0),
                             &dtmp) ) {
       sasfit_err("could not read error for parameter 7\n");
       return TCL_ERROR;
    }
    (*DLSParData).Err[6] = dtmp;



/*
 * read active Flag of parameter Par1 .. Par7
 */
    if (TCL_ERROR == Tcl_GetBoolean(interp,
                             Tcl_GetVar2(interp,argv[1],"Par1active",0),
                             &itmp) ) {
       sasfit_err("could not read active-flag for parameter 1\n");
       return TCL_ERROR;
    }
    (*DLSParData).active[0] = itmp;

    if (TCL_ERROR == Tcl_GetBoolean(interp,
                             Tcl_GetVar2(interp,argv[1],"Par2active",0),
                             &itmp) ) {
       sasfit_err("could not read active-flag for parameter 2\n");
       return TCL_ERROR;
    }
    (*DLSParData).active[1] = itmp;

    if (TCL_ERROR == Tcl_GetBoolean(interp,
                             Tcl_GetVar2(interp,argv[1],"Par3active",0),
                             &itmp) ) {
       sasfit_err("could not read active-flag for parameter 3\n");
       return TCL_ERROR;
    }
    (*DLSParData).active[2] = itmp;

    if (TCL_ERROR == Tcl_GetBoolean(interp,
                             Tcl_GetVar2(interp,argv[1],"Par4active",0),
                             &itmp) ) {
       sasfit_err("could not read active-flag for parameter 4\n");
       return TCL_ERROR;
    }
    (*DLSParData).active[3] = itmp;


    if (TCL_ERROR == Tcl_GetBoolean(interp,
                             Tcl_GetVar2(interp,argv[1],"Par5active",0),
                             &itmp) ) {
       sasfit_err("could not read active-flag for parameter 5\n");
       return TCL_ERROR;
    }
    (*DLSParData).active[4] = itmp;

    if (TCL_ERROR == Tcl_GetBoolean(interp,
                             Tcl_GetVar2(interp,argv[1],"Par6active",0),
                             &itmp) ) {
       sasfit_err("could not read active-flag for parameter 6\n");
       return TCL_ERROR;
    }
    (*DLSParData).active[5] = itmp;

    if (TCL_ERROR == Tcl_GetBoolean(interp,
                             Tcl_GetVar2(interp,argv[1],"Par7active",0),
                             &itmp) ) {
       sasfit_err("could not read active-flag for parameter 7\n");
       return TCL_ERROR;
    }
    (*DLSParData).active[6] = itmp;



/*
 *  read DLS model
 */

	strcpy((*DLSParData).DLSmodel,Tcl_GetVar2(interp,argv[1],"DLSmodel",0));

/*
 *  read type of error
 */

	strcpy((*DLSParData).error_type,Tcl_GetVar2(interp,argv[1],"DLSerror",0));


	return TCL_OK;
}


int save_DLSpar(clientData,interp,argv,DLSParData)
    ClientData       clientData;
    Tcl_Interp       *interp;
    char             **argv;
    struct DLSPar    *DLSParData;
{
 	char    sBuffer[256],tsBuffer[256];
     
    float_to_string(sBuffer,(*DLSParData).Par[0]);
    Tcl_SetVar2(interp,argv[1],"Par1", sBuffer,0);
    float_to_string(sBuffer,(*DLSParData).Par[1]);
    Tcl_SetVar2(interp,argv[1],"Par2", sBuffer,0);
    float_to_string(sBuffer,(*DLSParData).Par[2]);
    Tcl_SetVar2(interp,argv[1],"Par3", sBuffer,0);
    float_to_string(sBuffer,(*DLSParData).Par[3]);
    Tcl_SetVar2(interp,argv[1],"Par4", sBuffer,0);
    float_to_string(sBuffer,(*DLSParData).Par[4]);
    Tcl_SetVar2(interp,argv[1],"Par5", sBuffer,0);
    float_to_string(sBuffer,(*DLSParData).Par[5]);
    Tcl_SetVar2(interp,argv[1],"Par6", sBuffer,0);
    float_to_string(sBuffer,(*DLSParData).Par[6]);
    Tcl_SetVar2(interp,argv[1],"Par7", sBuffer,0);


    float_to_string(sBuffer,(*DLSParData).Err[0]);
    Tcl_SetVar2(interp,argv[1],"Par1err", sBuffer,0);
    float_to_string(sBuffer,(*DLSParData).Err[1]);
    Tcl_SetVar2(interp,argv[1],"Par2err", sBuffer,0);
    float_to_string(sBuffer,(*DLSParData).Err[2]);
    Tcl_SetVar2(interp,argv[1],"Par3err", sBuffer,0);
    float_to_string(sBuffer,(*DLSParData).Err[3]);
    Tcl_SetVar2(interp,argv[1],"Par4err", sBuffer,0);
    float_to_string(sBuffer,(*DLSParData).Err[4]);
    Tcl_SetVar2(interp,argv[1],"Par5err", sBuffer,0);
    float_to_string(sBuffer,(*DLSParData).Err[5]);
    Tcl_SetVar2(interp,argv[1],"Par6err", sBuffer,0);
    float_to_string(sBuffer,(*DLSParData).Err[6]);
    Tcl_SetVar2(interp,argv[1],"Par7err", sBuffer,0);

	float_to_string(sBuffer,(*DLSParData).chisqr);
    Tcl_SetVar2(interp,argv[1],"chisqr", sBuffer,0);

    float_to_string(sBuffer,(*DLSParData).iter_d);
	sprintf(tsBuffer," no. of used iter (%s)",sBuffer);
    Tcl_SetVar2(interp,argv[1],"iter_d", tsBuffer,0);

	return TCL_OK;
}

