#include <stdio.h>
#include <math.h>
#include "ry_polydisperse_core.h"
int main(void){
    ryp_system sys; ryp_status st;
    double s=0.2, meanSigma=1.0, nstar=0.005, Z=200.0, LB=7.01/250.0;
    double t=1.0/(s*s)-1.0, m3=1.0, phi; int m,i;
    for(m=1;m<=3;m++) m3 *= (t+m)/(t+1.0);
    phi=(M_PI/6.0)*nstar*m3;
    st=ryp_alloc(&sys,3,4095,100.0); if(st){printf("alloc: %s\n",ryp_strerror(st));return 1;}
    st=ryp_set_schulz(&sys,s,meanSigma); if(st){printf("schulz: %s\n",ryp_strerror(st));return 1;}
    printf("sigma = "); for(i=0;i<sys.p;i++) printf("%.4f ",sys.sigma[i]);
    printf("\nx     = "); for(i=0;i<sys.p;i++) printf("%.4f ",sys.x[i]); printf("\n");
    ryp_set_charges(&sys,Z,2.0,LB,NULL,NULL);
    ryp_set_volume_fraction(&sys,phi);
    ryp_build_potential(&sys);
    printf("n=%.6f  kappa=%.4f\n",sys.nTotal,sys.kappa);
    st=ryp_solve(&sys,0.5,8000,1e-10,0.3);
    printf("solve: %s\n",ryp_strerror(st));
    if(st==RYP_OK){
        double smax=0; int k;
        for(k=0;k<sys.N;k++) if(sys.SM[k]>smax) smax=sys.SM[k];
        printf("S^M_max = %.5f\n",smax);
        {FILE*f=fopen("c_SM.txt","w");
         for(k=0;k<sys.N;k++) fprintf(f,"%.12g %.12g\n",sys.qgrid[k],sys.SM[k]);
         fclose(f);}
    }
    ryp_free(&sys); return 0;
}
