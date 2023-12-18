

#include <math.h>

#include <stdio.h>
#include <stdlib.h>

#include "sasfit.h"
#include "SASFIT_nr.h"
#include "sasfit_sq.h"
#include "sasfit_ff.h"
#include <gsl/gsl_sf.h>

#include "bool.h"
#undef float

#include "f2c.h"

unsigned short MAXNUM[4] = {0xffff,0xffff,0xffff,0x7fef};
unsigned short MACHEP[4] = {0x0000,0x0000,0x0000,0x3ca0};

float MAXNUMF = 1.7014117331926442990585209174225846272e38;
float MACHEPF = 5.9604644775390625E-8;

//#include "pfq_f2c.inc"
#include "pfq.new.c"

double Hypergeom0F1(double c, double x)
{
	return gsl_sf_hyperg_0F1(c,x);
}

double Hypergeom1F2(double a1, double b1, double b2, double x)
{
	doublereal Res1F2;
	doublecomplex A[1], B[2], z, ret_val;
	integer ln_pFq, ix, nsigfig, ip, iq;
	ftnlen errorstr_len=256;
	bool error;
	char errorstr[256];
//	double err;

//	Res1F2 = onef2(a1,b1,b2,x,&err);
//	return Res1F2;

	ln_pFq = 1;
	ix = 0;
	nsigfig = 10;
	ip = 1;
	iq = 2;

	A[0].i = 0;
	A[0].r = a1;
	B[0].i = 0;
	B[0].r = b1;
	B[1].i = 0;
	B[1].r = b2;

	z.i=0;
	z.r=x;

	pfq_( &ret_val, A, B, &ip, &iq, &z, &ln_pFq, &ix, &nsigfig, &error, errorstr, errorstr_len);
//    pfq_( &ret_val, A, B, &ip, &iq, &z, &ln_pFq, &ix, &nsigfig);
	Res1F2 = ret_val.r;
	return exp(Res1F2);
}


double sasfit_2f1(double a, double b,
                  double c, double x,
                  sasfit_param * param)
{
/*
	doublereal Res2F1;
	doublecomplex A[2], B[1], z, ret_val;
	integer ln_pFq, ix, nsigfig, ip, iq,errorstr_len=256;
	bool error;
	char errorstr[256];
	double err;

 	ln_pFq = 0;
	ix = 700;
	nsigfig = 20;
	ip = 2;
	iq = 1;

	A[0].i = 0;
	A[0].r = a;

	A[1].i = 0;
	A[1].r = b;

	B[0].i = 0;
	B[0].r = c;

	z.i=0;
	z.r=x;

	pfq_( &ret_val, &A, &B, &ip, &iq, &z, &ln_pFq, &ix, &nsigfig, &error, errorstr,&errorstr_len);
    if (error == TRUE) sasfit_err("%s\ncall of 2F1(%lf,%lf,%lf,%lf) failed\n",errorstr,a,b,c,x);
	Res2F1 = ret_val.r;
	return Res2F1;
	return exp(Res2F1);
*/

    // Forrey, R. C.
    // Computing the Hypergeometric Function
    // Journal of Computational Physics,
    // 1997, 137, 79 - 100
    // http://dx.doi.org/10.1006/jcph.1997.5794
    double coeff1, coeff2, w;
    gsl_sf_result  c1,c2,c3,c4,arg;
    if (x<-1) {
        // I. w = 1/(1-z)
        w = 1./(1.-x);
        gsl_sf_lngamma_complex_e(c,0,  &c1,&arg);
        gsl_sf_lngamma_complex_e(b-a,0,&c2,&arg);
        gsl_sf_lngamma_complex_e(b,0,  &c3,&arg);
        gsl_sf_lngamma_complex_e(c-a,0,&c4,&arg);
        coeff1=pow(w,a)*exp(c1.val+c2.val-c3.val-c4.val);
        coeff1=pow(w,a)*gsl_sf_gamma(c)*gsl_sf_gamma(b-a)
                      /(gsl_sf_gamma(b)*gsl_sf_gamma(c-a));

        gsl_sf_lngamma_complex_e(c,0,  &c1,&arg);
        gsl_sf_lngamma_complex_e(a-b,0,&c2,&arg);
        gsl_sf_lngamma_complex_e(a,0,  &c3,&arg);
        gsl_sf_lngamma_complex_e(c-b,0,&c4,&arg);
        coeff2=pow(w,a)*exp(c1.val+c2.val-c3.val-c4.val);

        coeff2=pow(w,b)*gsl_sf_gamma(c)*gsl_sf_gamma(a-b)
                      /(gsl_sf_gamma(a)*gsl_sf_gamma(c-b));
        return   coeff1*gsl_sf_hyperg_2F1(a,c-b,a-b+1,w)
                +coeff2*gsl_sf_hyperg_2F1(b,c-a,b-a+1,w);
    } else if (-1<=x && x < 0) {
        // II. w=z/(z-1)
        w=x/(x-1.);
        return pow(1-w,a)*gsl_sf_hyperg_2F1(a,c-b,c,w);
    } else if (0<=x && x <=0.5) {
        // III. w = z
        return gsl_sf_hyperg_2F1(a,b,c,x);
    } else if (0.5<x && x<=1) {
        // IV. w=1-z
        w = 1.-x;
        coeff1 =  gsl_sf_gamma(c)  *gsl_sf_gamma(c-a-b)
                /(gsl_sf_gamma(c-a)*gsl_sf_gamma(c-b));
        coeff2 =  gsl_sf_gamma(c)*gsl_sf_gamma(a+b-c)
                /(gsl_sf_gamma(a)*gsl_sf_gamma(b))
                * pow(w,c-a-b);
        return   coeff1*gsl_sf_hyperg_2F1(a,b,a+b-c+1,w)
                +coeff2*gsl_sf_hyperg_2F1(c-a,c-b,c-a-b+1,w);
    } else if (1<x && x <=2) {
        // V. w = 1-1/z
        w = 1.-1./x;
        coeff1 =  gsl_sf_gamma(c)  *gsl_sf_gamma(c-a-b)
                /(gsl_sf_gamma(c-a)*gsl_sf_gamma(c-b))
                * pow(x,-a);
        coeff2 =  gsl_sf_gamma(c)*gsl_sf_gamma(a+b-c)
                /(gsl_sf_gamma(a)*gsl_sf_gamma(b))
                * pow(1-x,c-a-b)*pow(x,a-c);
        return   coeff1*gsl_sf_hyperg_2F1(a,a-c+1,a+b-c+1,w)
                +coeff2*gsl_sf_hyperg_2F1(c-a,1-a,c-a-b+1,w);
    } else {
        // VI. w = 1/z
        w = 1./x;
        coeff1 =  gsl_sf_gamma(c)*gsl_sf_gamma(b-a)
                /(gsl_sf_gamma(b)*gsl_sf_gamma(c-a))
                *pow(fabs(w),a);
        coeff2 =  gsl_sf_gamma(c)*gsl_sf_gamma(a-b)
                /(gsl_sf_gamma(a)*gsl_sf_gamma(c-b))
                * pow(fabs(w),b);
        // The real part is given by:
        return   cos(M_PI*a)* gsl_sf_hyperg_2F1(a,a-c+1,a-b+1,w)
                +cos(M_PI*b)* gsl_sf_hyperg_2F1(b-c+1,b,b-a+1,w);

        // The imaginary part is given by:
//        return   sin(M_PI*a)* gsl_sf_hyperg_2F1(a,a-c+1,a-b+1,w)
//                +sin(M_PI*b)* gsl_sf_hyperg_2F1(b-c+1,b,b-a+1,w);
    }

}

double sasfit_3f2(double a1, double a2, double a3,
                  double b1, double b2, double x,
                  sasfit_param * param)
{

	doublereal Res3F2;
	doublecomplex A[3], B[2], z, ret_val;
	integer ln_pFq, ix, nsigfig, ip, iq;
	ftnint errstr_len = STRLEN;

	SASFIT_ASSERT_PTR(param);

	ln_pFq = 1;
	ix = 0;
	nsigfig = 4;
	ip = 3;
	iq = 2;

	A[0].i = 0;
	A[0].r = a1;
	A[1].i = 0;
	A[1].r = a2;
	A[2].i = 0;
	A[2].r = a3;
	B[0].i = 0;
	B[0].r = b1;
	B[1].i = 0;
	B[1].r = b2;

	z.i = 0;
	z.r = x;

    param->errStatus=FALSE;
	// I assume ierr is just a flag here ?
	pfq_( &ret_val, A, B, &ip, &iq, &z, &ln_pFq, &ix, &nsigfig,
		&param->errStatus, param->errStr, errstr_len);
//    pfq_( &ret_val, A, B, &ip, &iq, &z, &ln_pFq, &ix, &nsigfig);
	// is errstr_len written written with the final str length afterwards ?
	// param->errLen = errstr_len;

	Res3F2 = ret_val.r;
	return exp(Res3F2);
}

double sasfit_pfq(double *p_r, double *p_i,  double *q_r, double *q_i, integer ip, integer iq,
                  double z_r, double z_i, integer ln_pFq, integer ix, double *pFq_r, double *pFq_i, integer nsigfig, sasfit_param * param)
{
	doublecomplex *P, *Q, z, ret_val;
	integer errstr_len = STRLEN;
	int i;

	P = malloc(ip * sizeof(doublecomplex));
	Q = malloc(iq * sizeof(doublecomplex));

    for (i=0; i<ip; i++) {
        P[i].r = p_r[i];
        P[i].i = p_i[i];
    }
    for (i=0; i<iq; i++) {
        Q[i].r = q_r[i];
        Q[i].i = q_i[i];
    }
  	z.i = z_i;
	z.r = z_r;
    param->errStatus=FALSE;
	pfq_( &ret_val, P, Q, &ip, &iq, &z, &ln_pFq, &ix, &nsigfig,&param->errStatus, param->errStr, errstr_len);
//	pfq_( &ret_val, P, Q, &ip, &iq, &z, &ln_pFq, &ix, &nsigfig);
    *pFq_r = ret_val.r;
    *pFq_i = ret_val.i;
    free(P);
    free(Q);
    return gsl_hypot(ret_val.r,ret_val.i);
}
#define float double
