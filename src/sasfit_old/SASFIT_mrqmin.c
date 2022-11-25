/*
 * src/sasfit_old/SASFIT_mrqmin.c
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
 * original file mrqmin.c
 * changes: #include "SASFIT_nr.h"
 *          all vectors and materices start now with [0] or [0][0]
 *          as there first element, therefore all loop counters has been
 *          decremented by 1
 *          also some declaration in the head of function mrqmin and
 *          the calling of the function mrqcof have been adapted.
 * Joachim Kohlbrecher
 * 08.05.98
 */

#include <math.h>
#include <tcl.h>
#include "include/SASFIT_nr.h"

void SASFITmrqminGlobal(interp,x,y,res,sig,yfit,ysub,ndata,max_SD,GlobalAP,GCP,error_type,
                        SASFIT_CData,chisq,funcs,alamda,error)
Tcl_Interp *interp;
float **x,**y,**sig,**res,**yfit,**ysub,*chisq,*alamda;
int   *ndata,max_SD,error_type;
bool  *error;
sasfit_analytpar *GlobalAP;
sasfit_commonpar *GCP;
struct sasfit_CData SASFIT_CData[];
void  (*funcs)();
{
	int k,kk,j,ihit;
	float *a,*da,*atry,*P_common, **oneda,*beta,ochisq;
    float **alpha, **covar;
	int   common_i;
        
	int ma, mfit,i,index;
	int *lista;
    
        alpha  = SASFIT_CData[0].alpha;
        covar  = SASFIT_CData[0].covar;
        beta   = SASFIT_CData[0].beta;
        oneda  = SASFIT_CData[0].oneda;
        a      = SASFIT_CData[0].a;
        da     = SASFIT_CData[0].da;
		P_common = SASFIT_CData[0].P_common;
		common_i = SASFIT_CData[0].common_i;
        atry   = SASFIT_CData[0].atry;
        ma     = SASFIT_CData[0].ma;
        mfit   = SASFIT_CData[0].mfit;
        max_SD = SASFIT_CData[0].max_SD;
        lista  = SASFIT_CData[0].lista;
        ochisq = SASFIT_CData[0].chisq;

    for (index=0;index<common_i;index++) P_common[index] = (*GCP).P_common[index];

	if (*alamda < 0.0) {
		kk=mfit;
		for (j=0;j<=ma-1;j++) {
			ihit=0;
			for (k=0;k<=mfit-1;k++)
				if (lista[k] == j) ihit++;
			if (ihit == 0) lista[kk++]=j;
			else if (ihit > 1) {
				sasfit_err("Bad LISTA permutation in MRQMINGlobal-1\n");
				*error = TRUE;
				sasfit_paramlist2ap(a, GlobalAP, GCP, max_SD);
				return;
			}
		}
		if (kk != ma) {
		   sasfit_err("Bad LISTA permutation in MRQMINGlobal-2\n");
		   sasfit_paramlist2ap(a, GlobalAP, GCP, max_SD);
		   *error = TRUE;
		   return;
		}
		*alamda=0.001;
		SASFITmrqcofGlobal(interp,x,y,res,sig,yfit,ysub,ndata,a,lista,ma,mfit,GlobalAP,
			GCP,max_SD,error_type,alpha,beta,chisq,funcs,error);
		if (*error == TRUE) {
			sasfit_paramlist2ap(a, GlobalAP, GCP, max_SD);
			return;
		}
		ochisq=(*chisq);
	}
	

	for (j=0;j<=mfit-1;j++) {
		for (k=0;k<=mfit-1;k++) {
			covar[j][k]=alpha[j][k];
		}
		covar[j][j]=alpha[j][j]*(1.0+(*alamda));
		oneda[j][0]=beta[j];
	}
	SASFITgaussj(interp,covar,mfit,oneda,1,error);
    if (*error == TRUE) {
		sasfit_paramlist2ap(a, GlobalAP, GCP, max_SD);
		return;
	}
	for (j=0;j<=mfit-1;j++) da[j]=oneda[j][0];
	if (*alamda == 0.0) {
		sasfit_covar2ap(covar,GlobalAP,GCP,max_SD);
		SASFITcovsrt(interp,covar,ma,lista,mfit,error);
        sasfit_paramlist2ap(a, GlobalAP, GCP, max_SD);
        sasfit_covsrt2ap(covar, GlobalAP, GCP, max_SD);
		return;
	}

	for (j=0;j<=ma-1;j++) atry[j]=a[j];

    for (j=0;j<=mfit-1;j++) {
       i = (int)floor(lista[j]/(3*MAXPAR));
       k = lista[j]-(int)floor(lista[j]/(3*MAXPAR))*(3*MAXPAR);
       if ((int)floor(k/MAXPAR) == 0) {
          index = GlobalAP[i].SD_index[k-(int)floor(k/MAXPAR)*MAXPAR];
	   } else if ((int)floor(k/MAXPAR) == 1) {
          index = GlobalAP[i].FF_index[k-(int)floor(k/MAXPAR)*MAXPAR];
	   } else {
          index = GlobalAP[i].SQ_index[k-(int)floor(k/MAXPAR)*MAXPAR];
	   }

       if (index > 0) {
		   atry[lista[j]] = a[lista[j]]+da[j];	
		   (*GCP).P_common[index-1] = (*GCP).P_common[index-1] + da[j];
	   } else {
		   atry[lista[j]] = a[lista[j]]+da[j];
	   }
	}

	SASFITmrqcofGlobal(interp,x,y,res,sig,yfit,ysub,ndata,atry,lista,ma,mfit,GlobalAP,GCP,max_SD,
	                 error_type,covar,da,chisq,funcs,error);

    if (*error == TRUE) {
	   sasfit_paramlist2ap(a, GlobalAP, GCP, max_SD);
	   return;
	}
	if (*chisq < ochisq) {
	   *alamda *= 0.1;
	   ochisq=(*chisq);
	   for (j=0;j<=mfit-1;j++) {
	       for (k=0;k<=mfit-1;k++) alpha[j][k]=covar[j][k];
		   beta[j]=da[j];
	       a[lista[j]]=atry[lista[j]];
	   }
	   sasfit_out("successful try\n");
	} else {
	   for (index=0;index<common_i;index++) {
		   (*GCP).P_common[index] = P_common[index];
	   }
	   *alamda *= 10.0;
	   *chisq=ochisq;
	   sasfit_out("This try failed\n");
	}

	sasfit_paramlist2ap(a, GlobalAP, GCP, max_SD);
	sasfit_covar2ap(covar,GlobalAP,GCP,max_SD);
    SASFIT_CData[0].chisq = ochisq;

	return;
}


void SASFITmrqmin(interp,x,y,res,sig,yfit,ysubstr,ndata,max_SD,AP,error_type,
                  SASFIT_CData,chisq,funcs,alamda,error)
Tcl_Interp *interp;
float x[],y[],sig[],res[],yfit[],ysubstr[],*chisq,*alamda;
int   ndata,max_SD,error_type;
bool  *error;
sasfit_analytpar *AP;
struct sasfit_CData SASFIT_CData[];
void  (*funcs)();
{
	int k,kk,j,ihit;
	float *a,*da,*atry,**oneda,*beta,ochisq;
    float **alpha, **covar;
	int ma, mfit;
	int *lista;

        alpha  = SASFIT_CData[0].alpha;
        covar  = SASFIT_CData[0].covar;
        beta   = SASFIT_CData[0].beta;
        oneda  = SASFIT_CData[0].oneda;
        a      = SASFIT_CData[0].a;
        da     = SASFIT_CData[0].da;
        atry   = SASFIT_CData[0].atry;
        ma     = SASFIT_CData[0].ma;
        mfit   = SASFIT_CData[0].mfit;
        max_SD = SASFIT_CData[0].max_SD;
        lista  = SASFIT_CData[0].lista;
        ochisq = SASFIT_CData[0].chisq;

	if (*alamda < 0.0) {
		kk=mfit;
		for (j=0;j<=ma-1;j++) {
			ihit=0;
			for (k=0;k<=mfit-1;k++)
				if (lista[k] == j) ihit++;
			if (ihit == 0)
				lista[kk++]=j;
			else if (ihit > 1) {
				sasfit_err("Bad LISTA permutation in MRQMIN-1\n");
				*error = TRUE;
				sasfit_paramlist2ap(a, AP, NULL, max_SD);
				return;
			}
		}
		if (kk != ma) {
		   sasfit_err("Bad LISTA permutation in MRQMIN-2\n");
		   sasfit_paramlist2ap(a, AP, NULL, max_SD);
		   *error = TRUE;
		   return;
		}
		*alamda=0.001;
		SASFITmrqcof(interp,x,y,res,sig,yfit,ysubstr,ndata,a,lista,ma,mfit,AP,
			max_SD,error_type,alpha,beta,chisq,funcs,error);
		if (*error == TRUE) {
			sasfit_paramlist2ap(a, AP, NULL, max_SD);
			return;
		}
		ochisq=(*chisq);
	}
	
	for (j=0;j<=mfit-1;j++) {
		for (k=0;k<=mfit-1;k++) {
			covar[j][k]=alpha[j][k];
		}
		covar[j][j]=alpha[j][j]*(1.0+(*alamda));
		oneda[j][0]=beta[j];
	}
	SASFITgaussj(interp,covar,mfit,oneda,1,error);
    if (*error == TRUE) {
		sasfit_paramlist2ap(a, AP, NULL, max_SD);
		return;
	}
	for (j=0;j<=mfit-1;j++)
		da[j]=oneda[j][0];
	if (*alamda == 0.0) {
		sasfit_covar2ap(covar, AP, NULL, max_SD);
		SASFITcovsrt(interp,covar,ma,lista,mfit,error);
		sasfit_paramlist2ap(a, AP, NULL, max_SD);
		sasfit_covsrt2ap(covar, AP, NULL, max_SD);
		return;
	}
	for (j=0;j<=ma-1;j++) atry[j]=a[j];
	for (j=0;j<=mfit-1;j++) {
		atry[lista[j]] = a[lista[j]]+da[j];
	}
	SASFITmrqcof(interp,x,y,res,sig,yfit,ysubstr,ndata,atry,lista,ma,mfit,AP,max_SD,
		error_type,covar,da,chisq,funcs,error);
	if (*error == TRUE) {
		sasfit_paramlist2ap(a, AP, NULL, max_SD);
		return;
	}
	if (*chisq < ochisq) {
		*alamda *= 0.1;
		ochisq=(*chisq);
		for (j=0;j<=mfit-1;j++) {
			for (k=0;k<=mfit-1;k++) alpha[j][k]=covar[j][k];
			beta[j]=da[j];
			a[lista[j]]=atry[lista[j]];
		}
	} else {
		*alamda *= 10.0;
		*chisq=ochisq;
	}
	sasfit_paramlist2ap(a, AP, NULL, max_SD);
	sasfit_covar2ap(covar, AP, NULL, max_SD);
        SASFIT_CData[0].chisq = ochisq;
	return;
}

void freeMem(float *da, float *atry, float **oneda, float *beta, 
             int ma, int mfit, int *freed)
{
	if (!freed || !da || !atry || !oneda || !beta) {
		return;
	}
	if (*freed) return; // avoid double free

	free_dvector(beta,0,ma);
	free_dvector(da,0,ma);
	free_dvector(atry,0,ma);
	free_matrix(oneda,0,mfit,0,1);
	*freed = 1;
}

void SASFIT2mrqmin(interp,x,y,sig,ndata,a,ma,lista,mfit,covar,alpha,chisq,
		funcs,alamda,error)
Tcl_Interp *interp;
bool *error;
float x[],y[],sig[],a[],**covar,**alpha,*chisq,*alamda;
int ndata,ma,lista[],mfit;
void (*funcs)();
{
	int k,kk,j,ihit;
	static float *da,*atry,**oneda,*beta,ochisq;
	static int freed = 1;

	if (*alamda < -1.0) {
		freeMem(da, atry, oneda, beta, ma, mfit, &freed);
		return;
	}
	if (*alamda == -1.0) {
		oneda=matrix(0,mfit,0,1);
		atry=dvector(0,ma);
		da=dvector(0,ma);
		beta=dvector(0,ma);
		freed = 0; // memory allocated
		kk=mfit;
		for (j=0;j<=ma-1;j++) {
			ihit=0;
			for (k=0;k<=mfit-1;k++)
				if (lista[k] == j) ihit++;
			if (ihit == 0)
				lista[kk++]=j;
			else if (ihit > 1) {
				sasfit_err("Bad LISTA permutation in MRQMIN-1\n");
				*error = TRUE;
				freeMem(da, atry, oneda, beta, ma, mfit, &freed);
				return;
			}
		}
		if (kk != ma) {
			sasfit_err("Bad LISTA permutation in MRQMIN-2\n");
			*error = TRUE;
			freeMem(da, atry, oneda, beta, ma, mfit, &freed);
			return;
		}
		*alamda=0.001;
		SASFIT2mrqcof(interp,x,y,sig,ndata,a,ma,lista,mfit,alpha,
			beta,chisq,funcs,error);
		if (*error == TRUE) {
			freeMem(da, atry, oneda, beta, ma, mfit, &freed);
			return;
		}
		ochisq=(*chisq);
	}
	for (j=0;j<=mfit-1;j++) {
		for (k=0;k<=mfit-1;k++) covar[j][k]=alpha[j][k];
		covar[j][j]=alpha[j][j]*(1.0+(*alamda));
		oneda[j][0]=beta[j];
	}
        SASFITgaussj(interp,covar,mfit,oneda,1,error);
	if (*error == TRUE) {
		freeMem(da, atry, oneda, beta, ma, mfit, &freed);
		return;
	}
	for (j=0;j<=mfit-1;j++)
		da[j]=oneda[j][0];
	if (*alamda == 0.0) {
		SASFITcovsrt(interp,covar,ma,lista,mfit,error);
		freeMem(da, atry, oneda, beta, ma, mfit, &freed);
		return;
	}
	for (j=0;j<=ma-1;j++) atry[j]=a[j];
	for (j=0;j<=mfit-1;j++)
		atry[lista[j]] = a[lista[j]]+da[j];
	SASFIT2mrqcof(interp,x,y,sig,ndata,atry,ma,lista,mfit,covar,da,chisq,
		funcs,error);
	if (*error == TRUE) {
		freeMem(da, atry, oneda, beta, ma, mfit, &freed);
		return;
	}
	if (*chisq < ochisq) {
		*alamda *= 0.1;
		ochisq=(*chisq);
		for (j=0;j<=mfit-1;j++) {
			for (k=0;k<=mfit-1;k++) alpha[j][k]=covar[j][k];
			beta[j]=da[j];
			a[lista[j]]=atry[lista[j]];
		}
	} else {
		*alamda *= 10.0;
		*chisq=ochisq;
	}
	return;
}
