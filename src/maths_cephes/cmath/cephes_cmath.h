#ifndef CEPHES_CMATH
#define CEPHES_CMATH 1

#ifdef __cplusplus
extern "C" {
#endif

#include "mconf.h"

double acosh(double x);
int airy(double, double *, double *, double *, double *);
double asin(double);
double acos(double);
double asinh(double x);
double atan(double);
double atan2(double y, double x);
double atanh(double);
double bdtrc(int k, int n, double p);
double bdtr(int k, int n, double p);
double bdtri(int k, int n, double y);
double beta(double a, double b);
double lbeta(double a, double b);
double btdtr(double a, double b, double x);
double cbrt(double);
double chbevl(double, void *, int);
double chdtrc(double df, double x);
double chdtr(double df, double x);
double chdtri(double df, double y);
void cmov(void *a, void *b);
double hypot(double, double);
double cosh(double);
double dawsn(double xx);
void eigens(double A[], double RR[], double E[], int N);
double ellie(double, double);
double ellik(double, double);
double ellpe(double);
int ellpj(double u, double m, double *sn, double *cn, double *dn, double *ph);
double ellpk(double);
double exp10(double);
double exp1m(double);
double exp2(double);
double expn(int n, double x);
double fac(int i);
double fdtrc(int ia, int ib, double x);
double fdtr(int ia, int ib, double x);
double fdtri(int ia, int ib, double y);
int fftr(double x[], int m0, double sine[]);
double ceil(double x);
double fabs(double);
double floor(double);
double frexp(double, int *);
double ldexp(double, int);
int signbit(double);
int isnan(double);
int isfinite(double);
double polevl(double, double *, int);
double p1evl(double, void *, int);
double exp(double);
double log(double);
double sin(double);
double cos(double);
double sqrt(double);
int fresnl(double xxa, double *ssa, double *cca);
double gamma(double);
double lgam(double);
double gdtr(double a, double b, double x);
double gdtrc(double a, double b, double x);
int gels(double A[], double R[], int M, double EPS, double AUX[]);
double hyp2f1(double a, double b, double c, double x);
double hyperg(double, double, double);
double hyp2f0(double a, double b, double x, int type, double *err);
double i0(double);
double i0e(double x);
double i1(double);
double i1e(double x);
double igamc(double, double);
double igam(double, double);
double igami(double, double);
double incbet(double, double, double);
double incbi(double, double, double);
double iv(double v, double x);
double j0(double);
double y0(double);
double j1(double);
double y1(double);
double jn(int n, double x);
double jv(double, double);
double k0(double x);
double k0e(double x);
double k1(double x);
double k1e(double x);
double kn(int nn, double x);
int levnsn(int n, double r[], double a[], double e[], double refl[]);
double log10(double);
double log2(double);
long lrand(void);
long lsqrt(long x);
int minv(double A[], double X[], int n, double B[], int IPS[]);
int mmmpy(int r, int c, double *A, double *B, double *Y);
int mtherr(char *name, int code);
int mtransp(int n, double *A, double *T);
int mvmpy(int r, int c, double *A, double *V, double *Y);
double nbdtrc(int k, int n, double p);
double nbdtr(int k, int n, double p);
double nbdtri(int k, int n, double p);
double ndtr(double a);
double erfc(double);
double erf(double);
double ndtri(double);
double pdtrc(int k, double m);
double pdtr(int k, double m);
double pdtri(int k, double y);
double pow(double, double);
double powi(double, int);
double psi(double);
void revers(double y[], double x[], int n);
double rgamma(double x);
double round(double);
int sprec(void);
int dprec(void);
int ldprec(void);
int shichi(double x, double *si, double *ci);
int sici(double x, double *si, double *ci);
double simpsn(double f[], double delta);
int simq(double A[], double B[], double X[], int n, int flag, int IPS[]);
double radian(double d, double m, double s);
/* Rename to avoid ugly surprises.
int sincos(double x, double *s, double *c, int flg);
*/
int sincosdg(double x, double *s, double *c, int flg);
double sindg(double x);
double cosdg(double x);
double sinh(double);
double spence(double);
double stdtr(int k, double t);
double stdtri(int k, double p);
double onef2(double a, double b, double c, double x, double *err);
double threef0(double a, double b, double c, double x, double *err);
double struve(double v, double x);
double tan(double);
double cot(double);
double tandg(double x);
double cotdg(double x);
double tanh(double);
double log1p(double);
double exmp1(double);
double cosm1(double x);
double yn(int, double);
double zeta(double x, double q);
double zetac(double);
int drand(double *a);
double smirnov(int, double);
double smirnovi(int, double);
double kolmogorov(double);
double kolmogi(double);

#ifdef __cplusplus
}
#endif

#endif // CEPHES_CMATH
