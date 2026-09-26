#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "ry_polydisperse_core.h"
static double now(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+1e-9*t.tv_nsec;}
static double run(int p,double s,double phiScale,const char*tag,double *SMout,int *N){
    ryp_system sys; double nstar=0.005*phiScale,Z=200.0,LB=7.01/250.0,t,m3=1.0,phi,t0,dt; int m,k;
    t=(s>0)?1.0/(s*s)-1.0:0; if(s>0) for(m=1;m<=3;m++) m3*=(t+m)/(t+1.0);
    phi=(M_PI/6.0)*nstar*m3;
    ryp_alloc(&sys,p,4095,100.0); ryp_set_schulz(&sys,s,1.0);
    ryp_set_charges(&sys,Z,2.0,LB,NULL,NULL); ryp_set_volume_fraction(&sys,phi);
    ryp_build_potential(&sys);
    t0=now(); ryp_status st=ryp_solve_robust(&sys,0.5,20000,1e-8); dt=now()-t0;
    if(st!=RYP_OK){printf("%-26s FAILED: %s\n",tag,ryp_strerror(st)); ryp_free(&sys); return -1;}
    {double mx=0; for(k=0;k<sys.N;k++) if(sys.SM[k]>mx)mx=sys.SM[k];
     printf("%-26s %5d iters  %5.2f s  S^M_max=%.6f  %s\n",tag,sys.lastIterations,dt,mx,
            sys.lastMethod);
     if(SMout){for(k=0;k<sys.N;k++) SMout[k]=sys.SM[k]; *N=sys.N;}
     ryp_free(&sys); return mx;}
}
int main(void){
    static double ref[4095]; int N;
    printf("robust solver across regimes:\n");
    run(3,0.2,1.0,"p=3 s=0.2 (reference)",ref,&N);
    run(1,0.0,1.0,"p=1 monodisperse",NULL,NULL);
    run(5,0.3,1.0,"p=5 s=0.3",NULL,NULL);
    run(3,0.4,1.0,"p=3 s=0.4",NULL,NULL);
    run(3,0.2,5.0,"p=3 s=0.2 5x density",NULL,NULL);
    run(3,0.2,0.2,"p=3 s=0.2 0.2x density",NULL,NULL);
    run(7,0.25,1.0,"p=7 s=0.25",NULL,NULL);
    {FILE*f=fopen("c_SM_aa.txt","w"); int k;
     for(k=0;k<N;k++) fprintf(f,"%.12g\n",ref[k]); fclose(f);}
    return 0;
}
