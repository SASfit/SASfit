#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "ry_polydisperse_core.h"
static double now(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+1e-9*t.tv_nsec;}
int main(int argc,char**argv){
    ryp_system s; double beta=atof(argv[1]); double t0,dt; int k;
    double sr=0.2,nstar=0.005,t=1.0/(0.2*0.2)-1.0,m3=1.0,phi; int m;
    for(m=1;m<=3;m++) m3*=(t+m)/(t+1.0);
    phi=(M_PI/6.0)*nstar*m3;
    ryp_alloc(&s,3,4095,100.0); ryp_set_schulz(&s,sr,1.0);
    ryp_set_charges(&s,200.0,2.0,7.01/250.0,NULL,NULL);
    ryp_set_volume_fraction(&s,phi); ryp_build_potential(&s);
    t0=now(); ryp_status st=ryp_solve_anderson(&s,0.5,3000,1e-8,beta); dt=now()-t0;
    if(st!=RYP_OK) printf("beta=%.2f  FAILED (%s) after %.1f s\n",beta,ryp_strerror(st),dt);
    else{double mx=0; for(k=0;k<s.N;k++) if(s.SM[k]>mx)mx=s.SM[k];
         printf("beta=%.2f  %4d iters  %.2f s  S^M_max=%.6f\n",beta,s.lastIterations,dt,mx);}
    ryp_free(&s); return 0;
}
