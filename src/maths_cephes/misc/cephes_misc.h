#ifndef CEPHES_MISC
#define CEPHES_MISC 1

#ifdef __cplusplus
extern "C" {
#endif

double beta(double a, double b);

double dawsn(double x);

double ei(double x);

double expn(int n, double x);

double fac(int i);

int fresnl(double xxa, double* ssa, double* cca);

double plancki(double w, double T);
double planckc(double w, double T);
double planckd(double w, double T);
double planckw(double w, double T);

double polylog(int n, double x);

double psi(double x);

double rgamma(double x);

int shichi(double x, double* shi, double* chi);

int sici(double x, double* si, double* ci);

double spence(double x);

double zeta(double x,double q);

double zetac(double x);

#ifdef __cplusplus
}
#endif

#endif // CEPHES_MISC
