# include <complex.h>

float complex c4_uniform_01 ( int *seed );
void c4mat_print ( int m, int n, float complex a[], char *title );
void c4mat_print_some ( int m, int n, float complex a[], int ilo, int jlo, 
  int ihi, int jhi, char *title );
float complex *c4mat_test ( int n );
float complex *c4mat_test_inverse ( int n );
double complex c8_uniform_01 ( int *seed );
void c8mat_print ( int m, int n, double complex a[], char *title );
void c8mat_print_some ( int m, int n, double complex a[], int ilo, int jlo, 
  int ihi, int jhi, char *title );
double complex *c8mat_test ( int n );
double complex *c8mat_test_inverse ( int n );
float cabs1 ( float complex z );
float cabs2 ( float complex z );
float cmach ( int job );
float complex csign1 ( float complex z1, float complex z2 );
float complex csign2 ( float complex z1, float complex z2 );
double dmach ( int job );
int i4_max ( int i1, int i2 );
int i4_min ( int i1, int i2 );
int lsame ( char ca, char cb );
float r4_abs ( float x );
float r4_max ( float x, float y );
float r4_sign ( float x );
float r4_uniform_01 ( int *seed );
float r4_uniform_ab ( float a, float b, int *seed );
void r4mat_print ( int m, int n, float a[], char *title );
void r4mat_print_some ( int m, int n, float a[], int ilo, int jlo, int ihi,
  int jhi, char *title );
float *r4mat_test ( char trans, int lda, int m, int n );
void r4mat_uniform_01 ( int m, int n, int *seed, float r[] );
void r4vec_print ( int n, float a[], char *title );
double r8_abs ( double x );
double r8_max ( double x, double y );
double r8_sign ( double x );
double r8_uniform_01 ( int *seed );
double r8_uniform_ab ( double a, double b, int *seed );
void r8mat_print ( int m, int n, double a[], char *title );
void r8mat_print_some ( int m, int n, double a[], int ilo, int jlo, int ihi,
  int jhi, char *title );
double *r8mat_test ( char trans, int lda, int m, int n );
void r8mat_uniform_01 ( int m, int n, int *seed, double r[] );
void r8vec_print ( int n, double a[], char *title );
float smach ( int job );
void timestamp ( );
int triangle_upper_to_i4 ( int i, int j );
void xerbla ( char *srname, int info );
double zabs1 ( double complex z );
double zabs2 ( double complex z );
double zmach ( int job );
double complex zsign1 ( double complex z1, double complex z2 );
double complex zsign2 ( double complex z1, double complex z2 );
