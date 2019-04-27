/*
 * src/sasfit_old/sasfit_extrapol.c
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
#include <time.h>
#include <math.h>

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
    int varint;
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
 * read the string IterationScheme
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

/*#########################################################################*/
/*#                                                                       #*/
/*# Sasfit_prEM_Cmd --                                                    #*/
/*#                                                                       #*/
/*#      This function implements the Tcl "sasfit_prEM_Cmd" command       #*/
/*#                                                                       #*/
/*# Results:                                                              #*/
/*#      A standard Tcl result.                                           #*/
/*#                                                                       #*/
/*# Side effects:                                                         #*/
/*#      None.                                                            #*/
/*#                                                                       #*/
/*#########################################################################*/

int Sasfit_prEM_Cmd(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int        argc;
    char       **argv;
{
struct extrapolPar EP;
EM_param_t EMparam;

scalar  *h, *Ih, *DIh;
scalar  *tx_k, *tx_km1, *tx_kp1, *ty, **A, *r, *dq, *dr, *tsig, *x, *y, *sig, yth, dyda[4], **S,smear;
scalar  **alpha, **covar;
scalar p1,p2,p3, QR, eps,chi2;
int    *lista,mfit,ma;
int    i,j,k,l,itst;
int nr;
scalar rmax;
char   errstr[256],Buffer[256];
bool   error, smooth_type;
Tcl_DString DsBuffer;

const gsl_rng_type * T;
gsl_rng * rgen;

gsl_rng_env_setup();

T = gsl_rng_default;
rgen = gsl_rng_alloc (T);

error = FALSE;

if (argc != 3) {
   sasfit_err("wrong # args; should be sasfit_DebyeFit ?StructPar? ?xye_data?\n");
   return TCL_ERROR;
}

if (TCL_ERROR == get_EP(clientData,interp,argv,&EP,&h,&Ih,&DIh)) {
   return TCL_ERROR;
}

if (TCL_ERROR == get_EM(clientData,interp,&EMparam)) {
   return TCL_ERROR;
}

nr = EMparam.nR;
rmax = M_PI/h[0];
rmax = EMparam.Rmax;
tx_k   = dvector(0,nr-1);
r   = dvector(0,nr-1);
dr   = dvector(0,nr-1);
tx_kp1   = dvector(0,nr-1);
tx_km1   = dvector(0,nr-1);
ty   = dvector(0,EP.ndata-1);
dq   = dvector(0,EP.ndata-1);
tsig = dvector(0,EP.ndata-1);
A = dmatrix(0,EP.ndata-1,0,nr-1);
S = dmatrix(0,nr-1,0,nr-1);
for (i=0;i<EP.ndata;i++){
    for (j=0;j<nr;j++) {
        r[j] = rmax/nr*(j+1);
        QR=h[i]*r[j];
 //       A[i][j] = gsl_sf_bessel_j0(h[i]*r[j]);
        A[i][j] = gsl_pow_2(4*M_PI*gsl_pow_3(r[j])*(sin(QR)-QR*cos(QR))/gsl_pow_3(QR))*pow(r[j],-EMparam.dim);
    }
    if (i==0) {dq[0]=h[0];} else {dq[i] = h[i]-h[i-1];}
}

for (i=0;i<EP.npoints;i++) {
   ty[i] = Ih[i]-EP.c0;
   Ih[i] =0;
}
gsl_rng_set(rgen, time(NULL));

if (strcmp("single",EMparam.smooth_type)==0) {
    smooth_type = FALSE;
} else {
    smooth_type = TRUE;
}
sasfit_out("%d %s\n",smooth_type,EMparam.smooth_type);

if (strcmp("constant",EMparam.seed)==0) {
    for (i=0;i<nr;i++) {
        tx_k[i] = 1e-7;
        tx_kp1[i] = 1e-7;
        dr[i]=rmax/nr;
    }
} else {
    for (i=0;i<nr;i++) {
        tx_k[i] = gsl_rng_uniform (rgen)*1e-7;
        tx_kp1[i] = gsl_rng_uniform (rgen)*1e-7;
        dr[i]=rmax/nr;
    }
}

gsl_rng_free (rgen);

for (i=0;i<nr;i++) {
    for (j=0;j<nr;j++) {
        S[i][j]=0;
    }
}
smear=EMparam.smooth;
S[0][0]=1-smear;
S[0][1]=smear;
for (i=1;i<nr-1;i++) {
    S[i][i]=1-2*smear;
    S[i][i-1]=smear;
    S[i][i+1]=smear;
}
S[nr-1][nr-2]=smear;
S[nr-1][nr-1]=1-smear;

k=0;
do {
    if (smooth_type) {
        tx_km1[0] = tx_kp1[0];
        tx_k[0]=exp(log(tx_kp1[0])*S[0][0]+log(tx_kp1[1])*S[0][1]);
        for (i=1;i<nr-1;i++) {
            tx_km1[i] = tx_kp1[i];
            tx_k[i]=exp(log(tx_kp1[i-1])*S[i][i-1]+log(tx_kp1[i])*S[i][i]+log(tx_kp1[i+1])*S[i][i+1]);
        }
        tx_km1[nr-1] = tx_kp1[nr-1];
        tx_k[nr-1]=exp(log(tx_kp1[nr-2])*S[nr-1][nr-2]+log(tx_kp1[nr-1])*S[nr-1][nr-1]);
    } else {
        for (i=0;i<nr;i++) {
            tx_km1[i] = tx_kp1[i];
        }
    }
    chi2=0;
    for (j=0;j<EP.ndata;j++) {
        Ih[j]=0;
        for (l=0;l<nr;l++) Ih[j]=Ih[j]+dr[l]*tx_k[l]*A[j][l];
        chi2=chi2+gsl_pow_2((Ih[j]-ty[j])/DIh[j]);
//        sasfit_out("%d %d %lf %lf %lf %lf\n",k, j,(Ih[j]-ty[j])/DIh[j], Ih[j], ty[j], DIh[j]);
    }
    chi2=chi2/EP.ndata;

    for (i=0;i<nr;i++) {
        p3=0;
        p1=0;
        for (j=0;j<EP.ndata;j++) {
            p1=p1+A[j][i]*dq[j];
            p2=ty[j]*A[j][i];
            p3=p3+p2*dq[j]/Ih[j];
        }
        tx_k[i]=tx_k[i]*p3/p1;
    }

    tx_kp1[0]=tx_k[0]*S[0][0]+tx_k[0]*S[0][1];
    eps = gsl_pow_2(tx_kp1[0]-tx_km1[0]);
    for (i=1;i<nr-1;i++) {
        tx_kp1[i]=tx_k[i-1]*S[i][i-1]+tx_k[i]*S[i][i]+tx_k[i+1]*S[i][i+1];
        eps = eps+gsl_pow_2(tx_kp1[i]-tx_km1[i]);
//        sasfit_out("%d %d %lf %lf %lf\n",k, i,tx_kp1[i]-tx_k[i], tx_kp1[i], tx_k[i]);
    }
    tx_kp1[nr-1]=tx_k[nr-2]*S[nr-1][nr-2]+tx_k[nr-1]*S[nr-1][nr-1];
    eps = eps+gsl_pow_2(tx_kp1[nr-1]-tx_km1[nr-1]);
//    sasfit_out("MC:%d\n",k);
    eps=sqrt(eps);
    k++;
// sasfit_out("it. eps chi2 %d %lg %lg\n",k, eps, chi2);
} while (k<3 || (k<EMparam.maxit && chi2 > EMparam.chi2 && eps>EMparam.eps));
sasfit_out("it. eps chi2 %d %lg %lg\n",k-1, eps, chi2);

for (i=0;i<EP.ndata;i++){
    Ih[i] = 0;
    for (j=0;j<nr;j++) {
        Ih[i] = Ih[i] + dr[j]*A[i][j]*tx_k[j];
    }
}

Tcl_ResetResult(interp);
Tcl_DStringInit(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<nr;i++) {
    sprintf(Buffer,"%lg",r[i]);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<nr;i++) {
	sprintf(Buffer,"%lg",tx_k[i]);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EP.ndata;i++) {
	sprintf(Buffer,"%lg",h[i]);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<EP.ndata;i++) {
	sprintf(Buffer,"%lg",Ih[i]+EP.c0);
	Tcl_DStringAppendElement(&DsBuffer,Buffer);
}
Tcl_DStringEndSublist(&DsBuffer);
Tcl_DStringResult(interp,&DsBuffer);
Tcl_DStringFree(&DsBuffer);

free_dvector(h,0,EP.ndata-1);
free_dvector(Ih,0,EP.ndata-1);
free_dvector(DIh,0,EP.ndata-1);
free_dvector(dq,0,EP.ndata-1);
free_dvector(r,0,nr-1);
free_dvector(dr,0,nr-1);
free_dvector(tx_k,0,nr-1);
free_dvector(tx_kp1,0,nr-1);
free_dvector(ty,0,EP.ndata-1);
free_dvector(tsig,0,EP.ndata-1);
free_dmatrix(A,0,EP.ndata-1,0,nr-1);
free_dmatrix(S,0,nr-1,0,nr-1);
return TCL_OK;
}
