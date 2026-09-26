#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "ry_polydisperse_core.h"
static double now(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+1e-9*t.tv_nsec;}
static void setup(ryp_system*s,int p,double sr,double sc){
    double nstar=0.005*sc,t,m3=1.0,phi; int m;
    t=(sr>0)?1.0/(sr*sr)-1.0:0; if(sr>0) for(m=1;m<=3;m++) m3*=(t+m)/(t+1.0);
    phi=(M_PI/6.0)*nstar*m3;
    ryp_alloc(s,p,4095,100.0); ryp_set_schulz(s,sr,1.0);
    ryp_set_charges(s,200.0,2.0,7.01/250.0,NULL,NULL);
    ryp_set_volume_fraction(s,phi); ryp_build_potential(s);
}
static void one(int p,double sr,double sc,const char*tag){
    ryp_system s; double t0,dt,mx; int k; ryp_status st;
    printf("%-24s",tag);
    setup(&s,p,sr,sc); t0=now(); st=ryp_solve_newton_krylov(&s,0.5,2000,RYP_FUNC_NORM_TOL,RYP_SCALED_STEP_TOL); dt=now()-t0;
    if(st!=RYP_OK) printf(" NK: %-22s","FAILED");
    else{mx=0;for(k=0;k<s.N;k++) if(s.SM[k]>mx)mx=s.SM[k];
         printf(" NK:%4d it %5.2fs S=%.5f",s.lastIterations,dt,mx);}
    ryp_free(&s);
    setup(&s,p,sr,sc); t0=now(); st=ryp_solve_anderson(&s,0.5,20000,1e-8,1.0); dt=now()-t0;
    if(st!=RYP_OK) printf("  | AA: FAILED");
    else{mx=0;for(k=0;k<s.N;k++) if(s.SM[k]>mx)mx=s.SM[k];
         printf("  | AA:%4d it %5.2fs S=%.5f",s.lastIterations,dt,mx);}
    ryp_free(&s); printf("\n");
}
int main(void){
    printf("Newton-Krylov (GMRES+linesearch) vs Anderson(KIN_FP+Picard precond)\n");
    one(3,0.2,1.0,"p=3 s=0.2");
    one(1,0.0,1.0,"p=1 mono");
    one(5,0.3,1.0,"p=5 s=0.3 (AA failed)");
    one(3,0.4,1.0,"p=3 s=0.4");
    one(7,0.25,1.0,"p=7 s=0.25");
    one(3,0.2,5.0,"p=3 s=0.2 5x dens");
    return 0;
}
