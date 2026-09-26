#include <stdio.h>
#include <math.h>
#include <time.h>
#include "sasfit_common.h"
#include "include/sasfit_ry_polydisperse_yukawa.h"
static double now(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+1e-9*t.tv_nsec;}
int main(void){
    sasfit_param P; int k,m; double t0,dt,v,qmax=0,smax=0;
    double s=0.2,nstar=0.005,t=1.0/(0.2*0.2)-1.0,m3=1.0,phi;
    for(m=1;m<=3;m++) m3*=(t+m)/(t+1.0);
    phi=(M_PI/6.0)*nstar*m3;
    P.p[0]=1.0; P.p[1]=s; P.p[2]=phi; P.p[3]=200.0; P.p[4]=7.01/250.0;
    P.p[5]=0.5; P.p[6]=3; P.p[7]=2.0;

    t0=now(); v=sasfit_sq_RYPolydisperseYukawa(1.0,&P); dt=now()-t0;
    printf("first call (triggers solve): S(1.0)=%.6f   [%.2f s]\n",v,dt);
    t0=now();
    for(k=0;k<2000;k++) sasfit_sq_RYPolydisperseYukawa(0.01+k*0.005,&P);
    dt=now()-t0;
    printf("2000 further q-points (cached): %.4f s  (%.1f us/point)\n",dt,1e6*dt/2000);

    for(k=1;k<4000;k++){double q=k*0.002, S=sasfit_sq_RYPolydisperseYukawa(q,&P);
        if(S>smax){smax=S;qmax=q;}}
    printf("peak: S^M_max=%.5f at q*sigma=%.3f   (reference S^M_max=1.74742)\n",smax,qmax);

    /* changing a parameter must trigger exactly one new solve */
    P.p[2]=phi*1.02;
    t0=now(); v=sasfit_sq_RYPolydisperseYukawa(1.0,&P); dt=now()-t0;
    printf("after phi change (warm re-solve): S(1.0)=%.6f  [%.2f s]\n",v,dt);

    printf("\n_f/_v stubs: %.1f %.1f (expect 0.0 0.0)\n",
        sasfit_sq_RYPolydisperseYukawa_f(1.0,&P),
        sasfit_sq_RYPolydisperseYukawa_v(1.0,&P,1));
    {sasfit_param B=P; B.p[2]=1.5; v=sasfit_sq_RYPolydisperseYukawa(1.0,&B);
     printf("bad input (phi=1.5) -> S=%.1f (expect 1.0)\n",v);}
    return 0;
}
