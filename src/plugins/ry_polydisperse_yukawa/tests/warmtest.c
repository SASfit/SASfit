#include <stdio.h>
#include <math.h>
#include <time.h>
#include "ry_polydisperse_core.h"
static double now(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+1e-9*t.tv_nsec;}
int main(void){
    ryp_system sys; double s=0.2,nstar=0.005,Z=200.0,LB=7.01/250.0;
    double t=1.0/(s*s)-1.0,m3=1.0,phi,t0; int m,i;
    for(m=1;m<=3;m++) m3*=(t+m)/(t+1.0);
    phi=(M_PI/6.0)*nstar*m3;
    ryp_alloc(&sys,3,4095,100.0); ryp_set_schulz(&sys,s,1.0);
    ryp_set_charges(&sys,Z,2.0,LB,NULL,NULL); ryp_set_volume_fraction(&sys,phi);
    ryp_build_potential(&sys);
    t0=now(); ryp_solve_robust(&sys,0.5,20000,1e-8);
    printf("cold solve            : %.2f s  %4d iters  S^M_max=%.5f\n",now()-t0,sys.lastIterations,
           ({double mx=0;int k;for(k=0;k<sys.N;k++) if(sys.SM[k]>mx)mx=sys.SM[k];mx;}));
    /* typical fitting step: change phi by 2%, re-solve warm */
    for(i=0;i<3;i++){
        ryp_set_volume_fraction(&sys,phi*(1.0+0.02*(i+1)));
        ryp_build_potential(&sys);
        t0=now(); ryp_solve_robust(&sys,0.5,20000,1e-8);
        printf("warm re-solve (+%d%% phi): %.2f s  %4d iters  S^M_max=%.5f\n",2*(i+1),now()-t0,sys.lastIterations,
               ({double mx=0;int k;for(k=0;k<sys.N;k++) if(sys.SM[k]>mx)mx=sys.SM[k];mx;}));
    }
    ryp_free(&sys); return 0;
}
