/*
 * src/sasfit_old/SASFIT_x_tcl.c
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
/*# Written by Joachim Kohlbrecher                                        #*/
/*#########################################################################*/
/*#     Version Vb1.0 12.12.2004  Joachim Kohlbrecher                     #*/
/*#########################################################################*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tcl.h>

#include "include/SASFIT_nr.h"
#define float double


/*#########################################################################*/
/*#                                                                       #*/
/*# save_AP --                                                            #*/
/*#                                                                       #*/
/*#      This function saves the value of the C-structure AnalytPar       #*/
/*#      into the tcl variable AP                                         #*/
/*#                                                                       #*/
/*# Results: TCL_OK, TCL_ERROR                                            #*/
/*#                                                                       #*/
/*#########################################################################*/
int save_AP(interp,APname,AP,max_SD,alambda)
    Tcl_Interp       *interp;
    const char       *APname;
    int              max_SD;
    float            alambda;
    sasfit_analytpar *AP;

{
    int              i,j;
    char             sBuffer[256];
    float            sumR_moments[9],sumfp;
    Tcl_DString      DsBuffer;

/*
 * saves the number of size distributions max_SD
 */
    sprintf(sBuffer,"%d",max_SD);
    Tcl_SetVar2(interp,APname,"max_SD",sBuffer,0);
/*
 * saves the value of alambda
 */
    float_to_string(sBuffer,alambda);
    Tcl_SetVar2(interp,APname,"alambda", sBuffer,0);
/*
 * saves parameter l[MAXPAR] of the "max_SD" form factors
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(j=0;j<MAXPAR;j++) {
           float_to_string(sBuffer,AP[i].FF_l[j]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,APname,"FF,l",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);
/*
 * save parameter err of the "max_SD" form factors
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(j=0;j<MAXPAR;j++) {
           float_to_string(sBuffer,AP[i].FF_err[j]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
        }
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,APname,"FF,err",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter min of the "max_SD" form factors
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(j=0;j<MAXPAR;j++) {
           float_to_string(sBuffer,AP[i].FF_min[j]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,APname,"FF,min",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter max of the "max_SD" form factors
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(j=0;j<MAXPAR;j++) {
           float_to_string(sBuffer,AP[i].FF_max[j]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,APname,"FF,max",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter limits of the "max_SD" form factors
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(j=0;j<MAXPAR;j++) {
           sprintf(sBuffer,"%d",AP[i].FF_limits[j]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,APname,"FF,limits",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);


/*
 * save parameter active of the "max_SD" form factors
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(j=0;j<MAXPAR;j++) {
           sprintf(sBuffer,"%d",AP[i].FF_active[j]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,APname,"FF,active",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter distr of the "max_SD" form factors
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(j=0;j<MAXPAR;j++) {
           sprintf(sBuffer,"%d",AP[i].FF_distr[j]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,APname,"FF,distr",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter typestr of the "max_SD" form factors
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringAppendElement(&DsBuffer,AP[i].FF.typestr);
    }
    Tcl_SetVar2(interp,APname,"FF,typestr",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter a of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(j=0;j<MAXPAR;j++) {
           float_to_string(sBuffer,AP[i].SD_a[j]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,APname,"SD,a",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter err of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(j=0;j<MAXPAR;j++) {
           float_to_string(sBuffer,AP[i].SD_err[j]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,APname,"SD,err",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter min of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(j=0;j<MAXPAR;j++) {
           float_to_string(sBuffer,AP[i].SD_min[j]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,APname,"SD,min",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter max of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(j=0;j<MAXPAR;j++) {
           float_to_string(sBuffer,AP[i].SD_max[j]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,APname,"SD,max",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter limits of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(j=0;j<MAXPAR;j++) {
           sprintf(sBuffer,"%d",AP[i].SD_limits[j]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,APname,"SD,limits",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);
/*
 * save parameter active of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(j=0;j<MAXPAR;j++) {
           sprintf(sBuffer,"%d",AP[i].SD_active[j]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,APname,"SD,active",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter typestr of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringAppendElement(&DsBuffer,AP[i].SD.typestr);
    }
    Tcl_SetVar2(interp,APname,"SD,typestr",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);


/*
 * save parameter a of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(j=0;j<MAXPAR;j++) {
           float_to_string(sBuffer,AP[i].SQ_s[j]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,APname,"SQ,s",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter err of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(j=0;j<MAXPAR;j++) {
           float_to_string(sBuffer,AP[i].SQ_err[j]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,APname,"SQ,err",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter min of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(j=0;j<MAXPAR;j++) {
           float_to_string(sBuffer,AP[i].SQ_min[j]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,APname,"SQ,min",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter max of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(j=0;j<MAXPAR;j++) {
           float_to_string(sBuffer,AP[i].SQ_max[j]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,APname,"SQ,max",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter limits of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(j=0;j<MAXPAR;j++) {
           sprintf(sBuffer,"%d",AP[i].SQ_limits[j]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,APname,"SQ,limits",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);
/*
 * save parameter active of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(j=0;j<MAXPAR;j++) {
           sprintf(sBuffer,"%d",AP[i].SQ_active[j]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,APname,"SQ,active",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter typestr of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringAppendElement(&DsBuffer,AP[i].SQ.typestr);
    }
    Tcl_SetVar2(interp,APname,"SQ,typestr",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);



/*
 * save parameter calcSDFF of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        if (AP[i].calcSDFF) {
		   /*
		    sprintf(sBuffer,"%d",AP[i].calcSDFF);
			*/
           Tcl_DStringAppendElement(&DsBuffer,"yes");
		} else { Tcl_DStringAppendElement(&DsBuffer,"no"); }
    }
    Tcl_SetVar2(interp,APname,"calcSDFF",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter substrSDFF of the "max_SD" size distribution
 */


    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        if (AP[i].substrSDFF) {
           Tcl_DStringAppendElement(&DsBuffer,"yes");
		} else { Tcl_DStringAppendElement(&DsBuffer,"no"); }
    }
    Tcl_SetVar2(interp,APname,"substrSDFF",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);


/*
 * save parameter fitSDFF of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
		if (AP[i].fitSDFF) {
		   /*
			sprintf(sBuffer,"%d",AP[i].fitSDFF);
		    */
           Tcl_DStringAppendElement(&DsBuffer,"yes");
		} else { Tcl_DStringAppendElement(&DsBuffer,"no"); }
    }
    Tcl_SetVar2(interp,APname,"fitSDFF",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter <R^n> of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
	   float_to_string(sBuffer,AP[i].R_moments[0]);
       Tcl_DStringAppendElement(&DsBuffer,sBuffer);
    }
	Tcl_SetVar2(interp,APname,"<R^0>",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

	for (j=1;j<=8;j++) {
       Tcl_DStringInit(&DsBuffer);
       for (i=0;i<max_SD;i++) {
		   if ( SASFIT_EQUAL(0.0, AP[i].R_moments[0]) ) {
			   sprintf(sBuffer,"0.0");
		   } else {
	           float_to_string(sBuffer,AP[i].R_moments[j]/AP[i].R_moments[0]);
		   }
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
	   }
       sprintf(sBuffer,"<R^%.1d>",j);
       Tcl_SetVar2(interp,APname,sBuffer,Tcl_DStringValue(&DsBuffer),0);
       Tcl_DStringFree(&DsBuffer);
    }


/*
 * save parameter fp of the "max_SD" size distribution
 */

    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
	   float_to_string(sBuffer,AP[i].fp);
       Tcl_DStringAppendElement(&DsBuffer,sBuffer);
    }
	Tcl_SetVar2(interp,APname,"fp",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter <R^n>/<R^m> of the "max_SD" size distribution
 */
/*
 *   R_li = 3/4 li = <R^3>/<R^2>
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
		if ( SASFIT_EQUAL(0.0, AP[i].R_moments[2]) ) {
           sprintf(sBuffer,"0.0");
		} else {
	       float_to_string(sBuffer,fabs(AP[i].R_moments[3]/AP[i].R_moments[2]));
		}
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
    }
	Tcl_SetVar2(interp,APname,"R_li",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 *   R_lc = 2/3 lc = <R^4>/<R^3>
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
		if ( SASFIT_EQUAL(0.0, AP[i].R_moments[3]) ) {
           sprintf(sBuffer,"0.0");
		} else {
	       float_to_string(sBuffer,fabs(AP[i].R_moments[4]/AP[i].R_moments[3]));
		}
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
    }
	Tcl_SetVar2(interp,APname,"R_lc",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);


/*
 *   R_Ac = sqrt(5/(4 pi) Ac) = sqrt(<R^5>/<R^3>)
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
		if ( SASFIT_EQUAL(0.0, AP[i].R_moments[3]) ) {
           sprintf(sBuffer,"0.0");
		} else {
	       float_to_string(sBuffer,pow(fabs(AP[i].R_moments[5]/AP[i].R_moments[3]),0.5));
		}
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
    }
	Tcl_SetVar2(interp,APname,"R_Ac",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 *   R_VP = (3/(4 pi) VP)^(1/3) = (<R^6>/<R^3>)^(1/3)
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
		if ( SASFIT_EQUAL(0.0, AP[i].R_moments[3]) ) {
			   sprintf(sBuffer,"0.0");
		} else {
	       float_to_string(sBuffer,pow(fabs(AP[i].R_moments[6]/AP[i].R_moments[3]),1.0/3.0));
		}
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
    }
	Tcl_SetVar2(interp,APname,"R_VP",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 *   R_RG = sqrt(5/3 RG) = sqrt(<R^8>/<R^6>)
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
		if ( SASFIT_EQUAL(0.0, AP[i].R_moments[6]) ) {
           sprintf(sBuffer,"0.0");
		} else {
	       float_to_string(sBuffer,pow(fabs(AP[i].R_moments[8]/AP[i].R_moments[6]),0.5));
		}
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
    }
	Tcl_SetVar2(interp,APname,"R_RG",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter <sumR^n> of the sum of ALL "max_SD" size distribution
 */
	for (j=0;j<=8;j++) {
        sumR_moments[j] = 0.0;
	    for (i=0;i<max_SD;i++) {
			sumR_moments[j] = sumR_moments[j] + AP[i].R_moments[j];
		}
	}

	Tcl_DStringInit(&DsBuffer);
    float_to_string(sBuffer,sumR_moments[0]);
    Tcl_DStringAppendElement(&DsBuffer,sBuffer);
	Tcl_SetVar2(interp,APname,"<sumR^0>",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

	for (j=1;j<=8;j++) {
        Tcl_DStringInit(&DsBuffer);
		if ( SASFIT_EQUAL(0.0, sumR_moments[0]) ) {
		   sprintf(sBuffer,"0.0");
		} else {
	       float_to_string(sBuffer,sumR_moments[j]/sumR_moments[0]);
		}
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
        sprintf(sBuffer,"<sumR^%.1d>",j);
        Tcl_SetVar2(interp,APname,sBuffer,Tcl_DStringValue(&DsBuffer),0);
        Tcl_DStringFree(&DsBuffer);
    }

/*
 * save parameter <R^n>/<R^m> of the "max_SD" size distribution
 */

/*
 *   sumR_li = 3/4 li = <sumR^3>/<sumR^2>
 */
    Tcl_DStringInit(&DsBuffer);
		if ( SASFIT_EQUAL(0.0, sumR_moments[2]) ) {
           sprintf(sBuffer,"0.0");
		} else {
	       float_to_string(sBuffer,fabs(sumR_moments[3]/sumR_moments[2]));
		}
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
	Tcl_SetVar2(interp,APname,"sumR_li",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 *   sumR_lc = 2/3 lc = <sumR^4>/<sumR^3>
 */
    Tcl_DStringInit(&DsBuffer);
		if ( SASFIT_EQUAL(0.0, sumR_moments[3]) ) {
           sprintf(sBuffer,"0.0");
		} else {
	       float_to_string(sBuffer,fabs(sumR_moments[4]/sumR_moments[3]));
		}
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
	Tcl_SetVar2(interp,APname,"sumR_lc",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);


/*
 *   sumR_Ac = sqrt(5/(4 pi) Ac) = sqrt(<sumR^5>/<sumR^3>)
 */
    Tcl_DStringInit(&DsBuffer);
		if ( SASFIT_EQUAL(0.0, sumR_moments[3]) ) {
           sprintf(sBuffer,"0.0");
		} else {
	       float_to_string(sBuffer,pow(fabs(sumR_moments[5]/sumR_moments[3]),0.5));
		}
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
	Tcl_SetVar2(interp,APname,"sumR_Ac",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 *   sumR_VP = (3/(4 pi) VP)^(1/3) = (<sumR^6>/<sumR^3>)^(1/3)
 */
    Tcl_DStringInit(&DsBuffer);
		if ( SASFIT_EQUAL(0.0, sumR_moments[3]) ) {
           sprintf(sBuffer,"0.0");
		} else {
	       float_to_string(sBuffer,pow(fabs(sumR_moments[6]/sumR_moments[3]),1.0/3.0));
		}
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
	Tcl_SetVar2(interp,APname,"sumR_VP",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 *   sumR_RG = sqrt(5/3 RG) = sqrt(<sumR^8>/<sumR^6>)
 */
    Tcl_DStringInit(&DsBuffer);
		if ( SASFIT_EQUAL(0.0, sumR_moments[6]) ) {
           sprintf(sBuffer,"0.0");
		} else {
	       float_to_string(sBuffer,pow(fabs(sumR_moments[8]/sumR_moments[6]),0.5));
		}
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
	Tcl_SetVar2(interp,APname,"sumR_RG",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 *   sumfp = sum fp[i]
 */
	sumfp = 0.0;
    Tcl_DStringInit(&DsBuffer);
	    for (i=0;i<max_SD;i++) {
			sumfp = sumfp + AP[i].fp;
		}
		float_to_string(sBuffer,sumfp);
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
	Tcl_SetVar2(interp,APname,"sumfp",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);


	return TCL_OK;
}






/*#########################################################################*/
/*#                                                                       #*/
/*# save_GlobalAP --                                                      #*/
/*#                                                                       #*/
/*#      This function saves the value of the C-structure GlobalAnalytPar #*/
/*#      into the tcl variable GlobalAP                                   #*/
/*#                                                                       #*/
/*# Results: TCL_OK, TCL_ERROR                                            #*/
/*#                                                                       #*/
/*#########################################################################*/
int save_GlobalAP(interp,GlobalAPname,GlobalAP,GCP,max_SD,alambda)
    Tcl_Interp       *interp;
    char             *GlobalAPname;
    int              max_SD;
    float            alambda;
    sasfit_analytpar *GlobalAP;
	sasfit_commonpar *GCP;
{
    int              i,j,k;
    char             sBuffer[256], sBuffer2[256];
    float            sumR_moments[9],sumfp;
    Tcl_DString      DsBuffer;

/*
 * saves the number of size distributions max_SD
 */
    sprintf(sBuffer,"%d",max_SD);
    Tcl_SetVar2(interp,GlobalAPname,"max_SD",sBuffer,0);
/*
 * saves the value of alambda
 */
    float_to_string(sBuffer,alambda);
    Tcl_SetVar2(interp,GlobalAPname,"alambda", sBuffer,0);



/*
 * saves global parameter P1,P2,....
 */
    for (i=0;i<(*GCP).common_i;i++) {
         sprintf(sBuffer2,"P%d",i+1);
		 float_to_string(sBuffer,(*GCP).P_common[i]);
         Tcl_SetVar2(interp,GlobalAPname,sBuffer2, sBuffer,0);
         sprintf(sBuffer2,"P%d,err",i+1);
		 float_to_string(sBuffer,(*GCP).P_common_err[i]);
         Tcl_SetVar2(interp,GlobalAPname,sBuffer2, sBuffer,0);
    }

/*
 * saves parameter l[4] of the "max_SD" form factors
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(k=0;k<MAXPAR;k++) {
           float_to_string(sBuffer,GlobalAP[i].FF_l[k]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,GlobalAPname,"FF,l",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);
/*
 * save parameter err of the "max_SD" form factors
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(k=0;k<MAXPAR;k++) {
           float_to_string(sBuffer,GlobalAP[i].FF_err[k]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,GlobalAPname,"FF,err",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter min of the "max_SD" form factors
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(k=0;k<MAXPAR;k++) {
           float_to_string(sBuffer,GlobalAP[i].FF_min[k]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,GlobalAPname,"FF,min",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter max of the "max_SD" form factors
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(k=0;k<MAXPAR;k++) {
           float_to_string(sBuffer,GlobalAP[i].FF_max[k]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,GlobalAPname,"FF,max",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter limits of the "max_SD" form factors
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(k=0;k<MAXPAR;k++) {
           sprintf(sBuffer,"%d",GlobalAP[i].FF_limits[k]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,GlobalAPname,"FF,limits",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);


/*
 * save parameter active of the "max_SD" form factors
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(k=0;k<MAXPAR;k++) {
           sprintf(sBuffer,"%d",GlobalAP[i].FF_active[k]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,GlobalAPname,"FF,active",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter distr of the "max_SD" form factors
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(k=0;k<MAXPAR;k++) {
           sprintf(sBuffer,"%d",GlobalAP[i].FF_distr[k]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,GlobalAPname,"FF,distr",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter typestr of the "max_SD" form factors
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringAppendElement(&DsBuffer,GlobalAP[i].FF.typestr);
    }
    Tcl_SetVar2(interp,GlobalAPname,"FF,typestr",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter a of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(k=0;k<MAXPAR;k++) {
           float_to_string(sBuffer,GlobalAP[i].SD_a[k]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,GlobalAPname,"SD,a",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter err of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(k=0;k<MAXPAR;k++) {
           float_to_string(sBuffer,GlobalAP[i].SD_err[k]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,GlobalAPname,"SD,err",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter min of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(k=0;k<MAXPAR;k++) {
           float_to_string(sBuffer,GlobalAP[i].SD_min[k]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,GlobalAPname,"SD,min",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter max of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(k=0;k<MAXPAR;k++) {
           float_to_string(sBuffer,GlobalAP[i].SD_max[k]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,GlobalAPname,"SD,max",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter limits of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(k=0;k<MAXPAR;k++) {
           sprintf(sBuffer,"%d",GlobalAP[i].SD_limits[k]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,GlobalAPname,"SD,limits",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);
/*
 * save parameter active of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(k=0;k<MAXPAR;k++) {
           sprintf(sBuffer,"%d",GlobalAP[i].SD_active[k]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,GlobalAPname,"SD,active",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter typestr of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringAppendElement(&DsBuffer,GlobalAP[i].SD.typestr);
    }
    Tcl_SetVar2(interp,GlobalAPname,"SD,typestr",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter a of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(k=0;k<MAXPAR;k++) {
           float_to_string(sBuffer,GlobalAP[i].SQ_s[k]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,GlobalAPname,"SQ,s",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter err of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(k=0;k<MAXPAR;k++) {
           float_to_string(sBuffer,GlobalAP[i].SQ_err[k]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,GlobalAPname,"SQ,err",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter min of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(k=0;k<MAXPAR;k++) {
           float_to_string(sBuffer,GlobalAP[i].SQ_min[k]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,GlobalAPname,"SQ,min",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter max of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(k=0;k<MAXPAR;k++) {
           float_to_string(sBuffer,GlobalAP[i].SQ_max[k]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,GlobalAPname,"SQ,max",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter limits of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(k=0;k<MAXPAR;k++) {
           sprintf(sBuffer,"%d",GlobalAP[i].SQ_limits[k]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,GlobalAPname,"SQ,limits",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);
/*
 * save parameter active of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
		for(k=0;k<MAXPAR;k++) {
           sprintf(sBuffer,"%d",GlobalAP[i].SQ_active[k]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,GlobalAPname,"SQ,active",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter typestr of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        Tcl_DStringAppendElement(&DsBuffer,GlobalAP[i].SQ.typestr);
    }
    Tcl_SetVar2(interp,GlobalAPname,"SQ,typestr",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);



/*
 * save parameter calcSDFF of the "max_SD" size distribution
 */
 /*
 Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        if (GlobalAP[i].calcSDFF) {
           Tcl_DStringAppendElement(&DsBuffer,"yes");
		} else { Tcl_DStringAppendElement(&DsBuffer,"no"); }
    }
    Tcl_SetVar2(interp,GlobalAPname,"calcSDFF",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);
*/

/*
 * save parameter substrSDFF of the "max_SD" size distribution
 */


    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
        if (GlobalAP[i].substrSDFF) {
           Tcl_DStringAppendElement(&DsBuffer,"yes");
		} else { Tcl_DStringAppendElement(&DsBuffer,"no"); }
    }
    Tcl_SetVar2(interp,GlobalAPname,"substrSDFF",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);


/*
 * save parameter fitSDFF of the "max_SD" size distribution
 */
/*
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
		if (GlobalAP[i].fitSDFF) {
           Tcl_DStringAppendElement(&DsBuffer,"yes");
		} else { Tcl_DStringAppendElement(&DsBuffer,"no"); }
    }
    Tcl_SetVar2(interp,GlobalAPname,"fitSDFF",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);
*/

/*
 * save parameter <R^n> of the "max_SD" size distribution
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
	   float_to_string(sBuffer,GlobalAP[i].R_moments[0]);
       Tcl_DStringAppendElement(&DsBuffer,sBuffer);
    }
	Tcl_SetVar2(interp,GlobalAPname,"<R^0>",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

	for (j=1;j<=8;j++) {
       Tcl_DStringInit(&DsBuffer);
       for (i=0;i<max_SD;i++) {
		   if ( SASFIT_EQUAL(0.0, GlobalAP[i].R_moments[0]) ) {
			   sprintf(sBuffer,"0.0");
		   } else {
	           float_to_string(sBuffer,GlobalAP[i].R_moments[j]/GlobalAP[i].R_moments[0]);
		   }
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
	   }
       sprintf(sBuffer,"<R^%.1d>",j);
       Tcl_SetVar2(interp,GlobalAPname,sBuffer,Tcl_DStringValue(&DsBuffer),0);
       Tcl_DStringFree(&DsBuffer);
    }


/*
 * save parameter fp of the "max_SD" size distribution
 */

    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
	   float_to_string(sBuffer,GlobalAP[i].fp);
       Tcl_DStringAppendElement(&DsBuffer,sBuffer);
    }
	Tcl_SetVar2(interp,GlobalAPname,"fp",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter <R^n>/<R^m> of the "max_SD" size distribution
 */
/*
 *   R_li = 3/4 li = <R^3>/<R^2>
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
		if ( SASFIT_EQUAL(0.0, GlobalAP[i].R_moments[2]) ) {
           sprintf(sBuffer,"0.0");
		} else {
	       float_to_string(sBuffer,fabs(GlobalAP[i].R_moments[3]/GlobalAP[i].R_moments[2]));
		}
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
    }
	Tcl_SetVar2(interp,GlobalAPname,"R_li",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 *   R_lc = 2/3 lc = <R^4>/<R^3>
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
		if ( SASFIT_EQUAL(0.0, GlobalAP[i].R_moments[3]) ) {
           sprintf(sBuffer,"0.0");
		} else {
	       float_to_string(sBuffer,fabs(GlobalAP[i].R_moments[4]/GlobalAP[i].R_moments[3]));
		}
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
    }
	Tcl_SetVar2(interp,GlobalAPname,"R_lc",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);


/*
 *   R_Ac = sqrt(5/(4 pi) Ac) = sqrt(<R^5>/<R^3>)
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
		if ( SASFIT_EQUAL(0.0, GlobalAP[i].R_moments[3]) ) {
           sprintf(sBuffer,"0.0");
		} else {
	       float_to_string(sBuffer,pow(fabs(GlobalAP[i].R_moments[5]/GlobalAP[i].R_moments[3]),0.5));
		}
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
    }
	Tcl_SetVar2(interp,GlobalAPname,"R_Ac",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 *   R_VP = (3/(4 pi) VP)^(1/3) = (<R^6>/<R^3>)^(1/3)
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
		if ( SASFIT_EQUAL(0.0, GlobalAP[i].R_moments[3]) ) {
           sprintf(sBuffer,"0.0");
		} else {
	       float_to_string(sBuffer,pow(fabs(GlobalAP[i].R_moments[6]/GlobalAP[i].R_moments[3]),1.0/3.0));
		}
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
    }
	Tcl_SetVar2(interp,GlobalAPname,"R_VP",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 *   R_RG = sqrt(5/3 RG) = sqrt(<R^8>/<R^6>)
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<max_SD;i++) {
		if ( SASFIT_EQUAL(0.0, GlobalAP[i].R_moments[6]) ) {
           sprintf(sBuffer,"0.0");
		} else {
	       float_to_string(sBuffer,pow(fabs(GlobalAP[i].R_moments[8]/GlobalAP[i].R_moments[6]),0.5));
		}
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
    }
	Tcl_SetVar2(interp,GlobalAPname,"R_RG",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 * save parameter <sumR^n> of the sum of ALL "max_SD" size distribution
 */
	for (j=0;j<=8;j++) {
        sumR_moments[j] = 0.0;
	    for (i=0;i<max_SD;i++) {
			sumR_moments[j] = sumR_moments[j] + GlobalAP[i].R_moments[j];
		}
	}

	Tcl_DStringInit(&DsBuffer);
    float_to_string(sBuffer,sumR_moments[0]);
    Tcl_DStringAppendElement(&DsBuffer,sBuffer);
	Tcl_SetVar2(interp,GlobalAPname,"<sumR^0>",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

	for (j=1;j<=8;j++) {
        Tcl_DStringInit(&DsBuffer);
		if ( SASFIT_EQUAL(0.0, sumR_moments[0]) ) {
		   sprintf(sBuffer,"0.0");
		} else {
	       float_to_string(sBuffer,sumR_moments[j]/sumR_moments[0]);
		}
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
        sprintf(sBuffer,"<sumR^%.1d>",j);
        Tcl_SetVar2(interp,GlobalAPname,sBuffer,Tcl_DStringValue(&DsBuffer),0);
        Tcl_DStringFree(&DsBuffer);
    }

/*
 * save parameter <R^n>/<R^m> of the "max_SD" size distribution
 */

/*
 *   sumR_li = 3/4 li = <sumR^3>/<sumR^2>
 */
    Tcl_DStringInit(&DsBuffer);
		if ( SASFIT_EQUAL(0.0, sumR_moments[2]) ) {
           sprintf(sBuffer,"0.0");
		} else {
	       float_to_string(sBuffer,fabs(sumR_moments[3]/sumR_moments[2]));
		}
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
	Tcl_SetVar2(interp,GlobalAPname,"sumR_li",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 *   sumR_lc = 2/3 lc = <sumR^4>/<sumR^3>
 */
    Tcl_DStringInit(&DsBuffer);
		if ( SASFIT_EQUAL(0.0, sumR_moments[3]) ) {
           sprintf(sBuffer,"0.0");
		} else {
	       float_to_string(sBuffer,fabs(sumR_moments[4]/sumR_moments[3]));
		}
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
	Tcl_SetVar2(interp,GlobalAPname,"sumR_lc",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);


/*
 *   sumR_Ac = sqrt(5/(4 pi) Ac) = sqrt(<sumR^5>/<sumR^3>)
 */
    Tcl_DStringInit(&DsBuffer);
		if ( SASFIT_EQUAL(0.0, sumR_moments[3]) ) {
           sprintf(sBuffer,"0.0");
		} else {
	       float_to_string(sBuffer,pow(fabs(sumR_moments[5]/sumR_moments[3]),0.5));
		}
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
	Tcl_SetVar2(interp,GlobalAPname,"sumR_Ac",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 *   sumR_VP = (3/(4 pi) VP)^(1/3) = (<sumR^6>/<sumR^3>)^(1/3)
 */
    Tcl_DStringInit(&DsBuffer);
		if ( SASFIT_EQUAL(0.0, sumR_moments[3]) ) {
           sprintf(sBuffer,"0.0");
		} else {
	       float_to_string(sBuffer,pow(fabs(sumR_moments[6]/sumR_moments[3]),1.0/3.0));
		}
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
	Tcl_SetVar2(interp,GlobalAPname,"sumR_VP",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 *   sumR_RG = sqrt(5/3 RG) = sqrt(<sumR^8>/<sumR^6>)
 */
    Tcl_DStringInit(&DsBuffer);
		if ( SASFIT_EQUAL(0.0, sumR_moments[6]) ) {
           sprintf(sBuffer,"0.0");
		} else {
	       float_to_string(sBuffer,pow(fabs(sumR_moments[8]/sumR_moments[6]),0.5));
		}
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
	Tcl_SetVar2(interp,GlobalAPname,"sumR_RG",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);

/*
 *   sumfp = sum fp[i]
 */
	sumfp = 0.0;
    Tcl_DStringInit(&DsBuffer);
	    for (i=0;i<max_SD;i++) {
			sumfp = sumfp + GlobalAP[i].fp;
		}
		float_to_string(sBuffer,sumfp);
        Tcl_DStringAppendElement(&DsBuffer,sBuffer);
	Tcl_SetVar2(interp,GlobalAPname,"sumfp",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);


	return TCL_OK;
}




/*#########################################################################*/
/*#                                                                       #*/
/*# save_covar_alpha --                                                   #*/
/*#                                                                       #*/
/*#      This function saves the matrices elements of "covar" and         #*/
/*#      into in AP(covar) and AP(alpha)                                  #*/
/*#                                                                       #*/
/*# Results: TCL_OK, TCL_ERROR                                            #*/
/*#                                                                       #*/
/*#########################################################################*/
int save_covar_alpha(interp,APname,covar,alpha,ma)
    Tcl_Interp       *interp;
    char             *APname;
    int              ma;
    float            **covar,**alpha;
{
int          i,k;
char         sBuffer[256];
Tcl_DString  DsBuffer;

/*
 * saves the matrix covar into TCL-variable AP(covar)
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<ma;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
        for (k=0;k<ma;k++) {
           float_to_string(sBuffer,covar[i][k]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
        }
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,APname,"covar",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);
/*
 * saves the matrix alpha into TCL-variable AP(alpha)
 */
    Tcl_DStringInit(&DsBuffer);
    for (i=0;i<ma;i++) {
        Tcl_DStringStartSublist(&DsBuffer);
        for (k=0;k<ma;k++) {
           float_to_string(sBuffer,alpha[i][k]);
           Tcl_DStringAppendElement(&DsBuffer,sBuffer);
        }
        Tcl_DStringEndSublist(&DsBuffer);
    }
    Tcl_SetVar2(interp,APname,"alpha",Tcl_DStringValue(&DsBuffer),0);
    Tcl_DStringFree(&DsBuffer);
	return TCL_OK;
}

/*#########################################################################*/
/*#                                                                       #*/
/*# get_covar_alpha --                                                    #*/
/*#                                                                       #*/
/*#      This function reads the matrices elements of "covar" and         #*/
/*#      stored in AP(covar) and AP(alpha)                                #*/
/*#                                                                       #*/
/*# Results: TCL_OK, TCL_ERROR                                            #*/
/*#                                                                       #*/
/*#########################################################################*/
int get_covar_alpha(interp,argv,covar,alpha,ma)
    Tcl_Interp       *interp;
    char             **argv;
    int              ma;
    float            ***covar,***alpha;
{
int    i,k;
int    Splitcode, Splitargc, tcode, targc;
const char   **Splitargv, **targv;

    (*covar) = matrix(0,ma-1,0,ma-1);
    (*alpha) = matrix(0,ma-1,0,ma-1);
/*
 * read matrix covar
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"covar",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       free_matrix((*covar),0,ma-1,0,ma-1);
       free_matrix((*alpha),0,ma-1,0,ma-1);
       sasfit_err("AP(covar) is not a list");
       return TCL_ERROR;
    }
    if (Splitargc < ma) ma=Splitargc;
    for (i=0;i<ma;i++) {
       tcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if (tcode == TCL_ERROR) {
          free_matrix((*covar),0,ma-1,0,ma-1);
          free_matrix((*alpha),0,ma-1,0,ma-1);
          sasfit_err("AP(covar) is not a list");
          return TCL_ERROR;
       }
       for (k=0;k<ma;k++) {
           tcode = Tcl_GetDoubleD(interp,targv[k],&(*covar)[i][k]);
           if (tcode == TCL_ERROR) {
              free_matrix((*covar),0,ma-1,0,ma-1);
              free_matrix((*alpha),0,ma-1,0,ma-1);
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert element covar[i][k] to double");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);
/*
 * read matrix alpha
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"alpha",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       free_matrix((*covar),0,ma-1,0,ma-1);
       free_matrix((*alpha),0,ma-1,0,ma-1);
       sasfit_err("AP(alpha) is not a list");
       return TCL_ERROR;
    }

    if (Splitargc < ma) ma=Splitargc;
    for (i=0;i<ma;i++) {
       tcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if (tcode == TCL_ERROR) {
          free_matrix((*covar),0,ma-1,0,ma-1);
          free_matrix((*alpha),0,ma-1,0,ma-1);
          sasfit_err("AP(alpha) is not a list");
          return TCL_ERROR;
       }
       for (k=0;k<ma;k++) {
           tcode = Tcl_GetDoubleD(interp,targv[k],&(*alpha)[i][k]);
           if (tcode == TCL_ERROR) {
              free_matrix((*covar),0,ma-1,0,ma-1);
              free_matrix((*alpha),0,ma-1,0,ma-1);
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert element alpha[i][k] to double");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);
    return TCL_OK;

}



/*#########################################################################*/
/*#                                                                       #*/
/*# get_GlobalAP --                                                       #*/
/*#                                                                       #*/
/*#      This function reads the value of the tcl variable AP             #*/
/*#      in the C-structure AnalyticPar                                   #*/
/*#                                                                       #*/
/*# Results: TCL_OK, TCL_ERROR                                            #*/
/*#                                                                       #*/
/*#########################################################################*/
int get_GlobalAP(Tcl_Interp       *interp,
		 const char      **argv,
		 sasfit_analytpar **GlobalAnalyticPar,
		 sasfit_commonpar *GlobalCommonPar,
		 int              *mmax_SD,
		 float            *aalambda,
		 int              *error_type,
		 float            ***Q,
		 float            ***IQ,
	 	 float            ***DIQ,
		 float            ***Qres,
		 float            ***Ith,
		 int              **n_data,
		 int              **hide)
{
    sasfit_analytpar *GlobalAP = 0;
    int    i,j,k,itmp;
    int    Splitcode, Splitargc, tcode, targc;
    const char   **Splitargv = 0, **targv = 0;
    int    Qargc, IQargc, DIQargc, Qresargc;
    const char   **Qargv = 0, **IQargv = 0, **DIQargv = 0, **Qresargv = 0;
    int    max_SD,ndata,nmultset;
    float  *h = 0, *Ih = 0, *DIh = 0, *res = 0;
    float  tfloat;
    const char   *xyer_data = 0;
    char   sBuffer[256];

	int *param_count_sd = 0, *param_count_sq = 0, *param_count_ff = 0;
	int tcl_result, tcl_int, len;
	double tcl_double;

	if (sasfit_eps_get_from_tcl(interp, argv) == TCL_ERROR) {
		return TCL_ERROR;
	}

	// read the number alambda
	SF_TCL_GET(double, argv[1], "alambda", *aalambda);
	// read the error type
	SF_TCL_GET(int, argv[1], "error", *error_type);

/*
 * read scattering curve h, Ih and DIh
 */
    xyer_data   = argv[2];
    Splitcode = Tcl_SplitList(interp,xyer_data,&Splitargc,&Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_err("scattering data {{h} {Ih} {DIh} {res}} is not a list\n");
       return TCL_ERROR;
    }
    if ((Splitargc < 3) || (Splitargc > 4)) {
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in {{h} {Ih} {DIh} {<res>}}\n");
       return TCL_ERROR;
    }
    Splitcode = Tcl_SplitList(interp,Splitargv[0],&Qargc,&Qargv);
    if (Splitcode == TCL_ERROR) {
       Tcl_Free((char *) Splitargv);
       Tcl_Free((char *) Qargv);
       sasfit_err("could not read {h}\n");
       return TCL_ERROR;
    }
	nmultset = Qargc;
	(*GlobalCommonPar).nmultset = nmultset;

    (*Q)=(double **) Tcl_Alloc((unsigned) (nmultset)*sizeof(double*));
	if (!(*Q)) {
		sasfit_err("allocation failure 1 in Q()\n");
        Tcl_Free((char *) Splitargv);
        Tcl_Free((char *) Qargv);
        return TCL_ERROR;
	}
    (*Ith)=(double **) Tcl_Alloc((unsigned) (nmultset)*sizeof(double*));
	if (!(*Ith)) {
		sasfit_err("allocation failure 1 in Ith()\n");
        Tcl_Free((char *) Splitargv);
        Tcl_Free((char *) Qargv);
		Tcl_Free((char *) Q);
        return TCL_ERROR;
	}
    (*IQ)=(double **) Tcl_Alloc((unsigned) (nmultset)*sizeof(double*));
	if (!(*IQ)) {
		sasfit_err("allocation failure 1 in IQ()\n");
        Tcl_Free((char *) Splitargv);
        Tcl_Free((char *) Qargv);
		Tcl_Free((char *) Q);
		Tcl_Free((char *) Ith);
        return TCL_ERROR;
	}
    (*DIQ)=(double **) Tcl_Alloc((unsigned) (nmultset)*sizeof(double*));
	if (!(*DIQ)) {
		sasfit_err("allocation failure 1 in DIQ()\n");
        Tcl_Free((char *) Splitargv);
        Tcl_Free((char *) Qargv);
		Tcl_Free((char *) Q);
		Tcl_Free((char *) IQ);
		Tcl_Free((char *) Ith);
        return TCL_ERROR;
	}
    (*Qres)=(double **) Tcl_Alloc((unsigned) (nmultset)*sizeof(double*));
	if (!(*Qres)) {
		sasfit_err("allocation failure 1 in Qres()\n");
        Tcl_Free((char *) Splitargv);
        Tcl_Free((char *) Qargv);
		Tcl_Free((char *) Q);
		Tcl_Free((char *) IQ);
		Tcl_Free((char *) Ith);
		Tcl_Free((char *) DIQ);
        return TCL_ERROR;
	}
    (*n_data)=(int *) Tcl_Alloc((unsigned) (nmultset)*sizeof(int));
	if (!(*n_data)) {
		sasfit_err("allocation failure 1 in ndata()\n");
        Tcl_Free((char *) Splitargv);
        Tcl_Free((char *) Qargv);
		Tcl_Free((char *) Q);
		Tcl_Free((char *) IQ);
		Tcl_Free((char *) Ith);
		Tcl_Free((char *) DIQ);
		Tcl_Free((char *) Qres);
        return TCL_ERROR;
	}
    (*hide) =(int *) Tcl_Alloc((unsigned) (nmultset)*sizeof(int));
	if (!(*hide)) {
		sasfit_err("allocation failure 1 in ndata()\n");
        Tcl_Free((char *) Splitargv);
        Tcl_Free((char *) Qargv);
		Tcl_Free((char *) Q);
		Tcl_Free((char *) IQ);
		Tcl_Free((char *) Ith);
		Tcl_Free((char *) DIQ);
		Tcl_Free((char *) Qres);
		Tcl_Free((char *) n_data);
        return TCL_ERROR;
	}


	Splitcode = Tcl_SplitList(interp,argv[3],&targc,&targv);
    if ((Splitcode == TCL_ERROR) || (nmultset!=targc)) {
       Tcl_Free((char *) Splitargv);
       Tcl_Free((char *) Qargv);
	   Tcl_Free((char *) Q);
	   Tcl_Free((char *) IQ);
	   Tcl_Free((char *) Ith);
	   Tcl_Free((char *) DIQ);
	   Tcl_Free((char *) Qres);
	   Tcl_Free((char *) n_data);
	   Tcl_Free((char *) hide);
       sasfit_err("could not read {hide}\n");
       return TCL_ERROR;
    }

	for (j=0; j < targc ;j++) {
       tcode = Tcl_GetBoolean(interp,targv[j],&itmp);
       if (tcode == TCL_ERROR) {
			Tcl_Free((char *) Splitargv);
			Tcl_Free((char *) Qargv);
			Tcl_Free((char *) Q);
			Tcl_Free((char *) IQ);
			Tcl_Free((char *) Ith);
			Tcl_Free((char *) DIQ);
			Tcl_Free((char *) Qres);
			Tcl_Free((char *) n_data);
			Tcl_Free((char *) hide);
			Tcl_Free((char *) targv);
			sasfit_err("could not convert hide[j]) to double\n");
			return TCL_ERROR;
	  }
	   (*hide)[j]=itmp;
	}

	Splitcode = Tcl_SplitList(interp,Splitargv[1],&IQargc,&IQargv);
    if (Splitcode == TCL_ERROR) {
       Tcl_Free((char *) Splitargv);
       Tcl_Free((char *) IQargv);
       Tcl_Free((char *) Qargv);
	   Tcl_Free((char *) Q);
	   Tcl_Free((char *) Q);
	   Tcl_Free((char *) Ith);
	   Tcl_Free((char *) DIQ);
	   Tcl_Free((char *) Qres);
	   Tcl_Free((char *) n_data);
       Tcl_Free((char *) hide);
       sasfit_err("could not read {IQ}\n");
       return TCL_ERROR;
    }

	Splitcode = Tcl_SplitList(interp,Splitargv[2],&DIQargc,&DIQargv);
    if (Splitcode == TCL_ERROR) {
       Tcl_Free((char *) Splitargv);
       Tcl_Free((char *) DIQargv);
       Tcl_Free((char *) IQargv);
       Tcl_Free((char *) Qargv);
	   Tcl_Free((char *) Q);
	   Tcl_Free((char *) IQ);
	   Tcl_Free((char *) Ith);
	   Tcl_Free((char *) DIQ);
	   Tcl_Free((char *) Qres);
	   Tcl_Free((char *) n_data);
       Tcl_Free((char *) hide);
       sasfit_err("could not read {DIQ}\n");
       return TCL_ERROR;
    }
    if (Splitargc == 4) {
       Splitcode = Tcl_SplitList(interp,Splitargv[3],&Qresargc,&Qresargv);
       if (Splitcode == TCL_ERROR) {
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) Qresargv);
          Tcl_Free((char *) DIQargv);
          Tcl_Free((char *) IQargv);
          Tcl_Free((char *) Qargv);
	      Tcl_Free((char *) Q);
	      Tcl_Free((char *) IQ);
	      Tcl_Free((char *) Ith);
	      Tcl_Free((char *) DIQ);
	      Tcl_Free((char *) Qres);
	      Tcl_Free((char *) n_data);
		  Tcl_Free((char *) hide);
          sasfit_err("could not read {Qres}\n");
          return TCL_ERROR;
	   }

	   if ((Qresargc == Qargc) && (Qresargc == IQargc) && (Qresargc == DIQargc)) {
          nmultset = Qargc;
	   } else {
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) Qresargv);
          Tcl_Free((char *) DIQargv);
          Tcl_Free((char *) IQargv);
          Tcl_Free((char *) Qargv);
	      Tcl_Free((char *) Q);
	      Tcl_Free((char *) IQ);
	      Tcl_Free((char *) Ith);
	      Tcl_Free((char *) DIQ);
	      Tcl_Free((char *) Qres);
	      Tcl_Free((char *) n_data);
		  Tcl_Free((char *) hide);
          sasfit_err("{Q} {IQ} {DIQ} {Qres} are of different length\n");
          return TCL_ERROR;
	   }
	}
	if ((Qargc == IQargc) && (Qargc == DIQargc)) {
       nmultset = Qargc;
	} else {
       Tcl_Free((char *) Splitargv);
       Tcl_Free((char *) DIQargv);
       Tcl_Free((char *) IQargv);
       Tcl_Free((char *) Qargv);
	   Tcl_Free((char *) Q);
	   Tcl_Free((char *) IQ);
	   Tcl_Free((char *) Ith);
	   Tcl_Free((char *) DIQ);
	   Tcl_Free((char *) Qres);
	   Tcl_Free((char *) n_data);
	   Tcl_Free((char *) hide);
	   sasfit_err("{Q} {IQ} {DIQ} are of different length\n");
       return TCL_ERROR;
	}

	for (k=0;k<nmultset;k++) {
	   Splitcode = Tcl_SplitList(interp,Qargv[k],&targc,&targv);
       if (Splitcode == TCL_ERROR) {
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) Qargv);
          sasfit_err("could not read {h}\n");
          return TCL_ERROR;
	   }
       ndata = targc;
       (*n_data)[k] = ndata;
       h   = dvector(0,ndata-1);
       res = dvector(0,ndata-1);
       Ih  = dvector(0,ndata-1);
       DIh = dvector(0,ndata-1);
	   (*Ith)[k] = dvector(0,ndata-1);
	   for (i=0;i<ndata;i++) (*Ith)[k][i]=0;

       for (j=0; j < targc ;j++) {
          tcode = Tcl_GetDoubleD(interp,targv[j],&h[j]);
          if (tcode == TCL_ERROR) {
             free_dvector(h,0,ndata-1);
             free_dvector(res,0,ndata-1);
             free_dvector(Ih,0,ndata-1);
             free_dvector(DIh,0,ndata-1);
             Tcl_Free((char *) Splitargv);
             Tcl_Free((char *) targv);
             sasfit_err("could not convert h[j]) to double\n");
             return TCL_ERROR;
		  }
	   }
       Splitcode = Tcl_SplitList(interp,IQargv[k],&targc,&targv);
       if (Splitcode == TCL_ERROR) {
          free_dvector(h,0,ndata-1);
          free_dvector(res,0,ndata-1);
          free_dvector(Ih,0,ndata-1);
          free_dvector(DIh,0,ndata-1);
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("could not read Ih\n");
          return TCL_ERROR;
	   }
       if (targc != ndata) {
          free_dvector(h,0,ndata-1);
          free_dvector(res,0,ndata-1);
          free_dvector(Ih,0,ndata-1);
          free_dvector(DIh,0,ndata-1);
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("could not convert Ih[j]) to double\n");
          return TCL_ERROR;
	   }
       for (j=0; j < targc ;j++) {
          tcode = Tcl_GetDoubleD(interp,targv[j],&Ih[j]);
          if (tcode == TCL_ERROR) {
             free_dvector(h,0,ndata-1);
             free_dvector(res,0,ndata-1);
             free_dvector(Ih,0,ndata-1);
             free_dvector(DIh,0,ndata-1);
             Tcl_Free((char *) Splitargv);
             Tcl_Free((char *) targv);
             sasfit_err("could not convert Ih[j]) to double\n");
             return TCL_ERROR;
		  }
	   }
       Splitcode = Tcl_SplitList(interp,DIQargv[k],&targc,&targv);
       if (Splitcode == TCL_ERROR) {
          free_dvector(h,0,ndata-1);
          free_dvector(res,0,ndata-1);
          free_dvector(Ih,0,ndata-1);
          free_dvector(DIh,0,ndata-1);
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("could not read DIh\n");
          return TCL_ERROR;
	   }
       if (targc != ndata) {
          free_dvector(h,0,ndata-1);
          free_dvector(res,0,ndata-1);
          free_dvector(Ih,0,ndata-1);
          free_dvector(DIh,0,ndata-1);
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("could not convert DIh[j]) to double\n");
          return TCL_ERROR;
	   }
       for (j=0; j < targc ;j++) {
          tcode = Tcl_GetDoubleD(interp,targv[j],&DIh[j]);
          if (tcode == TCL_ERROR) {
             free_dvector(h,0,ndata-1);
             free_dvector(res,0,ndata-1);
             free_dvector(Ih,0,ndata-1);
             free_dvector(DIh,0,ndata-1);
             Tcl_Free((char *) Splitargv);
             Tcl_Free((char *) targv);
             sasfit_err("could not convert DIh[j]) to double\n");
             return TCL_ERROR;
		  }
	   }

   	   if (Splitargc == 4) {
	      Splitcode = Tcl_SplitList(interp,Qresargv[k],&targc,&targv);
          if (Splitcode == TCL_ERROR) {
             free_dvector(h,0,ndata-1);
             free_dvector(res,0,ndata-1);
             free_dvector(Ih,0,ndata-1);
             free_dvector(DIh,0,ndata-1);
             Tcl_Free((char *) Splitargv);
             Tcl_Free((char *) targv);
             sasfit_err("could not read res\n");
             return TCL_ERROR;
		  }
          if (targc != ndata) {
             free_dvector(h,0,ndata-1);
             free_dvector(res,0,ndata-1);
             free_dvector(Ih,0,ndata-1);
             free_dvector(DIh,0,ndata-1);
             Tcl_Free((char *) Splitargv);
             Tcl_Free((char *) targv);
             sasfit_err("could not convert res[j]) to double\n");
             return TCL_ERROR;
		  }
          for (j=0; j < targc ;j++) {
              tcode = Tcl_GetDoubleD(interp,targv[j],&res[j]);
              if (tcode == TCL_ERROR) {
                 free_dvector(h,0,ndata-1);
                 free_dvector(res,0,ndata-1);
                 free_dvector(Ih,0,ndata-1);
                 free_dvector(DIh,0,ndata-1);
                 Tcl_Free((char *) Splitargv);
                 Tcl_Free((char *) targv);
                 sasfit_err("could not convert res[j]) to double\n");
                 return TCL_ERROR;
			  }
		  }
	   } else {
          for (i=0;i<ndata;i++) res[i] = 0.0;
	   }
       switch (*error_type) {
          case 0 : {break;}
          case 1 : { for (i=0;i<ndata;i++) DIh[i] = log(fabs(Ih[i])); break; }
          case 2 : { for (i=0;i<ndata;i++) DIh[i] = sqrt(fabs(Ih[i])); break; }
          case 3 : { for (i=0;i<ndata;i++) DIh[i] = 1.0; break;}
          default: { sasfit_err("#get_GlobalAP: unknown error_type %d\n",*error_type);
                     for (i=0;i<ndata;i++) DIh[i] = 1.0;
			break;
		}
	   }
	   (*Q)[k]    = h;
       (*IQ)[k]   = Ih;
       (*DIQ)[k]  = DIh;
	   (*Qres)[k] = res;
	}

	// read the number of size distributions max_SD
	SF_TCL_GET(int, argv[1], "max_SD", max_SD);
	(*mmax_SD) = max_SD;
	sasfit_init_AP(&GlobalAP, max_SD);

/*
 * read the number of global parameters
 */
    if (TCL_ERROR == Tcl_GetInt(interp,
                             Tcl_GetVar2(interp,argv[1],"common_i",0),
                             &itmp) ) {
       sasfit_err("could not read number of common parameters\n");
       return TCL_ERROR;
    }

    (*GlobalCommonPar).common_i       =  itmp;
    (*GlobalCommonPar).P_common       = dvector(0,((*GlobalCommonPar).common_i)-1);
    (*GlobalCommonPar).P_common_err   = dvector(0,((*GlobalCommonPar).common_i)-1);
    (*GlobalCommonPar).P_common_index = ivector(0,((*GlobalCommonPar).common_i)-1);

    for (i=0;i<(*GlobalCommonPar).common_i;i++) {
        (*GlobalCommonPar).P_common_index[i] = -1;
		(*GlobalCommonPar).P_common_err[i] = 0.0;
	}
/*
 * read GlobalCommonPar (P_common)
 */


	 for (i=0;i<(*GlobalCommonPar).common_i;i++) {
		 sprintf(sBuffer,"P%d",i+1);
		 tcode = Tcl_GetDoubleD(interp,
                              Tcl_GetVar2(interp,argv[1],sBuffer,0),
                              &tfloat);
		 (*GlobalCommonPar).P_common[i] = tfloat;
         if (tcode == TCL_ERROR) {
            sasfit_err("Could not read (*GlobalCommonPar).P_common[i]\n");
            return TCL_ERROR;
		 }
    }
    Tcl_Free((char *) Splitargv);
/*
 * read the number of parameters for each contribution to get only values
 * which are actually in use
 */
	tcl_result = sasfit_tcl_get_arr_int(interp, &param_count_ff,
	                    &len, argv[1],"FF,param_count");
	if (tcl_result != TCL_OK) {
		sasfit_free_AP(&GlobalAP);
		return TCL_ERROR;
	}
/*
 * read parameter l[MAXPAR] of the "max_SD" form factors
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"FF,l",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("GlobalAP(FF,l) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP(FF,l)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &GlobalAP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in GlobalAP[i](FF,l)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetDoubleD(interp,targv[j],&GlobalAP[i].FF_l[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &GlobalAP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert GlobalAP[i](FF,l[j]) to double\n");
              return TCL_ERROR;
           }
       }
    }
    Tcl_Free((char *) Splitargv);


/*
 * read parameter factor of the "max_SD" formfactor
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"FF,factor",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("could not read list GlobalAP[i](FF,factor)\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP(FF,factor)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &GlobalAP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in GlobalAP[i](FF,factor)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetDoubleD(interp,targv[j],&GlobalAP[i].FF_factor[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &GlobalAP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert GlobalAP[i](FF,factor[j]) to double\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);


/*
 * read parameter common of the "max_SD" form factors
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"FF,common",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("GlobalAP(FF,common) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP(FF,common)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &GlobalAP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in GlobalAP[i](FF,common)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
		   strcpy(GlobalAP[i].FF_common[j],targv[j]);
           GlobalAP[i].FF_index[j] = 0;
		   for (k=0;k<(*GlobalCommonPar).common_i;k++) {
               sprintf(sBuffer,"P%d",k+1);
			   if (strcmp(sBuffer,GlobalAP[i].FF_common[j])==0) {
                   GlobalAP[i].FF_index[j] = k+1;
			   }
		   }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);


/*
 * read parameter err of the "max_SD" form factors
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"FF,err",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("GlobalAP(FF,err) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP(FF,err)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &GlobalAP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in GlobalAP[i](FF,err)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetDoubleD(interp,targv[j],&GlobalAP[i].FF_err[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &GlobalAP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert GlobalAP[i](FF,err[j]) to double\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter min of the "max_SD" form factors
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"FF,min",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("GlobalAP(FF,min) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP(FF,min)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &GlobalAP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in GlobalAP[i](FF,min)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetDoubleD(interp,targv[j],&GlobalAP[i].FF_min[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &GlobalAP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert GlobalAP[i](FF,min[j]) to double\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter max of the "max_SD" form factors
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"FF,max",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("GlobalAP(FF,max) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP(FF,max)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &GlobalAP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in GlobalAP[i](FF,max)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetDoubleD(interp,targv[j],&GlobalAP[i].FF_max[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &GlobalAP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert GlobalAP[i](FF,max[j]) to double\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter limits of the "max_SD" form factors
 */
  Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"FF,limits",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("GlobalAP(FF,limits) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP(FF,limits)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &GlobalAP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in GlobalAP[i](FF,limits)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetBoolean(interp,targv[j],&GlobalAP[i].FF_limits[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &GlobalAP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert GlobalAP[i](FF,limits[j]) to integer\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter active of the "max_SD" form factors
 */
  Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"FF,active",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("GlobalAP(FF,active) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP(FF,active)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &GlobalAP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in GlobalAP[i](FF,active)\n");
          return TCL_ERROR;
       }
       for (j=0; j < MAXPAR ;j++) GlobalAP[i].FF_active[j] = 0;
       for (j=0; j < param_count_ff[i] ;j++) {
           tcode = Tcl_GetBoolean(interp,targv[j],&GlobalAP[i].FF_active[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &GlobalAP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert GlobalAP[i](FF,active[j]) to integer\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);


/*
 * read parameter distr of the "max_SD" form factors
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"FF,distr",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("GlobalAP(FF,distr) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_err("wrong # of list elements in GlobalAP[i](FF,distr)\n");
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_err("wrong # of list elements in GlobalAP[i](FF,distr[j])\n");
          sasfit_free_AP( &GlobalAP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetBoolean(interp,targv[j],&GlobalAP[i].FF_distr[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &GlobalAP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert GlobalAP[i](FF,distr[j]) to integer\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter typestr of the "max_SD" form factors
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"FF,typestr",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("could not convert list GlobalAP[i](FF,typestr)\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_err("wrong # of list elements in GlobalAP[i](FF,typestr)\n");
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       return TCL_ERROR;
    }
    if ( initTypeStr(GlobalAP, max_SD, Splitargv, FFfct) != TCL_OK ) return TCL_ERROR;
    Tcl_Free((char *) Splitargv);

/*
 * read the number of parameters for each contribution to get only values
 * which are actually in use
 */
	tcl_result = sasfit_tcl_get_arr_int(interp, &param_count_sd,
	                    &len, argv[1],"SD,param_count");
	if (tcl_result != TCL_OK) {
		sasfit_free_AP(&GlobalAP);
		return TCL_ERROR;
	}
/*
 * read parameter a of the "max_SD" size distribution
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SD,a",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("could not read list GlobalAP[i](SD,a)\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP(SD,a)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &GlobalAP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in GlobalAP[i](SD,a)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetDoubleD(interp,targv[j],&GlobalAP[i].SD_a[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &GlobalAP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert GlobalAP[i](SD,a[j]) to double\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter factor of the "max_SD" size distribution
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SD,factor",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("could not read list GlobalAP[i](SD,factor)\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP(SD,factor)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &GlobalAP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in GlobalAP[i](SD,factor)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetDoubleD(interp,targv[j],&GlobalAP[i].SD_factor[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &GlobalAP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert GlobalAP[i](SD,factor[j]) to double\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter common of the "max_SD" size distribution
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SD,common",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("GlobalAP(SD,common) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP(SD,common)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &GlobalAP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in GlobalAP[i](SD,common)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
		   strcpy(GlobalAP[i].SD_common[j],targv[j]);
           GlobalAP[i].SD_index[j] = 0;
		   for (k=0;k<(*GlobalCommonPar).common_i;k++) {
               sprintf(sBuffer,"P%d",k+1);
			   if (strcmp(sBuffer,GlobalAP[i].SD_common[j])==0) {
                   GlobalAP[i].SD_index[j] = k+1;
			   }
		   }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter err of the "max_SD" size distribution
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SD,err",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("could not read list GlobalAP[i](SD,err)\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP(SD,err)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &GlobalAP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in GlobalAP[i](SD,err)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetDoubleD(interp,targv[j],&GlobalAP[i].SD_err[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &GlobalAP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert GlobalAP[i](SD,err[j]) to double\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter min of the "max_SD" size distribution
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SD,min",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("could not read list GlobalAP(SD,min)\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP(SD,min)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &GlobalAP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in GlobalAP[i](SD,min)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetDoubleD(interp,targv[j],&GlobalAP[i].SD_min[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &GlobalAP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert GlobalAP[i](SD,min[j]) to double\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter max of the "max_SD" size distribution
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SD,max",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("could not read list GlobalAP(SD,max)\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP(SD,max)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &GlobalAP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in GlobalAP[i](SD,max)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetDoubleD(interp,targv[j],&GlobalAP[i].SD_max[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &GlobalAP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert GlobalAP[i](SD,max[j]) to double\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter limits of the "max_SD" size distribution
 */
  Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SD,limits",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("GlobalAP(SD,limits) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP[i](SD,limits)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &GlobalAP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in GlobalAP[i](SD,limits)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetBoolean(interp,targv[j],&GlobalAP[i].SD_limits[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &GlobalAP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert GlobalAP[i](SD,limits[j]) to integer\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter active of the "max_SD" size distribution
 */
  Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SD,active",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("GlobalAP(SD,active) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP[i](SD,active)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &GlobalAP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in GlobalAP[i](SD,active)\n");
          return TCL_ERROR;
       }
       for (j=0; j < MAXPAR ;j++) GlobalAP[i].SD_active[j] = 0;
       for (j=0; j < param_count_sd[i] ;j++) {
           tcode = Tcl_GetBoolean(interp,targv[j],&GlobalAP[i].SD_active[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &GlobalAP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert GlobalAP[i](SD,active[j]) to integer\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter typestr of the "max_SD" size distribution
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SD,typestr",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("GlobalAP(SD,typestr) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP[i](SD,typestr)\n");
       return TCL_ERROR;
    }

    if ( initTypeStr(GlobalAP, max_SD, Splitargv, SDfct) != TCL_OK ) return TCL_ERROR;
    Tcl_Free((char *) Splitargv);

/*
 * read the number of parameters for each contribution to get only values
 * which are actually in use
 */
	tcl_result = sasfit_tcl_get_arr_int(interp, &param_count_sq,
	                    &len, argv[1],"SQ,param_count");
	if (tcl_result != TCL_OK) {
		sasfit_free_AP(&GlobalAP);
		return TCL_ERROR;
	}
/*
 * read parameter a of the "max_SD" structure factor
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SQ,s",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("could not read list GlobalAP[i](SQ,s)\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP(SQ,s)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &GlobalAP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in GlobalAP[i](SQ,s)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetDoubleD(interp,targv[j],&GlobalAP[i].SQ_s[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &GlobalAP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert GlobalAP[i](SQ,s[j]) to double\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter factor of the "max_SD" structure factor
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SQ,factor",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("could not read list GlobalAP[i](SQ,factor)\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP(SQ,factor)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &GlobalAP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in GlobalAP[i](SQ,factor)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetDoubleD(interp,targv[j],&GlobalAP[i].SQ_factor[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &GlobalAP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert GlobalAP[i](SQ,factor[j]) to double\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter common of the "max_SD" structure factor
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SQ,common",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("GlobalAP(SQ,common) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP(SQ,common)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &GlobalAP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in GlobalAP[i](SQ,common)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
		strcpy(GlobalAP[i].SQ_common[j],targv[j]);
		GlobalAP[i].SQ_index[j] = 0;
		for (k=0;k<(*GlobalCommonPar).common_i;k++) {
			sprintf(sBuffer,"P%d",k+1);
			if (strcmp(sBuffer,GlobalAP[i].SQ_common[j])==0) {
				GlobalAP[i].SQ_index[j] = k+1;
			}
		}
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter err of the "max_SD" structure factor
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SQ,err",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("could not read list GlobalAP[i](SQ,err)\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP(SQ,err)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &GlobalAP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in GlobalAP[i](SQ,err)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetDoubleD(interp,targv[j],&GlobalAP[i].SQ_err[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &GlobalAP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert GlobalAP[i](SQ,err[j]) to double\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter min of the "max_SD" structure factor
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SQ,min",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("could not read list GlobalAP(SQ,min)\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP(SQ,min)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &GlobalAP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in GlobalAP[i](SQ,min)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetDoubleD(interp,targv[j],&GlobalAP[i].SQ_min[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &GlobalAP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert GlobalAP[i](SQ,min[j]) to double\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter max of the "max_SD" structure factor
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SQ,max",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("could not read list GlobalAP(SQ,max)\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP(SQ,max)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &GlobalAP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in GlobalAP[i](SQ,max)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetDoubleD(interp,targv[j],&GlobalAP[i].SQ_max[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &GlobalAP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert GlobalAP[i](SQ,max[j]) to double\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter limits of the "max_SD" structure factor
 */
  Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SQ,limits",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("GlobalAP(SQ,limits) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP[i](SQ,limits)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &GlobalAP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in GlobalAP[i](SQ,limits)\n");
          return TCL_ERROR;
       }
       for (j=0; j < targc ;j++) {
           tcode = Tcl_GetBoolean(interp,targv[j],&GlobalAP[i].SQ_limits[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &GlobalAP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert GlobalAP[i](SQ,limits[j]) to boolean\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter active of the "max_SD" structure factor
 */
  Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SQ,active",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("GlobalAP(SQ,active) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP[i](SQ,active)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
       Splitcode = Tcl_SplitList(interp,Splitargv[i],&targc,&targv);
       if ((Splitcode == TCL_ERROR) || (targc > MAXPAR)) {
          sasfit_free_AP( &GlobalAP );
          Tcl_Free((char *) Splitargv);
          Tcl_Free((char *) targv);
          sasfit_err("wrong # of list elements in GlobalAP[i](SQ,active)\n");
          return TCL_ERROR;
       }
       for (j=0; j < MAXPAR ;j++) GlobalAP[i].SQ_active[j] = 0;
       for (j=0; j < param_count_sq[i] ;j++) {
           tcode = Tcl_GetBoolean(interp,targv[j],&GlobalAP[i].SQ_active[j]);
           if (tcode == TCL_ERROR) {
              sasfit_free_AP( &GlobalAP );
              Tcl_Free((char *) Splitargv);
              Tcl_Free((char *) targv);
              sasfit_err("could not convert GlobalAP[i](SQ,active[j]) to integer\n");
              return TCL_ERROR;
           }
       }
       Tcl_Free((char *) targv);
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter typestr of the "max_SD" structure factor
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SQ,typestr",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("GlobalAP(SQ,typestr) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP[i](SQ,typestr)\n");
       return TCL_ERROR;
    }
    if ( initTypeStr(GlobalAP, max_SD, Splitargv, SQfct) != TCL_OK ) return TCL_ERROR;
    Tcl_Free((char *) Splitargv);



/*
 * read parameter how of the "max_SD" structure factor
 */

  Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"SQ,how",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("GlobalAP(SQ,how) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP[i](SQ_how)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
        tcode = Tcl_GetInt(interp,Splitargv[i],&GlobalAP[i].SQ_how);
        if (tcode == TCL_ERROR) {
           sasfit_free_AP( &GlobalAP );
           Tcl_Free((char *) Splitargv);
           sasfit_err("could not convert GlobalAP[i](SQ_how) to integer\n");
           return TCL_ERROR;
        }
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter substrSDFF of the "max_SD" size distribution
 */

  Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"substrSDFF",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("GlobalAP(substrSDFF) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP[i](substrSDFF)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
        tcode = Tcl_GetBoolean(interp,Splitargv[i],&GlobalAP[i].substrSDFF);
        if (tcode == TCL_ERROR) {
           sasfit_free_AP( &GlobalAP );
           Tcl_Free((char *) Splitargv);
           sasfit_err("could not convert GlobalAP[i](substrSDFF) to boolean\n");
           return TCL_ERROR;
        }
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter calcSDFF of the "max_SD" size distribution
 */

  Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"calcSDFF",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("GlobalAP(calcSDFF) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP[i](calcSDFF)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
        tcode = Tcl_GetBoolean(interp,Splitargv[i],&GlobalAP[i].calcSDFF);
        if (tcode == TCL_ERROR) {
           sasfit_free_AP( &GlobalAP );
           Tcl_Free((char *) Splitargv);
           sasfit_err("could not convert GlobalAP[i](calcSDFF) to boolean\n");
           return TCL_ERROR;
        }
    }
    Tcl_Free((char *) Splitargv);

/*
 * read parameter fitSDFF of the "max_SD" size distribution
 */
  Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"fitSDFF",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("GlobalAP(fitSDFF) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP[i](fitSDFF)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
        tcode = Tcl_GetBoolean(interp,Splitargv[i],&GlobalAP[i].fitSDFF);
        if (tcode == TCL_ERROR) {
           sasfit_free_AP( &GlobalAP );
           Tcl_Free((char *) Splitargv);
           sasfit_err("could not convert GlobalAP[i](fitSDFF) to boolean\n");
           return TCL_ERROR;
        }
    }
    Tcl_Free((char *) Splitargv);



/*
 * read parameter Nth_dataset of the "max_SD" size distribution
 */
  Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"dataset",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("GlobalAP(dataset) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP[i](dataset), argc: %i, maxSD: %i\n",
		       Splitargc, max_SD);
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) {
        tcode = Tcl_GetInt(interp,Splitargv[i],&GlobalAP[i].Nth_dataset);
        if (tcode == TCL_ERROR) {
           sasfit_free_AP( &GlobalAP );
           Tcl_Free((char *) Splitargv);
           sasfit_err("could not convert GlobalAP[i](dataset) to integer\n");
           return TCL_ERROR;
        }
		if (GlobalAP[i].Nth_dataset <= nmultset) {
			GlobalAP[i].calcSDFF = GlobalAP[i].calcSDFF * (1-(*hide)[GlobalAP[i].Nth_dataset-1]);
		} else {
			GlobalAP[i].calcSDFF = 0;
		}
    }
    Tcl_Free((char *) Splitargv);


/*
 * read parameter Nth_datalabel of the "max_SD" size distribution
 */
    Splitcode = Tcl_SplitList(interp,
                              Tcl_GetVar2(interp,argv[1],"datalabel",0),
                              &Splitargc,
                              &Splitargv);
    if (Splitcode == TCL_ERROR) {
       sasfit_free_AP( &GlobalAP );
       sasfit_err("GlobalAP(datalabel) is not a list\n");
       return TCL_ERROR;
    }
    if (Splitargc != max_SD) {
       sasfit_free_AP( &GlobalAP );
       Tcl_Free((char *) Splitargv);
       sasfit_err("wrong # of list elements in GlobalAP[i](datalabel)\n");
       return TCL_ERROR;
    }
    for (i=0;i<max_SD;i++) { strcpy(GlobalAP[i].Nth_datalabel,Splitargv[i]); }
    Tcl_Free((char *) Splitargv);

    (*GlobalAnalyticPar) = GlobalAP;
    (*mmax_SD) = max_SD;

    if (param_count_ff) free_ivector(param_count_ff,0,0);
    if (param_count_sq) free_ivector(param_count_sq,0,0);
    if (param_count_sd) free_ivector(param_count_sd,0,0);

    return TCL_OK;
}

