#include "sasfit_oz_solver.h"
#include <stdio.h>
#include <stdlib.h>
#include <fftw3.h>

int OZ_init( struct OZdata *OZd)
{  int i;
   double **temp, *tp;
   temp =(double**)malloc((OZd->Npoints+1)*sizeof(double*));
   for (i=0;i<(OZd->Npoints+1);i++) {
    temp[i] = (double*)malloc(2*sizeof(double));}
   OZd->Sq= temp;
    temp =(double**)malloc((OZd->Npoints+1)*sizeof(double*));
       for (i=0;i<(OZd->Npoints+1);i++) {
    temp[i] = (double*)malloc(2*sizeof(double));}
   OZd->gr=temp;
    temp =(double**)malloc((OZd->Npoints+1)*sizeof(double*));
       for (i=0;i<(OZd->Npoints+1);i++) {
    temp[i] = (double*)malloc(2*sizeof(double));
   }
   OZd->cr=temp;
   tp=(double*)malloc((OZd->Npoints+1)*sizeof(double));
   OZd->r=tp;
   tp=(double*)malloc((OZd->Npoints+1)*sizeof(double));
   OZd->k=tp;
   tp=(double*)malloc((OZd->Npoints+1)*sizeof(double));
   OZd->En=tp;
   tp=(double*)malloc((OZd->Npoints+1)*sizeof(double));
   OZd->G=tp;
   tp=(double*)malloc((OZd->Npoints+1)*sizeof(double));
   OZd->g=tp;
   tp=(double*)malloc((OZd->Npoints+1)*sizeof(double));
   OZd->c=tp;
   tp=(double*)malloc((OZd->Npoints+1)*sizeof(double));
   OZd->cf=tp;
   tp=(double*)malloc((OZd->Npoints+1)*sizeof(double));
   OZd->cfold=tp;
   tp=(double*)malloc((OZd->Npoints+1)*sizeof(double));
   OZd->cfnew=tp;
   tp=(double*)malloc((OZd->Npoints+1)*sizeof(double));
   OZd->Gf=tp;
   tp=(double*)malloc((OZd->Npoints+1)*sizeof(double));
   OZd->f=tp;
   tp=(double*)malloc((OZd->Npoints+1)*sizeof(double));
   OZd->S=tp;
   tp=(double*)malloc((OZd->Npoints+1)*sizeof(double));
   OZd->ud=tp;
  OZd->in=(double*)fftw_malloc(sizeof(double)*OZd->Npoints);
  OZd->out=(double*)fftw_malloc(sizeof(double)*OZd->Npoints);

    return 1;
}

double extrapolate (double x1,double x2, double x3, double y1, double y2,double y3)
{  double Ex;
   Ex=-(-pow(x2,2)*x3*y1+x2*pow(x3,2)*y1+pow(x1,2)*x3*y2-x1*pow(x3,2)*y2-pow(x1,2)*x2*y3+x1*pow(x2,2)*y3)/((x1-x2)*(x1-x3)*(x2-x3));
   return Ex;
}


int OZ_solver (struct OZdata *OZd)
{
    double  e, ro, dk, Sm, Norm, Normold,V;
    int i,j;
    Normold=1;
    ro=6*OZd->phi/(Pi*pow(OZd->pPot[0],3));
    dk = Pi/((OZd->Npoints+1)*OZd->dr);
    OZd->r[0]=0;OZd->k[0]=0;OZd->G[0]=0;i=0;
    //OZd->f[0]=pow(10,-10);

for (i=1; i < OZd->Npoints+1; i++)
    {
    OZd->r[i]=(OZd->r[i-1]+OZd->dr);
    OZd->k[i]=(OZd->k[i-1]+dk);
    OZd->G[i]=0;
    OZd->f[i-1]=1-exp(-OZd->alpha*OZd->r[i]);
    V = (*OZd->potential)(OZd->r[i],OZd->pPot);
    if (V == HUGE_VAL) {
        OZd->En[i]=0.0;
    } else {
        OZd->En[i]=exp(-(*OZd->potential)(OZd->r[i],OZd->pPot)/(kb*T));
    }
   }
 OZd->r[OZd->Npoints]=OZd->r[OZd->Npoints-1]+OZd->dr;
 OZd->k[OZd->Npoints]=OZd->k[OZd->Npoints-1]+dk;

FILE* f3 = fopen("err.txt", "w");
//for (n=1; n < OZd->maxsteps; n++ )
OZd->it=0; e=2*OZd->relerror;
while (OZd->it < OZd->maxsteps && e > OZd->relerror )
{      OZd->it++;
       for (i=0; i < OZd->Npoints; i++){

            if (OZd->cl==PY) {OZd->c[i]=(1.+OZd->G[i])*(OZd->En[i]-1.);
                             OZd->g[i]=OZd->En[i]*(1+OZd->G[i]);}

            if (OZd->cl==HNC) {OZd->c[i]=-1-OZd->G[i]+OZd->En[i]*exp(OZd->G[i]);
                             OZd->g[i]=OZd->En[i]*exp(OZd->G[i]);}

            if (OZd->cl==RY)   {
                    if (OZd->alpha == 0) {OZd->c[i]=(1.+OZd->G[i])*(OZd->En[i]-1.);
                             OZd->g[i]=OZd->En[i]*(1+OZd->G[i]);} else {
                             OZd->c[i]=OZd->En[i+1]*(1+((exp(OZd->f[i]*OZd->G[i])-1)/OZd->f[i]))-OZd->G[i]-1;
                             OZd->g[i]= OZd->c[i]+OZd->G[i]+1;}}

       OZd->in[i]=(i+1)*OZd->c[i];
        }

     OZd->pl=fftw_plan_r2r_1d(OZd->Npoints, OZd->in, OZd->out, FFTW_RODFT00, FFTW_ESTIMATE);
     fftw_execute(OZd->pl);

     for (j=0; j < OZd->Npoints; j++)
      {   OZd->cfnew[j]=4.*Pi*OZd->dr*OZd->dr*OZd->out[j]/(2*dk*(j+1));
          OZd->cf[j]=OZd->mixcoeff*OZd->cfnew[j]+(1-OZd->mixcoeff)*OZd->cfold[j];
          //OZd->cf[j]=4.*Pi*OZd->dr*OZd->dr*OZd->out[j]/(2*dk*(j+1));
          OZd->cfold[j]=OZd->cf[j];
          OZd->Gf[j]=ro*OZd->cf[j]*OZd->cf[j]/(1-ro*OZd->cf[j]);
          OZd->in[j]=OZd->Gf[j]*(j+1);
      }
    OZd->pl=fftw_plan_r2r_1d(OZd->Npoints, OZd->in, OZd->out, FFTW_RODFT00, FFTW_ESTIMATE);
      fftw_execute(OZd->pl);
    Sm=0;
   // OZd->it
    for (j=0; j < OZd->Npoints; j++)
    {OZd->G[j]=4.*Pi*dk*dk*OZd->out[j]/(2*OZd->dr*pow(2*Pi,3)*(j+1));
    OZd->S[j]=1./(1.-ro*OZd->cf[j]);
    Sm=pow(1./(1.-ro*OZd->cf[j]),2)+Sm;
    }
   Norm= sqrt(Sm);
   e = fabs((Norm-Normold)/Normold);
   fprintf(f3,"{%.d, %.8f},", OZd->it, e);
   Normold=Norm;
 }

//FILE* f2 = fopen("28.08.txt", "w");
OZd->Sq[0][0]=OZd->k[0];
OZd->Sq[0][1]=extrapolate(OZd->k[1],OZd->k[2],OZd->k[3],OZd->S[0],OZd->S[1],OZd->S[2]);
OZd->gr[0][0]=OZd->r[0];
OZd->gr[0][1]=extrapolate(OZd->r[1],OZd->r[2],OZd->r[3],OZd->g[0],OZd->g[1],OZd->g[2]);
OZd->cr[0][0]=OZd->r[0];
OZd->cr[0][1]=extrapolate(OZd->r[1],OZd->r[2],OZd->r[3],OZd->c[0],OZd->c[1],OZd->c[2]);

for (i=1; i< OZd->Npoints+1; i++)
  { OZd->Sq[i][0]=OZd->k[i];
    OZd->Sq[i][1]=OZd->S[i-1];
    OZd->gr[i][0]=OZd->r[i];
    OZd->gr[i][1]=OZd->g[i-1];
    OZd->cr[i][0]=OZd->r[i];
    OZd->cr[i][1]=OZd->c[i-1];
   }
    OZd->dq=dk;
//for (i=0; i<OZd->Npoints+1; i++)
  //  fprintf(f2,"{%.6f, %.6f},", OZd->gr[i][0],OZd->Sq[i][1]);


return 1;
}




void OZ_free (struct OZdata *OZd)
{        int i;
         for (i = 0; i < (OZd->Npoints+1); i++)
         free(OZd->Sq[i]);
         free(OZd->Sq);
         for (i = 0; i < (OZd->Npoints+1); i++)
         free(OZd->gr[i]);
         free(OZd->gr);
         for (i = 0; i < (OZd->Npoints+1); i++)
         free(OZd->cr[i]);
         free(OZd->cr);
         free(OZd->r);free(OZd->k); free(OZd->En); free(OZd->G); free(OZd->g); free(OZd->ud); free(OZd->c);  free(OZd->cf); free(OZd->cfnew);free(OZd->cfold);free(OZd->Gf); free(OZd->f);free(OZd->S);

        fftw_destroy_plan(OZd->pl);
        fftw_free(OZd->in); fftw_free(OZd->out);

}

void OZ_pot_der(struct OZdata *OZd)
{  int n;
    FILE* f4 = fopen("pot_der.txt", "w");
    OZd->ud[0]=((*OZd->potential)(OZd->r[2],OZd->pPot)-(*OZd->potential)(OZd->r[1],OZd->pPot))/(OZd->dr);
    for (n=1; n < (OZd->Npoints-1); n++)
    {OZd->ud[n]=((*OZd->potential)(OZd->r[n+2],OZd->pPot)-(*OZd->potential)(OZd->r[n],OZd->pPot))/(2*OZd->dr);
    }
     OZd->ud[OZd->Npoints-1]=((*OZd->potential)(OZd->r[OZd->Npoints-1],OZd->pPot)-(*OZd->potential)(OZd->r[OZd->Npoints-2],OZd->pPot))/(OZd->dr);
     for (n=0; n<OZd->Npoints; n++)
    fprintf(f4,"{%.20f },", OZd->ud[n]);
}

double RY_sc(double alpha, void *params)
{    struct  OZdata *OZd = (struct  OZdata *) params;
     int i;
     double iphi;
     double S0, chicp, chivir,r1,r2,r3,P2,P3, sum2,sum3;
     sum2=0;
     sum3=0;
   OZd->alpha=alpha;
   iphi = OZd->phi;
   OZ_solver(OZd);
   S0=OZd->Sq[0][1];
  // printf("S0:%g   %g\n", S0,  OZd->Sq[1][1]);
   r1=6*OZd->phi/(Pi*pow(OZd->pPot[0],3));
   chicp=S0*OZd->beta/r1;

   OZ_pot_der(OZd);

   OZd->phi=iphi-iphi*0.01;
   r2=6*OZd->phi/(Pi*pow(OZd->pPot[0],3));
   OZ_solver(OZd);
   for (i=0; i<OZd->Npoints; i++)
   { sum2=sum2+(OZd->ud[i])*OZd->g[i]*pow(OZd->r[i+1],3);
       //sum2=sum2+(OZd->ud[i])*(OZd->r[i+1])*pow(OZd->g[i],3);
   }
   P2=(r2/OZd->beta)*(1-(2*Pi*OZd->beta*r2/3)*OZd->dr*sum2);

   OZd->phi=iphi+iphi*0.01;
   r3=6*OZd->phi/(Pi*pow(OZd->pPot[0],3));
   OZ_solver(OZd);
   for (i=0; i<OZd->Npoints; i++)
   { sum3=sum3+(OZd->ud[i])*OZd->g[i]*pow(OZd->r[i+1],3);
        //sum3=sum3+(OZd->ud[i])*(OZd->r[i+1])*pow(OZd->g[i],3);
   }
   P3=(r3/OZd->beta)*(1-(2*Pi*OZd->beta*r3/3)*OZd->dr*sum3);
   chivir=(r3-r2)/((P3-P2)*r1);
   OZd->phi=iphi;
   //printf("%g   %g\n", chicp,  chivir);
   return chicp-chivir;
}

