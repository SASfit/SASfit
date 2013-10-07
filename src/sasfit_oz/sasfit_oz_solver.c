/*
 * Author(s) of this file:
 *   Evgeniy Ponomarev (evgeniy.ponomarev@epfl.ch)
 *   Modified 13.09.2013
 *   modified by Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   21.9.2013
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <sasfit_oz.h>

#define NP  OZd->Npoints
#define EN  OZd->En
#define r   OZd->r
#define DR  OZd->dr
#define k   OZd->k
#define GF  OZd->Gf
#define G  OZd->G
#define g  OZd->g
#define IN  OZd->in
#define OUT OZd->out
#define U   (*OZd->potential)
#define dU_dR OZd->ud
#define UBETA OZd->ubeta
#define PARAM OZd->pPot
#define dr  OZd->dr
#define c  OZd->c
#define CFOLD OZd->cfold
#define CFNEW OZd->cfnew
#define cf OZd->cf
#define ALPHA OZd->alpha
#define sBPGG OZd->sBPGG
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
   OZd->beta=1/(kb*T);
   tp=(double*)malloc((NP)*sizeof(double));
   r=tp;
   tp=(double*)malloc((NP)*sizeof(double));
   k=tp;
   tp=(double*)malloc((NP)*sizeof(double));
   EN=tp;
   tp=(double*)malloc((NP)*sizeof(double));
   G=tp;
   tp=(double*)malloc((NP)*sizeof(double));
   g=tp;
   tp=(double*)malloc((NP)*sizeof(double));
   c=tp;
   tp=(double*)malloc((NP)*sizeof(double));
   cf=tp;
   tp=(double*)malloc((NP)*sizeof(double));
   CFOLD=tp;
   tp=(double*)malloc((NP)*sizeof(double));
   CFNEW=tp;
   tp=(double*)malloc((NP)*sizeof(double));
   GF=tp;
   tp=(double*)malloc((NP)*sizeof(double));
   Fswitch=tp;
   tp=(double*)malloc((NP)*sizeof(double));
   S=tp;
   tp=(double*)malloc((NP)*sizeof(double));
   UBETA=tp;
   tp=(double*)malloc((NP)*sizeof(double));
   dU_dR=tp;
   IN=(double*)fftw_malloc(sizeof(double)*NP);
   OUT=(double*)fftw_malloc(sizeof(double)*NP);
   for (i=0; i < NP; i++) {
        G[i]=0;
   }
 }

//Extrapolation routine

double extrapolate (double x1,double x2, double x3, double y1, double y2,double y3)
{  double Ex;
   Ex=-(-pow(x2,2)*x3*y1+x2*pow(x3,2)*y1+pow(x1,2)*x3*y2-x1*pow(x3,2)*y2-pow(x1,2)*x2*y3+x1*pow(x2,2)*y3)/((x1-x2)*(x1-x3)*(x2-x3));
   return Ex;
}

//Calling OZ_solver and generating output
void OZ_calculation (sasfit_oz_data *OZd) {
  if (CLOSURE==RY || CLOSURE==BPGG) {
        printf("Root finding\n");
        root_finding(OZd);
  } else {
        printf("Root finding not required\n");
  }
  OZ_solver(OZd);
  OZ_pot_der(OZd);
}

// Iterative solution of Ornstein-Zernike equation

void OZ_solver (sasfit_oz_data *OZd)
{
    double  e, ro, dk, Sm, Norm, Normold,V;
    int i,j;
    Normold=1;
    ro=6*PHI/(M_PI*pow(PARAM[0],3));
    dk = M_PI/((NP+1.0)*dr);
    r[0]=dr;
    k[0]=dk;

    for (i=1; i < NP; i++) {
        r[i]=(r[i-1]+dr);
        k[i]=(k[i-1]+dk);
    }

    for (i=0; i < NP; i++) {
 //       G[i]=0;
        Fswitch[i]=1-exp(-ALPHA*r[i]);
        V = U(r[i],T,PARAM);
        UBETA[i]=V*OZd->beta;
        if (V == GSL_POSINF) {
            EN[i]=0.0;
        } else {
            EN[i]=exp(-V/(kb*T));
        }
    }

    OZd->it=0;
    e=2*RELERROR;

while (OZd->it < MAXSTEPS && e > RELERROR ) {
    OZd->it++;
    for (i=0; i < NP; i++){
        if (CLOSURE==PY) {
            c[i]=(1.+G[i])*(EN[i]-1.);
            g[i]=EN[i]*(1+G[i]);
        }
        if (CLOSURE==HNC) {
            c[i]=-1-G[i]+EN[i]*exp(G[i]);
            g[i]=EN[i]*exp(G[i]);
        }
        if (CLOSURE==RY)   {
            if (ALPHA == 0) {
                c[i]=(1.+G[i])*(EN[i]-1.);
                g[i]=EN[i]*(1+G[i]);
            } else {
                c[i]=EN[i]*(1+((exp(Fswitch[i]*G[i])-1)/Fswitch[i]))-G[i]-1;
                g[i]= c[i]+G[i]+1;
            }
        }
        if (CLOSURE==Verlet) {
            c[i]=-1.-G[i]+EN[i]*exp(G[i]+(gsl_pow_2(G[i])/(2.*(1.+0.8*G[i]))));
            g[i]=EN[i]*exp(G[i]+(gsl_pow_2(G[i])/(2.*(1.+0.8*G[i]))));
        }
        if (CLOSURE==MS) {
            c[i]=-1.-G[i]+EN[i]*exp(sqrt(1.+2.*G[i])-1.);
            g[i]=EN[i]*exp(sqrt(1.+2.*G[i])-1.);
        }
        if (CLOSURE==BPGG) {
            c[i]=-1.-G[i]+EN[i]*exp(pow(1+sBPGG*G[i],1./sBPGG)-1.);
            g[i]=EN[i]*exp(pow(1.+sBPGG*G[i],1./sBPGG)-1.);
        }
        IN[i]=(i+1)*c[i];
    }
    OZd->pl=fftw_plan_r2r_1d(NP, IN, OUT, FFTW_RODFT00, FFTW_ESTIMATE);
    fftw_execute(OZd->pl);

    for (j=0; j < NP; j++) {
        CFNEW[j]=4.*Pi*dr*dr*OUT[j]/(2*dk*(j+1));
        cf[j]=MIXCOEFF*CFNEW[j]+(1-MIXCOEFF)*CFOLD[j];
        CFOLD[j]=cf[j];
        GF[j]=ro*cf[j]*cf[j]/(1-ro*cf[j]);
        IN[j]=GF[j]*(j+1);
    }
    OZd->pl=fftw_plan_r2r_1d(NP, IN, OUT, FFTW_RODFT00, FFTW_ESTIMATE);
    fftw_execute(OZd->pl);
    Sm=0;
    for (j=0; j < NP; j++) {
        G[j]=4.*Pi*dk*dk*OUT[j]/(2*dr*pow(2*Pi,3)*(j+1));
        S[j]=1./(1.-ro*cf[j]);
        Sm=pow(1./(1.-ro*cf[j]),2)+Sm;
        }
    Norm= sqrt(Sm);
    e = fabs((Norm-Normold)/Normold);
    Normold=Norm;
}


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
     double S0, chicp, chivir,r1,r2,r3,P2,P3, sum2,sum3;
     sum2=0;
     sum3=0;

   OZd = (sasfit_oz_data *) params;
   if (CLOSURE==RY) ALPHA=scp;
   if (CLOSURE==BPGG)  sBPGG=scp;

   iphi = PHI;
   OZ_solver(OZd);
   S0=OZd->Sq0;
   r1=6*PHI/(Pi*pow(PARAM[0],3));
   chicp=S0*OZd->beta/r1;

   OZ_pot_der(OZd);

   PHI=iphi-iphi*0.01;
   r2=6*PHI/(Pi*pow(PARAM[0],3));
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
            sum2=sum2+(dU_dR[i])*g[i]*pow(r[i+1],3);
       }
   }
   P2=(r2/OZd->beta)*(1-(2*Pi*OZd->beta*r2/3)*dr*sum2);

   PHI=iphi+iphi*0.01;
   r3=6*PHI/(Pi*pow(PARAM[0],3));
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
            sum3=sum3+(dU_dR[i])*g[i]*pow(r[i+1],3);
       }
   }
   P3=(r3/OZd->beta)*(1-(2*Pi*OZd->beta*r3/3)*dr*sum3);
   chivir=(r3-r2)/((P3-P2)*r1);
   PHI=iphi;
   return chicp-chivir;
}

//Routine to minimize the difference between two compressibilities

   void root_finding (sasfit_oz_data *OZd)
   {   int signchange,i;
       double refnew, refold, alpha_left, alpha_right, scp_inter;
       refold=0;
       signchange = 0;
       printf("\nSearching for the interval where sign changes: \n");
       printf ("%s   |  %s \n","self-consistency parameter" , "(chicomp-chivir)/beta");
       printf ("-----------------------------|---------------------------\n");

       i = 0;
       while (signchange==0 && i<=18) {
        if (CLOSURE==RY){
            refnew=compressibility_calc(100/pow(10,i), OZd);
            printf ("            %15g  |   %15g \n", 100/pow(10,i) , refnew/OZd->beta);
        }
        if (CLOSURE==BPGG) {
            refnew=compressibility_calc(2.3-0.1*i, OZd);
            printf ("              %10f  |   %.10g \n", 2.3-0.1*i, refnew/OZd->beta);
        }

        if ((refnew<0 && refold>0) || (refnew>0 && refold<0))
         {if (CLOSURE==RY){alpha_left=100/pow(10,i);
                           alpha_right=100/pow(10,i-1);
                           }

         if (CLOSURE==BPGG) {alpha_left=2.3-0.1*i;
                             alpha_right=2.3-0.1*(i-1);
                            }

          signchange=1;
         } else {
             if (abs(refnew) < abs(refold)) {if (CLOSURE==RY) scp_inter=100/pow(10,i);
                                                if (CLOSURE==BPGG) scp_inter=2.3-0.1*i;}

                else                           {if (CLOSURE==RY)scp_inter=100/pow(10,i-1);
                                                if (CLOSURE==BPGG) scp_inter=2.3-0.1*(i-1);}

                refold=refnew;

               if (i==18) {printf ("No change of sign in %d steps \n", i);
                           if (CLOSURE==RY) ALPHA=scp_inter;
                           if (CLOSURE==BPGG)sBPGG=scp_inter;
                          }
            }
        i++;
       }
 if (signchange==1)
{ int status;
  int iter = 0;
  int max_iter = 100;
  const gsl_root_fsolver_type *Tt;
  gsl_root_fsolver *s;
  double root = 0.0;
  double x_lo = alpha_left;
  double  x_hi = alpha_right;
  gsl_function F;
  F.function = compressibility_calc;
  F.params = OZd;
  Tt = gsl_root_fsolver_brent;
  s = gsl_root_fsolver_alloc (Tt);
  gsl_root_fsolver_set (s, &F, x_lo, x_hi);
  printf ("using %s method\n",
          gsl_root_fsolver_name (s));

  printf ("%5s [%9s, %9s] %9s %9s\n",
          "iter", "lower", "upper", "root",
           "err(est)");
  do {
        iter++;
        status = gsl_root_fsolver_iterate (s);
        root = gsl_root_fsolver_root (s);
        x_lo = gsl_root_fsolver_x_lower (s);
        x_hi = gsl_root_fsolver_x_upper (s);
        status = gsl_root_test_interval (x_lo, x_hi, 0, 1e-5);
        if (status == GSL_SUCCESS) {
            printf ("Converged:\n");
        }
        printf ("%5d [%.7f, %.7f] %.7f %.7f\n",
              iter, x_lo, x_hi,  root,  x_hi - x_lo);
  } while (status == GSL_CONTINUE && iter < max_iter);
   if (CLOSURE==RY) {
        ALPHA=root;
        printf("Alpha parameter after optimization: %g \n", ALPHA);
   }
   if (CLOSURE==BPGG) {
        sBPGG=root;
        printf("S parameter after optimization: %g \n", sBPGG);
   }
   gsl_root_fsolver_free (s);
 }
   }

//Release of memory used for computation
void OZ_free (sasfit_oz_data *OZd) {
    free(r);
    free(k);
    free(EN);
    free(G);
    free(g);
    free(dU_dR);
    free(UBETA);
    free(c);
    free(cf);
    free(CFNEW);
    free(CFOLD);
    free(GF);
    free(Fswitch);
    free(S);

    fftw_destroy_plan(OZd->pl);
    fftw_free(IN);
    fftw_free(OUT);
}










