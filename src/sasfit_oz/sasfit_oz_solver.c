/*
 * Author(s) of this file:
 *   Evgeniy Ponomarev (evgeniy.ponomarev@epfl.ch)
 *   Modified 13.09.2013
 *   extended and last modified by Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   2.05.2014
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <sasfit_oz.h>
#include <bool.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_multiroots.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_cblas.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_errno.h>
#include "itlin.h"

#define kb GSL_CONST_MKSA_BOLTZMANN


int cp_gsl_vector_to_array(const gsl_vector *src, double *target, int dimtarget) {
    int i;
    if (src->size != dimtarget) return GSL_FAILURE;
    for (i=0;i<dimtarget;i++) target[i] = gsl_vector_get(src,i);
    return GSL_SUCCESS;
}

int cp_array_diff_to_gsl_vector(double *src1, gsl_vector *src2, gsl_vector *target, int dimsrc) {
    int i,errno;
    errno=0;
    if (dimsrc != target->size) return GSL_FAILURE;
    for (i=0;i<dimsrc;i++)  {
            gsl_vector_set(target,i,src1[i]-gsl_vector_get(src2,i));
    }
    return GSL_SUCCESS;
}

int cp_array_diff_array_to_gsl_vector(double *src1, double *src2,  gsl_vector *target, int dimsrc) {
    int i,errno;
    errno=0;
    if (dimsrc != target->size) return GSL_FAILURE;
    for (i=0;i<dimsrc;i++)  {
            gsl_vector_set(target,i,src1[i]-src2[i]);
    }
    return GSL_SUCCESS;
}


int cp_array_to_gsl_vector(double *src, gsl_vector *target, int dimsrc) {
    int i,errno;
    errno=0;
    if (dimsrc != target->size) return GSL_FAILURE;
    for (i=0;i<dimsrc;i++)  {
            gsl_vector_set(target,i,src[i]);
    }
    return GSL_SUCCESS;
}

int cp_array_to_array(double *src, double *target, int dim) {
    int i;
    for (i=0;i<dim;i++)  {
            target[i]=src[i];
    }
    return GSL_SUCCESS;
}

int OZ_f(gsl_vector * x, void *OZd, gsl_vector * fres) {
    double res;
    sasfit_oz_data *OZdata;
    OZdata = (sasfit_oz_data *) OZd;
    cp_gsl_vector_to_array(x,OZdata->G,OZdata->Npoints);
    res = OZ_step(OZdata);
//    sasfit_out("OZ_f just has been called\n");
    cp_array_diff_to_gsl_vector(OZdata->G,x,fres,OZdata->Npoints);
    return GSL_SUCCESS;
}

#define MINDIMOZ 128
#define NITSTEP 20

#define NP  OZd->Npoints
#define EN  OZd->En
#define r   OZd->r
#define DR  OZd->dr
#define k   OZd->k
#define GF  OZd->Gf
#define G  OZd->G
#define G0  OZd->G0
#define g  OZd->g
#define HR  OZd->h
#define GAMMA_R  OZd->gamma_r
#define g0  OZd->g0
#define OZIN  OZd->in
#define OZOUT OZd->out
#define U   (*OZd->potential)
#define dU_dR OZd->ud
#define UBETA OZd->ubeta
#define PARAM OZd->pPot
#define BRIDGE OZd->Br
#define CAVITY OZd->yr
#define MAYER OZd->fr
#define dr  OZd->dr
#define c  OZd->c
#define CFOLD OZd->cfold
#define CFNEW OZd->cfnew
#define cf OZd->cf
#define ALPHA OZd->alpha
#define sBPGG OZd->alpha
#define aCJVM OZd->alpha
#define fBB OZd->alpha
#define PHI OZd->phi
#define CLOSURE OZd->cl
#define Fswitch OZd->f
#define S OZd->S
#define MIXCOEFF OZd->mixcoeff
#define MAXSTEPS OZd->maxsteps
#define RELERROR OZd->relerror
#define T OZd->T


// Initialization of memory needed for computation

void OZ_init(sasfit_oz_data *OZd) {
   double *tp;
   int i;
   OZd->beta=1.0/(kb*T);
   r      = (double*)malloc((NP)*sizeof(double));
   k      = (double*)malloc((NP)*sizeof(double));
   EN     = (double*)malloc((NP)*sizeof(double));
   G      = (double*)malloc((NP)*sizeof(double));
   HR     = (double*)malloc((NP)*sizeof(double));
   G0     = (double*)malloc((NP)*sizeof(double));
   g      = (double*)malloc((NP)*sizeof(double));
   g0     = (double*)malloc((NP)*sizeof(double));
   c      = (double*)malloc((NP)*sizeof(double));
   cf     = (double*)malloc((NP)*sizeof(double));
   CFOLD  = (double*)malloc((NP)*sizeof(double));
   CFNEW  = (double*)malloc((NP)*sizeof(double));
   GF     = (double*)malloc((NP)*sizeof(double));
   Fswitch= (double*)malloc((NP)*sizeof(double));
   S      = (double*)malloc((NP)*sizeof(double));
   UBETA  = (double*)malloc((NP)*sizeof(double));
   dU_dR  = (double*)malloc((NP)*sizeof(double));
   BRIDGE = (double*)malloc((NP)*sizeof(double));
   CAVITY = (double*)malloc((NP)*sizeof(double));
   MAYER  = (double*)malloc((NP)*sizeof(double));
   OZIN   = (double*)fftw_malloc(sizeof(double)*NP);
   OZOUT  = (double*)fftw_malloc(sizeof(double)*NP);

 //  GAMMA_R  = gsl_vector_view_array(G,NP);
   GAMMA_R  = gsl_vector_alloc(NP);
   for (i=0; i < NP; i++) {
        G[i]=0;
   }
   OZd->pl=fftw_plan_r2r_1d(NP, OZIN, OZOUT, FFTW_RODFT00, FFTW_ESTIMATE);
   }

//Extrapolation routine
double extrapolate (double x1,double x2, double x3, double y1, double y2,double y3)
{  double Ex;
   Ex=-(-pow(x2,2)*x3*y1+x2*pow(x3,2)*y1+pow(x1,2)*x3*y2-x1*pow(x3,2)*y2-pow(x1,2)*x2*y3+x1*pow(x2,2)*y3)/((x1-x2)*(x1-x3)*(x2-x3));
   return Ex;
}


int OZ_first_order_divided_difference (sasfit_oz_data *OZd, double *y, double *x, double *res) {
    int i, j;
    int n;
    int kk;
    int l;
    int ms;
    double *u, *v, *Fu, *Fv;
    double Norm;
    gsl_matrix *DDF;
    gsl_matrix *DDFinv;
    gsl_permutation *perm;
    gsl_vector *F_xn, *xn;

    DDF = gsl_matrix_calloc(NP,NP);
    DDFinv = gsl_matrix_calloc(NP,NP);
    F_xn = gsl_vector_calloc(NP);
    xn = gsl_vector_calloc(NP);

    u = (double*)malloc((NP)*sizeof(double));
    v = (double*)malloc((NP)*sizeof(double));
    Fu = (double*)malloc((NP)*sizeof(double));
    Fv = (double*)malloc((NP)*sizeof(double));

    gsl_set_error_handler_off ();
    for (n=0;n<NP;n++) {
        for (kk=0; kk<=n; kk++) {
            u[kk] = y[kk];
            v[kk] = x[kk];
        }
        v[n] = x[n];
        for (kk=n+1; kk<NP; kk++) {
            u[kk] = x[kk];
            v[kk] = y[kk];
        }
        cp_array_to_array(u,G,NP);
        Norm=OZ_step(OZd);
        cp_array_to_array(G,Fu,NP);
        cp_array_to_array(v,G,NP);
        Norm=OZ_step(OZd);
        cp_array_to_array(G,Fv,NP);
//      DDF = [I - L(Fxn,xn)]
//      DDFinv = DDF^-1
        if ((y[n]-x[n])!=0) {
            for (l=0;l<NP;l++) {
                gsl_matrix_set(DDF,l,n,-(Fu[l]-Fv[l])/(y[n]-x[n]));
            }
            gsl_matrix_set(DDF,n,n,1-(Fu[n]-Fv[n])/(y[n]-x[n]));
        } else {
            sasfit_out("poblems in calculating difided difference operator due to division by 0\n");
            for (l=0;l<NP;l++) {
                gsl_matrix_set(DDF,l,n,-0);
            }
                gsl_matrix_set(DDF,n,n,1-0);
        }
    }
    perm=gsl_permutation_alloc(NP);
    gsl_linalg_LU_decomp(DDF, perm, &ms);
    gsl_linalg_LU_invert(DDF, perm, DDFinv);

    cp_array_to_gsl_vector(x,xn,NP);
    cp_array_diff_array_to_gsl_vector(y,x,F_xn,NP);

    gsl_blas_dgemv(CblasNoTrans,1,DDFinv,F_xn,1,xn);
    cp_gsl_vector_to_array(xn,res,NP);

    gsl_matrix_free(DDF);
    gsl_matrix_free(DDFinv);
    gsl_permutation_free(perm);
    gsl_vector_free(F_xn);
    gsl_vector_free(xn);
    free(u);
    free(v);
    free(Fu);
    free(Fv);

}

int OZ_solver_by_iteration(sasfit_oz_data *OZd, sasfit_oz_root_algorithms algorithm) {
    double err, errold, Norm, Normold;
    double alpha, beta, gama;
    double *xn, *yn, *zn, *Tx, *Ty, *Tz;
    double nsoliparam[5], tol[2];
    int j,n,iloop,ierr;
    Normold=1;
    err=2*RELERROR;
    switch (algorithm) {
        case Picard_iteration:
                n = 0;
                while (OZd->it < MAXSTEPS && err > RELERROR ) {
                    n++;
                    Norm = OZ_step(OZd);
                    err = fabs((Norm-Normold)/Norm);
                    Normold=Norm;
                    if ((n % 10)==0)
                        sasfit_out("iterations: %d, calls of OZ_step=%d, err=%g\n",n,OZd->it,err);
                    if (Norm != Norm) {
                        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
                        for (j=0; j < NP; j++) {
                            G[j]=0.0;
                        }
                    }
                }
                break;
        case Krasnoselskij_iteration:
                xn = (double*)malloc((NP)*sizeof(double));
                n=0;
                while (OZd->it < MAXSTEPS && err > RELERROR ) {
                    cp_array_to_array(G,xn,NP);
                    n++;
                    Norm = OZ_step(OZd);
                    err = fabs((Norm-Normold)/Norm);
                    if ((n % 10)==0)
                        sasfit_out("iterations: %d, calls of OZ_step=%d, err=%g\n",n,OZd->it,err);
                    Normold=Norm;
                    alpha=fabs(MIXCOEFF);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-alpha)*xn[j]+alpha*G[j];
                    }
                    if (Norm != Norm) {
                        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
                        for (j=0; j < NP; j++) {
                            G[j]=0.0;
                        }
                    }
                }
                free(xn);
                break;
        case Mann_iteration:
                xn = (double*)malloc((NP)*sizeof(double));
                Tx = (double*)malloc((NP)*sizeof(double));
                n=0;
                iloop = 0;
                while (OZd->it < MAXSTEPS && err > RELERROR ) {
                    errold=err;
                    cp_array_to_array(G,xn,NP);
                    n++;
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Tx,NP);
                    err = fabs((Norm-Normold)/Norm);
                    if (MIXCOEFF > 0) {
                        alpha=1./(1+MIXCOEFF*n);
                        beta=1./(1.+MIXCOEFF*MIXCOEFF*n*n);
                        alpha=(1-MIXCOEFF)*exp(-log10(err/RELERROR))+MIXCOEFF;
                        if (alpha>1) alpha =1.0;
                        beta=alpha;
                    } else {
                        if (err < errold) {
                            iloop++;
                            iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                        } else {
                            iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                        }
                        alpha=-MIXCOEFF+(1.0*iloop)/(1.0*NITSTEP)*(1+MIXCOEFF);
                        beta=alpha;
                    }
                    if ((n % 10)==0)
                        sasfit_out("iterations: %d, calls of OZ_step=%d, err=%g, alpha=%g\n",n,OZd->it,err,alpha);
                    Normold=Norm;
                    for (j=0; j < NP; j++) {
                            G[j]=(1-alpha)*xn[j]+alpha*Tx[j];
                    }
                    if (Norm != Norm) {
                        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
                        for (j=0; j < NP; j++) {
                            G[j]=0.0;
                        }
                    }
                }
                free(xn);
                free(Tx);
                break;
        case PMH_iteration:
                xn = (double*)malloc((NP)*sizeof(double));
                Tx = (double*)malloc((NP)*sizeof(double));
                yn = (double*)malloc((NP)*sizeof(double));
                Ty = (double*)malloc((NP)*sizeof(double));
                n=0;
                iloop = 0;
                while (OZd->it < MAXSTEPS && err > RELERROR ) {
                    errold=err;
                    cp_array_to_array(G,xn,NP);
                    n++;
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Tx,NP);
                    err = fabs((Norm-Normold)/Norm);
                    Normold=Norm;
                    if (MIXCOEFF > 0) {
                        alpha=1./(1+MIXCOEFF*n);
                        beta=1./(1.+MIXCOEFF*MIXCOEFF*n*n);
                        alpha=(1-MIXCOEFF)*exp(-log10(err/RELERROR))+MIXCOEFF;
                        if (alpha>1) alpha =1.0;
                        beta=alpha;
                    } else {
                        if (err < errold) {
                            iloop++;
                            iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                        } else {
                            iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                        }
                        alpha=-MIXCOEFF+(1.0*iloop)/(1.0*NITSTEP)*(1+MIXCOEFF);
                        beta=alpha;
                    }
                    if ((n % 10)==0)
                        sasfit_out("iterations: %d, calls of OZ_step=%d, err=%g, alpha=%g\n",n,OZd->it,err,alpha);
                    for (j=0; j < NP; j++) {
                            yn[j]=(1-beta)*xn[j]+beta*Tx[j];
                    }
                    cp_array_to_array(yn,G,NP);
                    Norm = OZ_step(OZd);
                    if (Norm != Norm) {
                        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
                        for (j=0; j < NP; j++) {
                            G[j]=0.0;
                        }
                    }
                }
                free(xn);
                free(Tx);
                free(yn);
                free(Ty);
                break;
        case MannII_iteration:
                errold=err;
                xn = (double*)malloc((NP)*sizeof(double));
                Tx = (double*)malloc((NP)*sizeof(double));
                yn = (double*)malloc((NP)*sizeof(double));
                Ty = (double*)malloc((NP)*sizeof(double));
                n=0;
                iloop = 0;
                while (OZd->it < MAXSTEPS && err > RELERROR ) {
                    errold=err;
                    cp_array_to_array(G,xn,NP);
                    n++;
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Tx,NP);
                    err = fabs((Norm-Normold)/Norm);
                    Normold=Norm;
                    if (MIXCOEFF > 0) {
                        alpha=1./(1+MIXCOEFF*n);
                        beta=1./(1.+MIXCOEFF*MIXCOEFF*n*n);
                        alpha=(1-MIXCOEFF)*exp(-log10(err/RELERROR))+MIXCOEFF;
                        if (alpha>1) alpha =1.0;
                        beta=alpha;
                    } else {
                        if (err < errold) {
                            iloop++;
                            iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                        } else {
                            iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                        }
                        alpha=-MIXCOEFF+(1.0*iloop)/(1.0*NITSTEP)*(1+MIXCOEFF);
                        beta=alpha;
                    }
                    if ((n % 10)==0)
                        sasfit_out("iterations: %d, calls of OZ_step=%d, err=%g, alpha=%g\n",n,OZd->it,err,alpha);
                    for (j=0; j < NP; j++) {
                            yn[j]=(1-beta)*xn[j]+beta*Tx[j];
                    }
                    cp_array_to_array(yn,G,NP);
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Ty,NP);
                    for (j=0; j < NP; j++) {
                            xn[j]=(1-alpha)*yn[j]+alpha*Ty[j];
                    }
                    cp_array_to_array(xn,G,NP);

                    if (Norm != Norm) {
                        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
                        for (j=0; j < NP; j++) {
                            G[j]=0.0;
                        }
                    }
                }
                free(xn);
                free(Tx);
                free(yn);
                free(Ty);
                break;
        case S_iteration:
                Tx = (double*)malloc((NP)*sizeof(double));
                Ty = (double*)malloc((NP)*sizeof(double));
                xn = (double*)malloc((NP)*sizeof(double));

                n=0;
                iloop = 0;
                while (OZd->it < MAXSTEPS && err > RELERROR ) {
                    errold=err;
                    n++;
                    cp_array_to_array(G,xn,NP);
                    Norm = OZ_step(OZd);
                    err = fabs((Norm-Normold)/Norm);
                    if (MIXCOEFF > 0) {
                        alpha=1./(1+MIXCOEFF*n);
                        beta=1./(1.+MIXCOEFF*MIXCOEFF*n*n);
                        alpha=(1-MIXCOEFF)*exp(-log10(err/RELERROR))+MIXCOEFF;
                        if (alpha>1) alpha =1.0;
                        beta=alpha;
                    } else {
                        if (err < errold) {
                            iloop++;
                            iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                        } else {
                            iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                        }
                        alpha=-MIXCOEFF+(1.0*iloop)/(1.0*NITSTEP)*(1+MIXCOEFF);
                        beta=alpha;
                    }
                    if ((n % 10)==0)
                        sasfit_out("iterations: %d, calls of OZ_step=%d, err=%g, alpha=%g\n",n,OZd->it,err,alpha);
                    cp_array_to_array(G,Tx,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-beta)*xn[j]+beta*Tx[j];
                    }
                    Norm = OZ_step(OZd);
//                    cp_array_to_array(G,Ty,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-alpha)*Tx[j]+alpha*G[j];
                    }
                    err = fabs((Norm-Normold)/Norm);
                    Normold=Norm;
                    if (Norm != Norm) {
                        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
                        for (j=0; j < NP; j++) {
                            G[j]=0.0;
                        }
                    }
                }
                free(xn);
                free(Tx);
                free(Ty);
                Norm = OZ_step(OZd);
                break;
        case Ishikawa_iteration:
                xn = (double*)malloc((NP)*sizeof(double));
                n=0;
                iloop = 0;
                while (OZd->it < MAXSTEPS && err > RELERROR ) {
                    errold=err;
                    n++;
                    cp_array_to_array(G,xn,NP);
                    Norm = OZ_step(OZd);
                    err = fabs((Norm-Normold)/Norm);
                    if (MIXCOEFF > 0) {
                        alpha=1./(1+MIXCOEFF*n);
                        beta=1./(1.+MIXCOEFF*MIXCOEFF*n*n);
                        alpha=(1-MIXCOEFF)*exp(-log10(err/RELERROR))+MIXCOEFF;
                        if (alpha>1) alpha =1.0;
                        beta=alpha;
                    } else {
                        if (err < errold) {
                            iloop++;
                            iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                        } else {
                            iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                        }
                        alpha=-MIXCOEFF+(1.0*iloop)/(1.0*NITSTEP)*(1+MIXCOEFF);
                        beta=alpha;
                    }
                    if ((n % 10)==0)
                        sasfit_out("iterations: %d, calls of OZ_step=%d, err=%g, alpha=%g\n",n,OZd->it,err,alpha);
                    for (j=0; j < NP; j++) {
                            G[j]=(1.0-beta)*xn[j]+beta*G[j];
                    }
                    Norm = OZ_step(OZd);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-alpha)*xn[j]+alpha*G[j];
                    }
                    err = fabs((Norm-Normold)/Norm);
                    Normold=Norm;
                    if (Norm != Norm) {
                        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
                        for (j=0; j < NP; j++) {
                            G[j]=0.0;
                        }
                    }
                }
                Norm = OZ_step(OZd);
                free(xn);
                break;
        case SP_iteration:
                Tx = (double*)malloc((NP)*sizeof(double));
                Ty = (double*)malloc((NP)*sizeof(double));
                Tz = (double*)malloc((NP)*sizeof(double));
                xn = (double*)malloc((NP)*sizeof(double));
                yn = (double*)malloc((NP)*sizeof(double));
                zn = (double*)malloc((NP)*sizeof(double));

                n=0;
                iloop=0;
                while (OZd->it < MAXSTEPS && err > RELERROR ) {
                    n++;
                    errold=err;
                    cp_array_to_array(G,xn,NP);
                    Norm = OZ_step(OZd);
                    err = fabs((Norm-Normold)/Norm);
                    if (MIXCOEFF > 0) {
                        alpha=1./(1+MIXCOEFF*n);
                        beta=1./(1.+MIXCOEFF*MIXCOEFF*n*n);
                        alpha=(1-MIXCOEFF)*exp(-log10(err/RELERROR))+MIXCOEFF;
                        if (alpha>1) alpha =1.0;
                        beta=alpha;
                        gama=alpha;
                    } else {
                        if (err < errold) {
                            iloop++;
                            iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                        } else {
                            iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                        }
                        alpha=-MIXCOEFF+(1.0*iloop)/(1.0*NITSTEP)*(1+MIXCOEFF);
                        beta=alpha;
                        gama=alpha;
                    }
                    if ((n % 10)==0)
                        sasfit_out("iterations: %d, calls of OZ_step=%d, err=%g, alpha=%g\n",n,OZd->it,err,alpha);
                    cp_array_to_array(G,Tx,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-gama)*xn[j]+gama*Tx[j];
                            zn[j]= G[j];
                    }
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Tz,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-beta)*zn[j]+beta*Tz[j];
                            yn[j] = G[j];
                    }
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Ty,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-alpha)*yn[j]+alpha*Ty[j];
                            xn[j] = G[j];
                    }
                    err = fabs((Norm-Normold)/Norm);
                    Normold=Norm;
                    if (Norm != Norm) {
                        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
                        for (j=0; j < NP; j++) {
                            G[j]=0.0;
                        }
                    }
                }
                free(xn);
                free(yn);
                free(zn);
                free(Tx);
                free(Ty);
                free(Tz);
                Norm = OZ_step(OZd);
                break;
        case Noor_iteration:
                Tx = (double*)malloc((NP)*sizeof(double));
                Ty = (double*)malloc((NP)*sizeof(double));
                Tz = (double*)malloc((NP)*sizeof(double));
                xn = (double*)malloc((NP)*sizeof(double));
                yn = (double*)malloc((NP)*sizeof(double));
                zn = (double*)malloc((NP)*sizeof(double));

                n=0;
                iloop=0;
                while (OZd->it < MAXSTEPS && err > RELERROR ) {
                    errold=err;
                    n++;
                    cp_array_to_array(G,xn,NP);
                    Norm = OZ_step(OZd);
                    err = fabs((Norm-Normold)/Norm);
                    if (MIXCOEFF > 0) {
                        alpha=1./(1+MIXCOEFF*n);
                        beta=1./(1.+MIXCOEFF*MIXCOEFF*n*n);
                        alpha=(1-MIXCOEFF)*exp(-log10(err/RELERROR))+MIXCOEFF;
                        if (alpha>1) alpha =1.0;
                        beta=alpha;
                        gama=alpha;
                    } else {
                        if (err < errold) {
                            iloop++;
                            iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                        } else {
                            iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                        }
                        alpha=-MIXCOEFF+(1.0*iloop)/(1.0*NITSTEP)*(1+MIXCOEFF);
                        beta=alpha;
                        gama=alpha;
                    }
                    if ((n % 10)==0)
                        sasfit_out("iterations: %d, calls of OZ_step=%d, err=%g, alpha=%g\n",n,OZd->it,err,alpha);
                    cp_array_to_array(G,Tx,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-gama)*xn[j]+gama*Tx[j];
                            zn[j]= G[j];
                    }
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Tz,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-beta)*xn[j]+beta*Tz[j];
                            yn[j] = G[j];
                    }
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Ty,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-alpha)*xn[j]+alpha*Ty[j];
                            xn[j] = G[j];
                    }
                    err = fabs((Norm-Normold)/Norm);
                    Normold=Norm;
                    if (Norm != Norm) {
                        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
                        for (j=0; j < NP; j++) {
                            G[j]=0.0;
                        }
                    }
                }
                free(xn);
                free(yn);
                free(zn);
                free(Tx);
                free(Ty);
                free(Tz);
                Norm = OZ_step(OZd);
                break;
        case CR_iteration:
                Tx = (double*)malloc((NP)*sizeof(double));
                Ty = (double*)malloc((NP)*sizeof(double));
                Tz = (double*)malloc((NP)*sizeof(double));
                xn = (double*)malloc((NP)*sizeof(double));
                yn = (double*)malloc((NP)*sizeof(double));
                zn = (double*)malloc((NP)*sizeof(double));

                n=0;
                iloop=0;
                while (OZd->it < MAXSTEPS && err > RELERROR ) {
                    errold=err;
                    n++;
                    cp_array_to_array(G,xn,NP);
                    Norm = OZ_step(OZd);
                    err = fabs((Norm-Normold)/Norm);
                    Normold=Norm;
                    if (MIXCOEFF > 0) {
                        alpha=1./(1+MIXCOEFF*n);
                        beta=1./(1.+MIXCOEFF*MIXCOEFF*n*n);
                        alpha=(1-MIXCOEFF)*exp(-log10(err/RELERROR))+MIXCOEFF;
                        if (alpha>1) alpha =1.0;
                        beta=alpha;
                        gama=alpha;
                    } else {
                        if (err < errold) {
                            iloop++;
                            iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                        } else {
                            iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                        }
                        alpha=-MIXCOEFF+(1.0*iloop)/(1.0*NITSTEP)*(1+MIXCOEFF);
                        beta=alpha;
                        gama=alpha;
                    }
                    if ((n % 10)==0)
                        sasfit_out("iterations: %d, calls of OZ_step=%d, err=%g, alpha=%g\n",n,OZd->it,err,alpha);
                    cp_array_to_array(G,Tx,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-gama)*xn[j]+gama*Tx[j];
                            zn[j]= G[j];
                    }
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Tz,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-beta)*Tx[j]+beta*Tz[j];
                            yn[j] = G[j];
                    }
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Ty,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-alpha)*yn[j]+alpha*Ty[j];
                            xn[j] = G[j];
                    }
                    if (Norm != Norm) {
                        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
                        for (j=0; j < NP; j++) {
                            G[j]=0.0;
                        }
                    }
                }
                free(xn);
                free(yn);
                free(zn);
                free(Tx);
                free(Ty);
                free(Tz);
                Norm = OZ_step(OZd);
                break;
        case PicardS_iteration:
                Tx = (double*)malloc((NP)*sizeof(double));
                Ty = (double*)malloc((NP)*sizeof(double));
                Tz = (double*)malloc((NP)*sizeof(double));
                xn = (double*)malloc((NP)*sizeof(double));
                yn = (double*)malloc((NP)*sizeof(double));
                zn = (double*)malloc((NP)*sizeof(double));

                n=0;
                iloop=0;
                while (OZd->it < MAXSTEPS && err > RELERROR ) {
                    n++;
                    errold=err;
                    cp_array_to_array(G,xn,NP);
                    Norm = OZ_step(OZd);
                    err = fabs((Norm-Normold)/Norm);
                    if (MIXCOEFF > 0) {
                        alpha=1./(1+MIXCOEFF*n);
                        beta=1./(1.+MIXCOEFF*MIXCOEFF*n*n);
                        alpha=(1-MIXCOEFF)*exp(-log10(err/RELERROR))+MIXCOEFF;
                        if (alpha>1) alpha =1.0;
                        beta=alpha;
                        gama=alpha;
                    } else {
                        if (err < errold) {
                            iloop++;
                            iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                        } else {
                            iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                        }
                        alpha=-MIXCOEFF+(1.0*iloop)/(1.0*NITSTEP)*(1+MIXCOEFF);
                        beta=alpha;
                        gama=alpha;
                    }
                    if ((n % 10)==0)
                        sasfit_out("iterations: %d, calls of OZ_step=%d, err=%g, alpha=%g\n",n,OZd->it,err,alpha);
                    cp_array_to_array(G,Tx,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-beta)*xn[j]+beta*Tx[j];
                            zn[j]= G[j];
                    }
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Tz,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-alpha)*Tx[j]+alpha*Tz[j];
                            yn[j] = G[j];
                    }
                    Norm = OZ_step(OZd);
                    err = fabs((Norm-Normold)/Norm);
                    Normold=Norm;
                    if (Norm != Norm) {
                        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
                        for (j=0; j < NP; j++) {
                            G[j]=0.0;
                        }
                    }
                }
                free(xn);
                free(yn);
                free(zn);
                free(Tx);
                free(Ty);
                free(Tz);
                Norm = OZ_step(OZd);
                break;
        case Sstar_iteration:
                Tx = (double*)malloc((NP)*sizeof(double));
                Ty = (double*)malloc((NP)*sizeof(double));
                Tz = (double*)malloc((NP)*sizeof(double));
                xn = (double*)malloc((NP)*sizeof(double));
                yn = (double*)malloc((NP)*sizeof(double));
                zn = (double*)malloc((NP)*sizeof(double));
                n=0;
                iloop=0;
                while (OZd->it < MAXSTEPS && err > RELERROR ) {
                    n++;
                    errold=err;
                    cp_array_to_array(G,xn,NP);
                    Norm = OZ_step(OZd);
                    err = fabs((Norm-Normold)/Norm);
                    if (MIXCOEFF > 0) {
                        alpha=1./(1+MIXCOEFF*n);
                        beta=1./(1.+MIXCOEFF*MIXCOEFF*n*n);
                        alpha=(1-MIXCOEFF)*exp(-log10(err/RELERROR))+MIXCOEFF;
                        beta=alpha;
                        gama=alpha;
                    } else {
                        if (err < errold) {
                            iloop++;
                            iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                        } else {
                            iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                        }
                        alpha=-MIXCOEFF+(1.0*iloop)/(1.0*NITSTEP)*(1+MIXCOEFF);
                        beta=alpha;
                        gama=alpha;
                    }
                    if ((n % 10)==0)
                        sasfit_out("iterations: %d, calls of OZ_step=%d, err=%g, alpha=%g\n",n,OZd->it,err,alpha);
                    cp_array_to_array(G,Tx,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-gama)*xn[j]+gama*Tx[j];
                            zn[j]= G[j];
                    }
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Tz,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-beta)*Tx[j]+beta*Tz[j];
                            yn[j] = G[j];
                    }
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Ty,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-alpha)*Tx[j]+alpha*Ty[j];
                            xn[j] = G[j];
                    }
                    err = fabs((Norm-Normold)/Norm);
                    Normold=Norm;
                    if (Norm != Norm) {
                        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
                        for (j=0; j < NP; j++) {
                            G[j]=0.0;
                        }
                    }
                }
                free(xn);
                free(yn);
                free(zn);
                free(Tx);
                free(Ty);
                free(Tz);
                Norm = OZ_step(OZd);
                break;
        case Steffensen_iteration:
                Tx = (double*)malloc((NP)*sizeof(double));
                Ty = (double*)malloc((NP)*sizeof(double));
                xn = (double*)malloc((NP)*sizeof(double));
                yn = (double*)malloc((NP)*sizeof(double));
                zn = (double*)malloc((NP)*sizeof(double));
                iloop = 0;
                while (OZd->it < MAXSTEPS && err > RELERROR ) {
                    iloop++;
                    cp_array_to_array(G,xn,NP);
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Tx,NP);
                    Norm = OZ_step(OZd);
                    err = fabs((Norm-Normold)/Norm);
                    cp_array_to_array(G,Ty,NP);

                    sasfit_out("loop: %d , error=%g\nnext step divided difference routine\n",iloop,err);
                    OZ_first_order_divided_difference(OZd,Ty,Tx,xn);
                    sasfit_out("up to now the number of OZ_step calls are: %d\n",OZd->it);

                    cp_array_to_array(xn,G,NP);

                    Normold=Norm;
                    if (Norm != Norm) {
                        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
                        for (j=0; j < NP; j++) {
                            G[j]=0.0;
                        }
                    }
                }
                free(xn);
                free(yn);
                free(zn);
                free(Tx);
                free(Ty);
                break;
        case NGMRES:
                xn = (double*)malloc((NP)*sizeof(double));
                Tx = (double*)malloc((NP)*sizeof(double));
                cp_array_to_array(G,xn,NP);
                nsoliparam[0]=240;
                nsoliparam[1]=480;
                nsoliparam[2]=-0.1;
                nsoliparam[3]=1;
                nsoliparam[4]=20;
                tol[0]=RELERROR;
                tol[1]=RELERROR;
                nsoli(xn,&OZ_step,OZd,tol,nsoliparam,Tx,&ierr);
                cp_array_to_array(Tx,G,NP);
                sasfit_out("up to now the number of OZ_step calls are: %d\n",OZd->it);
                free(xn);
                free(Tx);
                break;
        case NBiCGSTAB:
                xn = (double*)malloc((NP)*sizeof(double));
                Tx = (double*)malloc((NP)*sizeof(double));
                cp_array_to_array(G,xn,NP);
                nsoliparam[0]=240;
                nsoliparam[1]=480;
                nsoliparam[2]=-0.1;
                nsoliparam[3]=3;
                nsoliparam[4]=20;
                tol[0]=RELERROR;
                tol[1]=RELERROR;
                nsoli(xn,&OZ_step,OZd,tol,nsoliparam,Tx,&ierr);
                cp_array_to_array(Tx,G,NP);
                sasfit_out("up to now the number of OZ_step calls are: %d\n",OZd->it);
                free(xn);
                free(Tx);
                break;
        case NTFQMR:
                xn = (double*)malloc((NP)*sizeof(double));
                Tx = (double*)malloc((NP)*sizeof(double));
                cp_array_to_array(G,xn,NP);
                nsoliparam[0]=20;
                nsoliparam[1]=40;
                nsoliparam[2]=-0.1;
                nsoliparam[3]=4;
                nsoliparam[4]=20;
                tol[0]=RELERROR;
                tol[1]=RELERROR;
                nsoli(xn,&OZ_step,OZd,tol,nsoliparam,Tx,&ierr);
                cp_array_to_array(Tx,G,NP);
                sasfit_out("up to now the number of OZ_step calls are: %d\n",OZd->it);
                free(xn);
                free(Tx);
                break;
    }
}

int OZ_solver_by_gsl_multroot(sasfit_oz_data *OZd,sasfit_oz_root_algorithms algorithm) {
    const gsl_multiroot_fsolver_type *Tgsl;
    gsl_multiroot_fsolver *sgsl;
    gsl_multiroot_function Fgsl;
    int status,j;
    size_t iter;
    double Norm, Normold, err;


    Fgsl.f = &OZ_f;
    Fgsl.n = OZd->Npoints;
    Fgsl.params = OZd;
    switch (algorithm) {
        case dNewton:
                MIXCOEFF = 1.0;
                Tgsl = gsl_multiroot_fsolver_dnewton;
                break;
        case Hybrids:
                MIXCOEFF = 1.0;
                Tgsl = gsl_multiroot_fsolver_hybrids;
                break;
        case Hybrid:
                MIXCOEFF = 1.0;
                Tgsl = gsl_multiroot_fsolver_hybrid;
                break;
        case Broyden:
                MIXCOEFF = 1.0;
                Tgsl = gsl_multiroot_fsolver_broyden;
                break;
        default:
                MIXCOEFF = 1.0;
                Tgsl = gsl_multiroot_fsolver_broyden;
    }
    sasfit_out("initialising root solver: %d\n",algorithm);
    sgsl = gsl_multiroot_fsolver_alloc (Tgsl, Fgsl.n);
    gsl_multiroot_fsolver_set (sgsl, &Fgsl, GAMMA_R);
    iter = 0;
    Normold=1;
    do {
        sasfit_out("starting %d-th iteration\n", iter+1);
        iter++;
        status = gsl_multiroot_fsolver_iterate (sgsl);
        Norm=0;
        for (j=0; j < NP; j++) {
            Norm=gsl_pow_2(S[j])+Norm;
        }
        Norm=sqrt(Norm);
        err = fabs((Norm-Normold)/Norm);
        Normold=Norm;
//        status = gsl_multiroot_test_residual (sgsl->f, RELERROR);
        sasfit_out("iterations: %d, status = %s\n", iter, gsl_strerror (status));
        sasfit_out("up to now the number of OZ_step calls are: %d\n",OZd->it);
        sasfit_out("residual of gamma(r)-OZ(gamma(r)):%g \n",gsl_blas_dasum(sgsl->f));
        sasfit_out("error:%g \n",err);
//        if (status)  { /* check if solver is stuck */
//            break;
//       }
    } while (OZd->it < MAXSTEPS && err > RELERROR );
    gsl_multiroot_fsolver_free (sgsl);
    return 0;
}

//Calling OZ_solver and generating output
void OZ_calculation (sasfit_oz_data *OZd) {
  int igneg;
  OZd->it=0;
  if (CLOSURE==RY || CLOSURE==BPGG || CLOSURE==HMSA || CLOSURE==CJVM || CLOSURE==BB) {
        sasfit_out("Root finding\n");
        root_finding(OZd);
  } else if (CLOSURE==RMSA) {
        CLOSURE=MSA;
        sasfit_out("Solving with MSA-closure first ...");
        OZ_solver(OZd);
        sasfit_out(" done\n");
        CLOSURE = RMSA;
        igneg = 0;
// finding contact point sigma+
        while (EN[igneg] == 0.0 && igneg < NP) {
            igneg++;
        }
// check where g(sigma+) >= 0, than nothing has to be done as RMSA=MSA which has been solved already above
        if (g[igneg] >= 0 || igneg == NP) {
            sasfit_out("g(sigma+)>=0 or g(r) > 0 for all r, i.e. no rescaling necessary\n");
            return;
        }
  } else {
        sasfit_out("Root finding not required\n");
  }
  OZ_solver(OZd);
  OZ_pot_der(OZd);
}


double OZ_step(sasfit_oz_data *OZd) {
    double  err, ro, dk, Sm, Norm, V, Gstar,residualG,tmpr;
    static double Normold=0;
    int i,j;
    double powarg, logarg;
    bool doneB;

    OZ_func_one_t * tmp_potential;
    ro=6*PHI/(M_PI*gsl_pow_3(PARAM[0]));
    dk = M_PI/((NP+1.0)*dr);

    doneB=FALSE;
    OZd->it++;

    if (((OZd->it*OZd->Npoints) % (100*MINDIMOZ))==0) {
        Tcl_EvalEx(OZd->interp,"set OZ(progressbar) 1",-1,TCL_EVAL_DIRECT);
        Tcl_EvalEx(OZd->interp,"update",-1,TCL_EVAL_DIRECT);
    }
    for (i=0; i < NP; i++){
        switch (CLOSURE) {
        case PY:
            BRIDGE[i] = log(1.0+G[i])-G[i];
            doneB=TRUE;
            c[i]=(1.+G[i])*(EN[i]-1.);
            break;
        case HNC:
            BRIDGE[i] = 0.0;
            doneB=TRUE;
            c[i]=-1-G[i]+EN[i]*exp(G[i]);
            break;
        case RHNC:
            c[i]=g0[i]*exp((G[i]-G0[i])-OZd->beta*OZd->pertubation_pot(r[i],T,PARAM))-G[i]-1;
            break;
        case RY:
            if (ALPHA == 0) {
                BRIDGE[i] = log(1.0+G[i])-G[i];
                doneB=TRUE;
                c[i]=(1.+G[i])*(EN[i]-1.);
            } else {
                BRIDGE[i] = -G[i]+log(1.0+((exp(Fswitch[i]*G[i])-1)/Fswitch[i]));
                doneB=TRUE;
                c[i]=EN[i]*(1+(exp(Fswitch[i]*G[i])-1)/Fswitch[i])-G[i]-1;
            }
            break;
        case HMSA:
            Gstar = G[i]-OZd->beta*OZd->attractive_pot(r[i],T,PARAM);
            if (ALPHA == 0) {
                BRIDGE[i] = log(1.0+Gstar)-Gstar;
                doneB=TRUE;
                c[i]=(1.0+Gstar)*(EN[i]-1.0);
            } else {
                BRIDGE[i] = -Gstar+log(1.0+((exp(Fswitch[i]*Gstar)-1)/Fswitch[i]));
                doneB=TRUE;
                c[i]=EN[i]*exp(G[i]+BRIDGE[i])-G[i]-1.0;
            }
            break;
        case Verlet:
            BRIDGE[i] = -(gsl_pow_2(G[i])/(2.0*(1.+4.0/5.0*G[i])));
            doneB=TRUE;
            c[i]=EN[i]*exp(G[i]+BRIDGE[i])-G[i]-1.0;
            break;
        case DH:
            if (OZd->potential == &U_Lennard_Jones) {
                Gstar = G[i]+PARAM[1]*gsl_pow_6(PARAM[0]/r[i])
                            *exp(-PARAM[1]/OZd->beta*gsl_pow_6(PARAM[0]/r[i])/(PHI*M_PI/6.0));
            } else {
                Gstar = G[i]-OZd->beta*OZd->attractive_pot(r[i],T,PARAM);
            }
            BRIDGE[i] = -gsl_pow_2(Gstar)/(2.0*(1.0+(5.0*Gstar+11.0)/(7.0*Gstar+9.0)*Gstar));
            doneB=TRUE;
            c[i]=EN[i]*exp(G[i]+BRIDGE[i])-G[i]-1.0;
            break;
        case CG:
            Gstar = G[i]-OZd->beta*OZd->attractive_pot(r[i],T,PARAM);
            if (Gstar > 0) {
                BRIDGE[i] = -0.5*gsl_pow_2(Gstar)/(1.0+(1.0175-0.275*6*PHI/M_PI)*Gstar);
            } else {
                BRIDGE[i] = -0.5*gsl_pow_2(Gstar);
            }
            doneB=TRUE;
            c[i]=EN[i]*exp(G[i]+BRIDGE[i])-G[i]-1.0;
            break;
        case MS:
            powarg = 1.0+2.0*G[i];
            BRIDGE[i] = GSL_SIGN(powarg)*sqrt(fabs(powarg))-G[i]-1.0;
            doneB=TRUE;
            c[i]=EN[i]*exp(G[i]+BRIDGE[i])-G[i]-1.0;
            break;
        case BPGG:
            powarg = 1.0+sBPGG*G[i];
            BRIDGE[i] = GSL_SIGN(powarg)*pow(fabs(powarg),1.0/sBPGG)-G[i]-1.0;
            doneB=TRUE;
            c[i]=EN[i]*exp(G[i]+BRIDGE[i])-G[i]-1.0;
            break;
        case VM:
            Gstar = G[i]-OZd->beta*OZd->attractive_pot(r[i],T,PARAM);
            powarg = 1.0+2.0*Gstar;
            BRIDGE[i] = GSL_SIGN(powarg)*sqrt(fabs(powarg))-Gstar-1.0;
//            BRIDGE[i] = sqrt(1.0+2.0*Gstar)-Gstar-1.0;
            doneB=TRUE;
            c[i]=EN[i]*exp(G[i]+BRIDGE[i])-G[i]-1.0;
            break;
        case CJVM:
            Gstar = G[i]-OZd->beta*OZd->attractive_pot(r[i],T,PARAM);
            powarg = (1.0+4.0*aCJVM*Gstar);
            BRIDGE[i] = 1.0/(2*aCJVM)*(GSL_SIGN(powarg)*sqrt(fabs(powarg))-1.0-2.0*aCJVM*Gstar);
//            BRIDGE[i] = 1.0/(2*aCJVM)*(sqrt((1.0+4.0*aCJVM*Gstar))-1.0-2.0*aCJVM*Gstar);
            doneB=TRUE;
            c[i]=EN[i]*exp(G[i]+BRIDGE[i])-G[i]-1.0;
            break;
        case BB:
            Gstar = G[i]-OZd->beta*OZd->attractive_pot(r[i],T,PARAM);
            powarg = 1.0+2.0*Gstar+fBB*Gstar*Gstar;
            BRIDGE[i] = GSL_SIGN(powarg)*sqrt(fabs(powarg))-Gstar-1.0;
//           BRIDGE[i] = sqrt(1.0+2.0*Gstar+fBB*Gstar*Gstar)-1.0-Gstar;
            doneB=TRUE;
            c[i]=EN[i]*exp(G[i]+BRIDGE[i])-G[i]-1.0;
            break;
        case SMSA:
            Gstar = G[i]-OZd->beta*OZd->attractive_pot(r[i],T,PARAM);
            BRIDGE[i] = log(1.0+Gstar)-Gstar;
            doneB=TRUE;
            c[i]=EN[i]*exp(OZd->beta*OZd->attractive_pot(r[i],T,PARAM))*(Gstar+1.0)-G[i]-1.0;
            break;
        case MSA:
        case mMSA:
        case RMSA:
            if (EN[i]==0.0) {
                c[i] = -(G[i]+1.0);
            } else {
                if (CLOSURE==MSA || CLOSURE==RMSA) {
                    c[i] = -UBETA[i];
                } else if (CLOSURE==mMSA) {
                    c[i] = MAYER[i];
                }
            }
            break;
        }
        if (G[i]!=G[i]) {
            sasfit_out("i: %d gamma(r)=%g\n",i, G[i]);
            G[i]=0.0;
        }
        cp_array_to_gsl_vector(G,GAMMA_R,NP);

        g[i]= c[i]+G[i]+1.0;

        if (g[i]!=g[i]) {
            sasfit_out("i: %d g(r)=%g\n",i, g[i]);
            g[i]=0.0;
        }
        OZIN[i]=(i+1)*c[i];
    }
 //   OZd->pl=fftw_plan_r2r_1d(NP, OZIN, OZOUT, FFTW_RODFT00, FFTW_ESTIMATE);
    fftw_execute_r2r(OZd->pl,OZIN,OZOUT);

    for (j=0; j < NP; j++) {
        cf[j]=4.0*M_PI*dr*dr*OZOUT[j]/(2.0*dk*(j+1.0));
        CFOLD[j]=cf[j];
        GF[j]=ro*cf[j]*cf[j]/(1.0-ro*cf[j]);
        OZIN[j]=GF[j]*(j+1.0);
    }
//    OZd->pl=fftw_plan_r2r_1d(NP, OZIN, OZOUT, FFTW_RODFT00, FFTW_ESTIMATE);
    fftw_execute_r2r(OZd->pl,OZIN,OZOUT);
    Sm=0;
    residualG = 0;
    for (j=0; j < NP; j++) {
        tmpr=G[i];
        G[j]=4.*M_PI*dk*dk*OZOUT[j]/(2.0*dr*gsl_pow_3(2.0*M_PI)*(j+1.0));
        residualG = residualG+fabs(G[i]-tmpr);
        S[j]=1./(1.-ro*cf[j]);
        HR[j]=c[j]+G[j];
        Sm=gsl_pow_2(S[j])+Sm;
        if (!doneB) {
            if (g[j] > 0 && EN[j] != 0) {
                BRIDGE[j]=log(g[j])+UBETA[j]-G[j];
            } else {
                BRIDGE[j] = 0.0;
            }
        }
        CAVITY[j] = exp(BRIDGE[j]+G[j]);
    }
    return sqrt(Sm);
}

// Iterative solution of Ornstein-Zernike equation
void OZ_solver (sasfit_oz_data *OZd) {
    int i;
    double V, dk;
    OZ_func_one_t * tmp_potential;

    dk = M_PI/((NP+1.0)*dr);
    r[0]=dr;
    k[0]=dk;

    for (i=1; i < NP; i++) {
        r[i]=(r[i-1]+dr);
        k[i]=(k[i-1]+dk);
    }

    if (CLOSURE==RHNC) {
        tmp_potential =OZd->potential;
        OZd->potential=OZd->reference_pot;
        CLOSURE=PY;
        sasfit_out("Solving reference system (HS) with PY-closure first ...");
        OZ_solver(OZd);
        sasfit_out(" done\n");
        CLOSURE = RHNC;
        OZd->potential=tmp_potential;
        for (i=0; i < NP; i++){
            g0[i]=g[i];
            G0[i]=G[i];
        }
    }

    for (i=0; i < NP; i++) {
        Fswitch[i]=1-exp(-ALPHA*r[i]);
        V = U(r[i],T,PARAM);
        UBETA[i]=V*OZd->beta;
        if (CLOSURE==RMSA) {
            if (r[i]<PARAM[0] || V==GSL_POSINF) {
                EN[i]=0.0;
            } else {
                EN[i]=exp(-UBETA[i]);
            }
        } else {
            if (V == GSL_POSINF) {
                EN[i]=0.0;
            } else {
                EN[i]=exp(-UBETA[i]);
            }
        }
        MAYER[i] = EN[i]-1.;
        BRIDGE[i] = 0.0;
        CAVITY[i] = 0.0;
    }

    OZd->it=0;
    switch (OZd->root_algorithm) {
        case Picard_iteration:
                OZ_solver_by_iteration(OZd,Picard_iteration);
                break;
        case Mann_iteration:
                OZ_solver_by_iteration(OZd,Mann_iteration);
                break;
        case Ishikawa_iteration:
                OZ_solver_by_iteration(OZd,Ishikawa_iteration);
                break;
        case Noor_iteration:
                OZ_solver_by_iteration(OZd,Noor_iteration);
                break;
        case S_iteration:
                OZ_solver_by_iteration(OZd,S_iteration);
                break;
        case SP_iteration:
                OZ_solver_by_iteration(OZd,SP_iteration);
                break;
        case CR_iteration:
                OZ_solver_by_iteration(OZd,CR_iteration);
                break;
        case PicardS_iteration:
                OZ_solver_by_iteration(OZd,PicardS_iteration);
                break;
        case PMH_iteration:
                OZ_solver_by_iteration(OZd,PMH_iteration);
                break;
        case MannII_iteration:
                OZ_solver_by_iteration(OZd,MannII_iteration);
                break;
        case Krasnoselskij_iteration:
                OZ_solver_by_iteration(OZd,Krasnoselskij_iteration);
                break;
        case Sstar_iteration:
                OZ_solver_by_iteration(OZd,Sstar_iteration);
                break;
        case Steffensen_iteration:
                OZ_solver_by_iteration(OZd,Steffensen_iteration);
                break;
        case Broyden:
                OZ_solver_by_gsl_multroot(OZd,Broyden);
                break;
        case dNewton:
                OZ_solver_by_gsl_multroot(OZd,dNewton);
                break;
        case Hybrid:
                OZ_solver_by_gsl_multroot(OZd,Hybrid);
                break;
        case Hybrids:
                OZ_solver_by_gsl_multroot(OZd,Hybrids);
                break;
        case NGMRES:
                OZ_solver_by_iteration(OZd,NGMRES);
                break;
        case NTFQMR:
                OZ_solver_by_iteration(OZd,NTFQMR);
                break;
        default:
                sasfit_err("this algorithm is planned to be implemented\n");
    }

    sasfit_out("Needed %d calls to OZ_step\n",OZd->it);
    Tcl_EvalEx(OZd->interp,"set OZ(progressbar) 1",-1,TCL_EVAL_DIRECT);
    Tcl_EvalEx(OZd->interp,"update",-1,TCL_EVAL_DIRECT);


    OZd->Sq0=extrapolate(k[0],k[1],k[2],S[0],S[1],S[2]);
    OZd->gr0=extrapolate(r[0],r[1],r[2],g[0],g[1],g[2]);
    OZd->cr0=extrapolate(r[0],r[1],r[2],c[0],c[1],c[2]);
    OZd->dq=dk;
}

//Routine for calculation derivative from pair potential

void OZ_pot_der(sasfit_oz_data *OZd)
{  int n;

    if (U(r[0],T,PARAM)==GSL_POSINF && U(r[1],T,PARAM)==GSL_POSINF) {
        dU_dR[0]=0;
    } else if (U(r[0],T,PARAM)==GSL_POSINF && U(r[1],T,PARAM)!=GSL_POSINF) {
        dU_dR[0]=GSL_NEGINF;
    } else {
        dU_dR[0]=(U(r[1],T,PARAM)-U(r[0],T,PARAM))/(dr);
    }

    for (n=1; n < (NP-1); n++) {
        if (   (U(r[n  ],T,PARAM)==GSL_POSINF)
            && (U(r[n+1],T,PARAM)==GSL_POSINF)
            && (U(r[n-1],T,PARAM)==GSL_POSINF) ) {
            dU_dR[n]=0;
        } else if (   (U(r[n  ],T,PARAM)==GSL_POSINF)
                   && (U(r[n+1],T,PARAM)!=GSL_POSINF)) {
            dU_dR[n]=GSL_NEGINF;
        } else if (   (U(r[n  ],T,PARAM)!=GSL_POSINF)
                   && (U(r[n+1],T,PARAM)==GSL_POSINF)) {
            dU_dR[n]=GSL_POSINF;
        } else if (   (U(r[n-1],T,PARAM)==GSL_POSINF)
                   && (U(r[n  ],T,PARAM)!=GSL_POSINF)
                   && (U(r[n+1],T,PARAM)!=GSL_POSINF)) {
            dU_dR[n]=(U(r[n+1],T,PARAM)-U(r[n],T,PARAM))/(dr);
        } else{
            dU_dR[n]=(U(r[n+1],T,PARAM)-U(r[n-1],T,PARAM))/(2*dr);
        }
    }

     dU_dR[NP-1]=(U(r[NP-1],T,PARAM)-U(r[NP-2],T,PARAM))/(dr);
 }

//Routine for calculation the difference between compressibilities determined by virial and compressibility routes

double compressibility_calc(double scp, void *params)
{    sasfit_oz_data *OZd;
     int i;
     double iphi;
     double S0, chicp, chivir,Delta_chi, r1,r2,r3,P2,P3, sum2,sum3;
     sum2=0.0;
     sum3=0.0;

   OZd = (sasfit_oz_data *) params;
   ALPHA=scp;

   iphi = PHI;
   OZ_solver(OZd);
   S0=OZd->Sq0;
   r1=6.0*PHI/(M_PI*gsl_pow_3(PARAM[0]));
   chicp=S0*OZd->beta/r1;

   OZ_pot_der(OZd);

   PHI=iphi-iphi*0.01;
   r2=6.0*PHI/(M_PI*gsl_pow_3(PARAM[0]));
   OZ_solver(OZd);
   for (i=0; i<NP-1; i++) {
       if (dU_dR[i] == GSL_NEGINF) {
            if (i>0) {
                sum2=sum2-gsl_pow_3(r[i])*(g[i+1]-g[i-1])/OZd->beta/dr;
            } else {
                sum2=sum2-gsl_pow_3(r[i])*(g[i+1]-g[i])/OZd->beta/dr;
            }
       } else if (dU_dR[i] == GSL_POSINF) {
            if (i>0) {
                sum2=sum2+gsl_pow_3(r[i])*(g[i+1]-g[i-1])/OZd->beta/dr;
            } else {
                sum2=sum2+gsl_pow_3(r[i])*(g[i+1]-g[i])/OZd->beta/dr;
            }
       } else {
            sum2=sum2+(dU_dR[i])*g[i]*gsl_pow_3(r[i+1]);
       }
   }
   P2=(r2/OZd->beta)*(1.-(2.*M_PI*OZd->beta*r2/3.)*dr*sum2);

   PHI=iphi+iphi*0.01;
   r3=6.0*PHI/(M_PI*gsl_pow_3(PARAM[0]));
   OZ_solver(OZd);
   for (i=0; i<NP-1; i++) {
       if (dU_dR[i] == GSL_NEGINF) {
            if (i>0) {
                sum3=sum3-gsl_pow_3(r[i])*(g[i+1]-g[i-1])/OZd->beta/dr;
            } else {
                sum3=sum3-gsl_pow_3(r[i])*(g[i+1]-g[i])/OZd->beta/dr;
            }
       } else if (dU_dR[i] == GSL_POSINF) {
            if (i>0) {
                sum3=sum3+gsl_pow_3(r[i])*(g[i+1]-g[i-1])/OZd->beta/dr;
            } else {
                sum3=sum3+gsl_pow_3(r[i])*(g[i+1]-g[i])/OZd->beta/dr;
            }
       } else {
            sum3=sum3+(dU_dR[i])*g[i]*gsl_pow_3(r[i+1]);
       }
   }
   P3=(r3/OZd->beta)*(1.0-(2.0*M_PI*OZd->beta*r3/3.0)*dr*sum3);
   chivir=(r3-r2)/((P3-P2)*r1);
   PHI=iphi;
   Delta_chi = chicp-chivir;
   if (Delta_chi != Delta_chi) {
        sasfit_out("detected NAN for compressibility, alpha value: %g\n",ALPHA);
        for (i=0; i < NP; i++) {
            G[i]=0.0;
        }
    }
   return Delta_chi;
}
//Routine to minimize the difference between two compressibilities

void rescaleMSA (sasfit_oz_data *OZd) {
    int signchange,i,j;
    double refnew, refold, alpha_left, alpha_right, scp_inter;
    refold=0;
    signchange = 0;
    sasfit_out("\nrescale sigma so that g(sigma'+) becomes >=0: \n");
    sasfit_out("      sigma      |      sigma'+     |      g(sigma'+)     \n");
    sasfit_out("-----------------|------------------|---------------------\n");

    i = 0;
    while (signchange==0 && i<=28) {
        if (CLOSURE==RY || CLOSURE==HMSA){
            refnew=compressibility_calc(100/pow(2,i), OZd);
            alpha_left  = 100/pow(2,i);
            alpha_right = 100/pow(2,i-1);
        }
        if (CLOSURE==BPGG) {
            refnew=compressibility_calc(5.601-0.2*i, OZd);
            alpha_left  = 5.601-0.2*i;
            alpha_right = 5.601-0.2*(i-1);
        }
        if (CLOSURE==CJVM) {
            refnew=compressibility_calc(0.001+0.05*i, OZd);
            alpha_right  = 0.001+0.05*i;
            alpha_left = 0.001+0.05*(i-1);
        }
        if (CLOSURE==BB) {
            refnew=compressibility_calc(-0.5+0.1*i, OZd);
            alpha_right  = -0.5+0.1*i;
            alpha_left = -0.5+0.1*(i-1);
        }
        sasfit_out("          %15g  |   %15g \n", alpha_left , refnew/OZd->beta);

        if ((refnew*refold<0)) {
            signchange=1;
        } else {
            if (abs(refnew) < abs(refold)) {
                if (    CLOSURE==RY
                    ||  CLOSURE==HMSA)   scp_inter=alpha_left;

                if (    CLOSURE==BPGG
                    ||  CLOSURE==CJVM
                    ||  CLOSURE==BB)     scp_inter=alpha_left;
            } else {
                if (    CLOSURE==RY
                    ||  CLOSURE==HMSA)   scp_inter=alpha_right;

                if (    CLOSURE==BPGG
                    ||  CLOSURE==CJVM
                    ||  CLOSURE==BB)     scp_inter=alpha_right;
            }
            refold=refnew;
            if (i==28) {
                sasfit_out("No change of sign in %d steps \n", i);
                sasfit_out("choosen self-consistency parameter:%f\n",scp_inter);
                OZd->alpha=scp_inter;
            }
        }
        i++;
        Tcl_EvalEx(OZd->interp,"set OZ(progressbar) 1",-1,TCL_EVAL_DIRECT);
        Tcl_EvalEx(OZd->interp,"update",-1,TCL_EVAL_DIRECT);
    }
    if (signchange==1) {
        int status;
        int iter;
        int max_iter;
        const gsl_root_fsolver_type *Tt;
        gsl_root_fsolver *s;
        double root = 0.0;
        double x_lo = alpha_left;
        double x_hi = alpha_right;
        gsl_function F;

        F.function = compressibility_calc;
        F.params = OZd;

        Tt = gsl_root_fsolver_brent;
        s = gsl_root_fsolver_alloc (Tt);
        gsl_root_fsolver_set (s, &F, x_lo, x_hi);

        sasfit_out("using %s method\n",gsl_root_fsolver_name (s));
        sasfit_out(" iter |  lower      upper  |    root    err(est)\n");
        iter = 0;
        max_iter = 100;
        do {
            iter++;
            status = gsl_root_fsolver_iterate (s);
            root = gsl_root_fsolver_root (s);
            x_lo = gsl_root_fsolver_x_lower (s);
            x_hi = gsl_root_fsolver_x_upper (s);
            status = gsl_root_test_interval (x_lo, x_hi, 0, 1e-5);
            if (status == GSL_SUCCESS) {
                sasfit_out("Converged:\n");
            }
            sasfit_out("%5d |%.7f, %.7f| %.7f %.7f\n", iter, x_lo, x_hi,  root,  x_hi - x_lo);

            Tcl_EvalEx(OZd->interp,"set OZ(progressbar) 1",-1,TCL_EVAL_DIRECT);
            Tcl_EvalEx(OZd->interp,"update",-1,TCL_EVAL_DIRECT);
        } while (status == GSL_CONTINUE && iter < max_iter);
        OZd->alpha = root;
        sasfit_out("consistency parameter after optimization: %g \n", root);
        gsl_root_fsolver_free (s);
    }
}
//Routine to minimize the difference between two compressibilities

void root_finding (sasfit_oz_data *OZd) {
    int signchange,i,j;
    double refnew, refold, alpha_left, alpha_right, scp_inter;
    refold=0;
    signchange = 0;
    sasfit_out("\nSearching for the interval where sign changes: \n");
    sasfit_out("%s |  %s \n","self-consistency parameter" , "(chicomp-chivir)/beta");
    sasfit_out("---------------------------|-------------------------\n");

    i = 0;
    while (signchange==0 && i<=28) {
        if (CLOSURE==RY || CLOSURE==HMSA){
            refnew=compressibility_calc(100/pow(2,i), OZd);
            alpha_left  = 100/pow(2,i);
            alpha_right = 100/pow(2,i-1);
        }
        if (CLOSURE==BPGG) {
            refnew=compressibility_calc(5.601-0.2*i, OZd);
            alpha_left  = 5.601-0.2*i;
            alpha_right = 5.601-0.2*(i-1);
        }
        if (CLOSURE==CJVM) {
            refnew=compressibility_calc(0.001+0.05*i, OZd);
            alpha_right  = 0.001+0.05*i;
            alpha_left = 0.001+0.05*(i-1);
        }
        if (CLOSURE==BB) {
            refnew=compressibility_calc(-0.5+0.1*i, OZd);
            alpha_right  = -0.5+0.1*i;
            alpha_left = -0.5+0.1*(i-1);
        }
        sasfit_out("          %15g  |   %15g \n", alpha_left , refnew/OZd->beta);

        if ((refnew*refold<0)) {
            signchange=1;
        } else {
            if (abs(refnew) < abs(refold)) {
                if (    CLOSURE==RY
                    ||  CLOSURE==HMSA)   scp_inter=alpha_left;

                if (    CLOSURE==BPGG
                    ||  CLOSURE==CJVM
                    ||  CLOSURE==BB)     scp_inter=alpha_left;
            } else {
                if (    CLOSURE==RY
                    ||  CLOSURE==HMSA)   scp_inter=alpha_right;

                if (    CLOSURE==BPGG
                    ||  CLOSURE==CJVM
                    ||  CLOSURE==BB)     scp_inter=alpha_right;
            }
            refold=refnew;
            if (i==28) {
                sasfit_out("No change of sign in %d steps \n", i);
                sasfit_out("choosen self-consistency parameter:%f\n",scp_inter);
                OZd->alpha=scp_inter;
            }
        }
        i++;
        Tcl_EvalEx(OZd->interp,"set OZ(progressbar) 1",-1,TCL_EVAL_DIRECT);
        Tcl_EvalEx(OZd->interp,"update",-1,TCL_EVAL_DIRECT);
    }
    if (signchange==1) {
        int status;
        int iter;
        int max_iter;
        const gsl_root_fsolver_type *Tt;
        gsl_root_fsolver *s;
        double root = 0.0;
        double x_lo = alpha_left;
        double x_hi = alpha_right;
        gsl_function F;

        F.function = compressibility_calc;
        F.params = OZd;

        Tt = gsl_root_fsolver_brent;
        s = gsl_root_fsolver_alloc (Tt);
        gsl_root_fsolver_set (s, &F, x_lo, x_hi);

        sasfit_out("using %s method\n",gsl_root_fsolver_name (s));
        sasfit_out(" iter |  lower      upper  |    root    err(est)\n");
        iter = 0;
        max_iter = 100;
        do {
            iter++;
            status = gsl_root_fsolver_iterate (s);
            root = gsl_root_fsolver_root (s);
            x_lo = gsl_root_fsolver_x_lower (s);
            x_hi = gsl_root_fsolver_x_upper (s);
            status = gsl_root_test_interval (x_lo, x_hi, 0, 1e-5);
            if (status == GSL_SUCCESS) {
                sasfit_out("Converged:\n");
            }
            sasfit_out("%5d |%.7f, %.7f| %.7f %.7f\n", iter, x_lo, x_hi,  root,  x_hi - x_lo);

            Tcl_EvalEx(OZd->interp,"set OZ(progressbar) 1",-1,TCL_EVAL_DIRECT);
            Tcl_EvalEx(OZd->interp,"update",-1,TCL_EVAL_DIRECT);
        } while (status == GSL_CONTINUE && iter < max_iter);
        OZd->alpha = root;
        sasfit_out("consistency parameter after optimization: %g \n", root);
        gsl_root_fsolver_free (s);
    }
}

//Release of memory used for computation
void OZ_free (sasfit_oz_data *OZd) {
    free(r);
    free(k);
    free(EN);
    free(G);
    free(G0);
    free(g);
    free(HR);
    free(g0);
    free(dU_dR);
    free(UBETA);
    free(c);
    free(cf);
    free(CFNEW);
    free(CFOLD);
    free(GF);
    free(Fswitch);
    free(S);
    free(BRIDGE);
    free(CAVITY);
    free(MAYER);
    gsl_vector_free(GAMMA_R);

    fftw_destroy_plan(OZd->pl);
    fftw_free(OZIN);
    fftw_free(OZOUT);
}











