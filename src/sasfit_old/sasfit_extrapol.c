/*
 * src/sasfit_old/sasfit_extrapol.c
 *
 * Copyright (c) 2008-2020, Paul Scherrer Institute (PSI)
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
/*#     Version Va.1 27.09.99  JKcom                                      #*/
/*#########################################################################*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <tcl.h>

#include "include/sasfit.h"
#include "include/SASFIT_nr.h"
#include "include/tcl_cmds.h"
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_pow_int.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_roots.h>
#include <gsl/gsl_min.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_bspline.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_multifit_nlinear.h>
#include <gsl/gsl_multimin.h>
#include <gsl/gsl_sort_vector.h>
#include <time.h>
#include <math.h>
#include "sasfit_oz.h"
#include "sasfit_fixed_point_acc.h"
#include "nnls.h"
#include "linpack_d.h"
#include "blas1_d.h"
#include <kinsol/kinsol.h>

#define REDFACTOR 1.2

scalar inverse_quad_interp(scalar y, scalar x0, scalar y0, scalar x1, scalar y1, scalar x2, scalar y2) {
    scalar f0,f1,f2;
    f1=y1-y;
    f2=y2-y;
    f0=y0-y;
    return f1*f0/(f2-f1)/(f2-f0)*x2 + f2*f0/(f1-f2)/(f1-f0)*x1 + f2*f1/(f0-f2)/(f0-f1)*x0;
}

/*#########################################################################*/
/*#                                                                       #*/
/*#########################################################################*/
void Porod(Q,a,y,dyda,na,error,errstr)
float Q, a[], *y, dyda[];
int   na;
bool  *error;
char  *errstr;
{
   float c0, c1, c4, D;

   if (na != 4) {
      *error  = TRUE;
      sprintf(errstr,
              "Porod: needs three paramater in vector a[] but got %d",na);
      return;
   }
   c0 = a[0];
   c1 = a[1];
   c4 = a[2];
   D  = a[3];

   if (Q <= 0.0) {
      *error = TRUE;
      sprintf(errstr,
              "Porod: Q must be > 0; actual Q-value: %lg",Q);
      return;
   }

   *y = c0 + Q*c1 + c4*pow(Q,D-4.0);
   dyda[0] = 1.0;
   dyda[1] = Q;
   dyda[2] = pow(Q,D-4.0);
   dyda[3] = c4*pow(Q,D-4.0)*log(Q);
}

/*#########################################################################*/
/*#                                                                       #*/
/*#########################################################################*/
void Guinier(Q,a,y,dyda,na,error,errstr)
float Q, a[], *y, dyda[];
int  na;
bool *error;
char *errstr;
{
   float I0, Rg2;

   if (na != 2) {
      *error  = TRUE;
      sprintf(errstr,
              "Guinier: needs two paramater in vector a[] but got %d",na);
      return;
   }
   I0  = a[0];
   Rg2 = a[1];

   *y = I0 * exp(-Rg2*Q*Q/3.0);
   dyda[0] = exp(-Rg2*Q*Q/3.0);
   dyda[1] = -(*y) / 3.0 * Q*Q;
}

/*#########################################################################*/
/*#                                                                       #*/
/*#########################################################################*/
void Guinier2(Q,a,y,dyda,na,error,errstr)
float Q, a[], *y, dyda[];
int  na;
bool *error;
char *errstr;
{
   float I0, Rg;

   if (na != 2) {
      *error  = TRUE;
      sprintf(errstr,
              "Guinier2: needs two paramater in vector a[] but got %d",na);
      return;
   }
   I0 = a[0];
   Rg = a[1];

   *y = I0 * exp(-pow(Rg*Q,2.0)/3.0);
   dyda[0] = exp(-pow(Rg*Q,2.0)/3.0);
   dyda[1] = -(*y) * 2.0/3.0 * Rg*Q*Q;
}

/*#########################################################################*/
/*#                                                                       #*/
/*#########################################################################*/
void Debye(Q,a,y,dyda,na,error,errstr)
float Q, a[], *y, dyda[];
int  na;
bool *error;
char *errstr;
{
   float I0, Rg, u;

   if (na != 2) {
      *error  = TRUE;
      sprintf(errstr,
              "Debye: needs two paramater in vector a[] but got %d",na);
      return;
   }
   I0 = a[0];
   Rg = a[1];
   u  = pow(Q*Rg,2.0);

   if (u == 0.0) {
      *error  = TRUE;
      sprintf(errstr,
              "Debye: Rg*Q == 0");
      return;
   }
   if (Rg == 0.0) {
      *error  = TRUE;
      sprintf(errstr,
              "Debye: Rg == 0");
      return;
   }
   *y = I0 * 2.0*(u-1.0+exp(-u))/(u*u);
   dyda[0] = 2.0*(u-1.0+exp(-u))/(u*u);
   dyda[1] = -4.0*I0*(u-2.0+(2.0+u)*exp(-u))/(u*u*Rg);
}


/*#########################################################################*/
/*#                                                                       #*/
/*#########################################################################*/
void GuinierAlpha(Q,a,y,dyda,na,error,errstr)
float Q, a[], *y, dyda[];
int  na;
bool *error;
char *errstr;
{
   float I0, alpha, Rg2;

   if (na != 2) {
      *error  = TRUE;
      sprintf(errstr,
              "Guinier: needs two paramater in vector a[] but got %d",na);
      return;
   }
   I0  = a[0];
   Rg2 = a[1];
   alpha = a[2];

   if (alpha == 0) {
      *y = I0 * exp(-Rg2*Q*Q/3.0);
      dyda[0] = exp(-Rg2*Q*Q/3.0);
      dyda[1] = -(*y) / 3.0 * Q*Q;
   } else {
	   *y = I0*M_PI*alpha*pow(Q,-alpha)*exp(-Rg2*Q*Q/(3.0-alpha));
      dyda[0] = M_PI*alpha*pow(Q,-alpha)*exp(-Rg2*Q*Q/(3.0-alpha));
      dyda[1] = -(*y) / (3.0-alpha) * Q*Q;
   }
}

/*#########################################################################*/
/*#                                                                       #*/
/*#########################################################################*/
void Zimm(Q,a,y,dyda,na,error,errstr)
float Q, a[], *y, dyda[];
int   na;
bool  *error;
char  *errstr;
{
   float I0, Rg2;

   if (na != 2) {
      *error  = TRUE;
      sprintf(errstr,
              "Zimm: needs two paramater in vector a[] but got %d",na);
      return;
   }
   I0  = a[0];
   Rg2 = a[1];

   if ((1.0+Rg2*Q*Q/3.0) == 0.0) {
      *error  = TRUE;
      sprintf(errstr,
              "Zimm: cannot evaluate function because (1+Rg2*Q*Q/3) == 0.0");
      return;
   }

   *y = I0/(1.0+Rg2*Q*Q/3.0);
   dyda[0] = 1.0/(1.0+Rg2*Q*Q/3.0);
   dyda[1] = -3.0*I0*Q*Q/pow(3.0+Rg2*Q*Q,2.0);
}

/*#########################################################################*/
/*#                                                                       #*/
/*#########################################################################*/
void Zimm2(Q,a,y,dyda,na,error,errstr)
float Q, a[], *y, dyda[];
int   na;
bool  *error;
char  *errstr;
{
   float I0, Rg;

   if (na != 2) {
      *error  = TRUE;
      sprintf(errstr,
              "Zimm2: needs two paramater in vector a[] but got %d",na);
      return;
   }
   I0 = a[0];
   Rg = a[1];

   if ((1.0+pow(Rg*Q,2.0)/3.0) == 0.0) {
      *error  = TRUE;
      sprintf(errstr,
              "Zimm2: cannot evaluate function because (1+Rg^2*Q^2/3) == 0.0");
      return;
   }

   *y = I0/(1.0+pow(Rg*Q,2.0)/3.0);
   dyda[0] = 1.0/(1.0+pow(Rg*Q,2)/3.0);
   dyda[1] = -6.0*I0*Rg*Q*Q/pow(3.0+pow(Rg*Q,2.0),2.0);
}


/*#########################################################################*/
/*#                                                                       #*/
/*#########################################################################*/
void OrnsteinZernicke(Q,a,y,dyda,na,error,errstr)
float Q, a[], *y, dyda[];
int   na;
bool  *error;
char  *errstr;
{
   float I0, xi2;

   if (na != 2) {
      *error  = TRUE;
      sprintf(errstr,
              "OrnsteinZernicke: needs two paramater in vector a[] but got %d",
              na);
      return;
   }
   I0  = a[0];
   xi2 = a[1];

   if ((1.0+xi2*Q*Q) == 0.0) {
      *error  = TRUE;
      sprintf(errstr,
      "OrnsteinZernicke: cannot evaluate function because (1+xi2*Q^2) == 0.0");
      return;
   }

   *y = I0/(1.0+xi2*Q*Q);
   dyda[0] = 1.0/(1.0+xi2*Q*Q);
   dyda[1] = -I0*Q*Q/pow(1.0+xi2*Q*Q,2);
}

/*#########################################################################*/
/*#                                                                       #*/
/*#########################################################################*/
void OrnsteinZernicke2(Q,a,y,dyda,na,error,errstr)
float Q, a[], *y, dyda[];
int   na;
bool  *error;
char  *errstr;
{
   float I0, xi;

   if (na != 2) {
      *error  = TRUE;
      sprintf(errstr,
             "OrnsteinZernicke2: needs two paramater in vector a[] but got %d",
             na);
      return;
   }
   I0 = a[0];
   xi = a[1];

   if ((1.0+pow(xi*Q,2)) == 0.0) {
      *error  = TRUE;
      sprintf(errstr,
              "OrnsteinZernicke2: cannot evaluate function because (1+xi^2*Q^2) == 0.0");
      return;
   }

   *y = I0/(1.0+pow(xi*Q,2));
   dyda[0] = 1.0/(1.0+pow(xi*Q,2));
   dyda[1] = -2.0*I0*xi*Q*Q/pow(1.0+pow(xi*Q,2),2);
}



/*#########################################################################*/
/*#                                                                       #*/
/*# save_EP --                                                            #*/
/*#                                                                       #*/
/*#      This function saves the value of the C-structure ExtrapolPar     #*/
/*#      into the tcl variable EP                                         #*/
/*#                                                                       #*/
/*# Results: TCL_OK, TCL_ERROR                                            #*/
/*#                                                                       #*/
/*#########################################################################*/
int save_EP(clientData,interp,EP)
    ClientData         clientData;
    Tcl_Interp         *interp;
    struct extrapolPar EP;

{
    int              i;
    char             sBuffer[256];
    Tcl_DString      DsBuffer;
/*
 * saves the value EP.order into the tcl-variable EP(order)
 */
    sprintf(sBuffer,"%s",EP.order);
    Tcl_SetVar2(interp,EP.name,"order",sBuffer,0);

/*
 * saves the value EP.first in tcl-variable EP(first)
 */
    sprintf(sBuffer,"%d",EP.first);
    Tcl_SetVar2(interp,EP.name,"first",sBuffer,0);

/*
 * saves the value EP.npoints in tcl-variable EP(npoints)
 */
    sprintf(sBuffer,"%d",EP.npoints);
    Tcl_SetVar2(interp,EP.name,"npoints",sBuffer,0);

/*
 * saves the value EP.Guinierfirst in tcl-variable EP(Guinierfirst)
 */
    sprintf(sBuffer,"%d",EP.Guinierfirst);
    Tcl_SetVar2(interp,EP.name,"Guinierfirst",sBuffer,0);

/*
 * saves the value EP.Guiniernpoints in tcl-variable EP(Guiniernpoints)
 */
    sprintf(sBuffer,"%d",EP.Guiniernpoints);
    Tcl_SetVar2(interp,EP.name,"Guiniernpoints",sBuffer,0);

/*
 * saves the value EP.Porodfirst in tcl-variable EP(Porodfirst)
 */
    sprintf(sBuffer,"%d",EP.Porodfirst);
    Tcl_SetVar2(interp,EP.name,"Porodfirst",sBuffer,0);

/*
 * saves the value EP.Porodnpoints in tcl-variable EP(Porodnpoints)
 */
    sprintf(sBuffer,"%d",EP.Porodnpoints);
    Tcl_SetVar2(interp,EP.name,"Porodnpoints",sBuffer,0);

/*
 * saves the value EP.PorodD in tcl-variable EP(PorodD)
 */
    float_to_string(sBuffer,EP.PorodD);
    Tcl_SetVar2(interp,EP.name,"PorodD",sBuffer,0);

/*
 * saves the value EP.DPorodD in tcl-variable EP(DPorodD)
 */
    float_to_string(sBuffer,EP.DPorodD);
    Tcl_SetVar2(interp,EP.name,"DPorodD",sBuffer,0);

/*
 * save parameter EP.PorodDFit into tcl-variable EP(porodDfit)
 */
				if (EP.PorodDFit) {
       Tcl_SetVar2(interp,EP.name,"porodDfit","yes",0);
				} else {
       Tcl_SetVar2(interp,EP.name,"porodDfit","no",0);
				}

/*
 * saves the value EP.c0 in tcl-variable EP(c0)
 */
    float_to_string(sBuffer,EP.c0);
    Tcl_SetVar2(interp,EP.name,"c0",sBuffer,0);

/*
 * saves the value EP.Dc0 in tcl-variable EP(Dc0)
 */
    float_to_string(sBuffer,EP.Dc0);
    Tcl_SetVar2(interp,EP.name,"Dc0",sBuffer,0);

/*
 * saves the value EP.c4 in tcl-variable EP(c4)
 */
    float_to_string(sBuffer,EP.c4);
    Tcl_SetVar2(interp,EP.name,"c4",sBuffer,0);

/*
 * saves the value EP.Dc4 in tcl-variable EP(Dc4)
 */
    float_to_string(sBuffer,EP.Dc4);
    Tcl_SetVar2(interp,EP.name,"Dc4",sBuffer,0);

/*
 * saves the value EP.I0 in tcl-variable EP(I0)
 */
    float_to_string(sBuffer,EP.I0);
    Tcl_SetVar2(interp,EP.name,"I0",sBuffer,0);

/*
 * saves the value EP.DI0 in tcl-variable EP(DI0)
 */
    float_to_string(sBuffer,EP.DI0);
    Tcl_SetVar2(interp,EP.name,"DI0",sBuffer,0);

/*
 * saves the value EP.RG in tcl-variable EP(RG)
 */
    float_to_string(sBuffer,EP.RG);
    Tcl_SetVar2(interp,EP.name,"RG",sBuffer,0);

/*
 * saves the value EP.DRG in tcl-variable EP(DRG)
 */
    float_to_string(sBuffer,EP.DRG);
    Tcl_SetVar2(interp,EP.name,"DRG",sBuffer,0);

/*
 * saves the value EP.R2G in tcl-variable EP(R2G)
 */
    float_to_string(sBuffer,EP.R2G);
    Tcl_SetVar2(interp,EP.name,"R2G",sBuffer,0);

/*
 * saves the value EP.DR2G in tcl-variable EP(DR2G)
 */
    float_to_string(sBuffer,EP.DR2G);
    Tcl_SetVar2(interp,EP.name,"DR2G",sBuffer,0);

/*
 * saves the value EP.li in tcl-variable EP(li)
 */
    float_to_string(sBuffer,EP.li);
    Tcl_SetVar2(interp,EP.name,"li",sBuffer,0);

/*
 * saves the value EP.Dli in tcl-variable EP(Dli)
 */
    float_to_string(sBuffer,EP.Dli);
    Tcl_SetVar2(interp,EP.name,"Dli",sBuffer,0);

/*
 * saves the value EP.lc in tcl-variable EP(lc)
 */
    float_to_string(sBuffer,EP.lc);
    Tcl_SetVar2(interp,EP.name,"lc",sBuffer,0);

/*
 * saves the value EP.Dlc in tcl-variable EP(Dlc)
 */
    float_to_string(sBuffer,EP.Dlc);
    Tcl_SetVar2(interp,EP.name,"Dlc",sBuffer,0);

/*
 * saves the value EP.Ac in tcl-variable EP(Ac)
 */
    float_to_string(sBuffer,EP.Ac);
    Tcl_SetVar2(interp,EP.name,"Ac",sBuffer,0);

/*
 * saves the value EP.DAc in tcl-variable EP(DAc)
 */
    float_to_string(sBuffer,EP.DAc);
    Tcl_SetVar2(interp,EP.name,"DAc",sBuffer,0);

/*
 * saves the value EP.Invariant in tcl-variable EP(Invariant)
 */
    float_to_string(sBuffer,EP.Invariant);
    Tcl_SetVar2(interp,EP.name,"Invariant",sBuffer,0);

/*
 * saves the value EP.DInvariant in tcl-variable EP(DInvariant)
 */
    float_to_string(sBuffer,EP.DInvariant);
    Tcl_SetVar2(interp,EP.name,"DInvariant",sBuffer,0);

/*
 * saves the value EP.Qexp in tcl-variable EP(Qexp)
 */
    float_to_string(sBuffer,EP.Qexp);
    Tcl_SetVar2(interp,EP.name,"Qexp",sBuffer,0);

/*
 * saves the value EP.DQexp in tcl-variable EP(DQexp)
 */
    float_to_string(sBuffer,EP.DQexp);
    Tcl_SetVar2(interp,EP.name,"DQexp",sBuffer,0);

/*
 * saves the value EP.Qc4 in tcl-variable EP(Qc4)
 */
    float_to_string(sBuffer,EP.Qc4);
    Tcl_SetVar2(interp,EP.name,"Qc4",sBuffer,0);

/*
 * saves the value EP.DQc4 in tcl-variable EP(DQc4)
 */
    float_to_string(sBuffer,EP.DQc4);
    Tcl_SetVar2(interp,EP.name,"DQc4",sBuffer,0);

/*
 * saves the value EP.Qq0 in tcl-variable EP(Qq0)
 */
    float_to_string(sBuffer,EP.Qq0);
    Tcl_SetVar2(interp,EP.name,"Qq0",sBuffer,0);

/*
 * saves the value EP.DQq0 in tcl-variable EP(DQq0)
 */
    float_to_string(sBuffer,EP.DQq0);
    Tcl_SetVar2(interp,EP.name,"DQq0",sBuffer,0);
/*
 * saves the value EP.intI in tcl-variable EP(intI)
 */
    float_to_string(sBuffer,EP.intI);
    Tcl_SetVar2(interp,EP.name,"intI",sBuffer,0);

/*
 * saves the value EP.DintI in tcl-variable EP(DintI)
 */
    float_to_string(sBuffer,EP.DintI);
    Tcl_SetVar2(interp,EP.name,"DintI",sBuffer,0);

/*
 * saves the value EP.intIexp in tcl-variable EP(intIexp)
 */
    float_to_string(sBuffer,EP.intIexp);
    Tcl_SetVar2(interp,EP.name,"intIexp",sBuffer,0);

/*
 * saves the value EP.DintIexp in tcl-variable EP(DintIexp)
 */
    float_to_string(sBuffer,EP.DintIexp);
    Tcl_SetVar2(interp,EP.name,"DintIexp",sBuffer,0);

/*
 * saves the value EP.intIc4 in tcl-variable EP(intIc4)
 */
    float_to_string(sBuffer,EP.intIc4);
    Tcl_SetVar2(interp,EP.name,"intIc4",sBuffer,0);

/*
 * saves the value EP.DintIc4 in tcl-variable EP(DintIc4)
 */
    float_to_string(sBuffer,EP.DintIc4);
    Tcl_SetVar2(interp,EP.name,"DintIc4",sBuffer,0);

/*
 * saves the value EP.intIq0 in tcl-variable EP(intIq0)
 */
    float_to_string(sBuffer,EP.intIq0);
    Tcl_SetVar2(interp,EP.name,"intIq0",sBuffer,0);

/*
 * saves the value EP.DintIq0 in tcl-variable EP(DintIq0)
 */
    float_to_string(sBuffer,EP.DintIq0);
    Tcl_SetVar2(interp,EP.name,"DintIq0",sBuffer,0);
/*
 * saves the value EP.iI in tcl-variable EP(iI)
 */
    float_to_string(sBuffer,EP.iI);
    Tcl_SetVar2(interp,EP.name,"iI",sBuffer,0);

/*
 * saves the value EP.DiI in tcl-variable EP(DiI)
 */
    float_to_string(sBuffer,EP.DiI);
    Tcl_SetVar2(interp,EP.name,"DiI",sBuffer,0);

/*
 * saves the value EP.iIexp in tcl-variable EP(iIexp)
 */
    float_to_string(sBuffer,EP.iIexp);
    Tcl_SetVar2(interp,EP.name,"iIexp",sBuffer,0);

/*
 * saves the value EP.DiIexp in tcl-variable EP(DiIexp)
 */
    float_to_string(sBuffer,EP.DiIexp);
    Tcl_SetVar2(interp,EP.name,"DiIexp",sBuffer,0);

/*
 * saves the value EP.iIc4 in tcl-variable EP(iIc4)
 */
    float_to_string(sBuffer,EP.iIc4);
    Tcl_SetVar2(interp,EP.name,"iIc4",sBuffer,0);

/*
 * saves the value EP.DiIc4 in tcl-variable EP(DiIc4)
 */
    float_to_string(sBuffer,EP.DiIc4);
    Tcl_SetVar2(interp,EP.name,"DiIc4",sBuffer,0);

/*
 * saves the value EP.iIq0 in tcl-variable EP(iIq0)
 */
    float_to_string(sBuffer,EP.iIq0);
    Tcl_SetVar2(interp,EP.name,"iIq0",sBuffer,0);

/*
 * saves the value EP.DiIq0 in tcl-variable EP(DiIq0)
 */
    float_to_string(sBuffer,EP.DiIq0);
    Tcl_SetVar2(interp,EP.name,"DiIq0",sBuffer,0);

/*
 * saves the value EP.VP in tcl-variable EP(VP)
 */
    float_to_string(sBuffer,EP.VP);
    Tcl_SetVar2(interp,EP.name,"VP",sBuffer,0);

/*
 * saves the value EP.DVP in tcl-variable EP(DVP)
 */
    float_to_string(sBuffer,EP.DVP);
    Tcl_SetVar2(interp,EP.name,"DVP",sBuffer,0);

/*
 * saves the value EP.RP in tcl-variable EP(RP)
 */
    float_to_string(sBuffer,EP.RP);
    Tcl_SetVar2(interp,EP.name,"RP",sBuffer,0);

/*
 * saves the value EP.DRP in tcl-variable EP(DRP)
 */
    float_to_string(sBuffer,EP.DRP);
    Tcl_SetVar2(interp,EP.name,"DRP",sBuffer,0);

/*
 * saves the value EP.S_V in tcl-variable EP(S/V)
 */
    float_to_string(sBuffer,EP.S_V);
    Tcl_SetVar2(interp,EP.name,"S/V",sBuffer,0);

/*
 * saves the value EP.DS_V in tcl-variable EP(DS/V)
 */
    float_to_string(sBuffer,EP.DS_V);
    Tcl_SetVar2(interp,EP.name,"DS/V",sBuffer,0);

/*
 * saves the value EP.chisq in tcl-variable EP(chisq)
 */
    float_to_string(sBuffer,EP.chisq);
    Tcl_SetVar2(interp,EP.name,"chisq",sBuffer,0);

/*
 * saves the value EP.I0chisq in tcl-variable EP(I0chisq)
 */
    float_to_string(sBuffer,EP.I0chisq);
    Tcl_SetVar2(interp,EP.name,"I0chisq",sBuffer,0);

/*
 * saves the value EP.Porodchisq in tcl-variable EP(Porodchisq)
 */
    float_to_string(sBuffer,EP.Porodchisq);
    Tcl_SetVar2(interp,EP.name,"Porodchisq",sBuffer,0);

/*
 * saves the value EP.Qmin in tcl-variable EP(Qmin)
 */
    float_to_string(sBuffer,EP.Qmin);
    Tcl_SetVar2(interp,EP.name,"Qmin",sBuffer,0);

/*
 * saves the value EP.Qmax in tcl-variable EP(Qmax)
 */
    float_to_string(sBuffer,EP.Qmax);
    Tcl_SetVar2(interp,EP.name,"Qmax",sBuffer,0);

/*
 * saves the value EP.ndata in tcl-variable EP(ndata)
 */
    sprintf(sBuffer,"%d",EP.ndata);
    Tcl_SetVar2(interp,EP.name,"ndata",sBuffer,0);

/*
 * saves array EP.a[4] as a list in the tcl-variable EP(a)
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<4;i++) {
        float_to_string(sBuffer,EP.a[i]);
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
    }
    Tcl_SetVar2(interp,EP.name,"a",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save array EP.err[4] as a list in the tcl-variable EP(err)
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<4;i++) {
        float_to_string(sBuffer,EP.err[i]);
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
    }
    Tcl_SetVar2(interp,EP.name,"err",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save array EP.active[4] as a list in the tcl-variable EP(active)
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<4;i++) {
        sprintf(sBuffer,"%d",EP.active[i]);
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
    }
    Tcl_SetVar2(interp,EP.name,"active",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter EP.errortype into tcl-variable EP(errortype)
 */
    sprintf(sBuffer,"%s",EP.errortype);
    Tcl_SetVar2(interp,EP.name,"errortype",sBuffer,0);

/*
 * save parameter EP.typestr into tcl-variable EP(typestr)
 */
    sprintf(sBuffer,"%s",EP.typestr);
    Tcl_SetVar2(interp,EP.name,"typestr",sBuffer,0);

/*
 * save EP.aname[4] as a list into the tcl-variable EP(aname)
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<4;i++) {
        sprintf(sBuffer,"%s",EP.aname[i]);
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
    }
    Tcl_SetVar2(interp,EP.name,"aname",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

    return TCL_OK;
}

int assign_algoritmns(EM_param_t * EM) {
#define MAX_DEF_L 9
#define MAX_OPT_LAGRANGE_METHOD 8
#define MAX_DR_METHOD 6
#define MAX_LSS_METHOD 2
    const char * defL[MAX_DEF_L];
    const char * optLagrangeMethod[MAX_OPT_LAGRANGE_METHOD];
    const char * DRmeth[MAX_DR_METHOD];
    const char * LSSmeth[MAX_LSS_METHOD];
    int i,eq;
    if (!EM) return 0;

    LSSmeth[0] = "LLS";
    LSSmeth[1] = "NNLLS";

    defL[0] = "Idendity";
    defL[1] = "first deriv. (eps_b)";
    defL[2] = "first deriv. (eps_e)";
    defL[3] = "first deriv.";
    defL[4] = "second deriv.";
    defL[5] = "second deriv. (D-D)";
    defL[6] = "second deriv. (N-N)";
    defL[7] = "second deriv. (D-N)";
    defL[8] = "second deriv. (N-D)";

    optLagrangeMethod[0] = "L-corner (o)";
    optLagrangeMethod[1] = "L-corner2";
    optLagrangeMethod[2] = "GCV";
    optLagrangeMethod[3] = "red. chi2";
    optLagrangeMethod[4] = "manual";
    optLagrangeMethod[5] = "L-corner (l)";
    optLagrangeMethod[6] = "L-corner (w)";
    optLagrangeMethod[7] = "L-corner (w+o)";

    DRmeth[0] = "MuCh";
    DRmeth[1] = "EM (smoothing)";
    DRmeth[2] = "SDM";
    DRmeth[3] = "lin Reg";
    DRmeth[4] = "EM (ME constant prior)";
    DRmeth[5] = "EM (ME adaptive prior)";

    i=0;
    eq=-1;
    while (i<MAX_LSS_METHOD && eq != 0) {
        eq = strcmp(EM->LLS_type,LSSmeth[i]);
        i++;
    }
    switch (i-1) {
        case 0 :
            EM->LLSmethod= LLS;
            break;
        case 1 :
            EM->LLSmethod= NNLLS;
            break;
        default:
            EM->LLSmethod=LLS;
    }

    i=0;
    eq=-1;
    while (i<MAX_DEF_L && eq != 0) {
        eq = strcmp(EM->Lmatrix,defL[i]);
        i++;
    }
    switch (i-1) {
        case 0 :
            EM->defL=Idendity;
            break;
        case 1 :
            EM->defL=first_deriv_eps_b;
            break;
        case 2 :
            EM->defL=first_deriv_eps_e;
            break;
        case 3 :
            EM->defL=first_deriv;
            break;
        case 4 :
            EM->defL=second_deriv;
            break;
        case 5 :
            EM->defL=second_deriv_DD;
            break;
        case 6 :
            EM->defL=second_deriv_NN;
            break;
        case 7 :
            EM->defL=second_deriv_DN;
            break;
        case 8 :
            EM->defL=second_deriv_ND;
            break;
        default :
            EM->defL=second_deriv_DD;
            sasfit_out("L matrix definition not found: %s. Using ->second deriv. (D-D)<- instead.\n", EM->Lmatrix);
            sasfit_err("L matrix definition not found: %s. Using ->second deriv. (D-D)<- instead.\n", EM->Lmatrix);
            break;
    }
sasfit_out("%s:%d - EM->defL:%d\n",EM->Lmatrix,i-1,EM->defL);
    i=0;
    eq=-1;
    while (i<MAX_OPT_LAGRANGE_METHOD && eq != 0) {
        eq = strcmp(EM->opt_Lagrange_scheme,optLagrangeMethod[i]);
        i++;
    }
    switch (i-1) {
        case 0 :
            EM->optLagrange_method=Lcorner_o;
            break;
        case 1 :
            EM->optLagrange_method=Lcorner2;
            break;
        case 2 :
            EM->optLagrange_method=GCV;
            break;
        case 3 :
            EM->optLagrange_method=redchi2;
            break;
        case 4 :
            EM->optLagrange_method=manual;
            break;
        case 5 :
            EM->optLagrange_method=Lcorner_l;
            break;
        case 6 :
            EM->optLagrange_method=Lcorner_w;
            break;
        case 7 :
            EM->optLagrange_method=Lcorner_wo;
            break;
        default :
            EM->optLagrange_method=manual;
            sasfit_out("Algorithm for finding optimum Lagrange parameter not found: %s\n", EM->optLagrange_method);
            sasfit_err("Algorithm for finding optimum Lagrange parameter not found: %s\n", EM->optLagrange_method);
            break;
    }
sasfit_out("%s:%d - EM->optLagrange_method:%d\n",EM->opt_Lagrange_scheme,i-1,EM->optLagrange_method);
    i=0;
    eq=-1;
    while (i<MAX_DR_METHOD && eq != 0) {
        eq = strcmp(EM->FIinv,DRmeth[i]);
        i++;
    }
    switch (i-1) {
        case 0 :
            EM->DR_algorithm=MuCh;
            break;
        case 1 :
            EM->DR_algorithm=SDM;
            break;
        case 2 :
            EM->DR_algorithm=lin_Reg;
            break;
        case 3 :
            EM->DR_algorithm=EM_smoothing;
            break;
        case 4 :
            EM->DR_algorithm=EM_constant_prior;
            break;
        case 5 :
            EM->DR_algorithm=EM_adaptive_prior;
            break;
        default :
            EM->DR_algorithm=EM_constant_prior;
            sasfit_out("Algorithm for solving Fredholm integral not found: %s\n", EM->FIinv);
            sasfit_err("Algorithm for solving Fredholm integral not found: %s\n", EM->FIinv);
            break;
    }
    sasfit_out("EM->DR_algorithm:%d\nEM->defL:%d\nEM->optLagrange_method:%d\n",EM->DR_algorithm,EM->defL, EM->optLagrange_method);
    return 1;
}

int
assign_root_Algorithm_EM(const char * token, EM_param_t * EM)
{
#define MAXROOTALGORITHMS 24
    const char * RootAlgorithms[MAXROOTALGORITHMS];
    int i,eq;
    if (!token || !EM) return 0;
    RootAlgorithms[0] = "Picard iteration";
    RootAlgorithms[1] = "Mann iteration";
    RootAlgorithms[2] = "Ishikawa iteration";
    RootAlgorithms[3] = "Noor iteration";
    RootAlgorithms[4] = "S iteration";
    RootAlgorithms[5] = "SP iteration";
    RootAlgorithms[6] = "CR iteration";
    RootAlgorithms[7] = "Picard-S iteration";
    RootAlgorithms[8] = "PMH iteration";
    RootAlgorithms[9] = "Mann II iteration";
    RootAlgorithms[10] = "Krasnoselskij iteration";
    RootAlgorithms[11] = "S* iteration";
    RootAlgorithms[12] = "Steffensen iteration";
    RootAlgorithms[13] = "Anderson mixing";
    RootAlgorithms[14] = "GMRES" ;
    RootAlgorithms[15] = "Bi-CGStab" ;
    RootAlgorithms[16] = "TFQMR" ;
    RootAlgorithms[17] = "FGMRES";
    RootAlgorithms[18] = "KINSOL_FP";
    RootAlgorithms[19] = "Biggs_Andrews";
    RootAlgorithms[MAXROOTALGORITHMS-1] = "dNewton";
    RootAlgorithms[MAXROOTALGORITHMS-2] = "Hybrid";
    RootAlgorithms[MAXROOTALGORITHMS-3] = "Hybrids (int. sc.)";
    RootAlgorithms[MAXROOTALGORITHMS-4] = "Broyden";

    i=0;
    eq=-1;
    while (i<MAXROOTALGORITHMS && eq != 0) {
        eq = strcmp(token,RootAlgorithms[i]);
        i++;
    }
//    if (EM->PrintProgress) sasfit_out("%s %d\n",token,i-1);

    switch (i-1) {
        case 0 :
            EM->root_algorithm=Picard_iteration;
            break;
        case 1 :
            EM->root_algorithm=Mann_iteration;
            break;
        case 2 :
            EM->root_algorithm=Ishikawa_iteration;
            break;
        case 3 :
            EM->root_algorithm=Noor_iteration;
            break;
        case 4 :
            EM->root_algorithm=S_iteration;
            break;
        case 5 :
            EM->root_algorithm=SP_iteration;
            break;
        case 6 :
            EM->root_algorithm=CR_iteration;
            break;
        case 7 :
            EM->root_algorithm=PicardS_iteration;
            break;
        case 8 :
            EM->root_algorithm=PMH_iteration;
            break;
        case 9 :
            EM->root_algorithm=MannII_iteration;
            break;
        case 10 :
            EM->root_algorithm=Krasnoselskij_iteration;
            break;
        case 11 :
            EM->root_algorithm=Sstar_iteration;
            break;
        case 12 :
            EM->root_algorithm=Steffensen2_iteration;
            break;
        case 13 :
            EM->root_algorithm=AndersonAcc;
            break;
        case 14 :
            EM->root_algorithm=GMRES;
            break;
        case 15 :
            EM->root_algorithm=BiCGSTAB;
            break;
        case 16 :
            EM->root_algorithm=TFQMR;
            break;
        case 17 :
            EM->root_algorithm=FGMRES;
            break;
        case 18 :
            EM->root_algorithm=KINSOLFP;
            break;
        case 19 :
            EM->root_algorithm=BIGGS_ANDREWS;
            break;
        case MAXROOTALGORITHMS-1 :
            EM->root_algorithm=dNewton;
            break;
        case MAXROOTALGORITHMS-2 :
            EM->root_algorithm=Hybrid;
            break;
        case MAXROOTALGORITHMS-3 :
            EM->root_algorithm=Hybrids;
            break;
        case MAXROOTALGORITHMS-4 :
            EM->root_algorithm=Broyden;
            break;
        default :
            EM->root_algorithm=S_iteration;
            sasfit_out("Root finding Algorithm not found: %s. Using S iteration instead.\n", token);
            sasfit_err("Root finding Algorithm not found: %s. Using S iteration instead.\n", token);
            break;
    }
    if (i<=MAXROOTALGORITHMS) return 1;
    return 0;
}
/*#########################################################################*/
/*#                                                                       #*/
/*# get_EM --                                                             #*/
/*#                                                                       #*/
/*#      This function reads the value of the tcl variable EMOptions      #*/
/*#      in the C-structure EM                                            #*/
/*#                                                                       #*/
/*# Results: TCL_OK, TCL_ERROR                                            #*/
/*#                                                                       #*/
/*#########################################################################*/
int get_EM(clientData,interp,EM)
    ClientData         clientData;
    Tcl_Interp         *interp;
    EM_param_t *EM;

{
    int varint, status;
    scalar varscalar;
    const char *varstr;
 /*
 * read the nR
 */
    if (TCL_ERROR == Tcl_GetInt(interp,
                             Tcl_GetVar2(interp,"EMOptions","nR",0),
                             &varint) ) {
       sasfit_err("could not read number or R values\n");
       return TCL_ERROR;
    }
    (*EM).nR = varint;
/*
 * read the error type
 */
    if (TCL_ERROR == Tcl_GetInt(interp,
                             Tcl_GetVar2(interp,"EMOptions","maxit",0),
                             &varint) ) {
       sasfit_err("could not read number or maxit\n");
       return TCL_ERROR;
    }
    (*EM).maxit = varint;

/*
 * read the number Rmax
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,"EMOptions","Rmax",0),
                             &varscalar) ) {
       sasfit_err("could not read Rmax\n");
       return TCL_ERROR;
    }
    (*EM).Rmax = varscalar;

/*
 * read the number dim
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,"EMOptions","dim",0),
                             &varscalar) ) {
       sasfit_err("could not read dim\n");
       return TCL_ERROR;
    }
    (*EM).dim = varscalar;

/*
 * read the number dim
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,"EMOptions","lambda",0),
                             &varscalar) ) {
       sasfit_err("could not read lambda\n");
       return TCL_ERROR;
    }
    (*EM).lambda = varscalar;

/*
 * read the number eps
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,"EMOptions","eps",0),
                             &varscalar) ) {
       sasfit_err("could not read eps\n");
       return TCL_ERROR;
    }
    (*EM).eps = varscalar;

/*
 * read the number chi2
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,"EMOptions","chi2",0),
                             &varscalar) ) {
       sasfit_err("could not read chi2\n");
       return TCL_ERROR;
    }
    (*EM).chi2 = varscalar;


/*
 * read the number smooth
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,"EMOptions","smooth",0),
                             &varscalar) ) {
       sasfit_err("could not read smooth\n");
       return TCL_ERROR;
    }
    (*EM).smooth = varscalar;

    /*
 * read the string error_weight
 */
    varstr = Tcl_GetVar2(interp,"EMOptions","error_weight",0);
    if (NULL == varstr) {
       sasfit_err("could not read error_weight\n");
       return TCL_ERROR;
    }
    (*EM).em_weight = 0;
    if (strcmp(varstr,"on")==0) (*EM).em_weight = 1;
    sasfit_out("em_weight:%d\n",(*EM).em_weight);


/*
 * read the string smooth_type
 */
    varstr = Tcl_GetVar2(interp,"EMOptions","smooth_type",0);
    if (NULL == varstr) {
       sasfit_err("could not read smooth_type\n");
       return TCL_ERROR;
    }
    strcpy((*EM).smooth_type,varstr);

/*
 * read the string IterationScheme
 */
    varstr = Tcl_GetVar2(interp,"EMOptions","IterationScheme",0);
    if (NULL == varstr) {
       sasfit_err("could not read IterationScheme\n");
       return TCL_ERROR;
    }
    strcpy((*EM).iteration_scheme,varstr);
    status = assign_root_Algorithm_EM(Tcl_GetStringFromObj(sasfit_tcl_get_obj(interp, "EMOptions", "IterationScheme"), 0),EM);
    if (status == 0) {
            sasfit_err("Unknown Root finding Algorithm\n");
            return TCL_ERROR;
    }
/*
 * read the string LMatrix
 */
    varstr = Tcl_GetVar2(interp,"EMOptions","LMatrix",0);
    if (NULL == varstr) {
       sasfit_err("could not read LMatrix\n");
       return TCL_ERROR;
    }
    strcpy((*EM).Lmatrix,varstr);
/*
 * read the string LSS_type
 */
    varstr = Tcl_GetVar2(interp,"EMOptions","LLSmethod",0);
    if (NULL == varstr) {
       sasfit_err("could not read LLSmethod\n");
       return TCL_ERROR;
    }
    strcpy((*EM).LLS_type,varstr);
 /*
 * read the nLagrange
 */
    if (TCL_ERROR == Tcl_GetInt(interp,
                             Tcl_GetVar2(interp,"EMOptions","nLagrange",0),
                             &varint) ) {
       sasfit_err("could not read number or Lagrange values\n");
       return TCL_ERROR;
    }
    (*EM).nLagrange = varint;

/*
 * read the string maxLagrange
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,"EMOptions","maxLagrange",0),
                             &varscalar) ) {
       sasfit_err("could not read maxLagrange\n");
       return TCL_ERROR;
    }
    (*EM).maxLagrange = varscalar;

/*
 * read the string minLagrange
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,"EMOptions","minLagrange",0),
                             &varscalar) ) {
       sasfit_err("could not read minLagrange\n");
       return TCL_ERROR;
    }
    (*EM).minLagrange = varscalar;
/*
 * read the string maxslope
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,"EMOptions","maxslope",0),
                             &varscalar) ) {
       sasfit_err("could not read maxslope\n");
       return TCL_ERROR;
    }
    (*EM).maxslope = varscalar;

/*
 * read the string optimumLagrange
 */
    varstr = Tcl_GetVar2(interp,"EMOptions","optimumLagrange",0);
    if (NULL == varstr) {
       sasfit_err("could not read optimumLagrange\n");
       return TCL_ERROR;
    }
    strcpy((*EM).opt_Lagrange_scheme,varstr);

/*
 * read the string overrelaxation
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,"EMOptions","overrelaxation",0),
                             &varscalar) ) {
       sasfit_err("could not read overrelaxation\n");
       return TCL_ERROR;
    }
    (*EM).overrelaxation = varscalar;

/*
 * read the string maxKrylov
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,"EMOptions","maxKrylov",0),
                             &varscalar) ) {
       sasfit_err("could not read maxKrylov\n");
       return TCL_ERROR;
    }
    (*EM).maxKrylov = lround(varscalar);


/*
 * read the string Fredholm integral inversion "method"
 */
    varstr = Tcl_GetVar2(interp,"EMOptions","method",0);
    if (NULL == varstr) {
       sasfit_err("could not read method\n");
       return TCL_ERROR;
    }
    strcpy((*EM).FIinv,varstr);

    status = assign_algoritmns(EM);
    if (status == 0) {
            sasfit_err("Unknown  Algorithm\n");
            return TCL_ERROR;
    }

/*
 * read the string spacing
 */
    varstr = Tcl_GetVar2(interp,"EMOptions","spacing",0);
    if (NULL == varstr) {
       sasfit_err("could not read spacing\n");
       return TCL_ERROR;
    }
    strcpy((*EM).spacing,varstr);

/*
 * read the string seed
 */
    varstr = Tcl_GetVar2(interp,"EMOptions","seed",0);
    if (NULL == varstr) {
       sasfit_err("could not read seed\n");
       return TCL_ERROR;
    }
    strcpy((*EM).seed,varstr);
return TCL_OK;
}

/*#########################################################################*/
/*#                                                                       #*/
/*# get_EP --                                                             #*/
/*#                                                                       #*/
/*#      This function reads the value of the tcl variable EP             #*/
/*#      in the C-structure ExtrapolPar                                   #*/
/*#                                                                       #*/
/*# Results: TCL_OK, TCL_ERROR                                            #*/
/*#                                                                       #*/
/*#########################################################################*/
int get_EP(clientData,interp,argv,EP,Q,IQ,DIQ)
    ClientData         clientData;
    Tcl_Interp         *interp;
    char               **argv;
    struct extrapolPar *EP;
    float              **Q, **IQ, **DIQ;

{
    int    i,j;
    int    Splitcode, Splitargc, tcode, targc;
    const char   **Splitargv, **targv;
    int    ndata;
    float  *h, *Ih, *DIh, ftmp;
    int    *iwksp;
    float  *wksp;
    int    error;
    char   *xye_data;
    int    first,npoints;
    const char   *Buffer;

/*
 * saves name of the tcl-array into EP.name
 */
    strcpy((*EP).name,argv[1]);

/*
 * read the error type
 */
    if (TCL_ERROR == Tcl_GetInt(interp,
                             Tcl_GetVar2(interp,argv[1],"error",0),
                             &error) ) {
       sasfit_err("could not read error type\n");
       return TCL_ERROR;
    }
    (*EP).error = error;

/*
 * read the string errortype
 */
    Buffer = Tcl_GetVar2(interp,argv[1],"errortype",0);
    if (NULL == Buffer) {
       sasfit_err("could not read errortype\n");
       return TCL_ERROR;
    }
    strcpy((*EP).errortype,Buffer);

/*
 * read the string typestr
 */
    Buffer = Tcl_GetVar2(interp,argv[1],"typestr",0);
    if (NULL == Buffer) {
       sasfit_err("could not read typestr\n");
       return TCL_ERROR;
    }
    strcpy((*EP).typestr,Buffer);
/*
 * read the string typestr
 */
    Buffer = Tcl_GetVar2(interp,argv[1],"I0typestr",0);
    if (NULL == Buffer) {
       sasfit_err("could not read I0typestr\n");
       return TCL_ERROR;
    }
    strcpy((*EP).I0typestr,Buffer);
/*
 * read the string order
 */
    Buffer = Tcl_GetVar2(interp,argv[1],"order",0);
    if (NULL == Buffer) {
       sasfit_err("could not read order\n");
       return TCL_ERROR;
    }
    strcpy((*EP).order,Buffer);

/*
 * read the index of the first point to be fitted
 */
    if (TCL_ERROR == Tcl_GetInt(interp,
                             Tcl_GetVar2(interp,argv[1],"first",0),
                             &first) ) {
       sasfit_err("could not read first\n");
       return TCL_ERROR;
    }
    (*EP).first = first;

/*
 * read scattering curve h, Ih and DIh
 */
    xye_data   = argv[2];
    Splitcode = Tcl_SplitList(interp,xye_data,&Splitargc,&Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_err("scattering data {h Ih DIh} is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != 3) {
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in {h Ih DIh}\n");
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

    switch (error) {
       case 0 : {break;}
       case 1 : { for (i=0;i<ndata;i++) {	if (Ih[i] > 0.0) {
												 DIh[i] = log(Ih[i]);
											} else {
												DIh[i] = -1.0;
												error = 4;
											};
										}

		          break;
				}
       case 2 : { for (i=0;i<ndata;i++) {	if (Ih[i] > 0.0) {
												 DIh[i] = sqrt(Ih[i]);
											} else {
												DIh[i] = -1.0;
												error = 4;
											};
										}
		          break; }
       case 3 : {break;}
      default : { fprintf(stderr,"#get_EP: unknown error_type %d\n",error);
                  for (i=0;i<ndata;i++) DIh[i] = 1.0;
                  error = 3;
				  break;
                }
    }

	if (ndata > 1) {
       iwksp = ivector(0,ndata-1);
       wksp  = dvector(0,ndata-1);
       SASFIT2indexx(ndata,h,iwksp);
       for (i=0;i<ndata;i++) wksp[i] = h[i];
       for (i=0;i<ndata;i++) h[i] = wksp[iwksp[i]-1];
       for (i=0;i<ndata;i++) wksp[i] = Ih[i];
       for (i=0;i<ndata;i++) Ih[i] = wksp[iwksp[i]-1];
       for (i=0;i<ndata;i++) wksp[i] = DIh[i];
       for (i=0;i<ndata;i++) DIh[i] = wksp[iwksp[i]-1];
       free_dvector(wksp,0,ndata-1);
       free_ivector(iwksp,0,ndata-1);
	}
    (*Q)   = h;
    (*IQ)  = Ih;
    (*DIQ) = DIh;
/*
 * storing the total number of data points
 */
    (*EP).ndata = ndata;

/*
 * read the index of the first point to be fitted
 */
    if (TCL_ERROR == Tcl_GetInt(interp,
                             Tcl_GetVar2(interp,argv[1],"first",0),
                             &first) ) {
       sasfit_err("could not read first\n");
       return TCL_ERROR;
    }
    (*EP).first = first;

/*
 * read the number of points to be fitted
 */
    if (TCL_ERROR == Tcl_GetInt(interp,
                             Tcl_GetVar2(interp,argv[1],"npoints",0),
                             &npoints) ) {
       sasfit_err("could not read npoints\n");
       return TCL_ERROR;
    }
    (*EP).npoints = npoints;


/*
 * read the index of the first point to be fitted for Guinier fit
 */
    if (TCL_ERROR == Tcl_GetInt(interp,
                             Tcl_GetVar2(interp,argv[1],"Guinierfirst",0),
                             &first) ) {
       sasfit_err("could not read Guinierfirst\n");
       return TCL_ERROR;
    }
    (*EP).Guinierfirst = first;

/*
 * read the number of points to be fitted for Guinier fit
 */
    if (TCL_ERROR == Tcl_GetInt(interp,
                             Tcl_GetVar2(interp,argv[1],"Guiniernpoints",0),
                             &npoints) ) {
       sasfit_err("could not read Guiniernpoints\n");
       return TCL_ERROR;
    }
    (*EP).Guiniernpoints = npoints;

/*
 * read the index of the first point to be fitted for Porod fit
 */
    if (TCL_ERROR == Tcl_GetInt(interp,
                             Tcl_GetVar2(interp,argv[1],"Porodfirst",0),
                             &first) ) {
       sasfit_err("could not read Porodfirst\n");
       return TCL_ERROR;
    }
    (*EP).Porodfirst = first;

/*
 * read the number of points to be fitted for Porod fit
 */
    if (TCL_ERROR == Tcl_GetInt(interp,
                             Tcl_GetVar2(interp,argv[1],"Porodnpoints",0),
                             &npoints) ) {
       sasfit_err("could not read Porodnpoints\n");
       return TCL_ERROR;
    }
    (*EP).Porodnpoints = npoints;

/*
 * read the number PorodD
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"PorodD",0),
                             &ftmp) ) {
       sasfit_err("could not read PorodD\n");
       return TCL_ERROR;
    }
    (*EP).PorodD = ftmp;

/*
 * read the PorodDFit
 */
    if (TCL_ERROR == Tcl_GetBoolean(interp,
                             Tcl_GetVar2(interp,argv[1],"porodDfit",0),
                             &error) ) {
       sasfit_err("could not read porodDfit\n");
       return TCL_ERROR;
    }
    (*EP).PorodDFit = error;

/*
 * read the number DPorodD
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"DPorodD",0),
                             &ftmp) ) {
/*
       sasfit_err("could not read DPorodD\n");
       return TCL_ERROR;
 */
       ftmp = 0.0;
    }
    (*EP).DPorodD = ftmp;

/*
 * read the number c0
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"c0",0),
                             &ftmp) ) {
       sasfit_err("could not read c0\n");
       return TCL_ERROR;
    }
    (*EP).c0 = ftmp;

/*
 * read the number Dc0
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"Dc0",0),
                             &ftmp) ) {
/*
	   sasfit_err("could not read Dc0\n");
       return TCL_ERROR;
 */
       ftmp = 0.0;
    }
    (*EP).Dc0 = ftmp;

/*
 * read the c0fit
 */
    if (TCL_ERROR == Tcl_GetBoolean(interp,
                             Tcl_GetVar2(interp,argv[1],"c0fit",0),
                             &error) ) {
       sasfit_err("could not read porodDfit\n");
       return TCL_ERROR;
    }
    (*EP).c0Fit = error;

/*
 * read the number Invariant
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"Invariant",0),
                             &ftmp) ) {
       sasfit_err("could not read Invariant\n");
       return TCL_ERROR;
    }
    (*EP).Invariant = ftmp;

/*
 * read the number DInvariant
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"DInvariant",0),
                             &ftmp) ) {
/*
       sasfit_err("could not read DInvariant\n");
       return TCL_ERROR;
 */
	   ftmp = 0.0;
    }
    (*EP).DInvariant = ftmp;

/*
 * read the number Qexp
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"Qexp",0),
                             &ftmp) ) {
       sasfit_err("could not read Qexp\n");
       return TCL_ERROR;
    }
    (*EP).Qexp = ftmp;

/*
 * read the number DQexp
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"DQexp",0),
                             &ftmp) ) {
/*
       sasfit_err("could not read DQexp\n");
       return TCL_ERROR;
 */
	   ftmp = 0.0;
    }
    (*EP).DQexp = ftmp;

/*
 * read the number Qc4
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"Qc4",0),
                             &ftmp) ) {
       sasfit_err("could not read Qc4\n");
       return TCL_ERROR;
    }
    (*EP).Qc4 = ftmp;

/*
 * read the number DQc4
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"DQc4",0),
                             &ftmp) ) {
/*
       sasfit_err("could not read DQc4\n");
       return TCL_ERROR;
 */
       ftmp = 0.0;
    }
    (*EP).DQc4 = ftmp;

/*
 * read the number Qq0
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"Qq0",0),
                             &ftmp) ) {
       sasfit_err("could not read Qq0\n");
       return TCL_ERROR;
    }
    (*EP).Qq0 = ftmp;

/*
 * read the number DQq0
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"DQq0",0),
                             &ftmp) ) {
/*
       sasfit_err("could not read DQq0\n");
       return TCL_ERROR;
 */
       ftmp = 0.0;
    }
    (*EP).DQq0 = ftmp;

/*
 * read the number iI
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"iI",0),
                             &ftmp) ) {
       sasfit_err("could not read iI\n");
       return TCL_ERROR;
    }
    (*EP).iI = ftmp;

/*
 * read the number DiI
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"DiI",0),
                             &ftmp) ) {
/*
	   sasfit_err("could not read DiI\n");
       return TCL_ERROR;
 */
       ftmp = 0.0;
    }
    (*EP).DiI = ftmp;

/*
 * read the number iIexp
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"iIexp",0),
                             &ftmp) ) {
       sasfit_err("could not read iIexp\n");
       return TCL_ERROR;
    }
    (*EP).iIexp = ftmp;

/*
 * read the number DiIexp
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"DiIexp",0),
                             &ftmp) ) {
/*
	   sasfit_err("could not read DiIexp\n");
       return TCL_ERROR;
 */
       ftmp = 0.0;
    }
    (*EP).DiIexp = ftmp;


/*
 * read the number iIc4
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"iIc4",0),
                             &ftmp) ) {
       sasfit_err("could not read iIc4\n");
       return TCL_ERROR;
    }
    (*EP).iIc4 = ftmp;

/*
 * read the number DiIc4
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"DiIc4",0),
                             &ftmp) ) {
/*
       sasfit_err("could not read DiIc4\n");
       return TCL_ERROR;
 */
	   ftmp = 0.0;
    }
    (*EP).DiIc4 = ftmp;

/*
 * read the number iIq0
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"iIq0",0),
                             &ftmp) ) {
       sasfit_err("could not read iIq0\n");
       return TCL_ERROR;
    }
    (*EP).iIq0 = ftmp;

/*
 * read the number DiIq0
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"DiIq0",0),
                             &ftmp) ) {
/*
       sasfit_err("could not read DiIq0\n");
       return TCL_ERROR;
 */
       ftmp = 0.0;
    }
    (*EP).DiIq0 = ftmp;

/*
 * read the number c4
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"c4",0),
                             &ftmp) ) {
       sasfit_err("could not read c4\n");
       return TCL_ERROR;
    }
    (*EP).c4 = ftmp;

/*
 * read the number Dc4
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"Dc4",0),
                             &ftmp) ) {
/*
       sasfit_err("could not read Dc4\n");
       return TCL_ERROR;
 */
       ftmp = 0.0;
    }
    (*EP).Dc4 = ftmp;

/*
 * read the number I0
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"I0",0),
                             &ftmp) ) {
       sasfit_err("could not read I0\n");
       return TCL_ERROR;
    }
    (*EP).I0 = ftmp;

/*
 * read the number DI0
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"DI0",0),
                             &ftmp) ) {
/*
       sasfit_err("could not read DI0\n");
       return TCL_ERROR;
 */
    ftmp = 0.0;
    }
    (*EP).DI0 = ftmp;

/*
 * read the number RG
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"RG",0),
                             &ftmp) ) {
       sasfit_err("could not read RG\n");
       return TCL_ERROR;
    }
    (*EP).RG = ftmp;

/*
 * read the number DRG
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"DRG",0),
                             &ftmp) ) {
/*
       sasfit_err("could not read DRG\n");
       return TCL_ERROR;
 */
       ftmp = 0.0;
    }
    (*EP).DRG = ftmp;

/*
 * read the number R2G
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"R2G",0),
                             &ftmp) ) {
       sasfit_err("could not read R2G\n");
       return TCL_ERROR;
    }
    (*EP).R2G = ftmp;

/*
 * read the number DR2G
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"DR2G",0),
                             &ftmp) ) {
/*
       sasfit_err("could not read DR2G\n");
       return TCL_ERROR;
 */
       ftmp = 0.0;
    }
    (*EP).DR2G = ftmp;

/*
 * read the number li
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"li",0),
                             &ftmp) ) {
       sasfit_err("could not read li\n");
       return TCL_ERROR;
    }
    (*EP).li = ftmp;

/*
 * read the number Dli
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"Dli",0),
                             &ftmp) ) {
/*
       sasfit_err("could not read Dli\n");
       return TCL_ERROR;
 */
       ftmp = 0.0;
    }
    (*EP).Dli = ftmp;

/*
 * read the number lc
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"lc",0),
                             &ftmp) ) {
       sasfit_err("could not read lc\n");
       return TCL_ERROR;
    }
    (*EP).lc = ftmp;

/*
 * read the number Dlc
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"Dlc",0),
                             &ftmp) ) {
/*
       sasfit_err("could not read Dlc\n");
       return TCL_ERROR;
 */
       ftmp = 0.0;
    }
    (*EP).Dlc = ftmp;

/*
 * read the number VP
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"VP",0),
                             &ftmp) ) {
       sasfit_err("could not read VP\n");
       return TCL_ERROR;
    }
    (*EP).VP = ftmp;

/*
 * read the number DVP
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"DVP",0),
                             &ftmp) ) {
/*
       sasfit_err("could not read DVP\n");
       return TCL_ERROR;
 */
       ftmp = 0.0;
    }
    (*EP).DVP = ftmp;

/*
 * read the number RP
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"RP",0),
                             &ftmp) ) {
       sasfit_err("could not read RP\n");
       return TCL_ERROR;
    }
    (*EP).RP = ftmp;

/*
 * read the number DRP
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"DRP",0),
                             &ftmp) ) {
/*
       sasfit_err("could not read DRP\n");
       return TCL_ERROR;
 */
       ftmp = 0.0;
    }
    (*EP).DRP = ftmp;

/*
 * read the number S/V
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"S/V",0),
                             &ftmp) ) {
       sasfit_err("could not read S/V\n");
       return TCL_ERROR;
    }
    (*EP).S_V = ftmp;

/*
 * read the number DS/V
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"DS/V",0),
                             &ftmp) ) {
/*
       sasfit_err("could not read DS/V\n");
       return TCL_ERROR;
 */
       ftmp = 0.0;
    }
    (*EP).DS_V = ftmp;

/*
 * read the number Qmax
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"Qmax",0),
                             &ftmp) ) {
       sasfit_err("could not read Qmax\n");
       return TCL_ERROR;
    }
    (*EP).Qmax = ftmp;


/*
 * read the number Qmin
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"Qmin",0),
                             &ftmp) ) {
       sasfit_err("could not read Qmin\n");
       return TCL_ERROR;
    }
    (*EP).Qmin = ftmp;

/*
 * read the number chisq
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"chisq",0),
                             &ftmp) ) {
       sasfit_err("could not read chisq\n");
       return TCL_ERROR;
    }
    (*EP).chisq = ftmp;

/*
 * read the number I0chisq
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"I0chisq",0),
                             &ftmp) ) {
       sasfit_err("could not read I0chisq\n");
       return TCL_ERROR;
    }
    (*EP).I0chisq = ftmp;

/*
 * read the number Porodchisq
 */
    if (TCL_ERROR == Tcl_GetDouble(interp,
                             Tcl_GetVar2(interp,argv[1],"Porodchisq",0),
                             &ftmp) ) {
       sasfit_err("could not read Porodchisq\n");
       return TCL_ERROR;
    }
    (*EP).Porodchisq = ftmp;

/*
 * read parameter a[4] of the fitting function
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"a",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_err("EP(a) is not a list\n");
       return TCL_ERROR;
    }
    if ((Splitargc <= 0) || (Splitargc >4)) {
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in EP(a)\n");
       return TCL_ERROR;
    }
    for (i=0;i<Splitargc;i++) {
       Splitcode = Tcl_GetDouble(interp,Splitargv[i],&EP[0].a[i]);
       if (Splitcode == TCL_ERROR) {
          Tcl_Free((char *) Splitargv);
          sasfit_err("could not convert EP(a[i]) to double\n");
          return TCL_ERROR;
       }
    }
    for (i=Splitargc;i<4;i++) (*EP).a[i] = 0.0;
    Tcl_Free((char *) Splitargv);

/*
 * read parameter err[4] of the fitting function
 */
    for (i=Splitargc;i<4;i++) (*EP).err[i] = 0.0;

    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"err",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_err("EP(err) is not a list\n");
       return TCL_ERROR;
    }
    if ((Splitargc <= 0) || (Splitargc >4)) {
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in EP(err)\n");
       return TCL_ERROR;
    }
    for (i=0;i<Splitargc;i++) {
       Splitcode = Tcl_GetDouble(interp,Splitargv[i],&EP[0].err[i]);
       if (Splitcode == TCL_ERROR) {
		   EP[0].err[i] = 0.0;
/*
          Tcl_Free((char *) Splitargv);
          sasfit_err("could not convert EP(err[i]) to double\n");
          return TCL_ERROR;
 */
       }
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter active[4] of the fitting function
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"active",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_err("EP(active) is not a list\n");
       return TCL_ERROR;
    }
    if ((Splitargc <= 0) || (Splitargc >4)) {
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in EP(active)\n");
       return TCL_ERROR;
    }
    for (i=0;i<Splitargc;i++) {
       Splitcode = Tcl_GetBoolean(interp,Splitargv[i],&EP[0].active[i]);
       if (Splitcode == TCL_ERROR) {
          Tcl_Free((char *) Splitargv);
          sasfit_err("could not convert EP(active[i]) to boolean\n");
          return TCL_ERROR;
       }
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter aname[4] of the fitting function
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"aname",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_err("EP(aname) is not a list\n");
       return TCL_ERROR;
    }
    if ((Splitargc <= 0) || (Splitargc >4)) {
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in EP(aname)\n");
       return TCL_ERROR;
    }
    for (i=0;i<Splitargc;i++) {
       strcpy(EP[0].aname[i],Splitargv[i]);
    }
    for (i=Splitargc;i<4;i++) strcpy(EP[0].aname[i],"");
    Tcl_Free((char *) Splitargv);


    return TCL_OK;
}

/*#########################################################################*/
/*#                                                                       #*/
/*# Sasfit_StructParCalcCmd --                                            #*/
/*#                                                                       #*/
/*#      This function implements the Tcl "sasfit_StructParCalc" command. #*/
/*#                                                                       #*/
/*# Results:                                                              #*/
/*#      A standard Tcl result.                                           #*/
/*#                                                                       #*/
/*# Side effects:                                                         #*/
/*#      None.                                                            #*/
/*#                                                                       #*/
/*#########################################################################*/

int Sasfit_StructParCalcCmd(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int        argc;
    char       **argv;
{
struct extrapolPar EP;
float  *h, *Ih, *DIh, u;
float  *tx, *ty, *tsig;
int    i;
bool   error;

error = FALSE;

if (argc != 3) {
   sasfit_err("wrong # args; should be sasfit_PorodFit ?StructPar? ?xye_data?\n");
   return TCL_ERROR;
}

if (TCL_ERROR == get_EP(clientData,interp,argv,&EP,&h,&Ih,&DIh)) {
   return TCL_ERROR;
}

/*
 * extract h[first:first+npoints] and store values in x,
 * do the same with Ih[], DIh[] and store values in y[] and sig[]
 */
if (EP.first > EP.ndata) EP.first=EP.ndata;
if (EP.first < 1) EP.first=1;
if (EP.npoints <= 0) EP.npoints=0;
if ((EP.npoints+EP.first-1) > EP.ndata) EP.npoints = EP.ndata - EP.first + 1;
if (EP.npoints < 0) {
   sasfit_err("not enough data points, ndata = %d, npoints = %d\n", EP.ndata, EP.npoints);
   return TCL_ERROR;
}
tx   = dvector(0,EP.npoints-1);
ty   = dvector(0,EP.npoints-1);
tsig = dvector(0,EP.npoints-1);

for (i=0;i<EP.npoints;i++) {
   tx[i]   = h[i+EP.first-1];
   ty[i]   = Ih[i+EP.first-1];
   tsig[i] = DIh[i+EP.first-1];
}
free_dvector(h,0,EP.ndata-1);
free_dvector(Ih,0,EP.ndata-1);
free_dvector(DIh,0,EP.ndata-1);

/*
 * storing Qmin and Qmax
 */

EP.Qmin = tx[0];
EP.Qmax = tx[EP.npoints-1];


if (strcmp(EP.I0typestr,"Guinier") == 0) {
/*
 * Calculation of I(0) extrapolation contribution to scattering invariant
 */

	EP.Qq0 =   EP.I0*(   pow(fabs(EP.Qmin),3.0)/3.0
				   - 1.0/3.0*pow(fabs(EP.Qmin),5.0)/5.0 * EP.R2G
				   + 1.0/18.0*pow(fabs(EP.Qmin),7.0)/7.0 * EP.R2G*EP.R2G
				 )
	    	 - EP.c0*pow(fabs(EP.Qmin),3.0)/3.0;

   EP.DQq0 = pow(fabs(EP.DI0*(   pow(fabs(EP.Qmin),3.0)/3.0
			   	       - 1.0/3.0*pow(fabs(EP.Qmin),5.0)/5.0 * EP.R2G
				       + 1.0/18.0*pow(fabs(EP.Qmin),7.0)/7.0 * EP.R2G*EP.R2G
				     ) )
			 ,2.0)
	    + pow(fabs(EP.DR2G*EP.I0*(  1.0/9.0*pow(fabs(EP.Qmin),7.0)/7.0*EP.R2G
		                     - 1.0/3.0*pow(fabs(EP.Qmin),5.0)/5.0)),2.0)
		+ pow(fabs(EP.Dc0*pow(fabs(EP.Qmin),3.0)/3.0),2.0);
   EP.DQq0 = sqrt(fabs(EP.DQq0));
/*
 * Calculation of I(0) extrapolation contribution to integrated intensity iI = int(I(Q)*Q dQ)
 */
   EP.iIq0 =   EP.I0*(   pow(EP.Qmin,2.0)/2.0
				   - 1.0/3.0*pow(EP.Qmin,4.0)/4.0*EP.R2G
				   + 1.0/18.0*pow(EP.Qmin,6.0)/6.0*pow(EP.R2G,2.0)
				 )
		 - EP.c0*EP.Qmin*EP.Qmin/2.0;

   EP.DiIq0 = pow(fabs(EP.DI0*(EP.Qmin*EP.Qmin/2.0
			    	   - 1.0/3.0*pow(fabs(EP.Qmin),4.0)/4.0*EP.R2G
				       + 1.0/18.0*pow(fabs(EP.Qmin),6.0)/6.0*EP.R2G*EP.R2G
				     ) )
			 ,2.0)
	    + pow(fabs(EP.DR2G*EP.I0*(  1.0/9.0*pow(fabs(EP.Qmin),6.0)/6.0*EP.R2G
		                     - 1.0/3.0*pow(fabs(EP.Qmin),4.0)/4.0)),2.0)
		+ pow(fabs(EP.Dc0*EP.Qmin*EP.Qmin/2.0),2.0);
   EP.DiIq0 = sqrt(fabs(EP.DiIq0));
/*
 * Calculation of I(0) extrapolation contribution to integral intensity intI = int(I(Q) dQ)
 */
   if (EP.RG == 0.0) {
	   error = TRUE;
	   sasfit_err("StructParDataFit: EP.RG == 0\n");
   } else {
       EP.intIq0 = EP.I0/EP.RG * sqrt(3.0*PI)/2.0 *
		           sasfit_erf(EP.Qmin*EP.RG/sqrt(3.0));
   }
   if (EP.I0 == 0.0) {
	   error = TRUE;
	   sasfit_err("StructParDataFit: EP.I0 == 0\n");
	   return TCL_ERROR;
   }
   EP.DintIq0 =  pow(fabs(EP.I0*EP.intIq0/EP.I0),2) +
	             pow(fabs(EP.DRG*(EP.Qmin*EP.I0*exp(-pow(fabs(EP.Qmin*EP.RG),2.0)/3.0)-EP.intIq0)/EP.RG),2.0)
               + pow(fabs(EP.Dc0*EP.Qmin),2.0);
   EP.DintIq0 = sqrt(fabs(EP.intIq0));
} else if (strcmp(EP.I0typestr,"Zimm") == 0) {
/*
 * Calculation of I(0) extrapolation contribution to scattering invariant
 */
    u = EP.RG*EP.Qmin;
	if ((1.0+1.0/3.0*u*u) == 0.0) {
	   error = TRUE;
	   sasfit_err("StructParDataFit: (1+RG^2*Qmin^2/3) == 0\n");
	   return TCL_ERROR;
   }
	EP.Qq0 =   EP.I0*3.0/pow(fabs(EP.RG),3.0) *
		         (u-sqrt(3.0)*atan(u/sqrt(3.0)))
	    	 - EP.c0*pow(fabs(EP.Qmin),3.0)/3.0;

   EP.DQq0 =  pow(fabs(EP.DI0*EP.Qq0/EP.I0),2.0)
	        + pow(fabs(EP.DR2G*
			        (
					 3.0*u/pow(EP.RG,3.0)*(1.0-1.0/(1.0+1.0/3.0*u*u))
					 -3.0/EP.RG*EP.Qq0
					))
				  ,2.0)
		    + pow(fabs(EP.Dc0*pow(fabs(EP.Qmin),3.0)/3.0),2.0);
   EP.DQq0 = sqrt(fabs(EP.DQq0));
/*
 * Calculation of I(0) extrapolation contribution to integrated intensity iI = int(I(Q)*Q dQ)
 */
   EP.iIq0 =   EP.I0*3.0/2.0/pow(fabs(EP.RG),2.0) * (log(3.0+u*u)-log(3.0))
		 - EP.c0*EP.Qmin*EP.Qmin/2.0;

   EP.DiIq0 = pow(fabs(EP.DI0*EP.iIq0/EP.I0),2.0)
	        + pow(fabs(EP.DR2G* (
                     3.0*EP.I0*EP.Qmin*EP.Qmin/(EP.RG*(3.0+u*u))
                    -2.0/EP.RG*EP.iIq0
			     )),2.0)
		    + pow(fabs(EP.Dc0*EP.Qmin*EP.Qmin/2.0),2.0);
   EP.DiIq0 = sqrt(fabs(EP.DiIq0));
/*
 * Calculation of I(0) extrapolation contribution to integral intensity intI = int(I(Q) dQ)
 */
   EP.intIq0 = EP.I0/EP.RG * sqrt(3.0) * atan(u/sqrt(3.0));
   EP.DintIq0 =  pow(fabs(EP.I0*EP.intIq0/EP.I0),2)
	           + pow(fabs(EP.DRG*(EP.Qmin*EP.I0/EP.RG/(1.0+1.0/3.0*u*u)-EP.intIq0)),2.0)
               + pow(fabs(EP.Dc0*EP.Qmin),2.0);
   EP.DintIq0 = sqrt(fabs(EP.intIq0));

} else {
   sasfit_err("Sasfit_StructParCalcCmd: Unknown I(0) Extrapolation\n");
   tx   = dvector(0,EP.npoints-1);
   ty   = dvector(0,EP.npoints-1);
   tsig = dvector(0,EP.npoints-1);
   return TCL_ERROR;
}
/*
 * Calculation of integral intensity intI = int(I(Q) dQ)
 */

EP.intIexp = 0.0;
for (i=EP.first-1;i<EP.npoints-1;i++) {
    EP.intIexp += (tx[i+1]-tx[i])*(ty[i]+ty[i+1])/2.0;
}
EP.intIexp -= EP.c0/2.0 * (EP.Qmax - EP.Qmin);

EP.intIc4 = 0.0;
if (EP.PorodD <= 2.0) {
   EP.intIc4 += EP.c4 * pow(fabs(EP.Qmax),-(4.0 - EP.PorodD)+1.0)/(4.0-EP.PorodD-1.0);
}
EP.intI = EP.intIq0 + EP.intIexp + EP.intIc4;

EP.DintIexp = 0.0;
for (i=EP.first-1;i<EP.npoints-1;i++) {
    EP.DintIexp += pow(fabs((tx[i+1]-tx[i])*(tsig[i]+tsig[i+1])/2.0),2.0);
}
EP.DintIexp += pow(fabs(EP.Dc0/3.0 * (EP.Qmax - EP.Qmin)),2.0);
EP.DintIexp = sqrt(fabs(EP.DintIexp));
EP.DintIc4 = 0.0;
if (EP.PorodD < 1.0) {
   EP.DintIc4 = EP.Dc4 * pow(fabs(EP.Qmax),-(4.0 - EP.PorodD)+1.0)/(4.0 - EP.PorodD - 1.0);
}
EP.DintI = sqrt(EP.DintIq0*EP.DintIq0 + EP.DintIexp*EP.DintIexp + EP.DintIc4*EP.DintIc4);

/*
 * Calculation of scattering invariant
 */

EP.Qexp = 0.0;
for (i=EP.first-1;i<EP.npoints-1;i++) {
    EP.Qexp += (tx[i+1]-tx[i])*(tx[i]*tx[i]*ty[i]+tx[i+1]*tx[i+1]*ty[i+1])/2.0;
}
EP.Qexp -= EP.c0/3.0 * (pow(fabs(EP.Qmax),3.0) - pow(fabs(EP.Qmin),3.0));

EP.Qc4 = 0.0;
if (EP.PorodD < 1.0) {
   EP.Qc4 += EP.c4 * pow(fabs(EP.Qmax),-(4.0 - EP.PorodD)+3.0)/(4.0 - EP.PorodD - 3.0);
}
EP.Invariant = EP.Qq0 + EP.Qexp + EP.Qc4;

EP.DQexp = 0.0;
for (i=EP.first-1;i<EP.npoints-1;i++) {
    EP.DQexp += pow(fabs((tx[i+1]-tx[i])*(tx[i]*tx[i]*tsig[i]+tx[i+1]*tx[i+1]*tsig[i+1])/2.0),2.0);
}
EP.DQexp += pow(fabs(EP.Dc0/3.0 * (pow(fabs(EP.Qmax),3.0) - pow(fabs(EP.Qmin),3.0))),2.0);
EP.DQexp = sqrt(fabs(EP.DQexp));

EP.DQc4 = 0.0;
if (EP.PorodD < 1.0) {
   EP.DQc4 = EP.Dc4 * pow(fabs(EP.Qmax),-(4.0 - EP.PorodD)+3.0)/(4.0 - EP.PorodD - 3.0);
}
EP.DInvariant = sqrt(EP.DQq0*EP.DQq0 + EP.DQexp*EP.DQexp + EP.DQc4*EP.DQc4);

/*
 * Calculation of integrated intensity iI = int(I(Q)*Q dQ)
 */

EP.iIexp = 0.0;
for (i=EP.first-1;i<EP.npoints-1;i++) {
    EP.iIexp += (tx[i+1]-tx[i])*(tx[i]*ty[i]+tx[i+1]*ty[i+1])/2.0;
}
EP.iIexp -= EP.c0/2.0 * (EP.Qmax*EP.Qmax - EP.Qmin*EP.Qmin);

EP.iIc4 = 0.0;
if (EP.PorodD < 2.0) {
   EP.iIc4 += EP.c4 * pow(fabs(EP.Qmax),-(4.0 - EP.PorodD)+2.0)/(4.0 - EP.PorodD - 2.0);
}
EP.iI = EP.iIq0 + EP.iIexp + EP.iIc4;

EP.DiIexp = 0.0;
for (i=EP.first-1;i<EP.npoints-1;i++) {
    EP.DiIexp += pow(fabs((tx[i+1]-tx[i])*(tx[i]*tsig[i]+tx[i+1]*tsig[i+1])/2.0),2.0);
}
EP.DiIexp += pow(fabs(EP.Dc0/3.0 * (EP.Qmax*EP.Qmax - EP.Qmin*EP.Qmin)),2.0);
EP.DiIexp = sqrt(fabs(EP.DiIexp));

EP.DiIc4 = 0.0;
if (EP.PorodD < 1.0) {
   EP.DiIc4 = EP.Dc4 * pow(fabs(EP.Qmax),-(4.0 - EP.PorodD)+2.0)/(4.0 - EP.PorodD - 2.0);
}

EP.DiI = sqrt(EP.DiIq0*EP.DiIq0 + EP.DiIexp*EP.DiIexp + EP.DiIc4*EP.DiIc4);

if (EP.Invariant == 0.0) {
   error = TRUE;
   sasfit_err("StructParDataFit: EP.Invariant == 0\n");
   return TCL_ERROR;
}

if (EP.c4 == 0.0) {
   error = TRUE;
   sasfit_err("StructParDataFit: EP.c4 == 0\n");
   return TCL_ERROR;
}

if (EP.iI == 0.0) {
   error = TRUE;
   sasfit_err("StructParDataFit: EP.iI == 0\n");
   return TCL_ERROR;
}

if (EP.intI == 0.0) {
   error = TRUE;
   sasfit_err("StructParDataFit: EP.intI == 0\n");
   return TCL_ERROR;
}

/*
 * Calculation of specific surface
 */
EP.S_V  = PI/EP.Invariant * EP.c4;
EP.DS_V = sqrt( pow(fabs(EP.Dc4/EP.c4),2.0)
			  +pow(fabs(EP.DInvariant/EP.Invariant),2.0)) * fabs(EP.S_V);

/*
 * Calculation of correlation length
 */
EP.lc  = PI*EP.iI/EP.Invariant;
EP.Dlc = sqrt( pow(fabs(EP.DiI/EP.iI),2.0)
			  +pow(fabs(EP.DInvariant/EP.Invariant),2.0)) * fabs(EP.lc);
/*
 * Calculation of correlation area Ac
 */
EP.Ac  = 2*PI*EP.intI/EP.Invariant;
EP.DAc = sqrt( pow(fabs(EP.DintI/EP.intI),2.0)
			  +pow(fabs(EP.DInvariant/EP.Invariant),2.0)) * fabs(EP.Ac);
/*
 * calculation of intersection length
 */
EP.li  = 4.0/PI * EP.Invariant/EP.c4;
EP.Dli = sqrt( pow(fabs(EP.Dc4/EP.c4),2.0)
			  +pow(fabs(EP.DInvariant/EP.Invariant),2.0)) * fabs(EP.li);

if (EP.I0 == 0.0) {
   error = TRUE;
   sasfit_err("StructParDataFit: EP.I0 == 0\n");
   return TCL_ERROR;
}
/*
 * calculation of Porod volume
 */
EP.VP  = 2.0*PI*PI * EP.I0/EP.Invariant;

EP.DVP = sqrt( pow(fabs(EP.DI0/EP.I0),2.0)
			  +pow(fabs(EP.DInvariant/EP.Invariant),2.0)) * fabs(EP.VP);

/*
 * calculation of Porod radius
 */
if (EP.VP == 0.0) {
   error = TRUE;
   sasfit_err("StructParDataFit: EP.VP == 0\n");
   return TCL_ERROR;
}
EP.RP  = pow(3.0/(4.0*PI) * fabs(EP.VP),1.0/3.0);
EP.DRP = sqrt( pow(fabs(EP.DVP/EP.VP),2.0)) * fabs(EP.RP);

if (TCL_ERROR == save_EP(clientData,interp,EP)) {
   sasfit_err("can't save structure variable EP in tcl-array %s\n", argv[1]);
   free_dvector(tx,0,EP.Porodnpoints-1);
   free_dvector(ty,0,EP.Porodnpoints-1);
   free_dvector(tsig,0,EP.Porodnpoints-1);
   return TCL_ERROR;
}

   free_dvector(tx,0,EP.Porodnpoints-1);
   free_dvector(ty,0,EP.Porodnpoints-1);
   free_dvector(tsig,0,EP.Porodnpoints-1);

   return TCL_OK;
}

/*#########################################################################*/
/*#                                                                       #*/
/*# Sasfit_PorodFitCmd --                                                 #*/
/*#                                                                       #*/
/*#      This function implements the Tcl "sasfit_PorodFit" command.      #*/
/*#                                                                       #*/
/*# Results:                                                              #*/
/*#      A standard Tcl result.                                           #*/
/*#                                                                       #*/
/*# Side effects:                                                         #*/
/*#      None.                                                            #*/
/*#                                                                       #*/
/*#########################################################################*/

int Sasfit_PorodFitCmd(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int        argc;
    char       **argv;
{
struct extrapolPar EP;
float  *h, *Ih, *DIh;
float  *tx, *ty, *tsig, *x, *xx, *y, *yy, *sig, *ysig, yth, dyda[4], par[4], errpar[4];
float  **alpha, **covar;
int    *lista,mfit,ma;
float  ochisq, chisq, lchisq, oalambda, alambda, q;
int    i,j,k,itst;
char   errstr[256],Buffer[256];
bool   error;
Tcl_DString DsBuffer;

error = FALSE;

if (argc != 3) {
   sasfit_err("wrong # args; should be sasfit_PorodFit ?StructPar? ?xye_data?\n");
   return TCL_ERROR;
}

if (TCL_ERROR == get_EP(clientData,interp,argv,&EP,&h,&Ih,&DIh)) {
   return TCL_ERROR;
}

/*
 * setting the variables ma,mfit, lista[], alambda, **covar, **alpha for
 * the routine SASFIT2mrqmin
 */
ma = 4;
covar = matrix(0,ma-1,0,ma-1);
alpha = matrix(0,ma-1,0,ma-1);
lista = ivector(0,ma-1);
/*
 * setting lista[] and mfit by analysing EP.active
 */
j=0;
k=1;
for (i=0; i<ma; i++) {
   if (EP.active[i]) {
      lista[j] = i;
      j++;
   } else {
      lista[ma-k] = i;
      k++;
   }
}
mfit = j;

/*
 * extract h[first:first+npoints] and store values in x,
 * do the same with Ih[], DIh[] and store values in y[] and sig[]
 */
if (EP.first > EP.ndata) EP.first=EP.ndata-mfit;
if (EP.first < 1) EP.first=1;
if (EP.npoints <= mfit) EP.npoints=mfit+1;
if ((EP.npoints+EP.first-1) > EP.ndata) EP.npoints = EP.ndata - EP.first + 1;
if (EP.npoints <= mfit) {
   sasfit_err("not enough data points, ndata = %d, npoints = %d, mfit = %d\n", EP.ndata, EP.npoints, mfit);
   free_matrix(covar,0,ma-1,0,ma-1);
   free_matrix(alpha,0,ma-1,0,ma-1);
   free_ivector(lista,0,ma-1);
   return TCL_ERROR;
}
tx   = dvector(0,EP.npoints-1);
ty   = dvector(0,EP.npoints-1);
tsig = dvector(0,EP.npoints-1);

for (i=0;i<EP.npoints;i++) {
   tx[i]   = h[i+EP.first-1];
   ty[i]   = Ih[i+EP.first-1];
   tsig[i] = DIh[i+EP.first-1];
}
free_dvector(h,0,EP.ndata-1);
free_dvector(Ih,0,EP.ndata-1);
free_dvector(DIh,0,EP.ndata-1);

/*
 * extract tx[Porodfirst:first+Porodnpoints] and store values in x,
 * do the same with ty[], tsig[] and store values in y[] and sig[]
 */
if (EP.Porodfirst > EP.npoints) EP.Porodfirst=EP.npoints-mfit;
if (EP.Porodfirst < 1) EP.Porodfirst=1;
if (EP.Porodnpoints <= mfit) EP.Porodnpoints=mfit+1;
if ((EP.Porodnpoints+EP.Porodfirst-1)>EP.npoints) EP.Porodnpoints=EP.npoints-EP.Porodfirst+1;
if (EP.Porodnpoints <= mfit) {
   sasfit_err("not enough data points, npoints = %d, Porodnpoints = %d, mfit = %d\n", EP.npoints, EP.Porodnpoints, mfit);
   free_matrix(covar,0,ma-1,0,ma-1);
   free_matrix(alpha,0,ma-1,0,ma-1);
   free_ivector(lista,0,ma-1);
   free_dvector(tx,0,EP.npoints-1);
   free_dvector(ty,0,EP.npoints-1);
   free_dvector(tsig,0,EP.npoints-1);
   return TCL_ERROR;
}

x   = dvector(0,EP.Porodnpoints-1);
y   = dvector(0,EP.Porodnpoints-1);
sig = dvector(0,EP.Porodnpoints-1);
if (strcmp(EP.order,"ascending") == 0) {
   for (i=0;i<EP.Porodnpoints;i++) {
      x[i]   = tx[i+EP.Porodfirst-1];
      y[i]   = ty[i+EP.Porodfirst-1];
      sig[i] = tsig[i+EP.Porodfirst-1];
   }
   free_dvector(tx,0,EP.npoints-1);
   free_dvector(ty,0,EP.npoints-1);
   free_dvector(tsig,0,EP.npoints-1);
} else if (strcmp(EP.order,"descending") == 0) {
   for (i=0;i<EP.Porodnpoints;i++) {
      x[i]   = tx[EP.npoints-EP.Porodfirst-i];
      y[i]   = ty[EP.npoints-EP.Porodfirst-i];
      sig[i] = tsig[EP.npoints-EP.Porodfirst-i];
   }
   free_dvector(tx,0,EP.npoints-1);
   free_dvector(ty,0,EP.npoints-1);
   free_dvector(tsig,0,EP.npoints-1);
} else {
   sasfit_err("unknown order EP.order = %s\n", EP.order);
   free_matrix(covar,0,ma-1,0,ma-1);
   free_matrix(alpha,0,ma-1,0,ma-1);
   free_ivector(lista,0,ma-1);
   free_dvector(x,0,EP.Porodnpoints-1);
   free_dvector(y,0,EP.Porodnpoints-1);
   free_dvector(sig,0,EP.Porodnpoints-1);
   free_dvector(tx,0,EP.npoints-1);
   free_dvector(ty,0,EP.npoints-1);
   free_dvector(tsig,0,EP.npoints-1);
   return TCL_ERROR;
}
/*
 * for an initial guess for the parameters EP.a[0] and EP.a[2] I fit a straight line y=a+b*x
 * to the data I*Q^(4-D) = c0 + c4*Q^(4-D). EP.a[1] = c1 is assumed to be zero
 */
xx   = dvector(1,EP.Porodnpoints);
yy   = dvector(1,EP.Porodnpoints);
ysig = dvector(1,EP.Porodnpoints);
for (i=0;i<=EP.Porodnpoints-1;i++) {
   xx[i+1]   = pow(x[i],4.0-EP.a[3]);
   yy[i+1]   = y[i]*xx[i+1];
   ysig[i+1] = fabs(sig[i]*xx[i+1]);
}
for (i=0;i<4;i++) {
  EP.err[i] = 0.0;
  errpar[i] = 0.0;
}

par[0]    = EP.a[0];
errpar[0] = EP.err[0];
lchisq = 0.0;


fit(interp,xx,yy,EP.Porodnpoints,ysig,0,&EP.a[2],&EP.a[0],&EP.err[2],&EP.err[0],&lchisq,&q,&error);


if (!EP.active[0]) EP.a[0] = par[0];
for (i=0;i<4;i++) {
   par[i]    = EP.a[i];
   errpar[i] = EP.err[i];
}

free_dvector(xx,  1,EP.Porodnpoints);
free_dvector(yy,  1,EP.Porodnpoints);
free_dvector(ysig,1,EP.Porodnpoints);

if (error == TRUE) {
   free_matrix(covar,0,ma-1,0,ma-1);
   free_matrix(alpha,0,ma-1,0,ma-1);
   free_ivector(lista,0,ma-1);
   free_dvector(x,0,EP.Porodnpoints-1);
   free_dvector(y,0,EP.Porodnpoints-1);
   free_dvector(sig,0,EP.Porodnpoints-1);
   return TCL_ERROR;
}

/*
 * The Levenberg - Marquard Minimierung wird vorläufig nicht angewendet,
 * deshalb ist error auf TRUE gesetzt, EP.error auf 3 gesetzt und lchisq negiert
 */

/*
 * if (EP.error == 3) then only a straight line will be fitted to the data in Porod plot
 */


if (EP.error != 3) {

/*
 * start fitting Porod law c0 + c1*x[i] + c_4*x[i]^(-(4-D)) to the data y[i],
 * with EP.a[0] = c0  (active fit parameter)
 *      EP.a[1] = c1  (default value is zero)
 *      EP.a[2] = c4  (active fit parameter)
 *      EP.a[3] = D   (default value is zero)
 */

   alambda = -1.0;
   SASFIT2mrqmin(interp,x,y,sig,EP.Porodnpoints,
                 EP.a,ma,lista,mfit,covar,alpha,
				 &chisq,Porod,&alambda,&error);
   if (error == TRUE) goto PorodErrorHandler;
   k    = 1;
   itst = 0;
   while (itst < 3) {
	  if (k > 250) {
		  error = TRUE;
		  goto PorodErrorHandler;
	  }
      k++;
      ochisq =chisq;
      oalambda = alambda;
      SASFIT2mrqmin(interp,x,y,sig,EP.Porodnpoints,
                    EP.a,ma,lista,mfit,covar,alpha,
					&chisq,Porod,&alambda,&error);
      if (error == TRUE) goto PorodErrorHandler;
      if (alambda < oalambda) itst = 0; else itst++;
   }
   alambda = 0.0;
   SASFIT2mrqmin(interp,x,y,sig,EP.Porodnpoints,
                 EP.a,ma,lista,mfit,covar,alpha,
				 &chisq,Porod,&alambda,&error);
   if (error == TRUE) goto PorodErrorHandler;
   for (i=0;i<ma;i++) {
	  if (covar[i][i] <= 0.0)  {
		 EP.err[i] = 0.0;
	  } else {
	 	 EP.err[i] = sqrt(covar[i][i]);
	  }
   }
} else  { EP.a[1] = 0.0;}

PorodErrorHandler: if (error == TRUE) {
			// if an error occurs in the SASFIT2mrqmin routine the
			// results of the linear fit are used.
			chisq = -lchisq*(EP.Porodnpoints-mfit);
			for (i=0;i<4;i++) {
				EP.a[i]   = par[i];
				EP.err[i] = errpar[i];
			}
			EP.PorodDFit = FALSE;
			if (EP.error != 3) {
				alambda = -10.0;
				SASFIT2mrqmin(interp,x,y,sig,EP.Porodnpoints,
						EP.a,ma,lista,mfit,covar,alpha,
						&chisq,Porod,&alambda,&error);
			}
		}
error = FALSE;


if (EP.Porodnpoints > mfit) EP.Porodchisq = chisq/(EP.Porodnpoints-mfit);
    else EP.Porodchisq = -1.0;

EP.c0     = EP.a[0];
EP.Dc0    = EP.err[0]*sqrt(fabs(EP.Porodchisq));
EP.c1     = EP.a[1];
EP.Dc1    = EP.err[1]*sqrt(fabs(EP.Porodchisq));
EP.c4     = EP.a[2];
EP.Dc4    = EP.err[2]*sqrt(fabs(EP.Porodchisq));
EP.PorodD = EP.a[3];
EP.DPorodD= EP.err[3]*sqrt(fabs(EP.Porodchisq));

if (TCL_ERROR == save_EP(clientData,interp,EP)) {
   sasfit_err("can't save structure variable EP in tcl-array %s\n", argv[1]);
   free_matrix(covar,0,ma-1,0,ma-1);
   free_matrix(alpha,0,ma-1,0,ma-1);
   free_ivector(lista,0,ma-1);
   free_dvector(x,0,EP.Porodnpoints-1);
   free_dvector(y,0,EP.Porodnpoints-1);
   free_dvector(sig,0,EP.Porodnpoints-1);
			return TCL_ERROR;
}

Tcl_ResetResult(interp);
Tcl_DStringInit(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EP.Porodnpoints;i++) {
    sprintf(Buffer,"%lg",x[i]);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EP.Porodnpoints;i++) {
   	Porod(x[i],EP.a,&yth,dyda,4,&error,errstr);
	   if (error == TRUE) {
		Tcl_ResetResult(interp);
		sasfit_err("Could not calculate Porod function\nQ=%lg, c0=%lg, c1=%lg, c4=%lg, D=%lg\n",
							x[i],EP.a[0],EP.a[1],EP.a[2],EP.a[3]);
     		Tcl_DStringFree(&DsBuffer);
	     	free_matrix(covar,0,ma-1,0,ma-1);
       free_matrix(alpha,0,ma-1,0,ma-1);
       free_ivector(lista,0,ma-1);
       free_dvector(x,0,EP.Porodnpoints-1);
       free_dvector(y,0,EP.Porodnpoints-1);
       free_dvector(sig,0,EP.Porodnpoints-1);
			return TCL_ERROR;
				}
    sprintf(Buffer,"%lg",yth);
	   Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringResult(interp,&DsBuffer);
Tcl_DStringFree(&DsBuffer);

free_matrix(covar,0,ma-1,0,ma-1);
free_matrix(alpha,0,ma-1,0,ma-1);
free_ivector(lista,0,ma-1);
free_dvector(x,0,EP.Porodnpoints-1);
free_dvector(y,0,EP.Porodnpoints-1);
free_dvector(sig,0,EP.Porodnpoints-1);
return TCL_OK;
}

// helper for guinier linear intermediate result
void addStringList(Tcl_DString * DsBuffer, char * buf,
                   double * arr, int start, int end)
{
	int i;
	if (!DsBuffer || !buf) return;

	Tcl_DStringStartSublist(DsBuffer);
	for (i=start; i <= end; i++)
	{
		sprintf(buf,"%lg",arr[i]);
		Tcl_DStringAppendElement(DsBuffer,buf);
	}
	Tcl_DStringEndSublist(DsBuffer);
}

void forward_intermediate_guinier(Tcl_Interp * interp,
                                  struct extrapolPar * ep,
                                  Tcl_DString * dynbuf,
                                  char * buf,
                                  double lchisq,
				  double * xx, double * yy, double * ysig, int end)
{
	double rg, rg2, drg, drg2;
	int i;

	if (!interp || !ep || !dynbuf || !buf) return;

// printf("EP.Guiniernpoints: %d\n",ep->Guiniernpoints);

	sprintf(buf,"%lg", exp(ep->a[0]));
	Tcl_SetVar2(interp,ep->name,"guinier_lin_i0",buf,0);
	sprintf(buf,"%lg", exp(ep->a[0])*ep->err[0]);
	Tcl_SetVar2(interp,ep->name,"guinier_lin_i0_err",buf,0);

	rg = sqrt(fabs(ep->a[1]));
	rg2 = ep->a[1];
	drg2 = ep->err[1]*sqrt(fabs(lchisq));
	drg = 0.0;
	if (!SASFIT_EQUAL(ep->a[1], 0.0)) { // RG^2
		drg = fabs(drg2/rg);
	}
	sprintf(buf,"%lg", rg);
	Tcl_SetVar2(interp,ep->name,"guinier_lin_rg",buf,0);
	sprintf(buf,"%lg", rg2);
	Tcl_SetVar2(interp,ep->name,"guinier_lin_rg2",buf,0);
	sprintf(buf,"%lg", drg);
	Tcl_SetVar2(interp,ep->name,"guinier_lin_rg_err",buf,0);
	sprintf(buf,"%lg", drg2);
	Tcl_SetVar2(interp,ep->name,"guinier_lin_rg2_err",buf,0);

	sprintf(buf,"%lg", lchisq);
	Tcl_SetVar2(interp,ep->name,"guinier_lin_chisq",buf,0);

	Tcl_DStringInit(dynbuf);
//	addStringList(dynbuf, buf, xx, 1, end); // q
//	addStringList(dynbuf, buf, yy, 1, end); // exp. iq data
//	for (i=1; i <= end; i++) ysig[i] = ep->a[0] + ep->a[1] * xx[i];
//	addStringList(dynbuf, buf, ysig, 1, end); // iq, linear guinier approx
//	for (i=1; i <= end; i++) yy[i] = ysig[i] - yy[i];
//	addStringList(dynbuf, buf, yy, 1, end); // residuum, linear guinier approx
	addStringList(dynbuf, buf, xx, 1, end); // q
	addStringList(dynbuf, buf, yy, 1, end); // exp. iq data
	for (i=1; i <= end; i++) ysig[i] = ep->a[0] + ep->a[1] * xx[i];
	for (i=1; i <= end; i++) yy[i] = ysig[i] - yy[i];
	for (i=1; i <= end; i++) ysig[i] = exp(ysig[i]);
	addStringList(dynbuf, buf, ysig, 1, end); // iq, linear guinier approx
	addStringList(dynbuf, buf, yy, 1, end); // residuum, linear guinier approx
	Tcl_SetVar2(interp,ep->name,"guinier_lin_data",Tcl_DStringValue(dynbuf),0);
	Tcl_DStringFree(dynbuf);
}

/*#########################################################################*/
/*#                                                                       #*/
/*# Sasfit_GuinierFitCmd --                                               #*/
/*#                                                                       #*/
/*#      This function implements the Tcl "sasfit_GuinierFit" command.    #*/
/*#                                                                       #*/
/*# Results:                                                              #*/
/*#      A standard Tcl result.                                           #*/
/*#                                                                       #*/
/*# Side effects:                                                         #*/
/*#      None.                                                            #*/
/*#                                                                       #*/
/*#########################################################################*/

int Sasfit_GuinierFitCmd(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int        argc;
    char       **argv;
{
struct extrapolPar EP;
float  *h, *Ih, *DIh;
float  *tx, *ty, *tsig, *x, *xx, *y, *yy, *sig, *ysig, yth,
       dyda[4], par[4], errpar[4];
float  **alpha, **covar;
int    *lista,mfit,ma;
float  ochisq, chisq, lchisq, oalambda, alambda,q;
int    i,j,k,itst;
char   errstr[256],Buffer[256];
bool   error;
Tcl_DString DsBuffer;

error = FALSE;

if (argc != 3) {
   sasfit_err("wrong # args; should be sasfit_GuinierFit ?StructPar? ?xye_data?\n");
   return TCL_ERROR;
}

if (TCL_ERROR == get_EP(clientData,interp,argv,&EP,&h,&Ih,&DIh)) {
   return TCL_ERROR;
}

/*
 * setting the variables ma,mfit, lista[], alambda, **covar, **alpha for
 * the routine SASFIT2mrqmin
 */
ma = 2;
covar = matrix(0,ma-1,0,ma-1);
alpha = matrix(0,ma-1,0,ma-1);
lista = ivector(0,ma-1);
/*
 * setting lista[] and mfit by analysing EP.active
 */
j=0;
k=1;
for (i=0; i<ma; i++) {
   if (EP.active[i]) {
      lista[j] = i;
      j++;
   } else {
      lista[ma-k] = i;
      k++;
   }
}
mfit = j;

/*
 * extract h[first:first+npoints] and store values in x,
 * do the same with Ih[], DIh[] and store values in y[] and sig[]
 */
if (EP.first > EP.ndata) EP.first=EP.ndata-mfit;
if (EP.first < 1) EP.first=1;
if (EP.npoints <= mfit) EP.npoints=mfit;
if ((EP.npoints+EP.first-1) > EP.ndata) EP.npoints = EP.ndata - EP.first + 1;
if (EP.npoints < mfit) {
   sasfit_err("not enough data points, ndata = %d, npoints = %d, mfit = %d\n", EP.ndata, EP.npoints, mfit);
   free_matrix(covar,0,ma-1,0,ma-1);
   free_matrix(alpha,0,ma-1,0,ma-1);
   free_ivector(lista,0,ma-1);
   return TCL_ERROR;
}
tx   = dvector(0,EP.npoints-1);
ty   = dvector(0,EP.npoints-1);
tsig = dvector(0,EP.npoints-1);

for (i=0;i<EP.npoints;i++) {
   tx[i]   = h[i+EP.first-1];
   ty[i]   = Ih[i+EP.first-1]-EP.c0;
   tsig[i] = DIh[i+EP.first-1];
}
free_dvector(h,0,EP.ndata-1);
free_dvector(Ih,0,EP.ndata-1);
free_dvector(DIh,0,EP.ndata-1);
/*
 * extract tx[Guinierfirst:first+Guiniernpoints] and store values in x,
 * do the same with ty[], tsig[] and store values in y[] and sig[]
 */
if (EP.Guinierfirst > EP.npoints) EP.Guinierfirst=EP.npoints-mfit;
if (EP.Guinierfirst < 1) EP.Guinierfirst=1;
if (EP.Guiniernpoints <= mfit) EP.Guiniernpoints=mfit;
if ((EP.Guiniernpoints+EP.Guinierfirst-1)>EP.npoints) EP.Guiniernpoints=EP.npoints-EP.Guinierfirst+1;
if (EP.Guiniernpoints < mfit) {
   sasfit_err("not enough data points, npoints = %d, Guiniernpoints = %d, mfit = %d\n", EP.npoints, EP.Guiniernpoints, mfit);
   free_matrix(covar,0,ma-1,0,ma-1);
   free_matrix(alpha,0,ma-1,0,ma-1);
   free_ivector(lista,0,ma-1);
   free_dvector(tx,0,EP.npoints-1);
   free_dvector(ty,0,EP.npoints-1);
   free_dvector(tsig,0,EP.npoints-1);
   return TCL_ERROR;
}

x   = dvector(0,EP.Guiniernpoints-1);
y   = dvector(0,EP.Guiniernpoints-1);
sig = dvector(0,EP.Guiniernpoints-1);
if (strcmp(EP.order,"ascending") == 0) {
   for (i=0;i<EP.Guiniernpoints;i++) {
      x[i]   = tx[i+EP.Guinierfirst-1];
      y[i]   = ty[i+EP.Guinierfirst-1];
      sig[i] = tsig[i+EP.Guinierfirst-1];
   }
   free_dvector(tx,0,EP.npoints-1);
   free_dvector(ty,0,EP.npoints-1);
   free_dvector(tsig,0,EP.npoints-1);
} else if (strcmp(EP.order,"descending") == 0) {
   for (i=0;i<EP.Guiniernpoints;i++) {
      x[i]   = tx[EP.npoints-EP.Guinierfirst-i];
      y[i]   = ty[EP.npoints-EP.Guinierfirst-i];
      sig[i] = tsig[EP.npoints-EP.Guinierfirst-i];
   }
   free_dvector(tx,0,EP.npoints-1);
   free_dvector(ty,0,EP.npoints-1);
   free_dvector(tsig,0,EP.npoints-1);
} else {
   sasfit_err("unknown order EP.order = %s\n", EP.order);
   free_matrix(covar,0,ma-1,0,ma-1);
   free_matrix(alpha,0,ma-1,0,ma-1);
   free_ivector(lista,0,ma-1);
   free_dvector(x,0,EP.Guiniernpoints-1);
   free_dvector(y,0,EP.Guiniernpoints-1);
   free_dvector(sig,0,EP.Guiniernpoints-1);
   free_dvector(tx,0,EP.npoints-1);
   free_dvector(ty,0,EP.npoints-1);
   free_dvector(tsig,0,EP.npoints-1);
   return TCL_ERROR;
}
/*
 * for an initial guess for the parameters EP.a[0] and EP.a[1]
 * fit a straight line y=a+b*x  to the data
 * ln(I) = ln(I0) + Rg^2 * (-Q^2/3).
 */
xx   = dvector(1,EP.Guiniernpoints);
yy   = dvector(1,EP.Guiniernpoints);
ysig = dvector(1,EP.Guiniernpoints);
k = 0;
for (i=0;i<=EP.Guiniernpoints-1;i++) {
   if (y[i] <= 0.0) {
      k++;
   } else {
      xx[i+1-k]   = -x[i]*x[i]/3.0;
      yy[i+1-k]   = log(y[i]);
      ysig[i+1-k] = 1.0;
   }
}
EP.a[0] = EP.I0;
EP.a[1] = EP.RG*EP.RG;
for (i=0;i<4;i++) {
  EP.err[i] = 0.0;
  errpar[i] = 0.0;
}
fit(interp,xx,yy,EP.Guiniernpoints-k,ysig,0,&EP.a[0],&EP.a[1],&EP.err[0],&EP.err[1],&lchisq,&q,&error);

forward_intermediate_guinier(interp, &EP, &DsBuffer, Buffer,
		lchisq, xx, yy, ysig, EP.Guiniernpoints-k);

/*
 * Transformation of linear parameters back to I0 and Rg2
 */
EP.a[0]   = exp(EP.a[0]);
EP.err[0] = exp(EP.a[0])*EP.err[0];
EP.err[1] = EP.err[1];

for (i=0;i<4;i++) {
   par[i]    = EP.a[i];
   errpar[i] = EP.err[i];
}
free_dvector(xx,  1,EP.Guiniernpoints);
free_dvector(yy,  1,EP.Guiniernpoints);
free_dvector(ysig,1,EP.Guiniernpoints);
if (error == TRUE) {
   free_matrix(covar,0,ma-1,0,ma-1);
   free_matrix(alpha,0,ma-1,0,ma-1);
   free_ivector(lista,0,ma-1);
   free_dvector(x,0,EP.Guiniernpoints-1);
   free_dvector(y,0,EP.Guiniernpoints-1);
   free_dvector(sig,0,EP.Guiniernpoints-1);
   return TCL_ERROR;
}


/*
 * if (EP.error == 3) then only a straight line will be fitted
 * to the data in Guinier plot
 */


if (EP.error != 3) {

/*
 * start fitting Guinier law y = I0 * exp(-RG2*Q^2/3) to the data y[i],
 * with EP.a[0] = I0   (active fit parameter)
 *      EP.a[1] = RG2  (active fit parameter)
 */

   alambda = -1.0;
   SASFIT2mrqmin(interp,x,y,sig,EP.Guiniernpoints,
	             EP.a,ma,lista,mfit,covar,alpha,
		     &chisq,Guinier,&alambda,&error);
   if (error == TRUE) goto GuinierErrorHandler;
   k    = 1;
   itst = 0;
   while (itst < 3) {
	if (k > 250) {
		error = TRUE;
		goto GuinierErrorHandler;
	}
	k++;
	ochisq = chisq;
	oalambda = alambda;
	SASFIT2mrqmin(interp,x,y,sig,EP.Guiniernpoints,
	              EP.a,ma,lista,mfit,covar,alpha,
	              &chisq,Guinier,&alambda,&error);
	if (error == TRUE) goto GuinierErrorHandler;
	if (alambda < oalambda) itst = 0; else itst++;
   }
   alambda = 0.0;
   SASFIT2mrqmin(interp,x,y,sig,EP.Guiniernpoints,
                 EP.a,ma,lista,mfit,covar,alpha,
                 &chisq,Guinier,&alambda,&error);
   if (error == TRUE) goto GuinierErrorHandler;
   for (i=0;i<ma;i++) {
	if (covar[i][i] <= 0.0) {
		EP.err[i] = 0.0;
	} else {
		EP.err[i] = sqrt(covar[i][i]);
	}
   }
}

GuinierErrorHandler: if (error == TRUE) {         /*                                         */
            	        chisq = -lchisq *         /* if an error occurs in the SASFIT2mrqmin */
                         (EP.Guiniernpoints-mfit);/* routine the results of the linear fit   */
                        for (i=0;i<4;i++) {       /* are used.                               */
                           EP.a[i]   = par[i];    /*                                         */
                           EP.err[i] = errpar[i]; /*                                         */
			}
			if (EP.error != 3) {
                           alambda = -10.0;
                           SASFIT2mrqmin(interp,x,y,sig,EP.Guiniernpoints,
				             EP.a,ma,lista,mfit,covar,alpha,
						 &chisq,Guinier,&alambda,&error);
			}
		 }

error = FALSE;

if (EP.Guiniernpoints > mfit) {
	EP.I0chisq = chisq/(EP.Guiniernpoints-mfit);
} else {EP.I0chisq = -1.0;}

EP.I0     = EP.a[0];
EP.DI0    = EP.err[0]*sqrt(fabs(EP.I0chisq));
EP.DI0    = sqrt(EP.DI0*EP.DI0 + EP.Dc0*EP.Dc0);
EP.R2G    = EP.a[1];
EP.DR2G   = EP.err[1]*sqrt(fabs(EP.I0chisq));
EP.RG     = sqrt(fabs(EP.a[1]));
if (EP.R2G == 0) {
	EP.DRG = 0.0;
} else {
   EP.DRG = fabs(EP.DR2G/sqrt(EP.R2G));
}

if (TCL_ERROR == save_EP(clientData,interp,EP)) {
   sasfit_err("can't save structure variable EP in tcl-array %s\n", argv[1]);
   free_matrix(covar,0,ma-1,0,ma-1);
   free_matrix(alpha,0,ma-1,0,ma-1);
   free_ivector(lista,0,ma-1);
   free_dvector(x,0,EP.Guiniernpoints-1);
   free_dvector(y,0,EP.Guiniernpoints-1);
   free_dvector(sig,0,EP.Guiniernpoints-1);
   return TCL_ERROR;
}

Tcl_ResetResult(interp);
Tcl_DStringInit(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EP.Guiniernpoints;i++) {
    sprintf(Buffer,"%lg",x[i]);
	   Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EP.Guiniernpoints;i++) {
	   Guinier(x[i],EP.a,&yth,dyda,ma,&error,errstr);
	   if (error == TRUE) {
  	   	Tcl_ResetResult(interp);
       sasfit_err("Could not calculate Guinier function\nQ=%lg, I0=%lg, Rg2=%lg\n", x[i],EP.a[0],EP.a[1]);
	     	Tcl_DStringFree(&DsBuffer);
	     	free_matrix(covar,0,ma-1,0,ma-1);
       free_matrix(alpha,0,ma-1,0,ma-1);
       free_ivector(lista,0,ma-1);
       free_dvector(x,0,EP.Guiniernpoints-1);
       free_dvector(y,0,EP.Guiniernpoints-1);
       free_dvector(sig,0,EP.Guiniernpoints-1);
	     	return TCL_ERROR;
				}
    sprintf(Buffer,"%lg",yth+EP.c0);
	   Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringResult(interp,&DsBuffer);
Tcl_DStringFree(&DsBuffer);

free_matrix(covar,0,ma-1,0,ma-1);
free_matrix(alpha,0,ma-1,0,ma-1);
free_ivector(lista,0,ma-1);
free_dvector(x,0,EP.Guiniernpoints-1);
free_dvector(y,0,EP.Guiniernpoints-1);
free_dvector(sig,0,EP.Guiniernpoints-1);
return TCL_OK;
}


/*#########################################################################*/
/*#                                                                       #*/
/*# Sasfit_ZimmFitCmd --                                                  #*/
/*#                                                                       #*/
/*#      This function implements the Tcl "sasfit_ZimmFit" command.       #*/
/*#                                                                       #*/
/*# Results:                                                              #*/
/*#      A standard Tcl result.                                           #*/
/*#                                                                       #*/
/*# Side effects:                                                         #*/
/*#      None.                                                            #*/
/*#                                                                       #*/
/*#########################################################################*/

int Sasfit_ZimmFitCmd(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int        argc;
    char       **argv;
{
struct extrapolPar EP;
float  *h, *Ih, *DIh;
float  *tx, *ty, *tsig, *x, *xx, *y, *yy, *sig, *ysig, yth,
       dyda[4], par[4], errpar[4];
float  **alpha, **covar;
int    *lista,mfit,ma;
float  ochisq, chisq, lchisq, oalambda, alambda,q;
int    i,j,k,m,itst;
char   errstr[256],Buffer[256];
bool   error;
Tcl_DString DsBuffer;

error = FALSE;

if (argc != 3) {
   sasfit_err("wrong # args; should be sasfit_ZimmFit ?StructPar? ?xye_data?\n");
   return TCL_ERROR;
}

if (TCL_ERROR == get_EP(clientData,interp,argv,&EP,&h,&Ih,&DIh)) {
   return TCL_ERROR;
}

/*
 * setting the variables ma,mfit, lista[], alambda, **covar, **alpha for
 * the routine SASFIT2mrqmin
 */
ma = 2;
covar = matrix(0,ma-1,0,ma-1);
alpha = matrix(0,ma-1,0,ma-1);
lista = ivector(0,ma-1);
/*
 * setting lista[] and mfit by analysing EP.active
 */
j=0;
k=1;
for (i=0; i<ma; i++) {
   if (EP.active[i]) {
      lista[j] = i;
      j++;
   } else {
      lista[ma-k] = i;
      k++;
   }
}
mfit = j;

/*
 * extract h[first:first+npoints] and store values in x,
 * do the same with Ih[], DIh[] and store values in y[] and sig[]
 */
if (EP.first > EP.ndata) EP.first=EP.ndata-mfit;
if (EP.first < 1) EP.first=1;
if (EP.npoints < mfit) EP.npoints=mfit+1;
if ((EP.npoints+EP.first-1) > EP.ndata) EP.npoints = EP.ndata - EP.first + 1;
if (EP.npoints <= mfit) {
   sasfit_err("not enough data points, ndata = %d, npoints = %d, mfit = %d\n", EP.ndata, EP.npoints, mfit);
   free_matrix(covar,0,ma-1,0,ma-1);
   free_matrix(alpha,0,ma-1,0,ma-1);
   free_ivector(lista,0,ma-1);
   return TCL_ERROR;
}
tx   = dvector(0,EP.npoints-1);
ty   = dvector(0,EP.npoints-1);
tsig = dvector(0,EP.npoints-1);

for (i=0;i<EP.npoints;i++) {
   tx[i]   = h[i+EP.first-1];
   ty[i]   = Ih[i+EP.first-1]-EP.c0;
   tsig[i] = DIh[i+EP.first-1];
}
free_dvector(h,0,EP.ndata-1);
free_dvector(Ih,0,EP.ndata-1);
free_dvector(DIh,0,EP.ndata-1);
/*
 * extract tx[Guinierfirst:first+Guiniernpoints] and store values in x,
 * do the same with ty[], tsig[] and store values in y[] and sig[]
 */
if (EP.Guinierfirst > EP.npoints) EP.Guinierfirst=EP.npoints-mfit;
if (EP.Guinierfirst < 1) EP.Guinierfirst=1;
if (EP.Guiniernpoints <= mfit) EP.Guiniernpoints=mfit+1;
if ((EP.Guiniernpoints+EP.Guinierfirst-1)>EP.npoints) EP.Guiniernpoints=EP.npoints-EP.Guinierfirst+1;
if (EP.Guiniernpoints <= mfit) {
   sasfit_err("not enough data points, npoints = %d, Guiniernpoints = %d, mfit = %d\n", EP.npoints, EP.Guiniernpoints, mfit);
   free_matrix(covar,0,ma-1,0,ma-1);
   free_matrix(alpha,0,ma-1,0,ma-1);
   free_ivector(lista,0,ma-1);
   free_dvector(tx,0,EP.npoints-1);
   free_dvector(ty,0,EP.npoints-1);
   free_dvector(tsig,0,EP.npoints-1);
   return TCL_ERROR;
}

x   = dvector(0,EP.Guiniernpoints-1);
y   = dvector(0,EP.Guiniernpoints-1);
sig = dvector(0,EP.Guiniernpoints-1);
if (strcmp(EP.order,"ascending") == 0) {
   for (i=0;i<EP.Guiniernpoints;i++) {
      x[i]   = tx[i+EP.Guinierfirst-1];
      y[i]   = ty[i+EP.Guinierfirst-1];
      sig[i] = tsig[i+EP.Guinierfirst-1];
   }
   free_dvector(tx,0,EP.npoints-1);
   free_dvector(ty,0,EP.npoints-1);
   free_dvector(tsig,0,EP.npoints-1);
} else if (strcmp(EP.order,"descending") == 0) {
   for (i=0;i<EP.Guiniernpoints;i++) {
      x[i]   = tx[EP.npoints-EP.Guinierfirst-i];
      y[i]   = ty[EP.npoints-EP.Guinierfirst-i];
      sig[i] = tsig[EP.npoints-EP.Guinierfirst-i];
   }
   free_dvector(tx,0,EP.npoints-1);
   free_dvector(ty,0,EP.npoints-1);
   free_dvector(tsig,0,EP.npoints-1);
} else {
   sasfit_err("unknown order EP.order = %s\n", EP.order);
   free_matrix(covar,0,ma-1,0,ma-1);
   free_matrix(alpha,0,ma-1,0,ma-1);
   free_ivector(lista,0,ma-1);
   free_dvector(x,0,EP.Guiniernpoints-1);
   free_dvector(y,0,EP.Guiniernpoints-1);
   free_dvector(sig,0,EP.Guiniernpoints-1);
   free_dvector(tx,0,EP.npoints-1);
   free_dvector(ty,0,EP.npoints-1);
   free_dvector(tsig,0,EP.npoints-1);
   return TCL_ERROR;
}
/*
 * for an initial guess for the parameters EP.a[0] and EP.a[1] I fit a straight line y=a+b*x
 * to the data 1/I = 1/I0 + Rg^2/I0 * Q^2/3.
 */
xx   = dvector(1,EP.Guiniernpoints);
yy   = dvector(1,EP.Guiniernpoints);
ysig = dvector(1,EP.Guiniernpoints);
m = 0;
for (i=0;i<=EP.Guiniernpoints-1;i++) {
    if (y[i] == 0.0) {
	   m++;
	} else {
       xx[i+1-m]   = x[i]*x[i]/3.0;
       yy[i+1-m]   = 1.0/y[i];
       ysig[i+1-m] = sig[i];
	}
}
if ((EP.I0 > 0.0) && (EP.RG > 0.0)) {
  	EP.a[0] = 1.0/EP.I0;
    EP.a[1] = EP.RG*EP.RG/EP.I0;
} else {
   EP.a[0] = 0.001;
   EP.a[1] = 10.0;
}
for (i=0;i<4;i++) {
  EP.err[i] = 0.0;
  errpar[i] = 0.0;
}
fit(interp,xx,yy,EP.Guiniernpoints-m,ysig,0,&EP.a[0],&EP.a[1],&EP.err[0],&EP.err[1],&lchisq,&q,&error);
/*
 * Transformation of linear parameters back to I0 and Rg2
 */

if (EP.a[0] != 0.0) {
	EP.a[0] = 1.0/EP.a[0];
	EP.a[1] = sqrt(fabs(EP.a[1]*EP.a[0])) * EP.a[1]*EP.a[0]/fabs(EP.a[1]*EP.a[0]);
	EP.err[0] = EP.err[0]*pow(EP.a[0],2.0);
	EP.err[1] = sqrt(pow(EP.err[1]*EP.a[0]/EP.a[1],2.0) + pow(EP.err[0]*EP.a[1]/EP.a[0],2.0))/2.0;
} else {
	error = TRUE;
    sasfit_err("Sasfit_ZimmFitCmd: cannot perform Zimm Fit,\nI(0) seems to be infinity\n");
    free_dvector(xx,  1,EP.Guiniernpoints);
    free_dvector(yy,  1,EP.Guiniernpoints);
    free_dvector(ysig,1,EP.Guiniernpoints);
    free_matrix(covar,0,ma-1,0,ma-1);
    free_matrix(alpha,0,ma-1,0,ma-1);
    free_ivector(lista,0,ma-1);
    free_dvector(x,0,EP.Guiniernpoints-1);
    free_dvector(y,0,EP.Guiniernpoints-1);
    free_dvector(sig,0,EP.Guiniernpoints-1);
}
lchisq = 0.0;
for (i=0;i<EP.Guiniernpoints-m;i++) {
   Zimm2(xx[i+1],EP.a,&yth,dyda,ma,&error,errstr);
   lchisq = lchisq + pow( (yth-yy[i+1])/ysig[i+1] ,2.0)/(EP.Guiniernpoints-m-mfit);
}

for (i=0;i<4;i++) {
   par[i]    = EP.a[i];
   errpar[i] = EP.err[i];
}

free_dvector(xx,  1,EP.Guiniernpoints);
free_dvector(yy,  1,EP.Guiniernpoints);
free_dvector(ysig,1,EP.Guiniernpoints);
if (error == TRUE) {
   free_matrix(covar,0,ma-1,0,ma-1);
   free_matrix(alpha,0,ma-1,0,ma-1);
   free_ivector(lista,0,ma-1);
   free_dvector(x,0,EP.Guiniernpoints-1);
   free_dvector(y,0,EP.Guiniernpoints-1);
   free_dvector(sig,0,EP.Guiniernpoints-1);
   return TCL_ERROR;
}

/*
 * if (EP.error == 3) then only a straight line will be fitted to the data in Zimm plot
 */


if (EP.error != 3) {


/*
 * start fitting Zimm2 law y = I0 / (1+RG^2*Q^2/3) to the data y[i],
 * with EP.a[0] = I0   (active fit parameter)
 *      EP.a[1] = RG   (active fit parameter)
 */

   EP.a[0] = fabs(EP.a[0]);
   EP.a[1] = fabs(EP.a[1]);
   alambda = -1.0;
   SASFIT2mrqmin(interp,x,y,sig,EP.Guiniernpoints,
	             EP.a,ma,lista,mfit,covar,alpha,
				 &chisq,Zimm2,&alambda,&error);
   if (error == TRUE) goto ZimmErrorHandler;
   k    = 1;
   itst = 0;
   while (itst < 3) {
	     if (k > 250) {
			   sasfit_err("Sasfit_ZimmFitCmd: to many interations\n");
		       error = TRUE;
		       goto ZimmErrorHandler;
						}
      k++;
      ochisq = chisq;
      oalambda = alambda;
      SASFIT2mrqmin(interp,x,y,sig,EP.Guiniernpoints,
		            EP.a,ma,lista,mfit,covar,alpha,
					&chisq,Zimm2,&alambda,&error);
      if (error == TRUE) goto ZimmErrorHandler;
      if (alambda < oalambda) itst = 0; else itst++;
   }
   alambda = 0.0;
   SASFIT2mrqmin(interp,x,y,sig,EP.Guiniernpoints,
	             EP.a,ma,lista,mfit,covar,alpha,
				 &chisq,Zimm2,&alambda,&error);
   if (error == TRUE) goto ZimmErrorHandler;
   for (i=0;i<ma;i++) {
   	  if (covar[i][i] <= 0.0) {
	    EP.err[i] = 0.0;
	  } else {
	 	EP.err[i] = sqrt(covar[i][i]);
	  }
   }
}

ZimmErrorHandler: if (error == TRUE) {         /*                                         */
	    			                           /* if an error occurs in the SASFIT2mrqmin */
						                       /* routine the results of the linear fit   */
                                               /* are used.                               */

	                 chisq = -lchisq;
					 for (i=0;i<4;i++) {
			    	    EP.a[i]   = par[i];
				        EP.err[i] = errpar[i];
					 }

					 if (EP.error != 3) {
					     alambda = -10.0;
						 SASFIT2mrqmin(interp,x,y,sig,EP.Guiniernpoints,
							           EP.a,ma,lista,mfit,covar,alpha,
									   &chisq,Zimm2,&alambda,&error);
					 }
				  }
error = FALSE;

if (EP.Guiniernpoints > mfit) EP.I0chisq = chisq/(EP.Guiniernpoints-mfit);
   else EP.I0chisq = -1.0;

EP.I0     = EP.a[0];
EP.DI0    = EP.err[0]*sqrt(fabs(EP.I0chisq))+ EP.Dc0;
EP.RG     = EP.a[1];
EP.R2G    = EP.a[1]*EP.a[1];
EP.DRG    = EP.err[1];
EP.DR2G   = pow(EP.err[1],2.0);

if (TCL_ERROR == save_EP(clientData,interp,EP)) {
   sasfit_err("can't save structure variable EP in tcl-array %s\n", argv[1]);
   free_matrix(covar,0,ma-1,0,ma-1);
   free_matrix(alpha,0,ma-1,0,ma-1);
   free_ivector(lista,0,ma-1);
   free_dvector(x,0,EP.Guiniernpoints-1);
   free_dvector(y,0,EP.Guiniernpoints-1);
   free_dvector(sig,0,EP.Guiniernpoints-1);
   return TCL_ERROR;
}

Tcl_ResetResult(interp);
Tcl_DStringInit(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EP.Guiniernpoints;i++) {
    sprintf(Buffer,"%lg",x[i]);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EP.Guiniernpoints;i++) {
			Zimm2(x[i],EP.a,&yth,dyda,ma,&error,errstr);
			if (error == TRUE) {
						Tcl_ResetResult(interp);
						sasfit_err("Could not calculate Zimm function\nQ=%lg, I0=%lg, Rg2=%lg\n", x[i],EP.a[0],EP.a[1]);
						Tcl_DStringFree(&DsBuffer);
						free_matrix(covar,0,ma-1,0,ma-1);
						free_matrix(alpha,0,ma-1,0,ma-1);
						free_ivector(lista,0,ma-1);
      free_dvector(x,0,EP.Guiniernpoints-1);
      free_dvector(y,0,EP.Guiniernpoints-1);
						free_dvector(sig,0,EP.Guiniernpoints-1);
		    return TCL_ERROR;
			}
   sprintf(Buffer,"%lg",yth+EP.c0);
	  Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringResult(interp,&DsBuffer);
Tcl_DStringFree(&DsBuffer);

free_matrix(covar,0,ma-1,0,ma-1);
free_matrix(alpha,0,ma-1,0,ma-1);
free_ivector(lista,0,ma-1);
free_dvector(x,0,EP.Guiniernpoints-1);
free_dvector(y,0,EP.Guiniernpoints-1);
free_dvector(sig,0,EP.Guiniernpoints-1);
return TCL_OK;
}

/*#########################################################################*/
/*#                                                                       #*/
/*# Sasfit_DebyeFitCmd --                                                 #*/
/*#                                                                       #*/
/*#      This function implements the Tcl "sasfit_ZimmFit" command.       #*/
/*#                                                                       #*/
/*# Results:                                                              #*/
/*#      A standard Tcl result.                                           #*/
/*#                                                                       #*/
/*# Side effects:                                                         #*/
/*#      None.                                                            #*/
/*#                                                                       #*/
/*#########################################################################*/

int Sasfit_DebyeFitCmd(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int        argc;
    char       **argv;
{
struct extrapolPar EP;
float  *h, *Ih, *DIh;
float  *tx, *ty, *tsig, *x, *y, *sig, yth, dyda[4];
float  **alpha, **covar;
int    *lista,mfit,ma;
float  ochisq, chisq, oalambda, alambda;
int    i,j,k,itst;
char   errstr[256],Buffer[256];
bool   error;
Tcl_DString DsBuffer;

error = FALSE;

if (argc != 3) {
   sasfit_err("wrong # args; should be sasfit_DebyeFit ?StructPar? ?xye_data?\n");
   return TCL_ERROR;
}

if (TCL_ERROR == get_EP(clientData,interp,argv,&EP,&h,&Ih,&DIh)) {
   return TCL_ERROR;
}

/*
 * setting the variables ma,mfit, lista[], alambda, **covar, **alpha for
 * the routine SASFIT2mrqmin
 */
ma = 2;
covar = matrix(0,ma-1,0,ma-1);
alpha = matrix(0,ma-1,0,ma-1);
lista = ivector(0,ma-1);
/*
 * setting lista[] and mfit by analysing EP.active
 */
j=0;
k=1;
for (i=0; i<ma; i++) {
   if (EP.active[i]) {
      lista[j] = i;
      j++;
   } else {
      lista[ma-k] = i;
      k++;
   }
}
mfit = j;

/*
 * extract h[first:first+npoints] and store values in x,
 * do the same with Ih[], DIh[] and store values in y[] and sig[]
 */

if (EP.first > EP.ndata) EP.first=EP.ndata-mfit;
if (EP.first < 1) EP.first=1;
if (EP.npoints <= mfit) EP.npoints=mfit+1;
if ((EP.npoints+EP.first-1) > EP.ndata) EP.npoints = EP.ndata - EP.first + 1;
if (EP.npoints <= mfit) {
   sasfit_err("not enough data points, ndata = %d, npoints = %d, mfit = %d\n", EP.ndata, EP.npoints, mfit);
   free_matrix(covar,0,ma-1,0,ma-1);
   free_matrix(alpha,0,ma-1,0,ma-1);
   free_ivector(lista,0,ma-1);
   return TCL_ERROR;
}
tx   = dvector(0,EP.npoints-1);
ty   = dvector(0,EP.npoints-1);
tsig = dvector(0,EP.npoints-1);

for (i=0;i<EP.npoints;i++) {
   tx[i]   = h[i+EP.first-1];
   ty[i]   = Ih[i+EP.first-1]-EP.c0;
   tsig[i] = DIh[i+EP.first-1];
}
free_dvector(h,0,EP.ndata-1);
free_dvector(Ih,0,EP.ndata-1);
free_dvector(DIh,0,EP.ndata-1);
/*
 * extract tx[Guinierfirst:first+Guiniernpoints] and store values in x,
 * do the same with ty[], tsig[] and store values in y[] and sig[]
 */
if (EP.Guinierfirst > EP.npoints) EP.Guinierfirst=EP.npoints-mfit;
if (EP.Guinierfirst < 1) EP.Guinierfirst=1;
if (EP.Guiniernpoints <= mfit) EP.Guiniernpoints=mfit+1;
if ((EP.Guiniernpoints+EP.Guinierfirst-1)>EP.npoints) EP.Guiniernpoints=EP.npoints-EP.Guinierfirst+1;
if (EP.Guiniernpoints <= mfit) {
   sasfit_err("not enough data points, npoints = %d, Guiniernpoints = %d, mfit = %d\n", EP.npoints, EP.Guiniernpoints, mfit);
   free_matrix(covar,0,ma-1,0,ma-1);
   free_matrix(alpha,0,ma-1,0,ma-1);
   free_ivector(lista,0,ma-1);
   free_dvector(tx,0,EP.npoints-1);
   free_dvector(ty,0,EP.npoints-1);
   free_dvector(tsig,0,EP.npoints-1);
   return TCL_ERROR;
}

x   = dvector(0,EP.Guiniernpoints-1);
y   = dvector(0,EP.Guiniernpoints-1);
sig = dvector(0,EP.Guiniernpoints-1);
if (strcmp(EP.order,"ascending") == 0) {
   for (i=0;i<EP.Guiniernpoints-1;i++) {
      x[i]   = tx[i+EP.Guinierfirst-1];
      y[i]   = ty[i+EP.Guinierfirst-1];
      sig[i] = tsig[i+EP.Guinierfirst-1];
   }
   free_dvector(tx,0,EP.npoints-1);
   free_dvector(ty,0,EP.npoints-1);
   free_dvector(tsig,0,EP.npoints-1);
} else if (strcmp(EP.order,"descending") == 0) {
   for (i=0;i<EP.Guiniernpoints;i++) {
      x[i]   = tx[EP.npoints-EP.Guinierfirst-i];
      y[i]   = ty[EP.npoints-EP.Guinierfirst-i];
      sig[i] = tsig[EP.npoints-EP.Guinierfirst-i];
   }
   free_dvector(tx,0,EP.npoints-1);
   free_dvector(ty,0,EP.npoints-1);
   free_dvector(tsig,0,EP.npoints-1);
} else {
   sasfit_err("unknown order EP.order = %s\n", EP.order);
   free_matrix(covar,0,ma-1,0,ma-1);
   free_matrix(alpha,0,ma-1,0,ma-1);
   free_ivector(lista,0,ma-1);
   free_dvector(x,0,EP.Guiniernpoints-1);
   free_dvector(y,0,EP.Guiniernpoints-1);
   free_dvector(sig,0,EP.Guiniernpoints-1);
   free_dvector(tx,0,EP.npoints-1);
   free_dvector(ty,0,EP.npoints-1);
   free_dvector(tsig,0,EP.npoints-1);
   return TCL_ERROR;
}

if (EP.I0 <= 0.0) {
  	EP.a[0] = 100.0;
} else {EP.a[0] = EP.I0;}

if (EP.RG <= 0.0) {
	EP.a[1] = 100.0;
} else {EP.a[1] = EP.RG;}

/*
 * if (EP.error == 3) then only a straight line will be fitted to the data in Zimm plot
 */


if (EP.error != 3) {


/*
 * start fitting Debye law y = 2 I0 (1-u+exp(u))/u^2 to the data y[i],
 * with EP.a[0] = I0   (active fit parameter)
 *      EP.a[1] = RG   (active fit parameter)
 */

   alambda = -1.0;
   SASFIT2mrqmin(interp,x,y,sig,EP.Guiniernpoints,
	             EP.a,ma,lista,mfit,covar,alpha,
				 &chisq,Debye,&alambda,&error);
   if (error == TRUE) goto DebyeErrorHandler;
   k    = 1;
   itst = 0;
   while (itst < 3) {
		if (k > 250) {
		       error = TRUE;
			   sasfit_err("Sasfit_DebyeFitCmd: to many interations\n");
		       goto DebyeErrorHandler;
		}
      k++;
      ochisq = chisq;
      oalambda = alambda;
      SASFIT2mrqmin(interp,x,y,sig,EP.Guiniernpoints,
		            EP.a,ma,lista,mfit,covar,alpha,
					&chisq,Debye,&alambda,&error);
      if (error == TRUE) goto DebyeErrorHandler;
      if (alambda < oalambda) itst = 0; else itst++;
   }
   alambda = 0.0;
   SASFIT2mrqmin(interp,x,y,sig,EP.Guiniernpoints,
	             EP.a,ma,lista,mfit,covar,alpha,
				 &chisq,Debye,&alambda,&error);
   if (error == TRUE) goto DebyeErrorHandler;
   for (i=0;i<ma;i++) {
   	  if (covar[i][i] <= 0.0) {
	    EP.err[i] = 0.0;
	  } else {
	 	EP.err[i] = sqrt(covar[i][i]);
	  }
   }
}

DebyeErrorHandler: if (error == TRUE) {        /*                                         */
	    			                           /* if an error occurs in the SASFIT2mrqmin */
						                       /* routine the results of the linear fit   */
                                               /* are used.                               */
					 if (EP.error != 3) {
					     alambda = -10.0;
						 SASFIT2mrqmin(interp,x,y,sig,EP.Guiniernpoints,EP.a,ma,lista,mfit,
					                               covar,alpha,&chisq,Debye,&alambda,&error);
					 }
					 return TCL_ERROR;
				  }
error = FALSE;

if (EP.Guiniernpoints > mfit) EP.I0chisq = chisq/(EP.Guiniernpoints-mfit);
   else EP.I0chisq = -1.0;

EP.I0     = EP.a[0];
EP.DI0    = EP.err[0]*sqrt(fabs(EP.I0chisq))+ EP.Dc0;
EP.RG     = EP.a[1];
EP.R2G    = EP.a[1]*EP.a[1];
EP.DRG    = EP.err[1];
EP.DR2G   = pow(EP.err[1],2.0);

if (TCL_ERROR == save_EP(clientData,interp,EP)) {
   sasfit_err("can't save structure variable EP in tcl-array %s\n", argv[1]);
   free_matrix(covar,0,ma-1,0,ma-1);
   free_matrix(alpha,0,ma-1,0,ma-1);
   free_ivector(lista,0,ma-1);
   free_dvector(x,0,EP.Guiniernpoints-1);
   free_dvector(y,0,EP.Guiniernpoints-1);
   free_dvector(sig,0,EP.Guiniernpoints-1);
   return TCL_ERROR;
}

Tcl_ResetResult(interp);
Tcl_DStringInit(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EP.Guiniernpoints;i++) {
    sprintf(Buffer,"%lg",x[i]);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EP.Guiniernpoints;i++) {
	Debye(x[i],EP.a,&yth,dyda,ma,&error,errstr);
	if (error == TRUE) {
		Tcl_ResetResult(interp);
		sasfit_err("Could not calculate Debye function\nQ=%lg, I0=%lg, Rg=%lg\n", x[i],EP.a[0],EP.a[1]);
		Tcl_DStringFree(&DsBuffer);
		free_matrix(covar,0,ma-1,0,ma-1);
		free_matrix(alpha,0,ma-1,0,ma-1);
		free_ivector(lista,0,ma-1);
		free_dvector(x,0,EP.Guiniernpoints-1);
		free_dvector(y,0,EP.Guiniernpoints-1);
		free_dvector(sig,0,EP.Guiniernpoints-1);
		return TCL_ERROR;
	}
	sprintf(Buffer,"%lg",yth+EP.c0);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringResult(interp,&DsBuffer);
Tcl_DStringFree(&DsBuffer);

free_matrix(covar,0,ma-1,0,ma-1);
free_matrix(alpha,0,ma-1,0,ma-1);
free_ivector(lista,0,ma-1);
free_dvector(x,0,EP.Guiniernpoints-1);
free_dvector(y,0,EP.Guiniernpoints-1);
free_dvector(sig,0,EP.Guiniernpoints-1);
return TCL_OK;
}


/*#########################################################################*/
/*#                                                                       #*/
/*# Sasfit_OrnsteinZernickeFitCmd --                                      #*/
/*#                                                                       #*/
/*#      This function implements the Tcl "sasfit_OrnsteinZernickeFit"    #*/
/*#      command.                                                         #*/
/*#                                                                       #*/
/*# Results:                                                              #*/
/*#      A standard Tcl result.                                           #*/
/*#                                                                       #*/
/*# Side effects:                                                         #*/
/*#      None.                                                            #*/
/*#                                                                       #*/
/*#########################################################################*/

int Sasfit_OrnsteinZernickeFitCmd(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int        argc;
    char       **argv;
{
	return TCL_OK;
}

double EM_DR_DoubleSmooth_Operator(void *EM_structure) {
    EM_param_t *EMparam;
    scalar sumIth, sumIh;
    EMparam = (EM_param_t *)EM_structure;
    int i,j,l, nr, nh;
    scalar p1,p2,p3,PS, NormXwork, eps, chi2, S, nnlambda;

    nr=EMparam->nR;
    nh=EMparam->nh;

    if ((EMparam->smooth > 0) && EMparam->smooth_bool) {
        for (i=0;i<nr;i++) EMparam->in[i] = (fabs(EMparam->out[i])<=DBL_MIN?DBL_MIN:fabs(EMparam->out[i]));
        EMparam->xwork[0] = exp(log(EMparam->in[0])*EMparam->S[0][0]+log(EMparam->in[0])*EMparam->S[0][1]);
        for (i=1;i<nr-1;i++) {
            EMparam->xwork[i]=exp(log(EMparam->in[i-1])*EMparam->S[i][i-1]+log(EMparam->in[i])*EMparam->S[i][i]+log(EMparam->in[i+1])*EMparam->S[i][i+1]);
        }
        EMparam->xwork[nr-1]=exp(log(EMparam->in[nr-2])*EMparam->S[nr-1][nr-2]+log(EMparam->in[nr-1])*EMparam->S[nr-1][nr-1]);
    } else {
        for (i=0;i<nr;i++) {
            EMparam->in[i] = fabs(EMparam->out[i]);
            EMparam->xwork[i] = fabs(EMparam->out[i]);
        }
    }


    for (j=0;j<nh;j++) {
        EMparam->Ithwork[j]=0;
        for (l=0;l<nr;l++) EMparam->Ithwork[j]=EMparam->Ithwork[j]+EMparam->dr[l]*EMparam->xwork[l]*EMparam->Awork[j][l];
    }

    nnlambda = EMparam->overrelaxation*2-1;
    for (i=0;i<nr;i++) {
        p3=0;
        p1=0;
        for (j=0;j<nh;j++) {
            p1=p1+EMparam->Awork[j][i]*EMparam->dh[j];
            p2=EMparam->Ihwork[j]*EMparam->Awork[j][i];
            p3=p3+p2*EMparam->dh[j]/EMparam->Ithwork[j];
        }
        EMparam->out[i]=EMparam->xwork[i]*(p3/p1-1);
        if (EMparam->out[i]<0 && EMparam->in[i]/fabs(EMparam->out[i])<nnlambda) nnlambda = EMparam->in[i]/fabs(EMparam->out[i]);
    }
    for (i=0;i<nr;i++) {
        EMparam->xwork[i] = EMparam->xwork[i] + (nnlambda+1)*0.5*EMparam->out[i];
    }
    EMparam->out[0] = fabs(EMparam->xwork[0]*EMparam->S[0][0]+EMparam->xwork[0]*EMparam->S[0][1]);
    eps = gsl_pow_2(EMparam->out[0]-EMparam->in[0]);
    for (i=1;i<nr-1;i++) {
        EMparam->out[i]=fabs(EMparam->xwork[i-1]*EMparam->S[i][i-1]+EMparam->xwork[i]*EMparam->S[i][i]+EMparam->xwork[i+1]*EMparam->S[i][i+1]);
        eps = eps+gsl_pow_2(EMparam->out[i]-EMparam->in[i]);
    }
    EMparam->out[nr-1]=fabs(EMparam->xwork[nr-2]*EMparam->S[nr-1][nr-2]+EMparam->xwork[nr-1]*EMparam->S[nr-1][nr-1]);

    eps=0;
    for (i=0;i<nr;i++) {
        EMparam->out[0] = EMparam->xwork[0]*EMparam->S[0][0]+EMparam->xwork[0]*EMparam->S[0][1];
        for (i=1;i<nr-1;i++) {
            EMparam->out[i]=EMparam->xwork[i-1]*EMparam->S[i][i-1]+EMparam->xwork[i]*EMparam->S[i][i]+EMparam->xwork[i+1]*EMparam->S[i][i+1];
        }
        EMparam->out[nr-1]=EMparam->xwork[nr-2]*EMparam->S[nr-1][nr-2]+EMparam->xwork[nr-1]*EMparam->S[nr-1][nr-1];

        eps = eps+gsl_pow_2(EMparam->out[i]-EMparam->in[i]);
    }
    eps=sqrt(eps)/nr;

    chi2=0;;
    EMparam->Gtest=0;
    EMparam->JSDtest=0;
    EMparam->chi2test=0;
    sumIth=0;
    sumIh=0;
    for (i=0;i<EMparam->nh;i++){
        EMparam->Ithwork[i] = 0;
        for (j=0;j<nr;j++) {
            EMparam->Ithwork[i] = EMparam->Ithwork[i] + EMparam->dr[j]*EMparam->Awork[i][j]*EMparam->out[j];
        }
        if (!gsl_finite(EMparam->Ithwork[i])) EMparam->Ithwork[i]=gsl_isinf(EMparam->Ithwork[i])*DBL_MAX;
        sumIth+=EMparam->Ithwork[i]+EMparam->C4;
        sumIh +=EMparam->Ihwork[i] +EMparam->C4;
        chi2=chi2+gsl_pow_2((EMparam->Ithwork[i]-EMparam->Ihwork[i])*(1+EMparam->em_weight*(EMparam->DIh[i]-1))/EMparam->DIh[i]);
    }
    for (i=0;i<EMparam->nh;i++){
        if ((EMparam->Ithwork[i]+EMparam->C4 > 0)&& (EMparam->Ihwork[i]+EMparam->C4)>0) {
            EMparam->Gtest+=2*(EMparam->Ihwork[i]+EMparam->C4)*log((EMparam->Ihwork[i]+EMparam->C4)/(EMparam->Ithwork[i]+EMparam->C4)*sumIth/sumIh);
            EMparam->JSDtest+=0.5*(EMparam->Ihwork[i] +EMparam->C4)/sumIh *log((EMparam->Ihwork[i] +EMparam->C4)/sumIh /((EMparam->Ithwork[i]+EMparam->C4)/sumIth+(EMparam->Ihwork[i]+EMparam->C4)/sumIh)*2.);
            EMparam->JSDtest+=0.5*(EMparam->Ithwork[i]+EMparam->C4)/sumIth*log((EMparam->Ithwork[i]+EMparam->C4)/sumIth/((EMparam->Ithwork[i]+EMparam->C4)/sumIth+(EMparam->Ihwork[i]+EMparam->C4)/sumIh)*2.);
        }
        EMparam->chi2test+=gsl_pow_2(EMparam->Ihwork[i]-EMparam->Ithwork[i])/(EMparam->Ithwork[i]+EMparam->C4);
    }
    chi2=chi2/EMparam->nh;
    if (!gsl_finite(chi2)) chi2=DBL_MAX;
    EMparam->redchi2=chi2;
    return chi2;
}

double EM_DR_EntropyConstantPrior_Operator(void *EM_structure) {
    EM_param_t *EMparam;
    scalar sumIth, sumIh;
    EMparam = (EM_param_t *) EM_structure;
    int i,j,l, nr;
    scalar p1,p2,p3, NormXwork, eps, chi2, Entropy, nnlambda;
    nr=EMparam->nR;

    Entropy=0;
    NormXwork=0;
    for (i=0;i<nr;i++) {
        EMparam->in[i] =  fabs(EMparam->out[i]);
        if (EMparam->prior[i] == 0) {
                sasfit_out("i:%d prior:%lg\n",i,EMparam->prior[i]);
                EMparam->prior[i]=DBL_MIN;
        }
        if (EMparam->in[i] > 0 && gsl_finite(EMparam->in[i]*log(EMparam->in[i]/EMparam->prior[i]))) {
            Entropy = Entropy-EMparam->in[i]*log(EMparam->in[i]/EMparam->prior[i]);
            NormXwork=NormXwork+EMparam->in[i];
        }

    }
    Entropy=Entropy/NormXwork;

    for (j=0;j<EMparam->nh;j++) {
        EMparam->Ithwork[j]=0;
        for (l=0;l<nr;l++) EMparam->Ithwork[j]=EMparam->Ithwork[j]+EMparam->dr[l]*EMparam->in[l]*EMparam->Awork[j][l];
    }

    nnlambda = EMparam->overrelaxation*2-1;
    for (i=0;i<nr;i++) {
        p3=0;
        p1=0;
        for (j=0;j<EMparam->nh;j++) {
            p1=p1+EMparam->Awork[j][i]*EMparam->dh[j];
            p2=EMparam->Ihwork[j]*EMparam->Awork[j][i];
            p3=p3+p2*EMparam->dh[j]/EMparam->Ithwork[j];
        }
        if (EMparam->in[i] > 0) {
            EMparam->out[i]=EMparam->in[i]*(p3/p1-1.)
                                      -EMparam->in[i]*EMparam->lambda*(Entropy+log(EMparam->in[i]/EMparam->prior[i]));
        } else {
            EMparam->out[i]=DBL_MIN;
        }
        if (EMparam->out[i]<0 && EMparam->in[i]/fabs(EMparam->out[i])<nnlambda) nnlambda = EMparam->in[i]/fabs(EMparam->out[i]);
    }

    eps=0;
    for (i=0;i<nr;i++) {
        EMparam->out[i] = EMparam->in[i] + (nnlambda+1)/2.*EMparam->out[i];
        eps = eps+gsl_pow_2(EMparam->out[i]-EMparam->in[i]);
    }
    eps=sqrt(eps);

    chi2=0;;
    EMparam->Gtest=0;
    EMparam->JSDtest=0;
    EMparam->chi2test=0;
    sumIth=0;
    sumIh=0;
    for (i=0;i<EMparam->nh;i++){
        EMparam->Ithwork[i] = 0;
        for (j=0;j<nr;j++) {
            EMparam->Ithwork[i] = EMparam->Ithwork[i] + EMparam->dr[j]*EMparam->Awork[i][j]*EMparam->out[j];
        }
        if (!gsl_finite(EMparam->Ithwork[i])) EMparam->Ithwork[i]=gsl_isinf(EMparam->Ithwork[i])*DBL_MAX;
        sumIth+=EMparam->Ithwork[i]+EMparam->C4;
        sumIh +=EMparam->Ihwork[i] +EMparam->C4;
        chi2=chi2+gsl_pow_2((EMparam->Ithwork[i]-EMparam->Ihwork[i])/EMparam->DIh[i]*(1+EMparam->em_weight*(EMparam->DIh[i]-1)));
    }
    for (i=0;i<EMparam->nh;i++){
        if ((EMparam->Ithwork[i]+EMparam->C4 > 0)&& (EMparam->Ihwork[i]+EMparam->C4)>0) {
            EMparam->Gtest+=2*(EMparam->Ihwork[i]+EMparam->C4)*log((EMparam->Ihwork[i]+EMparam->C4)/(EMparam->Ithwork[i]+EMparam->C4)*sumIth/sumIh);
            EMparam->JSDtest+=0.5*(EMparam->Ihwork[i] +EMparam->C4)/sumIh *log((EMparam->Ihwork[i] +EMparam->C4)/sumIh /((EMparam->Ithwork[i]+EMparam->C4)/sumIth+(EMparam->Ihwork[i]+EMparam->C4)/sumIh)*2.);
            EMparam->JSDtest+=0.5*(EMparam->Ithwork[i]+EMparam->C4)/sumIth*log((EMparam->Ithwork[i]+EMparam->C4)/sumIth/((EMparam->Ithwork[i]+EMparam->C4)/sumIth+(EMparam->Ihwork[i]+EMparam->C4)/sumIh)*2.);
        }
        EMparam->chi2test+=gsl_pow_2(EMparam->Ihwork[i]-EMparam->Ithwork[i])/(EMparam->Ithwork[i]+EMparam->C4);
    }
    chi2=chi2/EMparam->nh;
    if (!gsl_finite(chi2)) chi2=DBL_MAX;
    EMparam->redchi2=chi2;
    return chi2;
}

double EM_DR_EntropyAdaptivePrior_Operator(void *EM_structure) {
    EM_param_t *EMparam;
    scalar sumIth, sumIh;
    EMparam = (EM_param_t *) EM_structure;
    int i,j,l, nr;
    scalar p1,p2,p3,PS, NormXwork, eps, chi2, S, nnlambda;
    nr=EMparam->nR;
    for (i=0;i<nr;i++) {
        EMparam->in[i] = fabs(EMparam->out[i]);
    }

    for (j=0;j<EMparam->nh;j++) {
        EMparam->Ithwork[j]=0;
        for (l=0;l<nr;l++) EMparam->Ithwork[j]=EMparam->Ithwork[j]+EMparam->dr[l]*EMparam->in[l]*EMparam->Awork[j][l];
    }

    for (i=0;i<nr;i++) {
        EMparam->prior[i]=0;
        for (j=0;j<nr;j++) {
            EMparam->prior[i]=EMparam->prior[i]+EMparam->in[j]*EMparam->S[i][j];
        }
        if (EMparam->prior[i] == 0) {
//                sasfit_out("i:%d prior:%lg\n",i,EMparam->prior[i]);
                EMparam->prior[i]=DBL_MIN;
        }
    }
    S=0;
    EMparam->Entropy=0;
    NormXwork=0;
    for (i=0;i<nr;i++) {
        if (EMparam->in[i]>0) {
            S = S+0*(EMparam->in[i]-EMparam->prior[i])-EMparam->in[i]*log(EMparam->in[i]/EMparam->prior[i]);
            EMparam->Entropy = EMparam->Entropy + (EMparam->in[i]-EMparam->prior[i])-EMparam->in[i]*log(EMparam->in[i]/EMparam->prior[i]);
            NormXwork=NormXwork+EMparam->in[i];
        } else {
            EMparam->Entropy = EMparam->Entropy + (EMparam->in[i]-EMparam->prior[i]);
        }
    }
    nnlambda = EMparam->overrelaxation*2-1;
    for (i=0;i<nr;i++) {
        p3=0;
        p1=0;
        for (j=0;j<EMparam->nh;j++) {
            p1=p1+EMparam->Awork[j][i]*EMparam->dh[j];
            p2=EMparam->Ihwork[j]*EMparam->Awork[j][i];
            p3=p3+p2*EMparam->dh[j]/EMparam->Ithwork[j];
        }
        PS=0;

        for (j=0;j<nr;j++) {
            PS=PS+EMparam->S[i][j]*EMparam->in[j]/EMparam->prior[j];
//            if (!gsl_finite(PS)) sasfit_out("j:%d, in:%lg, prior:%lg\n",j,EMparam->in[j],EMparam->prior[j]);
        }
        if (EMparam->in[i]>0) {
//            EMparam->out[i]=EMparam->in[i]+EMparam->in[i]*(p3/p1-1)
//                                      -EMparam->in[i]*EMparam->lambda*(S/NormXwork+log(EMparam->in[i]/EMparam->prior[i])+1-PS);
            EMparam->out[i]= EMparam->in[i]*(p3/p1-1)
                            -EMparam->in[i]*EMparam->lambda*(S/NormXwork+log(EMparam->in[i]/EMparam->prior[i])+1-PS);

        } else {
//            EMparam->out[i]=EMparam->in[i];
            EMparam->out[i]=0.0;
        }
        if (!gsl_finite(EMparam->out[i])) sasfit_out("i:%d, out:%lg, NormXwork:%lg S:%lg\n",j,EMparam->out[i],NormXwork,S);
        if (EMparam->out[i]<0 && EMparam->in[i]/fabs(EMparam->out[i])<nnlambda) nnlambda = EMparam->in[i]/fabs(EMparam->out[i]);
    }
    eps = 0;

    for (i=0;i<nr;i++) {
        EMparam->out[i] = EMparam->in[i] + (nnlambda+1)/2*EMparam->out[i];
        eps = eps+gsl_pow_2(EMparam->out[i]-EMparam->in[i]);
    }
    eps=sqrt(eps);
// sasfit_out("eps:%lg \n",eps);
    chi2=0;
    EMparam->Gtest=0;
    EMparam->JSDtest=0;
    EMparam->chi2test=0;
    sumIth=0;
    sumIh=0;
    for (i=0;i<EMparam->nh;i++){
        EMparam->Ithwork[i] = 0;
        for (j=0;j<nr;j++) {
            EMparam->Ithwork[i] = EMparam->Ithwork[i] + EMparam->dr[j]*EMparam->Awork[i][j]*EMparam->out[j];
        }
        if (!gsl_finite(EMparam->Ithwork[i])) EMparam->Ithwork[i]=gsl_isinf(EMparam->Ithwork[i])*DBL_MAX;
        sumIth+=EMparam->Ithwork[i]+EMparam->C4;
        sumIh +=EMparam->Ihwork[i] +EMparam->C4;
        chi2=chi2+gsl_pow_2((EMparam->Ithwork[i]-EMparam->Ihwork[i])/EMparam->DIh[i]*(1+EMparam->em_weight*(EMparam->DIh[i]-1)));
    }
    for (i=0;i<EMparam->nh;i++){
        if ((EMparam->Ithwork[i]+EMparam->C4 > 0)&& (EMparam->Ihwork[i]+EMparam->C4)>0) {
            EMparam->Gtest+=2*(EMparam->Ihwork[i]+EMparam->C4)*log((EMparam->Ihwork[i]+EMparam->C4)/(EMparam->Ithwork[i]+EMparam->C4)*sumIth/sumIh);
            EMparam->JSDtest+=0.5*(EMparam->Ihwork[i] +EMparam->C4)/sumIh *log((EMparam->Ihwork[i] +EMparam->C4)/sumIh /((EMparam->Ithwork[i]+EMparam->C4)/sumIth+(EMparam->Ihwork[i]+EMparam->C4)/sumIh)*2.);
            EMparam->JSDtest+=0.5*(EMparam->Ithwork[i]+EMparam->C4)/sumIth*log((EMparam->Ithwork[i]+EMparam->C4)/sumIth/((EMparam->Ithwork[i]+EMparam->C4)/sumIth+(EMparam->Ihwork[i]+EMparam->C4)/sumIh)*2.);
        }
        EMparam->chi2test+=gsl_pow_2(EMparam->Ihwork[i]-EMparam->Ithwork[i])/(EMparam->Ithwork[i]+EMparam->C4);
    }
    chi2=chi2/EMparam->nh;
    if (!gsl_finite(chi2)) chi2=DBL_MAX;
    EMparam->redchi2=chi2;
    return chi2;
}


void EM_DR_Free (void *FPd) {
    EM_param_t *EMparam;
    sasfit_fp_data *FixedPointData;
    FixedPointData = (sasfit_fp_data *)FPd;
    EMparam = (EM_param_t *) FixedPointData->FPstructure;
    FP_free(FixedPointData);
    free_dvector(EMparam->h,0,EMparam->nh-1);
    free_dvector(EMparam->Ih,0,EMparam->nh-1);
    free_dvector(EMparam->Ihred,0,EMparam->nh-1);
    free_dvector(EMparam->DIh,0,EMparam->nh-1);
    free_dvector(EMparam->Ith,0,EMparam->nh-1);
    free_dvector(EMparam->Ithred,0,EMparam->nh-1);
    free_dvector(EMparam->dh,0,EMparam->nh-1);
    free_dvector(EMparam->Iexp,0,EMparam->nh-1);
    free_dvector(EMparam->r,0,EMparam->nR-1);
    free_dvector(EMparam->dr,0,EMparam->nR-1);
    free_dvector(EMparam->xwork,0,EMparam->nR-1);
    free_dvector(EMparam->prior,0,EMparam->nR-1);
    free_dvector(EMparam->out,0,EMparam->nR-1);
    free_dvector(EMparam->in,0,EMparam->nR-1);
    free_dmatrix(EMparam->A,0,EMparam->nh-1,0,EMparam->nR-1);
    free_dmatrix(EMparam->Ared,0,EMparam->nh-1,0,EMparam->nR-1);
    free_dmatrix(EMparam->S,0,EMparam->nR-1,0,EMparam->nR-1);
    gsl_vector_free(EMparam->gsl_x);
    gsl_vector_free(EMparam->gsl_dx);
    gsl_vector_free(EMparam->gsl_b);
    gsl_vector_free(EMparam->gsl_S);
    gsl_vector_free(EMparam->gsl_dS);
    gsl_matrix_free(EMparam->gsl_A);
}

void EM_DR_Init (void *FPd) {
    scalar rmax, QR,Snorm;
    int nr,nh;
    int i,j;
    int smooth_int;
    EM_param_t *EMparam;
    sasfit_fp_data *FixedPointData;
    const gsl_rng_type * T;
    gsl_rng * rgen;

    gsl_rng_env_setup();
    T = gsl_rng_default;
    rgen = gsl_rng_alloc (T);

    FixedPointData = (sasfit_fp_data *)FPd;
    EMparam = (EM_param_t *) FixedPointData->FPstructure;


    EMparam->Dmax = M_PI/EMparam->h[0];
    EMparam->Nshannon = lround(EMparam->h[EMparam->nh-1]/EMparam->h[0]-1);
    EMparam->Delta_r = 0.5*EMparam->Dmax/EMparam->Nshannon;

    nh=EMparam->nh;

    if (EMparam->nR <=0) {
        EMparam->nR=EMparam->Nshannon;
        nr=EMparam->nR;
    } else {
        nr=EMparam->nR;
    }

    FixedPointData->Npoints=EMparam->nR;
    FixedPointData->maxsteps=EMparam->maxit;
    FixedPointData->relerror=EMparam->eps;
    FixedPointData->failed = 0;
    FixedPointData->interrupt = 0;
    FP_init(FixedPointData);
    FixedPointData->root_algorithm=EMparam->root_algorithm;
    strcpy(FixedPointData->Tcl_Array_Name,"EMOptions");
    EMparam->xwork   = dvector(0,EMparam->nR-1);
    EMparam->prior   = dvector(0,EMparam->nR-1);
    EMparam->r   = dvector(0,EMparam->nR-1);
    EMparam->dr   = dvector(0,EMparam->nR-1);
    EMparam->out   = dvector(0,EMparam->nR-1);
    EMparam->in   = dvector(0,EMparam->nR-1);
    FixedPointData->in = EMparam->in;
    FixedPointData->out = EMparam->out;
    EMparam->gsl_x=gsl_vector_calloc(EMparam->nR);
    EMparam->gsl_dx=gsl_vector_calloc(EMparam->nR);
    EMparam->gsl_S=gsl_vector_calloc(EMparam->nR);
    EMparam->gsl_dS=gsl_vector_calloc(EMparam->nR);
    EMparam->gsl_b=gsl_vector_calloc(EMparam->nh);
    EMparam->gsl_A=gsl_matrix_calloc(EMparam->nh,EMparam->nR);
    FixedPointData->DR = EMparam->gsl_x;
    EMparam->Ith   = dvector(0,EMparam->nh-1);
    EMparam->Ithred   = dvector(0,EMparam->nh-1);
    EMparam->Ithwork   = EMparam->Ithred;
    EMparam->Ih   = dvector(0,EMparam->nh-1);
    EMparam->Ihred   = dvector(0,EMparam->nh-1);
    EMparam->Ihwork   = EMparam->Ihred;
    EMparam->dh   = dvector(0,EMparam->nh-1);
    EMparam->A = dmatrix(0,EMparam->nh-1,0,EMparam->nR-1);
    EMparam->Ared = dmatrix(0,EMparam->nh-1,0,EMparam->nR-1);
    EMparam->Awork = EMparam->Ared;
    EMparam->S = dmatrix(0,EMparam->nR-1,0,EMparam->nR-1);

    for (i=0;i<nh;i++){
        if (i==0) {
            EMparam->dh[0]=EMparam->h[0];
        } else {
            EMparam->dh[i] = EMparam->h[i]-EMparam->h[i-1];
        }
    }
    rmax = EMparam->Rmax;
    if (rmax <= 0) {
        rmax = EMparam->Dmax/2.0;
        EMparam->Rmax = rmax;
    }
    for (j=0;j<nr;j++) {
            EMparam->r[j] = rmax/nr*(j+1);
            EMparam->prior[j]=1./nr;
    }

    gsl_rng_set(rgen, time(NULL));
    gsl_rng_set(rgen, 815);
    if (strcmp("single",EMparam->smooth_type)==0) {
        smooth_int = 0;
        EMparam->smooth_bool = FALSE;
    } else if (strcmp("double",EMparam->smooth_type)==0) {
        smooth_int = 1;
        EMparam->smooth_bool = TRUE;
    } else {
        smooth_int = 2;
        EMparam->smooth_bool = TRUE;
    }

    // sasfit_out("%d %s\n",smooth_type,EMparam->smooth_type);

    if (strcmp("constant",EMparam->seed)==0) {
        for (i=0;i<nr;i++) {
            EMparam->xwork[i] = 1e-7;
            EMparam->out[i] = 1e-7;
            EMparam->in[i] = 1e-7;
            EMparam->dr[i]=rmax/nr;
        }
    } else {
        for (i=0;i<nr;i++) {
            EMparam->xwork[i] = gsl_rng_uniform (rgen)*1e-7;
            EMparam->out[i] = gsl_rng_uniform (rgen)*1e-7;
            EMparam->in[i] = gsl_rng_uniform (rgen)*1e-7;
            EMparam->dr[i]=rmax/nr;
        }
    }
    gsl_rng_free (rgen);

    for (i=0;i<nr;i++) {
        gsl_vector_set(EMparam->gsl_x,i,EMparam->in[i]);
        gsl_vector_set(EMparam->gsl_dx,i,EMparam->dr[i]);
        for (j=0;j<nr;j++) {
            EMparam->S[i][j]=0;
        }
    }

//sasfit_out("smooth_int %d\n",smooth_int);
    switch (smooth_int) {
        case 0:
        case 1:
            EMparam->S[0][0]=1-EMparam->smooth;
            EMparam->S[0][1]=EMparam->smooth;
            for (i=1;i<nr-1;i++) {
                EMparam->S[i][i-1]=EMparam->smooth;
                EMparam->S[i][i]=1-2*EMparam->smooth;
                EMparam->S[i][i+1]=EMparam->smooth;
            }
            EMparam->S[nr-1][nr-2]=EMparam->smooth;
            EMparam->S[nr-1][nr-1]=1-EMparam->smooth;
            break;
        default: {
            for (i=0;i<nr;i++) {
                Snorm=0;
                for (j=0;j<nr;j++) {
                    EMparam->S[i][j]=exp(-0.5*gsl_pow_2((i-j)/EMparam->smooth));
                    Snorm = Snorm + exp(-0.5*gsl_pow_2((i-j)/EMparam->smooth));
                }
                for (j=0;j<nr;j++) {
                    EMparam->S[i][j]=EMparam->S[i][j]/Snorm;
                }
            }
        }
    }

    for (i=0;i<nh;i++){
        for (j=0;j<nr;j++) {
            QR=EMparam->h[i]*EMparam->r[j];
 //       A[i][j] = gsl_sf_bessel_j0(h[i]*r[j]);
            EMparam->A[i][j] = gsl_pow_2(4*M_PI*gsl_pow_3(EMparam->r[j])*(sin(QR)-QR*cos(QR))/gsl_pow_3(QR))*pow(EMparam->r[j],-EMparam->dim);
            EMparam->Ared[i][j] = EMparam->A[i][j]/EMparam->DIh[i];
            gsl_matrix_set(EMparam->gsl_A,i,j,EMparam->dr[j]*EMparam->Ared[i][j]);
        }
        EMparam->Ih[i] = EMparam->Iexp[i]-EMparam->C0;
        EMparam->Ihred[i] = EMparam->Ih[i]/ EMparam->DIh[i];
        gsl_vector_set(EMparam->gsl_b,i, EMparam->Ih[i]/ EMparam->DIh[i]);

        EMparam->Ith[i] =0;
        EMparam->Ithred[i] =0;
    }
    if (EMparam->em_weight==1) {
        EMparam->Awork=EMparam->Ared;
        EMparam->Ithwork=EMparam->Ithred;
        EMparam->Ihwork=EMparam->Ihred;
    } else {
        EMparam->Awork=EMparam->A;
        EMparam->Ithwork=EMparam->Ith;
        EMparam->Ihwork=EMparam->Ih;
    }



    FixedPointData->KINSetMAA = EMparam->maxKrylov;
    FixedPointData->KINSetFuncNormTol = EMparam->eps;
    FixedPointData->KINSetScaledSteptol = 0;
    FixedPointData->KINSetNumMaxIters = EMparam->maxit;
    FixedPointData->KINSetMaxNewtonStep = FixedPointData->Npoints*500.0;
    FixedPointData->KINSetPrintLevel = 0;
//    FixedPointData->PrintProgress = FixedPointData->KINSetPrintLevel;
    FixedPointData->KINSetEtaForm=KIN_ETACHOICE2; // KIN ETACHOICE1, KIN ETACHOICE2, or KIN ETACONSTANT
    FixedPointData->KINSetEtaConstValue = 0.1;
    FixedPointData->KINSpilsSetMaxRestarts =20;
    FixedPointData->KINSolStrategy=KIN_NONE;
    FixedPointData->KINConstraints = N_VNew_Serial(FixedPointData->Npoints);
    N_VConst_Serial(0., FixedPointData->KINConstraints);
}

double MuCh_DR_Operator(void *EM_structure) {

    EM_param_t *EMparam;
    EMparam = (EM_param_t *)EM_structure;
    int i,j,l, nr;
    scalar p1,p2,p3, eps, chi2;
/*
    sasfit_out("EP_Operator: in: ");
    for (i=0;i<10;i++) sasfit_out("%lg ",EMparam->in[i]);
    sasfit_out("\n");
    sasfit_out("EP_Operator: out(start): ");
    for (i=0;i<10;i++) sasfit_out("%lg ",EMparam->out[i]);
    sasfit_out("\n");
*/
    nr=EMparam->nR;


    if ((EMparam->smooth > 0) && EMparam->smooth_bool) {
        for (i=0;i<nr;i++) EMparam->in[i] = fabs(EMparam->out[i]);
        EMparam->xwork[0]=exp(log(EMparam->in[0])*EMparam->S[0][0]+log(EMparam->in[1])*EMparam->S[0][1]);
        for (i=1;i<nr-1;i++) {
            EMparam->xwork[i]=exp(log(EMparam->in[i-1])*EMparam->S[i][i-1]+log(EMparam->in[i])*EMparam->S[i][i]+log(EMparam->in[i+1])*EMparam->S[i][i+1]);
        }
        EMparam->xwork[nr-1]=exp(log(EMparam->in[nr-2])*EMparam->S[nr-1][nr-2]+log(EMparam->in[nr-1])*EMparam->S[nr-1][nr-1]);
    } else {
        for (i=0;i<nr;i++) {
            EMparam->in[i] = EMparam->out[i];
            EMparam->xwork[i] = EMparam->out[i];
        }
    }
/*
    sasfit_out("EP_Operator: work (1st smooth): ");
    for (i=0;i<10;i++) sasfit_out(" %lg ",EMparam->xwork[i]);
    sasfit_out("\n");
*/

//    sasfit_out("calculating {Ih-T1[DR]}*Q^4-C4\n");
    for (j=0;j<EMparam->nh;j++) {
        EMparam->Ith[j]=0;
        for (l=0;l<nr;l++) EMparam->Ith[j]=EMparam->Ith[j]+EMparam->dr[l]*EMparam->xwork[l]*EMparam->A[j][l];
        EMparam->Ih4mC4[j]=(EMparam->Ih[j]-EMparam->Ith[j])*gsl_pow_4(EMparam->h[j])-1*EMparam->C4;
    }

//    sasfit_out("applying T3\n");
    for (i=0;i<EMparam->nR;i++) {
        EMparam->xwork[i]=0;
        for (j=0;j<EMparam->nh;j++) {
            EMparam->xwork[i]=EMparam->xwork[i]+EMparam->T3[i][j]*EMparam->dh[j]/gsl_pow_2(EMparam->r[i])*EMparam->Ih4mC4[j];
        }
        EMparam->xwork[i]=EMparam->in[i]+EMparam->xwork[i];
/*
        if (EMparam->xwork[i]+EMparam->in[i]>0) {
            EMparam->xwork[i]=EMparam->in[i]+EMparam->xwork[i];
        } else {
            EMparam->xwork[i]=EMparam->in[i];
        }
*/
    }
//    EMparam->xwork[0]=0;
/*
    sasfit_out("EP_Operator: work (after T3 Operator): ");
    for (i=0;i<10;i++) sasfit_out(" %lg ",EMparam->xwork[i]);
    sasfit_out("\n");
*/
    EMparam->out[0]=(EMparam->xwork[0]*EMparam->S[0][0]+EMparam->xwork[0]*EMparam->S[0][1]);
    eps = gsl_pow_2(EMparam->out[0]-EMparam->in[0]);
    for (i=1;i<nr-1;i++) {
        EMparam->out[i]=(EMparam->xwork[i-1]*EMparam->S[i][i-1]+EMparam->xwork[i]*EMparam->S[i][i]+EMparam->xwork[i+1]*EMparam->S[i][i+1]);
        eps = eps+gsl_pow_2(EMparam->out[i]-EMparam->in[i]);
    }
    EMparam->out[nr-1]=(EMparam->xwork[nr-2]*EMparam->S[nr-1][nr-2]+EMparam->xwork[nr-1]*EMparam->S[nr-1][nr-1]);


//    sasfit_out("EP_Operator: out (2nd smooth): ");
//    for (i=0;i<10;i++) sasfit_out("%lg ",EMparam->out[i]);
//    sasfit_out("\n");

    eps = eps+gsl_pow_2(EMparam->out[nr-1]-EMparam->in[nr-1]);
    eps=sqrt(eps);

    chi2=0;

    for (i=0;i<EMparam->nh;i++){
        EMparam->Ith[i] = 0;
        for (j=0;j<nr;j++) {
            EMparam->Ith[i] = EMparam->Ith[i] + EMparam->dr[j]*EMparam->A[i][j]*EMparam->out[j];
        }
        chi2=chi2+gsl_pow_2((EMparam->Ith[i]-EMparam->Ih[i])/EMparam->DIh[i]);
    }
    chi2=chi2/EMparam->nh;
 //   sasfit_out("chi2:%lg\n",chi2);
 //   sasfit_out("\n");
    return chi2;
}

double SDM_DR_Operator(void *EM_structure) {

    EM_param_t *EMparam;
    EMparam = (EM_param_t *)EM_structure;
    int i,j,l, nr;
    scalar *W;
    scalar p1,p2,p3, eps, chi2, DeltaI, trans;
    const gsl_rng_type * T;
    gsl_rng * rgen;

    gsl_rng_env_setup();
    T = gsl_rng_default;
    rgen = gsl_rng_alloc (T);
    gsl_rng_set(rgen, time(NULL));
    gsl_interp_accel *acc;
    gsl_spline *spline;

    W =  dvector(0,EMparam->nh-1);
/*
    sasfit_out("EP_Operator: in: ");
    for (i=0;i<10;i++) sasfit_out("%lg ",EMparam->in[i]);
    sasfit_out("\n");
    sasfit_out("EP_Operator: out(start): ");
    for (i=0;i<10;i++) sasfit_out("%lg ",EMparam->out[i]);
    sasfit_out("\n");
*/
    nr=EMparam->nR;
    if ((EMparam->smooth > 0) && EMparam->smooth_bool) {
        for (i=0;i<nr;i++) EMparam->in[i] = fabs(EMparam->out[i]);
        EMparam->in[0]=0;
        EMparam->xwork[0]=exp(log(EMparam->in[0])*EMparam->S[0][0]+log(EMparam->in[1])*EMparam->S[0][1]);
        for (i=1;i<nr-1;i++) {
            EMparam->xwork[i]=exp(log(EMparam->in[i-1])*EMparam->S[i][i-1]+log(EMparam->in[i])*EMparam->S[i][i]+log(EMparam->in[i+1])*EMparam->S[i][i+1]);
        }
        EMparam->xwork[nr-1]=exp(log(EMparam->in[nr-2])*EMparam->S[nr-1][nr-2]+log(EMparam->in[nr-1])*EMparam->S[nr-1][nr-1]);
    } else {
        for (i=0;i<nr;i++) {
            EMparam->in[i] = EMparam->out[i];
            EMparam->xwork[i] = EMparam->out[i];
        }
    }
/*
    sasfit_out("EP_Operator: work (1st smooth): ");
    for (i=0;i<10;i++) sasfit_out(" %lg ",EMparam->xwork[i]);
    sasfit_out("\n");
*/

//    sasfit_out("calculating {Ih-T1[DR]}*Q^4-C4\n");
    for (j=0;j<EMparam->nh;j++) {
        EMparam->Ith[j]=0;
        for (l=0;l<nr;l++) EMparam->Ith[j]=EMparam->Ith[j]+EMparam->dr[l]*EMparam->xwork[l]*EMparam->A[j][l];
    }
    for (j=0;j<EMparam->nh;j++) {
        if (fabs(EMparam->Ith[j]) > 0 ) {
            W[j]=(EMparam->Ih[j]-EMparam->Ith[j])/EMparam->Ith[j];
        } else {
            W[j]=1;
        }
    }
    acc = gsl_interp_accel_alloc ();
    spline = gsl_spline_alloc (gsl_interp_linear, EMparam->nh);
    gsl_spline_init (spline, EMparam->h, W, EMparam->nh);
//    sasfit_out("applying T3\n");
    for (i=0;i<EMparam->nR;i++) {
        if (M_PI/EMparam->r[i] < EMparam->h[0]) {
            p1=gsl_spline_eval(spline, EMparam->h[0], acc);
        } else if (M_PI/EMparam->r[i] > EMparam->h[EMparam->nh-1]) {
            p1=gsl_spline_eval(spline, EMparam->h[EMparam->nh-1], acc);
        } else {
            p1=gsl_spline_eval(spline, M_PI/EMparam->r[i], acc);
        }
        EMparam->xwork[i]=EMparam->xwork[i]*(1.0+(0+1*gsl_rng_uniform(rgen))*p1);

/*
        if (fabs(EMparam->Ith[EMparam->nR-1-i]) > 0 ) {
            DeltaI = (EMparam->Ih[EMparam->nR-1-i]-EMparam->Ith[EMparam->nR-1-i]);
            trans = fabs(atan(DeltaI/EMparam->DIh[EMparam->nR-1-i])/M_PI_2);
            EMparam->xwork[i]=EMparam->in[i]*(1.0+(trans+(0.2+0.8*(gsl_rng_uniform(rgen)-0.2/0.8))*(1-trans))*DeltaI/EMparam->Ith[EMparam->nR-1-i]);
        } else {
            EMparam->xwork[i]=EMparam->in[i];
        }
*/
    }
    gsl_rng_free(rgen);
    gsl_spline_free(spline);
    gsl_interp_accel_free(acc);
//    EMparam->xwork[0]=0;

/*
    sasfit_out("EP_Operator: work (after SDM Operator): ");
    for (i=0;i<10;i++) sasfit_out(" %lg ",EMparam->xwork[i]);
    sasfit_out("\n");
*/
    EMparam->out[0]=(EMparam->xwork[0]*EMparam->S[0][0]+EMparam->xwork[0]*EMparam->S[0][1]);
    EMparam->out[0]=0;
    eps = gsl_pow_2(EMparam->out[0]-EMparam->in[0]);
    for (i=1;i<nr-1;i++) {
        EMparam->out[i]=(EMparam->xwork[i-1]*EMparam->S[i][i-1]+EMparam->xwork[i]*EMparam->S[i][i]+EMparam->xwork[i+1]*EMparam->S[i][i+1]);
        eps = eps+gsl_pow_2(EMparam->out[i]-EMparam->in[i]);
    }
    EMparam->out[nr-1]=0;


//    sasfit_out("EP_Operator: out (2nd smooth): ");
//    for (i=0;i<10;i++) sasfit_out("%lg ",EMparam->out[i]);
//    sasfit_out("\n");

    eps = eps+gsl_pow_2(EMparam->out[nr-1]-EMparam->in[nr-1]);
    eps=sqrt(eps);

    chi2=0;

    for (i=0;i<EMparam->nh;i++){
        EMparam->Ith[i] = 0;
        for (j=0;j<nr;j++) {
            EMparam->Ith[i] = EMparam->Ith[i] + EMparam->dr[j]*EMparam->A[i][j]*EMparam->out[j];
        }
        chi2=chi2+gsl_pow_2((EMparam->Ith[i]-EMparam->Ih[i])/EMparam->DIh[i]);
    }
    chi2=chi2/EMparam->nh;
 //   sasfit_out("chi2:%lg\n",chi2);
 //   sasfit_out("\n");
    return chi2;
}

void MuCh_DR_Free (void *FPd) {
    EM_param_t *EMparam;
    sasfit_fp_data *FixedPointData;
    FixedPointData = (sasfit_fp_data *)FPd;
    EMparam = (EM_param_t *) FixedPointData->FPstructure;
    EM_DR_Free(FPd);
    free_dmatrix(EMparam->T3,0,EMparam->nR-1,0,EMparam->nh-1);
    free_dvector(EMparam->Ih4mC4,0,EMparam->nh-1);
}

scalar alpha_MuCh(scalar QR) {
    scalar QR2;
    QR2=QR*QR;
    if (QR2 > 0) return (1. - 2./QR2)*cos(2*QR) - (2. - 1./QR2)*sin(2*QR)/QR;
    return GSL_POSINF;
}

void MuCh_DR_Init (void *FPd) {
    scalar QR;
    int i,j;
    EM_param_t *EMparam;
    sasfit_fp_data *FixedPointData;
    EM_DR_Init(FPd);

    FixedPointData = (sasfit_fp_data *)FPd;
    EMparam = (EM_param_t *) FixedPointData->FPstructure;

    EMparam->T3 = dmatrix(0,EMparam->nR-1,0,EMparam->nh-1);
    EMparam->Ih4mC4 =  dvector(0,EMparam->nh-1);
    for (i=0;i<EMparam->nR;i++){
        for (j=0;j<EMparam->nh;j++) {
            QR=EMparam->h[j]*EMparam->r[i];
            EMparam->T3[i][j] = alpha_MuCh(QR);
        }
    }
    for (j=0;j<EMparam->nR;j++) {
        EMparam->xwork[j] = 0;
        EMparam->out[j] = 0;
        EMparam->in[j] = 0;
    }
}

void SDM_DR_Free (void *FPd) {
    EM_param_t *EMparam;
    sasfit_fp_data *FixedPointData;
    FixedPointData = (sasfit_fp_data *)FPd;
    EMparam = (EM_param_t *) FixedPointData->FPstructure;
    EM_DR_Free(FPd);
}

void SDM_DR_Init (void *FPd) {
    scalar QR, rmax, rmin;
    int i,j, nh, nr;
    EM_param_t *EMparam;
    sasfit_fp_data *FixedPointData;

    FixedPointData = (sasfit_fp_data *)FPd;
    EMparam = (EM_param_t *) FixedPointData->FPstructure;

    EM_DR_Init(FPd);

    nh = EMparam->nh;
    nr = EMparam->nR;
    rmax = M_PI/EMparam->h[0];
    rmin = M_PI/EMparam->h[nh-1];
    EMparam->Rmax = rmax;

    for (j=0;j<nr;j++) {
        EMparam->r[j] = rmin+j*(rmax-rmin)/(nr-1.0);
        if (j==0) {
            EMparam->dr[0] = rmin;
        } else {
            EMparam->dr[j] = EMparam->r[j] - EMparam->r[j-1];
        }
    }
    for (i=0;i<nh;i++){
        for (j=0;j<nr;j++) {
            QR=EMparam->h[i]*EMparam->r[j];
            EMparam->A[i][j] = gsl_pow_2(4*M_PI*gsl_pow_3(EMparam->r[j])*(sin(QR)-QR*cos(QR))/gsl_pow_3(QR))*pow(EMparam->r[j],-EMparam->dim);
        }
    }
}

double Optimum_smooth4DR_EM(double smooth, void *FPd ) {

    EM_param_t *EMparam;
    sasfit_fp_data *FixedPointData;
    int i,j, nr;
    scalar Snorm;

    FixedPointData = (sasfit_fp_data *)FPd;
    EMparam = (EM_param_t *) FixedPointData->FPstructure;

    nr = EMparam->nR;
    EMparam->smooth = smooth;
    EMparam->S[0][0]=1-EMparam->smooth;
    EMparam->S[0][1]=EMparam->smooth;
    for (i=1;i<nr-1;i++) {
        EMparam->S[i][i]=1-2*EMparam->smooth;
        EMparam->S[i][i-1]=EMparam->smooth;
        EMparam->S[i][i+1]=EMparam->smooth;
    }
    EMparam->S[nr-1][nr-2]=EMparam->smooth;
    EMparam->S[nr-1][nr-1]=1-EMparam->smooth;

    FixedPointData->it=0;
    FP_solver(FixedPointData);

    sasfit_out("EM_smooth --- it:%d, smooth:%lg, chi2:%lg, set red. chi2:%lg\n",FixedPointData->it, smooth, FixedPointData->Chi2Norm, EMparam->chi2);
    return FixedPointData->Chi2Norm-EMparam->chi2;

}


double Optimum_lambda4DR_EM_ME(double lambda, void *FPd ) {

    EM_param_t *EMparam;
    sasfit_fp_data *FixedPointData;
    int i, nr;

    FixedPointData = (sasfit_fp_data *)FPd;
    EMparam = (EM_param_t *) FixedPointData->FPstructure;

    FixedPointData->it=0;
    EMparam->lambda = lambda;

    FP_solver(FixedPointData);

//    sasfit_out("EM_ME --- it:%d, lambda:%lg, chi2:%lg set:%lg, set red. chi2: %lg\n",FixedPointData->it, lambda, FixedPointData->Chi2Norm, EMparam->chi2, EMparam->redchi2);
    return FixedPointData->Chi2Norm-EMparam->chi2;

}

/*#########################################################################*/
/*#                                                                       #*/
/*# Sasfit_DR_EM_Cmd --                                                   #*/
/*#                                                                       #*/
/*#      This function implements the Tcl "sasfit_DR_EM_Cmd" command      #*/
/*#                                                                       #*/
/*# Results:                                                              #*/
/*#      A standard Tcl result.                                           #*/
/*#                                                                       #*/
/*# Side effects:                                                         #*/
/*#      None.                                                            #*/
/*#                                                                       #*/
/*#########################################################################*/
double menger_C(double xj, double xk, double xl, double yj, double yk, double yl) {
    double Pjk, Pkl, Plj, Ck;
    Pjk = gsl_hypot(xk-xj, yk-yj);
    Pkl = gsl_hypot(xl-xk, yl-yk);
    Plj = gsl_hypot(xj-xl, yj-yl);
    if (Pjk*Pkl*Plj == 0) return GSL_POSINF;
    Ck = 2*(xj*yk+xk*yl+xl*yj-xj*yl-xk*yj-xl*yk)/ (Pjk*Pkl*Plj);
    if (gsl_finite(Ck)) {
        return Ck;
    } else {
        return 0;
    }
}

double menger_r(double xj, double xk, double xl, double yj, double yk, double yl) {
    double Pjk, Pkl, Plj, Ck, r;
    Pjk = gsl_hypot(xk-xj, yk-yj);
    Pkl = gsl_hypot(xl-xk, yl-yk);
    Plj = gsl_hypot(xj-xl, yj-yl);
    Ck = 2*(xj*yk+xk*yl+xl*yj-xj*yl-xk*yj-xl*yk);
    if (Ck == 0) return GSL_DBL_MAX;
    return (Pjk*Pkl*Plj)/Ck;
}

int
sasfit_gsl_multifit_linear_lcurve (const gsl_vector * y,
                            gsl_vector * reg_param,
                            gsl_vector * rho, gsl_vector * eta,
                            double lmin,
                            double lmax,
                            gsl_multifit_linear_workspace * work)
{
  const size_t n = y->size;
  const size_t N = rho->size; /* number of points on L-curve */

  if (n != work->n)
    {
      GSL_ERROR("y vector does not match workspace", GSL_EBADLEN);
    }
  else if (N < 3)
    {
      GSL_ERROR ("at least 3 points are needed for L-curve analysis",
                 GSL_EBADLEN);
    }
  else if (N != eta->size)
    {
      GSL_ERROR ("size of rho and eta vectors do not match",
                 GSL_EBADLEN);
    }
  else if (reg_param->size != eta->size)
    {
      GSL_ERROR ("size of reg_param and eta vectors do not match",
                 GSL_EBADLEN);
    }
  else
    {
      int status = GSL_SUCCESS;
      const size_t p = work->p;

      size_t i, j;

      gsl_matrix_view A = gsl_matrix_submatrix(work->A, 0, 0, n, p);
      gsl_vector_view S = gsl_vector_subvector(work->S, 0, p);
      gsl_vector_view xt = gsl_vector_subvector(work->xt, 0, p);
      gsl_vector_view workp = gsl_matrix_subcolumn(work->QSI, 0, 0, p);
      gsl_vector_view workp2 = gsl_vector_subvector(work->D, 0, p); /* D isn't used for regularized problems */

      double smax = gsl_vector_get(&S.vector, 0);
      double smin = gsl_vector_get(&S.vector, p - 1);
      if (lmax > lmin && lmin > 0) {
        smax = lmax;
        smin = lmin;
      }
      double dr; /* residual error from projection */
      double normy = gsl_blas_dnrm2(y);
      double normUTy;

      /* compute projection xt = U^T y */
      gsl_blas_dgemv (CblasTrans, 1.0, &A.matrix, y, 0.0, &xt.vector);

      normUTy = gsl_blas_dnrm2(&xt.vector);
      dr = normy*normy - normUTy*normUTy;

      /* calculate regularization parameters */
      gsl_multifit_linear_lreg(smin, smax, reg_param);

      for (i = 0; i < N; ++i)
        {
          double lambda = gsl_vector_get(reg_param, i);
          double lambda_sq = lambda * lambda;

          for (j = 0; j < p; ++j)
            {
              double sj = gsl_vector_get(&S.vector, j);
              double xtj = gsl_vector_get(&xt.vector, j);
              double f = sj / (sj*sj + lambda_sq);

              gsl_vector_set(&workp.vector, j, f * xtj);
              gsl_vector_set(&workp2.vector, j, (1.0 - sj*f) * xtj);
            }

          gsl_vector_set(eta, i, gsl_blas_dnrm2(&workp.vector));
          gsl_vector_set(rho, i, gsl_blas_dnrm2(&workp2.vector));
        }

      if (n > p && dr > 0.0)
        {
          /* add correction to residual norm (see eqs 6-7 of [1]) */
          for (i = 0; i < N; ++i)
            {
              double rhoi = gsl_vector_get(rho, i);
              double *ptr = gsl_vector_ptr(rho, i);

              *ptr = sqrt(rhoi*rhoi + dr);
            }
        }

      /* restore D to identity matrix */
      gsl_vector_set_all(work->D, 1.0);

      return status;
    }
} /* gsl_multifit_linear_lcurve() */

int
sasfit_gsl_multifit_linear_corner_golden_rule (const gsl_vector * y,
                            gsl_vector * reg_param,
                            gsl_vector * rho, gsl_vector * eta,
                            double lmin,
                            double lmax,
                            double *lopt,
                            gsl_multifit_linear_workspace * work)
{
  const size_t n = y->size, maxDO1=100, maxDO2=100;
  size_t DO1, DO2;
  const size_t N = rho->size; /* number of points on L-curve */
  double l[4],x[4],Px[4],Py[4],C[4], epsterm=0.01,phi;
  double lambda, lambda_sq;
  phi=(1.0 + sqrt(5))/2.0;

  if (n != work->n)
    {
      GSL_ERROR("y vector does not match workspace", GSL_EBADLEN);
    }
  else if (N < 3)
    {
      GSL_ERROR ("at least 3 points are needed for L-curve analysis",
                 GSL_EBADLEN);
    }
  else if (N != eta->size)
    {
      GSL_ERROR ("size of rho and eta vectors do not match",
                 GSL_EBADLEN);
    }
  else if (reg_param->size != eta->size)
    {
      GSL_ERROR ("size of reg_param and eta vectors do not match",
                 GSL_EBADLEN);
    }
  else
    {
      int status = GSL_SUCCESS;
      const size_t p = work->p;

      size_t i, j;

      gsl_matrix_view A = gsl_matrix_submatrix(work->A, 0, 0, n, p);
      gsl_vector_view S = gsl_vector_subvector(work->S, 0, p);
      gsl_vector_view xt = gsl_vector_subvector(work->xt, 0, p);
      gsl_vector_view workp = gsl_matrix_subcolumn(work->QSI, 0, 0, p);
      gsl_vector_view workp2 = gsl_vector_subvector(work->D, 0, p); /* D isn't used for regularized problems */

      double smax = gsl_vector_get(&S.vector, 0);
      double smin = gsl_vector_get(&S.vector, p - 1);
      sasfit_out("---------- suggested range for lambda from %lg to %lg ++++++++++\n",smin,smax);
      if (lmax > lmin && lmin > 0) {
        smax = lmax;
        smin = lmin;
      }
      l[0]=smin*smin;
      l[3]=smax*smax;
      x[0] = log10(l[0]);
      x[3] = log10(l[3]);
      x[1]=(x[3]+phi*x[0])/(1+phi);
      l[1]=pow(10,x[1]);
      x[2] = x[0]+(x[3]-x[1]);
      l[2]=pow(10,x[2]);
      double dr; /* residual error from projection */
      double normy = gsl_blas_dnrm2(y);
      double normUTy;

      /* compute projection xt = U^T y */
      gsl_blas_dgemv (CblasTrans, 1.0, &A.matrix, y, 0.0, &xt.vector);

      normUTy = gsl_blas_dnrm2(&xt.vector);
      dr = normy*normy - normUTy*normUTy;

      /* calculate regularization parameters */

#define L_CURVE_P \
          lambda_sq = lambda * lambda;\
          for (j = 0; j < p; ++j) { \
              double sj = gsl_vector_get(&S.vector, j); \
              double xtj = gsl_vector_get(&xt.vector, j); \
              double f = sj / (sj*sj + lambda_sq); \
              gsl_vector_set(&workp.vector, j, f * xtj); \
              gsl_vector_set(&workp2.vector, j, (1.0 - sj*f) * xtj); \
          } \
          gsl_vector_set(rho, i, gsl_blas_dnrm2(&workp2.vector)); \
          if (n > p && dr > 0.0) { \
            Px[i] = log10(gsl_pow_2(gsl_vector_get(rho, i))+dr); \
          } else { \
            Px[i] = log10(gsl_pow_2(gsl_vector_get(rho, i))); \
          } \
          gsl_vector_set(eta, i, gsl_blas_dnrm2(&workp.vector)); \
          Py[i] = log10(gsl_pow_2(gsl_vector_get(eta, i)));

      for (i = 0; i < 4; ++i)
        {
          double lambda = sqrt(l[i]);
          L_CURVE_P
        }
      DO1=0;
      do {
        C[1] = menger_C(Px[0],Px[1],Px[2],Py[0],Py[1],Py[2]);
        C[2] = menger_C(Px[1],Px[2],Px[3],Py[1],Py[2],Py[3]);
        DO2=0;
        do {
            l[3]=l[2];
            x[3]=x[2];
            Px[3]=Px[2];
            Py[3]=Py[2];
            l[2]=l[1];
            x[2]=x[1];
            Px[2]=Px[1];
            Py[2]=Py[1];

            x[1]=(x[3]+phi*x[0])/(1+phi);
            l[1]=pow(10,x[1]);
            i = 1;
            lambda = sqrt(l[i]);
            L_CURVE_P
            C[2] = menger_C(Px[1],Px[2],Px[3],Py[1],Py[2],Py[3]);
            DO2++;
        } while (C[2]<=0 && DO2<=maxDO2);
        if (C[1]>C[2]) {
            *lopt = l[1]; // store lambda
            l[3]=l[2];
            x[3]=x[2];
            Px[3]=Px[2];
            Py[3]=Py[2];
            l[2]=l[1];
            x[2]=x[1];
            Px[2]=Px[1];
            Py[2]=Py[1];

            x[1]=(x[3]+phi*x[0])/(1+phi);
            l[1]=pow(10,x[1]);
            i=1;
            lambda = sqrt(l[i]);
            L_CURVE_P // only P[1] is recalculated
        } else {
            *lopt = l[2]; // store lambda
            l[0]=l[1];
            x[0]=x[1];
            Px[0]=Px[1];
            Py[0]=Py[1];
            l[1]=l[2];
            x[1]=x[2];
            Px[1]=Px[2];
            Py[1]=Py[2];

            x[2]=(x[0]+(x[3]-x[1]));
            l[2]=pow(10,x[2]);
            i=2;
            lambda = sqrt(l[i]);
            L_CURVE_P // only P[2] is recalculated
        }
        DO1++;
        sasfit_out(">>>>>>  lopt:%lg <<<<<<<<<<<<\n",sqrt(*lopt));
      } while ((l[3]-l[0])/l[3]>=epsterm && DO1<maxDO1);
      /* restore D to identity matrix */
      gsl_vector_set_all(work->D, 1.0);
      *lopt = sqrt(*lopt);
      return status;
    }
} /* sasfit_gsl_multifit_linear_corner_golden_rule() */

int
sasfit_bspline_lcorner(const gsl_vector *rho,
                       const gsl_vector *eta,
                             gsl_vector *lambda,
                             gsl_vector *Brho,
                             gsl_vector *Beta,
                             gsl_vector *Blambda)
{
    const size_t n = rho->size;
    #define NCOEFFS  12 // sasfit_eps_get_robertus_p()
    #define NBREAK   (NCOEFFS - 2)
    size_t ncoeffs = NCOEFFS;
    size_t nbreak = NBREAK;
    size_t i, j;
    gsl_bspline_workspace *bw;
    gsl_vector *B;
    double dy, xi, yi,yerr;
    gsl_vector *c, *w;
    gsl_vector *x, *y;
    gsl_matrix *X, *cov;
    gsl_multifit_linear_workspace *mw;
    double chisq, Rsq, dof, tss;

    if (n != eta->size && n != Brho->size && n != Beta->size) {
      GSL_ERROR ("size of rho, eta, Brho, and Beta vectors do not match",
                 GSL_EBADLEN);
    } else {
          /* allocate a cubic bspline workspace (k = 4) */
        bw = gsl_bspline_alloc(4, nbreak);
        B = gsl_vector_alloc(ncoeffs);
        x = gsl_vector_alloc(n);
        y = gsl_vector_alloc(n);
        X = gsl_matrix_alloc(n, ncoeffs);
        c = gsl_vector_alloc(ncoeffs);
        w = gsl_vector_alloc(n);
        cov = gsl_matrix_alloc(ncoeffs, ncoeffs);
        mw = gsl_multifit_linear_alloc(n, ncoeffs);
        gsl_vector_set_all(w, 1.0);
        /* use uniform breakpoints on [0, 15] */
        gsl_bspline_knots_uniform(log(gsl_vector_min(rho)), log(gsl_vector_max(rho)), bw);
          /* construct the fit matrix X */
        gsl_permutation * perm = gsl_permutation_alloc(n);
        gsl_permutation * rank = gsl_permutation_alloc(n);
        gsl_sort_vector_index (perm, rho);
        for (i = 0; i < n; ++i) {
            gsl_vector_set(x,i,log(gsl_vector_get(rho,gsl_permutation_get(perm,i))));
            gsl_vector_set(y,i,log(gsl_vector_get(eta,gsl_permutation_get(perm,i))));
            gsl_vector_set(Blambda,i,gsl_vector_get(lambda,gsl_permutation_get(perm,i)));
            double xi = gsl_vector_get(x, i);

      /* compute B_j(xi) for all j */
            gsl_bspline_eval(xi, B, bw);

      /* fill in row i of X */
            for (j = 0; j < ncoeffs; ++j) {
                double Bj = gsl_vector_get(B, j);
                gsl_matrix_set(X, i, j, Bj);
            }
        }

  /* do the fit */
        gsl_multifit_wlinear(X, w, y, c, cov, &chisq, mw);
  /* assign the smoothed curve */
        for (i = 0; i<n; i++) {
            xi = gsl_vector_get(x,i);
            gsl_bspline_eval(xi, B, bw);
            gsl_multifit_linear_est(B, c, cov, &yi, &yerr);
            gsl_vector_set(Brho,i,exp(xi));
            gsl_vector_set(Beta,i,exp(yi));
        }
        gsl_bspline_free(bw);
        gsl_vector_free(B);
        gsl_vector_free(x);
        gsl_vector_free(y);
        gsl_matrix_free(X);
        gsl_vector_free(c);
        gsl_vector_free(w);
        gsl_matrix_free(cov);
        gsl_permutation_free (perm);
        gsl_permutation_free (rank);
        gsl_multifit_linear_free(mw);
    }
}

int
sasfit_gsl_multifit_linear_lcorner(const gsl_vector *rho,
                            const gsl_vector *eta,
                             gsl_vector *MengerR,
                             gsl_vector *slope,
                             sasfit_EM_opt_Lagrange_param_method Lc,
                             double maxslope,
                            size_t *idx)
{
  const size_t n = rho->size;
      double rmin2 = -1;
      double rmin = -1;
      double r;
      double lslope;
      int idx2;
      bool init;

  if (n < 3)
    {
      GSL_ERROR ("at least 3 points are needed for L-curve analysis",
                 GSL_EBADLEN);
    }
  else if (n != eta->size)
    {
      GSL_ERROR ("size of rho and eta vectors do not match",
                 GSL_EBADLEN);
    }
  else
    {
      int s = GSL_SUCCESS;
      size_t i;
      double x1, y1;      /* first point of triangle on L-curve */
      double x2, y2;      /* second point of triangle on L-curve */
      double rmin = -1.0; /* minimum radius of curvature */
      /* initial values */
      init = FALSE;
      x1 = log(gsl_vector_get(rho, 0));
      y1 = log(gsl_vector_get(eta, 0));

      x2 = log(gsl_vector_get(rho, 1));
      y2 = log(gsl_vector_get(eta, 1));
      init = FALSE;
      for (i = 1; i < n - 1; ++i)
        {
          /*
           * The points (x1,y1), (x2,y2), (x3,y3) are the previous,
           * current, and next point on the L-curve. We will find
           * the circle which fits these 3 points and take its radius
           * as an estimate of the curvature at this point.
           */
          double x3 = log(gsl_vector_get(rho, i + 1));
          double y3 = log(gsl_vector_get(eta, i + 1));

          lslope = (y3-y2)/(x3-x2);
          if (fabs(lslope) < maxslope) init=TRUE;
          if ((fabs(lslope) > maxslope) && (init==TRUE)) break;

          double r = menger_r(x1,x2,x3,y1,y2,y3);
          gsl_vector_set(MengerR,i,r);
          gsl_vector_set(slope,i,(y3-y2)/(x3-x2));
          /* if d =~ 0 then there are nearly colinear points */
          if (gsl_finite(r) && r>0)
            {
              /* check for smallest radius of curvature */
              if (r < rmin || rmin < 0.0)
                {
                  rmin = r;
                  *idx = i;
                }
            }

          /* update previous/current L-curve values */
          x1 = x2;
          y1 = y2;
          x2 = x3;
          y2 = y3;
        }

      gsl_vector_set(MengerR,0,gsl_vector_get(MengerR,1));
      gsl_vector_set(MengerR,n-1,gsl_vector_get(MengerR,n-2));
      gsl_vector_set(slope,0,gsl_vector_get(slope,1));
      gsl_vector_set(slope,n-1,gsl_vector_get(slope,n-2));
      sasfit_out("idx:%d, rmin:%lg\n",*idx, rmin);
      if (Lc == Lcorner_o) return s;

      /* check if a minimum radius was found */
      init = FALSE;
      for (i = 2; i < n - 2; ++i)
        {
        lslope = gsl_vector_get(slope,i);
        r = gsl_vector_get(MengerR,i);

        if (fabs(lslope) < maxslope) init=TRUE;
        if ((fabs(lslope) > maxslope) && (init==TRUE)) break;

         if (   r > 0
             && gsl_vector_get(MengerR,i-1) >0
             && gsl_vector_get(MengerR,i-2) >0
             && gsl_vector_get(MengerR,i+1) >0
             && gsl_vector_get(MengerR,i+2) >0 ) {
            if (   r<gsl_vector_get(MengerR,i-1)
                && r<gsl_vector_get(MengerR,i-2)
                && r<gsl_vector_get(MengerR,i+1)
                && r<gsl_vector_get(MengerR,i+2)
                && r < 10*rmin) {
                rmin2 = r;
                idx2 = i;
sasfit_out("idx2:%d, rmin2:%lg, maxslope:%lg\n",idx2, rmin2,maxslope);
                }
        }
       }
       if (Lc == Lcorner_l) return s;
sasfit_out("idx2:%d, rmin2:%lg, maxslope:%lg\n",idx2, rmin2,maxslope);
      if (rmin2 < 0.0)
        {
          if (rmin<0.0)
          { /* possibly co-linear points */
            GSL_ERROR("failed to find minimum radius", GSL_EINVAL);
          }
        } else {
            *idx = idx2;
        }
      return s;
    }
} /* gsl_multifit_linear_lcorner() */

int
sasfit_gsl_multifit_linear_lcorner2(const gsl_vector *reg_param, const gsl_vector *rho,
                             const gsl_vector *eta,
                             gsl_vector *MengerR,
                             gsl_vector *slope,
                             double maxslope,
                             size_t *idx)
{
  const size_t n = reg_param->size;
  double r;
  double rmin2 = -1;
  double rmin1 = -1;
  int idx2;
  bool init;

  if (n < 3)
    {
      GSL_ERROR ("at least 3 points are needed for L-curve analysis",
                 GSL_EBADLEN);
    }
  else if (n != eta->size)
    {
      GSL_ERROR ("size of reg_param and eta vectors do not match",
                 GSL_EBADLEN);
    }
  else
    {
      int s = GSL_SUCCESS;
      size_t i;
      double x1, y1, u1, u2;      /* first point of triangle on L-curve */
      double x2, y2, v1, v2;      /* second point of triangle on L-curve */
      double rmin = -1.0; /* minimum radius of curvature */
      double rmin2 = -1;
      double lslope;
      int idx2;
      /* initial values */
      init = FALSE;
      x1 = gsl_vector_get(reg_param, 0);
      x1 = x1*x1;
      u1 = log(gsl_vector_get(rho, 0));
      y1 = gsl_vector_get(eta, 0);
      v1 = log(y1);
      y1 = y1*y1;

      x2 = gsl_vector_get(reg_param, 1);
      x2 = x2*x2;
      u2 = log(gsl_vector_get(rho, 1));
      y2 = gsl_vector_get(eta, 1);
      v2 = log(y2);
      y2 = y2*y2;

      for (i = 1; i < n - 1; ++i)
        {
          /*
           * The points (x1,y1), (x2,y2), (x3,y3) are the previous,
           * current, and next point on the L-curve. We will find
           * the circle which fits these 3 points and take its radius
           * as an estimate of the curvature at this point.
           */
          double lamip1 = gsl_vector_get(reg_param, i + 1);
          double etaip1 = gsl_vector_get(eta, i + 1);
          double x3 = lamip1 * lamip1;
          double y3 = etaip1 * etaip1;
          double u3 = log(log(gsl_vector_get(rho, i+1)));
          double v3 = log(etaip1);
          lslope = (v3-v2)/(u3-u2);
          if (fabs(lslope) < maxslope) init=TRUE;
          if ((fabs(lslope) > maxslope) && (init==TRUE)) break;

          double r = -menger_r(x1,x2,x3,y1,y2,y3);
          gsl_vector_set(MengerR,i,r);
          gsl_vector_set(slope,i,(v3-v2)/(u3-u2));

          if (gsl_finite(r) && r > 0) {
            if (   r < rmin || rmin < 0) {
                rmin = r;
                *idx = i;
                }
          }
          /* update previous/current L-curve values */
          x1 = x2;
          y1 = y2;
          x2 = x3;
          y2 = y3;
          u1 = u2;
          v1 = v2;
          u2 = u3;
          v2 = v3;
        }
      gsl_vector_set(MengerR,0,gsl_vector_get(MengerR,1));
      gsl_vector_set(MengerR,n-1,gsl_vector_get(MengerR,n-2));
      /* check if a minimum radius was found */
 sasfit_out("idx:%d, rmin:%lg\n",*idx, rmin);
    idx2=0;
    init = FALSE;
    for (i = 2; i < n - 2; ++i)
        {
        lslope = gsl_vector_get(slope,i);
        r = gsl_vector_get(MengerR,i);
        if (fabs(lslope) < maxslope) init=TRUE;
        if ((fabs(lslope) > maxslope) && (init==TRUE)) break;

        if (   gsl_vector_get(MengerR,i) > 0
            && gsl_vector_get(MengerR,i-1) >0
            && gsl_vector_get(MengerR,i-2) > 0
            && gsl_vector_get(MengerR,i+1) > 0
            && gsl_vector_get(MengerR,i+2) > 0 ) {
            if (   gsl_vector_get(MengerR,i)<gsl_vector_get(MengerR,i-1)
                && gsl_vector_get(MengerR,i)<gsl_vector_get(MengerR,i-2)
                && gsl_vector_get(MengerR,i)<gsl_vector_get(MengerR,i+1)
                && gsl_vector_get(MengerR,i)<gsl_vector_get(MengerR,i+2)
                && r < 10*rmin) {
                rmin2 = gsl_vector_get(MengerR,i);
                idx2 = i;
                }
        }
        }

 sasfit_out("idx2:%d, rmin2:%lg, maxslope:%lg\n",idx2, rmin2,maxslope);
    if (rmin2 < 0.0)
        {
          if (rmin<0.0)
          { /* possibly co-linear points */
            GSL_ERROR("failed to find minimum radius", GSL_EINVAL);
          }
        } else {
            *idx = idx2;
        }
    return s;
    }
} /* gsl_multifit_linear_lcorner2() */

scalar sasfit_Lcorner(sasfit_Lcurve *L, sasfit_EM_opt_Lagrange_param_method Lc) {
    scalar fract;
    scalar mx2, x2, mr, r, minr;
    int i,idx,idx2;
    fract=0.99;
    mx2=-1;
    for (i=0;i<L->length;i++) {
        x2 = L->Gtest[i];
        if ((mx2==-1 || x2<mx2)) mx2=x2;
    }
sasfit_out("smallest G-test value: %lg\n",mx2);
    mr=-1;
    idx2 = L->length-1;
    for (i=0;i<L->length;i++) {
        x2 = L->Gtest[i];
        r = L->Lradius[i]*pow(x2-mx2*fract,1);
        if ((mr==-1 && r>0) || (r>0 && r<mr)) {
            mr = r;
            idx2 = i;
        }
    }
    sasfit_out("smallest weighted r( %d ) value: %lg, lambda: %lg\n",idx2, mr, L->lagrange[idx2] );
    L->optlagrange=L->lagrange[idx2];
    if (Lc == Lcorner_w) return L->lagrange[idx2];
    minr=-1;
    idx = L->length-1;
    for (i=0;i<L->length;i++) {
        r = L->Lradius[i];
        if ((minr==-1 && r>0) || (r>0 && r<minr)) {
            minr = r;
            idx = i;
        }
    }
    sasfit_out("smallest overall r( %d ) value: %lg, lambda: %lg\n",idx, minr,L->lagrange[idx] );
    L->optlagrange=L->lagrange[idx];
    if (Lc == Lcorner_o) return L->lagrange[idx];
    if ((idx2 > 2) || (idx2 < L->length-1)) {
        minr = mr;
        idx = idx2;
    }
    sasfit_out("smallest lambda for weighted and overall r( %d ) value: %lg with lambda: %lg\n",idx, minr,L->lagrange[idx] );
    L->optlagrange=L->lagrange[idx2];
    if (Lc == Lcorner_wo) return L->lagrange[idx2];
    idx2=0;
    mr = -1;
    for (i=2;i<L->length-2;i++) {
        if (   L->Lradius[i] > 0
            && L->Lradius[i-1] > 0
            && L->Lradius[i-2] > 0
            && L->Lradius[i+1] > 0
            && L->Lradius[i+2] > 0
            && L->Lradius[i]<L->Lradius[i-1]
            && L->Lradius[i]<L->Lradius[i-2]
            && L->Lradius[i]<L->Lradius[i+1]
            && L->Lradius[i]<L->Lradius[i+2]
            && L->Lradius[i]<10*minr) {
                mr = L->Lradius[i];
                idx2 = i;
        }
    }
    sasfit_out("smallest r( %d ) value obtained via local minima consideration: %lg using lambda: %lg\n",idx2, mr,L->lagrange[idx2] );
    L->optlagrange=L->lagrange[idx2];
    if (Lc == Lcorner_l) return L->lagrange[idx2];
    if (!(mr < 0.0  && idx2 < 2)) {
        if (minr<0.0) { /* possibly co-linear points */
            GSL_ERROR("failed to find minimum radius", GSL_EINVAL);
        } else {
            idx = idx2;
        }
        L->optlagrange=L->lagrange[idx];
    }
/*
    for (i=0;i<L->length;i++) {
        sasfit_out("i:\t%d\tGtest:\t%lg\tchi2tes:\t%lg\tchi2_error:\t%lg\tfirstderiv:\t%lg\tsecondderiv:\t%lg\tentropy:\t%lg\tlagrange:\t%lg\tradius:\t%lg\n",
                   i,L->Gtest[i],L->chi2test[i],L->chi2_error[i],L->firstderiv[i],L->secondderiv[i],L->entropy[i],L->lagrange[i],L->Lradius[i]);
    }
*/
    return L->lagrange[idx];
}

int Sasfit_DR_EM_smoothing_Cmd(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int        argc;
    char       **argv;
{
struct extrapolPar EP;
EM_param_t EMparam;
sasfit_fp_data FPd;
FILE *fptr;
scalar p1,p2,p3, QR, eps,chi2;
int    i,j,k,l,itst;
scalar rmax;
sasfit_Lcurve L;
char   errstr[256],Buffer[256];
bool   error;
Tcl_DString DsBuffer;
#define XMAX 0.35
#define XHI 0.325
#define XLO 0.3
FPd.tm=NULL;
    int status;
    const gsl_root_fsolver_type *solver_type;
    gsl_root_fsolver *solver;

    const gsl_min_fminimizer_type *Tmin;
    gsl_min_fminimizer *smin;

    gsl_function F;
    double r, sm;
    double x_lo = XLO, x_hi = XHI, x_max=XMAX;
    double chi2_lo, chi2_hi, chi2_max;

error = FALSE;

if (argc != 3) {
   sasfit_err("wrong # args; should be sasfit_DR_EM ?EMOptions? ?xye_data?\n");
   return TCL_ERROR;
}

if (TCL_ERROR == get_EP(clientData,interp,argv,&EP,&EMparam.h,&EMparam.Iexp,&EMparam.DIh)) {
   return TCL_ERROR;
}

if (TCL_ERROR == get_EM(clientData,interp,&EMparam)) {
   return TCL_ERROR;
}
gsl_set_error_handler_off();
EMparam.C0 = EP.c0;
EMparam.C4 = EP.c4;
EMparam.nh = EP.ndata;
FPd.FPstructure=&EMparam;
FPd.FP_Op=&EM_DR_DoubleSmooth_Operator;
FPd.mixcoeff=0.5;
FPd.mixstrategy=mix_const;
//FPd.PrintProgress=0;

FPd.it=0;
FPd.interp=interp;
EM_DR_Init(&FPd);
/*
sasfit_out("D(R): %s algorithm: %d\n",EMparam.iteration_scheme, FPd.root_algorithm);
sasfit_out("maxteps: %d\n",FPd.maxsteps);
sasfit_out("R_max: %lg\n",EMparam.Rmax);
sasfit_out("number of R bins: %d\n",EMparam.nR);
sasfit_out("finished initialization\n");
*/
rmax = M_PI/EMparam.h[0];
rmax = EMparam.Rmax;

#define MAX_ROOT_ITERATIONS 100
switch (EMparam.optLagrange_method) {
 case redchi2:
//    fptr = fopen("c:/temp/SASfit.dat","a+");
    do {
        chi2_max=Optimum_smooth4DR_EM(x_max, &FPd);
    } while ((FPd.failed || FPd.it >= FPd.maxsteps && FPd.interrupt == 0) && x_max > FPd.relerror );
//    fprintf(fptr,"smooth\t%16.14lg\tchi2\t%16.14lg\tGtest\t%16.14lg\tchi2test\t%16.14lg\tJSDtest\t%16.14lg\tKLD\t%16.14lg\tJSD\t%16.14lg\tSum1st\t%16.14lg\tSum2nd\t%16.14lg\tEntropy\t%16.14lg\n",
//                  x_max, FPd.Chi2Norm,EMparam.Gtest,EMparam.chi2test,EMparam.JSDtest, FPd.KLD, FPd.JSD,FPd.Sum1stDeriv, FPd.Sum2ndDeriv,FPd.Entropy);
    x_hi = x_max/REDFACTOR;
    if ( FPd.interrupt == 0){
    do {
        if (FPd.failed || FPd.it >= FPd.maxsteps) x_hi=x_hi/REDFACTOR;
        chi2_hi=Optimum_smooth4DR_EM(x_hi, &FPd);
    } while ((FPd.failed || FPd.it >= FPd.maxsteps && FPd.interrupt == 0) && x_max > FPd.relerror );
    x_lo = x_hi/REDFACTOR;
    }
//    fprintf(fptr,"smooth\t%16.14lg\tchi2\t%16.14lg\tGtest\t%16.14lg\tchi2test\t%16.14lg\tJSDtest\t%16.14lg\tKLD\t%16.14lg\tJSD\t%16.14lg\tSum1st\t%16.14lg\tSum2nd\t%16.14lg\tEntropy\t%16.14lg\n",
//                  x_hi, FPd.Chi2Norm,EMparam.Gtest,EMparam.chi2test,EMparam.JSDtest, FPd.KLD, FPd.JSD,FPd.Sum1stDeriv, FPd.Sum2ndDeriv,FPd.Entropy);

    if ( FPd.interrupt == 0){
    do {
        if (FPd.failed || FPd.it >= FPd.maxsteps) x_lo=x_lo/REDFACTOR;
        chi2_lo=Optimum_smooth4DR_EM(x_lo, &FPd);
    } while ((FPd.failed || FPd.it >= FPd.maxsteps) && x_max > FPd.relerror && FPd.interrupt == 0);
    }
//    fprintf(fptr,"smooth\t%16.14lg\tchi2\t%16.14lg\tGtest\t%16.14lg\tchi2test\t%16.14lg\tJSDtest\t%16.14lg\tKLD\t%16.14lg\tJSD\t%16.14lg\tSum1st\t%16.14lg\tSum2nd\t%16.14lg\tEntropy\t%16.14lg\n",
//                  x_lo, FPd.Chi2Norm,EMparam.Gtest,EMparam.chi2test,EMparam.JSDtest, FPd.KLD, FPd.JSD,FPd.Sum1stDeriv, FPd.Sum2ndDeriv,FPd.Entropy);

    while (chi2_lo > 0 && x_lo > FPd.relerror && FPd.interrupt == 0) { // && chi2_lo < chi2_hi
        if (chi2_max > chi2_hi) {
            x_max = x_hi;
            chi2_max = chi2_hi;
            x_hi = x_lo;
            chi2_hi = chi2_lo;
        }
        x_lo = x_lo/REDFACTOR;
        chi2_lo=Optimum_smooth4DR_EM(x_lo, &FPd);
//    fprintf(fptr,"smooth\t%16.14lg\tchi2\t%16.14lg\tGtest\t%16.14lg\tchi2test\t%16.14lg\tJSDtest\t%16.14lg\tKLD\t%16.14lg\tJSD\t%16.14lg\tSum1st\t%16.14lg\tSum2nd\t%16.14lg\tEntropy\t%16.14lg\n",
//                  x_lo, FPd.Chi2Norm,EMparam.Gtest,EMparam.chi2test,EMparam.JSDtest, FPd.KLD, FPd.JSD,FPd.Sum1stDeriv, FPd.Sum2ndDeriv,FPd.Entropy);
//    fflush(fptr);
    }
//    fclose(fptr);
    sasfit_out ("x x_lo:%lg, x_max:%lg, x_hi:%lg\n", x_lo, x_max, x_hi);
    if (chi2_lo < 0 && chi2_max > 0 && FPd.interrupt == 0) {
        chi2_lo = chi2_lo+EMparam.chi2;
        chi2_max = chi2_max+EMparam.chi2;
        F.function = &Optimum_smooth4DR_EM;
        F.params = &FPd;
        solver_type = gsl_root_fsolver_brent;
        solver = gsl_root_fsolver_alloc(solver_type);
        status = gsl_root_fsolver_set(solver, &F, x_lo, x_hi);
        if (status != GSL_SUCCESS || FPd.interrupt == 1) {
            sasfit_out("error(%d): %s, interrupt (%d)\n", status, gsl_strerror(status),FPd.interrupt);
            sasfit_err("Increasing precision parameter might help to avoid this error message.\n");
        }
        status = GSL_CONTINUE;
        for (i = 1; i <= MAX_ROOT_ITERATIONS && status == GSL_CONTINUE &&(FPd.interrupt == 0); ++i) {
        /* iterate one step of the solver */
            status = gsl_root_fsolver_iterate(solver);

        /* get the solver's current best solution and bounds */
            r = gsl_root_fsolver_root(solver);
            if (x_max != gsl_root_fsolver_x_upper (solver)) {
                x_max = gsl_root_fsolver_x_upper (solver);
                chi2_max = FPd.Chi2Norm;
            }
            if (x_lo != gsl_root_fsolver_x_lower (solver)) {
                x_lo = gsl_root_fsolver_x_lower (solver);
                chi2_lo = FPd.Chi2Norm;
            }

        /* Check to see if the solution is within 0.001 */
            status = gsl_root_test_interval(chi2_lo, chi2_max, 0, 0.001);
            if (FPd.PrintProgress>0)  sasfit_out("%5d x_lo:%lg chi2_lo:%lg x_max:%lg chi2_max:%lg, r:%lg\n",i, x_lo, chi2_lo, x_max, chi2_max, r);
            if (status == GSL_SUCCESS) sasfit_out("Converged:\n");
        }

    /* Free the solver */
        gsl_root_fsolver_free(solver);

        if (status == GSL_CONTINUE) {
            sasfit_out("error: too many iterations");
        } else if (status != GSL_SUCCESS || FPd.interrupt == 1) {
            sasfit_out("error(%d): %s, interrupt (%d)\n", status, gsl_strerror(status),FPd.interrupt);
            sasfit_err("Increasing precision parameter might help to avoid this error message.\n");
        }
    } else {
//        x_max = 0.35;
//        x_lo = FPd.relerror;
//        x_hi = 1e-4;
        F.function = &Optimum_smooth4DR_EM;
        F.params = &FPd;
        if (FPd.PrintProgress==5) sasfit_out ("xx x_lo:%lg, x_max:%lg, x_hi:%lg\n", x_lo, x_max, x_hi);
        Tmin = gsl_min_fminimizer_brent;
        smin = gsl_min_fminimizer_alloc (Tmin);
        if (FPd.PrintProgress==5) sasfit_out ("xxx %lg %lg %lg\n", x_lo, x_max, x_hi);
        status = gsl_min_fminimizer_set (smin, &F, x_hi, x_lo, x_max);
        if (status != GSL_SUCCESS || FPd.interrupt == 1) {
            sasfit_out("error(%d): %s, interrupt (%d)\n", status, gsl_strerror(status),FPd.interrupt);
            sasfit_err("Increasing precision parameter might help to avoid this error message.\n");
        } else {
            if (FPd.PrintProgress==5) sasfit_out ("xxxx %lg %lg %lg\n", x_lo, x_max, x_hi);
            i=0;
            if ( FPd.interrupt == 0){
            do {
                i++;
                status = gsl_min_fminimizer_iterate (smin);

                if (x_max != gsl_min_fminimizer_x_upper (smin)) {
                    x_max = gsl_min_fminimizer_x_upper (smin);
                    chi2_max = chi2_hi;
                }
                if (x_lo != gsl_min_fminimizer_x_lower (smin)) {
                    x_lo = gsl_min_fminimizer_x_lower (smin);
                    chi2_lo = chi2_hi;
                }
                x_hi = gsl_min_fminimizer_x_minimum (smin);
                chi2_hi = FPd.Chi2Norm;

                status = gsl_min_test_interval (chi2_lo, chi2_max, 0.001, 0.0);

                if (status == GSL_SUCCESS) sasfit_out ("Converged:\n");

                if (FPd.PrintProgress>0)  sasfit_out ("%5d x_lo:%lg chi2_lo:%lg x_max:%lg chi2_max:%lg x_hi:%lg chi2_hi:%lg\n", i, x_lo, chi2_lo, x_max, chi2_max, x_hi, chi2_hi);
            } while (status == GSL_CONTINUE && i < MAX_ROOT_ITERATIONS && FPd.interrupt == 0);
            }
            if (status == GSL_CONTINUE) {
                sasfit_err("error: too many iterations");
            } else if (status != GSL_SUCCESS || FPd.interrupt == 1) {
                sasfit_out("error(%d): %s, interrupt (%d)\n", status, gsl_strerror(status),FPd.interrupt);
                sasfit_err("Increasing precision parameter might help to avoid this error message.\n");
            }
        }
        gsl_min_fminimizer_free (smin);
    }
    break;
 case manual:
    FP_solver(&FPd);
    break;
 case Lcorner_o:
 case Lcorner2:
 case Lcorner_l:
 case Lcorner_w:
 case Lcorner_wo:
    L.Gtest=malloc(2*sizeof(scalar));
    L.chi2test=malloc(2*sizeof(scalar));
    L.chi2_error=malloc(2*sizeof(scalar));
    L.firstderiv=malloc(2*sizeof(scalar));
    L.secondderiv=malloc(2*sizeof(scalar));
    L.entropy=malloc(2*sizeof(scalar));
    L.lagrange=malloc(2*sizeof(scalar));
    L.Lradius=malloc(2*sizeof(scalar));
    do {
        if (FPd.failed || FPd.it >= FPd.maxsteps) x_max=x_max/REDFACTOR;
        chi2_max= Optimum_smooth4DR_EM(x_max, &FPd);
    } while ((FPd.failed || FPd.it >= FPd.maxsteps) && x_max > FPd.relerror && FPd.interrupt==0);
    if (FPd.interrupt == 0) {
        L.lagrange[0]=x_max;
        L.Gtest[0] = EMparam.Gtest;
        L.chi2test[0]=EMparam.chi2test;
        L.chi2_error[0]=FPd.Chi2Norm;
        L.firstderiv[0]=FPd.Sum1stDeriv;
        L.secondderiv[0]=FPd.Sum2ndDeriv;
        L.entropy[0]=FPd.Entropy;

        x_hi = x_max/REDFACTOR;
        do {
            if (FPd.failed || FPd.it >= FPd.maxsteps) x_hi=x_hi/REDFACTOR;
            chi2_hi = Optimum_smooth4DR_EM(x_hi, &FPd);
        } while ((FPd.failed || FPd.it >= FPd.maxsteps) && x_max > FPd.relerror && FPd.interrupt==0);
        x_lo=x_hi/REDFACTOR;

        L.lagrange[1]=x_hi;
        L.Gtest[1] = EMparam.Gtest;
        L.chi2test[1]=EMparam.chi2test;
        L.chi2_error[1]=FPd.Chi2Norm;
        L.firstderiv[1]=FPd.Sum1stDeriv;
        L.secondderiv[1]=FPd.Sum2ndDeriv;
        L.entropy[1]=FPd.Entropy;

        switch (EMparam.defL) {
        case Idendity:
            L.y1 = L.entropy[0];
            L.y2 = L.entropy[1];
            break;
        case first_deriv_eps_b:
        case first_deriv_eps_e:
        case first_deriv:
            L.y1 = L.firstderiv[0];
            L.y2 = L.firstderiv[1];
            break;
        case second_deriv_DD:
        case second_deriv_NN:
        case second_deriv_DN:
        case second_deriv_ND:
        case second_deriv:
        default:
            L.y1 = L.secondderiv[0];
            L.y2 = L.secondderiv[1];
            break;
        }
        L.u1 = log(L.Gtest[0]);
        L.u2 = log(L.Gtest[1]);
        L.v1 = log(L.y1);
        L.v2 = log(L.y2);
        if (EMparam.optLagrange_method == Lcorner2) {
            L.x1 = log(gsl_pow_2(L.lagrange[0]));
            L.y1 *=L.y1;
            L.x2 = log(gsl_pow_2(L.lagrange[1]));
            L.y2 *=L.y2;
        } else {
            L.x1 = log(L.Gtest[0]);
            L.y1 = log(L.y1);
            L.x2 = log(L.Gtest[1]);
            L.y2 = log(L.y2);
        }
        L.length=2;
        L.rmin=-1;
        L.optlagrange=L.lagrange[1];
        L.s21=0;

        while (x_lo > FPd.relerror && fabs(L.s21)<EMparam.maxslope && FPd.interrupt == 0) { // && chi2_lo < chi2_hi
            x_lo = x_lo/REDFACTOR;
            if (FPd.PrintProgress==5)  sasfit_out("start while loop L.length:%d with smooth value:%lg\n",L.length,x_lo);
            chi2_lo=Optimum_smooth4DR_EM(x_lo, &FPd);
            L.length=L.length+1;
            i=L.length;
            L.Gtest=(scalar *) realloc(L.Gtest,i*sizeof(scalar));
            L.chi2test=(scalar *) realloc(L.chi2test,i*sizeof(scalar));
            L.chi2_error=(scalar *) realloc(L.chi2_error,i*sizeof(scalar));
            L.firstderiv=(scalar *) realloc(L.firstderiv,i*sizeof(scalar));
            L.secondderiv=(scalar *) realloc(L.secondderiv,i*sizeof(scalar));
            L.entropy=(scalar *) realloc(L.entropy,i*sizeof(scalar));
            L.lagrange=(scalar *) realloc(L.lagrange,i*sizeof(scalar));
            L.Lradius=(scalar *) realloc(L.Lradius,i*sizeof(scalar));

            L.lagrange[i-1]=x_lo;
            L.Gtest[i-1] = EMparam.Gtest;
            L.chi2test[i-1]=EMparam.chi2test;
            L.chi2_error[i-1]=FPd.Chi2Norm;
            L.firstderiv[i-1]=FPd.Sum1stDeriv;
            L.secondderiv[i-1]=FPd.Sum2ndDeriv;
            L.entropy[i-1]=FPd.Entropy;

//        L.x3 = log(L.Gtest[i-1]);
            switch (EMparam.defL) {
            case Idendity:
                L.y3 = L.entropy[i-1];
                break;
            case first_deriv_eps_b:
            case first_deriv_eps_e:
            case first_deriv:
                L.y3 = L.firstderiv[i-1];
                break;
            case second_deriv_DD:
            case second_deriv_NN:
            case second_deriv_DN:
            case second_deriv_ND:
            case second_deriv:
            default:
                L.y3 = L.secondderiv[i-1];
                break;
            }
            L.u3 = log(L.Gtest[i-1]);
            L.v3 = log(L.y3);
            if (EMparam.optLagrange_method == Lcorner2) {
                L.x3 = log(gsl_pow_2(L.lagrange[i-1]));
                L.y3 *=L.y3;
                L.signr = -1;
            } else {
                L.x3 = log(L.Gtest[i-1]);
                L.y3 = log(L.y3);
                L.signr = 1;
            }
            L.x21 = L.x2-L.x1;
            L.y21 = L.y2-L.y1;
            L.x31 = L.x3-L.x1;
            L.y31 = L.y3-L.y1;
            L.u31 = L.u3-L.u1;
            L.u21 = L.u2-L.u1;
            L.v31 = L.v3-L.v1;
            L.v21 = L.v2-L.v1;
            L.h21 = L.x21*L.x21 + L.y21*L.y21;
            L.h31 = L.x31*L.x31 + L.y31*L.y31;
            L.d = fabs(2.0 * (L.x21*L.y31 - L.x31*L.y21));
            L.s21= fabs(L.y21/L.x21);
            L.s31= fabs(L.y31/L.x31);
            L.signr = ((L.s31>L.s21) *2-1);
            L.s21= L.v21/L.u21;
            L.s31= L.v31/L.u31;
            r = L.signr*sqrt(L.h21*L.h31)*gsl_hypot(L.x3-L.x2,L.y3-L.y2) / L.d;
            L.Lradius[i-2] = -menger_r(L.x1,L.x2,L.x3,L.y1,L.y2,L.y3);
            if (FPd.PrintProgress==5) sasfit_out("r=%lg menger_r:%lg L.s21 %lg  L.y31:%lg\n",r,L.Lradius[i-2],L.s21, L.y31);
            if ((   L.Lradius[i-2] < L.rmin
                && L.Lradius[i-2] > 0
                && fabs(L.s21)<EMparam.maxslope)
                || (   L.rmin == -1.0
                    && L.Lradius[i-2]>0)) {
                L.rmin=L.Lradius[i-2];
                L.optlagrange=L.lagrange[i-2];
            }
            L.x1=L.x2;
            L.x2=L.x3;
            L.y1=L.y2;
            L.y2=L.y3;
            L.u1=L.u2;
            L.u2=L.u3;
            L.v1=L.v2;
            L.v2=L.v3;
        }
    L.Lradius[L.length-1]=L.Lradius[L.length-2];
    L.Lradius[0]=L.Lradius[1];

    EMparam.smooth=sasfit_Lcorner(&L,EMparam.optLagrange_method);
    sasfit_out("optimimum for smooth:%lg\n",L.optlagrange);

    chi2_lo=Optimum_smooth4DR_EM(EMparam.smooth, &FPd);
    }
    free(L.Gtest);
    free(L.chi2test);
    free(L.chi2_error);
    free(L.firstderiv);
    free(L.secondderiv);
    free(L.entropy);
    free(L.lagrange);
    free(L.Lradius);
    if (FPd.interrupt == 1) goto skipcode;
    chi2_lo=Optimum_smooth4DR_EM(EMparam.smooth, &FPd);
    if (FPd.interrupt == 1) goto skipcode;
    break;
 default:
    sasfit_err("no valid option to determine Lagrange parameter\n");
}

goto skipcode;
if (EMparam.chi2>0) {

    chi2_max=Optimum_smooth4DR_EM(x_max, &FPd);
    if (FPd.interrupt == 1) goto skipcode;
    chi2_hi=Optimum_smooth4DR_EM(x_hi, &FPd);
    if (FPd.interrupt == 1) goto skipcode;
    chi2_lo=Optimum_smooth4DR_EM(x_lo, &FPd);
    if (FPd.interrupt == 1) goto skipcode;
    while (chi2_lo > 0 && x_lo > FPd.relerror && chi2_lo < chi2_max && FPd.interrupt == 0 ) {
        if (chi2_max > chi2_hi) {
            x_max = x_hi;
            chi2_max = chi2_hi;
            x_hi = x_lo;
            chi2_hi = chi2_lo;
        }
        x_lo = x_lo/10.;
        chi2_lo=Optimum_smooth4DR_EM(x_lo, &FPd);
    }
    if (FPd.interrupt == 1) goto skipcode;
    if (FPd.PrintProgress==5) sasfit_out ("x x_lo:%lg, x_max:%lg x_hi:%lg\n", x_lo, x_max, x_hi);
    if (chi2_lo < 0 && chi2_max > 0 && FPd.interrupt == 0) {
        chi2_lo = chi2_lo+EMparam.chi2;
        chi2_max = chi2_max+EMparam.chi2;
        F.function = &Optimum_smooth4DR_EM;
        F.params = &FPd;
        solver_type = gsl_root_fsolver_brent;
        solver = gsl_root_fsolver_alloc(solver_type);
        status = gsl_root_fsolver_set(solver, &F, x_lo, x_hi);
        if (status != GSL_SUCCESS || FPd.interrupt == 1) {
            sasfit_out("error(%d): %s, interrupt (%d)\n", status, gsl_strerror(status),FPd.interrupt);
            sasfit_err("Increasing precision parameter might help to avoid this error message.\n");
        }
        status = GSL_CONTINUE;
        for (i = 1; i <= MAX_ROOT_ITERATIONS && status == GSL_CONTINUE && FPd.interrupt == 0; ++i) {
        /* iterate one step of the solver */
            status = gsl_root_fsolver_iterate(solver);

        /* get the solver's current best solution and bounds */
            r = gsl_root_fsolver_root(solver);
            if (x_max != gsl_root_fsolver_x_upper (solver)) {
                x_max = gsl_root_fsolver_x_upper (solver);
                chi2_max = FPd.Chi2Norm;
            }
            if (x_lo != gsl_root_fsolver_x_lower (solver)) {
                x_lo = gsl_root_fsolver_x_lower (solver);
                chi2_lo = FPd.Chi2Norm;
            }

        /* Check to see if the solution is within 0.001 */
            status = gsl_root_test_interval(chi2_lo, chi2_max, 0, 0.001);
            if (FPd.PrintProgress==5) sasfit_out("%5d x_lo:%lg chi2_lo:%lg x_max:%lg chi2_max:%lg, r:%lg\n",i, x_lo, chi2_lo, x_max, chi2_max, r);
            if (status == GSL_SUCCESS) sasfit_out("Converged:\n");
        }

    /* Free the solver */
        gsl_root_fsolver_free(solver);

        if (status == GSL_CONTINUE) {
            sasfit_out("error: too many iterations");
        } else if (status != GSL_SUCCESS || FPd.interrupt == 1) {
            sasfit_out("error(%d): %s, interrupt (%d)\n", status, gsl_strerror(status),FPd.interrupt);
            sasfit_err("Increasing precision parameter might help to avoid this error message.\n");
        }
    } else {
        x_max = 0.3;
//        x_lo = FPd.relerror;
//        x_hi = 1e-4;
        F.function = &Optimum_smooth4DR_EM;
        F.params = &FPd;
        if (FPd.PrintProgress==5) sasfit_out ("xx %lg, %lg %lg\n", x_lo, x_max, x_hi);
        Tmin = gsl_min_fminimizer_brent;
        smin = gsl_min_fminimizer_alloc (Tmin);
        if (FPd.PrintProgress==5) sasfit_out ("xxx %lg %lg %lg\n", x_lo, x_max, x_hi);
        status = gsl_min_fminimizer_set (smin, &F, x_hi, x_lo, x_max);
        if (status != GSL_SUCCESS || FPd.interrupt == 1) {
            sasfit_out("error(%d): %s, interrupt (%d)\n", status, gsl_strerror(status),FPd.interrupt);
            sasfit_err("Increasing precision parameter might help to avoid this error message.\n");
        }
        if (FPd.PrintProgress==5) sasfit_out ("xxxx %lg %lg %lg\n", x_lo, x_max, x_hi);
        i=0;
        do {
            i++;
            status = gsl_min_fminimizer_iterate (smin);

            if (x_max != gsl_min_fminimizer_x_upper (smin)) {
                x_max = gsl_min_fminimizer_x_upper (smin);
                chi2_max = chi2_hi;
            }
            if (x_lo != gsl_min_fminimizer_x_lower (smin)) {
                x_lo = gsl_min_fminimizer_x_lower (smin);
                chi2_lo = chi2_hi;
            }
            x_hi = gsl_min_fminimizer_x_minimum (smin);
            chi2_hi = FPd.Chi2Norm;

            status = gsl_min_test_interval (chi2_lo, chi2_max, 0.001, 0.0);

            if (status == GSL_SUCCESS) sasfit_out ("Converged:\n");

            if (FPd.PrintProgress==5) sasfit_out ("%5d x_lo:%lg chi2_lo:%lg x_max:%lg chi2_max:%lg x_hi:%lg chi2_hi:%lg\n", i, x_lo, chi2_lo, x_max, chi2_max, x_hi, chi2_hi);
        } while (status == GSL_CONTINUE && i < MAX_ROOT_ITERATIONS && FPd.interrupt == 0);
        if (status == GSL_CONTINUE) {
            sasfit_err("error: too many iterations");
        } else if (status != GSL_SUCCESS || FPd.interrupt == 1) {
                sasfit_out("error(%d): %s, interrupt (%d)\n", status, gsl_strerror(status),FPd.interrupt);
            sasfit_err("Increasing precision parameter might help to avoid this error message.\n");
        }
        gsl_min_fminimizer_free (smin);
    }
} else {
    FP_solver(&FPd);
}
skipcode:

sasfit_out("it. %d eps %lg chi2 %lg\n",FPd.it, FPd.gNorm, FPd.Chi2Norm);
sasfit_out("it. %d KLD %lg JSD %lg\n",FPd.it, FPd.KLD, FPd.JSD);

Tcl_ResetResult(interp);
Tcl_DStringInit(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nR;i++) {
    sprintf(Buffer,"%lg",EMparam.r[i]);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nR;i++) {
	sprintf(Buffer,"%lg",EMparam.out[i]);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nh;i++) {
	sprintf(Buffer,"%lg",EMparam.h[i]);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nh;i++) {
	sprintf(Buffer,"%lg",EMparam.Ithwork[i]*(1+EMparam.em_weight*(EMparam.DIh[i]-1))+EP.c0);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringResult(interp,&DsBuffer);
Tcl_DStringFree(&DsBuffer);

EM_DR_Free(&FPd);

return TCL_OK;
}

int Sasfit_DR_EM_ME_const_Cmd(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int        argc;
    char       **argv;
{
struct extrapolPar EP;
EM_param_t EMparam;
sasfit_fp_data FPd;
scalar p1,p2,p3, QR, eps,chi2;
int    i,j,k,l,itst;
scalar rmax;
sasfit_Lcurve L;
char   errstr[256],Buffer[256];
bool   error;
Tcl_DString DsBuffer;

FPd.tm=NULL;
    int status;
    const gsl_root_fsolver_type *solver_type;
    gsl_root_fsolver *solver;

    const gsl_min_fminimizer_type *Tmin;
    gsl_min_fminimizer *smin;

    gsl_function F;
    double r, sm;
    double x_lo, x_hi, x_max=2;
    double chi2_lo, chi2_hi, chi2_max;

error = FALSE;

if (argc != 3) {
   sasfit_err("wrong # args; should be sasfit_DR_EM_ME_const ?EMOptions? ?xye_data?\n");
   return TCL_ERROR;
}

if (TCL_ERROR == get_EP(clientData,interp,argv,&EP,&EMparam.h,&EMparam.Iexp,&EMparam.DIh)) {
   return TCL_ERROR;
}

if (TCL_ERROR == get_EM(clientData,interp,&EMparam)) {
   return TCL_ERROR;
}

gsl_set_error_handler_off();

EMparam.C0 = EP.c0;
EMparam.C4 = EP.c4;
EMparam.nh = EP.ndata;
FPd.FPstructure=&EMparam;
FPd.FP_Op=&EM_DR_EntropyConstantPrior_Operator;

FPd.mixcoeff=0.5;
FPd.mixstrategy=mix_const;

FPd.interp=interp;
FPd.it=0;
EM_DR_Init(&FPd);

#define MAX_ROOT_ITERATIONS 100
switch (EMparam.optLagrange_method) {
 case redchi2:
//    fptr = fopen("c:/temp/SASfit.dat","a+");
    do {
        if (FPd.failed || FPd.it >= FPd.maxsteps) x_max=x_max/REDFACTOR;
        chi2_max=Optimum_lambda4DR_EM_ME(x_max, &FPd);
    } while ((FPd.failed || FPd.it >= FPd.maxsteps) && x_max > FPd.relerror && FPd.interrupt == 0);
    x_hi = x_max/REDFACTOR;
    if ( FPd.interrupt == 0){
    do {
        if (FPd.failed || FPd.it >= FPd.maxsteps) x_hi=x_hi/REDFACTOR;
        chi2_hi=Optimum_lambda4DR_EM_ME(x_hi, &FPd);
    } while ((FPd.failed || FPd.it >= FPd.maxsteps) && x_max > FPd.relerror && FPd.interrupt == 0);
    x_lo = x_hi/REDFACTOR;
    }
    if ( FPd.interrupt == 0){
    do {
        if (FPd.failed || FPd.it >= FPd.maxsteps) x_lo=x_lo/REDFACTOR;
        chi2_lo=Optimum_lambda4DR_EM_ME(x_lo, &FPd);
    } while ((FPd.failed || FPd.it >= FPd.maxsteps) && x_max > FPd.relerror && FPd.interrupt == 0);
    }
//    fprintf(fptr,"smooth\t%16.14lg\tchi2\t%16.14lg\tGtest\t%16.14lg\tchi2test\t%16.14lg\tJSDtest\t%16.14lg\tKLD\t%16.14lg\tJSD\t%16.14lg\tSum1st\t%16.14lg\tSum2nd\t%16.14lg\tEntropy\t%16.14lg\n",
//                  x_lo, FPd.Chi2Norm,EMparam.Gtest,EMparam.chi2test,EMparam.JSDtest, FPd.KLD, FPd.JSD,FPd.Sum1stDeriv, FPd.Sum2ndDeriv,FPd.Entropy);

    while (chi2_lo > 0 && x_lo > FPd.relerror && FPd.interrupt == 0) { // && chi2_lo < chi2_hi
        if (chi2_max > chi2_hi) {
            x_max = x_hi;
            chi2_max = chi2_hi;
            x_hi = x_lo;
            chi2_hi = chi2_lo;
        }
        x_lo = x_lo/REDFACTOR;
        chi2_lo=Optimum_lambda4DR_EM_ME(x_lo, &FPd);
//    fprintf(fptr,"smooth\t%16.14lg\tchi2\t%16.14lg\tGtest\t%16.14lg\tchi2test\t%16.14lg\tJSDtest\t%16.14lg\tKLD\t%16.14lg\tJSD\t%16.14lg\tSum1st\t%16.14lg\tSum2nd\t%16.14lg\tEntropy\t%16.14lg\n",
//                  x_lo, FPd.Chi2Norm,EMparam.Gtest,EMparam.chi2test,EMparam.JSDtest, FPd.KLD, FPd.JSD,FPd.Sum1stDeriv, FPd.Sum2ndDeriv,FPd.Entropy);
//    fflush(fptr);
    }
//    fclose(fptr);
    sasfit_out ("x x_lo:%lg, x_max:%lg, x_hi:%lg\n", x_lo, x_max, x_hi);
    if (chi2_lo < 0 && chi2_max > 0 && FPd.interrupt == 0) {
        chi2_lo = chi2_lo+EMparam.chi2;
        chi2_max = chi2_max+EMparam.chi2;
        F.function = &Optimum_lambda4DR_EM_ME;
        F.params = &FPd;
        solver_type = gsl_root_fsolver_brent;
        solver = gsl_root_fsolver_alloc(solver_type);
        status = gsl_root_fsolver_set(solver, &F, x_lo, x_hi);
        if (status != GSL_SUCCESS || FPd.interrupt == 1) {
            sasfit_out("error(%d): %s, interrupt (%d)\n", status, gsl_strerror(status),FPd.interrupt);
            sasfit_err("Increasing precision parameter might help to avoid this error message.\n");
        }
        status = GSL_CONTINUE;
        for (i = 1; i <= MAX_ROOT_ITERATIONS && status == GSL_CONTINUE &&(FPd.interrupt == 0); ++i) {
        /* iterate one step of the solver */
            status = gsl_root_fsolver_iterate(solver);

        /* get the solver's current best solution and bounds */
            r = gsl_root_fsolver_root(solver);
            if (x_max != gsl_root_fsolver_x_upper (solver)) {
                x_max = gsl_root_fsolver_x_upper (solver);
                chi2_max = FPd.Chi2Norm;
            }
            if (x_lo != gsl_root_fsolver_x_lower (solver)) {
                x_lo = gsl_root_fsolver_x_lower (solver);
                chi2_lo = FPd.Chi2Norm;
            }

        /* Check to see if the solution is within 0.001 */
            status = gsl_root_test_interval(chi2_lo, chi2_max, 0, 0.001);
            if (FPd.PrintProgress==5) sasfit_out("%5d x_lo:%lg chi2_lo:%lg x_max:%lg chi2_max:%lg, r:%lg\n",i, x_lo, chi2_lo, x_max, chi2_max, r);
            if (status == GSL_SUCCESS) sasfit_out("Converged:\n");
        }

    /* Free the solver */
        gsl_root_fsolver_free(solver);

        if (status == GSL_CONTINUE) {
            sasfit_out("error: too many iterations");
        } else if (status != GSL_SUCCESS || FPd.interrupt == 1) {
            sasfit_out("error(%d): %s, interrupt (%d)\n", status, gsl_strerror(status),FPd.interrupt);
            sasfit_err("Increasing precision parameter might help to avoid this error message.\n");
        }
    } else {
//        x_max = 0.35;
//        x_lo = FPd.relerror;
//        x_hi = 1e-4;
        F.function = &Optimum_lambda4DR_EM_ME;
        F.params = &FPd;
        if (FPd.PrintProgress==5) sasfit_out ("xx x_lo:%lg, x_max:%lg, x_hi:%lg\n", x_lo, x_max, x_hi);
        Tmin = gsl_min_fminimizer_brent;
        smin = gsl_min_fminimizer_alloc (Tmin);
        if (FPd.PrintProgress==5) sasfit_out ("xxx %lg %lg %lg\n", x_lo, x_max, x_hi);
        status = gsl_min_fminimizer_set (smin, &F, x_hi, x_lo, x_max);
        if (status != GSL_SUCCESS || FPd.interrupt == 1) {
            sasfit_out("error(%d): %s, interrupt (%d)\n", status, gsl_strerror(status),FPd.interrupt);
            sasfit_err("Increasing precision parameter might help to avoid this error message.\n");
        } else {
           if (FPd.PrintProgress==5)  sasfit_out ("xxxx %lg %lg %lg\n", x_lo, x_max, x_hi);
            i=0;
            if ( FPd.interrupt == 0){
            do {
                i++;
                status = gsl_min_fminimizer_iterate (smin);

                if (x_max != gsl_min_fminimizer_x_upper (smin)) {
                    x_max = gsl_min_fminimizer_x_upper (smin);
                    chi2_max = chi2_hi;
                }
                if (x_lo != gsl_min_fminimizer_x_lower (smin)) {
                    x_lo = gsl_min_fminimizer_x_lower (smin);
                    chi2_lo = chi2_hi;
                }
                x_hi = gsl_min_fminimizer_x_minimum (smin);
                chi2_hi = FPd.Chi2Norm;

                status = gsl_min_test_interval (chi2_lo, chi2_max, 0.001, 0.0);

                if (status == GSL_SUCCESS) sasfit_out ("Converged:\n");

                if (FPd.PrintProgress==5) sasfit_out ("%5d x_lo:%lg chi2_lo:%lg x_max:%lg chi2_max:%lg x_hi:%lg chi2_hi:%lg\n", i, x_lo, chi2_lo, x_max, chi2_max, x_hi, chi2_hi);
            } while (status == GSL_CONTINUE && i < MAX_ROOT_ITERATIONS && FPd.interrupt == 0);
            }
            if (status == GSL_CONTINUE) {
                sasfit_err("error: too many iterations");
            } else if (status != GSL_SUCCESS || FPd.interrupt == 1) {
                sasfit_out("error(%d): %s, interrupt (%d)\n", status, gsl_strerror(status),FPd.interrupt);
                sasfit_err("Increasing precision parameter might help to avoid this error message.\n");
            }
        }
        gsl_min_fminimizer_free (smin);
    }
    break;
 case manual:
    FP_solver(&FPd);
    break;
 case Lcorner_o:
 case Lcorner2:
 case Lcorner_l:
 case Lcorner_w:
 case Lcorner_wo:
    L.Gtest=malloc(2*sizeof(scalar));
    L.chi2test=malloc(2*sizeof(scalar));
    L.chi2_error=malloc(2*sizeof(scalar));
    L.firstderiv=malloc(2*sizeof(scalar));
    L.secondderiv=malloc(2*sizeof(scalar));
    L.entropy=malloc(2*sizeof(scalar));
    L.lagrange=malloc(2*sizeof(scalar));
    L.Lradius=malloc(2*sizeof(scalar));
    do {
        if (FPd.failed || FPd.it >= FPd.maxsteps) x_max=x_max/REDFACTOR;
        chi2_max=Optimum_lambda4DR_EM_ME(x_max, &FPd);
    } while ((FPd.failed || FPd.it >= FPd.maxsteps) && x_max > FPd.relerror && FPd.interrupt==0);
    if (FPd.interrupt == 0) {
        L.lagrange[0]=x_max;
        L.Gtest[0] = EMparam.Gtest;
        L.chi2test[0]=EMparam.chi2test;
        L.chi2_error[0]=FPd.Chi2Norm;
        L.firstderiv[0]=FPd.Sum1stDeriv;
        L.secondderiv[0]=FPd.Sum2ndDeriv;
        L.entropy[0]=FPd.Entropy;

        x_hi = x_max/REDFACTOR;
        do {
            if (FPd.failed || FPd.it >= FPd.maxsteps) x_hi=x_hi/REDFACTOR;
            chi2_hi=Optimum_lambda4DR_EM_ME(x_hi, &FPd);
        } while ((FPd.failed || FPd.it >= FPd.maxsteps) && x_max > FPd.relerror && FPd.interrupt==0);
        x_lo=x_hi/REDFACTOR;

        L.lagrange[1]=x_hi;
        L.Gtest[1] = EMparam.Gtest;
        L.chi2test[1]=EMparam.chi2test;
        L.chi2_error[1]=FPd.Chi2Norm;
        L.firstderiv[1]=FPd.Sum1stDeriv;
        L.secondderiv[1]=FPd.Sum2ndDeriv;
        L.entropy[1]=FPd.Entropy;

        switch (EMparam.defL) {
        case Idendity:
            L.y1 = L.entropy[0];
            L.y2 = L.entropy[1];
            break;
        case first_deriv_eps_b:
        case first_deriv_eps_e:
        case first_deriv:
            L.y1 = L.firstderiv[0];
            L.y2 = L.firstderiv[1];
            break;
        case second_deriv_DD:
        case second_deriv_NN:
        case second_deriv_DN:
        case second_deriv_ND:
        case second_deriv:
        default:
            L.y1 = L.secondderiv[0];
            L.y2 = L.secondderiv[1];
            break;
        }
        L.u1 = log(L.Gtest[0]);
        L.u2 = log(L.Gtest[1]);
        L.v1 = log(L.y1);
        L.v2 = log(L.y2);
        if (EMparam.optLagrange_method == Lcorner2) {
            L.x1 = log(gsl_pow_2(L.lagrange[0]));
            L.y1 *=L.y1;
            L.x2 = log(gsl_pow_2(L.lagrange[1]));
            L.y2 *=L.y2;
        } else {
            L.x1 = log(L.Gtest[0]);
            L.y1 = log(L.y1);
            L.x2 = log(L.Gtest[1]);
            L.y2 = log(L.y2);
        }
        L.length=2;
        L.rmin=-1;
        L.optlagrange=L.lagrange[1];
        L.s21=0;

        while (x_lo > FPd.relerror && fabs(L.s21)<EMparam.maxslope && FPd.interrupt==0) { // && chi2_lo < chi2_hi
            x_lo = x_lo/REDFACTOR;
            if (FPd.PrintProgress==5) sasfit_out("start while loop L.length:%d with lambda value:%lg\n",L.length,x_lo);
            chi2_lo=Optimum_lambda4DR_EM_ME(x_lo, &FPd);
            L.length=L.length+1;
            i=L.length;
            L.Gtest=(scalar *) realloc(L.Gtest,i*sizeof(scalar));
            L.chi2test=(scalar *) realloc(L.chi2test,i*sizeof(scalar));
            L.chi2_error=(scalar *) realloc(L.chi2_error,i*sizeof(scalar));
            L.firstderiv=(scalar *) realloc(L.firstderiv,i*sizeof(scalar));
            L.secondderiv=(scalar *) realloc(L.secondderiv,i*sizeof(scalar));
            L.entropy=(scalar *) realloc(L.entropy,i*sizeof(scalar));
            L.lagrange=(scalar *) realloc(L.lagrange,i*sizeof(scalar));
            L.Lradius=(scalar *) realloc(L.Lradius,i*sizeof(scalar));

            L.lagrange[i-1]=x_lo;
            L.Gtest[i-1] = EMparam.Gtest;
            L.chi2test[i-1]=EMparam.chi2test;
            L.chi2_error[i-1]=FPd.Chi2Norm;
            L.firstderiv[i-1]=FPd.Sum1stDeriv;
            L.secondderiv[i-1]=FPd.Sum2ndDeriv;
            L.entropy[i-1]=FPd.Entropy;

//           L.x3 = log(L.Gtest[i-1]);
            switch (EMparam.defL) {
            case Idendity:
                L.y3 = L.entropy[i-1];
                break;
            case first_deriv_eps_b:
            case first_deriv_eps_e:
            case first_deriv:
                L.y3 = L.firstderiv[i-1];
                break;
            case second_deriv_DD:
            case second_deriv_NN:
            case second_deriv_DN:
            case second_deriv_ND:
            case second_deriv:
            default:
                L.y3 = L.secondderiv[i-1];
                break;
            }
            L.u3 = log(L.Gtest[i-1]);
            L.v3 = log(L.y3);
            if (EMparam.optLagrange_method == Lcorner2) {
                L.x3 = log(gsl_pow_2(L.lagrange[i-1]));
                L.y3 *=L.y3;
                L.signr = -1;
            } else {
                L.x3 = log(L.Gtest[i-1]);
                L.y3 = log(L.y3);
                L.signr = 1;
            }
            L.x21 = L.x2-L.x1;
            L.y21 = L.y2-L.y1;
            L.x31 = L.x3-L.x1;
            L.y31 = L.y3-L.y1;
            L.u31 = L.u3-L.u1;
            L.u21 = L.u2-L.u1;
            L.v31 = L.v3-L.v1;
            L.v21 = L.v2-L.v1;
            L.h21 = L.x21*L.x21 + L.y21*L.y21;
            L.h31 = L.x31*L.x31 + L.y31*L.y31;
            L.d = fabs(2.0 * (L.x21*L.y31 - L.x31*L.y21));
            L.s21= fabs(L.y21/L.x21);
            L.s31= fabs(L.y31/L.x31);
            L.signr = ((L.s31>L.s21) *2-1);
            L.s21= L.v21/L.u21;
            L.s31= L.v31/L.u31;
            r = L.signr*sqrt(L.h21*L.h31)*gsl_hypot(L.x3-L.x2,L.y3-L.y2) / L.d;
            L.Lradius[i-2] = -menger_r(L.x1,L.x2,L.x3,L.y1,L.y2,L.y3);
            if (FPd.PrintProgress==5) sasfit_out("r=%lg menger_r:%lg L.s21 %lg  L.y31:%lg\n",r,L.Lradius[i-2],L.s21, L.y31);
            if ((   L.Lradius[i-2] < L.rmin
                && L.Lradius[i-2] > 0
                && fabs(L.s21)<EMparam.maxslope)
                || (   L.rmin == -1.0
                    && L.Lradius[i-2]>0)) {
                L.rmin=L.Lradius[i-2];
                L.optlagrange=L.lagrange[i-2];
            }
            L.x1=L.x2;
            L.x2=L.x3;
            L.y1=L.y2;
            L.y2=L.y3;
            L.u1=L.u2;
            L.u2=L.u3;
            L.v1=L.v2;
            L.v2=L.v3;
        }
        L.Lradius[L.length-1]=L.Lradius[L.length-2];
        L.Lradius[0]=L.Lradius[1];

        EMparam.lambda=sasfit_Lcorner(&L,EMparam.optLagrange_method);
        sasfit_out("optimum for lambda:%lg\n",L.optlagrange);

        chi2_lo=Optimum_lambda4DR_EM_ME(EMparam.lambda, &FPd);
    }
    free(L.Gtest);
    free(L.chi2test);
    free(L.chi2_error);
    free(L.firstderiv);
    free(L.secondderiv);
    free(L.entropy);
    free(L.lagrange);
    free(L.Lradius);
    if (FPd.interrupt == 1) goto skipcodeconst;
    chi2_lo=Optimum_lambda4DR_EM_ME(EMparam.lambda, &FPd);
    if (FPd.interrupt == 1) goto skipcodeconst;
    break;
 default:
    sasfit_err("no valid option to determine Lagrange parameter\n");
}

goto skipcodeconst;
#define MAX_ROOT_ITERATIONS 100
switch (EMparam.optLagrange_method) {
 case redchi2:
    chi2_max=Optimum_lambda4DR_EM_ME(x_max, &FPd);
    if (FPd.interrupt == 1) goto skipcodeconst;
    chi2_hi=Optimum_lambda4DR_EM_ME(x_hi, &FPd);
    if (FPd.interrupt == 1) goto skipcodeconst;
    chi2_lo=Optimum_lambda4DR_EM_ME(x_lo, &FPd);
    if (FPd.interrupt == 1) goto skipcodeconst;
    while (chi2_lo > 0 && x_lo > FPd.relerror & FPd.interrupt == 0) {
        if (chi2_max > chi2_hi) {
            x_max = x_hi;
            chi2_max = chi2_hi;
            x_hi = x_lo;
            chi2_hi = chi2_lo;
        }
        x_lo = x_lo/10.;
        chi2_lo=Optimum_lambda4DR_EM_ME(x_lo, &FPd);
    }
    if (FPd.interrupt == 1) goto skipcodeconst;
    if (FPd.PrintProgress==5) sasfit_out ("x [%lg, %lg] %lg\n", x_lo, x_max, x_hi);
    if (chi2_lo < 0 && chi2_max > 0 && FPd.interrupt == 0) {
        chi2_lo = chi2_lo+EMparam.chi2;
        chi2_max = chi2_max+EMparam.chi2;
        F.function = &Optimum_lambda4DR_EM_ME;
        F.params = &FPd;
        solver_type = gsl_root_fsolver_brent;
        solver = gsl_root_fsolver_alloc(solver_type);
        status = gsl_root_fsolver_set(solver, &F, x_lo, x_hi);
        if (status != GSL_SUCCESS || FPd.interrupt == 1) {
            sasfit_out("error(%d): %s, interrupt (%d)\n", status, gsl_strerror(status),FPd.interrupt);
            sasfit_err("Increasing precision parameter might help to avoid this error message.\n");
        }
        status = GSL_CONTINUE;
        for (i = 1; i <= MAX_ROOT_ITERATIONS && status == GSL_CONTINUE && FPd.interrupt == 0; ++i) {
        /* iterate one step of the solver */
            status = gsl_root_fsolver_iterate(solver);

        /* get the solver's current best solution and bounds */
            r = gsl_root_fsolver_root(solver);
            if (x_max != gsl_root_fsolver_x_upper (solver)) {
                x_max = gsl_root_fsolver_x_upper (solver);
                chi2_max = FPd.Chi2Norm;
            }
            if (x_lo != gsl_root_fsolver_x_lower (solver)) {
                x_lo = gsl_root_fsolver_x_lower (solver);
                chi2_lo = FPd.Chi2Norm;
            }

        /* Check to see if the solution is within 0.001 */
            status = gsl_root_test_interval(chi2_lo, chi2_max, 0, 0.001);
            if (FPd.PrintProgress==5) sasfit_out("%5d x_lo:%lg chi2_lo:%lg x_max:%lg chi2_max:%lg, r:%lg\n",i, x_lo, chi2_lo, x_max, chi2_max, r);
            if (status == GSL_SUCCESS) sasfit_out("Converged:\n");
        }

    /* Free the solver */
        gsl_root_fsolver_free(solver);

        if (status == GSL_CONTINUE) {
            sasfit_out("error: too many iterations");
        } else if (status != GSL_SUCCESS || FPd.interrupt == 1) {
            sasfit_out("error(%d): %s, interrupt (%d)\n", status, gsl_strerror(status),FPd.interrupt);
            sasfit_err("Increasing precision parameter might help to avoid this error message.\n");
        }
    } else {
        x_max = 0.3;
        x_lo = FPd.relerror;
        x_hi = 1e-4;
        F.function = &Optimum_lambda4DR_EM_ME;
        F.params = &FPd;
        if (FPd.PrintProgress==5) sasfit_out ("xx %lg, %lg %lg\n", x_lo, x_max, x_hi);
        Tmin = gsl_min_fminimizer_brent;
        smin = gsl_min_fminimizer_alloc (Tmin);
        if (FPd.PrintProgress==5) sasfit_out ("xxx %lg %lg %lg\n", x_lo, x_max, x_hi);
        status = gsl_min_fminimizer_set (smin, &F, x_hi, x_lo, x_max);
        if (status != GSL_SUCCESS || FPd.interrupt == 1) {
            sasfit_out("error(%d): %s, interrupt (%d)\n", status, gsl_strerror(status),FPd.interrupt);
            sasfit_err("Increasing precision parameter might help to avoid this error message.\n");
        } else {
            sasfit_out ("xxxx %lg %lg %lg\n", x_lo, x_max, x_hi);
            i=0;
            do {
                i++;
                status = gsl_min_fminimizer_iterate (smin);

                if (x_max != gsl_min_fminimizer_x_upper (smin)) {
                    x_max = gsl_min_fminimizer_x_upper (smin);
                    chi2_max = chi2_hi;
                }
                if (x_lo != gsl_min_fminimizer_x_lower (smin)) {
                    x_lo = gsl_min_fminimizer_x_lower (smin);
                    chi2_lo = chi2_hi;
                }
                x_hi = gsl_min_fminimizer_x_minimum (smin);
                chi2_hi = FPd.Chi2Norm;

                status = gsl_min_test_interval (chi2_lo, chi2_max, 0.001, 0.0);

                if (status == GSL_SUCCESS) sasfit_out ("Converged:\n");

                if (FPd.PrintProgress==5) sasfit_out ("%5d x_lo:%lg chi2_lo:%lg x_max:%lg chi2_max:%lg x_hi:%lg chi2_hi:%lg\n", i, x_lo, chi2_lo, x_max, chi2_max, x_hi, chi2_hi);
            } while (status == GSL_CONTINUE && i < MAX_ROOT_ITERATIONS && FPd.interrupt == 0);
            if (status == GSL_CONTINUE) {
                sasfit_err("error: too many iterations");
            } else if (status != GSL_SUCCESS || FPd.interrupt == 1) {
                sasfit_out("error(%d): %s, interrupt (%d)\n", status, gsl_strerror(status),FPd.interrupt);
                sasfit_err("Increasing precision parameter might help to avoid this error message.\n");
            }
        }
        gsl_min_fminimizer_free (smin);
    }
    break;
 default:
    FP_solver(&FPd);
}

skipcodeconst:
if (FPd.interrupt == 1) sasfit_err("interrupt button has been pressed\n");

sasfit_out("it. %d eps %lg chi2 %lg\n",FPd.it, FPd.gNorm, FPd.Chi2Norm);
sasfit_out("it. %d KLD %lg JSD %lg\n",FPd.it, FPd.KLD, FPd.JSD);

Tcl_ResetResult(interp);
Tcl_DStringInit(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nR;i++) {
    sprintf(Buffer,"%lg",EMparam.r[i]);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nR;i++) {
	sprintf(Buffer,"%lg",EMparam.out[i]);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nh;i++) {
	sprintf(Buffer,"%lg",EMparam.h[i]);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nh;i++) {
	sprintf(Buffer,"%lg",EMparam.Ithwork[i]*(1+EMparam.em_weight*(EMparam.DIh[i]-1))+EP.c0);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringResult(interp,&DsBuffer);
Tcl_DStringFree(&DsBuffer);

EM_DR_Free(&FPd);

return TCL_OK;
}

int Sasfit_DR_EM_ME_adaptive_Cmd(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int        argc;
    char       **argv;
{
struct extrapolPar EP;
EM_param_t EMparam;
sasfit_fp_data FPd;
scalar p1,p2,p3, QR, eps,chi2;
int    i,j,k,l,itst;
scalar rmax;
sasfit_Lcurve L;
char   errstr[256],Buffer[256];
bool   error;
Tcl_DString DsBuffer;
FPd.tm=NULL;
    int status;
    const gsl_root_fsolver_type *solver_type;
    gsl_root_fsolver *solver;

    const gsl_min_fminimizer_type *Tmin;
    gsl_min_fminimizer *smin;

    gsl_function F;
    double r, sm;
    double x_lo = 0.1, x_hi = 1, x_max=10;
    double chi2_lo, chi2_hi, chi2_max;


error = FALSE;

if (argc != 3) {
   sasfit_err("wrong # args; should be sasfit_DR_EM ?EMOptions? ?xye_data?\n");
   return TCL_ERROR;
}

if (TCL_ERROR == get_EP(clientData,interp,argv,&EP,&EMparam.h,&EMparam.Iexp,&EMparam.DIh)) {
   return TCL_ERROR;
}

if (TCL_ERROR == get_EM(clientData,interp,&EMparam)) {
   return TCL_ERROR;
}

gsl_set_error_handler_off();

EMparam.C0 = EP.c0;
EMparam.C4 = EP.c4;
EMparam.nh = EP.ndata;
FPd.FPstructure=&EMparam;
FPd.FP_Op=&EM_DR_EntropyAdaptivePrior_Operator;

FPd.mixcoeff=0.5;
FPd.mixstrategy=mix_const;
//FPd.PrintProgress=0;

FPd.interp=interp;
FPd.it=0;
strcpy(EMparam.smooth_type,"Gauss");
EM_DR_Init(&FPd);
EMparam.smooth=1;

#define MAX_ROOT_ITERATIONS 100
switch (EMparam.optLagrange_method) {
 case redchi2:
//    fptr = fopen("c:/temp/SASfit.dat","a+");
    do {
        if (FPd.failed || FPd.it >= FPd.maxsteps) x_max=x_max/REDFACTOR;
        chi2_max=Optimum_lambda4DR_EM_ME(x_max, &FPd);
    } while ((FPd.failed || FPd.it >= FPd.maxsteps && FPd.interrupt == 0) && x_max > FPd.relerror );
    x_hi = x_max/REDFACTOR;
    if ( FPd.interrupt == 0){
    do {
        if (FPd.failed || FPd.it >= FPd.maxsteps) x_hi=x_hi/REDFACTOR;
        chi2_hi=Optimum_lambda4DR_EM_ME(x_hi, &FPd);
    } while ((FPd.failed || FPd.it >= FPd.maxsteps && FPd.interrupt == 0) && x_max > FPd.relerror );
    x_lo = x_hi/REDFACTOR;
    }
    if ( FPd.interrupt == 0){
    do {
        if (FPd.failed || FPd.it >= FPd.maxsteps) x_lo=x_lo/REDFACTOR;
        chi2_lo=Optimum_lambda4DR_EM_ME(x_lo, &FPd);
    } while ((FPd.failed || FPd.it >= FPd.maxsteps) && x_max > FPd.relerror && FPd.interrupt == 0);
    }
//    fprintf(fptr,"smooth\t%16.14lg\tchi2\t%16.14lg\tGtest\t%16.14lg\tchi2test\t%16.14lg\tJSDtest\t%16.14lg\tKLD\t%16.14lg\tJSD\t%16.14lg\tSum1st\t%16.14lg\tSum2nd\t%16.14lg\tEntropy\t%16.14lg\n",
//                  x_lo, FPd.Chi2Norm,EMparam.Gtest,EMparam.chi2test,EMparam.JSDtest, FPd.KLD, FPd.JSD,FPd.Sum1stDeriv, FPd.Sum2ndDeriv,FPd.Entropy);

    while (chi2_lo > 0 && x_lo > FPd.relerror && FPd.interrupt == 0 ) { // && chi2_lo < chi2_hi
        if (chi2_max > chi2_hi) {
            x_max = x_hi;
            chi2_max = chi2_hi;
            x_hi = x_lo;
            chi2_hi = chi2_lo;
        }
        x_lo = x_lo/REDFACTOR;
        chi2_lo=Optimum_lambda4DR_EM_ME(x_lo, &FPd);
//    fprintf(fptr,"smooth\t%16.14lg\tchi2\t%16.14lg\tGtest\t%16.14lg\tchi2test\t%16.14lg\tJSDtest\t%16.14lg\tKLD\t%16.14lg\tJSD\t%16.14lg\tSum1st\t%16.14lg\tSum2nd\t%16.14lg\tEntropy\t%16.14lg\n",
//                  x_lo, FPd.Chi2Norm,EMparam.Gtest,EMparam.chi2test,EMparam.JSDtest, FPd.KLD, FPd.JSD,FPd.Sum1stDeriv, FPd.Sum2ndDeriv,FPd.Entropy);
//    fflush(fptr);
    }
//    fclose(fptr);
    if (FPd.PrintProgress==5) sasfit_out ("x x_lo:%lg, x_max:%lg, x_hi:%lg\n", x_lo, x_max, x_hi);
    if (chi2_lo < 0 && chi2_max > 0 && FPd.interrupt == 0) {
        chi2_lo = chi2_lo+EMparam.chi2;
        chi2_max = chi2_max+EMparam.chi2;
        F.function = &Optimum_lambda4DR_EM_ME;
        F.params = &FPd;
        solver_type = gsl_root_fsolver_brent;
        solver = gsl_root_fsolver_alloc(solver_type);
        status = gsl_root_fsolver_set(solver, &F, x_lo, x_hi);
        if (status != GSL_SUCCESS || FPd.interrupt == 1) {
            sasfit_out("error(%d): %s, interrupt (%d)\n", status, gsl_strerror(status),FPd.interrupt);
            sasfit_err("Increasing precision parameter might help to avoid this error message.\n");
        }
        status = GSL_CONTINUE;
        for (i = 1; i <= MAX_ROOT_ITERATIONS && status == GSL_CONTINUE &&(FPd.interrupt == 0); ++i) {
        /* iterate one step of the solver */
            status = gsl_root_fsolver_iterate(solver);

        /* get the solver's current best solution and bounds */
            r = gsl_root_fsolver_root(solver);
            if (x_max != gsl_root_fsolver_x_upper (solver)) {
                x_max = gsl_root_fsolver_x_upper (solver);
                chi2_max = FPd.Chi2Norm;
            }
            if (x_lo != gsl_root_fsolver_x_lower (solver)) {
                x_lo = gsl_root_fsolver_x_lower (solver);
                chi2_lo = FPd.Chi2Norm;
            }

        /* Check to see if the solution is within 0.001 */
            status = gsl_root_test_interval(chi2_lo, chi2_max, 0, 0.001);
            if (FPd.PrintProgress==5) sasfit_out("%5d x_lo:%lg chi2_lo:%lg x_max:%lg chi2_max:%lg, r:%lg\n",i, x_lo, chi2_lo, x_max, chi2_max, r);
            if (status == GSL_SUCCESS) sasfit_out("Converged:\n");
        }

    /* Free the solver */
        gsl_root_fsolver_free(solver);

        if (status == GSL_CONTINUE) {
            sasfit_out("error: too many iterations");
        } else if (status != GSL_SUCCESS || FPd.interrupt == 1) {
            sasfit_out("error(%d): %s, interrupt (%d)\n", status, gsl_strerror(status),FPd.interrupt);
            sasfit_err("Increasing precision parameter might help to avoid this error message.\n");
        }
    } else {
//        x_max = 0.35;
//        x_lo = FPd.relerror;
//        x_hi = 1e-4;
        F.function = &Optimum_lambda4DR_EM_ME;
        F.params = &FPd;
        if (FPd.PrintProgress==5) sasfit_out ("xx x_lo:%lg, x_max:%lg, x_hi:%lg\n", x_lo, x_max, x_hi);
        Tmin = gsl_min_fminimizer_brent;
        smin = gsl_min_fminimizer_alloc (Tmin);
        if (FPd.PrintProgress==5) sasfit_out ("xxx %lg %lg %lg\n", x_lo, x_max, x_hi);
        status = gsl_min_fminimizer_set (smin, &F, x_hi, x_lo, x_max);
        if (status != GSL_SUCCESS || FPd.interrupt == 1) {
            sasfit_out("error(%d): %s, interrupt (%d)\n", status, gsl_strerror(status),FPd.interrupt);
            sasfit_err("Increasing precision parameter might help to avoid this error message.\n");
        } else {
            if (FPd.PrintProgress==5) sasfit_out ("xxxx %lg %lg %lg\n", x_lo, x_max, x_hi);
            i=0;
            if ( FPd.interrupt == 0){
            do {
                i++;
                status = gsl_min_fminimizer_iterate (smin);

                if (x_max != gsl_min_fminimizer_x_upper (smin)) {
                    x_max = gsl_min_fminimizer_x_upper (smin);
                    chi2_max = chi2_hi;
                }
                if (x_lo != gsl_min_fminimizer_x_lower (smin)) {
                    x_lo = gsl_min_fminimizer_x_lower (smin);
                    chi2_lo = chi2_hi;
                }
                x_hi = gsl_min_fminimizer_x_minimum (smin);
                chi2_hi = FPd.Chi2Norm;

                status = gsl_min_test_interval (chi2_lo, chi2_max, 0.001, 0.0);

                if (status == GSL_SUCCESS) sasfit_out ("Converged:\n");

                if (FPd.PrintProgress==5) sasfit_out ("%5d x_lo:%lg chi2_lo:%lg x_max:%lg chi2_max:%lg x_hi:%lg chi2_hi:%lg\n", i, x_lo, chi2_lo, x_max, chi2_max, x_hi, chi2_hi);
            } while (status == GSL_CONTINUE && i < MAX_ROOT_ITERATIONS && FPd.interrupt == 0);
            }
            if (status == GSL_CONTINUE) {
                sasfit_err("error: too many iterations");
            } else if (status != GSL_SUCCESS || FPd.interrupt == 1) {
                sasfit_out("error(%d): %s, interrupt (%d)\n", status, gsl_strerror(status),FPd.interrupt);
                sasfit_err("Increasing precision parameter might help to avoid this error message.\n");
            }
        }
        gsl_min_fminimizer_free (smin);
    }
    break;
 case manual:
    FP_solver(&FPd);
    break;
 case Lcorner_o:
 case Lcorner2:
 case Lcorner_l:
 case Lcorner_w:
 case Lcorner_wo:
    L.Gtest=malloc(2*sizeof(scalar));
    L.chi2test=malloc(2*sizeof(scalar));
    L.chi2_error=malloc(2*sizeof(scalar));
    L.firstderiv=malloc(2*sizeof(scalar));
    L.secondderiv=malloc(2*sizeof(scalar));
    L.entropy=malloc(2*sizeof(scalar));
    L.lagrange=malloc(2*sizeof(scalar));
    L.Lradius=malloc(2*sizeof(scalar));
    do {
        if (FPd.failed || FPd.it >= FPd.maxsteps) x_max=x_max/REDFACTOR;
        chi2_max=Optimum_lambda4DR_EM_ME(x_max, &FPd);
    } while ((FPd.failed || FPd.it >= FPd.maxsteps) && x_max > FPd.relerror && FPd.interrupt==0);
    if (FPd.interrupt == 0) {
        L.lagrange[0]=x_max;
        L.Gtest[0] = EMparam.Gtest;
        L.chi2test[0]=EMparam.chi2test;
        L.chi2_error[0]=FPd.Chi2Norm;
        L.firstderiv[0]=FPd.Sum1stDeriv;
        L.secondderiv[0]=FPd.Sum2ndDeriv;
        L.entropy[0]=FPd.Entropy;

        x_hi = x_max/REDFACTOR;
        do {
            if (FPd.failed || FPd.it >= FPd.maxsteps) x_hi=x_hi/REDFACTOR;
            chi2_hi=Optimum_lambda4DR_EM_ME(x_hi, &FPd);
        } while ((FPd.failed || FPd.it >= FPd.maxsteps) && x_max > FPd.relerror && FPd.interrupt==0);
        x_lo=x_hi/REDFACTOR;

        L.lagrange[1]=x_hi;
        L.Gtest[1] = EMparam.Gtest;
        L.chi2test[1]=EMparam.chi2test;
        L.chi2_error[1]=FPd.Chi2Norm;
        L.firstderiv[1]=FPd.Sum1stDeriv;
        L.secondderiv[1]=FPd.Sum2ndDeriv;
        L.entropy[1]=FPd.Entropy;

        switch (EMparam.defL) {
        case Idendity:
            L.y1 = L.entropy[0];
            L.y2 = L.entropy[1];
            break;
        case first_deriv_eps_b:
        case first_deriv_eps_e:
        case first_deriv:
            L.y1 = L.firstderiv[0];
            L.y2 = L.firstderiv[1];
            break;
        case second_deriv_DD:
        case second_deriv_NN:
        case second_deriv_DN:
        case second_deriv_ND:
        case second_deriv:
        default:
            L.y1 = L.secondderiv[0];
            L.y2 = L.secondderiv[1];
            break;
        }
        L.u1 = log(L.Gtest[0]);
        L.u2 = log(L.Gtest[1]);
        L.v1 = log(L.y1);
        L.v2 = log(L.y2);
        if (EMparam.optLagrange_method == Lcorner2) {
            L.x1 = log(gsl_pow_2(L.lagrange[0]));
            L.y1 *=L.y1;
            L.x2 = log(gsl_pow_2(L.lagrange[1]));
            L.y2 *=L.y2;
        } else {
            L.x1 = log(L.Gtest[0]);
            L.y1 = log(L.y1);
            L.x2 = log(L.Gtest[1]);
            L.y2 = log(L.y2);
        }
        L.length=2;
        L.rmin=-1;
        L.optlagrange=L.lagrange[1];
        L.s21=0;

        while (x_lo > FPd.relerror && fabs(L.s21)<EMparam.maxslope && FPd.interrupt==0) { // && chi2_lo < chi2_hi
            x_lo = x_lo/REDFACTOR;
            if (FPd.PrintProgress==5) sasfit_out("start while loop L.length:%d with lambda value:%lg\n",L.length,x_lo);
            chi2_lo=Optimum_lambda4DR_EM_ME(x_lo, &FPd);
            L.length=L.length+1;
            i=L.length;
            L.Gtest=(scalar *) realloc(L.Gtest,i*sizeof(scalar));
            L.chi2test=(scalar *) realloc(L.chi2test,i*sizeof(scalar));
            L.chi2_error=(scalar *) realloc(L.chi2_error,i*sizeof(scalar));
            L.firstderiv=(scalar *) realloc(L.firstderiv,i*sizeof(scalar));
            L.secondderiv=(scalar *) realloc(L.secondderiv,i*sizeof(scalar));
            L.entropy=(scalar *) realloc(L.entropy,i*sizeof(scalar));
            L.lagrange=(scalar *) realloc(L.lagrange,i*sizeof(scalar));
            L.Lradius=(scalar *) realloc(L.Lradius,i*sizeof(scalar));

            L.lagrange[i-1]=x_lo;
            L.Gtest[i-1] = EMparam.Gtest;
            L.chi2test[i-1]=EMparam.chi2test;
            L.chi2_error[i-1]=FPd.Chi2Norm;
            L.firstderiv[i-1]=FPd.Sum1stDeriv;
            L.secondderiv[i-1]=FPd.Sum2ndDeriv;
            L.entropy[i-1]=FPd.Entropy;

//        L.x3 = log(L.Gtest[i-1]);
            switch (EMparam.defL) {
            case Idendity:
                L.y3 = L.entropy[i-1];
                break;
            case first_deriv_eps_b:
            case first_deriv_eps_e:
            case first_deriv:
                L.y3 = L.firstderiv[i-1];
                break;
            case second_deriv_DD:
            case second_deriv_NN:
            case second_deriv_DN:
            case second_deriv_ND:
            case second_deriv:
            default:
                L.y3 = L.secondderiv[i-1];
                break;
            }
            L.u3 = log(L.Gtest[i-1]);
            L.v3 = log(L.y3);
            if (EMparam.optLagrange_method == Lcorner2) {
                L.x3 = log(gsl_pow_2(L.lagrange[i-1]));
                L.y3 *=L.y3;
                L.signr = -1;
            } else {
                L.x3 = log(L.Gtest[i-1]);
                L.y3 = log(L.y3);
                L.signr = 1;
            }
            L.x21 = L.x2-L.x1;
            L.y21 = L.y2-L.y1;
            L.x31 = L.x3-L.x1;
            L.y31 = L.y3-L.y1;
            L.u31 = L.u3-L.u1;
            L.u21 = L.u2-L.u1;
            L.v31 = L.v3-L.v1;
            L.v21 = L.v2-L.v1;
            L.h21 = L.x21*L.x21 + L.y21*L.y21;
            L.h31 = L.x31*L.x31 + L.y31*L.y31;
            L.d = fabs(2.0 * (L.x21*L.y31 - L.x31*L.y21));
            L.s21= fabs(L.y21/L.x21);
            L.s31= fabs(L.y31/L.x31);
            L.signr = ((L.s31>L.s21) *2-1);
            L.s21= L.v21/L.u21;
            L.s31= L.v31/L.u31;
            r = L.signr*sqrt(L.h21*L.h31)*gsl_hypot(L.x3-L.x2,L.y3-L.y2) / L.d;
            L.Lradius[i-2] = -menger_r(L.x1,L.x2,L.x3,L.y1,L.y2,L.y3);
            if (FPd.PrintProgress>0) {
                sasfit_out("r=%lg menger_r:%lg L.s21 %lg  L.y31:%lg\n",r,L.Lradius[i-2],L.s21, L.y31);
            }
            if ((   L.Lradius[i-2] < L.rmin
                && L.Lradius[i-2] > 0
                && fabs(L.s21)<EMparam.maxslope)
                || (   L.rmin == -1.0
                    && L.Lradius[i-2]>0)) {
                L.rmin=L.Lradius[i-2];
                L.optlagrange=L.lagrange[i-2];
            }
            L.x1=L.x2;
            L.x2=L.x3;
            L.y1=L.y2;
            L.y2=L.y3;
            L.u1=L.u2;
            L.u2=L.u3;
            L.v1=L.v2;
            L.v2=L.v3;
        }
        L.Lradius[L.length-1]=L.Lradius[L.length-2];
        L.Lradius[0]=L.Lradius[1];

        EMparam.lambda=sasfit_Lcorner(&L,EMparam.optLagrange_method);
        sasfit_out("optimum for lambda:%lg\n",L.optlagrange);
    }
    free(L.Gtest);
    free(L.chi2test);
    free(L.chi2_error);
    free(L.firstderiv);
    free(L.secondderiv);
    free(L.entropy);
    free(L.lagrange);
    free(L.Lradius);
    if (FPd.interrupt == 1) goto skipcodeadaptive;
    chi2_lo=Optimum_lambda4DR_EM_ME(EMparam.lambda, &FPd);
    if (FPd.interrupt == 1) goto skipcodeadaptive;
    break;
 default:
    sasfit_err("no valid option to determine Lagrange parameter\n");
}

#define MAX_ROOT_ITERATIONS 100
switch (EMparam.optLagrange_method) {
 case redchi2:
    chi2_max=Optimum_lambda4DR_EM_ME(x_max, &FPd);
    if (FPd.interrupt == 1) goto skipcodeadaptive;
    chi2_hi=Optimum_lambda4DR_EM_ME(x_hi, &FPd);
    if (FPd.interrupt == 1) goto skipcodeadaptive;
    chi2_lo=Optimum_lambda4DR_EM_ME(x_lo, &FPd);
    if (FPd.interrupt == 1) goto skipcodeadaptive;
    while (chi2_lo > 0 && x_lo > FPd.relerror && FPd.interrupt == 0 ) {
        if (chi2_max > chi2_hi) {
            x_max = x_hi;
            chi2_max = chi2_hi;
            x_hi = x_lo;
            chi2_hi = chi2_lo;
        }
        x_lo = x_lo/10.;
        chi2_lo=Optimum_lambda4DR_EM_ME(x_lo, &FPd);
    }
    if (FPd.interrupt == 1) goto skipcodeadaptive;
    if (FPd.PrintProgress==5) sasfit_out ("x [%lg, %lg] %lg\n", x_lo, x_max, x_hi);
    if (chi2_lo < 0 && chi2_max > 0 && FPd.interrupt == 0) {
        chi2_lo = chi2_lo+EMparam.chi2;
        chi2_max = chi2_max+EMparam.chi2;
        F.function = &Optimum_lambda4DR_EM_ME;
        F.params = &FPd;
        solver_type = gsl_root_fsolver_brent;
        solver = gsl_root_fsolver_alloc(solver_type);
        status = gsl_root_fsolver_set(solver, &F, x_lo, x_hi);
        if (status != GSL_SUCCESS || FPd.interrupt == 1) {
            sasfit_out("error(%d): %s, interrupt (%d)\n", status, gsl_strerror(status),FPd.interrupt);
            sasfit_err("Increasing precision parameter might help to avoid this error message.\n");
        }
        status = GSL_CONTINUE;
        for (i = 1; i <= MAX_ROOT_ITERATIONS && status == GSL_CONTINUE && FPd.interrupt == 0; ++i) {
        /* iterate one step of the solver */
            status = gsl_root_fsolver_iterate(solver);

        /* get the solver's current best solution and bounds */
            r = gsl_root_fsolver_root(solver);
            if (x_max != gsl_root_fsolver_x_upper (solver)) {
                x_max = gsl_root_fsolver_x_upper (solver);
                chi2_max = FPd.Chi2Norm;
            }
            if (x_lo != gsl_root_fsolver_x_lower (solver)) {
                x_lo = gsl_root_fsolver_x_lower (solver);
                chi2_lo = FPd.Chi2Norm;
            }

        /* Check to see if the solution is within 0.001 */
            status = gsl_root_test_interval(chi2_lo, chi2_max, 0, 0.001);
            if (FPd.PrintProgress==5) sasfit_out("%5d x_lo:%lg chi2_lo:%lg x_max:%lg chi2_max:%lg, r:%lg\n",i, x_lo, chi2_lo, x_max, chi2_max, r);
            if (status == GSL_SUCCESS) sasfit_out("Converged:\n");
        }

    /* Free the solver */
        gsl_root_fsolver_free(solver);

        if (status == GSL_CONTINUE) {
            sasfit_out("error: too many iterations");
        } else if (status != GSL_SUCCESS || FPd.interrupt == 1) {
            sasfit_out("error(%d): %s, interrupt (%d)\n", status, gsl_strerror(status),FPd.interrupt);
            sasfit_err("Increasing precision parameter might help to avoid this error message.\n");
        }
    } else {
        x_max = 0.3;
        x_lo = FPd.relerror;
        x_hi = 1e-4;
        F.function = &Optimum_lambda4DR_EM_ME;
        F.params = &FPd;
        if (FPd.PrintProgress==5) sasfit_out ("xx %lg, %lg %lg\n", x_lo, x_max, x_hi);
        Tmin = gsl_min_fminimizer_brent;
        smin = gsl_min_fminimizer_alloc (Tmin);
        if (FPd.PrintProgress==5) sasfit_out ("xxx %lg %lg %lg\n", x_lo, x_max, x_hi);
        status = gsl_min_fminimizer_set (smin, &F, x_hi, x_lo, x_max);
        if (status != GSL_SUCCESS || FPd.interrupt == 1) {
            sasfit_out("error(%d): %s, interrupt (%d)\n", status, gsl_strerror(status),FPd.interrupt);
            sasfit_err("Increasing precision parameter might help to avoid this error message.\n");
        }
        if (FPd.PrintProgress==5) sasfit_out ("xxxx %lg %lg %lg\n", x_lo, x_max, x_hi);
        i=0;
        do {
            i++;
            status = gsl_min_fminimizer_iterate (smin);

            if (x_max != gsl_min_fminimizer_x_upper (smin)) {
                x_max = gsl_min_fminimizer_x_upper (smin);
                chi2_max = chi2_hi;
            }
            if (x_lo != gsl_min_fminimizer_x_lower (smin)) {
                x_lo = gsl_min_fminimizer_x_lower (smin);
                chi2_lo = chi2_hi;
            }
            x_hi = gsl_min_fminimizer_x_minimum (smin);
            chi2_hi = FPd.Chi2Norm;

            status = gsl_min_test_interval (chi2_lo, chi2_max, 0.001, 0.0);

            if (status == GSL_SUCCESS) sasfit_out ("Converged:\n");

            if (FPd.PrintProgress==5) sasfit_out ("%5d x_lo:%lg chi2_lo:%lg x_max:%lg chi2_max:%lg x_hi:%lg chi2_hi:%lg\n", i, x_lo, chi2_lo, x_max, chi2_max, x_hi, chi2_hi);
        } while (status == GSL_CONTINUE && i < MAX_ROOT_ITERATIONS && FPd.interrupt == 0);
        if (status == GSL_CONTINUE) {
            sasfit_err("error: too many iterations");
        } else if (status != GSL_SUCCESS || FPd.interrupt == 1) {
            sasfit_out("error(%d): %s, interrupt (%d)\n", status, gsl_strerror(status),FPd.interrupt);
            sasfit_err("Increasing precision parameter might help to avoid this error message.\n");
        }
        gsl_min_fminimizer_free (smin);
    }
    break;
 default:
    FP_solver(&FPd);
}
skipcodeadaptive:
if (FPd.interrupt == 1) sasfit_err("interrupt button has been pressed\n");


sasfit_out("it. %d eps %lg chi2 %lg\n",FPd.it, FPd.gNorm, FPd.Chi2Norm);
sasfit_out("it. %d KLD %lg JSD %lg\n",FPd.it, FPd.KLD, FPd.JSD);

Tcl_ResetResult(interp);
Tcl_DStringInit(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nR;i++) {
    sprintf(Buffer,"%lg",EMparam.r[i]);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nR;i++) {
	sprintf(Buffer,"%lg",EMparam.out[i]);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nh;i++) {
	sprintf(Buffer,"%lg",EMparam.h[i]);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nh;i++) {
	sprintf(Buffer,"%lg",EMparam.Ithwork[i]*(1+EMparam.em_weight*(EMparam.DIh[i]-1))+EP.c0);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringResult(interp,&DsBuffer);
Tcl_DStringFree(&DsBuffer);

EM_DR_Free(&FPd);

return TCL_OK;
}


int sasfit_svd_linpack ( int m, int n, double a[], double u[], double s[],
  double v[] )

/******************************************************************************/
/*
  Purpose:

    SASFIT_SVD_LINPACK gets the SVD of a matrix using a call to LINPACK.

  Discussion:

    The singular value decomposition of a real MxN matrix A has the form:

      A = U * S * V'

    where

      U is MxM orthogonal,
      S is MxN, and entirely zero except for the diagonal;
      V is NxN orthogonal.

    Moreover, the nonzero entries of S are positive, and appear
    in order, from largest magnitude to smallest.

    This routine calls the LINPACK routine DSVDC to compute the
    factorization.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    19 June 2012

  Author:

    John Burkardt

  Parameters:

    Input, int M, N, the number of rows and columns in the matrix A.

    Input, double A[M*N], the matrix whose singular value
    decomposition we are investigating.

    Output, double U[M*M], S[M*N], V[N*N], the factors
    that form the singular value decomposition of A.
*/
{
  double *a_copy;
  double *e;
  int i;
  int info;
  int j;
  int lda;
  int ldu;
  int ldv;
  int job;
  int lwork;
  double *sdiag;
  double *work;
/*
  The correct size of E and SDIAG is min ( m+1, n).
*/
  a_copy = ( double * ) malloc ( m * n * sizeof ( double ) );
  e = ( double * ) malloc ( ( m + n ) * sizeof ( double ) );
  sdiag = ( double * ) malloc ( ( m + n )  * sizeof ( double ) );
  work = ( double * ) malloc ( m * sizeof ( double ) );
/*
  Compute the eigenvalues and eigenvectors.
*/
  job = 11;
  lda = m;
  ldu = m;
  ldv = n;
/*
  The input matrix is destroyed by the routine.  Since we need to keep
  it around, we only pass a copy to the routine.
*/
  for ( j = 0; j < n; j++ )
  {
    for ( i = 0; i < m; i++ )
    {
      a_copy[i+j*m] = a[i+j*m];
    }
  }
  info = dsvdc ( a_copy, lda, m, n, sdiag, e, u, ldu, v, ldv, work, job );

  if ( info != 0 )
  {
    sasfit_out ( "\n" );
    sasfit_out ( "SASFIT_SVD_LINPACK - Failure!\n" );
    sasfit_out ( "  The SVD could not be calculated.\n" );
    sasfit_out ( "  LINPACK routine DSVDC returned a nonzero\n" );
    sasfit_out ( "  value of the error flag, INFO = %d\n", info );
    return info;
  }
/*
  Make the MxN matrix S from the diagonal values in SDIAG.
*/
  for ( j = 0; j < n; j++ )
  {
    for ( i = 0; i < m; i++ )
    {
      if ( i == j )
      {
        s[i+j*m] = sdiag[i];
      }
      else
      {
        s[i+j*m] = 0.0;
      }
    }
  }
/*
  Note that we do NOT need to transpose the V that comes out of LINPACK!
*/
  free ( a_copy );
  free ( e );
  free ( sdiag );
  free ( work );

  return 0;
}
/******************************************************************************/



int Sasfit_DR_linReg_Cmd(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int        argc;
    char       **argv;
{
struct extrapolPar EP;
EM_param_t EMparam;
sasfit_fp_data FPd;

scalar p1,p2,p3, QR, eps,chi2,lambda_opt;
int    i,j,k,l,itst,l0,l1,l2;
scalar rmax;
char   errstr[256],Buffer[256];
bool   error;
Tcl_DString DsBuffer;
gsl_matrix *L;
gsl_vector *Gvec;
gsl_matrix *LQR;
gsl_vector *Ltau;
gsl_matrix *M;
gsl_matrix *X;
gsl_matrix *Xs;
gsl_vector *y;
gsl_vector *ys;
gsl_vector *weights;
double **AAA, *AAmem, *bb, *xx, *wp, *zzp, *Af2c, *Amn, *U, *S, *V;
int infoDSVDC;
int *indexp;
int n,m, mda, ierr;

gsl_multifit_linear_workspace *w;
gsl_vector *c;     /* OLS solution */
gsl_vector *c_lcurve;   /* regularized solution (L-curve) */
gsl_vector *c_gcv;      /* regularized solution (GCV) */
gsl_vector *c_reg; /* regularized solution */
gsl_vector *reg_param, *Breg_param;
gsl_vector *MengerR;
gsl_vector *slope;
gsl_vector *rho, *Brho;  /* residual norms */
gsl_vector *eta, *Beta;  /* solution norms */
gsl_vector *nnls_rho;  /* residual norms */
gsl_vector *nnls_eta;  /* solution norms */
double lambda_l, lambda_gcv, G_gcv;                           /* optimal regularization parameter */
size_t reg_idx;                          /* index of optimal lambda */
double rcond;                            /* reciprocal condition number of X */
double chisq, rnorm, snorm;

FPd.tm=NULL;
error = FALSE;

if (argc != 3) {
   sasfit_err("wrong # args; should be sasfit_DR_linReg ?EMOptions? ?xye_data?\n");
   return TCL_ERROR;
}

if (TCL_ERROR == get_EP(clientData,interp,argv,&EP,&EMparam.h,&EMparam.Iexp,&EMparam.DIh)) {
   return TCL_ERROR;
}

if (TCL_ERROR == get_EM(clientData,interp,&EMparam)) {
   return TCL_ERROR;
}

EMparam.C0 = EP.c0;
EMparam.C4 = EP.c4;
EMparam.nh = EP.ndata;
FPd.FPstructure=&EMparam;
FPd.FP_Op=NULL;
FPd.mixcoeff=0.5;
FPd.mixstrategy=mix_const;

FPd.interp=interp;
EM_DR_Init(&FPd);
//FPd.PrintProgress=2;

if (EMparam.nR > EMparam.nh) {
//    EMparam.nR=EMparam.nh;
//    sasfit_out("number of points in r-space has been reduced to %d\n",EMparam.nR);
sasfit_out("number of points in q-space has been increase and filled with zeros to %d->%d\n",EMparam.nh,EMparam.nR);
    X = gsl_matrix_calloc(EMparam.nR, EMparam.nR);
    y = gsl_vector_calloc(EMparam.nR);
    weights = gsl_vector_calloc(EMparam.nR);
    w = gsl_multifit_linear_alloc(EMparam.nR, EMparam.nR);
    for (i=0;i<EMparam.nh;i++) {
        gsl_vector_set(y,i,EMparam.Ih[i]);
        gsl_vector_set(weights,i,1./gsl_pow_2(EMparam.DIh[i]));
        for (j=0;j<EMparam.nR;j++) {
            gsl_matrix_set(X,i,j,EMparam.dr[j]*EMparam.A[i][j]);
        }
    }
    for (i=EMparam.nh;i<EMparam.nR;i++) {
        gsl_vector_set(y,i,0);
        gsl_vector_set(weights,i,1.0);
        for (j=0;j<EMparam.nR;j++) {
            gsl_matrix_set(X,i,j,0);
        }
    }
} else {
    X = gsl_matrix_calloc(EMparam.nh, EMparam.nR);
    y = gsl_vector_calloc(EMparam.nh);
    weights = gsl_vector_calloc(EMparam.nh);
    w = gsl_multifit_linear_alloc(EMparam.nh, EMparam.nR);
    for (i=0;i<EMparam.nh;i++) {
        gsl_vector_set(y,i,EMparam.Ih[i]);
        gsl_vector_set(weights,i,1./gsl_pow_2(EMparam.DIh[i]));
        for (j=0;j<EMparam.nR;j++) {
            gsl_matrix_set(X,i,j,EMparam.dr[j]*EMparam.A[i][j]);
        }
    }
}

M    = gsl_matrix_calloc(EMparam.nR, EMparam.nR);
L    = gsl_matrix_calloc(EMparam.nR, EMparam.nR);
LQR  = gsl_matrix_calloc(EMparam.nR, EMparam.nR);
Ltau = gsl_vector_calloc(EMparam.nR);
Xs   = gsl_matrix_calloc(GSL_MAX(EMparam.nh,EMparam.nR), EMparam.nR);
ys   = gsl_vector_calloc(GSL_MAX(EMparam.nh,EMparam.nR));

c = gsl_vector_calloc(EMparam.nR);
c_reg = gsl_vector_calloc(EMparam.nR);
c_lcurve = gsl_vector_calloc(EMparam.nR);
c_gcv = gsl_vector_calloc(EMparam.nR);
MengerR = gsl_vector_calloc(EMparam.nLagrange);
slope = gsl_vector_calloc(EMparam.nLagrange);
rho = gsl_vector_calloc(EMparam.nLagrange);
eta = gsl_vector_calloc(EMparam.nLagrange);
Brho = gsl_vector_calloc(EMparam.nLagrange);
Beta = gsl_vector_calloc(EMparam.nLagrange);
reg_param = gsl_vector_calloc(EMparam.nLagrange);
Breg_param = gsl_vector_calloc(EMparam.nLagrange);
nnls_rho = gsl_vector_calloc(EMparam.nLagrange);
nnls_eta = gsl_vector_calloc(EMparam.nLagrange);
Gvec = gsl_vector_calloc(EMparam.nLagrange);


switch (EMparam.defL) {
 case Idendity:
    for (i=0;i<EMparam.nR;i++) {
        gsl_matrix_set(L,i,i,1.0);
    }
    break;
 case first_deriv_eps_e:
    for (i=0;i<EMparam.nR-1;i++) {
        gsl_matrix_set(L,i,i  ,-1.0);
        gsl_matrix_set(L,i,i+1,1.0);
    }
    gsl_matrix_set(L,EMparam.nR-1,EMparam.nR-1,1.0e-6);
    break;
 case first_deriv_eps_b:
    gsl_matrix_set(L,0,0,1.0e-6);
    for (i=1;i<EMparam.nR;i++) {
        gsl_matrix_set(L,i,i-1,-1.0);
        gsl_matrix_set(L,i,i  ,1.0);
    }
    sasfit_out("set penalty function to >first_deriv_eps_b<\n");
    break;
 case first_deriv:
 //   gsl_matrix_set(L,0,EMparam.nR-1,-1.0);
 //   gsl_matrix_set(L,0,1           , 1.0);
    for (i=0;i<EMparam.nR-1;i++) {
        gsl_matrix_set(L,i,i,-1.0);
        gsl_matrix_set(L,i,i+1, 1.0);
    }
    gsl_matrix_set(L,EMparam.nR-1,EMparam.nR-1,1.0);
//    gsl_matrix_set(L,EMparam.nR-1,0, 1.0);
    sasfit_out("set penalty function to >first_deriv<\n");
    break;
 case second_deriv:
    gsl_matrix_set(L,0,0, 2.0);
    gsl_matrix_set(L,0,1, -1.0);
    gsl_matrix_set(L,0,EMparam.nR-1, -1.0);
    for (i=1;i<EMparam.nR-1;i++) {
        gsl_matrix_set(L,i,i-1,  -1.0);
        gsl_matrix_set(L,i,i,  2.0);
        gsl_matrix_set(L,i,i+1,-1.0);
    }

    gsl_matrix_set(L,EMparam.nR-1,0, -1.0);
    gsl_matrix_set(L,EMparam.nR-1,EMparam.nR-2, -1.0);
    gsl_matrix_set(L,EMparam.nR-1,EMparam.nR-1, 2.0);
    sasfit_out("set penalty function to >second_deriv<\n");
    break;
 case second_deriv_ND:
    gsl_matrix_set(L,0,0,2.0);  //  case Dirichlet-Neumann
    gsl_matrix_set(L,0,1,-1.0); // case Dirichlet-Neumann
    for (i=1;i<EMparam.nR-1;i++) {
        gsl_matrix_set(L,i,i-1,-1.0);
        gsl_matrix_set(L,i,i  ,2.0);
        gsl_matrix_set(L,i,i+1,-1.0);
    }
    gsl_matrix_set(L,EMparam.nR-1,EMparam.nR-2,-1.0); //  case Neumann-Dirichlet
    gsl_matrix_set(L,EMparam.nR-1,EMparam.nR-1,1.0);  //  case Neumann-Dirichlet
    sasfit_out("set penalty function to >Neumann-Dirichlet<:second_deriv_ND\n");
    break;
 case second_deriv_DN:
    gsl_matrix_set(L,0,0, 1.0); // case Neumann-Dirichlet
    gsl_matrix_set(L,0,1,-1.0); // case Neumann-Dirichlet
    for (i=1;i<EMparam.nR-1;i++) {
        gsl_matrix_set(L,i,i-1,-1.0);
        gsl_matrix_set(L,i,i  ,2.0);
        gsl_matrix_set(L,i,i+1,-1.0);
    }
    gsl_matrix_set(L,EMparam.nR-1,EMparam.nR-2,-1.0); // case Neumann-Dirichlet
    gsl_matrix_set(L,EMparam.nR-1,EMparam.nR-1,2.0);  // case Neumann-Dirichlet
    sasfit_out("set penalty function to >Dirichlet-Neumann<:second_deriv_DN\n");
    break;
 case second_deriv_DD:
    gsl_matrix_set(L,0,0,2.0);  // case Dirichlet-Dirichlet
    gsl_matrix_set(L,0,1,-1.0); // case Dirichlet-Dirichlet
    for (i=1;i<EMparam.nR-1;i++) {
        gsl_matrix_set(L,i,i-1,-1.0);
        gsl_matrix_set(L,i,i  ,2.0);
        gsl_matrix_set(L,i,i+1,-1.0);
    }
    gsl_matrix_set(L,EMparam.nR-1,EMparam.nR-2,-1.0); // case Dirichlet-Dirichlet
    gsl_matrix_set(L,EMparam.nR-1,EMparam.nR-1,2.0);  // case Dirichlet-Dirichlet
    sasfit_out("set penalty function to >Dirichlet-Dirichlet<:second_deriv_DD\n");
    break;
 case second_deriv_NN:
    gsl_matrix_set(L,0,0,1.0);  // case Dirichlet-Dirichlet
    gsl_matrix_set(L,0,1,-1.0); // case Dirichlet-Dirichlet
    for (i=1;i<EMparam.nR-1;i++) {
        gsl_matrix_set(L,i,i-1,-1.0);
        gsl_matrix_set(L,i,i  ,2.0);
        gsl_matrix_set(L,i,i+1,-1.0);
    }
    gsl_matrix_set(L,EMparam.nR-1,EMparam.nR-2,-1.0); // case Neumann-Neumann
    gsl_matrix_set(L,EMparam.nR-1,EMparam.nR-1,1.0);  // case Neumann-Neumann
    sasfit_out("set penalty function to >Neumann-Neumann<:second_deriv_NN\n");
    break;
 default:
    gsl_matrix_set(L,0,0,2.0);  // case Dirichlet-Dirichlet
    gsl_matrix_set(L,0,1,-1.0); // case Dirichlet-Dirichlet
    for (i=1;i<EMparam.nR-1;i++) {
        gsl_matrix_set(L,i,i-1,-1.0);
        gsl_matrix_set(L,i,i  ,2.0);
        gsl_matrix_set(L,i,i+1,-1.0);
    }
    gsl_matrix_set(L,EMparam.nR-1,EMparam.nR-2,-1.0); // case Dirichlet-Dirichlet
    gsl_matrix_set(L,EMparam.nR-1,EMparam.nR-1,2.0);  // case Dirichlet-Dirichlet
    sasfit_out("set penalty function to >Dirichlet-Dirichlet<:default\n");
}
//gsl_multifit_linear_svd(X, w);
gsl_matrix_memcpy(LQR, L);
gsl_multifit_linear_L_decomp(LQR, Ltau);
gsl_multifit_linear_wstdform2(LQR, Ltau, X, weights, y, Xs, ys, M, w);
gsl_multifit_linear_svd(Xs, w);

rcond = gsl_multifit_linear_rcond(w);
sasfit_out("rcond:%lg\n",rcond);
/* calculate L-curve and find its corner */
switch (EMparam.optLagrange_method) {
 case GCV:
    gsl_multifit_linear_gcv(ys, reg_param, Gvec, &lambda_gcv, &G_gcv, w);
	 	    /* regularize with lambda_gcv */
    gsl_multifit_linear_solve(lambda_gcv, Xs, ys, c_gcv, &rnorm, &snorm, w);
    if (FPd.PrintProgress > 0) {
        sasfit_out("=== Regularized fit ===\n");
        sasfit_out("lambda\tG\n");
        for (j=0; j < EMparam.nLagrange; ++j) {
            sasfit_out("i:\t%d\tlambda:\t%lg\tG(lambda):\t%lg\n", j,gsl_vector_get(reg_param,j),gsl_vector_get(Gvec,j));
        }
    }
    gsl_multifit_linear_wgenform2(LQR, Ltau, X, weights, y, c_gcv, M, c, w);
    sasfit_out("lambda_opt\tG\trho\teta\n");
    sasfit_out("%lg\t%lg\t%lg\t%lg\n", lambda_gcv,G_gcv,gsl_pow_2(rnorm),pow(lambda_gcv * snorm, 2.0));
    break;
 case Lcorner2:
    sasfit_gsl_multifit_linear_corner_golden_rule(ys, reg_param, rho, eta, EMparam.minLagrange, EMparam.maxLagrange, &lambda_opt, w);
    sasfit_out(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    sasfit_out("\t lambda_opt=%lf\n",lambda_opt);
    sasfit_out("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
    sasfit_gsl_multifit_linear_lcurve(ys, reg_param, rho, eta, EMparam.minLagrange, EMparam.maxLagrange, w);
    sasfit_gsl_multifit_linear_lcorner2(reg_param,rho, eta,MengerR,slope,EMparam.maxslope, &reg_idx);
    if (FPd.PrintProgress > 0) {
        sasfit_out("=== Regularized fit ===\n");
        sasfit_out("lambda   \t rho   \t  eta\n");
        for (j=0; j < EMparam.nLagrange; ++j) {
            sasfit_out("i\t%d\tlambda\t%lg\trho\t%lg\teta\t%lg\tMengerR\t%lg\tslope\t%lg\n",
                       j, gsl_vector_get(reg_param,j),
                       gsl_pow_2(gsl_vector_get(rho,j))/EMparam.nh,
                       gsl_vector_get(eta,j),
                       gsl_vector_get(MengerR,j),
                       gsl_vector_get(slope,j) );
        }
    }
/* store optimal regularization parameter */
    lambda_l = gsl_vector_get(reg_param, reg_idx);
    sasfit_out("lambda[%d]:%lg\n",reg_idx,lambda_l);

    gsl_multifit_linear_solve(lambda_l, Xs, ys, c_reg, &rnorm, &snorm, w);
    gsl_multifit_linear_wgenform2(LQR, Ltau, X, weights, y, c_reg, M, c, w);
    sasfit_out("lambda_opt   \t rho   \t  eta\n");
    sasfit_out("%lg\t%lg\t%lg\n", lambda_l,gsl_pow_2(rnorm),pow(lambda_l * snorm, 2.0));
    break;
case Lcorner_o:
case Lcorner_l:
case Lcorner_w:
case Lcorner_wo:
    sasfit_gsl_multifit_linear_corner_golden_rule(ys, reg_param, rho, eta, EMparam.minLagrange, EMparam.maxLagrange, &lambda_opt, w);
    sasfit_out(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    sasfit_out("\t lambda_opt=%lf\n",lambda_opt);
    sasfit_out("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
    sasfit_gsl_multifit_linear_lcurve(ys, reg_param, rho, eta, EMparam.minLagrange, EMparam.maxLagrange, w);
    sasfit_bspline_lcorner(rho, eta, reg_param, Brho, Beta, Breg_param);
    sasfit_gsl_multifit_linear_lcorner(Brho, Beta,MengerR, slope,EMparam.optLagrange_method,EMparam.maxslope, &reg_idx);
    if (FPd.PrintProgress > 0) {
        sasfit_out("=== Regularized fit ===\n");
        sasfit_out("lambda   \t rho   \t  eta\n");
        for (j=0; j < EMparam.nLagrange; ++j) {
            sasfit_out("i\t%d\tlambda\t%lg\trho\t%lg\teta\t%lg\tMengerR\t%lg\tslope\t%lg\n",
                       j, gsl_vector_get(reg_param,j),
                       gsl_pow_2(gsl_vector_get(rho,j))/EMparam.nh,
                       gsl_vector_get(eta,j),
                       gsl_vector_get(MengerR,j),
                       gsl_vector_get(slope,j));
       }
    }
/* store optimal regularization parameter */
    lambda_l = gsl_vector_get(Breg_param, reg_idx);
    sasfit_out("lambda[%d]:%lg\n",reg_idx,lambda_l);

    gsl_multifit_linear_solve(lambda_l, Xs, ys, c_reg, &rnorm, &snorm, w);
    gsl_multifit_linear_wgenform2(LQR, Ltau, X, weights, y, c_reg, M, c, w);
    sasfit_out("lambda_opt   \t rho   \t  eta\n");
    sasfit_out("%lg\t%lg\t%lg\n", lambda_l,gsl_pow_2(rnorm),pow(lambda_l * snorm, 2.0));
    break;
case manual:
    lambda_l = fabs(EMparam.lambda);
    gsl_multifit_linear_solve(lambda_l, Xs, ys, c_reg, &rnorm, &snorm, w);
    gsl_multifit_linear_wgenform2(LQR, Ltau, X, weights, y, c_reg, M, c, w);
    sasfit_out("lambda_opt   \t rho   \t  eta\n");
    sasfit_out("%lg\t%lg\t%lg\n", lambda_l,gsl_pow_2(rnorm),pow(lambda_l * snorm, 2.0));
    break;
case redchi2:
//   gsl_multifit_linear_lcurve(ys, reg_param, rho, eta, w);
    sasfit_gsl_multifit_linear_lcurve(ys, reg_param, rho, eta, EMparam.minLagrange, EMparam.maxLagrange, w);
    j=1;
    l2=-1;
    l1=0;
    l0=1;
    do {
        j++;
        sasfit_out("lambda:%lg, redchi2:%lg, chi2:%lg\n",gsl_vector_get(reg_param, j),gsl_pow_2(gsl_vector_get(rho,j))/EMparam.nh,EMparam.chi2);
        l2=l1;
        l1=l0;
        l0=j;
    } while (gsl_pow_2(gsl_vector_get(rho,j))/EMparam.nh > EMparam.chi2);

    lambda_l = inverse_quad_interp(EMparam.chi2, gsl_vector_get(reg_param, l0),gsl_pow_2(gsl_vector_get(rho,l0))/EMparam.nh,
                                                 gsl_vector_get(reg_param, l1),gsl_pow_2(gsl_vector_get(rho,l1))/EMparam.nh,
                                                 gsl_vector_get(reg_param, l2),gsl_pow_2(gsl_vector_get(rho,l2))/EMparam.nh) ;
    sasfit_out("lambda_opt_%d :%lg\n",j,lambda_l);

    gsl_multifit_linear_solve(lambda_l, Xs, ys, c_reg, &rnorm, &snorm, w);
    gsl_multifit_linear_wgenform2(LQR, Ltau, X, weights, y, c_reg, M, c, w);
    sasfit_out("lambda_opt   \t rho   \t  eta\n");
    sasfit_out("%lg\t%lg\t%lg\n", lambda_l,gsl_pow_2(rnorm),pow(lambda_l * snorm, 2.0));
    break;
default:
//   gsl_multifit_linear_lcurve(ys, reg_param, rho, eta, w);
    sasfit_gsl_multifit_linear_lcurve(ys, reg_param, rho, eta, EMparam.minLagrange, EMparam.maxLagrange, w);
    sasfit_gsl_multifit_linear_lcorner(rho, eta,MengerR, slope,EMparam.optLagrange_method,EMparam.maxslope, &reg_idx);
    if (FPd.PrintProgress > 0) {
        sasfit_out("=== Regularized fit ===\n");
        sasfit_out("lambda   \t rho   \t  eta\n");
        for (j=0; j < EMparam.nLagrange; ++j) {
            sasfit_out("i\t%d\tlambda\t%lg\trho\t%lg\teta\t%lg\tMengerR\t%lg\tslope\t%lg\n",
                       j,
                       gsl_pow_2(gsl_vector_get(rho,j))/EMparam.nh,
                       gsl_vector_get(rho,j),
                       gsl_vector_get(eta,j),
                       gsl_vector_get(MengerR,j),
                       gsl_vector_get(slope,j));
       }
    }
/* store optimal regularization parameter */
    lambda_l = gsl_vector_get(reg_param, reg_idx);
    sasfit_out("lambda[%d]:%lg\n",reg_idx,lambda_l);

    gsl_multifit_linear_solve(lambda_l, Xs, ys, c_reg, &rnorm, &snorm, w);
    gsl_multifit_linear_wgenform2(LQR, Ltau, X, weights, y, c_reg, M, c, w);
    sasfit_out("lambda_opt   \t rho   \t  eta\n");
    sasfit_out("%lg\t%lg\t%lg\n", lambda_l,gsl_pow_2(rnorm),pow(lambda_l * snorm, 2.0));
}

/* regularize with nnls */
if (EMparam.LLSmethod == NNLLS) {
    mda = (EMparam.nR+EMparam.nh);
    m = mda;
    n = EMparam.nR;
    Amn = (double *) malloc(m*n*sizeof(double));
    U = (double *) malloc ( m * m * sizeof ( double ) );
    S = (double *) calloc ( m * n , sizeof ( double ) );
    V = (double *) malloc ( n * n * sizeof ( double ) );
    AAA = (double **) malloc((EMparam.nR)*sizeof(double*));
    for (i = 0; i < (EMparam.nR); i++) {
       AAA[i] = (double *) malloc((EMparam.nR+EMparam.nh)*sizeof(double));
    }
    Af2c = (double*) malloc(mda*n*sizeof(double));
    bb = (double*) calloc(m,sizeof(double));
    xx = (double*) calloc(n,sizeof(double));
    zzp = (double*) calloc(m,sizeof(double));
    wp = (double*) calloc(n,sizeof(double));
    indexp = (int*) calloc(n,sizeof(int));

    if (EMparam.optLagrange_method==Lcorner_o  ||
        EMparam.optLagrange_method==Lcorner_l  ||
        EMparam.optLagrange_method==Lcorner_w  ||
        EMparam.optLagrange_method==Lcorner_wo ) {
    for (l=0;l<EMparam.nLagrange;l++) {
        for (i = 0; i < (EMparam.nh); i++) {
            for (j = 0; j < EMparam.nR; j++) {
                Af2c[i+mda*j] = gsl_matrix_get(X,i,j)*sqrt(gsl_vector_get(weights,i));
                Amn[i+m*j] = gsl_matrix_get(X,i,j)*sqrt(gsl_vector_get(weights,i));
                AAA[j][i] = gsl_matrix_get(X,i,j)*sqrt(gsl_vector_get(weights,i));
            }
            bb[i]=gsl_vector_get(y,i)*sqrt(gsl_vector_get(weights,i));
        }
        for (i = 0; i < (EMparam.nR); i++) {
            for (j = 0; j < EMparam.nR; j++) {
                Af2c[(i+EMparam.nh)+mda*j] = gsl_vector_get(reg_param,l)*gsl_matrix_get(L,i,j);
                AAA[j][(i+EMparam.nh)]     = gsl_vector_get(reg_param,l)*gsl_matrix_get(L,i,j);
            }
        }
        if (l >= 0) {
            gsl_multifit_linear_solve(gsl_vector_get(reg_param,l), Xs, ys, c_reg, &rnorm, &snorm, w);
            gsl_multifit_linear_wgenform2(LQR, Ltau, X, weights, y, c_reg, M, c, w);
            for (i = 0; i < EMparam.nR; i++) {
                xx[i] = fabs(gsl_vector_get(c,i));
            }
        }
        ierr=nnls(AAA,m,n,bb,xx,&rnorm,wp,zzp,indexp);
        for (i=0;i<EMparam.nR;i++) {
            EMparam.out[i]=xx[i];
        }
        rnorm = 0;
        for (i=0;i<EMparam.nh;i++) {
            EMparam.Ith[i]=0;
            for (j=0; j < EMparam.nR; ++j) EMparam.Ith[i]=EMparam.Ith[i]+EMparam.dr[j]*EMparam.A[i][j]*EMparam.out[j];
            rnorm=rnorm+gsl_pow_2((EMparam.Ith[i]-EMparam.Ih[i])/EMparam.DIh[i]);
        }
        gsl_vector_set(rho,l,rnorm);
        rnorm = 0;
        for (i = 0; i < (EMparam.nR); i++) {
            snorm =0;
            for (j = 0; j < EMparam.nR; j++) {
                snorm = snorm + gsl_matrix_get(L,i,j)*xx[j];
            }
            rnorm = rnorm + snorm*snorm; // gsl_pow_2(gsl_vector_get(reg_param,l)*snorm);
        }
        gsl_vector_set(eta,l,rnorm);
        if (FPd.PrintProgress > 0) {
                sasfit_out("finished %d  nnls(lambda=%lg) with error: %d and rho: %lg and eta: %lg\n",l,gsl_vector_get(reg_param,l),ierr,gsl_vector_get(rho,l),gsl_vector_get(eta,l));
        }
    }
    sasfit_bspline_lcorner(rho,eta,reg_param,Brho,Beta,Breg_param);
    sasfit_gsl_multifit_linear_lcorner(rho, eta, MengerR, slope,EMparam.optLagrange_method,EMparam.maxslope, &reg_idx);
    if (FPd.PrintProgress > 0) {
    sasfit_out("=== Regularized NNLLS fit ===\n");
    for (j=0; j < EMparam.nLagrange; ++j) {
        sasfit_out("i\t%d\tlambda\t%lg\trho\t%lg\teta\t%lg\tMengerR\t%lg\tslope\t%lg\n",
                j, gsl_vector_get(reg_param,j),
                gsl_pow_2(gsl_vector_get(rho,j))/EMparam.nh,
                gsl_vector_get(eta,j),
                gsl_vector_get(MengerR,j),
                gsl_vector_get(slope,j));
    }
    }
/* store optimal regularization parameter */
    lambda_l = gsl_vector_get(reg_param, reg_idx);
    sasfit_out("lambda_opt %d:%lg\n",reg_idx,lambda_l);
    }
    for (i = 0; i < (EMparam.nh); i++) {
        for (j = 0; j < EMparam.nR; j++) {
            Af2c[i+mda*j] = gsl_matrix_get(X,i,j)*sqrt(gsl_vector_get(weights,i));
            Amn[i+m*j] = gsl_matrix_get(X,i,j)*sqrt(gsl_vector_get(weights,i));
            AAA[j][i] = gsl_matrix_get(X,i,j)*sqrt(gsl_vector_get(weights,i));
        }
        bb[i]=gsl_vector_get(y,i)*sqrt(gsl_vector_get(weights,i));
    }
    for (i = 0; i < (EMparam.nR); i++) {
        for (j = 0; j < EMparam.nR; j++) {
            Af2c[(i+EMparam.nh)+mda*j] = (EMparam.optLagrange_method==GCV?lambda_gcv:lambda_l)*gsl_matrix_get(L,i,j);
            AAA[j][(i+EMparam.nh)]     = (EMparam.optLagrange_method==GCV?lambda_gcv:lambda_l)*gsl_matrix_get(L,i,j);
        }
    }
    for (i = 0; i < EMparam.nR; i++) {
        xx[i] = fabs(gsl_vector_get(c,i));
    }

/*
    infoDSVDC = sasfit_svd_linpack(m,n,Amn,U,S,V);
    for (i=0;i<(m>n?n:m);i++)
            sasfit_out("SVD:%d info:%d gsl:%lg linpack:%lg\n",i,infoDSVDC,gsl_vector_get(w->S,i),(S[i+m*i]));
*/
    sasfit_out("initialized corresponding matrices and vector\ntry to perform now nnls\n");
//    nnls_f2c(Af2c, &mda,&m,&n,bb,xx,&rnorm,wp,zzp,indexp,&ierr);
    ierr=nnls(AAA,m,n,bb,xx,&rnorm,wp,zzp,indexp);
    for (i=0;i<EMparam.nR;i++) {
        EMparam.out[i]=xx[i];
    }
    sasfit_out("finished nnls with error: %d and rnorm:%lf\n",ierr,rnorm);
    sasfit_out("freeing now all the memory\nstart to free AA\n");
    for (i=0; i<(EMparam.nR+0*EMparam.nh);i++) {
//       sasfit_out("start to free AA :%d \n",i);
       free(AAA[i]);
//       sasfit_out("free-ed AAA: %d\n",i);
    }
    free(AAA);

    free(Af2c);
    sasfit_out("free-ed AA\n");
    free(bb);
    sasfit_out("free-ed bb\n");
    free(wp);
    sasfit_out("free-ed wp\n");
    free(zzp);
    sasfit_out("free-ed zzp\n");
    free(indexp);
    sasfit_out("free-ed indexp\n");
    free(xx);
    sasfit_out("free-ed xx\n");
    free(Amn);
    free(U);
    free(S);
    free(V);
} else {
    for (i=0;i<EMparam.nR;i++) {
        EMparam.out[i]=gsl_vector_get(c,i);
    }
}
chisq = 0;
for (i=0;i<EMparam.nh;i++) {
    EMparam.Ith[i]=0;
    for (j=0; j < EMparam.nR; ++j) EMparam.Ith[i]=EMparam.Ith[i]+EMparam.dr[j]*EMparam.A[i][j]*EMparam.out[j];
    chisq=chisq+gsl_pow_2((EMparam.Ith[i]-EMparam.Ih[i])/EMparam.DIh[i]);
}
chisq=chisq/EMparam.nh;
sasfit_out("chisq:%lg\n", chisq);

Tcl_ResetResult(interp);
Tcl_DStringInit(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nR;i++) {
    sprintf(Buffer,"%lg",EMparam.r[i]);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nR;i++) {
	sprintf(Buffer,"%lg",EMparam.out[i]);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nh;i++) {
	sprintf(Buffer,"%lg",EMparam.h[i]);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nh;i++) {
	sprintf(Buffer,"%lg",EMparam.Ith[i]+EP.c0);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringResult(interp,&DsBuffer);
Tcl_DStringFree(&DsBuffer);

EM_DR_Free(&FPd);

  gsl_multifit_linear_free(w);
  gsl_vector_free(c);
  gsl_vector_free(c_reg);
  gsl_vector_free(c_lcurve);
  gsl_vector_free(c_gcv);
  gsl_vector_free(reg_param);
  gsl_vector_free(Breg_param);
  gsl_vector_free(MengerR);
  gsl_vector_free(slope);
  gsl_vector_free(rho);
  gsl_vector_free(eta);
  gsl_vector_free(Brho);
  gsl_vector_free(Beta);
  gsl_vector_free(nnls_rho);
  gsl_vector_free(nnls_eta);
  gsl_matrix_free(L);
  gsl_matrix_free(X);
  gsl_vector_free(y);
  gsl_matrix_free(Xs);
  gsl_matrix_free(M);
  gsl_matrix_free(LQR);
  gsl_vector_free(Ltau);
  gsl_vector_free(Gvec);
  gsl_vector_free(ys);
  gsl_vector_free(weights);

return TCL_OK;
}

double
MEM_f (const gsl_vector *v, void *FPd)
{
    int i,status;
    double xi,sumx,sump,S;
    gsl_vector *r;
    EM_param_t *EMparam;
    sasfit_fp_data *FixedPointData;

    FixedPointData = (sasfit_fp_data *)FPd;
    EMparam = (EM_param_t *) FixedPointData->FPstructure;

    r = gsl_vector_alloc(EMparam->nh);
    status = gsl_vector_memcpy(EMparam->gsl_b,r);
    status = gsl_blas_dgemv(CblasNoTrans, 1.0, EMparam->gsl_A, v, -1.0, r);

    gsl_blas_ddot(EMparam->gsl_dx,v,&sump);

    for (i=0;i<EMparam->nR;i++) {
        xi = gsl_vector_get(v,i);
        if (fabs(xi)>0 && sump>0) {
            gsl_vector_set(EMparam->gsl_S,i,xi*log(xi/sump));
        } else {
            gsl_vector_set(EMparam->gsl_S,i,0);
        }
    }
    gsl_blas_ddot(EMparam->gsl_dx,EMparam->gsl_S,&S);
    return gsl_blas_dnrm2(r)-gsl_pow_2(EMparam->lambda)*S;
}

/* The gradient of f, df = (df/dx, df/dy). */
void
MEM_df (const gsl_vector *v, void *FPd,
       gsl_vector *df)
{
    int i,status;
    double xi,sumx,sump,S;
    gsl_vector *r;
    EM_param_t *EMparam;
    sasfit_fp_data *FixedPointData;

    FixedPointData = (sasfit_fp_data *)FPd;
    EMparam = (EM_param_t *) FixedPointData->FPstructure;

    r = gsl_vector_alloc(EMparam->nh);
    status = gsl_vector_memcpy(EMparam->gsl_b,r);
    status = gsl_blas_dgemv(CblasNoTrans, 1.0, EMparam->gsl_A, v, -1.0, r);

    gsl_blas_ddot(EMparam->gsl_dx,v,&sump);

    for (i=0;i<EMparam->nR;i++) {
        xi = gsl_vector_get(v,i);
        if (fabs(xi)>0 && sump>0) {
            gsl_vector_set(EMparam->gsl_dS,i,(1+log(xi/sump))*EMparam->dr[i]);
        } else {
            gsl_vector_set(EMparam->gsl_dS,i,GSL_NEGINF);
        }
    }

    status = gsl_vector_memcpy(EMparam->gsl_dS,df);
    status = gsl_blas_dgemv(CblasTrans, 2.0, EMparam->gsl_A, r, -gsl_pow_2(EMparam->lambda), df);
}

/* Compute both f and df together. */
void
MEM_fdf (const gsl_vector *x, void *FPd,
        double *f, gsl_vector *df)
{
    int i,status;
    double xi,sumx,sump,S;
    gsl_vector *r;
    EM_param_t *EMparam;
    sasfit_fp_data *FixedPointData;

    FixedPointData = (sasfit_fp_data *)FPd;
    EMparam = (EM_param_t *) FixedPointData->FPstructure;

    r = gsl_vector_alloc(EMparam->nh);
    status = gsl_vector_memcpy(EMparam->gsl_b,r);
    status = gsl_blas_dgemv(CblasNoTrans, 1.0, EMparam->gsl_A, x, -1.0, r);

    gsl_blas_ddot(EMparam->gsl_dx,x,&sump);

    for (i=0;i<EMparam->nR;i++) {
        xi = gsl_vector_get(x,i);
        if (fabs(xi)>0 && sump>0) {
            gsl_vector_set(EMparam->gsl_dS,i,(1+log(xi/sump))*EMparam->dr[i]);
            gsl_vector_set(EMparam->gsl_S,i,xi*log(xi/sump));
        } else {
            gsl_vector_set(EMparam->gsl_dS,i,GSL_NEGINF);
            gsl_vector_set(EMparam->gsl_S,i,0);
        }
    }
    gsl_blas_ddot(EMparam->gsl_dx,EMparam->gsl_S,&S);
    *f = gsl_blas_dnrm2(r)-gsl_pow_2(EMparam->lambda)*S;
    status = gsl_vector_memcpy(EMparam->gsl_dS,df);
    status = gsl_blas_dgemv(CblasTrans, 2.0, EMparam->gsl_A, r, -gsl_pow_2(EMparam->lambda), df);
}

int Sasfit_DR_MEM_Cmd(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int        argc;
    char       **argv;
{
struct extrapolPar EP;
EM_param_t EMparam;
sasfit_fp_data FPd;

FPd.tm=NULL;

scalar rmax;
char   errstr[256],Buffer[256];
bool   error;
Tcl_DString DsBuffer;
double chisq, rnorm, snorm;
int i,j;
int status;
gsl_vector *x;
size_t iter = 0;
gsl_multimin_function_fdf MEM_func;
const gsl_multimin_fdfminimizer_type *T;
gsl_multimin_fdfminimizer *solver;
gsl_multimin_fminimizer *simplex_solver;


error = FALSE;

if (argc != 3) {
   sasfit_err("wrong # args; should be sasfit_DR_MEM ?EMOptions? ?xye_data?\n");
   return TCL_ERROR;
}

if (TCL_ERROR == get_EP(clientData,interp,argv,&EP,&EMparam.h,&EMparam.Iexp,&EMparam.DIh)) {
   return TCL_ERROR;
}

if (TCL_ERROR == get_EM(clientData,interp,&EMparam)) {
   return TCL_ERROR;
}

EMparam.C0 = EP.c0;
EMparam.C4 = EP.c4;
EMparam.nh = EP.ndata;
FPd.FPstructure=&EMparam;
FPd.FP_Op=NULL;
FPd.mixcoeff=0.5;
FPd.mixstrategy=mix_const;

FPd.interp=interp;
EM_DR_Init(&FPd);
//FPd.PrintProgress=1;

x = gsl_vector_alloc (EMparam.nR);
for (j=0; j < EMparam.nR; ++j) gsl_vector_set (x, j, EMparam.in[j]);

MEM_func.n = EMparam.nR;  /* number of function components */
MEM_func.f = &MEM_f;
MEM_func.df = &MEM_df;
MEM_func.fdf = &MEM_fdf;
MEM_func.params = (void *) (&FPd);


T = gsl_multimin_fdfminimizer_conjugate_fr;
// T = gsl_multimin_fdfminimizer_conjugate_pr;
// T = gsl_multimin_fdfminimizer_vector_bfgs2;
// T = gsl_multimin_fdfminimizer_vector_bfgs;
// T = gsl_multimin_fdfminimizer_steepest_descent;
// T = gsl_multimin_fminimizer_nmsimplex2;
// T = gsl_multimin_fminimizer_nmsimplex;
// T = gsl_multimin_fminimizer_nmsimplex2rand

solver = gsl_multimin_fdfminimizer_alloc (T, MEM_func.n);

gsl_multimin_fdfminimizer_set (solver, &MEM_func, x, 0.01, 1e-4);
// gsl_multimin_fminimizer_set (simplex_solver, &MEM_func, x, ss);
do
    {
      iter++;
      status = gsl_multimin_fdfminimizer_iterate (solver);

      if (status)
        break;

      status = gsl_multimin_test_gradient (solver->gradient, 1e-3);

      if (status == GSL_SUCCESS)
        sasfit_out ("Minimum found after %d iterations\n", iter);
    }
while (status == GSL_CONTINUE && iter < 100);
for (j=0; j < EMparam.nR; ++j) EMparam.out[j]=gsl_vector_get(solver->x, j);
gsl_multimin_fdfminimizer_free (solver);
gsl_vector_free (x);

chisq = 0;
for (i=0;i<EMparam.nh;i++) {
    EMparam.Ith[i]=0;
    for (j=0; j < EMparam.nR; ++j) EMparam.Ith[i]=EMparam.Ith[i]+EMparam.dr[j]*EMparam.A[i][j]*EMparam.out[j];
    chisq=chisq+gsl_pow_2((EMparam.Ith[i]-EMparam.Ih[i])/EMparam.DIh[i]);
}
chisq=chisq/EMparam.nh;
sasfit_out("chisq:%lg\n", chisq);

Tcl_ResetResult(interp);
Tcl_DStringInit(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nR;i++) {
    sprintf(Buffer,"%lg",EMparam.r[i]);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nR;i++) {
	sprintf(Buffer,"%lg",EMparam.out[i]);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nh;i++) {
	sprintf(Buffer,"%lg",EMparam.h[i]);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nh;i++) {
	sprintf(Buffer,"%lg",EMparam.Ith[i]+EP.c0);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringResult(interp,&DsBuffer);
Tcl_DStringFree(&DsBuffer);

EM_DR_Free(&FPd);
return TCL_OK;
}



int Sasfit_DR_MuCh_Cmd(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int        argc;
    char       **argv;
{
struct extrapolPar EP;
EM_param_t EMparam;
sasfit_fp_data FPd;

FPd.tm=NULL;
scalar p1,p2,p3, QR, eps,chi2;
int    i,j,k,l,itst;
scalar rmax;
char   errstr[256],Buffer[256];
bool   error;
Tcl_DString DsBuffer;


error = FALSE;

if (argc != 3) {
   sasfit_err("wrong # args; should be sasfit_DR_MuCh ?EMOptions? ?xye_data?\n");
   return TCL_ERROR;
}

if (TCL_ERROR == get_EP(clientData,interp,argv,&EP,&EMparam.h,&EMparam.Iexp,&EMparam.DIh)) {
   return TCL_ERROR;
}

if (TCL_ERROR == get_EM(clientData,interp,&EMparam)) {
   return TCL_ERROR;
}

EMparam.C0 = EP.c0;
EMparam.C4 = EP.c4;
EMparam.nh = EP.ndata;
FPd.FPstructure=&EMparam;
FPd.FP_Op=&MuCh_DR_Operator;
FPd.mixcoeff=0.5;
FPd.mixstrategy=mix_const;
//FPd.PrintProgress=1;

FPd.interp=interp;
FPd.it=0;
MuCh_DR_Init(&FPd);
sasfit_out("D(R): %s algorithm: %d\n",EMparam.iteration_scheme, FPd.root_algorithm);
sasfit_out("maxteps: %d\n",FPd.maxsteps);
sasfit_out("R_max: %lg\n",EMparam.Rmax);
sasfit_out("number of R bins: %d\n",EMparam.nR);
sasfit_out("finished initialization\n");
rmax = M_PI_2/EMparam.h[0];
rmax = EMparam.Rmax;

FP_solver(&FPd);
sasfit_out("it. %d eps %lg chi2 %lg\n",FPd.it, FPd.gNorm, FPd.Chi2Norm);
sasfit_out("it. %d KLD %lg JSD %lg\n",FPd.it, FPd.KLD, FPd.JSD);

Tcl_ResetResult(interp);
Tcl_DStringInit(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nR;i++) {
    sprintf(Buffer,"%lg",EMparam.r[i]);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nR;i++) {
	sprintf(Buffer,"%lg",EMparam.out[i]);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nh;i++) {
	sprintf(Buffer,"%lg",EMparam.h[i]);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nh;i++) {
	sprintf(Buffer,"%lg",EMparam.Ith[i]+EP.c0);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringResult(interp,&DsBuffer);
Tcl_DStringFree(&DsBuffer);

MuCh_DR_Free(&FPd);

return TCL_OK;
}

int Sasfit_DR_SDM_Cmd(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int        argc;
    char       **argv;
{
struct extrapolPar EP;
EM_param_t EMparam;
sasfit_fp_data FPd;

FPd.tm=NULL;
scalar p1,p2,p3, QR, eps,chi2;
int    i,j,k,l,itst;
scalar rmax;
char   errstr[256],Buffer[256];
bool   error;
Tcl_DString DsBuffer;


error = FALSE;

if (argc != 3) {
   sasfit_err("wrong # args; should be sasfit_DR_SDM ?EMOptions? ?xye_data?\n");
   return TCL_ERROR;
}

if (TCL_ERROR == get_EP(clientData,interp,argv,&EP,&EMparam.h,&EMparam.Iexp,&EMparam.DIh)) {
   return TCL_ERROR;
}

if (TCL_ERROR == get_EM(clientData,interp,&EMparam)) {
   return TCL_ERROR;
}

EMparam.C0 = EP.c0;
EMparam.C4 = EP.c4;
EMparam.nh = EP.ndata;
FPd.FPstructure=&EMparam;
FPd.FP_Op=&SDM_DR_Operator;
FPd.mixcoeff=0.5;
FPd.mixstrategy=mix_const;
//FPd.PrintProgress=1;

FPd.interp=interp;
FPd.it=0;
SDM_DR_Init(&FPd);
sasfit_out("D(R): %s algorithm: %d\n",EMparam.iteration_scheme, FPd.root_algorithm);
sasfit_out("maxteps: %d\n",FPd.maxsteps);
sasfit_out("R_max: %lg\n",EMparam.Rmax);
sasfit_out("number of R bins: %d\n",EMparam.nR);
sasfit_out("finished initialization\n");

rmax = EMparam.Rmax;

FP_solver(&FPd);
sasfit_out("it. %d eps %lg chi2 %lg\n",FPd.it, FPd.gNorm, FPd.Chi2Norm);
sasfit_out("it. %d KLD %lg JSD %lg\n",FPd.it, FPd.KLD, FPd.JSD);

Tcl_ResetResult(interp);
Tcl_DStringInit(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nR;i++) {
    sprintf(Buffer,"%lg",EMparam.r[i]);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nR;i++) {
	sprintf(Buffer,"%lg",EMparam.out[i]);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nh;i++) {
	sprintf(Buffer,"%lg",EMparam.h[i]);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EMparam.nh;i++) {
	sprintf(Buffer,"%lg",EMparam.Ith[i]+EP.c0);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringResult(interp,&DsBuffer);
Tcl_DStringFree(&DsBuffer);

SDM_DR_Free(&FPd);

return TCL_OK;
}
