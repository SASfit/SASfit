#include <stdio.h>
#include <math.h>
#include <time.h>
#include "ry_polydisperse_core.h"
static double now(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+1e-9*t.tv_nsec;}
int main(void){
    double mixes[6]={0.2,0.3,0.5,0.7,0.9,1.0}; int mi,k;
    for(mi=0;mi<6;mi++){
        ryp_system sys; double s=0.2,nstar=0.005,Z=200.0,LB=7.01/250.0;
        double t=1.0/(s*s)-1.0,m3=1.0,phi,t0; int m; ryp_status st;
        for(m=1;m<=3;m++) m3*=(t+m)/(t+1.0);
        phi=(M_PI/6.0)*nstar*m3;
        ryp_alloc(&sys,3,4095,100.0); ryp_set_schulz(&sys,s,1.0);
        ryp_set_charges(&sys,Z,2.0,LB,NULL,NULL); ryp_set_volume_fraction(&sys,phi);
        ryp_build_potential(&sys);
        t0=now(); st=ryp_solve(&sys,0.5,20000,1e-8,mixes[mi]);
        if(st!=RYP_OK) printf("mix=%.1f  FAILED: %s\n",mixes[mi],ryp_strerror(st));
        else {double mx=0; for(k=0;k<sys.N;k++) if(sys.SM[k]>mx)mx=sys.SM[k];
              printf("mix=%.1f  %5d iters  %.2f s  S^M_max=%.6f\n",mixes[mi],sys.lastIterations,now()-t0,mx);}
        ryp_free(&sys);
    }
    return 0;
}
