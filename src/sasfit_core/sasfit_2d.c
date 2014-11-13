/*
 * src/sasfit_core/sasfit_2d.c
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

#include <string.h>
#include "include/sasfit_core.h"

sasfit_2d_param_t	sasfit_2d_param;

void sasfit_2d_init (void)
{
	sasfit_2d_param.num_pix = 0;
	sasfit_2d_param.pixelsize = 0.0;
	sasfit_2d_param.lambda = 0.0;
	sasfit_2d_param.dist = 0.0;
	sasfit_2d_param.min = 0.0;
	sasfit_2d_param.max = 0.0;
	sasfit_2d_param.scale = LOG;
	sasfit_2d_param.mode = AUTO;
	sasfit_2d_param.geom = PINHOLE;
	sasfit_2d_param.qwidth = 0.0;
	sasfit_param_override_init();
}

/**
 * Gets user configuration for 2D mode from the GUI (TCL code).
 * \returns TCL_OK on success, TCL_ERROR if an error occured.
 */
int sasfit_2d_set(Tcl_Interp * interp, const char * argv[])
{
	int tcl_result, tcl_int;
	double tcl_double;
	const char * str = NULL;

	// read the number nPix.
	SF_TCL_GET(int,    argv[3], "nPix",   sasfit_2d_param.num_pix);
	// read the number pixelsize.
	SF_TCL_GET(double, argv[3], "pixres", sasfit_2d_param.pixelsize);
	// read wavelength.
	SF_TCL_GET(double, argv[3], "lambda", sasfit_2d_param.lambda);
	// read sample detector distance.
	SF_TCL_GET(double, argv[3], "SD",     sasfit_2d_param.dist);
	// read min
	SF_TCL_GET(double, argv[3], "min",    sasfit_2d_param.min);
	// read max
	SF_TCL_GET(double, argv[3], "max",    sasfit_2d_param.max);
	// read qwidth
	SF_TCL_GET(double, argv[3], "Qwidth",    sasfit_2d_param.qwidth);
	// read qminbs
	SF_TCL_GET(double, argv[3], "QminBS",    sasfit_2d_param.qminbs);

	str = Tcl_GetVar2(interp, argv[3], "auto", 0);
	if (str) {
		if (strcmp(str, "auto") == 0) sasfit_2d_param.mode = AUTO;
		else                          sasfit_2d_param.mode = MANUAL;
	}
	str = Tcl_GetVar2(interp, argv[3], "scale", 0);
	if (str) {
		if (strcmp(str, "sqrt(y)") == 0)
			sasfit_2d_param.scale = SQRT;
		else if (strcmp(str, "arcsinh(y)") == 0)
			sasfit_2d_param.scale = ARCSINH;
		else if (strcmp(str, "log(y)") == 0)
			sasfit_2d_param.scale = LOG;
        else sasfit_2d_param.scale = LIN;
	}
    str = Tcl_GetVar2(interp, argv[3], "resolution", 0);
	if (str) {
		if (strcmp(str, "slit") == 0)
			sasfit_2d_param.geom = SLIT;
		else if (strcmp(str, "pinhole") == 0)
			sasfit_2d_param.geom  = PINHOLE;
		else sasfit_2d_param.geom = GEOMETRYUNKNOWN;
	}
//	strcpy(Det2DPar.ct,Tcl_GetVar2(interp,argv[3],"ct",0));

	return TCL_OK;
}

double ** dmatrix(int nrl, int nrh, int ncl, int nch);
void free_dmatrix(double ** m, int nrl, int nrh, int ncl, int nch);
void IQ(Tcl_Interp *interp, double Q, double Qres, double *par, double *Ifit,
	double *Isubstract, double *dydpar, int max_SD, sasfit_analytpar *AP,
	int error_type, int Nthdataset, bool *error);

int Sasfit_2DiqCmd(ClientData    clientData,
                   Tcl_Interp *  interp,
                   int           argc,
                   const char ** argv)
{
	sasfit_analytpar * AP;
	int                i, j, k,l, nres, nsigma, max_SD;
	scalar             alambda, wres, tmp;
	char               sBuffer[256];
	scalar             Bx, By, rx, ry, r, Q, DQx,DQy,Qx, Qy, Theta, TwoTheta;
	Tcl_DString        DsBuffer;
	scalar             * h, * Ih, * DIh, * res;
	scalar             **Deth, **DetIth, Detres, Detsubstract;
	scalar             Imin = 0.0, Imax = 0.0; // was uninitialized, see line 198 ff
	int                * lista, ma, mfit,ndata;
	bool               error;
	int                error_type;
	scalar             * a, * dydpar;

	error = FALSE;
	// Det2DPar.calc2D = TRUE;
	sasfit_param_override_init();

	if (argc != 4)
	{
		sasfit_err("wrong # args: shoud be sasfit_2Diq ?analyt_par? "
			"?xyer_data? ?Detector2DParameter?\n");
		return TCL_ERROR;
	}

	if (TCL_ERROR == sasfit_2d_set(interp,argv)) {
		return TCL_ERROR;
	}

	if (TCL_ERROR == get_AP(interp, argv, &AP, &max_SD, &alambda,
		                &error_type,&h,&Ih,&DIh,&res,&ndata))
	{
		return TCL_ERROR;
	}

	for (i=0; i < max_SD; i++) {
		AP[i].fit = FALSE;
		for (j=0; j < MAXPAR; j++) {
			AP[i].SD_active[j] = 0;
			AP[i].FF_active[j] = 0;
		}
	}
	sasfit_ap2paramlist(&lista,&ma,&mfit,&a,AP,NULL,max_SD);

	//free_dvector(Ith,0,ndata-1);
	free_dvector(h,0,ndata-1);
	free_dvector(res,0,ndata-1);
	free_dvector(Ih,0,ndata-1);
	free_dvector(DIh,0,ndata-1);

	dydpar = dvector(0,ma-1);
	Deth   = dmatrix(0,sasfit_2d_param.num_pix-1,0,sasfit_2d_param.num_pix-1);
	DetIth = dmatrix(0,sasfit_2d_param.num_pix-1,0,sasfit_2d_param.num_pix-1);

    Bx = 0.5*(sasfit_2d_param.num_pix-1.);
    By = 0.5*(sasfit_2d_param.num_pix-1.);

	for (i=0; i < sasfit_2d_param.num_pix; i++) {
        rx = (i-Bx)*sasfit_2d_param.pixelsize*1e-3;

        for (j=0; j < sasfit_2d_param.num_pix;j++) {
            ry = (j-By)*sasfit_2d_param.pixelsize*1e-3;

			// Det2DPar.Psi = atan(ry/rx);
			sasfit_param_override_set_psi( atan(ry/rx) );
			r = sqrt(rx*rx+ry*ry);
			TwoTheta = atan(r/sasfit_2d_param.dist);
			Theta = 0.5*TwoTheta;
			Q=4.0*M_PI*sin(Theta)/sasfit_2d_param.lambda;
			Deth[i][j] = Q;
			Detres=-1.;

            switch (sasfit_2d_param.geom) {
               case PINHOLE : {
                    if (sasfit_2d_param.qwidth == 0) {
                        IQ(interp,Deth[i][j],Detres,a,&DetIth[i][j],&Detsubstract,
                            dydpar,max_SD,AP,error_type,0,&error);
                        if (error==TRUE) {
                            free_dmatrix(Deth,0,sasfit_2d_param.num_pix-1,0,sasfit_2d_param.num_pix-1);
                            free_dmatrix(DetIth,0,sasfit_2d_param.num_pix-1,0,sasfit_2d_param.num_pix-1);
                            free_dvector(dydpar,0,ma-1);
                            Tcl_Free((char *) AP);
                            return TCL_ERROR;
                        }
                    } else {
                        nsigma = 2;
                        nres = 10;
                        tmp = 0.0;
                        for (k=-nres;k<=nres,k++;) {
                            for (l=-nres;l<=nres;l++) {
                                Qx = Q*cos(atan(ry/rx));
                                Qy = Q*sin(atan(ry/rx));
                                DQx = k*nsigma*sasfit_2d_param.qwidth/nres;
                                DQy = l*nsigma*sasfit_2d_param.qwidth/nres;
                                wres=1./(2.*M_PI*gsl_pow_2(sasfit_2d_param.qwidth))
                                     *exp(-(DQx*DQx+DQy*DQy)*0.5/gsl_pow_2(sasfit_2d_param.qwidth));

                                sasfit_param_override_set_psi( atan((Qy+DQy)/(Qx+DQx)) );
                                Deth[i][j] = sqrt(gsl_pow_2(Qy+DQy)+gsl_pow_2(Qx+DQx));
                                IQ(interp,Deth[i][j],Detres,a,&DetIth[i][j],&Detsubstract,
                                    dydpar,max_SD,AP,error_type,0,&error);
                                if (error==TRUE) {
                                    free_dmatrix(Deth,0,sasfit_2d_param.num_pix-1,0,sasfit_2d_param.num_pix-1);
                                    free_dmatrix(DetIth,0,sasfit_2d_param.num_pix-1,0,sasfit_2d_param.num_pix-1);
                                    free_dvector(dydpar,0,ma-1);
                                    Tcl_Free((char *) AP);
                                    return TCL_ERROR;
                                }
                                tmp = tmp+wres*DetIth[i][j];
                            }
                        }
                        DetIth[i][j] = tmp*gsl_pow_2(nsigma*sasfit_2d_param.qwidth/nres);
                        Deth[i][j] = Q;
                    }
                    break ;
                    } // end case PINHOLE
                case SLIT : {
                    break;
                    } // end case SLIT
                default: {
                    IQ(interp,Deth[i][j],Detres,a,&DetIth[i][j],&Detsubstract,
                        dydpar,max_SD,AP,error_type,0,&error);
                    if (error==TRUE) {
                        free_dmatrix(Deth,0,sasfit_2d_param.num_pix-1,0,sasfit_2d_param.num_pix-1);
                        free_dmatrix(DetIth,0,sasfit_2d_param.num_pix-1,0,sasfit_2d_param.num_pix-1);
                        free_dvector(dydpar,0,ma-1);
                        Tcl_Free((char *) AP);
                        return TCL_ERROR;
                    }
                }
            } // end switch
            if (i==0 && j==0) {
				Imin = DetIth[i][j];
				Imax = DetIth[i][j];
			} else {
				if ((DetIth[i][j] < Imin) && (sasfit_2d_param.qminbs<Deth[i][j])) {
					Imin=DetIth[i][j];
				}
				if ((DetIth[i][j] > Imax) && (sasfit_2d_param.qminbs<Deth[i][j])) {
					Imax=DetIth[i][j];
				}
			}
        }  // end for-j-loop
	} // end for-i-loop

    if (sasfit_2d_param.mode == AUTO) {
        sprintf(sBuffer,"%e",Imin);
		//  sasfit_err(sBuffer);
        Tcl_SetVar2(interp,"Detector2DIQGraph","min",sBuffer,0);
        sprintf(sBuffer,"%e",Imax);
		//  sasfit_err(sBuffer);
        Tcl_SetVar2(interp,"Detector2DIQGraph","max",sBuffer,0);
    } else {
        Imin = sasfit_2d_param.min;
        Imax = sasfit_2d_param.max;
    }

    Tcl_DStringInit(&DsBuffer);

	for (i=0;i<sasfit_2d_param.num_pix;i++) {
		Tcl_DStringStartSublist(&DsBuffer);
		for (j=0;j<sasfit_2d_param.num_pix;j++) {
            if (sasfit_2d_param.qminbs < Deth[i][j]) {
                DetIth[i][j] = (DetIth[i][j]-Imin)/(Imax-Imin);
            } else {
                DetIth[i][j] = 0.0;
            }

			if (sasfit_2d_param.scale == SQRT) {
				DetIth[i][j] = sqrt(fabs(DetIth[i][j]))*255.;
			}
			else if (sasfit_2d_param.scale == ARCSINH) {
				DetIth[i][j] = log(DetIth[i][j]+hypot(DetIth[i][j],1))*255./log(1+hypot(1,1));
			}
			else if (sasfit_2d_param.scale == LOG) {
				DetIth[i][j]=log(fabs(DetIth[i][j])+0.001);
				DetIth[i][j]=(DetIth[i][j]-log(0.001))/(1.-log(0.001))*255;
			} else
                DetIth[i][j] = fabs(DetIth[i][j])*255.;
			sprintf(sBuffer,"%d",lround(DetIth[i][j]));
			Tcl_DStringAppendElement(&DsBuffer,sBuffer);
		}
		Tcl_DStringEndSublist(&DsBuffer);
	}

	Tcl_DStringResult(interp,&DsBuffer);
	Tcl_DStringFree(&DsBuffer);
	free_dmatrix(Deth,0,sasfit_2d_param.num_pix-1,0,sasfit_2d_param.num_pix-1);
	free_dmatrix(DetIth,0,sasfit_2d_param.num_pix-1,0,sasfit_2d_param.num_pix-1);
	free_dvector(dydpar,0,ma-1);
	Tcl_Free((char *) AP);

	return TCL_OK;
}


