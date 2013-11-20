/*
 * Author(s) of this file:
 *   Evgeniy Ponomarev (evgeniy.ponomarev@epfl.ch)
 *   Modified 13.09.2013
 *   modified by Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   19.11.2013
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <sasfit_oz.h>
#include <bool.h>

#define NP  OZd->Npoints
#define EN  OZd->En
#define r   OZd->r
#define DR  OZd->dr
#define k   OZd->k
#define GF  OZd->Gf
#define G  OZd->G
#define G0  OZd->G0
#define g  OZd->g
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

//Calling OZ_solver and generating output
void OZ_calculation (sasfit_oz_data *OZd) {
int igneg;
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
// check if where g(sigma+) >= 0 than nothing has to be done and RMSA=MSA which has been solved already above
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

// Iterative solution of Ornstein-Zernike equation
void OZ_solver (sasfit_oz_data *OZd) {
    double  err, ro, dk, Sm, Norm, Normold,V, Gstar;
    int i,j;
    double powarg, logarg;
    bool doneB;
    OZ_func_one_t * tmp_potential;
    Normold=1;
    ro=6*PHI/(M_PI*gsl_pow_3(PARAM[0]));
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
err=2*RELERROR;

while (OZd->it < MAXSTEPS && err > RELERROR ) {
    doneB = FALSE;
    OZd->it++;
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
        CFNEW[j]=4.0*M_PI*dr*dr*OZOUT[j]/(2.0*dk*(j+1.0));
        cf[j]=MIXCOEFF*CFNEW[j]+(1-MIXCOEFF)*CFOLD[j];
        CFOLD[j]=cf[j];
        GF[j]=ro*cf[j]*cf[j]/(1.0-ro*cf[j]);
        OZIN[j]=GF[j]*(j+1.0);
    }
//    OZd->pl=fftw_plan_r2r_1d(NP, OZIN, OZOUT, FFTW_RODFT00, FFTW_ESTIMATE);
    fftw_execute_r2r(OZd->pl,OZIN,OZOUT);
    Sm=0;
    for (j=0; j < NP; j++) {
        G[j]=4.*M_PI*dk*dk*OZOUT[j]/(2.0*dr*gsl_pow_3(2.0*M_PI)*(j+1.0));
        S[j]=1./(1.-ro*cf[j]);
        Sm=gsl_pow_2(1./(1.-ro*cf[j]))+Sm;
        if (!doneB) {
            if (g[j] > 0 && EN[j] != 0) {
                BRIDGE[j]=log(g[j])+UBETA[j]-G[j];
            } else {
                BRIDGE[j] = 0.0;
            }
        }
        CAVITY[j] = exp(BRIDGE[j]+G[j]);
    }
    Norm= sqrt(Sm);
    err = fabs((Norm-Normold)/Norm);
    Normold=Norm;
    if ((OZd->it*4096 % (100*OZd->Npoints) )==0) {
        Tcl_EvalEx(OZd->interp,"set OZ(progressbar) 1",-1,TCL_EVAL_DIRECT);
        Tcl_EvalEx(OZd->interp,"update",-1,TCL_EVAL_DIRECT);
    }
    if (Norm != Norm) {
        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
        for (j=0; j < NP; j++) {
            G[j]=0.0;
        }
    }
}
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
    sasfit_out("      sigma+     |      sigma'      |      g(sigma+)      \n");
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

    fftw_destroy_plan(OZd->pl);
    fftw_free(OZIN);
    fftw_free(OZOUT);
}











