#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <gsl/gsl_fft_real.h>
static void dst1(const double *in,double *out,int N,double *buf){
    const int M=2*(N+1); int k;
    memset(buf,0,sizeof(double)*(size_t)M);
    for(k=0;k<N;k++){ buf[k+1]=in[k]; buf[M-1-k]=-in[k]; }
    gsl_fft_real_radix2_transform(buf,1,(size_t)M);
    for(k=0;k<N;k++){ double im=buf[M-(k+1)]; out[k]=-im; }
}
int main(void){
    int N=7,k; double in[7],out[7],*buf=malloc(sizeof(double)*2*(N+1));
    for(k=0;k<N;k++) in[k]=1.0/(k+1.0);
    dst1(in,out,N,buf);
    for(k=0;k<N;k++) printf("%.10f\n",out[k]);
    free(buf); return 0;
}
