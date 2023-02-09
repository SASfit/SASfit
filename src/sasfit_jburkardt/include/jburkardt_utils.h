#ifndef SASFIT_JBURKARDT_H
#define SASFIT_JBURKARDT_H

#include "blas0.h"
#include "blas1_d.h"

int i4_choose ( int n, int k );
int i4_factorial2 ( int n );
int i4_modp ( int i, int j );
int i4_mop ( int i );
int i4_power ( int i, int j );
int i4vec_product ( int n, int a[] );

void i4mat_print ( int m, int n, int a[], char *title );
void i4mat_print_some ( int m, int n, int a[], int ilo, int jlo, int ihi,
  int jhi, char *title );
int *i4vec_cum0_new ( int n, int a[] );


void i4vec_print ( int n, int a[], char *title );
int i4vec_product ( int n, int a[] );
int i4vec_sum ( int n, int a[] );
void i4vec_transpose_print ( int n, int a[], char *title );


int *r8cvv_offset ( int m, int nr[] );
void r8cvv_print ( int mn, double a[], int m, int roff[], char *title );
double *r8cvv_rget_new ( int mn, double a[], int m, int roff[], int i );
void r8cvv_rset ( int mn, double a[], int m, int roff[], int i, double ai[] );
double *r8mat_normal_01_new ( int m, int n, int *seed );
void r8mat_transpose_print ( int m, int n, double a[], char *title );
void r8mat_transpose_print_some ( int m, int n, double a[], int ilo, int jlo,
  int ihi, int jhi, char *title );
double *r8mat_uniform_01_new ( int m, int n, int *seed );
void r8vec_copy ( int n, double a1[], double a2[] );
void r8vec_direct_product ( int factor_index, int factor_order,
  double factor_value[], int factor_num, int point_num, double x[] );
void r8vec_direct_product2 ( int factor_index, int factor_order,
  double factor_value[], int factor_num, int point_num, double w[] );
double r8vec_dot_product ( int n, double a1[], double a2[] );
double *r8vec_normal_01_new ( int n, int *seed );
double r8vec_sum ( int n, double a[] );
void r8vec_transpose_print ( int n, double a[], char *title );
double *r8vec_uniform_01_new ( int n, int *seed );
void r8mat_write ( char *output_filename, int m, int n, double table[] );


void sort_heap_external ( int n, int *indx, int *i, int *j, int isgn );
int symmetric_sparse_size ( int nr, int dim, double nodes[], double x0 );
void tensor_product ( int d, int order1d[], int n1d, double x1d[], 
  double w1d[], int n, double xnd[], double wnd[] );
void tensor_product_cell ( int nc, double xc[], double wc[], int dim, int nr[], 
  int roff[], int np, double xp[], double wp[] );
  
 
double cpu_time ( );

void vec_colex_next2 ( int dim_num, int base[], int a[], int *more );



double cpu_time ( );

#endif // SASFIT_JBURKARDT_H