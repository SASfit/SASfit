#ifndef CEPHES_POLYN
#define CEPHES_POLYN 1

#ifdef __cplusplus
extern "C" {
#endif

void revers(double y[], double x[], int n);

typedef struct
{
  double n;
  double d;
} fract;

void radd( fract *, fract *, fract * );
void rsub( fract *, fract *, fract * );
void rmul( fract *, fract *, fract * );
void rdiv( fract *, fract *, fract * );

/*
void polini( int maxdeg );
void polprt( fract a[], int na, int d );
void polclr( fract a[], int n );
void polmov( fract a[], int na, fract b[] );
void polmul( fract a[], int na, fract b[], int nb, fract c[] );
void poladd( fract a[], int na, fract b[], int nb, fract c[] );
void polsub( fract a[], int na, fract b[], int nb, fract c[] );
int poldiv( fract a[], int na, fract b[], int nb, fract c[] );
void polsbt( fract a[], int na, fract b[], int nb, fract c[] );
void poleva( fract a[], int na, fract* x, fract* s );
*/
void polini( int maxdeg );
void polprt( double a[], int na, int d );
void polclr( double *a, int n );
void polmov( double *a, int na, double *b );
void polmul( double a[], int na, double b[], int nb, double c[] );
void poladd( double a[], int na, double b[], int nb, double c[] );
void polsub( double a[], int na, double b[], int nb, double c[] );
int poldiv( double a[], int na, double b[], int nb, double c[] );
void polsbt( double a[], int na, double b[], int nb, double c[] );
double poleva( double a[], int na, double x );

int polrt( double xcof[], double cof[], int m, cmplx root[] );

void polatn( double num[], double den[], double ans[], int nn );
void polsqt( double pol[], double ans[], int nn );
void polsin( double x[], double y[], int nn );
void polcos( double x[], double y[], int nn );

#ifdef __cplusplus
}
#endif

#endif // CEPHES_POLYN
