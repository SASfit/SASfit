/*
 * src/sasfit_old/SASFIT_trapzd.c
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

#include <sasfit_sd.h>
#include <sasfit_core.h>
#include "include/SASFIT_nr.h"
#include "include/sasfit.h"


float SASFITtrapzdNR_V_dR(Tcl_Interp* interp, 
			  float* aa, 
			  sasfit_function* SD,
			  sasfit_function* FF,
			  float a, 
			  float b, 
			  int n, 
			  bool* error)
{
	float x,tnm,sum,del;
	float t1, t2;
	static float s;
	static int it;
	int j;

	if (n == 1) {
		it=1;
		t1 = 4.0/3.0*PI*a*a*a*sasfit_sd(a,aa,SD,NULL,error);
		if (*error) return 0.0;
		t2 = 4.0/3.0*PI*b*b*b*sasfit_sd(b,aa,SD,NULL,error);
		if (*error) return 0.0;
		return (s=0.5*(b-a)*(t1+t2));
	} else {
		tnm=it;
		del=(b-a)/tnm;
		x=a+0.5*del;
		for (sum=0.0,j=1;j<=it;j++,x+=del) {
			t1 = 4.0/3.0*PI*x*x*x*sasfit_sd(x,aa,SD,NULL,error);
			if (*error) return 0.0;
			sum += t1;
		}
		it *= 2;
		s=0.5*(s+(b-a)*sum/tnm);
		return s;
	}
}

float SASFITtrapzdIQdR(Tcl_Interp* interp, 
		       int* dF_dpar, 
		       float l[], 
		       float sq[], 
		       float Q, 
		       float aa[], 
		       sasfit_function* SD,
		       sasfit_function* FF,
		       sasfit_function* SQ, 
		       int distr, 
		       float a, 
		       float b, 
		       int n, 
		       bool* error)
{
	float x,tnm,sum,del;
	float t1, t2;
	static float s;
	static int it;
	int j;

	if (n == 1) {
		it=1;
		t1 = IQ_core(interp,dF_dpar,l,sq,a,Q,aa,SD,FF,SQ,distr,error);
		if (*error) return 0.0;
		t2 = IQ_core(interp,dF_dpar,l,sq,b,Q,aa,SD,FF,SQ,distr,error);
		if (*error) return 0.0;
		return (s=0.5*(b-a)*(t1+t2));
	} else {
		tnm=it;
		del=(b-a)/tnm;
		x=a+0.5*del;
		for (sum=0.0,j=1;j<=it;j++,x+=del) {
			t1 = IQ_core(interp,dF_dpar,l,sq,x,Q,aa,SD,FF,SQ,distr,error);
			if (*error) return 0.0;
			sum += t1;
		}
		it *= 2;
		s=0.5*(s+(b-a)*sum/tnm);
		return s;
	}
}

float SASFITtrapzdIQSQdR(Tcl_Interp* interp, 
			 int* dF_dpar, 
			 float l[], 
			 float sq[], 
			 float Q, 
			 float aa[], 
			 sasfit_function* SD, 
			 sasfit_function* FF, 
			 sasfit_function* SQ, 
			 int distr, 
			 float a, 
			 float b, 
			 int n, 
			 bool* error)
{
	float x,tnm,sum,del;
	float t1, t2, V;
	float sqs[MAXPAR];
	static float s;
	static int it;
	int j;

	for (j=0;j<MAXPAR;j++) sqs[j]=sq[j];

	if (n == 1) {
		it=1;
		t1 = IQ_core(interp,dF_dpar,l,sqs,a,Q,aa,SD,FF,SQ,distr,error)		;
		if (*error) return 0.0;
		V = sasfit_volume(a,l,FF,distr,error);
		sqs[0] = pow( 3./(4.*M_PI) *  V, 1./3.);
		//t1 = t1*fSQ(interp,dF_dpar,Q,sqs,SQ,error);
		t1 = t1*sasfit_sq(Q,sqs,SQ,dF_dpar,error);
		if (*error) return 0.0;
		t2 = IQ_core(interp,dF_dpar,l,sqs,b,Q,aa,SD,FF,SQ,distr,error);
		if (*error) return 0.0;
		V = sasfit_volume(b,l,FF,distr,error);
		sqs[0] = pow( 3./(4.*M_PI) *  V, 1./3.);
		//t2 = t2*fSQ(interp,dF_dpar,Q,sqs,SQ,error);
		t2 = t2*sasfit_sq(Q,sqs,SQ,dF_dpar,error);
		if (*error) return 0.0;
		return (s=0.5*(b-a)*(t1+t2));
	} else {
		tnm=it;
		del=(b-a)/tnm;
		x=a+0.5*del;
		for (sum=0.0,j=1;j<=it;j++,x+=del) {
			t1 = IQ_core(interp,dF_dpar,l,sqs,x,Q,aa,SD,FF,SQ,distr,error);
			if (*error) return 0.0;
			V = sasfit_volume(x,l,FF,distr,error);
			sqs[0] = pow( 3./(4.*M_PI) *  V, 1./3.);
   		    t1 = t1*sasfit_sq(Q,sqs,SQ,dF_dpar,error);
			if (*error) return 0.0;
			sum += t1;
		}
		it *= 2;
		s=0.5*(s+(b-a)*sum/tnm);
		return s;
	}
}

float SASFITtrapzdIQSQijdRi(Tcl_Interp* interp, 
			    int* dF_dpar, 
			    float l[], 
			    float len1, 
			    float sq[], 
			    float Q, 
			    float aa[], 
			    sasfit_function* SD, 
			    sasfit_function* FF, 
			    sasfit_function* SQ, 
			    int distr, 
			    float a, 
			    float b, 
			    int n, 
			    bool* error)
{
	float x,tnm,sum,del;
	float t1, t2;
	float sqs[MAXPAR];
	static float s;
	static int it;
	int j;
	for (j=0;j<MAXPAR;j++) sqs[j]=sq[j];
	if (n == 1) {
		it=1;
		t1 = IQSQij_core(interp,dF_dpar,l,sqs,len1,a,Q,aa,SD,FF,SQ,distr,error)		;
		if (*error) return 0.0;
		t2 = IQSQij_core(interp,dF_dpar,l,sqs,len1,b,Q,aa,SD,FF,SQ,distr,error);
		if (*error) return 0.0;
		return (s=0.5*(b-a)*(t1+t2));
	} else {
		tnm=it;
		del=(b-a)/tnm;
		x=a+0.5*del;
		for (sum=0.0,j=1;j<=it;j++,x+=del) {
			t1 = IQSQij_core(interp,dF_dpar,l,sqs,len1,x,Q,aa,SD,FF,SQ,distr,error);
			if (*error) return 0.0;
			sum += t1;
		}
		it *= 2;
		s=0.5*(s+(b-a)*sum/tnm);
		return s;
	}
}


float SASFITtrapzdIQSQijdRj(Tcl_Interp* interp, 
			    int* dF_dpar, 
			    float l[], 
			    float sq[], 
			    float Q, 
			    float aa[], 
			    sasfit_function* SD, 
			    sasfit_function* FF, 
			    sasfit_function* SQ, 
			    int distr, 
			    float a, 
			    float b, 
			    int n, 
			    bool* error)
{
	float x,tnm,sum,del;
	float t1, t2;
	static float s;
	static int it;
	int j;

	if (n == 1) {
		it=1;
		t1 = SASFITqrombIQSQijdRi(interp,dF_dpar,l,a,sq,Q,aa,SD,FF,SQ,distr,a,b,error);
		if (*error) return 0.0;
		t2 = SASFITqrombIQSQijdRi(interp,dF_dpar,l,b,sq,Q,aa,SD,FF,SQ,distr,a,b,error);
		if (*error) return 0.0;
		return (s=0.5*(b-a)*(t1+t2));
	} else {
		tnm=it;
		del=(b-a)/tnm;
		x=a+0.5*del;
		for (sum=0.0,j=1;j<=it;j++,x+=del) {
			t1 = SASFITqrombIQSQijdRi(interp,dF_dpar,l,x,sq,Q,aa,SD,FF,SQ,distr,a,b,error);
			if (*error) return 0.0;
			sum += t1;
		}
		it *= 2;
		s=0.5*(s+(b-a)*sum/tnm);
		return s;
	}
}




float SASFITtrapzdSA_IQSQijdRi(Tcl_Interp* interp, 
			    int* dF_dpar, 
			    float l[], 
			    float len1, 
			    float xav, 
			    float sq[], 
			    float Q, 
			    float aa[], 
			    sasfit_function* SD, 
			    sasfit_function* FF, 
			    sasfit_function* SQ, 
			    int distr, 
			    float a, 
			    float b, 
			    int n, 
			    bool* error)
{
	float x,tnm,sum,del;
	float t1, t2;
	float sqs[MAXPAR];
	static float s;
	static int it;
	int j;
	for (j=0;j<MAXPAR;j++) sqs[j]=sq[j];
	if (n == 1) {
		it=1;
		t1 = IQSQij_SA_core(interp,dF_dpar,l,sqs,len1,a,xav,Q,aa,SD,FF,SQ,distr,error)		;
		if (*error) return 0.0;
		t2 = IQSQij_SA_core(interp,dF_dpar,l,sqs,len1,b,xav,Q,aa,SD,FF,SQ,distr,error);
		if (*error) return 0.0;
		return (s=0.5*(b-a)*(t1+t2));
	} else {
		tnm=it;
		del=(b-a)/tnm;
		x=a+0.5*del;
		for (sum=0.0,j=1;j<=it;j++,x+=del) {
			t1 = IQSQij_SA_core(interp,dF_dpar,l,sqs,len1,x,xav,Q,aa,SD,FF,SQ,distr,error);
			if (*error) return 0.0;
			sum += t1;
		}
		it *= 2;
		s=0.5*(s+(b-a)*sum/tnm);
		return s;
	}
}


float SASFITtrapzdSA_IQSQijdRj(Tcl_Interp* interp, 
			    int* dF_dpar, 
			    float l[], 
			    float xav, 
			    float sq[], 
			    float Q, 
			    float aa[], 
			    sasfit_function* SD, 
			    sasfit_function* FF, 
			    sasfit_function* SQ, 
			    int distr, 
			    float a, 
			    float b, 
			    int n, 
			    bool* error)
{
	float x,tnm,sum,del;
	float t1, t2;
	static float s;
	static int it;
	int j;

	if (n == 1) {
		it=1;
		t1 = SASFITqrombSA_IQSQijdRi(interp,dF_dpar,l,a,xav,sq,Q,aa,SD,FF,SQ,distr,a,b,error);
		if (*error) return 0.0;
		t2 = SASFITqrombSA_IQSQijdRi(interp,dF_dpar,l,b,xav,sq,Q,aa,SD,FF,SQ,distr,a,b,error);
		if (*error) return 0.0;
		return (s=0.5*(b-a)*(t1+t2));
	} else {
		tnm=it;
		del=(b-a)/tnm;
		x=a+0.5*del;
		for (sum=0.0,j=1;j<=it;j++,x+=del) {
			t1 = SASFITqrombSA_IQSQijdRi(interp,dF_dpar,l,x,xav,sq,Q,aa,SD,FF,SQ,distr,a,b,error);
			if (*error) return 0.0;
			sum += t1;
		}
		it *= 2;
		s=0.5*(s+(b-a)*sum/tnm);
		return s;
	}
}


float SASFITtrapzdIQ(interp,Qmin,Qmax,Q,Qres,par,Ifit,Isubstract,dydpar,max_SD,AP,error_type,n,error)
Tcl_Interp	*interp;
int			n;
float		Qmin,Qmax,Qres,Q;
float		*par;
float		*Ifit;
float       *Isubstract;
float		*dydpar; 
int			max_SD;
sasfit_analytpar *AP;
int			error_type;
bool		*error;
{
	float x,tnm,sum,ssum,*dsum,del;
	float t1,ts1, t2,ts2, *dt1, *dt2, tRav;
	static float s,ss;
	static int it;
	int j,i,k;
    float Rav();
    dsum = dvector(0,(3*MAXPAR)*max_SD-1);
	dt1 = dvector(0,(3*MAXPAR)*max_SD-1);
	dt2 = dvector(0,(3*MAXPAR)*max_SD-1);

	if (n == 1) {
		it=1;
		IQ_t(interp,Qmin,par,&t1,&ts1,dt1,max_SD,AP,error_type,error);
		if (*error) {
			*Ifit = 0.0;
			free_dvector(dt1,0,(3*MAXPAR)*max_SD-1);
			free_dvector(dt2,0,(3*MAXPAR)*max_SD-1);
			free_dvector(dsum,0,(3*MAXPAR)*max_SD-1);
			return *Ifit;
		}
		IQ_t(interp,Qmax,par,&t2,&ts2,dt2,max_SD,AP,error_type,error);
		if (*error) {
			free_dvector(dt1,0,(3*MAXPAR)*max_SD-1);
			free_dvector(dt2,0,(3*MAXPAR)*max_SD-1);
			free_dvector(dsum,0,(3*MAXPAR)*max_SD-1);
			*Ifit = 0.0;
			return 0.0;
		}
		t1 = t1*Rav(Qmin,Q,Qres);
		t2 = t2*Rav(Qmax,Q,Qres);
		ts1 = ts1*Rav(Qmin,Q,Qres);
		ts2 = ts2*Rav(Qmin,Q,Qres);
		*Ifit = (s=0.5*(Qmax-Qmin)*(t1+t2));
		*Isubstract = (ss=0.5*(Qmax-Qmin)*(ts1+ts2));
		for (i=0;i<max_SD;i++) {
		    for (k=0;k<(3*MAXPAR);k++) {
		       dydpar[i*(3*MAXPAR)+k] = 0.5*(Qmax-Qmin)
			                 * (  dt1[i*(3*MAXPAR)+k]*Rav(Qmin,Q,Qres)
			                    + dt2[i*(3*MAXPAR)+k]*Rav(Qmax,Q,Qres)
							   );
			}
		}
		free_dvector(dt1,0,(3*MAXPAR)*max_SD-1);
		free_dvector(dt2,0,(3*MAXPAR)*max_SD-1);
		free_dvector(dsum,0,(3*MAXPAR)*max_SD-1);
		return *Ifit;
	} else {
		tnm=it;
		del=(Qmax-Qmin)/tnm;
		x=Qmin+0.5*del;
		for (i=0;i<max_SD;i++) {
			for (k=0;k<(3*MAXPAR);k++) {
				dsum[i*(3*MAXPAR)+k] = 0.0;
			}
		}
		for (sum=ssum=0.0,j=1;j<=it;j++,x+=del) {
			IQ_t(interp,x,par,&t1,&ts1,dt1,max_SD,AP,error_type,error);
			if (*error) {
				free_dvector(dt1,0,(3*MAXPAR)*max_SD-1);
				free_dvector(dt2,0,(3*MAXPAR)*max_SD-1);
				free_dvector(dsum,0,(3*MAXPAR)*max_SD-1);
				*Ifit = 0.0;
				*Isubstract = 0.0;
				return *Ifit;
			}
			tRav = Rav(x,Q,Qres);
			sum += t1*tRav;
			ssum += ts1*tRav;
			for (i=0;i<max_SD;i++) {
				for (k=0;k<(3*MAXPAR);k++) {
					dsum[i*(3*MAXPAR)+k] += dt1[i*(3*MAXPAR)+k]*tRav;
				}
			}
		}
		it *= 2;
		*Ifit      =(s =0.5*(s +(Qmax-Qmin)* sum/tnm));
		*Isubstract=(ss=0.5*(ss+(Qmax-Qmin)*ssum/tnm));
		for (i=0;i<max_SD;i++) {
			for (k=0;k<(3*MAXPAR);k++) {
				dydpar[i*(3*MAXPAR)+k] = 0.5*(dydpar[i*(3*MAXPAR)+k]+(Qmax-Qmin)*dsum[i*(3*MAXPAR)+k]/tnm);
			}
		}
		free_dvector(dt1,0,(3*MAXPAR)*max_SD-1);
		free_dvector(dt2,0,(3*MAXPAR)*max_SD-1);
		free_dvector(dsum,0,(3*MAXPAR)*max_SD-1);
		return *Ifit;
	}
}


float SASFITtrapzdIQglobal(interp,Qmin,Qmax,Q,Qres,par,Ifit,Isub,dydpar,max_SD,GAP,GCP,error_type,n,error)
Tcl_Interp	*interp;
int			n;
float		Qmin,Qmax,Qres,Q;
float		*par;
float		*Ifit;
float		*Isub;
float		*dydpar; 
int			max_SD;
sasfit_analytpar *GAP;
sasfit_commonpar *GCP;
int			error_type;
bool		*error;
{
	float x,tnm,sum,ssum,*dsum,del;
	float t1, t2, *dt1, *dt2, ts1, ts2, tRav;
	static float s, ss;
	static int it;
	int j,i,k;
    float Rav();
    dsum = dvector(0,(3*MAXPAR)*max_SD-1);
	dt1 = dvector(0,(3*MAXPAR)*max_SD-1);
	dt2 = dvector(0,(3*MAXPAR)*max_SD-1);

	if (n == 1) {
		it=1;
		IQ_t_global(interp,Qmin,par,&t1,&ts1,dt1,max_SD,GAP,GCP,error_type,error);
		if (*error) {
			*Ifit = 0.0;
			*Isub = 0.0;
			free_dvector(dt1,0,(3*MAXPAR)*max_SD-1);
			free_dvector(dt2,0,(3*MAXPAR)*max_SD-1);
			free_dvector(dsum,0,(3*MAXPAR)*max_SD-1);
			return *Ifit;
		}
		IQ_t_global(interp,Qmax,par,&t2,&ts2,dt2,max_SD,GAP,GCP,error_type,error);
		if (*error) {
			free_dvector(dt1,0,(3*MAXPAR)*max_SD-1);
			free_dvector(dt2,0,(3*MAXPAR)*max_SD-1);
			free_dvector(dsum,0,(3*MAXPAR)*max_SD-1);
			*Ifit = 0.0;
			*Isub = 0.0;
			return 0.0;
		}
		t1 = t1*Rav(Qmin,Q,Qres);
		t2 = t2*Rav(Qmax,Q,Qres);
		ts1 = ts1*Rav(Qmin,Q,Qres);
		ts2 = ts2*Rav(Qmax,Q,Qres);
		*Ifit = (s =0.5*(Qmax-Qmin)*(t1+t2));
		*Isub = (ss=0.5*(Qmax-Qmin)*(ts1+ts2));

		for (i=0;i<max_SD;i++) {
		    for (k=0;k<(3*MAXPAR);k++) {
		       dydpar[i*(3*MAXPAR)+k] = 0.5*(Qmax-Qmin)
			                 * (  dt1[i*(3*MAXPAR)+k]*Rav(Qmin,Q,Qres)
			                    + dt2[i*(3*MAXPAR)+k]*Rav(Qmax,Q,Qres)
							   );
			}
		}
		free_dvector(dt1,0,(3*MAXPAR)*max_SD-1);
		free_dvector(dt2,0,(3*MAXPAR)*max_SD-1);
		free_dvector(dsum,0,(3*MAXPAR)*max_SD-1);
		return *Ifit;
	} else {
		tnm=it;
		del=(Qmax-Qmin)/tnm;
		x=Qmin+0.5*del;
		for (i=0;i<max_SD;i++) {
			for (k=0;k<(3*MAXPAR);k++) {
				dsum[i*(3*MAXPAR)+k] = 0.0;
			}
		}
		for (sum=0.0,ssum=0.0,j=1;j<=it;j++,x+=del) {
			IQ_t_global(interp,x,par,&t1,&ts1,dt1,max_SD,GAP,GCP,error_type,error);
			if (*error) {
				free_dvector(dt1,0,(3*MAXPAR)*max_SD-1);
				free_dvector(dt2,0,(3*MAXPAR)*max_SD-1);
				free_dvector(dsum,0,(3*MAXPAR)*max_SD-1);
				*Ifit = 0.0;
				*Isub = 0.0;
				return *Ifit;
			}
			tRav = Rav(x,Q,Qres);
			sum += t1*tRav;
			ssum += ts1*tRav;
			for (i=0;i<max_SD;i++) {
				for (k=0;k<(3*MAXPAR);k++) {
					dsum[i*(3*MAXPAR)+k] += dt1[i*(3*MAXPAR)+k]*tRav;
				}
			}
		}
		it *= 2;
		*Ifit = (s =0.5*(s +(Qmax-Qmin)* sum/tnm));
		*Isub = (ss=0.5*(ss+(Qmax-Qmin)*ssum/tnm));
		for (i=0;i<max_SD;i++) {
			for (k=0;k<(3*MAXPAR);k++) {
				dydpar[i*(3*MAXPAR)+k] = 0.5*(dydpar[i*(3*MAXPAR)+k]+(Qmax-Qmin)*dsum[i*(3*MAXPAR)+k]/tnm);
			}
		}
		free_dvector(dt1,0,(3*MAXPAR)*max_SD-1);
		free_dvector(dt2,0,(3*MAXPAR)*max_SD-1);
		free_dvector(dsum,0,(3*MAXPAR)*max_SD-1);
		return *Ifit;
	}
}


float SASFITtrapzdIQ_gsl_global(interp,Qmin,Qmax,Q,Qres,par,Ifit,max_SD,GAP,GCP,error_type,n,error)
Tcl_Interp	*interp;
int			n;
float		Qmin,Qmax,Qres,Q;
float		*par;
float		*Ifit;
int			max_SD;
sasfit_analytpar *GAP;
sasfit_commonpar *GCP;
int			error_type;
bool		*error;
{
	float x,tnm,sum,del;
	float t1, t2, tRav;
	static float s;
	static int it;
	int j;
    float Rav();

	if (n == 1) {
		it=1;
		IQ_t_gsl_global(interp,Qmin,par,&t1,max_SD,GAP,GCP,error_type,error);
		if (*error) {
			*Ifit = 0.0;
			return *Ifit;
		}
		IQ_t_gsl_global(interp,Qmax,par,&t2,max_SD,GAP,GCP,error_type,error);
		if (*error) {
			*Ifit = 0.0;
			return 0.0;
		}
		t1 = t1*Rav(Qmin,Q,Qres);
		t2 = t2*Rav(Qmax,Q,Qres);
		*Ifit = (s=0.5*(Qmax-Qmin)*(t1+t2));
		return *Ifit;
	} else {
		tnm=it;
		del=(Qmax-Qmin)/tnm;
		x=Qmin+0.5*del;
		for (sum=0.0,j=1;j<=it;j++,x+=del) {
			IQ_t_gsl_global(interp,x,par,&t1,max_SD,GAP,GCP,error_type,error);
			if (*error) {
				*Ifit = 0.0;
				return *Ifit;
			}
			tRav = Rav(x,Q,Qres);
			sum += t1*tRav;
		}
		it *= 2;
		*Ifit=(s=0.5*(s+(Qmax-Qmin)*sum/tnm));
		return *Ifit;
	}
}

