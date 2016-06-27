/*
 * src/sasfit_old/SASFIT_qromb.c
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

#include <math.h>
#include <sasfit.h>
#include "include/SASFIT_nr.h"

#ifndef M_PI
#define M_PI       3.14159265358979323846264338328      /* pi */
#endif

#ifndef M_PI_2
#define M_PI_2     1.57079632679489661923132169164      /* pi/2 */
#endif

#define EPS 1.0e-4
#define EPS_RES 1.0e-4
#define EPS_D_RES 1.0e-4
#define EPS_IQ 1.0e-5
#define EPS_ELL1 1.0e-5
#define EPS_ELL2 1.0e-5
#define EPS_STACKDISCS 1.0e-5
#define JMAX 22
#define JMAXP JMAX+1
#define JMAXELL2 22
#define JMAXPELL2 JMAXELL2+1
#define JMAXINF 14
#define JMAXPINF JMAX+1

#define K 5


float SASFITqrombNR_V_dR(Tcl_Interp *interp, 
                            float a[],
                            sasfit_function*  SD, 
                            sasfit_function*  FF,
                            float Rstart, 
                            float Rend, 
                            bool  *error)
{
	float ss,dss;
	float s[JMAXP+1],h[JMAXP+1];
	int j;

	h[1]=1.0;
	for (j=1;j<=JMAX;j++) {
		s[j]=SASFITtrapzdNR_V_dR(interp,a,SD,FF,Rstart,Rend,j,error);
		if (*error) return 0.0;
		if (j >= K) {
			SASFITpolint(interp,&h[j-K],&s[j-K],K,0.0,&ss,&dss,error);
			if (*error) return 0.0;
			if (fabs(dss) <= sasfit_eps_get_nriq()*fabs(ss)) return ss;
		}
		s[j+1]=s[j];
		h[j+1]=0.25*h[j];
	}
	sasfit_err("Too many steps in routine SASFITqrombNRdR\n");
	*error = TRUE;
	return 0.0;
}


scalar SASFITqrombIQdR(Tcl_Interp *interp,
			int   *dF_dpar,
			scalar l[],
			scalar sq[],
			scalar Q, 
			scalar a[],
			sasfit_function*  SD, 
			sasfit_function*  FF,
			sasfit_function*  SQ,
			int   distr,
			scalar Len_start, 
			scalar Len_end, 
			bool  *error)
{
    scalar *aw, res,err;
    int lenaw=4000;
    sasfit_param4int param4int;
    param4int.dF_dpar=dF_dpar;
    param4int.l=l;
    param4int.sq=sq;
    param4int.Q=Q;
    param4int.a=a;
    param4int.SD=SD;
    param4int.FF=FF;
    param4int.SQ=SQ;
    param4int.distr=distr;
    param4int.error=error;
    
//    aw = (scalar *)malloc((lenaw)*sizeof(scalar));
//    sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
//    sasfit_intde(&IQ_IntdLen, Len_start,Len_end, aw, &res, &err,&param4int);
    aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
    sasfit_intccini(lenaw, aw);
    sasfit_intcc(&IQ_IntdLen, Len_start,Len_end, sasfit_eps_get_nriq(), lenaw, aw, &res, &err,&param4int);
    free(aw);
    if (err < 0) {
        sasfit_err("Integration Int[N(R)I(Q,R),R=0,Infty] did not converged for Q=%lf",Q);
    }
    return res;
}

float SASFITqrombIQdR_old(Tcl_Interp *interp,
		      int  *dF_dpar,
		      float l[],
		      float sq[],
		      float Q, 
		      float a[],
		      sasfit_function*  SD, 
		      sasfit_function*  FF,
		      sasfit_function*  SQ,
		      int   distr,
		      float Rstart, 
		      float Rend, 
		      bool  *error)
{
	float ss,dss;
	float s[JMAXP+1],h[JMAXP+1];
	int j;

	h[1]=1.0;
	for (j=1;j<=JMAX;j++) {
		s[j]=SASFITtrapzdIQdR(interp,dF_dpar,l,sq,Q,a,SD,FF,SQ,distr,Rstart,Rend,j,error);
		if (*error) return 0.0;
		if (j >= K) {
			SASFITpolint(interp,&h[j-K],&s[j-K],K,0.0,&ss,&dss,error);
			if (*error) return 0.0;
			if (fabs(dss) <= sasfit_eps_get_nriq()*fabs(ss)) return ss;
		}
		s[j+1]=s[j];
		h[j+1]=0.25*h[j];
	}
	return ss;
	sasfit_err("Too many steps in routine SASFITqrombIQdR\n");
	*error = TRUE;
	return 0.0;
}


          
float SASFITqrombIQSQdR(Tcl_Interp *interp,
			int   *dF_dpar,
			float l[],
			float sq[],
			float Q, 
			float a[],
			sasfit_function*  SD, 
			sasfit_function*  FF,
			sasfit_function*  SQ,
			int   distr,
			float Rstart, 
			float Rend, 
			bool  *error)
{
	float ss,dss;
	float s[JMAXP+1],h[JMAXP+1];
	int j;

	h[1]=1.0;
	for (j=1;j<=JMAX;j++) {
		s[j]=SASFITtrapzdIQSQdR(interp,dF_dpar,l,sq,Q,a,SD,FF,SQ,distr,Rstart,Rend,j,error);
		if (*error) return 0.0;
		if (j >= K) {
			SASFITpolint(interp,&h[j-K],&s[j-K],K,0.0,&ss,&dss,error);
			if (*error) return 0.0;
			if (fabs(dss) <= sasfit_eps_get_nriq()*fabs(ss)) return ss;
		}
		s[j+1]=s[j];
		h[j+1]=0.25*h[j];
	}
	return ss;
	sasfit_err("Too many steps in routine SASFITqrombIQSQdR\n");
	*error = TRUE;
	return 0.0;
}



float SASFITqrombIQSQijdRi(Tcl_Interp *interp,
						    int *dF_dpar,
						    float l[],
							float len1,
							float sq[],
                            float Q, 
                            float a[],
                            sasfit_function*  SD, 
                            sasfit_function*  FF,
							sasfit_function*  SQ,
                            int   distr,
                            float Rstart, 
                            float Rend, 
                            bool  *error)
{
	float ss,dss;
	float s[JMAXP+1],h[JMAXP+1];
	int j;

	h[1]=1.0;
	for (j=1;j<=JMAX;j++) {
		s[j]=SASFITtrapzdIQSQijdRi(interp,dF_dpar,l,len1,sq,Q,a,SD,FF,SQ,distr,Rstart,Rend,j,error);
		if (*error) return 0.0;
		if (j >= K) {
			SASFITpolint(interp,&h[j-K],&s[j-K],K,0.0,&ss,&dss,error);
			if (*error) return 0.0;
			if (fabs(dss) <= sasfit_eps_get_nriq()*fabs(ss)) return ss;
		}
		s[j+1]=s[j];
		h[j+1]=0.25*h[j];
	}
	return ss;
	sasfit_err("Too many steps in routine SASFITqrombIQSQdR\n");
	*error = TRUE;
	return 0.0;
}

float SASFITqrombIQSQijdRj(Tcl_Interp *interp,
						    int   *dF_dpar,
						    float l[],
							float sq[],
                            float Q, 
                            float a[],
                            sasfit_function*  SD, 
                            sasfit_function*  FF,
							sasfit_function*  SQ,
                            int   distr,
                            float Rstart, 
                            float Rend, 
                            bool  *error)
{
	float ss,dss;
	float s[JMAXP+1],h[JMAXP+1];
	int j;

	h[1]=1.0;
	for (j=1;j<=JMAX;j++) {
		s[j]=SASFITtrapzdIQSQijdRj(interp,dF_dpar,l,sq,Q,a,SD,FF,SQ,distr,Rstart,Rend,j,error);
		if (*error) return 0.0;
		if (j >= K) {
			SASFITpolint(interp,&h[j-K],&s[j-K],K,0.0,&ss,&dss,error);
			if (*error) return 0.0;
			if (fabs(dss) <= sasfit_eps_get_nriq()*fabs(ss)) return ss;
		}
		s[j+1]=s[j];
		h[j+1]=0.25*h[j];
	}
	return ss;
	sasfit_err("Too many steps in routine SASFITqrombIQSQdR\n");
	*error = TRUE;
	return 0.0;
}

float SASFITqrombSA_IQSQijdRi(Tcl_Interp *interp,
                            int   *dF_dpar,
						    float l[],
							float len1,
							float xav,
							float sq[],
                            float Q, 
                            float a[],
                            sasfit_function*  SD, 
                            sasfit_function*  FF,
							sasfit_function*  SQ,
                            int   distr,
                            float Rstart, 
                            float Rend,
                            bool  *error)
{
	float ss,dss;
	float s[JMAXP+1],h[JMAXP+1];
	int j;

	h[1]=1.0;
	for (j=1;j<=JMAX;j++) {
		s[j]=SASFITtrapzdSA_IQSQijdRi(interp,dF_dpar,l,len1,xav,sq,Q,a,SD,FF,SQ,distr,Rstart,Rend,j,error);
		if (*error) return 0.0;
		if (j >= K) {
			SASFITpolint(interp,&h[j-K],&s[j-K],K,0.0,&ss,&dss,error);
			if (*error) return 0.0;
			if (fabs(dss) <= sasfit_eps_get_nriq()*fabs(ss)) return ss;
		}
		s[j+1]=s[j];
		h[j+1]=0.25*h[j];
	}
	return ss;
	sasfit_err("Too many steps in routine SASFITqrombSA_IQSQdR\n");
	*error = TRUE;
	return 0.0;
}

float SASFITqrombSA_IQSQijdRj(Tcl_Interp *interp,
                            int   *dF_dpar,
			    float l[],
			    float xav,
			    float sq[],
                            float Q, 
                            float a[],
                            sasfit_function*  SD, 
                            sasfit_function*  FF,
			    sasfit_function*  SQ,
                            int   distr,
                            float Rstart, 
                            float Rend, 
                            bool  *error)
{
	float ss,dss;
	float s[JMAXP+1],h[JMAXP+1];
	int j;

	h[1]=1.0;
	for (j=1;j<=JMAX;j++) {
		s[j]=SASFITtrapzdSA_IQSQijdRj(interp,dF_dpar,l,xav,sq,Q,a,SD,FF,SQ,distr,Rstart,Rend,j,error);
		if (*error) return 0.0;
		if (j >= K) {
			SASFITpolint(interp,&h[j-K],&s[j-K],K,0.0,&ss,&dss,error);
			if (*error) return 0.0;
			if (fabs(dss) <= sasfit_eps_get_nriq()*fabs(ss)) return ss;
		}
		s[j+1]=s[j];
		h[j+1]=0.25*h[j];
	}
	return ss;
	sasfit_err("Too many steps in routine SASFITqrombSA_IQSQdR\n");
	*error = TRUE;
	return 0.0;
}

float SASFITqromoGz(interp,aa,bb,Q,Qres,par,Ifit,Isubstract,dydpar,max_SD,AP,error_type,error)
Tcl_Interp	*interp;
float		aa,bb,Q,Qres;
float		*par;
float		*Ifit;
float		*Isubstract;
float		*dydpar; 
int			max_SD;
sasfit_analytpar *AP;
int			error_type;
bool		*error;
{
	float ss, dss, *DDss, *DDdss;
	float s[JMAXPINF+1],h[JMAXPINF+1], **DDs, **DDh;
	int j,i,kk;
	bool integrate_ready;

	DDss  = dvector(0,(3*MAXPAR)*max_SD-1);
	DDdss = dvector(0,(3*MAXPAR)*max_SD-1);
    DDs   = matrix(0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
	DDh   = matrix(0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
	h[1]=1.0;
	for (i=0;i<max_SD;i++) {
		for (kk=0;kk<(3*MAXPAR);kk++) {
	       DDh[i*(3*MAXPAR)+kk][1] = 1.0;
		}
	}
	for (j=1;j<=JMAXINF;j++) {
		integrate_ready = TRUE;
		s[j]=SASFITmidexpGzHankel(interp,aa,bb,Q,Qres,par,Ifit,Isubstract,dydpar,max_SD,AP,error_type,j,error);
		for (i=0;i<max_SD;i++) {
		    for (kk=0;kk<(3*MAXPAR);kk++) {
				DDs[i*(3*MAXPAR)+kk][j] = dydpar[i*(3*MAXPAR)+kk];
			}
		}
		if (*error) {
			free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
			free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
			free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
			free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
			return 0.0;
		}
		if (j >= K) {
			SASFITpolint(interp,&h[j-K],&s[j-K],K,0.0,&ss,&dss,error);
			if (*error) {
				free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
				free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
				free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
				free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
				return 0.0;
			}
			for (i=0;i<max_SD;i++) {
				for (kk=0;kk<(3*MAXPAR);kk++) {
					SASFITpolint(interp,&DDh[i*(3*MAXPAR)+kk][j-K],&DDs[i*(3*MAXPAR)+kk][j-K],K,0.0,&DDss[i*(3*MAXPAR)+kk],&DDdss[i*(3*MAXPAR)+kk],error);
					if (*error) {
						free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
						free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
						free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
						free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
						return 0.0;
					}
				}
			}
			for (i=0;i<max_SD;i++) {
				for (kk=0;kk<(3*MAXPAR);kk++) {
					if (fabs(DDdss[i*(3*MAXPAR)+kk]) > sasfit_eps_get_res()*fabs(DDss[i*(3*MAXPAR)+kk])) integrate_ready = FALSE;
				}
			}
			if (fabs(dss) > sasfit_eps_get_res()*fabs(ss)) integrate_ready = FALSE;
			if (integrate_ready == TRUE) return ss;
		}
		s[j+1]=s[j];
		h[j+1]=h[j]/9.0;
		for (i=0;i<max_SD;i++) {
		    for (kk=0;kk<(3*MAXPAR);kk++) {
				DDs[i*(3*MAXPAR)+kk][j+1] = DDs[i*(3*MAXPAR)+kk][j];
				DDh[i*(3*MAXPAR)+kk][j+1] = DDh[i*(3*MAXPAR)+kk][j]/9.0;
			}
		}
	}
	sasfit_err("Too many steps in routine SASFITqromoGz\n");
	*error = TRUE;
	free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
	free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
	free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
	free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
	return 0.0;
}

float SASFITqromoGlobalGz(interp,aa,bb,Q,Qres,par,Ifit,Isub,dydpar,max_SD,GAP,GCP,error_type,error)
Tcl_Interp	*interp;
float		aa,bb,Q,Qres;
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
	float ss, dss, *DDss, *DDdss;
	float s[JMAXPINF+1],h[JMAXPINF+1], **DDs, **DDh;
	int j,i,kk;
	bool integrate_ready;

	DDss  = dvector(0,(3*MAXPAR)*max_SD-1);
	DDdss = dvector(0,(3*MAXPAR)*max_SD-1);
    DDs   = matrix(0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
	DDh   = matrix(0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
	h[1]=1.0;
	for (i=0;i<max_SD;i++) {
		for (kk=0;kk<(3*MAXPAR);kk++) {
	       DDh[i*(3*MAXPAR)+kk][1] = 1.0;
		}
	}
	for (j=1;j<=JMAXINF;j++) {
		integrate_ready = TRUE;
		s[j]=SASFITmidexpGlobalGzHankel(interp,aa,bb,Q,Qres,par,Ifit,Isub,dydpar,max_SD,GAP,GCP,error_type,j,error);
		for (i=0;i<max_SD;i++) {
		    for (kk=0;kk<(3*MAXPAR);kk++) {
				DDs[i*(3*MAXPAR)+kk][j] = dydpar[i*(3*MAXPAR)+kk];
			}
		}
		if (*error) {
			free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
			free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
			free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
			free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
			return 0.0;
		}
		if (j >= K) {
			SASFITpolint(interp,&h[j-K],&s[j-K],K,0.0,&ss,&dss,error);
			if (*error) {
				free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
				free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
				free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
				free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
				return 0.0;
			}
			for (i=0;i<max_SD;i++) {
				for (kk=0;kk<(3*MAXPAR);kk++) {
					SASFITpolint(interp,&DDh[i*(3*MAXPAR)+kk][j-K],&DDs[i*(3*MAXPAR)+kk][j-K],K,0.0,&DDss[i*(3*MAXPAR)+kk],&DDdss[i*(3*MAXPAR)+kk],error);
					if (*error) {
						free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
						free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
						free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
						free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
						return 0.0;
					}
				}
			}
			for (i=0;i<max_SD;i++) {
				for (kk=0;kk<(3*MAXPAR);kk++) {
					if (fabs(DDdss[i*(3*MAXPAR)+kk]) > sasfit_eps_get_res()*fabs(DDss[i*(3*MAXPAR)+kk])) integrate_ready = FALSE;
				}
			}
			if (fabs(dss) > sasfit_eps_get_res()*fabs(ss)) integrate_ready = FALSE;
			if (integrate_ready == TRUE) {
				free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
			    free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
			    free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
  			    free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
				return ss;
			}
		}
		s[j+1]=s[j];
		h[j+1]=h[j]/9.0;
		for (i=0;i<max_SD;i++) {
		    for (kk=0;kk<(3*MAXPAR);kk++) {
				DDs[i*(3*MAXPAR)+kk][j+1] = DDs[i*(3*MAXPAR)+kk][j];
				DDh[i*(3*MAXPAR)+kk][j+1] = DDh[i*(3*MAXPAR)+kk][j]/9.0;
			}
		}
	}
	sasfit_err("Too many steps in routine SASFITqromoGlobalGz\n");
	*error = TRUE;
	free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
	free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
	free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
	free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
	return 0.0;
}

float SASFITqrombIQ(interp,Qmin,Qmax,Q,Qres,par,Ifit,Isubstract,dydpar,max_SD,AP,error_type,error)
Tcl_Interp	*interp;
float		Qmin,Qmax,Q,Qres;
float		*par;
float		*Ifit;
float		*Isubstract;
float		*dydpar; 
int			max_SD;
sasfit_analytpar *AP;
int			error_type;
bool		*error;
{
	float ss, dss, *DDss, *DDdss;
	float s[JMAXP+1],h[JMAXP+1], **DDs, **DDh;
	int j,i,kk;
	bool integrate_ready;

	DDss  = dvector(0,(3*MAXPAR)*max_SD-1);
	DDdss = dvector(0,(3*MAXPAR)*max_SD-1);
    DDs   = matrix(0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
	DDh   = matrix(0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
	h[1]=1.0;
	for (i=0;i<max_SD;i++) {
		for (kk=0;kk<(3*MAXPAR);kk++) {
	       DDh[i*(3*MAXPAR)+kk][1] = 1.0;
		}
	}
	for (j=1;j<=JMAX;j++) {
		integrate_ready = TRUE;
		s[j]=SASFITtrapzdIQ(interp,Qmin,Qmax,Q,Qres,par,Ifit,Isubstract,dydpar,max_SD,AP,error_type,j,error);
		for (i=0;i<max_SD;i++) {
		    for (kk=0;kk<(3*MAXPAR);kk++) {
				DDs[i*(3*MAXPAR)+kk][j] = dydpar[i*(3*MAXPAR)+kk];
			}
		}
		if (*error) {
			free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
			free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
			free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
			free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
			return 0.0;
		}
		if (j >= K) {
			SASFITpolint(interp,&h[j-K],&s[j-K],K,0.0,&ss,&dss,error);
			if (*error) {
				free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
				free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
				free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
				free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
				return 0.0;
			}
			for (i=0;i<max_SD;i++) {
				for (kk=0;kk<(3*MAXPAR);kk++) {
					SASFITpolint(interp,&DDh[i*(3*MAXPAR)+kk][j-K],&DDs[i*(3*MAXPAR)+kk][j-K],K,0.0,&DDss[i*(3*MAXPAR)+kk],&DDdss[i*(3*MAXPAR)+kk],error);
					if (*error) {
						free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
						free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
						free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
						free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
						return 0.0;
					}
				}
			}
			for (i=0;i<max_SD;i++) {
				for (kk=0;kk<(3*MAXPAR);kk++) {
					if (fabs(DDdss[i*(3*MAXPAR)+kk]) > sasfit_eps_get_res()*fabs(DDss[i*(3*MAXPAR)+kk])) integrate_ready = FALSE;
				}
			}
			if (fabs(dss) > sasfit_eps_get_res()*fabs(ss)) integrate_ready = FALSE;
			if (integrate_ready == TRUE) return ss;
		}
		s[j+1]=s[j];
		h[j+1]=0.25*h[j];
		for (i=0;i<max_SD;i++) {
		    for (kk=0;kk<(3*MAXPAR);kk++) {
				DDs[i*(3*MAXPAR)+kk][j+1] =      DDs[i*(3*MAXPAR)+kk][j];
				DDh[i*(3*MAXPAR)+kk][j+1] = 0.25*DDh[i*(3*MAXPAR)+kk][j];
			}
		}
	}
	sasfit_err("Too many steps in routine SASFITqrombIQ\n");
	*error = TRUE;
	free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
	free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
	free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
	free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
	return 0.0;
}

float SASFITqrombIQglobal(interp,Qmin,Qmax,Q,Qres,par,Ifit,Isub,dydpar,max_SD,GAP,GCP,error_type,error)
Tcl_Interp	*interp;
float		Qmin,Qmax,Q,Qres;
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
	float ss, dss, *DDss, *DDdss;
	float s[JMAXP+1],h[JMAXP+1], **DDs, **DDh;
	int j,i,kk;
	bool integrate_ready;

	DDss  = dvector(0,(3*MAXPAR)*max_SD-1);
	DDdss = dvector(0,(3*MAXPAR)*max_SD-1);
    DDs   = matrix(0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
	DDh   = matrix(0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
	h[1]=1.0;
	for (i=0;i<max_SD;i++) {
		for (kk=0;kk<(3*MAXPAR);kk++) {
	       DDh[i*(3*MAXPAR)+kk][1] = 1.0;
		}
	}
	for (j=1;j<=JMAX;j++) {
		integrate_ready = TRUE;
		s[j]=SASFITtrapzdIQglobal(interp,Qmin,Qmax,Q,Qres,par,Ifit,Isub,dydpar,max_SD,GAP,GCP,error_type,j,error);
		for (i=0;i<max_SD;i++) {
		    for (kk=0;kk<(3*MAXPAR);kk++) {
				DDs[i*(3*MAXPAR)+kk][j] = dydpar[i*(3*MAXPAR)+kk];
			}
		}
		if (*error) {
			free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
			free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
			free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
			free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
			return 0.0;
		}
		if (j >= K) {
			SASFITpolint(interp,&h[j-K],&s[j-K],K,0.0,&ss,&dss,error);
			if (*error) {
				free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
				free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
				free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
				free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
				return 0.0;
			}
			for (i=0;i<max_SD;i++) {
				for (kk=0;kk<(3*MAXPAR);kk++) {
					SASFITpolint(interp,&DDh[i*(3*MAXPAR)+kk][j-K],&DDs[i*(3*MAXPAR)+kk][j-K],K,0.0,&DDss[i*(3*MAXPAR)+kk],&DDdss[i*(3*MAXPAR)+kk],error);
					if (*error) {
						free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
						free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
						free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
						free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
						return 0.0;
					}
				}
			}
			for (i=0;i<max_SD;i++) {
				for (kk=0;kk<(3*MAXPAR);kk++) {
					if (fabs(DDdss[i*(3*MAXPAR)+kk]) > sasfit_eps_get_res()*fabs(DDss[i*(3*MAXPAR)+kk])) integrate_ready = FALSE;
				}
			}
			if (fabs(dss) > sasfit_eps_get_res()*fabs(ss)) integrate_ready = FALSE;
			if (integrate_ready == TRUE) {
				free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
			    free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
			    free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
  			    free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
				return ss;
			}
		}
		s[j+1]=s[j];
		h[j+1]=0.25*h[j];
		for (i=0;i<max_SD;i++) {
		    for (kk=0;kk<(3*MAXPAR);kk++) {
				DDs[i*(3*MAXPAR)+kk][j+1] =      DDs[i*(3*MAXPAR)+kk][j];
				DDh[i*(3*MAXPAR)+kk][j+1] = 0.25*DDh[i*(3*MAXPAR)+kk][j];
			}
		}
	}
	sasfit_err("Too many steps in routine SASFITqrombIQ\n");
	*error = TRUE;
	free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
	free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
	free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
	free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
	return 0.0;
}


float SASFITqrombIQ_gsl_global(interp,Qmin,Qmax,Q,Qres,par,Ifit,max_SD,GAP,GCP,error_type,error)
Tcl_Interp	*interp;
float		Qmin,Qmax,Q,Qres;
float		*par;
float		*Ifit;
int			max_SD;
sasfit_analytpar *GAP;
sasfit_commonpar *GCP;
int			error_type;
bool		*error;
{
	float ss, dss;
	float s[JMAXP+1],h[JMAXP+1];
	int j;
	bool integrate_ready;

	h[1]=1.0;

	for (j=1;j<=JMAX;j++) {
		integrate_ready = TRUE;
		s[j]=SASFITtrapzdIQ_gsl_global(interp,Qmin,Qmax,Q,Qres,par,Ifit,max_SD,GAP,GCP,error_type,j,error);
		if (j >= K) {
			SASFITpolint(interp,&h[j-K],&s[j-K],K,0.0,&ss,&dss,error);
			if (fabs(dss) > sasfit_eps_get_res()*fabs(ss)) integrate_ready = FALSE;
			if (integrate_ready == TRUE) return ss;
		}
		s[j+1]=s[j];
		h[j+1]=0.25*h[j];
	}
	sasfit_err("Too many steps in routine SASFITqrombIQ\n");
	*error = TRUE;
	return 0.0;
}

#undef EPS
#undef EPS_D_RES
#undef EPS_RES
#undef EPS_IQ
#undef EPS_ELL1
#undef EPS_ELL2
#undef EPS_STACKDISCS
#undef JMAX
#undef JMAXP
#undef JMAXELL2
#undef JMAXPELL2
#undef JMAXINF 
#undef JMAXPINF 
#undef K

