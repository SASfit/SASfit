/*
 * src/sasfit_old/SASFIT_mrqcof.c
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
 * original file mrqcof.c
 * changes: #include "SASFIT_nr.h"
 *          all vectors and matrices start now with [0] or [0][0]
 *          as their first element, therefore all loop counters has been
 *          decremented by 1
 *          also some declaration in the head of function mrqcof and
 *          the additional call of the function SASFIT_lista has been
 *          changed
 * Joachim Kohlbrecher
 * 08.11.2004
 */

#include <math.h>
#include <tcl.h>
#include "include/SASFIT_nr.h"


void SASFITmrqcofGlobal(interp,x,y,res,sig,yfit,ysubstr,ndata,a,lista,ma,mfit,GlobalAP,GCP,max_SD,
		error_type,alpha,beta,chisq,funcs,error)
Tcl_Interp *interp;
float **x,**y,**sig,**yfit,**ysubstr,**res,a[],**alpha,beta[],*chisq;
int   *ndata,ma,mfit,lista[],max_SD,error_type;
bool  *error;
sasfit_analytpar *GlobalAP;
sasfit_commonpar *GCP;
void (*funcs)();
{
	int k,j,i,m;
	float ymod,ysub,yexp,dy,wt,sigma,sig2i,*dyda;

	dyda=dvector(0,ma-1);
	for (i=0;i<=ma-1;i++) dyda[i] = 0.0;

	for (j=0;j<=mfit-1;j++) {
		for (k=0;k<=j;k++) alpha[j][k]=0.0;
		beta[j]=0.0;
	}

	(*chisq)=0.0;

for (m=0;m<(*GCP).nmultset;m++) {
	for (i=0;i<ndata[m];i++) {
        switch (error_type) {
           case 0 :  { yexp = y[m][i];       break; }
           case 1 :  { yexp = log(y[m][i]);  break; }
           case 2 :  { yexp = sqrt(y[m][i]); break; }
           case 3 :  { yexp = y[m][i];       break; }
           default : { 
                       *error = TRUE;
	                   sasfit_err("SASFITmrqcofGlobal: unknown error type\n");
		               free_dvector(dyda,0,ma-1);
		               return;
					 }
        }
		(*funcs)(interp,x[m][i],res[m][i],a,&ymod,&ysub,dyda,max_SD,GlobalAP,GCP,error_type,m+1,error);
		if (*error == TRUE) {
			free_dvector(dyda,0,ma-1);
			return;
        }
        switch (error_type) {
           case 0 :  { yfit[m][i] = ymod;		ysubstr[m][i]=ysub;			sigma = sig[m][i];	break; }
           case 1 :  { yfit[m][i] = log(ymod);	ysubstr[m][i]=log(ysub);	sigma = 1.0;		break; }
           case 2 :  { yfit[m][i] = sqrt(ymod); ysubstr[m][i]=sqrt(ysub);	sigma = 1.0;		break; }
		   case 3 :  { yfit[m][i] = ymod;		ysubstr[m][i]=ysub;			sigma = 1.0;		break; }
           default : { 
                       *error = TRUE;
	                   sasfit_err("SASFITmrqcofGlobal: unknown error type\n");
		               free_dvector(dyda,0,ma-1);
		               return;
					 }
        }
		sig2i=1.0/(sigma*sigma);
		dy=yexp-ymod;

		for (j=0;j<=mfit-1;j++) {
			wt=dyda[lista[j]]*sig2i;
			for (k=0;k<=j;k++) {
				alpha[j][k] += wt*dyda[lista[k]];
//				sasfit_err("%lf",wt*dyda[lista[k]]);
			}
			beta[j] += dy*wt;
		} 
		(*chisq) += dy*dy*sig2i;
	}
// for (j=0;j<=mfit-1;j++) {
//     for (k=0;k<=mfit-1;k++) {
// 			sasfit_err("%d alpha_%d_%d:%lf",m,j,k,alpha[j][k]);
// 	   }
// }

}


for (j=1;j<=mfit-1;j++) {
    for (k=0;k<=j-1;k++) {
	    alpha[k][j]=alpha[j][k];
	}
}
//for (j=0;j<=mfit-1;j++) {
//    for (k=0;k<=mfit-1;k++) {
//			sasfit_err("nmultset:%d alpha_%d_%d:%lf",(*GCP).nmultset,k,j,alpha[k][j]);
//	}
//}
free_dvector(dyda,0,ma-1);
}



void SASFITmrqcof(interp,x,y,res,sig,yfit,ysubstr,ndata,a,lista,ma,mfit,AP,max_SD,
		error_type,alpha,beta,chisq,funcs,error)
Tcl_Interp *interp;
float x[],y[],sig[],yfit[],ysubstr[],res[],a[],**alpha,beta[],*chisq;
int   ndata,ma,mfit,lista[],max_SD,error_type;
bool  *error;
sasfit_analytpar *AP;
void (*funcs)();
{
	int k,j,i;
	float ymod,ysub,yexp,dy,wt,sigma,sig2i,*dyda;

	dyda=dvector(0,ma-1);
	for (j=0;j<=mfit-1;j++) {
		for (k=0;k<=mfit-1;k++) alpha[j][k]=0.0;
		beta[j]=0.0;
	}
	*chisq=0.0;
	sasfit_paramlist2ap(a, AP, NULL, max_SD);
	for (i=0;i<=ndata-1;i++) {
                switch (error_type) {
                  case 0 :  { yexp = y[i]; break; }
                  case 1 :  { yexp = log(y[i]); break; }
                  case 2 :  { yexp = sqrt(y[i]); break; }
				  case 3 :  { yexp = y[i]; break; }
                  default : { 
                             *error = TRUE;
	                         sasfit_err("SASFITmrqcof: unknown error type\n");
			                 free_dvector(dyda,0,ma-1);
			                 return;
							}
                }
		(*funcs)(interp,x[i],res[i],a,&ymod,&ysub,dyda,max_SD,AP,error_type,0,error);
		if (*error == TRUE) {
			free_dvector(dyda,0,ma-1);
			return;
		}
                switch (error_type) {
                  case 0 :  { yfit[i] = ymod;		ysubstr[i] = ysub;			sigma = sig[i]; break; }
                  case 1 :  { yfit[i] = log(ymod);	ysubstr[i] = log(ysub);		sigma = 1.0;	break; }
                  case 2 :  { yfit[i] = sqrt(ymod); ysubstr[i] = sqrt(ysub);	sigma = 1.0;	break; }
                  case 3 :  { yfit[i] = ymod;		ysubstr[i] = ysub;			sigma = 1.0;	break; }
                  default : { 
                             *error = TRUE;
	                         sasfit_err("SASFITmrqcof: unknown error type\n");
				 free_dvector(dyda,0,ma-1);
				 return;
				}
                }
		sig2i=1.0/(sigma*sigma);
		dy=yexp-ymod;
		for (j=0;j<=mfit-1;j++) {
			wt=dyda[lista[j]]*sig2i;
			for (k=0;k<=j;k++) {
				alpha[j][k] += wt*dyda[lista[k]];
			}
			beta[j] += dy*wt;
		} 
		(*chisq) += dy*dy*sig2i;
	}
	for (j=1;j<=mfit-1;j++){
		for (k=0;k<=j-1;k++) {
			alpha[k][j]=alpha[j][k];
		}
	}
	free_dvector(dyda,0,ma-1);
}




void SASFIT2mrqcof(interp,x,y,sig,ndata,a,ma,lista,mfit,alpha,beta,chisq,
	funcs,error)
Tcl_Interp *interp;
bool *error;
float x[],y[],sig[],a[],**alpha,beta[],*chisq;
int ndata,ma,lista[],mfit;
void (*funcs)();
{
	int k,j,i;
	float ymod,wt,sig2i,dy,*dyda;
    char errstr[132];

	dyda=dvector(0,ma-1);
	for (j=0;j<=mfit-1;j++) {
		for (k=0;k<=j;k++) alpha[j][k]=0.0;
		beta[j]=0.0;
	}
	*chisq=0.0;
	for (i=0;i<=ndata-1;i++) {
		(*funcs)(x[i],a,&ymod,dyda,ma,error,errstr);
		if (*error == TRUE) {
			sasfit_err("SASFITmrqcof: error in function evaluation\n%s\n",errstr);
			free_dvector(dyda,0,ma-1);
			return;
		}
		sig2i=1.0/(sig[i]*sig[i]);
		dy=y[i]-ymod;
		for (j=0;j<=mfit-1;j++) {
			wt=dyda[lista[j]]*sig2i;
			for (k=0;k<=j;k++)
				alpha[j][k] += wt*dyda[lista[k]];
			beta[j] += dy*wt;
		}
		(*chisq) += dy*dy*sig2i;
	}
	for (j=1;j<=mfit-1;j++)
		for (k=0;k<=j-1;k++) alpha[k][j]=alpha[j][k];
	free_dvector(dyda,0,ma-1);
}
