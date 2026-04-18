#ifndef CEPHES_BESSEL
#define CEPHES_BESSEL 1

#ifdef __cplusplus
extern "C" {
#endif

double jv (double nu, double x);
double jn(int n, double x);
double j0(double x);
double j1(double x);

double yv (double nu, double x);
double yn(int n, double x);
double y0(double x);
double y1(double x);

double onef2 (double a, double b, double c, double x, double* err);

double threef0 (double a, double b, double c, double x, double* err);

double cephes_struve(double nu, double x);

double cephes_psi(double x);

double iv(double nu, double x);
double in(int n, double x);
double i0(double x);
double i1(double x);

double kn(int n, double x);
double k0(double x);
double k1(double x);

double hyperg(double a, double b, double x);
double hyp2f1(double a, double b, double c, double x);

int airy(double x, double* ai, double* aip, double* bi, double* bip);

#ifdef __cplusplus
}
#endif

#endif // CEPHES_BESSEL
