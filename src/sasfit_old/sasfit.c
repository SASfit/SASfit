/*
 * src/sasfit_old/sasfit.c
 *
 * Copyright (c) 2008-2017, Paul Scherrer Institute (PSI)
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
/*# Fitting size distributions of different particle shapes on            #*/
/*# experimental data Q, I(Q), Delta I(Q)                                 #*/
/*#                                                                       #*/
/*# implemented size distributions:                                       #*/
/*#   log normal distribution :  logNorm(s,p,m,R)                         #*/
/*#                                                                       #*/
/*# implemented form factors:                                             #*/
/*#  sphere         : K(Q,R,eta)                                          #*/
/*#  spherical shell: Ksh(Q,R1,R2,eta,mu)=K(Q,R1,eta)-K(Q,R2,eta*(1-mu))  #*/
/*#  spherical shell: Ksh(Q,R,nu*R,eta,mu)=K(Q,R,eta)-K(Q,nu*R,eta*(1-mu))#*/
/*#                                                                       #*/
/*# Written by Joachim Kohlbrecher                                        #*/
/*#########################################################################*/
/*#     Version Va1.0 07.03.2005  Joachim Kohlbrecher                     #*/
/*#########################################################################*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef MACOSX
#include <sys/malloc.h>
#else
#include <malloc.h>
#endif

#include <sasfit_sd.h>
#include <sasfit_sq.h>

#include <tcl.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_integration.h>

#include "include/sasfit.h"
#include "include/SASFIT_nr.h"
#include "include/SASFIT_x_tcl.h"
#include "include/SASFIT_resolution.h"
#include "include/tcl_cmds.h"
#include <sasfit_common.h>
#include <sasfit_plugin.h>
#include <sasfit_core.h>
#include <sasfit_plugin_backend.h>
#include <sasfit_oz.h>
#include <sasfit_frida.h>
//#include <omp.h>

#define NRES 30
#define n_percent 0.1
#define Nint 20




typedef enum {
	LogNorm,
	BiLogNorm,
	WEIBULL,
	GAUSSIAN,
	SchultzZimm,
	Maxwell,
	GEX,
	GEV,
	Uniform,
	Triangular,
	fractalSD,
	Beta,
	gammaSD,
	Delta
} SizeDistr;


int check_interrupt4calc(Tcl_Interp *interp, bool *error) {
    int interupt_signal;
    char *sBuffer;

    sBuffer = Tcl_GetVar(interp,"::SASfitinterrupt",TCL_GLOBAL_ONLY);
    sscanf(sBuffer,"%d",&interupt_signal);
    if (interupt_signal == 1) {
            *error = TRUE;
            sasfit_err("interrupting calculation (%d)\n",interupt_signal);
    }
    return interupt_signal;
}

int select_str(char *str)
{
	SizeDistr sd;
	if ((strcmp(str,"LogNorm")==0)         ||
		(strcmp(str,"d LogNorm / d a1")==0) ||
		(strcmp(str,"d LogNorm / d a2")==0) ||
		(strcmp(str,"d LogNorm / d a3")==0) ||
		(strcmp(str,"d LogNorm / d a4")==0)
		)
	{
			return LogNorm;
	} else if ((strcmp(str,"BiLogNorm")==0)         ||
		(strcmp(str,"d BiLogNorm / d a1")==0) ||
		(strcmp(str,"d BiLogNorm / d a2")==0) ||
		(strcmp(str,"d BiLogNorm / d a3")==0) ||
		(strcmp(str,"d BiLogNorm / d a4")==0)
		)
	{
			return BiLogNorm;
	} else if ((strcmp(str,"Schultz-Zimm")==0)         ||
		(strcmp(str,"d Schulz-Zimm / d a1")==0) ||
		(strcmp(str,"d Schulz-Zimm / d a2")==0) ||
		(strcmp(str,"d Schulz-Zimm / d a3")==0) ||
		(strcmp(str,"d Schulz-Zimm / d a4")==0)
		)
	{
			return SchultzZimm;
	}
}

void which_len(Tcl_Interp *interp,
	       scalar len,
               scalar l[],
               int   distr,
               bool  *err)
/*############################################################################*/
/******************************************************************************
 * set the parameter from the array l which has a size distrubution           *
 * to the value of the variable len                                           *
 ******************************************************************************/
{
	if (distr < 0) return; // nothing to be done
	if (distr >= MAXPAR)
	{
		if (err) *err = TRUE;
		sasfit_err("form factor param out of range: %d\n", distr);
	}
	l[distr] = len;
}

/******************************************************************************
 * this procedure returns a reasonable integration range [Rstart,Rend]        *
 * of the integration of a LogNorm size distribution and suggest a            *
 * number of subintervals for the integration routine integral_int_core.      *
 * the integration stops at R_n for which LogNorm(R_n)/LogNorm(Rmax) =        *
 *                                        n_percent/100                       *
 * the number of integration intervals is calculated by R_n/R_50 * Nint       *
 ******************************************************************************/
void find_integration_range(Tcl_Interp *interp,
							scalar *Rstart,
                            scalar *Rend,
                            int   *n_intervals,
                            scalar *a,
                            char  *SD_typestr,
							int moment,
                            bool  *error)
{
	scalar R_n, R_50,R_0,R_max, tmp;
	scalar R2_n, R2_50,R2_0, R2start, R2end;
	int   n;
	scalar a1,a2,a3,a4,a5;
	int funcid;
	sasfit_param subParam;
	const sasfit_plugin_func_t * func_descr;

    func_descr = NULL;

	a1=a[0];
	a2=a[1];
	a3=a[2];
	a4=a[3];
	a5=a[4];

	sasfit_init_param( &subParam );
	subParam.p[0] = a[0];
	subParam.p[1] = a[1];
	subParam.p[2] = a[2];
	subParam.p[3] = a[3];
	subParam.p[4] = a[4];

	if ( (strcmp(SD_typestr,"LogNorm")          == 0) ||
	     (strcmp(SD_typestr,"d LogNorm / d a1") == 0) ||
	     (strcmp(SD_typestr,"d LogNorm / d a2") == 0) ||
	     (strcmp(SD_typestr,"d LogNorm / d a3") == 0) ||
	     (strcmp(SD_typestr,"d LogNorm / d a4") == 0)   ) {
	   a4 = fabs(a4);
	   R_0  = a4*exp(-a2*a2*(a3-moment));
	   R_n  = a4*exp(-a2*a2*(a3-moment)+sqrt(2.0*a2*a2*log(100.0/n_percent)));
	   R_50 = a4*exp(-a2*a2*(a3-moment)+sqrt(2.0*a2*a2*log(100.0/50.0)));
//sasfit_out("find_integration_range, a0: %lg, a1: %lg, a2: %lg, a3: %lg, a4: %lg\n",a[0],a[1],a[2],a[3],a[4]);
//sasfit_out("find_integration_range, R_0: %lg, R_n: %lg, R_50: %lg\n",R_0,R_n,R_50);
	   if (SASFIT_EQUAL(R_50, 0.0)) {
		  sasfit_err("#find_integration_range: can't guess good integration interval: >%s(%lg,%lg,%lg,%lg)<\n",SD_typestr,a1,a2,a3,a4);
	      sasfit_out("find_integration_range: ");
	      sasfit_out("can't guess good integration interval\n");
	      sasfit_out("find_integration_range: >%s(%lg,%lg,%lg,%lg)<\n", SD_typestr,a1,a2,a3,a4);
	      *Rstart=0.0;
	      *Rend = 100.0;
	      *n_intervals = 500;
	      *error = TRUE;
	      return;
	   } else {
	      *n_intervals =  abs((int)ceil(R_n/(R_0-R_50))) * Nint;
//sasfit_out("n_intervals: %d, (R_n-R_0): %lg, (2.0*R_0-R_n): %lg, cond: %d\n",*n_intervals, (R_n-R_0),(2.0*R_0-R_n),((R_n-R_0) < R_0));
	      *Rstart = 0.0;
	      *Rend = R_n;
		  if (*n_intervals > 4000) {
			  if ((R_n-R_0) < R_0) {
		      *Rend = R_n;
				  *Rstart = 2.0*R_0-R_n;
				  *n_intervals = Nint;
//sasfit_out("Rstart: %f\n",Rstart);
			  } else {
			sasfit_err("#find_integration_range: too many (%d) integration interval: >%s(%lg,%lg,%lg,%lg)<\n",*n_intervals,SD_typestr,a1,a2,a3,a4);
		    *error = TRUE;
			  }
		  }
//sasfit_out("find_integration_rangef, rstart: %lg, rend: %lg, n_intervals: %d, Nint: %d\n",*Rstart,*Rend,*n_intervals, Nint);
	      return;
	   }
	} else if ( (strcmp(SD_typestr,"BiLogNorm")          == 0) ||
	     (strcmp(SD_typestr,"d BiLogNorm / d a1") == 0) ||
	     (strcmp(SD_typestr,"d BiLogNorm / d a2") == 0) ||
	     (strcmp(SD_typestr,"d BiLogNorm / d a3") == 0) ||
	     (strcmp(SD_typestr,"d BiLogNorm / d a4") == 0)   ) {
	   a4 = fabs(a4);
	   R_0  = a4*exp(-a2*a2*(a3-moment));
	   R_n  = a4*exp(-a2*a2*(a3-moment)+sqrt(2.0*a2*a2*log(100.0/n_percent)));
	   R_50 = a4*exp(-a2*a2*(a3-moment)+sqrt(2.0*a2*a2*log(100.0/50.0)));
	   if (SASFIT_EQUAL(R_50, 0.0)) {
		  sasfit_err("#find_integration_range: can't guess good integration interval: >%s(%lg,%lg,%lg,%lg)<\n",SD_typestr,a1,a2,a3,a4);
	      sasfit_out("find_integration_range: ");
	      sasfit_out("can't guess good integration interval\n");
	      sasfit_out("find_integration_range: >%s(%lg,%lg,%lg,%lg)<\n", SD_typestr,a1,a2,a3,a4);
	      *Rstart=0.0;
	      *Rend = 100.0;
	      *n_intervals = 500;
	      *error = TRUE;
	      return;
	   } else {
	      *n_intervals =  abs((int)ceil(R_n/(R_0-R_50))) * Nint;
	      *Rstart = 0.0;
	      *Rend = R_n;
		  if (*n_intervals > 500) {
			  if ((R_n-R_0) < R_0) {
		      *Rend = R_n;
				  *Rstart = 2.0*R_0-R_n;
				  *n_intervals = Nint;
			  } else {
			sasfit_err("#find_integration_range: to many (%d) integration interval: >%s(%lg,%lg,%lg,%lg)<\n",*n_intervals,SD_typestr,a1,a2,a3,a4);
		    *error = TRUE;
				return;
			  }
		  }
	   }

	   a1=a[4];
	   a2=a[5];
	   a3=a[6];
	   a4=a[7];
	   R2_0 = R_0;
	   R2_n = R_n;
	   R2_50 = R_50;
	   R2start = *Rstart;
	   R2end = *Rend;

	   R_0  = a4*exp(-a2*a2*(a3-moment));
	   R_n  = a4*exp(-a2*a2*(a3-moment)+sqrt(2.0*a2*a2*log(100.0/n_percent)));
	   R_50 = a4*exp(-a2*a2*(a3-moment)+sqrt(2.0*a2*a2*log(100.0/50.0)));
	   if (SASFIT_EQUAL(R_50, 0.0)) {
		  sasfit_err("#find_integration_range: can't guess good integration interval: >%s(%lg,%lg,%lg,%lg)<\n",SD_typestr,a1,a2,a3,a4);
	      sasfit_out("find_integration_range: ");
	      sasfit_out("can't guess good integration interval\n");
	      sasfit_out("find_integration_range: >%s(%lg,%lg,%lg,%lg)<\n", SD_typestr,a1,a2,a3,a4);
	      *Rstart=0.0;
	      *Rend = 100.0;
	      *n_intervals = 500;
	      *error = TRUE;
	      return;
	   } else {
	      *n_intervals =  abs((int)ceil(R_n/(R_0-R_50))) * Nint;
	      *Rstart = 0.0;
	      *Rend = R_n;
		  if (*n_intervals > 500) {
			  if ((R_n-R_0) < R_0) {
		      *Rend = R_n;
				  *Rstart = 2.0*R_0-R_n;
				  *n_intervals = Nint;
			  } else {
			sasfit_err("#find_integration_range: to many (%d) integration interval: >%s(%lg,%lg,%lg,%lg)<\n",*n_intervals,SD_typestr,a1,a2,a3,a4);
		    *error = TRUE;
				return;
			  }
		  }
		  if (R2end > *Rend) *Rend = R2end;
		  if (R2end < *Rend) *Rstart = R2start;
	   }
	} else if (   (strcmp(SD_typestr,"Monodisperse")==0)
			    ||(strcmp(SD_typestr,"A_div_B")==0)
			    ||(strcmp(SD_typestr,"d A_div_B / d a1")==0)
			    ||(strcmp(SD_typestr,"d A_div_B / d a2")==0)
			    ||(strcmp(SD_typestr,"Delta")==0)
			    ||(strcmp(SD_typestr,"d Delta / d a1")==0)) {
	   *n_intervals = 1;
	   *Rstart = 0.0;
	   *Rend = 10.0*a4;
	} else if ( (strcmp(SD_typestr,"Maxwell")          == 0) ||
		    (strcmp(SD_typestr,"d Maxwell / d a1") == 0) ||
		    (strcmp(SD_typestr,"d Maxwell / d a2") == 0) ||
		    (strcmp(SD_typestr,"d Maxwell / d a3") == 0)   ) {
	   R_max = sqrt(8.0*a2*a2);
	   R_n = R_max;
	   n = 1;
	   tmp = 0.0;
	   if (a3 > 0.0)  tmp=a3;
	   while ( (pow((n*R_n+tmp),moment) * sasfit_sd_Maxwell(n*R_n+tmp, &subParam) ) >
		   (pow(R_max,moment) * n_percent / 100.0
				   * sasfit_sd_Maxwell(R_max, &subParam)) ) {
	      n++;
	   }
	   R_n = n*R_n;
	   *Rstart = tmp;
	   *Rend   = R_n+tmp;
	   *n_intervals = Nint * n;
	} else if ( (strcmp(SD_typestr,"Schulz-Zimm")          == 0) ||
		    (strcmp(SD_typestr,"d Schulz-Zimm / d a1") == 0) ||
		    (strcmp(SD_typestr,"d Schulz-Zimm / d a2") == 0) ||
		    (strcmp(SD_typestr,"d Schulz-Zimm / d a3") == 0)   ) {
	   if (a3 > 1.0) {
	      R_n = (a3-1+moment)/a3 * a2;
	      R_max =  R_n;
	   } else {
	      R_n = a2;
	      R_max = a2;
	   }
	   R_n = R_max;
	   n = 1;
	   while ((pow(n*R_n,moment)*sasfit_sd_Schulz_Zimm(n*R_n, &subParam)) >
		  (n_percent / 100.0 * pow(R_max,moment)
				     * sasfit_sd_Schulz_Zimm(R_max, &subParam)) ) {
	      n++;
	   }
	   R_n = n*R_n;
	   *Rend   = R_n;
	   if (a3 < 1.0) {
	      *n_intervals = Nint * n;
	      *Rstart = R_n/(*n_intervals+1.0);
	   } else {
	      *n_intervals = Nint* n;
	      *Rstart = 0.0;
	   }
	} else if ( (strcmp(SD_typestr,"Weibull")          == 0) ||
		    (strcmp(SD_typestr,"d Weibull / d a1") == 0) ||
		    (strcmp(SD_typestr,"d Weibull / d a2") == 0) ||
		    (strcmp(SD_typestr,"d Weibull / d a3") == 0)   ) {
	   if (a3 > 0.0) {
	      R_max = fabs(a2*pow((a3-1+moment)/a3,1.0/a3));
	      R_n = R_max;
	      n = 1;
	      tmp = 0.0;
	      if (a4 > 0.0) tmp = a4;
	      while ( (pow(n*R_n+tmp,moment) * sasfit_sd_Weibull(n*R_n+tmp, &subParam)) >
		      (pow(R_max+a4,moment)  * n_percent / 100.0
				      * sasfit_sd_Weibull(R_max+a4, &subParam)) ) {
		 n++;
	      }
	      R_n = n*R_n;
	      *Rstart = tmp;
	      *Rend   = R_n+tmp;
	      *n_intervals = Nint * n;
	   } else if (a3 == 0.0) {
	      R_50 = log(2.0) * a2;
	      R_n = log(n_percent / 100.0) * a2 + a4;
	      *Rend = R_n;
	      *Rstart = 0.0;
	      *n_intervals = abs(Nint * (3+(int)ceil(R_n/R_50)));
	   } else {
	      *error = TRUE;
		sasfit_err("#find_integration_range: parameter l of Weibull fct. is < 1\n");
	      *Rstart = 0.0;
	      *Rend = 10.0*a4+a3;
	   }
	} else if ( (strcmp(SD_typestr,"Gaussian")          == 0) ||
		    (strcmp(SD_typestr,"d Gaussian / d a1") == 0) ||
		    (strcmp(SD_typestr,"d Gaussian / d a2") == 0) ||
		    (strcmp(SD_typestr,"d Gaussian / d a3") == 0)   ) {
	   R_max = 0.5*a3+0.5*sqrt(a3*a3+24.0*a2*a2);
	   R_n = R_max;
	   n = 1;
	   tmp = 1.0;
	   if (a3 > 0.0)  tmp=R_max;
	   while ( (pow(n*R_n+tmp,moment) * sasfit_sd_GaussDistribution(n*R_n+tmp, &subParam) ) >
		   (pow(R_max,moment)     * n_percent / 100.0
				       * sasfit_sd_GaussDistribution(R_max, &subParam)) ) {
	      n++;
	   }
	   R_n = n*R_n;
	   *Rstart = 0.0;
	   *Rend   = R_n+tmp;
	   if ((R_n+tmp) < R_max)  *Rstart=(R_max-(R_n+tmp));
	   *n_intervals = Nint * n;
	} else if ( (strcmp(SD_typestr,"GEX")          == 0) ||
		    (strcmp(SD_typestr,"d GEX / d a1") == 0) ||
		    (strcmp(SD_typestr,"d GEX / d a2") == 0) ||
		    (strcmp(SD_typestr,"d GEX / d a3") == 0) ||
		    (strcmp(SD_typestr,"d GEX / d a4") == 0) ||
		    (strcmp(SD_typestr,"d GEX / d a5") == 0)   ) {
	   R_max = exp((log(a2)*a3+log((1+a4+moment)/a3))/a3)+a5;
	   R_n = R_max;
	   n = 1;
	   tmp = 1.0;
	   if (R_max > 0.0)  tmp=R_max;
	   if (a2 < 0) {
		   *Rstart = 0;
		   *Rend = a4-a3/a2;
		   if (*Rend < 0) *Rend =1;
		   n = 10;
		   *n_intervals = Nint * n;
	   } else {
			while ( ((pow(n*R_n+tmp,moment) * sasfit_sd_GEX(n*R_n+tmp, &subParam) ) >
					 (pow(R_max,moment)     * n_percent / 100.0
							     * sasfit_sd_GEX(R_max, &subParam)) ) && (n < Nint-1)) {
				  n++;
			}
			R_n = n*R_n;
			*Rstart = 0.0;
			*Rend   = R_n+tmp;
			if ((R_n+tmp) < R_max)  *Rstart=(R_max-(R_n+tmp));
			*n_intervals = Nint * n;
	   }
	   if (n>Nint) {
			sasfit_err("#find_integration_range: to many (%d) integration interval: >%s(%lg,%lg,%lg,%lg)<\n",*n_intervals,SD_typestr,a1,a2,a3,a4);
		    *error = TRUE;
	   }
	} else if ( (strcmp(SD_typestr,"GEV")          == 0) ||
		    (strcmp(SD_typestr,"d GEV / d a1") == 0) ||
		    (strcmp(SD_typestr,"d GEV / d a2") == 0) ||
		    (strcmp(SD_typestr,"d GEV / d a3") == 0) ||
		    (strcmp(SD_typestr,"d GEV / d a4") == 0)   ) {
	//   R_max = a4+(pow(2.0,-a2)-1.)/a2;
	   R_max = a3/(a2*pow(a2+1,a2))-a3/a2+a4;
	//   if (a2<0.) R_max = a4;
	   R_n = fabs(R_max);
	   n = 1;
	   tmp = 1.0;
	   if (R_max > 0.0)  tmp=R_max;
	   while ( ((pow(n*R_n+tmp,moment) * sasfit_sd_GEV(n*R_n+tmp, &subParam) ) >
		    (pow(R_max,moment)     * n_percent / 100.0
					* sasfit_sd_GEV(R_max, &subParam)) ) && (n < Nint-1)) {
	      n++;
	   }
	   R_n = n*R_n;
	   *Rstart = 0.0;
	   *Rend   = R_n+tmp;
	   if ((R_n+tmp) < R_max)  *Rstart=(R_max-(R_n+tmp));
	   *n_intervals = Nint * n;
	   if (n>Nint) {
			sasfit_err("#find_integration_range: to many (%d) integration interval: >%s(%lg,%lg,%lg,%lg)<\n",*n_intervals,SD_typestr,a1,a2,a3,a4);
		    *error = TRUE;
	   }
	} else if ( (strcmp(SD_typestr,"Triangular")          == 0) ||
		    (strcmp(SD_typestr,"d Triangular / d a1") == 0) ||
		    (strcmp(SD_typestr,"d Triangular / d a2") == 0) ||
		    (strcmp(SD_typestr,"d Triangular / d a3") == 0) ||
		    (strcmp(SD_typestr,"d Triangular / d a4") == 0)   ) {
	   *Rstart = a2;
	   *Rend   = a4;
	   *n_intervals = Nint;
	} else if ( (strcmp(SD_typestr,"fractalSD")          == 0) ||
		    (strcmp(SD_typestr,"d fractalSD / d a1") == 0) ||
		    (strcmp(SD_typestr,"d fractalSD / d a2") == 0) ||
		    (strcmp(SD_typestr,"d fractalSD / d a3") == 0) ||
		    (strcmp(SD_typestr,"d fractalSD / d a4") == 0)   ) {
	   *Rstart = a2;
	   *Rend   = a3;
	   *n_intervals = Nint;
	} else if ( (strcmp(SD_typestr,"Uniform")          == 0) ||
		    (strcmp(SD_typestr,"d Uniform / d a1") == 0) ||
		    (strcmp(SD_typestr,"d Uniform / d a2") == 0) ||
		    (strcmp(SD_typestr,"d Uniform / d a3") == 0)    ) {
	   *Rstart = a2;
	   *Rend   = a3;
	   *n_intervals = Nint;
	} else if ( (strcmp(SD_typestr,"Beta")      == 0) ||
		    (strcmp(SD_typestr,"d Beta / d a1") == 0) ||
		    (strcmp(SD_typestr,"d Beta / d a2") == 0) ||
		    (strcmp(SD_typestr,"d Beta / d a3") == 0) ||
		    (strcmp(SD_typestr,"d Beta / d a4") == 0) ||
		    (strcmp(SD_typestr,"d Beta / d a5") == 0)    ) {
	   *Rstart = a2;
	   *Rend   = a3;
	   *n_intervals = Nint;
	} else if ( (strcmp(SD_typestr,"gammaSD")          == 0) ||
		    (strcmp(SD_typestr,"d gammaSD / d a1") == 0) ||
		    (strcmp(SD_typestr,"d gammaSD / d a2") == 0) ||
		    (strcmp(SD_typestr,"d gammaSD / d a3") == 0)   ) {
	//     R_max = theta*(k+5.) ;
	   R_max = a3*a3/a2*(a2/a3*a2/a3-1+moment);
	   R_n = fabs(R_max);
	   n = 1;
	   tmp = 1.0;
	   if (R_max > 0.0)  tmp=R_max;
	   while ( ((pow(n*R_n+tmp,moment) * sasfit_sd_gammaSD(n*R_n+tmp, &subParam) ) >
		 (pow(R_max,moment)     * n_percent / 100.0
					 * sasfit_sd_gammaSD(R_max, &subParam)) ) && (n < Nint-1)) {
				n++;
			}
		R_n = n*R_n;
		*Rstart = 0.0;
		*Rend   = R_n+tmp;
		if ((R_n+tmp) < R_max)  *Rstart=(R_max-(R_n+tmp));
		*n_intervals = Nint * n;
	   if (n>Nint) {
			sasfit_err("#find_integration_range: to many (%d) integration interval: >%s(%lg,%lg,%lg)<\n",*n_intervals,SD_typestr,a1,a2,a3);
		    *error = TRUE;
	   }
	} else {
		if ( Tcl_GetInt(sasfit_env_get_interp(), SD_typestr, &funcid)
			  != TCL_OK )
		{
			*n_intervals = 10*Nint;
			*Rstart = 0.0;
			*Rend = 10.0*a4;
			sasfit_err("#find_integration_range: unknown size distribution: >%s<\n",SD_typestr);
			sasfit_out("find_integration_range: ");
			sasfit_out("unknown size distribution >%s<\n",SD_typestr);
			sasfit_out("if this error occurs inform me please\n");
			*error = TRUE;
		} else {
			func_descr = sasfit_plugin_db_get_by_id(funcid);
			SASFIT_ASSERT_VOID(func_descr);
			if (strcmp(func_descr->name,"sd_fractal_series1")      == 0) {
                *Rstart = a[1];
                *Rend = a[3];
                *n_intervals = Nint;
			} else if (strcmp(func_descr->name,"sd_fractal_series2")      == 0) {
                *Rstart = a[1];
                *Rend = a[5];
                *n_intervals = Nint;
			} else if (strcmp(func_descr->name,"sd_fractal_series3")      == 0) {
                *Rstart = a[1];
                *Rend = a[7];
                *n_intervals = Nint;
			} else if ( (strcmp(func_descr->name,"sd_akima8")      == 0) ) {
				*Rstart = a1;
				*Rend   = a2;
				*n_intervals = Nint;
			} else if ( (strcmp(func_descr->name,"sd_cspline8")      == 0) ) {
				*Rstart = a1;
				*Rend   = a2;
				*n_intervals = Nint;
			} else if ( (strcmp(func_descr->name,"sd_steffen8")      == 0) ) {
				*Rstart = a1;
				*Rend   = a2;
				*n_intervals = Nint;
			} else if ( (strcmp(func_descr->name,"sd_lognorm_fp")       == 0) ) {
			   a4 = fabs(a4);
			   R_0  = a4*exp(-a2*a2*(a3-moment));
			   R_n  = a4*exp(-a2*a2*(a3-moment)+sqrt(2.0*a2*a2*log(100.0/n_percent)));
			   R_50 = a4*exp(-a2*a2*(a3-moment)+sqrt(2.0*a2*a2*log(100.0/50.0)));
			   if (SASFIT_EQUAL(R_50, 0.0)) {
				  sasfit_err("#find_integration_range: can't guess good integration interval: >%s(%lg,%lg,%lg,%lg)<\n",SD_typestr,a1,a2,a3,a4);
			      sasfit_out("find_integration_range: ");
			      sasfit_out("can't guess good integration interval\n");
			      sasfit_out("find_integration_range: >%s(%lg,%lg,%lg,%lg)<\n", SD_typestr,a1,a2,a3,a4);
			      *Rstart=0.0;
			      *Rend = 100.0;
			      *n_intervals = 500;
			      *error = TRUE;
			      return;
			   } else {
			      *n_intervals =  abs((int)ceil(R_n/(R_0-R_50))) * Nint;
			      *Rstart = 0.0;
			      *Rend = R_n;
				  if (*n_intervals > 500) {
					  if ((R_n-R_0) < R_0) {
				      *Rend = R_n;
						  *Rstart = 2.0*R_0-R_n;
						  *n_intervals = Nint;
					  } else {
					sasfit_err("#find_integration_range: too many (%d) integration interval: >%s(%lg,%lg,%lg,%lg)<\n",*n_intervals,SD_typestr,a1,a2,a3,a4);
				    *error = TRUE;
					  }
				  }
			      return;
			   }
			} else {
				*n_intervals = 10*Nint;
				*Rstart = 0.0;
				*Rend = 10.0*a4;
				sasfit_err("#find_integration_range: unknown size distribution: >%s<\n",SD_typestr);
				sasfit_out("find_integration_range: ");
				sasfit_out("unknown size distribution >%s<\n",func_descr->name);
				sasfit_out("if this error occurs inform send a bug report!\n");
				*error = TRUE;
			}
		}
	}
	if ( subParam.errStatus != FALSE )
	{
		sasfit_err("#find_integration_range: >%s<\n",subParam.errStr);
		*error = TRUE;
	}
	return;
}

scalar IQ_IntdLen(scalar x, void *param4int) {

    return IQ_core( ((sasfit_param4int *)param4int)->interp,
                    ((sasfit_param4int *)param4int)->dF_dpar,
                    ((sasfit_param4int *)param4int)->l,
                    ((sasfit_param4int *)param4int)->s,
                    x,
                    ((sasfit_param4int *)param4int)->Q,
                    ((sasfit_param4int *)param4int)->a,
                    ((sasfit_param4int *)param4int)->SD,
                    ((sasfit_param4int *)param4int)->FF,
                    ((sasfit_param4int *)param4int)->SQ,
                    ((sasfit_param4int *)param4int)->distr,
                    ((sasfit_param4int *)param4int)->error);
}
scalar IQ_core(Tcl_Interp *interp,
	      int *dF_dpar,
	      scalar l[],
	      scalar s[],
          scalar len,
          scalar Q,
          scalar a[],
          sasfit_function*  SD,
          sasfit_function*  FF,
	      sasfit_function*  SQ,
          int   FFdistr,
          bool  *error)
/*############################################################################*/
/******************************************************************************
 * calculates the integral core of the scattering intensity for               *
 * particles with shape FF_typestr and a size distribution SD_typestr         *
 * for a given Q-value                                                        *
 ******************************************************************************/
{
   which_len(interp,len,l,FFdistr,error);
   if (*error == TRUE) return 0.0;
   return sasfit_sd(len,a,SD,dF_dpar,error)
          * sasfit_ff(Q,l,FF,dF_dpar,error);
}


scalar IQSQij_core(Tcl_Interp *interp,
		  int *dF_dpar,
		  scalar l[],
		  scalar s[],
          scalar len1,
		  scalar len2,
		  scalar Q,
		  scalar a[],
		  sasfit_function*  SD,
		  sasfit_function*  FF,
		  sasfit_function*  SQ,
		  int   FFdistr,
		  bool  *error)
/*############################################################################*/
/******************************************************************************
 * calculates the integral core of the scattering intensity for               *
 * particles with shape FF_typestr and a size distribution SD_typestr         *
 * for a given Q-value                                                        *
 ******************************************************************************/
{  scalar Fij, Vi, Vj;
   scalar sq[MAXPAR];
   int j;

   for (j=0;j<MAXPAR;j++) sq[j]=s[j];

   which_len(interp,len1,l,FFdistr,error);
   if (*error == TRUE) return 0.0;
   Fij =       sasfit_sd(len1,a,SD,dF_dpar,error)
             * sasfit_ff(Q,l,FF,dF_dpar,error);
   if (*error == TRUE) return 0.0;

   which_len(interp,len2,l,FFdistr,error);
   Fij = Fij * sasfit_sd(len2,a,SD,dF_dpar,error)
             * sasfit_ff(Q,l,FF,dF_dpar,error);
   if (*error == TRUE) return 0.0;

   Vi = sasfit_volume(len1,l,FF,FFdistr,error);
   Vj = sasfit_volume(len2,l,FF,FFdistr,error);

   if (*error) return Fij;

   sq[0] = ( pow( 3./(4.*M_PI) *  Vi, 1./3.) + pow( 3./(4.*M_PI) *  Vj, 1./3.) ) / 2.0;

   which_len(interp,(len1+len2)/2.,sq,0,error);
   Fij = Fij *(sasfit_sq(Q,sq,SQ,dF_dpar,error)-1.0);
   if (*error == TRUE) return 0.0;
   return Fij;
}

double IQSQij_core_cub(const double* v, size_t n, void* p ){
    SQij_param *param4SQij;
    param4SQij = (SQij_param *)p;
    if (n!=2) sasfit_err("wrong dimension of input parameters");
    return IQSQij_core(param4SQij->interp,
		     param4SQij->dF_dpar,
		     param4SQij->l,
		     param4SQij->s,
		     v[0],
		     v[1],
		     param4SQij->Q,
		     param4SQij->a,
		     param4SQij->SD,
		     param4SQij->FF,
		     param4SQij->SQ,
		     param4SQij->distr,
		     param4SQij->error);
}

scalar Kernel_Nth_V_Moment(double *x, size_t dim, void *pam)
{
	sasfit_param * param;
	multint_cub *cub;
	cub = (multint_cub *) pam;
	param = (sasfit_param *) cub->param;
	return (*cub->KernelnD_fct)(x,dim,param);
}


scalar CalcNth_V_Moment(Tcl_Interp *interp,
		       scalar a[],
		       scalar l[],
		       sasfit_function*  SD,
		       sasfit_function*  FF,
		       int	distr,
		       int   whichMoment,
		       bool  *error)
{
    int    i;
    scalar  NR, NRprevious, NRint;
    scalar  Rstart,Rend,dR;
    int    n_intervals;
    scalar  V_moments, Vim1, Vi;
	sasfit_param param;

    if( SD == 0 )
    {
    	sasfit_out("CalcNth_V_Moment(): SD is 0 !\n");
	if ( error != 0 ) *error = TRUE;
	return 0.0;
    }

    Rstart = 0.0;
    Rend   = 0.0;
    dR = 0.0;
    V_moments = 0.0;
    n_intervals = 1;
    NRint = 0.;

    find_integration_range(interp,&Rstart, &Rend, &n_intervals,
                          a,SD->typestr,6,error);
    if ((*error == FALSE) && (n_intervals > 1) ) {
        dR = (Rend - Rstart) / n_intervals;
        V_moments = 0.0;
    } else {
		return V_moments;
	}
    if ( !(    (strncmp(SD->typestr,"Delta",5) == 0)
           || (strncmp(SD->typestr,"d Delta",7) == 0)
 	       || (strncmp(SD->typestr,"Monodisperse",10) == 0)
	      )  ) {
	  NRprevious = 0.0;
	  Vi   = sasfit_volume(Rstart,l,FF,distr,error);
      for(i=0;i<=n_intervals;i++) {
         NR = sasfit_sd(Rstart+dR*i,a,SD,NULL,error);
		 if (i>0) {
			 Vim1 = Vi;
			 Vi   = sasfit_volume(Rstart+dR*i,l,FF,distr,error);
			 if (*error ) return V_moments;
			 V_moments = V_moments + ( NRprevious*pow(Vim1,whichMoment)
					                  +NR        *pow(Vi  ,whichMoment)
								     ) * dR/2.0;
			 NRint = NRint + (NRprevious+NR)*dR/2.;
		 }
		 NRprevious = NR;
         if (*error) {
		sasfit_err("could not properly calculate average particle volume\n");
             return V_moments;
         }
      }
    }
    return V_moments/NRint;
}

scalar IQSQij_SA_core(Tcl_Interp *interp,
		     int   *dF_dpar,
		     scalar l[],
		     scalar s[],
		     scalar len1,
		     scalar len2,
		     scalar Vav,
		     scalar Q,
		     scalar a[],
		     sasfit_function*  SD,
		     sasfit_function*  FF,
		     sasfit_function*  SQ,
		     int   FFdistr,
		     bool  *error)
/*############################################################################*/
/******************************************************************************
 * calculates the integral core of the scattering intensity for               *
 * particles with shape FF_typestr and a size distribution SD_typestr         *
 * for a given Q-value                                                        *
 ******************************************************************************/
{  scalar Fij, Vi, Vj;
   scalar sq[MAXPAR];
   int j;

   for (j=0;j<MAXPAR;j++) sq[j]=s[j];
   which_len(interp,len1,l,FFdistr,error);
   if (*error == TRUE) return 0.0;

   Fij =       sasfit_sd(len1,a,SD,dF_dpar,error)
             * sasfit_ff(Q,l,FF,dF_dpar,error);
   if (*error == TRUE) return 0.0;

   which_len(interp,len2,l,FFdistr,error);
   Fij = Fij * sasfit_sd(len2,a,SD,dF_dpar,error)
             * sasfit_ff(Q,l,FF,dF_dpar,error);
   if (*error == TRUE) return 0.0;

   Vi = sasfit_volume(len1,l,FF,FFdistr,error);
   Vj = sasfit_volume(len2,l,FF,FFdistr,error);

   if (*error) return Fij;

   sq[0] = ( pow( 3./(4.*M_PI) *  Vi, 1./3.) + pow( 3./(4.*M_PI) *  Vj, 1./3.) ) / 2.0;

   Fij = Fij *(sasfit_sq(Q,sq,SQ,dF_dpar,error)-1.0)*(Vi+Vj)/(2.*Vav);
   if (*error == TRUE) return 0.0;
   return Fij;
}

double IQSQijSA_core_cub(const double* v, size_t n, void* p ){
    SQij_param *param4SQij;
    param4SQij = (SQij_param *)p;
    if (n!=2) sasfit_err("wrong dimension of input parameters");
    return IQSQij_SA_core(param4SQij->interp,
		     param4SQij->dF_dpar,
		     param4SQij->l,
		     param4SQij->s,
		     v[0],
		     v[1],
		     param4SQij->Vav,
		     param4SQij->Q,
		     param4SQij->a,
		     param4SQij->SD,
		     param4SQij->FF,
		     param4SQij->SQ,
		     param4SQij->distr,
		     param4SQij->error);
}
scalar integral_IQ_int_core( Tcl_Interp *interp,
			    int dF_dpar[],
			    scalar l[],
			    scalar s[],
                scalar Q,
                scalar a[],
                sasfit_function*  SD,
                sasfit_function*  FF,
			    sasfit_function*  SQ,
                int   distr,
			    int   SQ_how,
				scalar Rstart,
                scalar Rend,
                int   nintervals,
                bool  *error)
/*############################################################################*/
{
    scalar cubmin[2], cubmax[2];
    SQij_param param4SQij;
	scalar res,res2,intErr,restot,Vav, Vx;
	char FF_typestr[132];
	char strtmp[256];

	if( FF == 0 )
	{
		sasfit_err("integral_IQ_int_core(): FF is 0 !\n");
		if( error != 0 ) *error = TRUE;
		return 0.0;
	}
	if( SD == 0 )
	{
		sasfit_err("integral_IQ_int_core(): SD is 0 !\n");
		if( error != 0 ) *error = TRUE;
		return 0.0;
	}
	if( SQ == 0 ) // error ! has to be of type 'None' at least ...
	{
	//	if (SQ->F_identifier != SQ_id_RPA) {
			sasfit_err("integral_IQ_int_core(): SQ is 0 !\n");
			if( error != 0 ) *error = TRUE;
			return 0.0;
	//	}
	}

//strcpy(FF_typestr,FF->typestr);

   if (    (SD->F_identifier == SD_id_Monodisperse)
	   || ((SD->F_identifier == SD_id_Delta) && (dF_dpar[SDfct] == 1)) ) {
/******************************************************************************
 *  no integration over R, if the particle has no size distribution;          *
 *  only the value of the squared form factor is returned                     *
 ******************************************************************************/
      if ((SQ->F_identifier == SQ_id_RPA) && (dF_dpar[SQfct] == 0)) {
		  res2 = sasfit_ff(0,l,FF,dF_dpar,error);
		  res = sasfit_ff(Q,l,FF,dF_dpar,error);
		  if (sasfit_eps_get_sq_or_iq() < 0) {
			  return 1./(1.+fabs(s[0])*res/res2);
		  }
		  return res/(1.+fabs(s[0])*res/res2);
	  } else if ((SQ->F_identifier == SQ_id_RPA) && (dF_dpar[SQfct] == 1)) {
		  res2 = sasfit_ff(0,l,FF,dF_dpar,error);
		  res = sasfit_ff(Q,l,FF,dF_dpar,error);
		  if (sasfit_eps_get_sq_or_iq() < 0) {
			  return -GSL_SIGN(s[0])*pow(1./(1.+fabs(s[0])*res/res2),2)*res/res2;
		  }
		  return -GSL_SIGN(s[0])*pow(res/(1.+fabs(s[0])*res/res2),2.)/res2;
	  }

	  if ((sasfit_eps_get_sq_or_iq() < 0) && (strstr(FF_typestr,"Robertus") == NULL)) {
         return sasfit_sq(Q,s,SQ,dF_dpar,error);
	  } else {
	      return   sasfit_ff(Q,l,FF,dF_dpar,error)
	             * sasfit_sq(Q,s,SQ,dF_dpar,error);
	  }
   } else if ((SD->F_identifier == SD_id_AdivB) && (dF_dpar[SDfct] == 1)) {
      if ((SQ->F_identifier == SQ_id_RPA) && (dF_dpar[SQfct] == 0)) {
		  res2 = sasfit_ff(0,l,FF,dF_dpar,error);
		  res = sasfit_ff(Q,l,FF,dF_dpar,error);
		  if (sasfit_eps_get_sq_or_iq() < 0) {
			  return 1./a[1]/(1.+fabs(s[0])*res/res2);
		  }
		  return res/(1.+fabs(s[0])*res/res2);
	  } else if ((SQ->F_identifier == SQ_id_RPA) && (dF_dpar[SQfct] == 1)) {
		  res2 = sasfit_ff(0,l,FF,dF_dpar,error);
		  res = sasfit_ff(Q,l,FF,dF_dpar,error);
		  if (sasfit_eps_get_sq_or_iq() < 0) {
			  return -1./a[1]*GSL_SIGN(s[0])*pow(1./(1.+fabs(s[0])*res/res2),2)*res/res2;
		  }
		  return -1./a[1]*GSL_SIGN(s[0])*pow(res/(1.+fabs(s[0])*res/res2),2.)/res2;
	  }

	  if ((sasfit_eps_get_sq_or_iq() < 0) && (strstr(FF_typestr,"Robertus") == NULL)) {
         return sasfit_sq(Q,s,SQ,dF_dpar,error);
	  } else {
	      return   1./a[1]*sasfit_ff(Q,l,FF,dF_dpar,error)
	             * sasfit_sq(Q,s,SQ,dF_dpar,error);
	  }
   } else if ((SD->F_identifier == SD_id_Delta) && (dF_dpar[SDfct] == 0)) {
      if ((SQ->F_identifier == SQ_id_RPA) && (dF_dpar[SQfct] == 0)) {
		  res2 = sasfit_ff(0,l,FF,dF_dpar,error);
		  res = sasfit_ff(Q,l,FF,dF_dpar,error);
		  if (sasfit_eps_get_sq_or_iq() < 0) {
			  return a[0] /(1.+fabs(s[0])*res/res2);
		  }
		  return a[0] * res/(1.+fabs(s[0])*res/res2);
	  } else if ((SQ->F_identifier == SQ_id_RPA) && (dF_dpar[SQfct] == 1)) {
		  res2 = sasfit_ff(0,l,FF,dF_dpar,error);
		  res = sasfit_ff(Q,l,FF,dF_dpar,error);
		  if (sasfit_eps_get_sq_or_iq() < 0) {
			  return -a[0]*GSL_SIGN(s[0])*pow(1./(1.+fabs(s[0])*res/res2),2)*res/res2;
		  }
		  return -a[0]*GSL_SIGN(s[0])*pow(res/(1.+fabs(s[0])*res/res2),2.)/res2;
	  }

	  if ((sasfit_eps_get_sq_or_iq() < 0) && (strstr(FF->typestr,"Robertus1") == NULL)) {
         return sasfit_sq(Q,s,SQ,dF_dpar,error);
	  } else {
	      return   a[0] * sasfit_ff(Q,l,FF,dF_dpar,error)
	             * sasfit_sq(Q,s,SQ,dF_dpar,error);
	  }
  } else if ((SD->F_identifier == SD_id_AdivB) && (dF_dpar[SDfct] == 0)) {

      if ((SQ->F_identifier == SQ_id_RPA) && (dF_dpar[SQfct] == 0)) {
		  res2 = sasfit_ff(0,l,FF,dF_dpar,error);
		  res = sasfit_ff(Q,l,FF,dF_dpar,error);
		  if (sasfit_eps_get_sq_or_iq() < 0) {
			  return a[0]/a[1] /(1.+fabs(s[0])*res/res2);
		  }
		  return a[0]/a[1] * res/(1.+fabs(s[0])*res/res2);
	  } else if ((SQ->F_identifier == SQ_id_RPA) && (dF_dpar[SQfct] == 1)) {
		  res2 = sasfit_ff(0,l,FF,dF_dpar,error);
		  res = sasfit_ff(Q,l,FF,dF_dpar,error);
		  if (sasfit_eps_get_sq_or_iq() < 0) {
			  return -a[0]/a[1]*GSL_SIGN(s[0])*pow(1./(1.+fabs(s[0])*res/res2),2)*res/res2;
		  }
		  return -a[0]/a[1]*GSL_SIGN(s[0])*pow(res/(1.+fabs(s[0])*res/res2),2.)/res2;
	  }

	  if ((sasfit_eps_get_sq_or_iq() < 0) && (strstr(FF->typestr,"Robertus1") == NULL)) {
         return sasfit_sq(Q,s,SQ,dF_dpar,error);
	  } else {
	      return   a[0]/a[1] * sasfit_ff(Q,l,FF,dF_dpar,error)
	             * sasfit_sq(Q,s,SQ,dF_dpar,error);
	  }
  } else if ((SD->F_identifier == SD_id_AdivB) && (dF_dpar[SDfct] == 2)) {

      if ((SQ->F_identifier == SQ_id_RPA) && (dF_dpar[SQfct] == 0)) {
		  res2 = sasfit_ff(0,l,FF,dF_dpar,error);
		  res = sasfit_ff(Q,l,FF,dF_dpar,error);
		  if (sasfit_eps_get_sq_or_iq() < 0) {
			  return -a[0]/gsl_pow_2(a[1]) /(1.+fabs(s[0])*res/res2);
		  }
		  return a[0]/a[1] * res/(1.+fabs(s[0])*res/res2);
	  } else if ((SQ->F_identifier == SQ_id_RPA) && (dF_dpar[SQfct] == 1)) {
		  res2 = sasfit_ff(0,l,FF,dF_dpar,error);
		  res = sasfit_ff(Q,l,FF,dF_dpar,error);
		  if (sasfit_eps_get_sq_or_iq() < 0) {
			  return a[0]/gsl_pow_2(a[1])*GSL_SIGN(s[0])*pow(1./(1.+fabs(s[0])*res/res2),2)*res/res2;
		  }
		  return a[0]/gsl_pow_2(a[1])*GSL_SIGN(s[0])*pow(res/(1.+fabs(s[0])*res/res2),2.)/res2;
	  }

	  if ((sasfit_eps_get_sq_or_iq() < 0) && (strstr(FF->typestr,"Robertus1") == NULL)) {
         return sasfit_sq(Q,s,SQ,dF_dpar,error);
	  } else {
	      return   -a[0]/gsl_pow_2(a[1]) * sasfit_ff(Q,l,FF,dF_dpar,error)
	             * sasfit_sq(Q,s,SQ,dF_dpar,error);
	  }
   } else {
/******************************************************************************
 * integration over R, if the particle has a size distribution                *
 ******************************************************************************/
   }


   if ((SQ->F_identifier == SQ_id_RPA) && (dF_dpar[SQfct] == 0)) {
	   res2 = SASFITqrombIQdR(interp,dF_dpar,l,s,0,a,
		                    SD,FF,SQ,
							distr,Rstart,Rend,error);
       res = SASFITqrombIQdR(interp,dF_dpar,l,s,Q,a,
		                    SD,FF,SQ,
							distr,Rstart,Rend,error);
	   if (sasfit_eps_get_sq_or_iq() < 0) {
		   return 1./(1.+fabs(s[0])*res/res2);
	   }
	   return res/(1.+fabs(s[0])*res/res2);
   } else if ((SQ->F_identifier == SQ_id_RPA) && (dF_dpar[SQfct] == 1)){
	   res2 = SASFITqrombIQdR(interp,dF_dpar,l,s,0,a,
		                    SD,FF,SQ,
							distr,Rstart,Rend,error);
       res = SASFITqrombIQdR(interp,dF_dpar,l,s,Q,a,
		                    SD,FF,SQ,
							distr,Rstart,Rend,error);
	   if (sasfit_eps_get_sq_or_iq() < 0) {
		   return -GSL_SIGN(s[0])*pow(1./(1.+fabs(s[0])*res/res2),2.)/res2;
	   }
	   return -GSL_SIGN(s[0])*pow(res/(1.+fabs(s[0])*res/res2),2.)/res2;
   }
   if (*error==TRUE) return 1;

   switch (SQ_how) {
    case 0  : {if (sasfit_eps_get_sq_or_iq() < 0) return sasfit_sq(Q,s,SQ,dF_dpar,error);
		       res = SASFITqrombIQdR(interp,dF_dpar,l,s,Q,a,
		                    SD,FF,SQ,
							distr,Rstart,Rend,error);
			   res = res * sasfit_sq(Q,s,SQ,dF_dpar,error);
	           return res;
			  }
	case 1  : {res = SASFITqrombIQdR(interp,dF_dpar,l,s,Q,a,
		                    SD,FF,SQ,
							distr,Rstart,Rend,error);
			   strcpy(strtmp,"<F> ");
               strcat(strtmp,FF_typestr);
	       FF->compute_f = TRUE;
               res2 = SASFITqrombIQdR(interp,dF_dpar,l,s,Q,a,SD,FF,SQ,distr,Rstart,Rend,error);
	       FF->compute_f = FALSE;
			   if (*error == TRUE) {
			       sasfit_err("For this form factor only the monodisperse or eventually the local monodisperse approach for calculating a structure factor is implemented\n");
			       return res;
			   } else {
				   restot = res + res2*res2/a[0]*(sasfit_sq(Q,s,SQ,dF_dpar,error)-1.0);
			   }
			   if (sasfit_eps_get_sq_or_iq() < 0) {
				   return restot/res;
			   } else {
			       return restot;
			   }
			  }
	case 2  : {
			res = SASFITqrombIQSQdR(interp,dF_dpar,l,s,Q,a,
		                    SD,FF,SQ,
							distr,Rstart,Rend,error);
			   if (*error == TRUE) {
				   sasfit_err("For this form factor only the monodisperse approximation for calculating a structure factor is implemented (1)\n");
				   return res;
			   }
		       if (sasfit_eps_get_sq_or_iq() >= 0) return res;
               res = res/SASFITqrombIQdR(interp,dF_dpar,l,s,Q,a,
		                    SD,FF,SQ,
							distr,Rstart,Rend,error);
			   if (*error == TRUE) {
				   sasfit_err("For this form factor only the monodisperse approximation for calculating a structure factor is implemented (2)\n");
				   return res;
			   }
			   return res;
			  }
	case 3  : {res = SASFITqrombIQdR(interp,dF_dpar,l,s,Q,a,
		                    SD,FF,SQ,
							distr,Rstart,Rend,error);
			   strcpy(strtmp,"<F> ");
               strcat(strtmp,FF_typestr);
	       FF->compute_f = TRUE;
               cubmin[0]=Rstart;
               cubmin[1]=Rstart;
               cubmax[0]=Rend;
               cubmax[1]=Rend;
                param4SQij.interp=interp;
                param4SQij.dF_dpar=dF_dpar;
                param4SQij.l=l;
                param4SQij.s=s;
                param4SQij.Vav= Vav;
                param4SQij.Q=Q;
                param4SQij.a=a;
                param4SQij.SD=SD;
                param4SQij.FF=FF;
                param4SQij.SQ=SQ;
                param4SQij.distr=distr;
                param4SQij.error=error;
                sasfit_cubature(2,cubmin,cubmax,&IQSQij_core_cub,&param4SQij,sasfit_eps_get_nriq(),&res2,&intErr);
//               res2 = SASFITqrombIQSQijdRj(interp,dF_dpar,l,s,Q,a,SD,FF,SQ,distr,Rstart,Rend,error);
	       FF->compute_f = FALSE;
			   if (*error == TRUE) {
			       sasfit_err("For this form factor only the monodisperse or eventually the local monodisperse approach for calculating a structure factor is implemented\n");
			       return res;
			   } else {
				   restot = res + res2/a[0];
			   }
			   if (sasfit_eps_get_sq_or_iq() < 0) return restot/res;
			   return restot;
			  }
	case 4  : {res = SASFITqrombIQdR(interp,dF_dpar,l,s,Q,a,
		                    SD,FF,SQ,
							distr,Rstart,Rend,error);
			   if (*error == TRUE) {
			       sasfit_err("You lost, something went wrong\n");
			       return res;
			   }

			   Vav = CalcNth_V_Moment(interp,a,l,SD,FF,distr,1,error);
			   if (*error == TRUE) {
				   sasfit_err("For this form factor only the monodisperse or eventually the local monodisperse approach for calculating a structure factor is implemented\n");
				   return res;
			   }

			   strcpy(strtmp,"<F> ");
               strcat(strtmp,FF_typestr);

	       FF->compute_f = TRUE;
               cubmin[0]=Rstart;
               cubmin[1]=Rstart;
               cubmax[0]=Rend;
               cubmax[1]=Rend;
                param4SQij.interp=interp;
                param4SQij.dF_dpar=dF_dpar;
                param4SQij.l=l;
                param4SQij.s=s;
                param4SQij.Vav= Vav;
                param4SQij.Q=Q;
                param4SQij.a=a;
                param4SQij.SD=SD;
                param4SQij.FF=FF;
                param4SQij.SQ=SQ;
                param4SQij.distr=distr;
                param4SQij.error=error;
                sasfit_cubature(2,cubmin,cubmax,&IQSQijSA_core_cub,&param4SQij,sasfit_eps_get_nriq(),&res2,&intErr);
              // res2 = SASFITqrombSA_IQSQijdRj(interp,dF_dpar,l,Vav,s,Q,a,SD,FF,SQ,distr,Rstart,Rend,error);
	       FF->compute_f = FALSE;
			   if (*error == TRUE) {
			       sasfit_err("For this form factor only the monodisperse or eventually the local monodisperse approach for calculating a structure factor is implemented\n");
			       return res;
			   } else {
				   restot = res + res2/a[0];
			   }
			   if (sasfit_eps_get_sq_or_iq() < 0) return restot/res;
			   return restot;
			  }
	case 5  : {res = SASFITqrombIQdR(interp,dF_dpar,l,s,Q,a,
		                    SD,FF,SQ,
							distr,Rstart,Rend,error);
			   if (*error == TRUE) {
			       sasfit_err("You lost, something went wrong\n");
			       return res;
			   }

			   Vav = CalcNth_V_Moment(interp,a,l,SD,FF,distr,1,error);
//			   Vx  = Calculate_Vx_Moment(interp,a,l,SD,FF,distr,1,error);
			   if (*error == TRUE) {
				   sasfit_err("For this form factor only the monodisperse or eventually the local monodisperse approach for calculating a structure factor is implemented\n");
				   return res;
			   }

			   strcpy(strtmp,"<F> ");
               strcat(strtmp,FF_typestr);
	       FF->compute_f = TRUE;
                param4SQij.interp=interp;
                param4SQij.dF_dpar=dF_dpar;
                param4SQij.l=l;
                param4SQij.s=s;
                param4SQij.Vav= Vav;
                param4SQij.Q=Q;
                param4SQij.a=a;
                param4SQij.SD=SD;
                param4SQij.FF=FF;
                param4SQij.SQ=SQ;
                param4SQij.distr=distr;
                param4SQij.error=error;
               *error=sasfit_cubature(2,cubmin,cubmax,&IQSQijSA_core_cub,&param4SQij,sasfit_eps_get_nriq(),&res2,&intErr);
//               res2 = SASFITqrombSA_IQSQijdRj(interp,dF_dpar,l,Vav,s,Q,a,SD,FF,SQ,distr,Rstart,Rend,error);
	       FF->compute_f = FALSE;
			   if (*error == TRUE) {
			       sasfit_err("For this form factor only the monodisperse or eventually the local monodisperse approach for calculating a structure factor is implemented\n");
			       return res;
			   } else {
				   restot = res + res2/a[0];
			   }
			   if (sasfit_eps_get_sq_or_iq() < 0) return restot/res;
			   return restot;
			  }
    default : {
	           *error = TRUE;
			   sasfit_err("This way of including a structure factor is \nNOT YET IMPLEMENTED\n");
			   return 1.;
	          }
   }
}

scalar HTIQ_OOURA(scalar Q, void *param4int) {
        Tcl_Interp *interp;
        int *dF_dpar;
        sasfit_function*  SD;
        sasfit_function*  FF;
        sasfit_function*  SQ;
        int   distr;
        int   SQ_how;
		scalar Rstart, Rend;
        int   nintervals;
        bool  *error;
        scalar Icalc, *l, *s, *a, z;


        interp = (( sasfit_param4int *) param4int)->interp;
        dF_dpar = (( sasfit_param4int *) param4int)->dF_dpar;
        z = (( sasfit_param4int *) param4int)->z;
        SD = (( sasfit_param4int *) param4int)->SD;
        FF = (( sasfit_param4int *) param4int)->FF;
        SQ = (( sasfit_param4int *) param4int)->SQ;
        SQ_how = (( sasfit_param4int *) param4int)->SQ_how;
        distr = (( sasfit_param4int *) param4int)->distr;
        Rstart = (( sasfit_param4int *) param4int)->Rstart;
        Rend = ((sasfit_param4int *) param4int)->Rend;
        nintervals = ((sasfit_param4int *) param4int)->nintervals;
        error = (( sasfit_param4int *) param4int)->error;
        l = (( sasfit_param4int *) param4int)->l;
        a = (( sasfit_param4int *) param4int)->a;
        s = (( sasfit_param4int *) param4int)->s;
        Icalc = integral_IQ_int_core(interp,dF_dpar,l,s,Q,a,SD,FF,SQ,distr,SQ_how,Rstart,Rend,nintervals,error);
        if (*error) return 0;
//        *((( sasfit_GzIntStruct *)GIP)->Ifit) = *Ifit;
//        *((( sasfit_GzIntStruct *)GIP)->Isub) = *Isub;
        return (Icalc)*Q*bessj0(Q*z)/(2*M_PI);
}

scalar integral_IQ_incl_Gztransform( Tcl_Interp *interp,
			    int dF_dpar[],
			    scalar l[],
			    scalar s[],
                scalar Q,
                scalar Qres,
                scalar a[],
                sasfit_function*  SD,
                sasfit_function*  FF,
			    sasfit_function*  SQ,
                int   distr,
			    int   SQ_how,
				scalar Rstart,
                scalar Rend,
                int   nintervals,
                bool  *error)
{
    sasfit_param4int param4int;
    scalar *aw, Gz, Xi, err;
    int lenaw;

    if (sasfit_get_iq_or_gz() == 0) {
        return integral_IQ_int_core(interp,dF_dpar,l,s,Q,a,SD,FF,SQ,distr,SQ_how,Rstart,Rend,nintervals,error);
    } else {
        param4int.dF_dpar=dF_dpar;
        param4int.l=l;
        param4int.s=s;
        param4int.Q=Q;
        param4int.a=a;
        param4int.SD=SD;
        param4int.FF=FF;
        param4int.SQ=SQ;
        param4int.Rstart=Rstart;
        param4int.Rend=Rend;
        param4int.nintervals=nintervals;
        param4int.distr=distr;
        param4int.error=error;

        lenaw=4000;

        aw = (scalar *)malloc((lenaw)*sizeof(scalar));
        if ((sasfit_get_iq_or_gz()!=3) && (sasfit_get_iq_or_gz()!=4)) {
            param4int.z = 0;
            if (Qres <=0) {
                sasfit_intdeiini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
                sasfit_intdei(&HTIQ_OOURA, 0.0, aw, &Xi, &err,&param4int);
            } else {
                sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
                sasfit_intde(&HTIQ_OOURA, 0.0, Qres, aw, &Xi, &err,&param4int);
            }
        }
        param4int.z = Q;
        if (Qres <=0) {
            sasfit_intdeoini(lenaw,GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
            sasfit_intdeo(&HTIQ_OOURA, 0.0, Q, aw, &Gz, &err,&param4int);
        } else {
            sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
            sasfit_intde(&HTIQ_OOURA, 0.0, Qres, aw, &Gz, &err,&param4int);
        }

//           sasfit_out("z:%lf, Gz:%lf, exp(Gz-Xi):%lf\n",GIP.z,Gz,*Ifit);
        free(aw) ;
//      Here only (Gz-Xi) is returned as at this stage the contribution of different scattering species is still a linear combination
//      Only at the very end, when all contributions are summed up the exponential is taken. (see IQ() and IQ_Global())
        if ((sasfit_get_iq_or_gz()==3) || (sasfit_get_iq_or_gz()==4)) {
            return Gz;
        } else {
            return (Gz-Xi);
        }
    }
}

void IQ_int_core(Tcl_Interp *interp,
				 scalar *IQ,
                 scalar *dIQ_da,
                 scalar l[],
                 scalar Q,
                 scalar Qres,
                 scalar a[],
				 scalar s[],
                 sasfit_analytpar *AP,
                 bool  *error)
/*############################################################################*/
/******************************************************************************
 * this procedure returns the scattering intensity IQ of particles            *
 * of type FF_typestr and with a size distribution of type SD_typestr   and   *
 * its derivatives d a1, d a2, d a3, d a4, d l1, d l2, d l3, d l4             *
 * for a given Q-value                                                        *
 ******************************************************************************/
{
scalar Rstart, Rend,t_a[MAXPAR],t_l[MAXPAR],tDLS_l[MAXPAR],tDLS_a[MAXPAR], t_s[MAXPAR];
scalar tmp_a[MAXPAR];
int   nintervals,SQdistr,FFdistr,i;
char  strtmp[132];
scalar d_a2_IntNVdR, d_a3_IntNVdR, d_a4_IntNVdR, IntNVdR, tmpIQ,
       d_a5_IntNVdR,d_a6_IntNVdR,
       d_a7_IntNVdR,d_a8_IntNVdR,d_a9_IntNVdR,d_a10_IntNVdR;
char  tmpFF_typestr[132];
int dF_dpar[3] = {0,0,0};

/*
 * variable transformation if fit parameter have limits
 */

   for (i=0;i<MAXPAR;i++) {
      if ((AP->FF_limits[i]) && AP->FF_active[i] && (AP->fit == TRUE) && AP->fitSDFF) {
         t_l[i] = AP->FF_min[i] + 0.5*(AP->FF_max[i] - AP->FF_min[i])
                  * ( sin(l[i]) + 1.0 );
      } else {t_l[i] = l[i]; }
      if ((AP->SD_limits[i]) && AP->SD_active[i] && (AP->fit == TRUE) && AP->fitSDFF) {
         t_a[i] = AP->SD_min[i] + 0.5*(AP->SD_max[i] - AP->SD_min[i])
                  * ( sin(a[i]) + 1.0 );
      } else {t_a[i] = a[i]; }
      if ((AP->SQ_limits[i]) && AP->SQ_active[i] && (AP->fit == TRUE) && AP->fitSDFF) {
         t_s[i] = AP->SQ_min[i] + 0.5*(AP->SQ_max[i] - AP->SQ_min[i])
                  * ( sin(s[i]) + 1.0 );
      } else {t_s[i] = s[i]; }
   }

   find_integration_range(interp,&Rstart,&Rend,&nintervals,
                          t_a,AP->SD.typestr,6,error);
   if (*error == TRUE) {
	   return;
   }

   FFdistr = -1;
   SQdistr = -1;
   for (i=0;i<MAXPAR;i++) {
       if (AP->FF_distr[i] == 1) { FFdistr = i; }
	   if (AP->SQ_distr[i] == 1) { SQdistr = i; }
   }


/******************************************************************************
 * calculate Normalization factor in case of Langevin formfactor              *
 ******************************************************************************/

   IntNVdR = 1.0;
   d_a2_IntNVdR = 0.0;
   d_a3_IntNVdR = 0.0;
   d_a4_IntNVdR = 0.0;
   d_a5_IntNVdR = 0.0;
   d_a6_IntNVdR = 0.0;
   d_a7_IntNVdR = 0.0;
   d_a8_IntNVdR = 0.0;
   d_a9_IntNVdR = 0.0;
   d_a10_IntNVdR = 0.0;

   if (strcmp(AP->FF.typestr,"Langevin")==0) {
      if (strcmp(AP->SD.typestr,"Monodisperse") == 0) {
         if (t_l[0] != 0.0) {
            IntNVdR = 3.0/(4.0*PI*t_l[0]*t_l[0]*t_l[0]);
		 } else {
		    IntNVdR = 1.0;
		 }
         d_a2_IntNVdR = 0.0;
         d_a3_IntNVdR = 0.0;
         d_a4_IntNVdR = 0.0;
         d_a5_IntNVdR = 0.0;
         d_a6_IntNVdR = 0.0;
         d_a7_IntNVdR = 0.0;
         d_a8_IntNVdR = 0.0;
         d_a9_IntNVdR = 0.0;
         d_a10_IntNVdR = 0.0;
	  } else {

         strcpy(tmpFF_typestr,AP->FF.typestr);
	     strcpy(AP->FF.typestr,"tmpLangevin");
		 for (i=0;i<MAXPAR;i++) tmp_a[i] = t_a[i];
         tmp_a[0] = 1.0;
	     IntNVdR=SASFITqrombNR_V_dR(interp,tmp_a,&AP->SD,&AP->FF,Rstart,Rend,error);
		 if ( IntNVdR != 0.0) {
             IntNVdR = 1.0 / IntNVdR;
		 } else {
			 IntNVdR = 0.0;
		 }
         d_a5_IntNVdR = 0.0;
         d_a6_IntNVdR = 0.0;
         d_a7_IntNVdR = 0.0;
         d_a8_IntNVdR = 0.0;
         d_a9_IntNVdR = 0.0;
         d_a10_IntNVdR = 0.0;

		 for (i=0;i<MAXPAR;i++) tmp_a[i] = t_a[i];
         tmp_a[0] = 1.0;
         tmp_a[1] = t_a[1]*(1.0+1.0e-2);
         d_a2_IntNVdR=SASFITqrombNR_V_dR(interp,tmp_a,&AP->SD,&AP->FF,Rstart,Rend,error);
		 for (i=0;i<MAXPAR;i++) tmp_a[i] = t_a[i];
         tmp_a[0] = 1.0;
         tmp_a[2] = t_a[2]*(1.0+1.0e-2);
         d_a3_IntNVdR=SASFITqrombNR_V_dR(interp,tmp_a,&AP->SD,&AP->FF,Rstart,Rend,error);
		 for (i=0;i<MAXPAR;i++) tmp_a[i] = t_a[i];
         tmp_a[0] = 1.0;
         tmp_a[3] = t_a[3]*(1.0+1.0e-2);
         d_a4_IntNVdR=SASFITqrombNR_V_dR(interp,tmp_a,&AP->SD,&AP->FF,Rstart,Rend,error);
		 if ( (d_a2_IntNVdR != 0.0) && (t_a[1] != 0.0)) {
             d_a2_IntNVdR = (1.0/d_a2_IntNVdR - IntNVdR)/(t_a[1]*1.0e-2);
		 } else {
			 d_a2_IntNVdR = 0.0;
		 }
		 if ( (d_a3_IntNVdR != 0.0) && (t_a[2] != 0.0)) {
             d_a3_IntNVdR = (1.0/d_a3_IntNVdR - IntNVdR)/(t_a[2]*1.0e-2);
		 } else {
			 d_a3_IntNVdR = 0.0;
		 }
		 if ( (d_a4_IntNVdR != 0.0) && (t_a[3] != 0.0)) {
             d_a4_IntNVdR = (1.0/d_a4_IntNVdR - IntNVdR)/(t_a[3]*1.0e-2);
		 } else {
			 d_a4_IntNVdR = 0.0;
		 }
	     strcpy(AP->FF.typestr,tmpFF_typestr);
	  }
   }

/******************************************************************************
 * calculate Normalization factor in case of DLS_Sphere_RDG                   *
 ******************************************************************************/
   if (strcmp(AP->FF.typestr,"DLS_Sphere_RDG")==0) {
	  if (strcmp(AP->SD.typestr,"Monodisperse") == 0) {
         if (t_l[0] != 0.0) {
             IntNVdR = 3.0/(4.0*PI*t_l[0]*t_l[0]*t_l[0]);
		 } else {
			 IntNVdR = 1.0;
		 }
         d_a2_IntNVdR = 0.0;
         d_a3_IntNVdR = 0.0;
         d_a4_IntNVdR = 0.0;
         d_a5_IntNVdR = 0.0;
         d_a6_IntNVdR = 0.0;
         d_a7_IntNVdR = 0.0;
         d_a8_IntNVdR = 0.0;
         d_a9_IntNVdR = 0.0;
         d_a10_IntNVdR = 0.0;
	  } else {
         strcpy(strtmp,"Sphere");
		 for (i=0;i<=MAXPAR;i++) {
		     tDLS_l[i] = t_l[i];
		     tDLS_a[i] = t_a[i];
		 }
		 tDLS_l[3] = 1.0;
		 tDLS_a[0] = 1.0;
		 if (FFdistr == 0) {
// This Function call below is wrong
  	        IntNVdR=integral_IQ_incl_Gztransform(interp,dF_dpar,tDLS_l,t_s,t_l[3],Qres,tDLS_a,
                                     &AP->SD, &AP->FF, &AP->SQ, FFdistr,
                                     AP->SQ_how,Rstart,Rend,nintervals,error);
		 } else {IntNVdR = 1.0;}
		 if ( IntNVdR != 0.0) {
            IntNVdR = 1.0 / IntNVdR;
		 } else {
		    IntNVdR = 1.0;
		 }
	  }
   }

/******************************************************************************
 * calculate IQ                                                               *
 ******************************************************************************/
   dF_dpar[SDfct] = 0;
   dF_dpar[FFfct] = 0;
   dF_dpar[SQfct] = 0;

   tmpIQ = integral_IQ_incl_Gztransform(interp,dF_dpar,t_l,t_s,Q,Qres, t_a,
                             &AP->SD, &AP->FF, &AP->SQ, FFdistr,
                             AP->SQ_how,Rstart,Rend,nintervals,error);
   *IQ = tmpIQ * IntNVdR;
   if (*error == TRUE) {
	   return;
   }


/******************************************************************************
 * calculate dIQ/da[i]                                                        *
 ******************************************************************************/
   for (i=0;i<MAXPAR;i++) {
		if (AP->SD_active[i] == TRUE)  {
			dF_dpar[SDfct] = i+1;
			dF_dpar[FFfct] = 0;
			dF_dpar[SQfct] = 0;
//	    strcpy(strtmp,"d ");
//      strcat(strtmp,AP.SD.typestr);
//      strcat(strtmp," / d a1");
//   sasfit_out("sasfit.c: 1532, \nSD: %s\nFF: %s\nSQ: %s\n", AP.SD.typestr, AP.FF.typestr, AP.SQ.typestr);
			dIQ_da[i] = integral_IQ_incl_Gztransform(interp,dF_dpar,t_l,t_s,Q,Qres,t_a,
                                       &AP->SD, &AP->FF, &AP->SQ, FFdistr,
                                       AP->SQ_how,Rstart,Rend,nintervals,error)
				  * IntNVdR;
			if (*error == TRUE) {
				return;
			}
			if ((AP->SD_limits[i]) && AP->SD_active[i] && (AP->fit == TRUE) && AP->fitSDFF) {
				dIQ_da[i] = dIQ_da[i] * 0.5*(AP->SD_max[i]-AP->SD_min[i])*cos(a[i]);
			}
		} else dIQ_da[i] = 0.0;
   }


/******************************************************************************
 * calculate dIQ/dl[i]                                                        *
 ******************************************************************************/
   for (i=0;i<MAXPAR;i++) {
		if (AP->FF_active[i] == TRUE) {
			dF_dpar[SDfct] = 0;
			dF_dpar[FFfct] = i+1;
			dF_dpar[SQfct] = 0;
//			sprintf(strtmp,"d %s / d l%d",AP->FF.typestr,i);
//			strcat(strtmp,AP.FF_typestr);
//			strcat(strtmp," / d l0");
//   sasfit_out("sasfit.c: 1558, \nSD: %s\nFF: %s\nSQ: %s\n", AP.SD.typestr, AP.FF.typestr, AP.SQ.typestr);
			dIQ_da[MAXPAR+i] = integral_IQ_incl_Gztransform(interp,dF_dpar,t_l, t_s, Q,Qres,t_a,
                                       &AP->SD,&AP->FF,&AP->SQ, FFdistr,
                                       AP->SQ_how,Rstart,Rend,nintervals,error)
								* IntNVdR;
			if (*error == TRUE) {
				return;
			}
			if ((AP->FF_limits[i]) && AP->FF_active[i] && (AP->fit == TRUE) && AP->fitSDFF) {
				dIQ_da[MAXPAR+i] = dIQ_da[MAXPAR+i] * 0.5*(AP->FF_max[i]-AP->FF_min[i])*cos(l[i]);
			}
		} else dIQ_da[MAXPAR+i] = 0.0;
   }


/******************************************************************************
 * calculate dIQ/ds[i]                                                        *
 ******************************************************************************/
   for (i=0;i<MAXPAR;i++) {
		if (AP->SQ_active[i] == TRUE)  {
			dF_dpar[SDfct] = 0;
			dF_dpar[FFfct] = 0;
			dF_dpar[SQfct] = i+1;
//			strcpy(strtmp,"d ");
//			strcat(strtmp,AP.SQ_typestr);
//			strcat(strtmp," / d s0");
//   sasfit_out("sasfit.c: 1584, \nSD: %s\nFF: %s\nSQ: %s\n", AP.SD.typestr, AP.FF.typestr, AP.SQ.typestr);
			dIQ_da[2*MAXPAR+i] = integral_IQ_incl_Gztransform(interp,dF_dpar,t_l,t_s,Q,Qres,t_a,
                                       &AP->SD, &AP->FF, &AP->SQ, FFdistr,
                                       AP->SQ_how,Rstart,Rend,nintervals,error)
							  * IntNVdR;
			if (*error == TRUE) {
				return;
			}
			if ((AP->SQ_limits[i]) && AP->SQ_active[i] && (AP->fit == TRUE) && AP->fitSDFF) {
				dIQ_da[2*MAXPAR+i] = dIQ_da[2*MAXPAR+i] * 0.5*(AP->SQ_max[i]-AP->SQ_min[i])*cos(s[i]);
			}
		} else dIQ_da[2*MAXPAR+i] = 0.0;
   }

}

void IQ_t(Tcl_Interp *interp,
		  scalar Q,
          scalar Qres,
          scalar *par,
          scalar *Ifit,
		  scalar *Isubstract,
          scalar *dydpar,
          int   max_SD,
          sasfit_analytpar *AP,
          int   error_type,
          bool  *error)
/*############################################################################*/
/******************************************************************************
 *                                                                            *
 ******************************************************************************/
{
int   i,j;
scalar TmpIfit,a[MAXPAR],l[MAXPAR],s[MAXPAR];
   *Ifit = 0.0;
   *Isubstract = 0.0;

   for (i=0; i< max_SD;i++) {
	   if (AP[i].calcSDFF) {
		   for (j=0;j<MAXPAR;j++) {
                           a[j] = par[i*(3*MAXPAR) + 0*MAXPAR + j];
			   l[j] = par[i*(3*MAXPAR) + 1*MAXPAR + j];
			   s[j] = par[i*(3*MAXPAR) + 2*MAXPAR + j];
		   }
	      switch(error_type){
            case 0 :
/******************************************************************************
 *  in this case the function f(x) = IQ(Q) will be returned,                  *
 *  for calculating chi^2 in the following way:                               *
 *  chi^2 = sum(( y_i^2 - f(x_i)^2 ) / Delta_y_i^2,i=1,n_points)              *
 ******************************************************************************/
/******************************************************************************
 * fitting function: int(size_distrib(par,R)*F^2(Q,R,l[]), R=0..infinity)     *
 *                   or just F^2(Q,R,l[])                                     *
 ******************************************************************************/
                     IQ_int_core(interp,&TmpIfit,&dydpar[i*(3*MAXPAR)],l,Q,Qres,a,s,&AP[i], error);
   				     if (*error == TRUE) return;
					 if (AP[i].substrSDFF && sasfit_eps_get_fitorsim()) {
						*Isubstract += TmpIfit;
					 }
                     *Ifit       += TmpIfit;
                     break;
            case 1 :
/******************************************************************************
 *  in this case the function f(x) = IQ(Q) will be returned,                  *
 *  to calculate chi^2 in the following way:                                  *
 *  chi^2 = sum(ln^2(y_i) - ln^2(f(x_i)),i=1,n_points)                        *
 ******************************************************************************/
                     IQ_int_core(interp,&TmpIfit,&dydpar[i*(3*MAXPAR)],l,Q,Qres,a,
                                 s,&AP[i],error);
				     if (*error == TRUE) return;
                     *Ifit = log(exp(*Ifit) + TmpIfit);
					 if (AP[i].substrSDFF && sasfit_eps_get_fitorsim()) {
	                     *Isubstract = log(exp(*Isubstract) + TmpIfit);
					 }
                     break;
            case 2 :
/******************************************************************************
 *  in this case the function f(x) = IQ(Q) will be returned,                  *
 *  to calculate chi^2 in the following way:                                  *
 *  chi^2 = sum(abs(y_i) - abs(f(x_i)),i=1,n_points)                          *
 ******************************************************************************/
                     IQ_int_core(interp,&TmpIfit,&dydpar[i*(3*MAXPAR)],l,Q,Qres,a,
                                 s,&AP[i],error);
				     if (*error == TRUE) return;
                     *Ifit = sqrt( (*Ifit)*(*Ifit) + TmpIfit);
					 if (AP[i].substrSDFF && sasfit_eps_get_fitorsim()) {
	                     *Isubstract = sqrt( (*Isubstract)*(*Isubstract) + TmpIfit);
					 }
                     break;
            case 3 :
/******************************************************************************
 *  in this case the function f(x) = IQ(Q) will be returned,                  *
 *  for calculating chi^2 in the following way:                               *
 *  chi^2 = sum(( y_i^2 - f(x_i)^2 ) / 1,i=1,n_points)                        *
 ******************************************************************************/
/******************************************************************************
 * fitting function: int(size_distrib(par,R)*F^2(Q,R,l[]), R=0..infinity)     *
 *                   or just F^2(Q,R,l[])                                     *
 ******************************************************************************/
                     IQ_int_core(interp,&TmpIfit,&dydpar[i*(3*MAXPAR)],l,Q,Qres,a,
                                 s,&AP[i], error);
   				     if (*error == TRUE) return;
					 if (AP[i].substrSDFF && sasfit_eps_get_fitorsim()) {
	                     *Isubstract += TmpIfit;
					 }
                     *Ifit += TmpIfit;
                     break;
            default: sasfit_out("IQ: unknown error type -- 1,error_type = %d\n", error_type);
                     break;
		  }
	   }
   }
   for (i=0;i<max_SD;i++) {
	   if (AP[i].calcSDFF) {
          switch(error_type) {
             case 0 :  break;
             case 1 :  { for (j=0;j<(3*MAXPAR);j++) dydpar[i*(3*MAXPAR)+j]=dydpar[i*(3*MAXPAR)+j]/exp(*Ifit);
                         break;
					   }
             case 2 :  { for (j=0;j<(3*MAXPAR);j++) dydpar[i*(3*MAXPAR)+j]=0.5*dydpar[i*(3*MAXPAR)+j]/(*Ifit);
                         break;
					   }
             case 3 :  break;
             default : {
			sasfit_out("IQ: unknown error type \n");
			sasfit_out("IQ: inform me when this error occurs - 2\n");
			break;
					   }
		  }
	   }
   }
}


scalar HTIQGlobal_OOURA(scalar Q, void *GIP) {
        Tcl_Interp *interp;
	    scalar z;
        scalar *par;
        scalar *Ifit;
		scalar *Isub;
        scalar *dydpar;
        scalar Qres;
        int   max_SD;
        sasfit_analytpar *GAP;
        sasfit_commonpar *GCP;
        int   error_type;
        bool  *error;

        interp = (( sasfit_GzIntStruct *) GIP)->interp;
        z = (( sasfit_GzIntStruct *) GIP)->z;
        par = (( sasfit_GzIntStruct *) GIP)->par;
        Ifit = (( sasfit_GzIntStruct *) GIP)->Ifit;
        Isub = (( sasfit_GzIntStruct *) GIP)->Isub;
        dydpar = (( sasfit_GzIntStruct *) GIP)->dydpar;
        max_SD = (( sasfit_GzIntStruct *) GIP)->max_SD;
        GAP = (( sasfit_GzIntStruct *) GIP)->GAP;
        GCP = (( sasfit_GzIntStruct *) GIP)->GCP;
        error_type = ((sasfit_GzIntStruct *) GIP)->error_type;
        error = (( sasfit_GzIntStruct *) GIP)->error;
        Qres = (( sasfit_GzIntStruct *) GIP)->Qres;
//        IQ_t(interp,Q,Qres,par,Ifit,Isub,dydpar,max_SD,GAP,error_type,error);
//        IQ_t_global_calc(interp,Q,Qres,par,Ifit,Isub,dydpar,max_SD,GAP,GCP,error_type,error);
        IQ_t_global(interp,Q,Qres,par,Ifit,Isub,dydpar,max_SD,GAP,GCP,error_type,error);
        if (*error) return 0;
//        *((( sasfit_GzIntStruct *)GIP)->Ifit) = *Ifit;
//        *((( sasfit_GzIntStruct *)GIP)->Isub) = *Isub;
        return (*Ifit)*Q*bessj0(Q*z)/(2*M_PI);
}

void IQ(Tcl_Interp *interp,
	    scalar Q,
	  	scalar Qres,
        scalar *par,
        scalar *Ifit,
		scalar *Isub,
        scalar *dydpar,
        int   max_SD,
        sasfit_analytpar *AP,
        int   error_type,
		int   Nthdataset,
        bool  *error)
{
	int i,kk, DLS;
    sasfit_analytpar *tmpAP;
	sasfit_analytpar *APdummy;
	scalar Qmin,Qmax, Xi, Xisub;
    gsl_integration_workspace * w;
    gsl_integration_workspace * wc;
    gsl_integration_qawo_table * wo;
    int status;
    int lenaw;
    scalar *aw;
    scalar Gz, err;
    scalar res0to8, res8toInftyCoSine, res8toInftySine, abserr;
    gsl_function intF;
    sasfit_GzIntStruct GIP;


    *Ifit = 0.0;
	*Isub = 0.0;
    DLS = 0;

	tmpAP = (sasfit_analytpar *) Tcl_Alloc(sizeof(sasfit_analytpar)*max_SD);
	APdummy = (sasfit_analytpar *) Tcl_Alloc(sizeof(sasfit_analytpar));

	APdummy[0].calcSDFF = FALSE;
	for (i=0;i<max_SD;i++) {
	    if ( strcmp(AP[i].FF.typestr,"DLS_sphere_RDG") == 0 ) {
			DLS++;
		}
	    if (((AP[i].Nth_dataset == Nthdataset) && (Nthdataset != 0)) || (Nthdataset == 0)) {
			tmpAP[i]=AP[i];
		} else {
			tmpAP[i] = APdummy[0];
			tmpAP[i].calcSDFF = FALSE;
		}

	}

    if (sasfit_get_iq_or_gz() == 0) {
        if (Qres <= 0.0) {
            IQ_t(interp,Q,Qres,par,Ifit,Isub,dydpar,max_SD,tmpAP,error_type,error);
        } else {
            if (Q-3.0*Qres<=0.0) {
                Qmin = 1.0e-3*Q;
            } else {
                Qmin = Q-3.0*Qres;
            }
            Qmax = Q+3*Qres;
            SASFITqrombIQ(interp,Qmin,Qmax,Q,Qres,par,Ifit,Isub,dydpar,max_SD,tmpAP,error_type,error);
        }
        if (DLS==max_SD) *Ifit = (*Ifit)*(*Ifit);
    } else {
        IQ_t(interp,Q,Qres, par,Ifit,Isub,dydpar,max_SD,tmpAP,error_type,error);
// In case of calculating the SESANS signal up to now only (Gz-Xi) has been calculated.
// Now the still missing exponential of it is taken, as well as for the derivatives of the fitting parameters.
        if (sasfit_get_iq_or_gz() == 1) {
            *Ifit = exp((*Ifit));
            *Isub = exp((*Isub));
            for (i=0;i<max_SD*(3*MAXPAR);i++) dydpar[i] = dydpar[i] * (*Ifit);
        }
    }

	Tcl_Free((char *)tmpAP);
	Tcl_Free((char *)APdummy);
    return;
}



void IQ_int_core_global(interp,IQ,l,Q,Qres,a,s,AP,error)
scalar *l, *a,*s,*IQ,Q,Qres;
sasfit_analytpar *AP;
Tcl_Interp *interp;
bool *error;
/*############################################################################*/
/******************************************************************************
 * this procedure returns the scattering intensity IQ of particles            *
 * of type FF_typestr and with a size distribution of type SD_typestr   and   *
 * its derivatives d a1, d a2, d a3, d a4, d l1, d l2, d l3, d l4             *
 * for a given Q-value                                                        *
 ******************************************************************************/
{
scalar Rstart, Rend,tmp_a[MAXPAR],t_a[MAXPAR],t_l[MAXPAR], t_s[MAXPAR];
int   nintervals,distr,i;
scalar d_a2_IntNVdR, d_a3_IntNVdR, d_a4_IntNVdR, IntNVdR, tmpIQ;
char  tmpFF_typestr[132];
int dF_dpar[3];
/*
 * variable transformation if fit parameter have limits
 */

   for (i=0;i<MAXPAR;i++) {
      if ((AP->FF_limits[i]) && AP->FF_active[i] && (AP->fit == TRUE)  && AP->fitSDFF) {
         t_l[i] = (*AP).FF_min[i] + 0.5*((*AP).FF_max[i] - (*AP).FF_min[i])
                  * ( sin(l[i]) + 1.0 );
      } else {t_l[i] = l[i]; }
      if (((*AP).SD_limits[i]) && (*AP).SD_active[i] && ((*AP).fit == TRUE) && (*AP).fitSDFF) {
         t_a[i] = (*AP).SD_min[i] + 0.5*((*AP).SD_max[i] - (*AP).SD_min[i])
                  * ( sin(a[i]) + 1.0 );
      } else {t_a[i] = a[i]; }
      if (((*AP).SQ_limits[i]) && (*AP).SQ_active[i] && ((*AP).fit == TRUE) && (*AP).fitSDFF) {
         t_s[i] = (*AP).SQ_min[i] + 0.5*((*AP).SQ_max[i] - (*AP).SQ_min[i])
                  * ( sin(s[i]) + 1.0 );
      } else {t_s[i] = s[i]; }
   }

   find_integration_range(interp,&Rstart,&Rend,&nintervals,
                          t_a,AP->SD.typestr,6,error);
   if (*error == TRUE) {
	   return;
   }
   distr = -1;
   for (i=0;i<MAXPAR;i++) {
       if (AP->FF_distr[i] == 1) { distr = i; }
   }

/******************************************************************************
 * calculate Normalization factor in case of Langevin formfactor              *
 ******************************************************************************/
     if (strcmp(AP->FF.typestr,"Langevin")==0) {
	   if (strcmp(AP->SD.typestr,"Monodisperse") == 0) {
          if (t_l[0] != 0.0) {
             IntNVdR = 3.0/(4.0*PI*t_l[0]*t_l[0]*t_l[0]);
		  } else {
			 IntNVdR = 0.0;
		  }
          d_a2_IntNVdR = 0.0;
          d_a3_IntNVdR = 0.0;
          d_a4_IntNVdR = 0.0;
	   } else {

          strcpy(tmpFF_typestr,AP->FF.typestr);
	      strcpy(AP->FF.typestr,"tmpLangevin");
  		  for (i=0;i<MAXPAR;i++) tmp_a[i] = t_a[i];
          tmp_a[0] = 1.0;
	      IntNVdR=SASFITqrombNR_V_dR(interp,tmp_a,&AP->SD,&AP->FF,Rstart,Rend,error);


		  if ( IntNVdR != 0.0) {
             IntNVdR = 1.0 / IntNVdR;
		  } else {
			 IntNVdR = 0.0;
		  }
  		  for (i=0;i<MAXPAR;i++) tmp_a[i] = t_a[i];
          tmp_a[0] = 1.0;
          tmp_a[1] = t_a[1]*(1.0+1.0e-2);
          d_a2_IntNVdR=SASFITqrombNR_V_dR(interp,tmp_a,&AP->SD,&AP->FF,Rstart,Rend,error);
  		  for (i=0;i<MAXPAR;i++) tmp_a[i] = t_a[i];
          tmp_a[0] = 1.0;
          tmp_a[2] = t_a[2]*(1.0+1.0e-2);
          d_a3_IntNVdR=SASFITqrombNR_V_dR(interp,tmp_a,&AP->SD,&AP->FF,Rstart,Rend,error);
  		  for (i=0;i<MAXPAR;i++) tmp_a[i] = t_a[i];
          tmp_a[0] = 1.0;
          tmp_a[3] = t_a[3]*(1.0+1.0e-2);
          d_a4_IntNVdR=SASFITqrombNR_V_dR(interp,tmp_a,&AP->SD,&AP->FF,Rstart,Rend,error);
		  if ( (d_a2_IntNVdR != 0.0) && (t_a[1] != 0.0)) {
             d_a2_IntNVdR = (1.0/d_a2_IntNVdR - IntNVdR)/(t_a[1]*1.0e-2);
		  } else {
			 d_a2_IntNVdR = 0.0;
		  }
		  if ( (d_a3_IntNVdR != 0.0) && (t_a[2] != 0.0)) {
             d_a3_IntNVdR = (1.0/d_a3_IntNVdR - IntNVdR)/(t_a[2]*1.0e-2);
		  } else {
			 d_a3_IntNVdR = 0.0;
		  }
		  if ( (d_a4_IntNVdR != 0.0) && (t_a[3] != 0.0)) {
             d_a4_IntNVdR = (1.0/d_a4_IntNVdR - IntNVdR)/(t_a[3]*1.0e-2);
		  } else {
			 d_a4_IntNVdR = 0.0;
		  }
	      strcpy(AP->FF.typestr,tmpFF_typestr);
	   }
   } else {
	   IntNVdR = 1.0;
       d_a2_IntNVdR = 0.0;
       d_a3_IntNVdR = 0.0;
       d_a4_IntNVdR = 0.0;
   }


/******************************************************************************
 * calculate IQ                                                               *
 ******************************************************************************/
   dF_dpar[SDfct] = 0;
   dF_dpar[FFfct] = 0;
   dF_dpar[SQfct] = 0;
   tmpIQ = integral_IQ_incl_Gztransform(interp,dF_dpar,t_l,t_s,Q, Qres, t_a,
                             &AP->SD, &AP->FF, &AP->SQ, distr,
                             AP->SQ_how,Rstart,Rend,nintervals,error);
   *IQ = tmpIQ * IntNVdR;
   if (*error == TRUE) {
	   return;
   }

}
void IQ_t_global_init(Tcl_Interp *interp,
		  scalar Q,
          scalar *par,
          scalar *Ifit,
		  scalar *Isub,
          scalar *dydpar,
          int   max_SD,
          sasfit_analytpar *GAP,
		  sasfit_commonpar *GCP,
          int   error_type,
          bool  *error)
/*############################################################################*/
/******************************************************************************
 *                                                                            *
 ******************************************************************************/
{
int   i,j,k;
bool  active;
   for (k=1; k <= (*GCP).common_i;k++) {
	   active = FALSE;
       for (i=0; i< max_SD;i++) {
           for (j=0;j<MAXPAR;j++) {
			   if ((GAP[i].FF_index[j] == k)  && GAP[i].FF_active[j] && GAP[i].fit) {
				   active = TRUE;
			   }
			   if ((GAP[i].SD_index[j] == k)  && GAP[i].SD_active[j] && GAP[i].fit) {
				   active = TRUE;
			   }
			   if ((GAP[i].SQ_index[j] == k)  && GAP[i].SQ_active[j] && GAP[i].fit) {
				   active = TRUE;
			   }
		   }
	   }
       for (i=0;i<max_SD;i++) {
           for (j=0;j<MAXPAR;j++) {
			   if (GAP[i].FF_index[j] == k) {
				     GAP[i].FF_active[j] = active;
			   }
			   if (GAP[i].SD_index[j] == k) {
				     GAP[i].SD_active[j] = active;
			   }
			   if (GAP[i].SQ_index[j] == k) {
				     GAP[i].SQ_active[j] = active;
			   }
		   }
	   }
   }

   *Ifit = 0.0;
   *Isub = 0.0;
}

void IQ_t_global_calc(Tcl_Interp *interp,
		  scalar Q,
          scalar Qres,
          scalar *par,
          scalar *Ifit,
		  scalar *Isub,
          scalar *dydpar,
          int   max_SD,
          sasfit_analytpar *GAP,
		  sasfit_commonpar *GCP,
          int   error_type,
          bool  *error)
/*############################################################################*/
/******************************************************************************
 *                                                                            *
 ******************************************************************************/
{
int   i,j,k;
scalar TmpIfit,Tmp2Ifit,Tmp3Ifit,a[MAXPAR],l[MAXPAR],s[MAXPAR];
scalar dx = 0.0; // was uninitialized, see line 1737
scalar h;
bool  dy,dy2,active;

   h = sasfit_eps_get_h();

   Tmp3Ifit = 0.0;
   Tmp2Ifit = 0.0;
   TmpIfit = 0.0;

   for (i=0; i<(3*MAXPAR)*max_SD;i++)  dydpar[i] = 0.0;
/************************************************************************
       Calculates Intensity
 ************************************************************************/

   for (i=0;i<max_SD;i++) {
	   if (GAP[i].calcSDFF) {
          for (j=0;j<MAXPAR;j++) {
	          if (GAP[i].SD_index[j] > 0) {
                 a[j] = GAP[i].SD_factor[j]*(*GCP).P_common[GAP[i].SD_index[j]-1];
			  } else {
		         a[j] = par[i*(3*MAXPAR)+j+0];
			  }
	          if (GAP[i].FF_index[j] > 0) {
                 l[j] = GAP[i].FF_factor[j]*(*GCP).P_common[GAP[i].FF_index[j]-1];
			  } else {
		         l[j] = par[i*(3*MAXPAR)+j+MAXPAR];
			  }
	          if (GAP[i].SQ_index[j] > 0) {
                 s[j] = GAP[i].SQ_factor[j]*(*GCP).P_common[GAP[i].SQ_index[j]-1];
			  } else {
		         s[j] = par[i*(3*MAXPAR)+j+(2*MAXPAR)];
			  }
		  }
	   }
	   if (GAP[i].calcSDFF) {
		  IQ_int_core_global(interp,&TmpIfit,l,Q,Qres,a,s,&GAP[i],error);;
		  if (*error == TRUE) return;
		  if (GAP[i].substrSDFF && sasfit_eps_get_fitorsim()) {
			  *Isub = (*Isub) + TmpIfit;
		  }
          *Ifit = (*Ifit) + TmpIfit;
	   }
   }

/************************************************************************
       Calculates derivatives dI / da(i) of local parameters a(i)
 ************************************************************************/

   for (k=0; k < max_SD*(3*MAXPAR);k++) {
	   Tmp2Ifit=0.0;
       Tmp3Ifit=0.0;
       for (i=0; i< max_SD;i++) {
           dy = FALSE;
   	       if (GAP[i].calcSDFF) {
              for (j=0; j<MAXPAR;j++) {
	              if (GAP[i].SD_index[j] > 0) {
                     a[j] = GAP[i].SD_factor[j]*(*GCP).P_common[GAP[i].SD_index[j]-1];
				  } else {
		             a[j] = par[i*(3*MAXPAR)+j+(0*MAXPAR)];
				  }
			      if ((i*(3*MAXPAR)+j+(0*MAXPAR)) == k)  {
				      if (GAP[i].SD_active[j] && (GAP[i].SD_index[j] == 0) && GAP[i].fit) {
					     dx = h*a[j];
				         a[j] = a[j]*(1.0+h);
					     dy = TRUE;
					  }
				  }
	              if (GAP[i].FF_index[j] > 0) {
                     l[j] = GAP[i].FF_factor[j]*(*GCP).P_common[GAP[i].FF_index[j]-1];
				  } else {
		             l[j] = par[i*(3*MAXPAR)+j+(1*MAXPAR)];
				  }
			      if ((i*(3*MAXPAR)+j+(1*MAXPAR)) == k)  {
				     if (GAP[i].FF_active[j] && (GAP[i].FF_index[j] == 0) && GAP[i].fit) {
					    dx = h*l[j];
				        l[j] = l[j]*(1.0+h);
					    dy = TRUE;
					 }
				  }
	              if (GAP[i].SQ_index[j] > 0) {
                     s[j] = GAP[i].SQ_factor[j]*(*GCP).P_common[GAP[i].SQ_index[j]-1];
				  } else {
		             s[j] = par[i*(3*MAXPAR)+j+(2*MAXPAR)];
				  }
			      if ((i*(3*MAXPAR)+j+(2*MAXPAR)) == k)  {
				     if (GAP[i].SQ_active[j] && (GAP[i].SQ_index[j] == 0) && GAP[i].fit) {
					    dx = h*s[j];
				        s[j] = s[j]*(1.0+h);
					    dy = TRUE;
					 }
				  }
			  }
		   }
	       if (GAP[i].calcSDFF && dy) {
			  TmpIfit = 0.0;
		      IQ_int_core_global(interp,&TmpIfit,l,Q,Qres,a,s,&GAP[i],error);
		      if (*error == TRUE) return;
              Tmp2Ifit += TmpIfit;
		   }
	   }
	   for (i=0; i< max_SD;i++) {
           dy = FALSE;
   	       if (GAP[i].calcSDFF) {
              for (j=0; j<MAXPAR;j++) {
	              if (GAP[i].SD_index[j] > 0) {
                     a[j] = GAP[i].SD_factor[j]*(*GCP).P_common[GAP[i].SD_index[j]-1];
				  } else {
		             a[j] = par[i*(3*MAXPAR)+j+(0*MAXPAR)];
				  }
			      if ((i*(3*MAXPAR)+j+(0*MAXPAR)) == k)  {
				      if (GAP[i].SD_active[j] && (GAP[i].SD_index[j] == 0) && GAP[i].fit) {
					     dx = h*a[j];
				         a[j] = a[j]*(1.0-h);
					     dy = TRUE;
					  }
				  }
	              if (GAP[i].FF_index[j] > 0) {
                     l[j] = GAP[i].FF_factor[j]*(*GCP).P_common[GAP[i].FF_index[j]-1];
				  } else {
		             l[j] = par[i*(3*MAXPAR)+j+(1*MAXPAR)];
				  }
			      if ((i*(3*MAXPAR)+j+(1*MAXPAR)) == k)  {
				     if (GAP[i].FF_active[j] && (GAP[i].FF_index[j] == 0) && GAP[i].fit) {
					    dx = h*l[j];
				        l[j] = l[j]*(1.0-h);
					    dy = TRUE;
					 }
				  }
	              if (GAP[i].SQ_index[j] > 0) {
                     s[j] = GAP[i].SQ_factor[j]*(*GCP).P_common[GAP[i].SQ_index[j]-1];
				  } else {
		             s[j] = par[i*(3*MAXPAR)+j+(2*MAXPAR)];
				  }
			      if ((i*(3*MAXPAR)+j+(2*MAXPAR)) == k)  {
				     if (GAP[i].SQ_active[j] && (GAP[i].SQ_index[j] == 0) && GAP[i].fit) {
					    dx = h*s[j];
				        s[j] = s[j]*(1.0-h);
					    dy = TRUE;
					 }
				  }
			  }
		   }
	       if (GAP[i].calcSDFF && dy) {
			  TmpIfit = 0.0;
		      IQ_int_core_global(interp,&TmpIfit,l,Q,Qres,a,s,&GAP[i],error);
		      if (*error == TRUE) return;
              Tmp3Ifit += TmpIfit;
		   }
	   }
	   if ((dx != 0.0)) dydpar[k] = 0.0*dydpar[k] + (Tmp2Ifit - Tmp3Ifit)/(2.0*dx);
   }


/************************************************************************
 Calculates derivative dI / dP_common(i) of global parameters P_common(i)
 ************************************************************************/
   for (k=1; k <= (*GCP).common_i;k++) {
	   Tmp2Ifit=0.0;
	   Tmp3Ifit=0.0;
	   dy = FALSE;

/* Check if global parameter k has to be fitted */

       for (i=0;i<max_SD;i++) {
  	       if (GAP[i].calcSDFF) {
              for (j=0;j<MAXPAR;j++) {
                  if ((GAP[i].SD_index[j] == k) && GAP[i].SD_active[j] && GAP[i].fit){
				     dy = TRUE;
				  }
                  if ((GAP[i].FF_index[j] == k) && GAP[i].FF_active[j] && GAP[i].fit){
				     dy = TRUE;
				  }
                  if ((GAP[i].SQ_index[j] == k) && GAP[i].SQ_active[j] && GAP[i].fit){
				     dy = TRUE;
				  }
			  }
		   }
	   }

       dx = h*(*GCP).P_common[k-1];
       if (dy) {
		   for (i=0;i<max_SD;i++) {
              for (j=0;j<MAXPAR;j++) {
                  if (GAP[i].SD_index[j] == k){
				     GAP[i].SD_active[j] = TRUE;
				  }
                  if (GAP[i].FF_index[j] == k){
				     GAP[i].FF_active[j] = TRUE;
				  }
                  if (GAP[i].SQ_index[j] == k){
				     GAP[i].SQ_active[j] = TRUE;
				  }
			  }
		   }

/* set all a[i], l[i], and s[i], which are global paramter k to (1+h)*a[i] ....     */
/* this is needed to calculated dI / d P_common[k]                                  */
          for (i=0; i< max_SD;i++) {
			  dy2 = FALSE;
  	          if (GAP[i].calcSDFF) {
                 for (j=0;j<MAXPAR;j++) {
	                 if (GAP[i].SD_index[j] > 0) {
                        a[j] = GAP[i].SD_factor[j]*(*GCP).P_common[GAP[i].SD_index[j]-1];
				        if (GAP[i].SD_index[j] == k) {
					       a[j]=a[j]*(1.0+h);
						   dy2 = TRUE;
						}
					 } else {
		                a[j] = par[i*(3*MAXPAR)+j+0];
					 }
	                 if (GAP[i].FF_index[j] > 0) {
                        l[j] = GAP[i].FF_factor[j]*(*GCP).P_common[GAP[i].FF_index[j]-1];
				        if (GAP[i].FF_index[j] == k) {
					       l[j]=l[j]*(1.0+h);
						   dy2 = TRUE;
						}
					 } else {
		                l[j] = par[i*(3*MAXPAR)+j+MAXPAR];
					 }
	                 if (GAP[i].SQ_index[j] > 0) {
                        s[j] = GAP[i].SQ_factor[j]*(*GCP).P_common[GAP[i].SQ_index[j]-1];
				        if (GAP[i].SQ_index[j] == k) {
					       s[j]=s[j]*(1.0+h);
						   dy2 = TRUE;
						}
					 } else {
		                s[j] = par[i*(3*MAXPAR)+j+(2*MAXPAR)];
					 }
				 }
			  }
	          if (GAP[i].calcSDFF && dy2) {
		         IQ_int_core_global(interp,&TmpIfit,l,Q,Qres,a,s,&GAP[i], error);
		         if (*error == TRUE) return;
                 Tmp2Ifit += TmpIfit;
			  }
		  }
/* set all a[i], l[i], and s[i], which are global paramter k to (1-h)*a[i] ....     */
/* this is needed to calculated dI / d P_common[k]                                  */
          for (i=0; i< max_SD;i++) {
			  dy2 = FALSE;
  	          if (GAP[i].calcSDFF) {
                 for (j=0;j<MAXPAR;j++) {
	                 if (GAP[i].SD_index[j] > 0) {
                        a[j] = GAP[i].SD_factor[j]*(*GCP).P_common[GAP[i].SD_index[j]-1];
				        if (GAP[i].SD_index[j] == k) {
					       a[j]=a[j]*(1.0-h);
						   dy2 = TRUE;
						}
					 } else {
		                a[j] = par[i*(3*MAXPAR)+j+0];
					 }
	                 if (GAP[i].FF_index[j] > 0) {
                        l[j] = GAP[i].FF_factor[j]*(*GCP).P_common[GAP[i].FF_index[j]-1];
				        if (GAP[i].FF_index[j] == k) {
					       l[j]=l[j]*(1.0-h);
						   dy2 = TRUE;
						}
					 } else {
		                l[j] = par[i*(3*MAXPAR)+j+MAXPAR];
					 }
	                 if (GAP[i].SQ_index[j] > 0) {
                        s[j] = GAP[i].SQ_factor[j]*(*GCP).P_common[GAP[i].SQ_index[j]-1];
				        if (GAP[i].SQ_index[j] == k) {
					       s[j]=s[j]*(1.0-h);
						   dy2 = TRUE;
						}
					 } else {
		                s[j] = par[i*(3*MAXPAR)+j+(2*MAXPAR)];
					 }
				 }
			  }
	          if (GAP[i].calcSDFF && dy2) {
		         IQ_int_core_global(interp,&TmpIfit,l,Q,Qres,a,s,&GAP[i], error);
		         if (*error == TRUE) return;
                 Tmp3Ifit += TmpIfit;
			  }
		  }
//
// Now calculate the derivative
//
	      for (i=0;i<max_SD;i++) {
              for (j=0;j<MAXPAR;j++) {
                  if (GAP[i].calcSDFF && (dx !=0.0) && (GAP[i].SD_index[j] == k)) {
				     dydpar[i*(3*MAXPAR)+j+0*MAXPAR] += (Tmp2Ifit - Tmp3Ifit)/(2.0*dx);
				  }
                  if (GAP[i].calcSDFF && (dx !=0.0) && (GAP[i].FF_index[j] == k)) {
				     dydpar[i*(3*MAXPAR)+j+1*MAXPAR] += (Tmp2Ifit - Tmp3Ifit)/(2.0*dx);
				  }
                  if (GAP[i].calcSDFF && (dx !=0.0) && (GAP[i].SQ_index[j] == k)) {
				     dydpar[i*(3*MAXPAR)+j+2*MAXPAR] += (Tmp2Ifit - Tmp3Ifit)/(2.0*dx);
				  }
			  }
		  }

	      for (i=0;i<max_SD;i++) {
              for (j=0;j<MAXPAR;j++) {
                  if ((dx !=0.0) && (GAP[i].SD_index[j] == k)) {
				     dydpar[i*(3*MAXPAR)+j+0*MAXPAR] += (Tmp2Ifit - Tmp3Ifit)/(2.0*dx);
				  }
                  if ((dx !=0.0) && (GAP[i].FF_index[j] == k)) {
				     dydpar[i*(3*MAXPAR)+j+1*MAXPAR] += (Tmp2Ifit - Tmp3Ifit)/(2.0*dx);
				  }
                  if ((dx !=0.0) && (GAP[i].SQ_index[j] == k)) {
				     dydpar[i*(3*MAXPAR)+j+2*MAXPAR] += (Tmp2Ifit - Tmp3Ifit)/(2.0*dx);
				  }
			  }
		  }


	   }
   }
}


void IQ_t_global(Tcl_Interp *interp,
		  scalar Q,
          scalar Qres,
          scalar *par,
          scalar *Ifit,
		  scalar *Isub,
          scalar *dydpar,
          int   max_SD,
          sasfit_analytpar *GAP,
		  sasfit_commonpar *GCP,
          int   error_type,
          bool  *error)
/*############################################################################*/
/******************************************************************************
 *                                                                            *
 ******************************************************************************/
{
/*
    IQ_t_global_init(interp,Q,par,
          Ifit,Isub,dydpar,max_SD,
          GAP,GCP,error_type,error);
    IQ_t_global_calc(interp,Q,Qres,par,
          Ifit,Isub,dydpar,max_SD,
          GAP,GCP,error_type,error);
*/
int   i,j,k;
scalar TmpIfit,Tmp2Ifit,Tmp3Ifit,a[MAXPAR],l[MAXPAR],s[MAXPAR];
scalar dx = 0.0; // was uninitialized, see line 1737
scalar h;
bool  dy,dy2,active;

   for (k=1; k <= (*GCP).common_i;k++) {
	   active = FALSE;
       for (i=0; i< max_SD;i++) {
           for (j=0;j<MAXPAR;j++) {
			   if ((GAP[i].FF_index[j] == k)  && GAP[i].FF_active[j] && GAP[i].fit) {
				   active = TRUE;
			   }
			   if ((GAP[i].SD_index[j] == k)  && GAP[i].SD_active[j] && GAP[i].fit) {
				   active = TRUE;
			   }
			   if ((GAP[i].SQ_index[j] == k)  && GAP[i].SQ_active[j] && GAP[i].fit) {
				   active = TRUE;
			   }
		   }
	   }
       for (i=0;i<max_SD;i++) {
           for (j=0;j<MAXPAR;j++) {
			   if (GAP[i].FF_index[j] == k) {
				     GAP[i].FF_active[j] = active;
			   }
			   if (GAP[i].SD_index[j] == k) {
				     GAP[i].SD_active[j] = active;
			   }
			   if (GAP[i].SQ_index[j] == k) {
				     GAP[i].SQ_active[j] = active;
			   }
		   }
	   }
   }

   *Ifit = 0.0;
   *Isub = 0.0;

   h = sasfit_eps_get_h();

   Tmp3Ifit = 0.0;
   Tmp2Ifit = 0.0;
   TmpIfit = 0.0;

   for (i=0; i<(3*MAXPAR)*max_SD;i++)  dydpar[i] = 0.0;
/************************************************************************
       Calculates Intensity
 ************************************************************************/

   for (i=0;i<max_SD;i++) {
	   if (GAP[i].calcSDFF) {
          for (j=0;j<MAXPAR;j++) {
	          if (GAP[i].SD_index[j] > 0) {
                 a[j] = GAP[i].SD_factor[j]*(*GCP).P_common[GAP[i].SD_index[j]-1];
			  } else {
		         a[j] = par[i*(3*MAXPAR)+j+0];
			  }
	          if (GAP[i].FF_index[j] > 0) {
                 l[j] = GAP[i].FF_factor[j]*(*GCP).P_common[GAP[i].FF_index[j]-1];
			  } else {
		         l[j] = par[i*(3*MAXPAR)+j+MAXPAR];
			  }
	          if (GAP[i].SQ_index[j] > 0) {
                 s[j] = GAP[i].SQ_factor[j]*(*GCP).P_common[GAP[i].SQ_index[j]-1];
			  } else {
		         s[j] = par[i*(3*MAXPAR)+j+(2*MAXPAR)];
			  }
		  }
	   }
	   if (GAP[i].calcSDFF) {
		  IQ_int_core_global(interp,&TmpIfit,l,Q,Qres,a,s,&GAP[i],error);;
		  if (*error == TRUE) return;
		  if (GAP[i].substrSDFF && sasfit_eps_get_fitorsim()) {
			  *Isub = (*Isub) + TmpIfit;
		  }
          *Ifit = (*Ifit) + TmpIfit;
	   }
   }

/************************************************************************
       Calculates derivatives dI / da(i) of local parameters a(i)
 ************************************************************************/

   for (k=0; k < max_SD*(3*MAXPAR);k++) {
	   Tmp2Ifit=0.0;
       Tmp3Ifit=0.0;
       for (i=0; i< max_SD;i++) {
           dy = FALSE;
   	       if (GAP[i].calcSDFF) {
              for (j=0; j<MAXPAR;j++) {
	              if (GAP[i].SD_index[j] > 0) {
                     a[j] = GAP[i].SD_factor[j]*(*GCP).P_common[GAP[i].SD_index[j]-1];
				  } else {
		             a[j] = par[i*(3*MAXPAR)+j+(0*MAXPAR)];
				  }
			      if ((i*(3*MAXPAR)+j+(0*MAXPAR)) == k)  {
				      if (GAP[i].SD_active[j] && (GAP[i].SD_index[j] == 0) && GAP[i].fit) {
					     dx = h*a[j];
				         a[j] = a[j]*(1.0+h);
					     dy = TRUE;
					  }
				  }
	              if (GAP[i].FF_index[j] > 0) {
                     l[j] = GAP[i].FF_factor[j]*(*GCP).P_common[GAP[i].FF_index[j]-1];
				  } else {
		             l[j] = par[i*(3*MAXPAR)+j+(1*MAXPAR)];
				  }
			      if ((i*(3*MAXPAR)+j+(1*MAXPAR)) == k)  {
				     if (GAP[i].FF_active[j] && (GAP[i].FF_index[j] == 0) && GAP[i].fit) {
					    dx = h*l[j];
				        l[j] = l[j]*(1.0+h);
					    dy = TRUE;
					 }
				  }
	              if (GAP[i].SQ_index[j] > 0) {
                     s[j] = GAP[i].SQ_factor[j]*(*GCP).P_common[GAP[i].SQ_index[j]-1];
				  } else {
		             s[j] = par[i*(3*MAXPAR)+j+(2*MAXPAR)];
				  }
			      if ((i*(3*MAXPAR)+j+(2*MAXPAR)) == k)  {
				     if (GAP[i].SQ_active[j] && (GAP[i].SQ_index[j] == 0) && GAP[i].fit) {
					    dx = h*s[j];
				        s[j] = s[j]*(1.0+h);
					    dy = TRUE;
					 }
				  }
			  }
		   }
	       if (GAP[i].calcSDFF && dy) {
			  TmpIfit = 0.0;
		      IQ_int_core_global(interp,&TmpIfit,l,Q,Qres,a,s,&GAP[i],error);
		      if (*error == TRUE) return;
              Tmp2Ifit += TmpIfit;
		   }
	   }
	   for (i=0; i< max_SD;i++) {
           dy = FALSE;
   	       if (GAP[i].calcSDFF) {
              for (j=0; j<MAXPAR;j++) {
	              if (GAP[i].SD_index[j] > 0) {
                     a[j] = GAP[i].SD_factor[j]*(*GCP).P_common[GAP[i].SD_index[j]-1];
				  } else {
		             a[j] = par[i*(3*MAXPAR)+j+(0*MAXPAR)];
				  }
			      if ((i*(3*MAXPAR)+j+(0*MAXPAR)) == k)  {
				      if (GAP[i].SD_active[j] && (GAP[i].SD_index[j] == 0) && GAP[i].fit) {
					     dx = h*a[j];
				         a[j] = a[j]*(1.0-h);
					     dy = TRUE;
					  }
				  }
	              if (GAP[i].FF_index[j] > 0) {
                     l[j] = GAP[i].FF_factor[j]*(*GCP).P_common[GAP[i].FF_index[j]-1];
				  } else {
		             l[j] = par[i*(3*MAXPAR)+j+(1*MAXPAR)];
				  }
			      if ((i*(3*MAXPAR)+j+(1*MAXPAR)) == k)  {
				     if (GAP[i].FF_active[j] && (GAP[i].FF_index[j] == 0) && GAP[i].fit) {
					    dx = h*l[j];
				        l[j] = l[j]*(1.0-h);
					    dy = TRUE;
					 }
				  }
	              if (GAP[i].SQ_index[j] > 0) {
                     s[j] = GAP[i].SQ_factor[j]*(*GCP).P_common[GAP[i].SQ_index[j]-1];
				  } else {
		             s[j] = par[i*(3*MAXPAR)+j+(2*MAXPAR)];
				  }
			      if ((i*(3*MAXPAR)+j+(2*MAXPAR)) == k)  {
				     if (GAP[i].SQ_active[j] && (GAP[i].SQ_index[j] == 0) && GAP[i].fit) {
					    dx = h*s[j];
				        s[j] = s[j]*(1.0-h);
					    dy = TRUE;
					 }
				  }
			  }
		   }
	       if (GAP[i].calcSDFF && dy) {
			  TmpIfit = 0.0;
		      IQ_int_core_global(interp,&TmpIfit,l,Q,Qres,a,s,&GAP[i],error);
		      if (*error == TRUE) return;
              Tmp3Ifit += TmpIfit;
		   }
	   }
	   if ((dx != 0.0)) dydpar[k] = 0.0*dydpar[k] + (Tmp2Ifit - Tmp3Ifit)/(2.0*dx);
   }


/************************************************************************
 Calculates derivative dI / dP_common(i) of global parameters P_common(i)
 ************************************************************************/
   for (k=1; k <= (*GCP).common_i;k++) {
	   Tmp2Ifit=0.0;
	   Tmp3Ifit=0.0;
	   dy = FALSE;

/* Check if global parameter k has to be fitted */

       for (i=0;i<max_SD;i++) {
  	       if (GAP[i].calcSDFF) {
              for (j=0;j<MAXPAR;j++) {
                  if ((GAP[i].SD_index[j] == k) && GAP[i].SD_active[j] && GAP[i].fit){
				     dy = TRUE;
				  }
                  if ((GAP[i].FF_index[j] == k) && GAP[i].FF_active[j] && GAP[i].fit){
				     dy = TRUE;
				  }
                  if ((GAP[i].SQ_index[j] == k) && GAP[i].SQ_active[j] && GAP[i].fit){
				     dy = TRUE;
				  }
			  }
		   }
	   }

       dx = h*(*GCP).P_common[k-1];
       if (dy) {
		   for (i=0;i<max_SD;i++) {
              for (j=0;j<MAXPAR;j++) {
                  if (GAP[i].SD_index[j] == k){
				     GAP[i].SD_active[j] = TRUE;
				  }
                  if (GAP[i].FF_index[j] == k){
				     GAP[i].FF_active[j] = TRUE;
				  }
                  if (GAP[i].SQ_index[j] == k){
				     GAP[i].SQ_active[j] = TRUE;
				  }
			  }
		   }

/* set all a[i], l[i], and s[i], which are global paramter k to (1+h)*a[i] ....     */
/* this is needed to calculated dI / d P_common[k]                                  */
          for (i=0; i< max_SD;i++) {
			  dy2 = FALSE;
  	          if (GAP[i].calcSDFF) {
                 for (j=0;j<MAXPAR;j++) {
	                 if (GAP[i].SD_index[j] > 0) {
                        a[j] = GAP[i].SD_factor[j]*(*GCP).P_common[GAP[i].SD_index[j]-1];
				        if (GAP[i].SD_index[j] == k) {
					       a[j]=a[j]*(1.0+h);
						   dy2 = TRUE;
						}
					 } else {
		                a[j] = par[i*(3*MAXPAR)+j+0];
					 }
	                 if (GAP[i].FF_index[j] > 0) {
                        l[j] = GAP[i].FF_factor[j]*(*GCP).P_common[GAP[i].FF_index[j]-1];
				        if (GAP[i].FF_index[j] == k) {
					       l[j]=l[j]*(1.0+h);
						   dy2 = TRUE;
						}
					 } else {
		                l[j] = par[i*(3*MAXPAR)+j+MAXPAR];
					 }
	                 if (GAP[i].SQ_index[j] > 0) {
                        s[j] = GAP[i].SQ_factor[j]*(*GCP).P_common[GAP[i].SQ_index[j]-1];
				        if (GAP[i].SQ_index[j] == k) {
					       s[j]=s[j]*(1.0+h);
						   dy2 = TRUE;
						}
					 } else {
		                s[j] = par[i*(3*MAXPAR)+j+(2*MAXPAR)];
					 }
				 }
			  }
	          if (GAP[i].calcSDFF && dy2) {
		         IQ_int_core_global(interp,&TmpIfit,l,Q,Qres,a,s,&GAP[i], error);
		         if (*error == TRUE) return;
                 Tmp2Ifit += TmpIfit;
			  }
		  }
/* set all a[i], l[i], and s[i], which are global paramter k to (1-h)*a[i] ....     */
/* this is needed to calculated dI / d P_common[k]                                  */
          for (i=0; i< max_SD;i++) {
			  dy2 = FALSE;
  	          if (GAP[i].calcSDFF) {
                 for (j=0;j<MAXPAR;j++) {
	                 if (GAP[i].SD_index[j] > 0) {
                        a[j] = GAP[i].SD_factor[j]*(*GCP).P_common[GAP[i].SD_index[j]-1];
				        if (GAP[i].SD_index[j] == k) {
					       a[j]=a[j]*(1.0-h);
						   dy2 = TRUE;
						}
					 } else {
		                a[j] = par[i*(3*MAXPAR)+j+0];
					 }
	                 if (GAP[i].FF_index[j] > 0) {
                        l[j] = GAP[i].FF_factor[j]*(*GCP).P_common[GAP[i].FF_index[j]-1];
				        if (GAP[i].FF_index[j] == k) {
					       l[j]=l[j]*(1.0-h);
						   dy2 = TRUE;
						}
					 } else {
		                l[j] = par[i*(3*MAXPAR)+j+MAXPAR];
					 }
	                 if (GAP[i].SQ_index[j] > 0) {
                        s[j] = GAP[i].SQ_factor[j]*(*GCP).P_common[GAP[i].SQ_index[j]-1];
				        if (GAP[i].SQ_index[j] == k) {
					       s[j]=s[j]*(1.0-h);
						   dy2 = TRUE;
						}
					 } else {
		                s[j] = par[i*(3*MAXPAR)+j+(2*MAXPAR)];
					 }
				 }
			  }
	          if (GAP[i].calcSDFF && dy2) {
		         IQ_int_core_global(interp,&TmpIfit,l,Q,Qres,a,s,&GAP[i], error);
		         if (*error == TRUE) return;
                 Tmp3Ifit += TmpIfit;
			  }
		  }
//
// Now calculate the derivative
//
	      for (i=0;i<max_SD;i++) {
              for (j=0;j<MAXPAR;j++) {
                  if (GAP[i].calcSDFF && (dx !=0.0) && (GAP[i].SD_index[j] == k)) {
				     dydpar[i*(3*MAXPAR)+j+0*MAXPAR] += (Tmp2Ifit - Tmp3Ifit)/(2.0*dx);
				  }
                  if (GAP[i].calcSDFF && (dx !=0.0) && (GAP[i].FF_index[j] == k)) {
				     dydpar[i*(3*MAXPAR)+j+1*MAXPAR] += (Tmp2Ifit - Tmp3Ifit)/(2.0*dx);
				  }
                  if (GAP[i].calcSDFF && (dx !=0.0) && (GAP[i].SQ_index[j] == k)) {
				     dydpar[i*(3*MAXPAR)+j+2*MAXPAR] += (Tmp2Ifit - Tmp3Ifit)/(2.0*dx);
				  }
			  }
		  }

	      for (i=0;i<max_SD;i++) {
              for (j=0;j<MAXPAR;j++) {
                  if ((dx !=0.0) && (GAP[i].SD_index[j] == k)) {
				     dydpar[i*(3*MAXPAR)+j+0*MAXPAR] += (Tmp2Ifit - Tmp3Ifit)/(2.0*dx);
				  }
                  if ((dx !=0.0) && (GAP[i].FF_index[j] == k)) {
				     dydpar[i*(3*MAXPAR)+j+1*MAXPAR] += (Tmp2Ifit - Tmp3Ifit)/(2.0*dx);
				  }
                  if ((dx !=0.0) && (GAP[i].SQ_index[j] == k)) {
				     dydpar[i*(3*MAXPAR)+j+2*MAXPAR] += (Tmp2Ifit - Tmp3Ifit)/(2.0*dx);
				  }
			  }
		  }


	   }
   }
}

void copyGAP(sasfit_analytpar *fromGAP,sasfit_analytpar *toGAP)
{
	int i;
   (*toGAP)=(*fromGAP);
   return;
   (*toGAP).fit=(*fromGAP).fit;
   (*toGAP).calcSDFF=(*fromGAP).calcSDFF;
   (*toGAP).fitSDFF=(*fromGAP).fitSDFF;
   (*toGAP).substrSDFF=(*fromGAP).substrSDFF;
   for (i=0;i<MAXPAR;i++) {
	   (*toGAP).FF_l[i]      =(*fromGAP).FF_l[i];
	   (*toGAP).FF_err[i]    =(*fromGAP).FF_err[i];
	   (*toGAP).FF_min[i]    =(*fromGAP).FF_min[i];
	   (*toGAP).FF_max[i]    =(*fromGAP).FF_max[i];
	   (*toGAP).FF_limits[i] =(*fromGAP).FF_limits[i];
	   (*toGAP).FF_active[i] =(*fromGAP).FF_active[i];
	   (*toGAP).FF_distr[i]  =(*fromGAP).FF_distr[i];
	   (*toGAP).FF_factor[i] =(*fromGAP).FF_factor[i];
	   (*toGAP).FF_index[i]  =(*fromGAP).FF_index[i];
	   strcpy((*toGAP).FF_common[i],(*fromGAP).FF_common[i]);
   }
   (*toGAP).FF=(*fromGAP).FF;

   for (i=0;i<MAXPAR;i++) {
	   (*toGAP).SD_a[i]      =(*fromGAP).SD_a[i];
	   (*toGAP).SD_err[i]    =(*fromGAP).SD_err[i];
	   (*toGAP).SD_min[i]    =(*fromGAP).SD_min[i];
	   (*toGAP).SD_max[i]    =(*fromGAP).SD_max[i];
	   (*toGAP).SD_limits[i] =(*fromGAP).SD_limits[i];
	   (*toGAP).SD_active[i] =(*fromGAP).SD_active[i];
	   (*toGAP).SD_factor[i] =(*fromGAP).SD_factor[i];
	   (*toGAP).SD_index[i]  =(*fromGAP).SD_index[i];
	   strcpy((*toGAP).SD_common[i],(*fromGAP).SD_common[i]);
   }
   (*toGAP).SD=(*fromGAP).SD;

   for (i=0;i<MAXPAR;i++) {
	   (*toGAP).SQ_s[i]      =(*fromGAP).SQ_s[i];
	   (*toGAP).SQ_err[i]    =(*fromGAP).SQ_err[i];
	   (*toGAP).SQ_min[i]    =(*fromGAP).SQ_min[i];
	   (*toGAP).SQ_max[i]    =(*fromGAP).SQ_max[i];
	   (*toGAP).SQ_limits[i] =(*fromGAP).SQ_limits[i];
	   (*toGAP).SQ_active[i] =(*fromGAP).SQ_active[i];
	   (*toGAP).SQ_factor[i] =(*fromGAP).SQ_factor[i];
	   (*toGAP).SQ_index[i]  =(*fromGAP).SQ_index[i];
	   strcpy((*toGAP).SQ_common[i],(*fromGAP).SQ_common[i]);
   }
   (*toGAP).SQ=(*fromGAP).SQ;

   strcpy((*toGAP).Nth_datalabel,(*fromGAP).Nth_datalabel);
   (*toGAP).Nth_dataset=(*fromGAP).Nth_dataset;
   for (i=0;i<MAXPAR;i++) {
	   (*toGAP).R_moments[i]=(*fromGAP).R_moments[i];
   }
   (*toGAP).fp=(*fromGAP).fp;
   (*toGAP).SQ_how=(*fromGAP).SQ_how;
   return;
}

/*
 * scalar test(scalar x,scalar* a, Tcl_Interp *interp) {
    return exp(-x*x);
}

void int_test(void) {
    scalar a;
    a=2;
    sasfit_integrate(0,GSL_POSINF,&test,&a);
}
*/

void IQ_Global(Tcl_Interp *interp,
	    scalar Q,
	  	scalar Qres,
        scalar *par,
        scalar *Ifit,
		scalar *Isub,
        scalar *dydpar,
        int   max_SD,
        sasfit_analytpar *GAP,
		sasfit_commonpar *GCP,
        int   error_type,
		int   Nthdataset,
        bool  *error)
{
	int i,j,kk;
    sasfit_analytpar *tmpGAP;
	sasfit_analytpar *GAPdummy;
	scalar Qmin,Qmax;

    *Ifit = 0.0;
	*Isub = 0.0;


	tmpGAP = (sasfit_analytpar *) Tcl_Alloc(sizeof(sasfit_analytpar)*max_SD);
	GAPdummy = (sasfit_analytpar *) Tcl_Alloc(sizeof(sasfit_analytpar));

	GAPdummy[0].calcSDFF = FALSE;
	for (i=0;i<max_SD;i++) {
	    if (((GAP[i].Nth_dataset == Nthdataset) && (Nthdataset != 0)) || (Nthdataset == 0)) {
			copyGAP(&GAP[i],&tmpGAP[i]);
		} else {
			copyGAP(&GAP[i],&tmpGAP[i]);
			tmpGAP[i].calcSDFF = FALSE;
		}
		if (GAP[i].dodydpar == 0) {
			for (j=0;j<MAXPAR;j++) {
				tmpGAP[i].SD_active[j] = 0;
				tmpGAP[i].FF_active[j] = 0;
				tmpGAP[i].SQ_active[j] = 0;
			}
		}
	}

    if (sasfit_get_iq_or_gz() == 0) {
        if (Qres <= 0.0) {
            IQ_t_global(interp,Q,Qres,par,Ifit,Isub,dydpar,max_SD,tmpGAP,GCP,error_type,error);
        } else {
            if (Q-3.0*Qres<=0.0) {
                Qmin = 1.0e-3*Q;
            } else {
                Qmin = Q-3.0*Qres;
            }
            Qmax = Q+3*Qres;
            SASFITqrombIQglobal(interp,Qmin,Qmax,Q,Qres,par,Ifit,Isub,dydpar,max_SD,tmpGAP,GCP,error_type,error);
        }
    } else {
        IQ_t_global(interp,Q,Qres,par,Ifit,Isub,dydpar,max_SD,tmpGAP,GCP,error_type,error);
// In case of calculating the SESANS signal up to now only (Gz-Xi) has been calculated.
// Now the still missing exponential of it is taken, as well as for the derivatives of the fitting parameters.
        if (sasfit_get_iq_or_gz()==1) {
            *Ifit = exp((*Ifit));
            *Isub = exp((*Isub));
            for (i=0;i<max_SD*(3*MAXPAR);i++) dydpar[i] = dydpar[i] * (*Ifit);
        }
    }

    Tcl_Free((char *)tmpGAP);
    Tcl_Free((char *)GAPdummy);
    return;
}


void IQ_t_gsl_global(Tcl_Interp *interp,
		  scalar Q,
          scalar Qres,
          scalar *par,
          scalar *Ifit,
          int   max_SD,
          sasfit_analytpar *GAP,
		  sasfit_commonpar *GCP,
          int   error_type,
          bool  *error)
/*############################################################################*/
/******************************************************************************
 *                                                                            *
 ******************************************************************************/
{
int   i,j,k;
scalar TmpIfit=0.0,a[MAXPAR],l[MAXPAR],s[MAXPAR];
bool  active;
   for (k=1; k <= (*GCP).common_i;k++) {
	   active = FALSE;
       for (i=0; i< max_SD;i++) {
           for (j=0;j<MAXPAR;j++) {
			   if ((GAP[i].FF_index[j] == k)  && GAP[i].FF_active[j] && GAP[i].fit) {
				   active = TRUE;
			   }
			   if ((GAP[i].SD_index[j] == k)  && GAP[i].SD_active[j] && GAP[i].fit) {
				   active = TRUE;
			   }
			   if ((GAP[i].SQ_index[j] == k)  && GAP[i].SQ_active[j] && GAP[i].fit) {
				   active = TRUE;
			   }
		   }
	   }
       for (i=0;i<max_SD;i++) {
           for (j=0;j<MAXPAR;j++) {
			   if (GAP[i].FF_index[j] == k) {
				     GAP[i].FF_active[j] = active;
			   }
			   if (GAP[i].SD_index[j] == k) {
				     GAP[i].SD_active[j] = active;
			   }
			   if (GAP[i].SQ_index[j] == k) {
				     GAP[i].SQ_active[j] = active;
			   }
		   }
	   }
   }

   *Ifit = 0.0;
/************************************************************************
       Calculates Intensity
 ************************************************************************/

   for (i=0;i<max_SD;i++) {
	   if (GAP[i].calcSDFF) {
          for (j=0;j<MAXPAR;j++) {
	          if (GAP[i].SD_index[j] > 0) {
                 a[j] = GAP[i].SD_factor[j]*(*GCP).P_common[GAP[i].SD_index[j]-1];
			  } else {
		         a[j] = par[i*(3*MAXPAR)+j+0];
			  }
	          if (GAP[i].FF_index[j] > 0) {
                 l[j] = GAP[i].FF_factor[j]*(*GCP).P_common[GAP[i].FF_index[j]-1];
			  } else {
		         l[j] = par[i*(3*MAXPAR)+j+MAXPAR];
			  }
	          if (GAP[i].SQ_index[j] > 0) {
                 s[j] = GAP[i].SQ_factor[j]*(*GCP).P_common[GAP[i].SQ_index[j]-1];
			  } else {
		         s[j] = par[i*(3*MAXPAR)+j+(2*MAXPAR)];
			  }
		  }
	   }
	   if (GAP[i].calcSDFF) {
		  IQ_int_core_global(interp,&TmpIfit,l,Q,Qres,a,s,&GAP[i], error);
		  if (*error == TRUE) return;
          *Ifit += TmpIfit;
	   }
   }
}



/*#########################################################################*/
/*#                                                                       #*/
/*# Sasfit_Init --                                                        #*/
/*#                                                                       #*/
/*#      This procedure initializes the sasfit command.                   #*/
/*#                                                                       #*/
/*# Results:                                                              #*/
/*#      A standard Tcl result.                                           #*/
/*#                                                                       #*/
/*# Side effects:                                                         #*/
/*#      None.                                                            #*/
/*#                                                                       #*/
/*#########################################################################*/
SASFIT_LIB_EXPORT int Sasfit_Init(Tcl_Interp *interp)
{
	struct sasfit_CData *SASFIT_CData;
	struct sasfit_CData *SASGlobalFIT_CData;

	if (Tcl_InitStubs(interp, "8.3", 0) == NULL) {
		return TCL_ERROR;
	}

	SASFIT_CData=(struct sasfit_CData *) Tcl_Alloc(sizeof(struct sasfit_CData));
	SASFIT_CData->ma=0;
	SASFIT_CData->mfit=0;
	SASFIT_CData->chisq=0.0;
	SASGlobalFIT_CData=(struct sasfit_CData *) Tcl_Alloc(sizeof(struct sasfit_CData));
	SASGlobalFIT_CData[0].ma=0;
	SASGlobalFIT_CData[0].mfit=0;
	SASGlobalFIT_CData[0].chisq=0.0;

	sasfit_env_set_interp(interp);
	sasfit_set_msg_handler(CHAN_INFO, 1, sasfit_tcl_out);
	sasfit_set_msg_handler(CHAN_ERR, 1, sasfit_tcl_err);
//	sasfit_param_override_init();
	sasfit_2d_init();

	Tcl_CreateCommand(interp, "sasfit_iq", (Tcl_CmdProc*) Sasfit_iqCmd, NULL, NULL);
	Tcl_CreateCommand(interp, "sasfit_2Diq", (Tcl_CmdProc*) Sasfit_2DiqCmd, NULL, NULL);
	Tcl_CreateCommand(interp, "sasfit_2Diqfit", (Tcl_CmdProc*) Sasfit_2DiqfitCmd, NULL, NULL);
	Tcl_CreateCommand(interp, "sasfit_global_iq", (Tcl_CmdProc*) Sasfit_global_iqCmd, NULL, NULL);
	Tcl_CreateCommand(interp, "sasfit_nr", (Tcl_CmdProc*) Sasfit_nrCmd, NULL, NULL);
	Tcl_CreateCommand(interp, "sasfit_res", (Tcl_CmdProc*) Sasfit_resCmd, NULL, NULL);
	Tcl_CreateCommand(interp, "sasfit_iqfit", (Tcl_CmdProc*) Sasfit_iqfitCmd, (void *)SASFIT_CData, NULL);
	Tcl_CreateCommand(interp, "sasfit_iqglobalfit", (Tcl_CmdProc*) Sasfit_iqglobalfitCmd,(void *) SASGlobalFIT_CData, NULL);
	Tcl_CreateCommand(interp, "sasfit_guess_err", (Tcl_CmdProc*) Sasfit_guess_errCmd, NULL, NULL);

	// some useful new commands
	Tcl_CreateObjCommand(interp, "sasfit_get_maxpar", (Tcl_ObjCmdProc*) sasfit_get_maxpar_cmd, NULL, NULL);
	Tcl_CreateObjCommand(interp, "sasfit_get_lib_prefix", (Tcl_ObjCmdProc*) sasfit_get_lib_prefix_cmd, NULL, NULL);
	Tcl_CreateObjCommand(interp, "sasfit_get_lib_suffix", (Tcl_ObjCmdProc*) sasfit_get_lib_suffix_cmd, NULL, NULL);
	Tcl_CreateObjCommand(interp, "sasfit_load_plugin", (Tcl_ObjCmdProc*) sasfit_load_plugin_cmd, NULL, NULL);
	Tcl_CreateObjCommand(interp, "sasfit_covar", (Tcl_ObjCmdProc*) sasfit_covar_cmd, (void *)SASFIT_CData, NULL);

    // sasfit_oz commands
	Tcl_CreateObjCommand(interp, "sasfit_oz_calc", (Tcl_ObjCmdProc*) sasfit_oz_calc_cmd, NULL, NULL);
	Tcl_CreateObjCommand(interp, "sasfit_oz_assign_data_sq", (Tcl_ObjCmdProc*) sasfit_oz_assign_data_sq_cmd, NULL, NULL);
	// DLS
	Tcl_CreateCommand(interp, "dls_CumulantFit", (Tcl_CmdProc*) DLS_CumulantFitCmd, NULL, NULL);
//	Tcl_CreateCommand(interp, "dls_ExponentialFit", (Tcl_CmdProc*) DLS_ExponentialFitCmd, NULL, NULL);
//	Tcl_CreateCommand(interp, "dls_StretchedExpFit", (Tcl_CmdProc*) DLS_StretchedExpFitCmd, NULL, NULL);
	Tcl_CreateCommand(interp, "dls_CumulantCalc", (Tcl_CmdProc*) DLS_CumulantCalcCmd, NULL, NULL);
//	Tcl_CreateCommand(interp, "dls_ExponentialCalc", (Tcl_CmdProc*) DLS_ExponentialCalcCmd, NULL, NULL);
//	Tcl_CreateCommand(interp, "dls_StretchedExpCalc", (Tcl_CmdProc*) DLS_StretchedExpCalcCmd, NULL, NULL);

	// sasfit extrapol
	Tcl_CreateCommand(interp, "sasfit_PorodFit", (Tcl_CmdProc*) Sasfit_PorodFitCmd, NULL, NULL);
	Tcl_CreateCommand(interp, "sasfit_GuinierFit", (Tcl_CmdProc*) Sasfit_GuinierFitCmd, NULL, NULL);
	Tcl_CreateCommand(interp, "sasfit_StructParCalc", (Tcl_CmdProc*) Sasfit_StructParCalcCmd, NULL, NULL);
	Tcl_CreateCommand(interp, "sasfit_ZimmFit", (Tcl_CmdProc*) Sasfit_ZimmFitCmd, NULL, NULL);
	Tcl_CreateCommand(interp, "sasfit_DebyeFit", (Tcl_CmdProc*) Sasfit_DebyeFitCmd, NULL, NULL);
	Tcl_CreateCommand(interp, "sasfit_OrnsteinZernickeFit", (Tcl_CmdProc*) Sasfit_OrnsteinZernickeFitCmd, NULL, NULL);

	Tcl_CreateCommand(interp, "sasfit_DR_EM_smoothing", (Tcl_CmdProc*) Sasfit_DR_EM_smoothing_Cmd, NULL, NULL);
	Tcl_CreateCommand(interp, "sasfit_DR_EM_ME_const", (Tcl_CmdProc*) Sasfit_DR_EM_ME_const_Cmd, NULL, NULL);
	Tcl_CreateCommand(interp, "sasfit_DR_EM_ME_adaptive", (Tcl_CmdProc*) Sasfit_DR_EM_ME_adaptive_Cmd, NULL, NULL);
	Tcl_CreateCommand(interp, "sasfit_DR_linReg", (Tcl_CmdProc*) Sasfit_DR_linReg_Cmd, NULL, NULL);
	Tcl_CreateCommand(interp, "sasfit_DR_MEM", (Tcl_CmdProc*) Sasfit_DR_MEM_Cmd, NULL, NULL);
	Tcl_CreateCommand(interp, "sasfit_DR_MuCh", (Tcl_CmdProc*) Sasfit_DR_MuCh_Cmd, NULL, NULL);
	Tcl_CreateCommand(interp, "sasfit_DR_SDM", (Tcl_CmdProc*) Sasfit_DR_SDM_Cmd, NULL, NULL);
	Tcl_CreateCommand(interp, "sasfit_FredholmIntegrals_Regularization", (Tcl_CmdProc*) Sasfit_FredholmIntegrals_RegularizationCmd, (void *)SASFIT_CData, NULL);


	return Tcl_PkgProvide(interp, "sasfit", "1.0");
}



/*#########################################################################*/
/*#                                                                       #*/
/*# Sasfit_iqCmd --                                                       #*/
/*#                                                                       #*/
/*#      This function implements the Tcl "sasfit_iq" command.            #*/
/*#                                                                       #*/
/*# Results:                                                              #*/
/*#      A standard Tcl result.                                           #*/
/*#                                                                       #*/
/*# Side effects:                                                         #*/
/*#      None.                                                            #*/
/*#                                                                       #*/
/*#########################################################################*/

int Sasfit_iqCmd(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int        argc;
    const char       **argv;
{
	sasfit_analytpar *AP;
	int    i, j;
	int    max_SD;
	scalar  alambda,chisq,reducedchisq,R,wR,QQ,diffR,obsR,wdiffR,wobsR;
	scalar  varianceOFfit, avgsigma;
	char   sBuffer[256];
	Tcl_DString DsBuffer;
	scalar  *h, *Ih, *DIh, *Ith, *res, *Ihsubstract;
	int    *lista;
	int    ma, mfit,ndata;
	bool   error;
	int    error_type, rcode, interrupt;
	scalar  *a, *dydpar;
	int th_id, nthreads;
    sasfit_timer *tm;

	error = FALSE;
	//Det2DPar.calc2D = FALSE;
	sasfit_param_override_init();

	chisq = 0.0;
	reducedchisq = 0.0;
    ndata = 0.0;
    mfit = 0.0;
    QQ = 0.0;
    R = 0.0;
    wR = 0.0;
    diffR = 0.0;
    wdiffR = 0.0;
    obsR = 0.0;
    wobsR = 0.0;
    varianceOFfit = 0.0;
    avgsigma = 0.0;

	if (argc != 3) {
		sasfit_err("wrong # args: shoud be sasfit_iq ?analyt_par? ?xyer_data?\n");
		return TCL_ERROR;
	}
	if (TCL_ERROR == get_AP(interp,argv,&AP,&max_SD,&alambda,&error_type,&h,&Ih,&DIh,&res,&ndata))
	{
		return TCL_ERROR;
	}
	for (i=0;i<max_SD;i++) {
		AP[i].fit = FALSE;
		for (j=0;j<MAXPAR;j++) {
			AP[i].SD_active[j] = 0;
			AP[i].FF_active[j] = 0;
			AP[i].SQ_active[j] = 0;
		}
	}

	sasfit_ap2paramlist(&lista,&ma,&mfit,&a,AP,NULL,max_SD);
    if (sasfit_get_iq_or_gz()==4) sasfit_set_iq_or_gz(1);

	Ith = dvector(0,ndata-1);
	Ihsubstract = dvector(0,ndata-1);
	dydpar = dvector(0,ma-1);
    for (i=0;i<ndata;i++) {
        Ith[i] = 0;
        Ihsubstract[i] = 0;
    }
	Tcl_DStringInit(&DsBuffer);

	Tcl_DStringStartSublist(&DsBuffer);

//     sasfit_out("max num threads %d\n",omp_get_num_procs());
//     omp_set_num_threads((omp_get_num_procs()>1)?omp_get_num_procs()-1:1);
 //    omp_set_num_threads(1);

    sasfit_int_ws_init();
//{
 //   #pragma omp  parallel for
//	for (i=0;i<ndata;i++) {
 //       IQ(interp,h[i],res[i],a,&Ith[i],&Ihsubstract[i],dydpar,max_SD,AP,error_type,0,&error);
//      #pragma omp atom
//        sasfit_out("nthreads: %d, i:%d\n",omp_get_num_threads(),i);
//	}
//}
    tm = sasfit_timer_create();
    sasfit_timer_start(tm);

    for (i=0;i<ndata;i++) {
		IQ(interp,h[i],res[i],a,&Ith[i],&Ihsubstract[i],dydpar,max_SD,AP,error_type,0,&error);
		if (DIh[i] !=0) {
			chisq = chisq+pow((Ih[i]-Ith[i])/DIh[i],2.0);
            wobsR = wobsR + Ih[i]*Ih[i]/(DIh[i]*DIh[i]);
            avgsigma = avgsigma+1.0/gsl_pow_2(DIh[i]);
		}
	    diffR = diffR + fabs((Ith[i]-Ih[i]));
	    obsR = obsR + fabs(Ih[i]);
	    wdiffR=chisq;
		if (error==TRUE) {
			free_dvector(Ith,0,ndata-1);
			free_dvector(Ihsubstract,0,ndata-1);
			free_dvector(h,0,ndata-1);
			free_dvector(res,0,ndata-1);
			free_dvector(Ih,0,ndata-1);
			free_dvector(DIh,0,ndata-1);
			free_dvector(dydpar,0,ma-1);
			Tcl_Free((char *) AP);
			sasfit_timer_destroy(&tm);
			return TCL_ERROR;
		}
		if (sasfit_timer_measure(tm) > MAXTM4UPDATE) {
            sprintf(sBuffer,"set ::SASfitprogressbar %lf",(i+1.0)/(1.0*ndata)*100.0);
            Tcl_EvalEx(interp,sBuffer,-1,TCL_EVAL_DIRECT);
            Tcl_EvalEx(interp,"update",-1,TCL_EVAL_DIRECT);
            interrupt = check_interrupt4calc(interp,&error);
            sasfit_timer_start(tm);
        }
		if (error==TRUE) {
			free_dvector(Ith,0,ndata-1);
			free_dvector(Ihsubstract,0,ndata-1);
			free_dvector(h,0,ndata-1);
			free_dvector(res,0,ndata-1);
			free_dvector(Ih,0,ndata-1);
			free_dvector(DIh,0,ndata-1);
			free_dvector(dydpar,0,ma-1);
			Tcl_Free((char *) AP);
			sasfit_timer_destroy(&tm);
			return TCL_ERROR;
		}
		float_to_string(sBuffer,Ith[i]);
		Tcl_DStringAppendElement(&DsBuffer,sBuffer);
	}
    sprintf(sBuffer,"set ::SASfitprogressbar %lf",100.0);
    Tcl_EvalEx(interp,sBuffer,-1,TCL_EVAL_DIRECT);
    Tcl_EvalEx(interp,"update",-1,TCL_EVAL_DIRECT);
    interrupt = check_interrupt4calc(interp,&error);
    sasfit_timer_destroy(&tm);
//	float_to_string(sBuffer,chisq/ndata);
//	Tcl_SetVar2(interp,argv[1],"chisq", sBuffer,0);


int_to_string(sBuffer,mfit);
Tcl_SetVar2(interp,argv[1],"mfit", sBuffer,0);
int_to_string(sBuffer,ndata);
Tcl_SetVar2(interp,argv[1],"ndata", sBuffer,0);
float_to_string(sBuffer,chisq);
Tcl_SetVar2(interp,argv[1],"chisq", sBuffer,0);

if ((ndata-mfit) > 0) {
	reducedchisq = chisq/(ndata-mfit);
}
if ((avgsigma) > 0) {
	varianceOFfit = reducedchisq/(avgsigma/ndata);
}
float_to_string(sBuffer,reducedchisq);
Tcl_SetVar2(interp,argv[1],"reducedchisq", sBuffer,0);
float_to_string(sBuffer,varianceOFfit);
Tcl_SetVar2(interp,argv[1],"varianceOFfit", sBuffer,0);

if (obsR == 0) {
        float_to_string(sBuffer,0);
    } else {
        float_to_string(sBuffer,diffR/obsR);
    }
Tcl_SetVar2(interp,argv[1],"R", sBuffer,0);

if (wobsR == 0) {
        float_to_string(sBuffer,0);
    } else {
        float_to_string(sBuffer,sqrt(wdiffR/wobsR));
    }
Tcl_SetVar2(interp,argv[1],"wR", sBuffer,0);

QQ=gsl_sf_gamma_inc_Q((ndata-mfit)/2.,chisq/2.);
float_to_string(sBuffer,QQ);
Tcl_SetVar2(interp,argv[1],"Q", sBuffer,0);

int_to_string(sBuffer,mfit);
Tcl_SetVar2(interp,argv[1],"mfit", sBuffer,0);
int_to_string(sBuffer,ndata);
Tcl_SetVar2(interp,argv[1],"ndata", sBuffer,0);

	Tcl_DStringEndSublist(&DsBuffer);

	Tcl_DStringStartSublist(&DsBuffer);
	for (i=0;i<ndata;i++) {
    if (sasfit_get_iq_or_gz() == 1) {
            float_to_string(sBuffer,Ih[i]/Ihsubstract[i]);
        } else {
            float_to_string(sBuffer,Ih[i]-Ihsubstract[i]);
        }
		Tcl_DStringAppendElement(&DsBuffer,sBuffer);
	}
	Tcl_DStringEndSublist(&DsBuffer);

	Tcl_DStringStartSublist(&DsBuffer);
	for (i=0;i<ndata;i++) {
        if (sasfit_get_iq_or_gz() == 1) {
            float_to_string(sBuffer,Ith[i]/Ihsubstract[i]);
        } else {
            float_to_string(sBuffer,Ith[i]-Ihsubstract[i]);
        }
		Tcl_DStringAppendElement(&DsBuffer,sBuffer);
	}
	Tcl_DStringEndSublist(&DsBuffer);

	Tcl_DStringStartSublist(&DsBuffer);
	for (i=0;i<ndata;i++) {
		if (DIh[i] == 0.0) {
			free_dvector(Ith,0,ndata-1);
			free_dvector(Ihsubstract,0,ndata-1);
			free_dvector(h,0,ndata-1);
			free_dvector(res,0,ndata-1);
			free_dvector(Ih,0,ndata-1);
			free_dvector(DIh,0,ndata-1);
			free_dvector(dydpar,0,ma-1);
			Tcl_Free((char *) AP);
			return TCL_ERROR;
		}
		float_to_string(sBuffer,(Ih[i]-Ith[i])/DIh[i]);
		Tcl_DStringAppendElement(&DsBuffer,sBuffer);
	}
	Tcl_DStringEndSublist(&DsBuffer);

	Tcl_DStringStartSublist(&DsBuffer);
	for (i=0;i<ndata;i++) {
		float_to_string(sBuffer,DIh[i]);
		Tcl_DStringAppendElement(&DsBuffer,sBuffer);
	}
	Tcl_DStringEndSublist(&DsBuffer);


	Tcl_DStringResult(interp,&DsBuffer);
	Tcl_DStringFree(&DsBuffer);

	free_dvector(Ith,0,ndata-1);
	free_dvector(Ihsubstract,0,ndata-1);
	free_dvector(h,0,ndata-1);
	free_dvector(res,0,ndata-1);
	free_dvector(Ih,0,ndata-1);
	free_dvector(DIh,0,ndata-1);
	free_dvector(dydpar,0,ma-1);

	Tcl_Free((char *) AP);
	return TCL_OK;
}



/*#########################################################################*/
/*#                                                                       #*/
/*# Sasfit_global_iqCmd --                                                #*/
/*#                                                                       #*/
/*#      This function implements the Tcl "sasfit_global_iq" command.     #*/
/*#                                                                       #*/
/*# Results:                                                              #*/
/*#      A standard Tcl result.                                           #*/
/*#                                                                       #*/
/*# Side effects:                                                         #*/
/*#      None.                                                            #*/
/*#                                                                       #*/
/*#########################################################################*/

int Sasfit_global_iqCmd(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int        argc;
    char       **argv;
{
    sasfit_analytpar *GAP;
    sasfit_commonpar GCP;
    int    i, j, k;
    int    max_SD;
    scalar  alambda, chisq,reducedchisq,R,wR,QQ,diffR,obsR,wdiffR,wobsR;
    scalar  avgsigma,varianceOFfit;
    Tcl_DString DsBuffer;
    scalar  **h, **Ih, **DIh, **Ith, **res, **Isub;
    int    *lista, npoints, ipoint;
    int    ma, mfit,*ndata,*hide, totndata;
    bool   error;
    int    error_type, interrupt;
    scalar  *a, *dydpar;
	char   sBuffer[132];
    sasfit_timer *tm;

	error = FALSE;
	//Det2DPar.calc2D = FALSE;
	sasfit_param_override_init();

	chisq = 0.0;
	reducedchisq = 0.0;
    totndata = 0.0;
    mfit = 0.0;
    QQ = 0.0;
    R = 0.0;
    wR = 0.0;
    diffR = 0.0;
    wdiffR = 0.0;
    obsR = 0.0;
    wobsR = 0.0;
    avgsigma = 0.0;
    varianceOFfit = 0.0;

    if (argc != 4) {
       sasfit_err("wrong # args: shoud be sasfit_global_iq ?analyt_par? ?xyer_data? ?hide?\n");
       return TCL_ERROR;
    }

    if (TCL_ERROR == get_GlobalAP(interp,argv,
                            &GAP,&GCP,&max_SD,&alambda,&error_type,
                            &h,&Ih,&DIh,&res,&Ith,&ndata,&hide)) {
       return TCL_ERROR;
    }


sasfit_ap2paramlist(&lista,&ma,&mfit,&a,GAP,&GCP,max_SD);
if (sasfit_get_iq_or_gz()==4) sasfit_set_iq_or_gz(1);

dydpar = dvector(0,ma-1);
Isub = (scalar **) Tcl_Alloc((unsigned) (GCP.nmultset)*sizeof(scalar*));
for (k=0;k<GCP.nmultset;k++) {
	Isub[k] = dvector(0,ndata[k]-1);
}

    for(i=0;i<max_SD;i++){
        GAP[i].fit = FALSE;
        GAP[i].dodydpar = 0;
        for(j=0;j<MAXPAR;j++) {
            GAP[i].SD_active[j] = 0;
            GAP[i].FF_active[j] = 0;
            GAP[i].SQ_active[j] = 0;
        }
    }

Tcl_DStringInit(&DsBuffer);

Tcl_DStringStartSublist(&DsBuffer);

npoints=0;
for (j=0;j<GCP.nmultset;j++) npoints = npoints+ndata[j];

ipoint=0;
tm = sasfit_timer_create();
sasfit_timer_start(tm);

for (j=0;j<GCP.nmultset;j++) {
	Tcl_DStringStartSublist(&DsBuffer);
	for (i=0;i<ndata[j];i++) {
       IQ_Global(interp,h[j][i],res[j][i],a,&Ith[j][i],&Isub[j][i],dydpar,max_SD,GAP,&GCP,error_type,j+1,&error);
	   if (DIh[j][i] == 0) {
		   sasfit_err("Sasfit_global_iqCmd: zero error bar\n");
		   error = TRUE;
	   } else if (!hide[j]){
            chisq = chisq + (Ith[j][i]-Ih[j][i])*(Ith[j][i]-Ih[j][i])/(DIh[j][i]*DIh[j][i]);
            diffR = diffR + fabs((Ith[j][i]-Ih[j][i]));
            obsR = obsR + fabs(Ih[j][i]);
            wdiffR=wdiffR + (Ith[j][i]-Ih[j][i])*(Ith[j][i]-Ih[j][i])/(DIh[j][i]*DIh[j][i]);
            wobsR=wobsR + Ih[j][i]*Ih[j][i]/(DIh[j][i]*DIh[j][i]);
            avgsigma = avgsigma + 1.0/(DIh[j][i]*DIh[j][i]);
            totndata++;
       }
	   ipoint++;
	   if (error==TRUE ) {
	      for (k=0;k<GCP.nmultset;k++) {
             free_dvector(Ith[k],0,ndata[k]-1);
             free_dvector(Isub[k],0,ndata[k]-1);
             free_dvector(h[k],0,ndata[k]-1);
             free_dvector(res[k],0,ndata[k]-1);
             free_dvector(Ih[k],0,ndata[k]-1);
             free_dvector(DIh[k],0,ndata[k]-1);
		  }
          Tcl_Free((char *) GAP);
		  free_dvector(dydpar,0,ma-1);
		Tcl_Free((char *) Isub);
		Tcl_Free((char *) h);
		Tcl_Free((char *) Ih);
		Tcl_Free((char *) Ith);
		Tcl_Free((char *) DIh);
		Tcl_Free((char *) res);
		Tcl_Free((char *) ndata);
		Tcl_Free((char *) hide);
//		  free_ivector(ndata,0,GCP.nmultset-1); // see below
		  sasfit_timer_destroy(&tm);
          return TCL_ERROR;
	   }
		if (sasfit_timer_measure(tm) > MAXTM4UPDATE) {
            sprintf(sBuffer,"set ::SASfitprogressbar %lf",100.0* ipoint/(1.0*npoints));
            Tcl_EvalEx(interp,sBuffer,-1,TCL_EVAL_DIRECT);
            Tcl_EvalEx(interp,"update",-1,TCL_EVAL_DIRECT);
            interrupt = check_interrupt4calc(interp,&error);
            sasfit_timer_start(tm);
        }
	   if (error==TRUE ) {
	      for (k=0;k<GCP.nmultset;k++) {
             free_dvector(Ith[k],0,ndata[k]-1);
             free_dvector(Isub[k],0,ndata[k]-1);
             free_dvector(h[k],0,ndata[k]-1);
             free_dvector(res[k],0,ndata[k]-1);
             free_dvector(Ih[k],0,ndata[k]-1);
             free_dvector(DIh[k],0,ndata[k]-1);
		  }
          Tcl_Free((char *) GAP);
		  free_dvector(dydpar,0,ma-1);
		Tcl_Free((char *) Isub);
		Tcl_Free((char *) h);
		Tcl_Free((char *) Ih);
		Tcl_Free((char *) Ith);
		Tcl_Free((char *) DIh);
		Tcl_Free((char *) res);
		Tcl_Free((char *) ndata);
		Tcl_Free((char *) hide);
//		  free_ivector(ndata,0,GCP.nmultset-1); // see below
		  sasfit_timer_destroy(&tm);
          return TCL_ERROR;
	   }
   	   float_to_string(sBuffer,Ith[j][i]);
       Tcl_DStringAppendElement(&DsBuffer,sBuffer);
 	}
	Tcl_DStringEndSublist(&DsBuffer);
}
sprintf(sBuffer,"set ::SASfitprogressbar %lf",100.0);
Tcl_EvalEx(interp,sBuffer,-1,TCL_EVAL_DIRECT);
Tcl_EvalEx(interp,"update",-1,TCL_EVAL_DIRECT);
interrupt = check_interrupt4calc(interp,&error);
sasfit_timer_destroy(&tm);
Tcl_DStringEndSublist(&DsBuffer);

Tcl_DStringStartSublist(&DsBuffer);
for (j=0;j<GCP.nmultset;j++) {
	Tcl_DStringStartSublist(&DsBuffer);
	for (i=0;i<ndata[j];i++) {
       if (sasfit_get_iq_or_gz() == 1 && Isub[j][i]!=0) {
           float_to_string(sBuffer,Ith[j][i]/Isub[j][i]);
       } else {
           float_to_string(sBuffer,Ith[j][i]-Isub[j][i]);
       }
       Tcl_DStringAppendElement(&DsBuffer,sBuffer);
 	}
	Tcl_DStringEndSublist(&DsBuffer);
}
Tcl_DStringEndSublist(&DsBuffer);

Tcl_DStringStartSublist(&DsBuffer);
for (j=0;j<GCP.nmultset;j++) {
	Tcl_DStringStartSublist(&DsBuffer);
	for (i=0;i<ndata[j];i++) {
       if (sasfit_get_iq_or_gz() == 1 && Isub[j][i]!=0) {
           float_to_string(sBuffer,Ih[j][i]/Isub[j][i]);
       } else {
           float_to_string(sBuffer,Ih[j][i]-Isub[j][i]);
       }
       Tcl_DStringAppendElement(&DsBuffer,sBuffer);
 	}
	Tcl_DStringEndSublist(&DsBuffer);
}
Tcl_DStringEndSublist(&DsBuffer);

Tcl_DStringStartSublist(&DsBuffer);
for (j=0;j<GCP.nmultset;j++) {
	Tcl_DStringStartSublist(&DsBuffer);
	for (i=0;i<ndata[j];i++) {
   	   float_to_string(sBuffer,DIh[j][i]);
       Tcl_DStringAppendElement(&DsBuffer,sBuffer);
 	}
	Tcl_DStringEndSublist(&DsBuffer);
}
Tcl_DStringEndSublist(&DsBuffer);

Tcl_DStringStartSublist(&DsBuffer);
for (j=0;j<GCP.nmultset;j++) {
   Tcl_DStringStartSublist(&DsBuffer);
   for (i=0;i<ndata[j];i++) {
	  if (DIh[j][i] == 0) {
	     sasfit_err("Sasfit_global_iq_Cmd: zero error bar\n");
	     return TCL_ERROR;
	  }
      float_to_string(sBuffer,(Ith[j][i]-Ih[j][i])/DIh[j][i]);
      Tcl_DStringAppendElement(&DsBuffer,sBuffer);
   }
   Tcl_DStringEndSublist(&DsBuffer);
}
Tcl_DStringEndSublist(&DsBuffer);



float_to_string(sBuffer,chisq);
Tcl_SetVar2(interp,argv[1],"chisq", sBuffer,0);
if ((totndata-mfit) !=0) {
	reducedchisq = chisq/(totndata-mfit);
}
float_to_string(sBuffer,reducedchisq);
Tcl_SetVar2(interp,argv[1],"reducedchisq", sBuffer,0);

if (obsR == 0) {
        float_to_string(sBuffer,0);
    } else {
        float_to_string(sBuffer,diffR/obsR);
    }
Tcl_SetVar2(interp,argv[1],"R", sBuffer,0);

if (wobsR == 0) {
        float_to_string(sBuffer,0);
    } else {
        float_to_string(sBuffer,sqrt(wdiffR/wobsR));
    }
Tcl_SetVar2(interp,argv[1],"wR", sBuffer,0);

QQ=gsl_sf_gamma_inc_Q((totndata-mfit)/2.,chisq/2.);
float_to_string(sBuffer,QQ);
Tcl_SetVar2(interp,argv[1],"Q", sBuffer,0);

if (avgsigma > 0) varianceOFfit = reducedchisq*totndata/avgsigma;
float_to_string(sBuffer,varianceOFfit);
Tcl_SetVar2(interp,argv[1],"varianceOFfit", sBuffer,0);

int_to_string(sBuffer,mfit);
Tcl_SetVar2(interp,argv[1],"mfit", sBuffer,0);
int_to_string(sBuffer,totndata);
Tcl_SetVar2(interp,argv[1],"ndata", sBuffer,0);

Tcl_DStringResult(interp,&DsBuffer);
Tcl_DStringFree(&DsBuffer);

for (k=0;k<GCP.nmultset;k++) {
   free_dvector(Ith[k],0,ndata[k]-1);
   free_dvector(Isub[k],0,ndata[k]-1);
   free_dvector(h[k],0,ndata[k]-1);
   free_dvector(res[k],0,ndata[k]-1);
   free_dvector(Ih[k],0,ndata[k]-1);
   free_dvector(DIh[k],0,ndata[k]-1);
}
free_dvector(dydpar,0,ma-1);
Tcl_Free((char *) GAP);
/* what do these lines ? Isub[0] == *Isub
*/
Tcl_Free((char *) Isub); // Isub was not allocated by dvector() !!!
//free_dvector(Isub,0,GCP.nmultset-1);
Tcl_Free((char *) h);
//free_dvector(h,0,GCP.nmultset-1);
Tcl_Free((char *) Ih);
//free_dvector(Ih,0,GCP.nmultset-1);
Tcl_Free((char *) Ith);
//free_dvector(Ith,0,GCP.nmultset-1);
Tcl_Free((char *) DIh);
//free_dvector(DIh,0,GCP.nmultset-1);
Tcl_Free((char *) res);
Tcl_Free((char *) ndata);
Tcl_Free((char *) hide);
//free_dvector(res,0,GCP.nmultset-1);
//free_ivector(ndata,0,GCP.nmultset-1);
/*
// needs further investigation, separate+custom free-routine for TCL_ALLOC memory ..
*/
return TCL_OK;
}

/*############################################################################################*/
/*#                                                                                          #*/
/*# Sasfit_FredholmIntegrals_RegularizationCmd --                                            #*/
/*#                                                                                          #*/
/*#      This function implements the Tcl "Sasfit_FredholmIntegrals_Regularization" command. #*/
/*#                                                                                          #*/
/*# Results:                                                                                 #*/
/*#      A standard Tcl result.                                                              #*/
/*#                                                                                          #*/
/*# Side effects:                                                                            #*/
/*#      None.                                                                               #*/
/*#                                                                                          #*/
/*############################################################################################*/

int Sasfit_FredholmIntegrals_RegularizationCmd(SASFIT_CData, interp, argc, argv)
    struct sasfit_CData *SASFIT_CData;
    Tcl_Interp *interp;
    int        argc;
    char       **argv;
{
sasfit_analytpar *GlobalAP;
sasfit_commonpar GCP;
int    i,j,m,k;
int    max_SD;
char   sBuffer[256];
scalar  **h, **Ih, **DIh, **Ith, **Ihtrans, **DIhtrans, **Isub, **res;
int    *lista;
int    ma;
int    mfit,*ndata,*hide, totndata;
bool   error;
int    error_type;
scalar  *a;
scalar  chisq,reducedchisq,R,wR,QQ,diffR,obsR,wdiffR,wobsR;
scalar  avgsigma,varianceOFfit;
scalar  alambda;
Tcl_DString DsBuffer;
sasfit_timer *tm;

error = FALSE;
//Det2DPar.calc2D = FALSE;
sasfit_param_override_init();

if (argc != 5) {
   sasfit_err("wrong # args: shoud be sasfit_iqglobalfit ?analyt_par? ?xyer_data? ?hide?\n");
   return TCL_ERROR;
}
if (TCL_ERROR == get_GlobalAP(interp,argv,
                        &GlobalAP,&GCP,&max_SD,&alambda,&error_type,
                        &h,&Ih,&DIh,&res,&Ith,&ndata,&hide)) {
   return TCL_ERROR;
}


sasfit_out("nmultset: %d\n",GCP.nmultset);

Isub = (scalar **) Tcl_Alloc((unsigned) (GCP.nmultset)*sizeof(scalar*));
DIhtrans = (scalar **) Tcl_Alloc((unsigned) (GCP.nmultset)*sizeof(scalar*));
Ihtrans = (scalar **) Tcl_Alloc((unsigned) (GCP.nmultset)*sizeof(scalar*));
for (k=0;k<GCP.nmultset;k++) {
	Isub[k] = dvector(0,ndata[k]-1);
	DIhtrans[k] = dvector(0,ndata[k]-1);
	Ihtrans[k] = dvector(0,ndata[k]-1);
}

sasfit_ap2paramlist(&lista,&ma,&mfit,&a,GlobalAP,&GCP,max_SD);

if (SASFIT_CData->ma == 0) {
   SASFIT_CData->lista = ivector(0,ma-1);
   SASFIT_CData->a = dvector(0,ma-1);
}
if ((SASFIT_CData->ma != 0) && (   (SASFIT_CData->mfit != mfit)
                                  || (SASFIT_CData->ma != ma    ))  ){
   free_matrix(SASFIT_CData->alpha,0,SASFIT_CData->ma-1,
                                     0,SASFIT_CData->ma-1);
   free_matrix(SASFIT_CData->covar,0,SASFIT_CData->ma-1,
                                     0,SASFIT_CData->ma-1);
   free_dvector(SASFIT_CData->da,0,SASFIT_CData->ma-1);
   free_dvector(SASFIT_CData->P_common,0,GCP.common_i-1);
   free_dvector(SASFIT_CData->atry,0,SASFIT_CData->ma-1);
   free_dvector(SASFIT_CData->beta,0,SASFIT_CData->ma-1);
   free_matrix(SASFIT_CData->oneda,0,SASFIT_CData->ma-1,0,0);
}
if (    (SASFIT_CData->ma == 0)
     || (SASFIT_CData->ma != ma)
     || (SASFIT_CData->mfit != mfit) ) {
   SASFIT_CData->alpha = matrix(0,ma-1,0,ma-1);
   SASFIT_CData->covar = matrix(0,ma-1,0,ma-1);
   SASFIT_CData->da    = dvector(0,ma-1);
   SASFIT_CData->atry  = dvector(0,ma-1);
   SASFIT_CData->P_common = dvector(0,GCP.common_i-1);
   SASFIT_CData->common_i = GCP.common_i;
   SASFIT_CData->beta  = dvector(0,ma-1);
   SASFIT_CData->oneda = matrix(0,ma-1,0,0);
}
free_ivector(SASFIT_CData->lista,0,ma-1);
free_dvector(SASFIT_CData->a,0,ma-1);
SASFIT_CData->lista = lista;
SASFIT_CData->a = a;
SASFIT_CData->ma    = ma;
SASFIT_CData->mfit  = mfit;
SASFIT_CData->max_SD = max_SD;

Tcl_ResetResult(interp);

for (i=0;i<max_SD;i++) {
	GlobalAP[i].dodydpar = 1;
}

if (sasfit_get_iq_or_gz() == 4) {
    for (j=0;j<GCP.nmultset;j++) {
        for (i=0;i<ndata[j];i++) {
            Ihtrans[j][i] = log(Ih[j][i]/Ih[j][ndata[j]-1]);
            DIhtrans[j][i] = sqrt( gsl_pow_2(DIh[j][i]/Ih[j][i]) + gsl_pow_2(DIh[j][ndata[j]-1]/Ih[j][ndata[j]-1]) );
        }
 	}
} else {
    for (j=0;j<GCP.nmultset;j++) {
        for (i=0;i<ndata[j];i++) {
            Ihtrans[j][i] = Ih[j][i];
            DIhtrans[j][i] = DIh[j][i];
        }
    }
}
SASFITfridaGlobal(interp,h,Ihtrans,res,DIhtrans,Ith,Isub,ndata,max_SD,GlobalAP,&GCP,error_type,
       SASFIT_CData,&chisq,IQ_Global,&alambda,&error);

if (error == TRUE) {
   save_GlobalAP(interp,argv[1],GlobalAP,&GCP,max_SD,alambda);
   for (k=0;k<GCP.nmultset;k++) {
      free_dvector(Ith[k],0,ndata[k]-1);
      free_dvector(Isub[k],0,ndata[k]-1);
      free_dvector(h[k],0,ndata[k]-1);
      free_dvector(res[k],0,ndata[k]-1);
      free_dvector(Ih[k],0,ndata[k]-1);
      free_dvector(DIh[k],0,ndata[k]-1);
      free_dvector(DIhtrans[k],0,ndata[k]-1);
      free_dvector(Ihtrans[k],0,ndata[k]-1);
   }
   free_dvector(GCP.P_common,0,GCP.common_i-1);
   free_ivector(GCP.P_common_index,0,GCP.common_i-1);
   free_dvector(GCP.P_common_err,0,GCP.common_i-1);
   Tcl_Free((char *) GlobalAP);
//   Tcl_Free((char *) *Isub);
//   free_dvector(dydpar,0,ma-1);
   free_ivector(ndata,0,GCP.nmultset-1);
//   Tcl_Free((char *) *h);
//   Tcl_Free((char *) *Ih);
//   Tcl_Free((char *) *Ith);
//   Tcl_Free((char *) *DIh);
//   Tcl_Free((char *) *res);
   return TCL_ERROR;
}


sasfit_ap2paramlist(&lista,&ma,&mfit,&a,GlobalAP,&GCP,max_SD);

chisq = 0.0;
reducedchisq = 0.0;
totndata = 0.0;
mfit = 0.0;
QQ = 0.0;
R = 0.0;
wR = 0.0;
diffR = 0.0;
wdiffR = 0.0;
obsR = 0.0;
wobsR = 0.0;
avgsigma = 0.0;
varianceOFfit = 0.0;

Tcl_DStringInit(&DsBuffer);

Tcl_DStringStartSublist(&DsBuffer);
for (m=0;m<GCP.nmultset;m++) {
   Tcl_DStringStartSublist(&DsBuffer);
   for (i=0;i<ndata[m];i++) {
      float_to_string(sBuffer,Ith[m][i]);
      Tcl_DStringAppendElement(&DsBuffer,sBuffer);
	  if (DIh[m][i] == 0) {
	     sasfit_err("Sasfit_global_iqCmd: zero error bar\n");
	     return TCL_ERROR;
	  }
	  chisq = chisq + pow((Ith[m][i]-Ih[m][i])/DIh[m][i],2.0);
      diffR = diffR + fabs((Ith[m][i]-Ih[m][i]));
      obsR = obsR + fabs(Ih[m][i]);
	  wdiffR=wdiffR + (Ith[m][i]-Ih[m][i])*(Ith[m][i]-Ih[m][i])/(DIh[m][i]*DIh[m][i]);
	  wobsR=wobsR + Ih[m][i]*Ih[m][i]/(DIh[m][i]*DIh[m][i]);
	  avgsigma = avgsigma/(DIh[m][i]*DIh[m][i]);
      totndata++;
   }
   Tcl_DStringEndSublist(&DsBuffer);
}
Tcl_DStringEndSublist(&DsBuffer);

Tcl_DStringStartSublist(&DsBuffer);
for (j=0;j<GCP.nmultset;j++) {
	Tcl_DStringStartSublist(&DsBuffer);
	for (i=0;i<ndata[j];i++) {
       if (((sasfit_get_iq_or_gz() == 1) || (sasfit_get_iq_or_gz() == 4)) && Isub[j][i]!=0) {
           float_to_string(sBuffer,Ith[j][i]/Isub[j][i]);
       } else {
           float_to_string(sBuffer,Ith[j][i]-Isub[j][i]);
       }
       Tcl_DStringAppendElement(&DsBuffer,sBuffer);
 	}
	Tcl_DStringEndSublist(&DsBuffer);
}
Tcl_DStringEndSublist(&DsBuffer);

Tcl_DStringStartSublist(&DsBuffer);
for (j=0;j<GCP.nmultset;j++) {
	Tcl_DStringStartSublist(&DsBuffer);
	for (i=0;i<ndata[j];i++) {
       if (((sasfit_get_iq_or_gz() == 1) || (sasfit_get_iq_or_gz() == 4)) && (Isub[j][i] !=0)){
           float_to_string(sBuffer,Ih[j][i]/Isub[j][i]);
       } else {
           float_to_string(sBuffer,Ih[j][i]-Isub[j][i]);
       }
       Tcl_DStringAppendElement(&DsBuffer,sBuffer);
 	}
	Tcl_DStringEndSublist(&DsBuffer);
}
Tcl_DStringEndSublist(&DsBuffer);


Tcl_DStringStartSublist(&DsBuffer);
for (j=0;j<GCP.nmultset;j++) {
	Tcl_DStringStartSublist(&DsBuffer);
	for (i=0;i<ndata[j];i++) {
   	   float_to_string(sBuffer,DIh[j][i]);
       Tcl_DStringAppendElement(&DsBuffer,sBuffer);
 	}
	Tcl_DStringEndSublist(&DsBuffer);
}
Tcl_DStringEndSublist(&DsBuffer);


Tcl_DStringStartSublist(&DsBuffer);
for (m=0;m<GCP.nmultset;m++) {
   Tcl_DStringStartSublist(&DsBuffer);
   for (i=0;i<ndata[m];i++) {
	  if (DIh[m][i] == 0) {
	     sasfit_err("Sasfit_iqglobalfitCmd: zero error bar\n");
	     return TCL_ERROR;
	  }
      float_to_string(sBuffer,(Ith[m][i]-Ih[m][i])/DIh[m][i]);
      Tcl_DStringAppendElement(&DsBuffer,sBuffer);
   }
   Tcl_DStringEndSublist(&DsBuffer);
}
Tcl_DStringEndSublist(&DsBuffer);


Tcl_DStringResult(interp,&DsBuffer);
Tcl_DStringFree(&DsBuffer);

float_to_string(sBuffer,chisq);
Tcl_SetVar2(interp,argv[1],"chisq", sBuffer,0);
if ((totndata-mfit) !=0) {
	reducedchisq = chisq/(totndata-mfit);
}
float_to_string(sBuffer,reducedchisq);
Tcl_SetVar2(interp,argv[1],"reducedchisq", sBuffer,0);

if (obsR == 0) {
        float_to_string(sBuffer,0);
    } else {
        float_to_string(sBuffer,diffR/obsR);
    }
Tcl_SetVar2(interp,argv[1],"R", sBuffer,0);

if (wobsR == 0) {
        float_to_string(sBuffer,0);
    } else {
        float_to_string(sBuffer,sqrt(wdiffR/wobsR));
    }
Tcl_SetVar2(interp,argv[1],"wR", sBuffer,0);

QQ=gsl_sf_gamma_inc_Q((totndata-mfit)/2.0,chisq/2.0);
float_to_string(sBuffer,QQ);
Tcl_SetVar2(interp,argv[1],"Q", sBuffer,0);
if (avgsigma > 0) varianceOFfit=reducedchisq*totndata/avgsigma;
float_to_string(sBuffer,varianceOFfit);
Tcl_SetVar2(interp,argv[1],"varianceOFfit", sBuffer,0);

save_GlobalAP(interp,argv[1],GlobalAP,&GCP,max_SD,alambda);
for (k=0;k<GCP.nmultset;k++) {
   free_dvector(Ith[k],0,ndata[k]-1);
   free_dvector(Isub[k],0,ndata[k]-1);
   free_dvector(h[k],0,ndata[k]-1);
   free_dvector(res[k],0,ndata[k]-1);
   free_dvector(Ih[k],0,ndata[k]-1);
   free_dvector(DIh[k],0,ndata[k]-1);
   free_dvector(DIhtrans[k],0,ndata[k]-1);
   free_dvector(Ihtrans[k],0,ndata[k]-1);
}
free_dvector(GCP.P_common,0,GCP.common_i-1);
free_ivector(GCP.P_common_index,0,GCP.common_i-1);
free_dvector(GCP.P_common_err,0,GCP.common_i-1);

Tcl_Free((char *) GlobalAP);
free_ivector(ndata,0,GCP.nmultset-1);
free_ivector(hide,0,GCP.nmultset-1);

Tcl_Free((char *) Isub);
Tcl_Free((char *) h);
Tcl_Free((char *) Ih);
Tcl_Free((char *) Ith);
Tcl_Free((char *) DIh);
Tcl_Free((char *) DIhtrans);
Tcl_Free((char *) Ihtrans);
Tcl_Free((char *) res);

return TCL_OK;
}


/*#########################################################################*/
/*#                                                                       #*/
/*# Sasfit_iqglobalfitCmd --                                              #*/
/*#                                                                       #*/
/*#      This function implements the Tcl "sasfit_iqglobalfit" command.   #*/
/*#                                                                       #*/
/*# Results:                                                              #*/
/*#      A standard Tcl result.                                           #*/
/*#                                                                       #*/
/*# Side effects:                                                         #*/
/*#      None.                                                            #*/
/*#                                                                       #*/
/*#########################################################################*/
int Sasfit_iqglobalfitCmd(SASFIT_CData, interp, argc, argv)
    struct sasfit_CData *SASFIT_CData;
    Tcl_Interp *interp;
    int        argc;
    char       **argv;
{
sasfit_analytpar *GlobalAP;
sasfit_commonpar GCP;
int    i,j,m,k;
int    max_SD;
char   sBuffer[256];
scalar  **h, **Ih, **DIh, **Ith, **Ihtrans, **DIhtrans, **Isub, **res;
int    *lista;
int    ma;
int    mfit,*ndata,*hide, totndata;
bool   error;
int    error_type;
scalar  *a;
scalar  chisq,reducedchisq,R,wR,QQ,diffR,obsR,wdiffR,wobsR;
scalar  avgsigma,varianceOFfit;
scalar  alambda;
Tcl_DString DsBuffer;

error = FALSE;
//Det2DPar.calc2D = FALSE;
sasfit_param_override_init();

if (argc != 5) {
   sasfit_err("wrong # args: shoud be sasfit_iqglobalfit ?analyt_par? ?xyer_data? ?hide?\n");
   return TCL_ERROR;
}
if (TCL_ERROR == get_GlobalAP(interp,argv,
                        &GlobalAP,&GCP,&max_SD,&alambda,&error_type,
                        &h,&Ih,&DIh,&res,&Ith,&ndata,&hide)) {
   return TCL_ERROR;
}


//sasfit_out("nmultset: %d\n",GCP.nmultset);

Isub = (scalar **) Tcl_Alloc((unsigned) (GCP.nmultset)*sizeof(scalar*));
DIhtrans = (scalar **) Tcl_Alloc((unsigned) (GCP.nmultset)*sizeof(scalar*));
Ihtrans = (scalar **) Tcl_Alloc((unsigned) (GCP.nmultset)*sizeof(scalar*));
for (k=0;k<GCP.nmultset;k++) {
	Isub[k] = dvector(0,ndata[k]-1);
	DIhtrans[k] = dvector(0,ndata[k]-1);
	Ihtrans[k] = dvector(0,ndata[k]-1);
}

for (j=0;j<max_SD;j++){
   GlobalAP[j].fit = TRUE;
   for (i=0;i<MAXPAR;i++) {
	  if (GlobalAP[j].FF_limits[i] && GlobalAP[j].FF_active[i] && GlobalAP[j].fitSDFF) {
         if (GlobalAP[j].FF_l[i] < GlobalAP[j].FF_min[i]) GlobalAP[j].FF_l[i] = GlobalAP[j].FF_min[i];
         if (GlobalAP[j].FF_l[i] > GlobalAP[j].FF_max[i]) GlobalAP[j].FF_l[i] = GlobalAP[j].FF_max[i];
         GlobalAP[j].FF_l[i] = asin((2.0 * (GlobalAP[j].FF_l[i] - GlobalAP[j].FF_min[i])
   		                           / (GlobalAP[j].FF_max[i] - GlobalAP[j].FF_min[i])
							       - 1.0));
	  }
      if (GlobalAP[j].SD_limits[i] && GlobalAP[j].SD_active[i] && GlobalAP[j].fitSDFF) {
         if (GlobalAP[j].SD_a[i] < GlobalAP[j].SD_min[i]) GlobalAP[j].SD_a[i] = GlobalAP[j].SD_min[i];
         if (GlobalAP[j].SD_a[i] > GlobalAP[j].SD_max[i]) GlobalAP[j].SD_a[i] = GlobalAP[j].SD_max[i];
         GlobalAP[j].SD_a[i] = asin((2.0 * (GlobalAP[j].SD_a[i] - GlobalAP[j].SD_min[i])
		                           / (GlobalAP[j].SD_max[i] - GlobalAP[j].SD_min[i])
							       - 1.0 ));
	  }
      if (GlobalAP[j].SQ_limits[i] && GlobalAP[j].SQ_active[i] && GlobalAP[j].fitSDFF) {
         if (GlobalAP[j].SQ_s[i] < GlobalAP[j].SQ_min[i]) GlobalAP[j].SQ_s[i] = GlobalAP[j].SQ_min[i];
         if (GlobalAP[j].SQ_s[i] > GlobalAP[j].SQ_max[i]) GlobalAP[j].SQ_s[i] = GlobalAP[j].SQ_max[i];
         GlobalAP[j].SQ_s[i] = asin((2.0 * (GlobalAP[j].SQ_s[i] - GlobalAP[j].SQ_min[i])
		                           / (GlobalAP[j].SQ_max[i] - GlobalAP[j].SQ_min[i])
							       - 1.0 ));
	  }
   }
}

sasfit_ap2paramlist(&lista,&ma,&mfit,&a,GlobalAP,&GCP,max_SD);

if (SASFIT_CData->ma == 0) {
   SASFIT_CData->lista = ivector(0,ma-1);
   SASFIT_CData->a = dvector(0,ma-1);
}
if ((SASFIT_CData->ma != 0) && (   (SASFIT_CData->mfit != mfit)
                                  || (SASFIT_CData->ma != ma    ))  ){
   free_matrix(SASFIT_CData->alpha,0,SASFIT_CData->ma-1,
                                     0,SASFIT_CData->ma-1);
   free_matrix(SASFIT_CData->covar,0,SASFIT_CData->ma-1,
                                     0,SASFIT_CData->ma-1);
   free_dvector(SASFIT_CData->da,0,SASFIT_CData->ma-1);
   free_dvector(SASFIT_CData->P_common,0,GCP.common_i-1);
   free_dvector(SASFIT_CData->atry,0,SASFIT_CData->ma-1);
   free_dvector(SASFIT_CData->beta,0,SASFIT_CData->ma-1);
   free_matrix(SASFIT_CData->oneda,0,SASFIT_CData->ma-1,0,0);
}
if (    (SASFIT_CData->ma == 0)
     || (SASFIT_CData->ma != ma)
     || (SASFIT_CData->mfit != mfit) ) {
   SASFIT_CData->alpha = matrix(0,ma-1,0,ma-1);
   SASFIT_CData->covar = matrix(0,ma-1,0,ma-1);
   SASFIT_CData->da    = dvector(0,ma-1);
   SASFIT_CData->atry  = dvector(0,ma-1);
   SASFIT_CData->P_common = dvector(0,GCP.common_i-1);
   SASFIT_CData->common_i = GCP.common_i;
   SASFIT_CData->beta  = dvector(0,ma-1);
   SASFIT_CData->oneda = matrix(0,ma-1,0,0);
}
free_ivector(SASFIT_CData->lista,0,ma-1);
free_dvector(SASFIT_CData->a,0,ma-1);
SASFIT_CData->lista = lista;
SASFIT_CData->a = a;
SASFIT_CData->ma    = ma;
SASFIT_CData->mfit  = mfit;
SASFIT_CData->max_SD = max_SD;

Tcl_ResetResult(interp);

for (i=0;i<max_SD;i++) {
	GlobalAP[i].dodydpar = 1;
}

if (sasfit_get_iq_or_gz() == 4) {
    for (j=0;j<GCP.nmultset;j++) {
        for (i=0;i<ndata[j];i++) {
            Ihtrans[j][i] = log(Ih[j][i]/Ih[j][ndata[j]-1]);
            DIhtrans[j][i] = sqrt( gsl_pow_2(DIh[j][i]/Ih[j][i]) + gsl_pow_2(DIh[j][ndata[j]-1]/Ih[j][ndata[j]-1]) );
        }
 	}
} else {
    for (j=0;j<GCP.nmultset;j++) {
        for (i=0;i<ndata[j];i++) {
            Ihtrans[j][i] = Ih[j][i];
            DIhtrans[j][i] = DIh[j][i];
        }
    }
}
SASFITmrqminGlobal(interp,h,Ihtrans,res,DIhtrans,Ith,Isub,ndata,max_SD,GlobalAP,&GCP,error_type,
       SASFIT_CData,&chisq,IQ_Global,&alambda,&error);

if (sasfit_get_iq_or_gz() == 4) {
    for (j=0;j<GCP.nmultset;j++) {
        for (i=0;i<ndata[j];i++) {
            Ith[j][i] = exp(Ith[j][i])*(Ih[j][ndata[j]-1]);
            if (Isub[j][i] == 0) {
                Isub[j][i] == 1.0;
            } else {
                Isub[j][i] = exp(Isub[j][i])*(Ih[j][ndata[j]-1]);
            }
        }
 	}
}

for (j=0;j<max_SD;j++) {
   GlobalAP[j].fit = FALSE;
   for (i=0;i<MAXPAR;i++) {
      if (GlobalAP[j].FF_limits[i] && GlobalAP[j].FF_active[i] && GlobalAP[j].fitSDFF) {
         GlobalAP[j].FF_l[i] = GlobalAP[j].FF_min[i] + 0.5*(GlobalAP[j].FF_max[i] - GlobalAP[j].FF_min[i])
                * ( sin(GlobalAP[j].FF_l[i]) + 1.0 );
	  }
      if (GlobalAP[j].SD_limits[i] && GlobalAP[j].SD_active[i] && GlobalAP[j].fitSDFF) {
         GlobalAP[j].SD_a[i] = GlobalAP[j].SD_min[i] + 0.5*(GlobalAP[j].SD_max[i] - GlobalAP[j].SD_min[i])
                * ( sin(GlobalAP[j].SD_a[i]) + 1.0 );
	  }
      if (GlobalAP[j].SQ_limits[i] && GlobalAP[j].SQ_active[i] && GlobalAP[j].fitSDFF) {
         GlobalAP[j].SQ_s[i] = GlobalAP[j].SQ_min[i] + 0.5*(GlobalAP[j].SQ_max[i] - GlobalAP[j].SQ_min[i])
                * ( sin(GlobalAP[j].SQ_s[i]) + 1.0 );
	  }
   }
}

if (error == TRUE) {
   save_GlobalAP(interp,argv[1],GlobalAP,&GCP,max_SD,alambda);
   for (k=0;k<GCP.nmultset;k++) {
      free_dvector(Ith[k],0,ndata[k]-1);
      free_dvector(Isub[k],0,ndata[k]-1);
      free_dvector(h[k],0,ndata[k]-1);
      free_dvector(res[k],0,ndata[k]-1);
      free_dvector(Ih[k],0,ndata[k]-1);
      free_dvector(DIh[k],0,ndata[k]-1);
      free_dvector(DIhtrans[k],0,ndata[k]-1);
      free_dvector(Ihtrans[k],0,ndata[k]-1);
   }
   free_dvector(GCP.P_common,0,GCP.common_i-1);
   free_ivector(GCP.P_common_index,0,GCP.common_i-1);
   free_dvector(GCP.P_common_err,0,GCP.common_i-1);
   Tcl_Free((char *) GlobalAP);
//   Tcl_Free((char *) *Isub);
//   free_dvector(dydpar,0,ma-1);
   free_ivector(ndata,0,GCP.nmultset-1);
//   Tcl_Free((char *) *h);
//   Tcl_Free((char *) *Ih);
//   Tcl_Free((char *) *Ith);
//   Tcl_Free((char *) *DIh);
//   Tcl_Free((char *) *res);
//   Tcl_Free((char *) *ndata);
//   Tcl_Free((char *) *hide);
   return TCL_ERROR;
}


sasfit_ap2paramlist(&lista,&ma,&mfit,&a,GlobalAP,&GCP,max_SD);

chisq = 0.0;
reducedchisq = 0.0;
totndata = 0.0;
mfit = 0.0;
QQ = 0.0;
R = 0.0;
wR = 0.0;
diffR = 0.0;
wdiffR = 0.0;
obsR = 0.0;
wobsR = 0.0;
avgsigma = 0.0;
varianceOFfit = 0.0;

Tcl_DStringInit(&DsBuffer);

Tcl_DStringStartSublist(&DsBuffer);
for (m=0;m<GCP.nmultset;m++) {
   Tcl_DStringStartSublist(&DsBuffer);
   for (i=0;i<ndata[m];i++) {
      float_to_string(sBuffer,Ith[m][i]);
      Tcl_DStringAppendElement(&DsBuffer,sBuffer);
	  if (DIh[m][i] == 0) {
	     sasfit_err("Sasfit_global_iqCmd: zero error bar\n");
	     return TCL_ERROR;
	  } else if (!hide[m]) {
	     chisq = chisq + pow((Ith[m][i]-Ih[m][i])/DIh[m][i],2.0);
         diffR = diffR + fabs((Ith[m][i]-Ih[m][i]));
         obsR = obsR + fabs(Ih[m][i]);
	     wdiffR=wdiffR + (Ith[m][i]-Ih[m][i])*(Ith[m][i]-Ih[m][i])/(DIh[m][i]*DIh[m][i]);
	     wobsR=wobsR + Ih[m][i]*Ih[m][i]/(DIh[m][i]*DIh[m][i]);
	     avgsigma = avgsigma/(DIh[m][i]*DIh[m][i]);
         totndata++;
      }
   }
   Tcl_DStringEndSublist(&DsBuffer);
}
Tcl_DStringEndSublist(&DsBuffer);

Tcl_DStringStartSublist(&DsBuffer);
for (j=0;j<GCP.nmultset;j++) {
	Tcl_DStringStartSublist(&DsBuffer);
	for (i=0;i<ndata[j];i++) {
       if (((sasfit_get_iq_or_gz() == 1) || (sasfit_get_iq_or_gz() == 4)) && Isub[j][i]!=0) {
           float_to_string(sBuffer,Ith[j][i]/Isub[j][i]);
       } else {
           float_to_string(sBuffer,Ith[j][i]-Isub[j][i]);
       }
       Tcl_DStringAppendElement(&DsBuffer,sBuffer);
 	}
	Tcl_DStringEndSublist(&DsBuffer);
}
Tcl_DStringEndSublist(&DsBuffer);

Tcl_DStringStartSublist(&DsBuffer);
for (j=0;j<GCP.nmultset;j++) {
	Tcl_DStringStartSublist(&DsBuffer);
	for (i=0;i<ndata[j];i++) {
       if (((sasfit_get_iq_or_gz() == 1) || (sasfit_get_iq_or_gz() == 4)) && (Isub[j][i] !=0)){
           float_to_string(sBuffer,Ih[j][i]/Isub[j][i]);
       } else {
           float_to_string(sBuffer,Ih[j][i]-Isub[j][i]);
       }
       Tcl_DStringAppendElement(&DsBuffer,sBuffer);
 	}
	Tcl_DStringEndSublist(&DsBuffer);
}
Tcl_DStringEndSublist(&DsBuffer);


Tcl_DStringStartSublist(&DsBuffer);
for (j=0;j<GCP.nmultset;j++) {
	Tcl_DStringStartSublist(&DsBuffer);
	for (i=0;i<ndata[j];i++) {
   	   float_to_string(sBuffer,DIh[j][i]);
       Tcl_DStringAppendElement(&DsBuffer,sBuffer);
 	}
	Tcl_DStringEndSublist(&DsBuffer);
}
Tcl_DStringEndSublist(&DsBuffer);


Tcl_DStringStartSublist(&DsBuffer);
for (m=0;m<GCP.nmultset;m++) {
   Tcl_DStringStartSublist(&DsBuffer);
   for (i=0;i<ndata[m];i++) {
	  if (DIh[m][i] == 0) {
	     sasfit_err("Sasfit_iqglobalfitCmd: zero error bar\n");
	     return TCL_ERROR;
	  }
      float_to_string(sBuffer,(Ith[m][i]-Ih[m][i])/DIh[m][i]);
      Tcl_DStringAppendElement(&DsBuffer,sBuffer);
   }
   Tcl_DStringEndSublist(&DsBuffer);
}
Tcl_DStringEndSublist(&DsBuffer);


Tcl_DStringResult(interp,&DsBuffer);
Tcl_DStringFree(&DsBuffer);

float_to_string(sBuffer,chisq);
Tcl_SetVar2(interp,argv[1],"chisq", sBuffer,0);
if ((totndata-mfit) !=0) {
	reducedchisq = chisq/(totndata-mfit);
}
float_to_string(sBuffer,reducedchisq);
Tcl_SetVar2(interp,argv[1],"reducedchisq", sBuffer,0);

if (obsR == 0) {
        float_to_string(sBuffer,0);
    } else {
        float_to_string(sBuffer,diffR/obsR);
    }
Tcl_SetVar2(interp,argv[1],"R", sBuffer,0);

if (wobsR == 0) {
        float_to_string(sBuffer,0);
    } else {
        float_to_string(sBuffer,sqrt(wdiffR/wobsR));
    }
Tcl_SetVar2(interp,argv[1],"wR", sBuffer,0);

QQ=gsl_sf_gamma_inc_Q((totndata-mfit)/2.0,chisq/2.0);
float_to_string(sBuffer,QQ);
Tcl_SetVar2(interp,argv[1],"Q", sBuffer,0);
if (avgsigma > 0) varianceOFfit=reducedchisq*totndata/avgsigma;
float_to_string(sBuffer,varianceOFfit);
Tcl_SetVar2(interp,argv[1],"varianceOFfit", sBuffer,0);

save_GlobalAP(interp,argv[1],GlobalAP,&GCP,max_SD,alambda);
for (k=0;k<GCP.nmultset;k++) {
   free_dvector(Ith[k],0,ndata[k]-1);
   free_dvector(Isub[k],0,ndata[k]-1);
   free_dvector(h[k],0,ndata[k]-1);
   free_dvector(res[k],0,ndata[k]-1);
   free_dvector(Ih[k],0,ndata[k]-1);
   free_dvector(DIh[k],0,ndata[k]-1);
   free_dvector(DIhtrans[k],0,ndata[k]-1);
   free_dvector(Ihtrans[k],0,ndata[k]-1);
}
free_dvector(GCP.P_common,0,GCP.common_i-1);
free_ivector(GCP.P_common_index,0,GCP.common_i-1);
free_dvector(GCP.P_common_err,0,GCP.common_i-1);

Tcl_Free((char *) GlobalAP);
//free_ivector(ndata,0,GCP.nmultset-1);
//free_ivector(hide,0,GCP.nmultset-1);

Tcl_Free((char *) Isub);
Tcl_Free((char *) h);
Tcl_Free((char *) Ih);
Tcl_Free((char *) Ith);
Tcl_Free((char *) DIh);
Tcl_Free((char *) DIhtrans);
Tcl_Free((char *) Ihtrans);
Tcl_Free((char *) res);
Tcl_Free((char *) ndata);
Tcl_Free((char *) hide);

return TCL_OK;
}




/*#########################################################################*/
/*#                                                                       #*/
/*# Sasfit_resCmd --                                                      #*/
/*#                                                                       #*/
/*#      This function implements the Tcl "sasfit_res" command.           #*/
/*#                                                                       #*/
/*# Results:                                                              #*/
/*#      A standard Tcl result.                                           #*/
/*#                                                                       #*/
/*# Side effects:                                                         #*/
/*#      None.                                                            #*/
/*#                                                                       #*/
/*#########################################################################*/

int Sasfit_resCmd(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int        argc;
    char       **argv;
{
	bool  error;
	scalar Dd, D, Dlam, lam, r1, r2, L, l;
	scalar *Q;
	int   ndata;
	int   Splitcode, tcode, targc, j;
    const char  **targv;
    char sBuffer[256];
    Tcl_DString DsBuffer;

    error = FALSE;
    if (argc != 10) {
       sasfit_err("wrong # args: shoud be sasfit_res ?lambda? ?delta_lambda? ?n-guide ap.?\n                                  ?coll? ?sample ap.? ?sample-detector?\n                                  ?det. pixel? ?rad. av. step? ?Q-data?\n");
       return TCL_ERROR;
    }


    if (TCL_ERROR == Tcl_GetDouble(interp,argv[1],&lam) ) {
       sasfit_err("could not read lambda\n");
       return TCL_ERROR;
    }

	if (TCL_ERROR == Tcl_GetDouble(interp,argv[2],&Dlam) ) {
       sasfit_err("could not read delta_lambda\n");
       return TCL_ERROR;
    }

    if (TCL_ERROR == Tcl_GetDouble(interp,argv[3],&r1) ) {
       sasfit_err("could not read n-guide aperture size\n");
       return TCL_ERROR;
    }

    if (TCL_ERROR == Tcl_GetDouble(interp,argv[4],&L) ) {
       sasfit_err("could not read collimation length\n");
       return TCL_ERROR;
    }

	if (TCL_ERROR == Tcl_GetDouble(interp,argv[5],&r2) ) {
       sasfit_err("could not read sample aperture size\n");
       return TCL_ERROR;
    }

    if (TCL_ERROR == Tcl_GetDouble(interp,argv[6],&l) ) {
       sasfit_err("could not read sample detector distance\n");
       return TCL_ERROR;
    }

    if (TCL_ERROR == Tcl_GetDouble(interp,argv[7],&Dd) ) {
       sasfit_err("could not read pixel size of detector\n");
       return TCL_ERROR;
    }

    if (TCL_ERROR == Tcl_GetDouble(interp,argv[8],&D) ) {
       sasfit_err("could not read radial averaging step size\n");
       return TCL_ERROR;
    }

    Splitcode = Tcl_SplitList(interp,argv[9],&targc,&targv);
    if (Splitcode == TCL_ERROR) {
       Tcl_Free((char *) targv);
       sasfit_err("could not read Q\n");
       return TCL_ERROR;
    }
    ndata = targc;
    Q = dvector(0,ndata-1);
    for (j=0;j<targc;j++) {
       tcode = Tcl_GetDouble(interp,targv[j],&Q[j]);
       if (tcode == TCL_ERROR) {
          free_dvector(Q,0,ndata-1);
          Tcl_Free((char *) targv);
          sasfit_err("could not convert Q[j] to double\n");
          return TCL_ERROR;
       }
    }
Tcl_ResetResult(interp);

Tcl_DStringInit(&DsBuffer);
for(j=0;j<ndata;j++) {
   float_to_string(sBuffer,sigma_t(lam,Dlam,r1,L,r2,l,Dd,D,Q[j]));
   Tcl_DStringAppendElement(&DsBuffer,sBuffer);

}
free_dvector(Q,0,ndata-1);
Tcl_DStringResult(interp,&DsBuffer);
/*
sasfit_err(Tcl_DStringValue(&DsBuffer));
*/
Tcl_DStringFree(&DsBuffer);

return TCL_OK;
}

/*#########################################################################*/
/*#                                                                       #*/
/*# Sasfit_nrGlobalCmd --                                                       #*/
/*#                                                                       #*/
/*#      This function implements the Tcl "sasfit_nr" command.            #*/
/*#                                                                       #*/
/*# Results:                                                              #*/
/*#      A standard Tcl result.                                           #*/
/*#                                                                       #*/
/*# Side effects:                                                         #*/
/*#      None.                                                            #*/
/*#                                                                       #*/
/*#########################################################################*/

int Sasfit_nrGlobalCmd(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int        argc;
    const char  **argv;
{
}

/*#########################################################################*/
/*#                                                                       #*/
/*# Sasfit_nrCmd --                                                       #*/
/*#                                                                       #*/
/*#      This function implements the Tcl "sasfit_nr" command.            #*/
/*#                                                                       #*/
/*# Results:                                                              #*/
/*#      A standard Tcl result.                                           #*/
/*#                                                                       #*/
/*# Side effects:                                                         #*/
/*#      None.                                                            #*/
/*#                                                                       #*/
/*#########################################################################*/

int Sasfit_nrCmd(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int        argc;
    const char       **argv;
{
	sasfit_analytpar *AP;
	int    i, j, k, distr;
	int    max_SD;
	scalar  alambda;
	char   sBuffer[256];
	Tcl_DString DsBuffer;
	scalar  *h, *Ih, *DIh, NR, NRprevious, tmpNR, *res;
	int    ndata;
	bool   error, Verror;
	int    error_type;
	scalar  *Rstart_SD, *Rend_SD, *dR_SD;
	int    *n_intervals_SD, nSD;
	scalar  Rstart,Rend,dR;
	scalar  tmp_Rstart,tmp_Rend,tmp_dR;
	scalar  Rstart_total, Rend_total, dR_total;
	int    tmp_n_intervals,n_intervals_total,n_intervals;

	error = FALSE;
	Verror = FALSE;
	if (argc != 3) {
		sasfit_err("wrong # args: shoud be sasfit_nr ?analyt_par? ?xye-data?\n");
		return TCL_ERROR;
	}
	if (TCL_ERROR == get_AP(interp,argv,&AP,&max_SD,&alambda,&error_type,&h,&Ih,&DIh,&res,&ndata))
	{
		sasfit_err("get_AP failed.");
		return TCL_ERROR;
	}
	for (j=0;j<max_SD;j++){
		AP[j].fit = FALSE;
	}

	free_dvector(h,0,ndata-1);
	free_dvector(res,0,ndata-1);
	free_dvector(Ih,0,ndata-1);
	free_dvector(DIh,0,ndata-1);

	Rstart = 0.0;
	Rend   = 0.0;
	Rend_total = 0.0;
	Rstart_total = 0.0;
	dR = 0.0;
	Rstart_SD = dvector(0,max_SD-1);
	Rend_SD   = dvector(0,max_SD-1);
	dR_SD     = dvector(0,max_SD-1);
	n_intervals_SD = ivector(0,max_SD-1);
	n_intervals = 1;
	tmp_n_intervals = 1;
	n_intervals_total = 500;
	for (i=0;i<max_SD;i++) {
		find_integration_range(interp,&tmp_Rstart, &tmp_Rend, &tmp_n_intervals,AP[i].SD_a,AP[i].SD.typestr,6,&error);
		if ((error == FALSE) && (tmp_n_intervals > 1) )
		{
			dR = (Rend - Rstart) / n_intervals;
			n_intervals_SD[i] = tmp_n_intervals;
			dR_SD[i]     = (tmp_Rend - tmp_Rstart)/tmp_n_intervals;
			for (j=0;j<=MAXPAR;j++) AP[i].R_moments[j] = 0.0;
			Rstart_SD[i] = tmp_Rstart;
			Rend_SD[i]   = tmp_Rend;
			if (Rstart > tmp_Rstart) {
				Rstart = tmp_Rstart;
				Rstart_total = tmp_Rstart;
			}
			if (Rend < tmp_Rend){
				Rend = tmp_Rend;
				Rend_total = tmp_Rend;
			}
			tmp_dR = ((tmp_Rend - tmp_Rstart)/tmp_n_intervals);
			if ( (tmp_dR < dR) || (dR == 0.0)) {
				dR = tmp_dR;
			}
			n_intervals = (int) ceil((Rend-Rstart)/dR);
		}
	}
	dR_total = (Rend_total-Rstart_total)/n_intervals_total;
	if (error == TRUE) {
		Tcl_Free((char *) AP);
		sasfit_err("#sasfit_nr: couldn't calculate size distribution >%s<\n", AP[i].SD.typestr);
		free_dvector(Rstart_SD,0,max_SD-1);
		free_dvector(Rend_SD,0,max_SD-1);
		free_dvector(dR_SD,0,max_SD-1);
		free_ivector(n_intervals_SD,0,max_SD-1);
		return TCL_ERROR;
	}

	Tcl_ResetResult(interp);
	Tcl_DStringInit(&DsBuffer);

/*
	Tcl_DStringEndSublist(&DsBuffer);
*/
	nSD = 0;
	for (j=0;j<max_SD;j++) {
		if ( !((strncmp(AP[j].SD.typestr,"Delta",5) == 0) ||
		       (strncmp(AP[j].SD.typestr,"d Delta",7) == 0) ||
		       (strncmp(AP[j].SD.typestr,"Monodisperse",10) == 0)) && AP[j].calcSDFF)
		{
			nSD++;
		}
	}

	// prepend the combination of all distributions
	if (nSD >= 1) {
		Tcl_DStringStartSublist(&DsBuffer);
		Tcl_DStringStartSublist(&DsBuffer);
		for(i=0;i<=n_intervals_total;i++) {
			float_to_string(sBuffer,Rstart_total+i*dR_total);
			Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
		Tcl_DStringEndSublist(&DsBuffer);
		Tcl_DStringStartSublist(&DsBuffer);
		NR = 0.0;
		tmpNR = 0.0;
		for(i=0;i<=n_intervals_total;i++) {
			NR = 0.0;
			for (j=0;j<max_SD;j++) {
				if ( !((strncmp(AP[j].SD.typestr,"Delta",5) == 0) ||
				       (strncmp(AP[j].SD.typestr,"d Delta",7) == 0) ||
				       (strncmp(AP[j].SD.typestr,"Monodisperse",10) == 0)) && AP[j].calcSDFF)
				{
					tmpNR = sasfit_sd(Rstart_total+dR_total*i,AP[j].SD_a, &AP[j].SD, NULL, &error);
					if (error) {
						Tcl_Free((char *) AP);
						Tcl_DStringFree(&DsBuffer);
						sasfit_err("#sasfit_nr: couldn't calculate size distribution\n");
						free_dvector(Rstart_SD,0,max_SD-1);
						free_dvector(Rend_SD,0,max_SD-1);
						free_dvector(dR_SD,0,max_SD-1);
						free_ivector(n_intervals_SD,0,max_SD-1);
						return TCL_ERROR;
					}
					NR = NR + tmpNR;
				}
			}
			if (fabs(NR) < 1.0e-200) {tmpNR = 0.0;} else {tmpNR=NR;}
			float_to_string(sBuffer,tmpNR);
			Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
		Tcl_DStringEndSublist(&DsBuffer);
		Tcl_DStringEndSublist(&DsBuffer);
	}

	// add each distribution to the list we return
	for (j=0;j<max_SD;j++) {
		AP[j].fp = 0;
		if ( !((strncmp(AP[j].SD.typestr,"Delta",5) == 0) ||
		       (strncmp(AP[j].SD.typestr,"d Delta",7) == 0) ||
		       (strncmp(AP[j].SD.typestr,"Monodisperse",10) == 0))  && AP[j].calcSDFF)
		{
			Tcl_DStringStartSublist(&DsBuffer);
			Tcl_DStringStartSublist(&DsBuffer);
			for(i=0;i<=n_intervals_SD[j];i++) {
				float_to_string(sBuffer,Rstart_SD[j]+i*dR_SD[j]);
				Tcl_DStringAppendElement(&DsBuffer,sBuffer);
			}
			Tcl_DStringEndSublist(&DsBuffer);
			Tcl_DStringStartSublist(&DsBuffer);
			NRprevious = 0.0;
			distr = -1;
			for (k=0;k<MAXPAR;k++) {
				if (AP[j].FF_distr[k] == 1) { distr = k; }
			}
			for(i=0;i<=n_intervals_SD[j];i++) {
				NR = sasfit_sd(Rstart_SD[j]+dR_SD[j]*i ,AP[j].SD_a, &AP[j].SD, NULL, &error);
				if (i>0) {
					for (k=0;k<=8;k++) {
						AP[j].R_moments[k] = AP[j].R_moments[k] + (
							NRprevious*pow(Rstart_SD[j]+dR_SD[j]*(i-1.0),k) +
								NR*pow(Rstart_SD[j]+dR_SD[j]* i    , k)) *
							dR_SD[j]/2.0;
					}
				}
				AP[j].fp = AP[j].fp + (
					NRprevious*sasfit_volume(Rstart_SD[j]+dR_SD[j]*(i-1.0),AP[j].FF_l,&AP[j].FF,distr,&Verror) +
						NR*sasfit_volume(Rstart_SD[j]+dR_SD[j]*i      ,AP[j].FF_l,&AP[j].FF,distr,&Verror)) *
					dR_SD[j]/2.0 * 1.e21;
				if (Verror) {
					AP[j].fp = 0.0;
				}
				NRprevious = NR;
				if (error) {
					Tcl_Free((char *) AP);
					Tcl_DStringFree(&DsBuffer);
					sasfit_err("#sasfit_nr: couldn't calculate size distribution\n");
					free_dvector(Rstart_SD,0,max_SD-1);
					free_dvector(Rend_SD,0,max_SD-1);
					free_dvector(dR_SD,0,max_SD-1);
					free_ivector(n_intervals_SD,0,max_SD-1);
					return TCL_ERROR;
				}
				if (fabs(NR) < 1.0e-200) {tmpNR = 0.0;} else {tmpNR=NR;}
				float_to_string(sBuffer,tmpNR);
				Tcl_DStringAppendElement(&DsBuffer,sBuffer);
			}
			Tcl_DStringEndSublist(&DsBuffer);
			Tcl_DStringEndSublist(&DsBuffer);
		} // if not Delta or monodisperse
	} // for each SD in max_SD

	save_AP(interp,argv[1],AP,max_SD,alambda);
	Tcl_DStringResult(interp,&DsBuffer);

	Tcl_Free((char *) AP);
	Tcl_DStringFree(&DsBuffer);
	free_dvector(Rstart_SD,0,max_SD-1);
	free_dvector(Rend_SD,0,max_SD-1);
	free_dvector(dR_SD,0,max_SD-1);
	free_ivector(n_intervals_SD,0,max_SD-1);
	return TCL_OK;
}

/*#########################################################################*/
/*#                                                                       #*/
/*# Sasfit_iqfitCmd --                                                    #*/
/*#                                                                       #*/
/*#      This function implements the Tcl "sasfit_iqfit" command.         #*/
/*#                                                                       #*/
/*# Results:                                                              #*/
/*#      A standard Tcl result.                                           #*/
/*#                                                                       #*/
/*# Side effects:                                                         #*/
/*#      None.                                                            #*/
/*#                                                                       #*/
/*#########################################################################*/
int Sasfit_iqfitCmd(struct sasfit_CData *SASFIT_CData, Tcl_Interp *interp, int argc, const char **argv)
{
sasfit_analytpar *AP;
int    i,j;
int    max_SD;
char   sBuffer[256];
scalar  *h, *Ih, *DIh, *Ith, *Ihsub, *res, *Ihtrans, *DIhtrans;
int    *lista;
int    ma;
int    mfit,ndata;
bool   error;
int    error_type;
scalar  *a;
scalar  chisq,reducedchisq,R,wR,QQ,diffR,obsR,wdiffR,wobsR;
scalar  avgsigma,varianceOFfit;
scalar  alambda;
Tcl_DString DsBuffer;

ndata = 0.0;
mfit = 0.0;

error = FALSE;
//Det2DPar.calc2D = FALSE;
sasfit_param_override_init();

if (argc != 4) {
   sasfit_err("wrong # args: shoud be sasfit_iqfit ?analyt_par? ?xyer_data?\n");
   return TCL_ERROR;
}
if (TCL_ERROR == get_AP(interp,argv,
                        &AP,&max_SD,&alambda,&error_type,
                        &h,&Ih,&DIh,&res,&ndata)) {
   return TCL_ERROR;
}

for (j=0;j<max_SD;j++){
   AP[j].fit = TRUE;
   // FIXME: Substitution doesn't work if upper limit stays at +1e200 and
   // lower limit is supposed to be 0 -> asin: [-1.57; 1.57], fixed in gui
   for (i=0;i<MAXPAR;i++) {
	  if (AP[j].FF_limits[i] && AP[j].FF_active[i] && AP[j].fitSDFF) {
         if (AP[j].FF_l[i] < AP[j].FF_min[i]) AP[j].FF_l[i] = AP[j].FF_min[i];
         if (AP[j].FF_l[i] > AP[j].FF_max[i]) AP[j].FF_l[i] = AP[j].FF_max[i];
         AP[j].FF_l[i] = asin((2.0 * (AP[j].FF_l[i] - AP[j].FF_min[i])
   		                           / (AP[j].FF_max[i] - AP[j].FF_min[i])
							       - 1.0));
	  }
      if (AP[j].SD_limits[i] && AP[j].SD_active[i] && AP[j].fitSDFF) {
         if (AP[j].SD_a[i] < AP[j].SD_min[i]) AP[j].SD_a[i] = AP[j].SD_min[i];
         if (AP[j].SD_a[i] > AP[j].SD_max[i]) AP[j].SD_a[i] = AP[j].SD_max[i];
         AP[j].SD_a[i] = asin((2.0 * (AP[j].SD_a[i] - AP[j].SD_min[i])
		                           / (AP[j].SD_max[i] - AP[j].SD_min[i])
							       - 1.0 ));
	  }
      if (AP[j].SQ_limits[i] && AP[j].SQ_active[i] && AP[j].fitSDFF) {
         if (AP[j].SQ_s[i] < AP[j].SQ_min[i]) AP[j].SQ_s[i] = AP[j].SQ_min[i];
         if (AP[j].SQ_s[i] > AP[j].SQ_max[i]) AP[j].SQ_s[i] = AP[j].SQ_max[i];
         AP[j].SQ_s[i] = asin((2.0 * (AP[j].SQ_s[i] - AP[j].SQ_min[i])
		                           / (AP[j].SQ_max[i] - AP[j].SQ_min[i])
							       - 1.0 ));
	  }
   }
}

sasfit_ap2paramlist(&lista,&ma,&mfit,&a,AP,NULL,max_SD);

if (SASFIT_CData->ma == 0) {
   SASFIT_CData->lista = ivector(0,ma-1);
   SASFIT_CData->a = dvector(0,ma-1);
}
if ((SASFIT_CData->ma != 0) && (   (SASFIT_CData->mfit != mfit)
                                  || (SASFIT_CData->ma != ma    ))  ){
   free_matrix(SASFIT_CData->alpha,0,SASFIT_CData->ma-1,
                                     0,SASFIT_CData->ma-1);
   free_matrix(SASFIT_CData->covar,0,SASFIT_CData->ma-1,
                                     0,SASFIT_CData->ma-1);
   free_dvector(SASFIT_CData->da,0,SASFIT_CData->ma-1);
   free_dvector(SASFIT_CData->atry,0,SASFIT_CData->ma-1);
   free_dvector(SASFIT_CData->beta,0,SASFIT_CData->ma-1);
   free_matrix(SASFIT_CData->oneda,0,SASFIT_CData->ma-1,0,0);
}
if (    (SASFIT_CData->ma == 0)
     || (SASFIT_CData->ma != ma)
     || (SASFIT_CData->mfit != mfit) ) {
   SASFIT_CData->alpha = matrix(0,ma-1,0,ma-1);
   SASFIT_CData->covar = matrix(0,ma-1,0,ma-1);
   SASFIT_CData->da    = dvector(0,ma-1);
   SASFIT_CData->atry  = dvector(0,ma-1);
   SASFIT_CData->beta  = dvector(0,ma-1);
   SASFIT_CData->oneda = matrix(0,ma-1,0,0);
}
free_ivector(SASFIT_CData->lista,0,ma-1);
free_dvector(SASFIT_CData->a,0,ma-1);
SASFIT_CData->lista = lista;
SASFIT_CData->a = a;
SASFIT_CData->ma    = ma;
SASFIT_CData->mfit  = mfit;
SASFIT_CData->max_SD = max_SD;

Ith = dvector(0,ndata-1);
Ihsub = dvector(0,ndata-1);
Ihtrans = dvector(0,ndata-1);
DIhtrans = dvector(0,ndata-1);
Tcl_ResetResult(interp);

if (sasfit_get_iq_or_gz() == 4) {
   for (i=0;i<ndata;i++) {
        Ihtrans[i] = log(Ih[i]/Ih[ndata-1]);
        DIhtrans[i] = sqrt( gsl_pow_2(DIh[i]/Ih[i]) + gsl_pow_2(DIh[ndata-1]/Ih[ndata-1]) );
    }
} else {
    for (i=0;i<ndata;i++) {
        Ihtrans[i] = Ih[i];
        DIhtrans[i] = DIh[i];
    }
}
SASFITmrqmin(interp,h,Ihtrans,res,DIhtrans,Ith,Ihsub,ndata,max_SD,AP,error_type,
       SASFIT_CData,&chisq,IQ,&alambda,&error);
if (sasfit_get_iq_or_gz() == 4) {
   for (i=0;i<ndata;i++) {
            Ith[i] = exp(Ith[i])*(Ih[ndata-1]);
            if (Ihsub[i] == 0) {
                Ihsub[i] == 1.0;
            } else {
                Ihsub[i] = exp(Ihsub[i])*(Ih[ndata-1]);
            }
    }
} else {
    for (i=0;i<ndata;i++) {
        Ihtrans[i] = Ih[i];
        DIhtrans[i] = DIh[i];
    }
}

for (j=0;j<max_SD;j++) {
   AP[j].fit = FALSE;
   for (i=0;i<MAXPAR;i++) {
      if (AP[j].FF_limits[i] && AP[j].FF_active[i] && AP[j].fitSDFF) {
         AP[j].FF_l[i] = AP[j].FF_min[i] + 0.5*(AP[j].FF_max[i] - AP[j].FF_min[i])
                * ( sin(AP[j].FF_l[i]) + 1.0 );
	  }
      if (AP[j].SD_limits[i] && AP[j].SD_active[i] && AP[j].fitSDFF) {
         AP[j].SD_a[i] = AP[j].SD_min[i] + 0.5*(AP[j].SD_max[i] - AP[j].SD_min[i])
                * ( sin(AP[j].SD_a[i]) + 1.0 );
	  }
      if (AP[j].SQ_limits[i] && AP[j].SQ_active[i] && AP[j].fitSDFF) {
         AP[j].SQ_s[i] = AP[j].SQ_min[i] + 0.5*(AP[j].SQ_max[i] - AP[j].SQ_min[i])
                * ( sin(AP[j].SQ_s[i]) + 1.0 );
	  }
   }
}

if (error == TRUE) {
   save_AP(interp,argv[1],AP,max_SD,alambda);
   free_dvector(Ith,0,ndata-1);
   free_dvector(Ihsub,0,ndata-1);
   free_dvector(h,0,ndata-1);
   free_dvector(res,0,ndata-1);
   free_dvector(Ih,0,ndata-1);
   free_dvector(DIh,0,ndata-1);
   free_dvector(Ihtrans,0,ndata-1);
   free_dvector(DIhtrans,0,ndata-1);
   return TCL_ERROR;
}

sasfit_ap2paramlist(&lista,&ma,&mfit,&a,AP,NULL,max_SD);


Tcl_DStringInit(&DsBuffer);

Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<ndata;i++) {
   float_to_string(sBuffer,Ith[i]);
   Tcl_DStringAppendElement(&DsBuffer,sBuffer);
}
Tcl_DStringEndSublist(&DsBuffer);

Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<ndata;i++) {
    if (((sasfit_get_iq_or_gz() == 1) || (sasfit_get_iq_or_gz() == 4)) && (Ihsub[i]!=0)) {
        float_to_string(sBuffer,Ih[i]/Ihsub[i]);
    } else {
        float_to_string(sBuffer,Ih[i]-Ihsub[i]);
    }
   Tcl_DStringAppendElement(&DsBuffer,sBuffer);
}
Tcl_DStringEndSublist(&DsBuffer);


Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<ndata;i++) {
    if (((sasfit_get_iq_or_gz() == 1) || (sasfit_get_iq_or_gz() == 4)) && (Ihsub[i]!=0)) {
        float_to_string(sBuffer,Ith[i]/Ihsub[i]);
    } else {
        float_to_string(sBuffer,Ith[i]-Ihsub[i]);
    }
   Tcl_DStringAppendElement(&DsBuffer,sBuffer);
}
Tcl_DStringEndSublist(&DsBuffer);

	chisq = 0.0;
	reducedchisq = 0.0;
    QQ = 0.0;
    R = 0.0;
    wR = 0.0;
    diffR = 0.0;
    wdiffR = 0.0;
    obsR = 0.0;
    wobsR = 0.0;
    avgsigma = 0.0;
    varianceOFfit = 0.0;

Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<ndata;i++) {
   if (DIh[i] == 0.0) {
      free_dvector(Ith,0,ndata-1);
      free_dvector(h,0,ndata-1);
      free_dvector(res,0,ndata-1);
      free_dvector(Ih,0,ndata-1);
      free_dvector(DIh,0,ndata-1);
      free_dvector(Ihtrans,0,ndata-1);
      free_dvector(DIhtrans,0,ndata-1);
//      free_dvector(dydpar,0,ma-1);
      Tcl_Free((char *) AP);
      return TCL_ERROR;
   }
   float_to_string(sBuffer,(Ih[i]-Ith[i])/DIh[i]);
   Tcl_DStringAppendElement(&DsBuffer,sBuffer);
   chisq = chisq+pow((Ih[i]-Ith[i])/DIh[i],2.0);
   wobsR = wobsR + Ih[i]*Ih[i]/(DIh[i]*DIh[i]);
   avgsigma = avgsigma + 1.0/(DIh[i]*DIh[i]);
   diffR = diffR + fabs((Ith[i]-Ih[i]));
   obsR = obsR + fabs(Ih[i]);
   wdiffR=chisq;
}
Tcl_DStringEndSublist(&DsBuffer);

Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<ndata;i++) {
   float_to_string(sBuffer,DIh[i]);
   Tcl_DStringAppendElement(&DsBuffer,sBuffer);
}
Tcl_DStringEndSublist(&DsBuffer);

Tcl_DStringResult(interp,&DsBuffer);


save_AP(interp,argv[1],AP,max_SD,alambda);

//float_to_string(sBuffer,SASFIT_CData->chisq/(ndata-mfit));
//Tcl_SetVar2(interp,argv[1],"chisq", sBuffer,0);


if (alambda !=0) {
int_to_string(sBuffer,mfit);
Tcl_SetVar2(interp,argv[1],"mfit", sBuffer,0);
int_to_string(sBuffer,ndata);
Tcl_SetVar2(interp,argv[1],"ndata", sBuffer,0);
float_to_string(sBuffer,chisq);
Tcl_SetVar2(interp,argv[1],"chisq", sBuffer,0);

if ((ndata-mfit) !=0) {
	reducedchisq = chisq/(ndata-mfit);
}
float_to_string(sBuffer,reducedchisq);
Tcl_SetVar2(interp,argv[1],"reducedchisq", sBuffer,0);

if (obsR == 0) {
        float_to_string(sBuffer,0);
    } else {
        float_to_string(sBuffer,diffR/obsR);
    }
Tcl_SetVar2(interp,argv[1],"R", sBuffer,0);

if (wobsR == 0) {
        float_to_string(sBuffer,0);
    } else {
        float_to_string(sBuffer,sqrt(wdiffR/wobsR));
    }
Tcl_SetVar2(interp,argv[1],"wR", sBuffer,0);

QQ=gsl_sf_gamma_inc_Q((ndata-mfit)/2.,chisq/2.);
float_to_string(sBuffer,QQ);
Tcl_SetVar2(interp,argv[1],"Q", sBuffer,0);
if (avgsigma>0) varianceOFfit=reducedchisq*ndata/avgsigma;
float_to_string(sBuffer,varianceOFfit);
Tcl_SetVar2(interp,argv[1],"varianceOFfit", sBuffer,0);

}

free_dvector(Ith,0,ndata-1);
free_dvector(h,0,ndata-1);
free_dvector(res,0,ndata-1);
free_dvector(Ih,0,ndata-1);
free_dvector(DIh,0,ndata-1);
free_dvector(Ihtrans,0,ndata-1);
free_dvector(DIhtrans,0,ndata-1);
Tcl_Free((char *) AP);
return TCL_OK;
}


/*#########################################################################*/
/*#                                                                       #*/
/*# Sasfit_guess_errCmd --                                                #*/
/*#                                                                       #*/
/*#      This function implements the Tcl "sasfit_guess_err" command.     #*/
/*#                                                                       #*/
/*# Results:                                                              #*/
/*#      A standard Tcl result.                                           #*/
/*#                                                                       #*/
/*# Side effects:                                                         #*/
/*#      None.                                                            #*/
/*#                                                                       #*/
/*#########################################################################*/
int Sasfit_guess_errCmd(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int        argc;
    char       **argv;
{
int    i,j,k;
int    sw, po;
char   sBuffer[256];
scalar  *x, *y, *err;
int    Splitcode, Splitargc_x, Splitargc_y,code;
const char   **Splitargv_x,**Splitargv_y;
int    ndata,nd,ncoeffs;
scalar chisq;
gsl_matrix *X, *cov;
gsl_vector *yd, *c,*w;
Tcl_DString DsBuffer;
int status;
gsl_multifit_linear_workspace * work ;

	Tcl_ResetResult(interp);

	if (argc != 5) {
		sasfit_err("wrong # args: shoud be sasfit_guess_err ?sw? ?po? ?xy_data?\n?2*sw+1? are the number of points to fit a polynome of the order ?po?\n");
		return TCL_ERROR;
	}


	sscanf(argv[1],"%d",&sw);
	sscanf(argv[2],"%d",&po);


	if (po < 0) {
		sasfit_err("?po? needs to be larger or equal to 0\n");
		return TCL_ERROR;
	}
	if (po >= 2*sw) {
		sasfit_err("?po? needs to be smaller than 2*?sw?\n");
		return TCL_ERROR;
	}


    Splitcode = Tcl_SplitList(interp,argv[3],&Splitargc_x,&Splitargv_x);
    if (Splitcode == TCL_ERROR) {
       sasfit_err("x data {x} is not a list\n");
       return TCL_ERROR;
    }

    Splitcode = Tcl_SplitList(interp,argv[4],&Splitargc_y,&Splitargv_y);
    if (Splitcode == TCL_ERROR) {
       sasfit_err("y data {y} is not a list\n");
       return TCL_ERROR;
    }

	if (Splitargc_x != Splitargc_y) {
       sasfit_err("y and x data list need to contain the same number of elements\n");
       Tcl_Free((char *) Splitargv_x);
       Tcl_Free((char *) Splitargv_x);
	   return TCL_ERROR;
	}

	gsl_set_error_handler_off();

	ndata = Splitargc_x;

	if (ndata < 2*sw+1) {
		sasfit_err("not enough data points available\n");
		return TCL_ERROR;
	}

    x   = dvector(0,ndata-1);
    y   = dvector(0,ndata-1);
    err = dvector(0,ndata-1);

    for (j=0;j<ndata;j++) {
        code = Tcl_GetDouble(interp,Splitargv_x[j],&x[j]);
        if (code == TCL_ERROR) {
			sasfit_err("an element in x data list is not a float number\n");
			    Tcl_Free((char *) Splitargv_x);
				Tcl_Free((char *) Splitargv_y);
				free_dvector(x,0,ndata-1);
				free_dvector(y,0,ndata-1);
				free_dvector(err,0,ndata-1);
			return TCL_ERROR;
		}
        code = Tcl_GetDouble(interp,Splitargv_y[j],&y[j]);
        if (code == TCL_ERROR) {
			sasfit_err("an element in y data list is not a float number\n");
			    Tcl_Free((char *) Splitargv_x);
				Tcl_Free((char *) Splitargv_y);
				free_dvector(x,0,ndata-1);
				free_dvector(y,0,ndata-1);
				free_dvector(err,0,ndata-1);
			return TCL_ERROR;
		}
	}
    Tcl_Free((char *) Splitargv_x);
	Tcl_Free((char *) Splitargv_y);

	nd = 2*sw+1;
	ncoeffs = po+1;
	X = gsl_matrix_alloc (nd,ncoeffs);
	yd = gsl_vector_alloc (nd);
	w = gsl_vector_alloc (nd);

	c	 = gsl_vector_alloc(ncoeffs);
	cov  = gsl_matrix_alloc(ncoeffs,ncoeffs);

	work = gsl_multifit_linear_alloc(nd,ncoeffs);


	for (i=sw;i<=ndata-sw-1;++i) {
		for (j=i-sw;j<=i+sw;++j) {
			for (k=0;k<=po;++k) {
				gsl_matrix_set (X, j-i+sw, k, pow(x[j],k) );
			}
			gsl_vector_set (yd,j-i+sw, y[j]);
			gsl_vector_set (w, j-i+sw, 1.0 );
		}

	    status = gsl_multifit_wlinear (X, w, yd, c, cov, &chisq, work);
		if (status) {
			gsl_multifit_linear_free (work);
			gsl_vector_free(c);
			gsl_matrix_free(cov);
			gsl_matrix_free(X);
			gsl_vector_free(yd);
			gsl_vector_free(w);
			free_dvector(x,0,ndata-1);
			free_dvector(y,0,ndata-1);
			free_dvector(err,0,ndata-1);
			return TCL_ERROR;
		}
		err[i] =  sqrt(chisq/(2*sw+1-(po+1)));
	}
	for (i=ndata-1;	i>=ndata-sw	;i--) err[i] = err[ndata-sw-1];
    for (i=0;		i<=sw-1		;i++) err[i] = err[sw];

    gsl_multifit_linear_free (work);
	gsl_vector_free(c);
	gsl_matrix_free(cov);
	gsl_matrix_free(X);
	gsl_vector_free(yd);
	gsl_vector_free(w);



//SASFITmrqmin(interp,h,Ih,res,DIh,Ith,Ihsub,ndata,max_SD,AP,error_type,
//       SASFIT_CData,&chisq,IQ,&alambda,&error);


Tcl_DStringInit(&DsBuffer);

//Tcl_DStringStartSublist(&DsBuffer);
for (i=0;i<ndata;i++) {
	sprintf(sBuffer,"%lg",err[i]);
   Tcl_DStringAppendElement(&DsBuffer,sBuffer);
}
//Tcl_DStringEndSublist(&DsBuffer);


Tcl_DStringResult(interp,&DsBuffer);


free_dvector(x,0,ndata-1);
free_dvector(y,0,ndata-1);
free_dvector(err,0,ndata-1);
return TCL_OK;
}


