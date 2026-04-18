#ifndef CEPHES_ELLF
#define CEPHES_ELLF 1

#ifdef __cplusplus
extern "C" {
#endif

double ellik(double phi, double m);
double ellpk(double x);

double ellie(double phi, double m);
double ellpe(double x);

int ellpj(double u, double m, double *sn, double *cn, double *dn, double *ph);

#ifdef __cplusplus
}
#endif

#endif // CEPHES_ELLF
