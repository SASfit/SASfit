#ifndef CEPHES_CPROB
#define CEPHES_CPROB 1

#ifdef __cplusplus
extern "C" {
#endif

double log1p(double x);
double expm1(double x);
double cosm1(double x);

double stdtr(int  k, double t);
double stdtri(int k, double p);

double polevl(double x, double coef[], int N);

double pdtrc(int k, double m);
double pdtr(int k, double m);
double pdtri(int k, double y);

double ndtri(double y0);

double ndtr(double a);
double erfc(double a);
double erf(double x);

double incbet (double aa, double bb, double xx);
double incbi (double aa, double bb, double yy0);
double nbdtrc(int k, int n, double p);
double nbdtr(int k, int n, double p);
double nbdtri(int k, int n, double p);

int mtherr(char * name, int code);

double smirnov (int n, double e);
double kolmogorov (double y);
double smirnovi (int n, double p);
double kolmogi (double p);

double igami(double a, double y0);

double igamc(double a, double x);
double igam(double a, double x);

double gdtr(double a, double b, double x);

double gamma(double x);
double lgam(double x);

double fdtrc(int ia, int ib, double x);
double fdtr(int ia, int ib, double x);
double fdtri(int ia, int ib, double y);

double expx2 (double x, int ssign);

int drand(double *a);

double chdtrc(double df,double x);
double chdtr(double df,double x);
double chdtri(double df, double y);

double btdtr(double a, double b, double x);

double bdtrc(int k, int n, double p);
double bdtr(int k, int n, double p);
double bdtri(int k, int n, double y);

#ifdef __cplusplus
}
#endif

#endif // CEPHES_CPROB
