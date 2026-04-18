#ifndef CEPHES_CMPLX
#define CEPHES_CMPLX 1

#ifdef __cplusplus
extern "C" {
#endif

double _Complex cgamma(double _Complex x);

double _Complex cpolylog (int n, double _Complex x);

#ifdef __cplusplus
}
#endif

#endif // CEPHES_CMPLX
