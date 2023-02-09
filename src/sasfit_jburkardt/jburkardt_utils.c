# include <stdlib.h>
# include <stdio.h>
# include <math.h>
# include <time.h>
# include <string.h>

# include "jburkardt_utils.h"

/******************************************************************************/

double cpu_time ( )

/******************************************************************************/
/*
  Purpose:

    CPU_TIME returns the current reading on the CPU clock.

  Discussion:

    The CPU time measurements available through this routine are often
    not very accurate.  In some cases, the accuracy is no better than
    a hundredth of a second.  

  Licensing:

    This code is distributed under the GNU LGPL license. 

  Modified:

    06 June 2005

  Author:

    John Burkardt

  Parameters:

    Output, double CPU_TIME, the current reading of the CPU clock, in seconds.
*/
{
  double value;

  value = ( double ) clock ( ) 
        / ( double ) CLOCKS_PER_SEC;

  return value;
}
/******************************************************************************/

int i4_choose ( int n, int k )

/******************************************************************************/
/*
  Purpose:

    I4_CHOOSE computes the binomial coefficient C(N,K).

  Discussion:

    The value is calculated in such a way as to avoid overflow and
    roundoff.  The calculation is done in integer arithmetic.

    The formula used is:

      C(N,K) = N! / ( K! * (N-K)! )

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    03 May 2008

  Author:

    John Burkardt

  Reference:

    ML Wolfson, HV Wright,
    Algorithm 160:
    Combinatorial of M Things Taken N at a Time,
    Communications of the ACM,
    Volume 6, Number 4, April 1963, page 161.

  Parameters:

    Input, int N, K, are the values of N and K.

    Output, int I4_CHOOSE, the number of combinations of N
    things taken K at a time.
*/
{
  int i;
  int mn;
  int mx;
  int value;

  mn = i4_min ( k, n - k );

  if ( mn < 0 )
  {
    value = 0;
  }
  else if ( mn == 0 )
  {
    value = 1;
  }
  else
  {
    mx = i4_max ( k, n - k );
    value = mx + 1;

    for ( i = 2; i <= mn; i++ )
    {
      value = ( value * ( mx + i ) ) / i;
    }
  }

  return value;
}
/******************************************************************************/

int i4_factorial2 ( int n )

/******************************************************************************/
/*
  Purpose:

    I4_FACTORIAL2 computes the double factorial function.

  Discussion:

    FACTORIAL2( N ) = Product ( N * (N-2) * (N-4) * ... * 2 )  (N even)
                    = Product ( N * (N-2) * (N-4) * ... * 1 )  (N odd)

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    04 March 2008

  Author:

    John Burkardt

  Parameters:

    Input, int N, the argument of the double factorial function.
    If N is less than 1, I4_FACTORIAL2 is returned as 1.

    Output, int I4_FACTORIAL2, the value of the double factorial function.
*/
{
  int n_copy;
  int value;

  if ( n < 1 )
  {
    value = 1;
    return value;
  }

  n_copy = n;
  value = 1;

  while ( 1 < n_copy )
  {
    value = value * n_copy;
    n_copy = n_copy - 2;
  }

  return value;
}
/******************************************************************************/

int i4_mop ( int i )

/******************************************************************************/
/*
  Purpose:

    I4_MOP returns the I-th power of -1 as an I4 value.

  Discussion:

    An I4 is an int value.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    16 November 2007

  Author:

    John Burkardt

  Parameters:

    Input, int I, the power of -1.

    Output, int I4_MOP, the I-th power of -1.
*/
{
  int value;

  if ( ( i % 2 ) == 0 )
  {
    value = 1;
  }
  else
  {
    value = -1;
  }

  return value;
}
/******************************************************************************/

int i4_modp ( int i, int j )

/******************************************************************************/
/*
  Purpose:

    I4_MODP returns the nonnegative remainder of I4 division.

  Discussion:

    If
      NREM = I4_MODP ( I, J )
      NMULT = ( I - NREM ) / J
    then
      I = J * NMULT + NREM
    where NREM is always nonnegative.

    The MOD function computes a result with the same sign as the
    quantity being divided.  Thus, suppose you had an angle A,
    and you wanted to ensure that it was between 0 and 360.
    Then mod(A,360) would do, if A was positive, but if A
    was negative, your result would be between -360 and 0.

    On the other hand, I4_MODP(A,360) is between 0 and 360, always.

  Example:

        I         J     MOD  I4_MODP   I4_MODP Factorization

      107        50       7       7    107 =  2 *  50 + 7
      107       -50       7       7    107 = -2 * -50 + 7
     -107        50      -7      43   -107 = -3 *  50 + 43
     -107       -50      -7      43   -107 =  3 * -50 + 43

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    12 January 2007

  Author:

    John Burkardt

  Parameters:

    Input, int I, the number to be divided.

    Input, int J, the number that divides I.

    Output, int I4_MODP, the nonnegative remainder when I is
    divided by J.
*/
{
  int value;

  if ( j == 0 )
  {
    fprintf ( stderr, "\n" );
    fprintf ( stderr, "I4_MODP - Fatal error!\n" );
    fprintf ( stderr, "  I4_MODP ( I, J ) called with J = %d\n", j );
    exit ( 1 );
  }

  value = i % j;

  if ( value < 0 )
  {
    value = value + abs ( j );
  }

  return value;
}
/******************************************************************************/

int i4_power ( int i, int j )

/******************************************************************************/
/*
  Purpose:

    I4_POWER returns the value of I^J.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    23 October 2007

  Author:

    John Burkardt

  Parameters:

    Input, int I, J, the base and the power.  J should be nonnegative.

    Output, int I4_POWER, the value of I^J.
*/
{
  int k;
  int value;

  if ( j < 0 )
  {
    if ( i == 1 )
    {
      value = 1;
    }
    else if ( i == 0 )
    {
      fprintf ( stderr, "\n" );
      fprintf ( stderr, "I4_POWER - Fatal error!\n" );
      fprintf ( stderr, "  I^J requested, with I = 0 and J negative.\n" );
      exit ( 1 );
    }
    else
    {
      value = 0;
    }
  }
  else if ( j == 0 )
  {
    if ( i == 0 )
    {
      fprintf ( stderr, "\n" );
      fprintf ( stderr, "I4_POWER - Fatal error!\n" );
      fprintf ( stderr, "  I^J requested, with I = 0 and J = 0.\n" );
      exit ( 1 );
    }
    else
    {
      value = 1;
    }
  }
  else if ( j == 1 )
  {
    value = i;
  }
  else
  {
    value = 1;
    for ( k = 1; k <= j; k++ )
    {
      value = value * i;
    }
  }
  return value;
}
/******************************************************************************/

void i4mat_print ( int m, int n, int a[], char *title )

/******************************************************************************/
/*
  Purpose:

    I4MAT_PRINT prints an I4MAT.

  Discussion:

    An I4MAT is an MxN array of I4's, stored by (I,J) -> [I+J*M].

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    28 May 2008

  Author:

    John Burkardt

  Parameters:

    Input, int M, the number of rows in A.

    Input, int N, the number of columns in A.

    Input, int A[M*N], the M by N matrix.

    Input, char *TITLE, a title.
*/
{
  i4mat_print_some ( m, n, a, 1, 1, m, n, title );

  return;
}
/******************************************************************************/

void i4mat_print_some ( int m, int n, int a[], int ilo, int jlo, int ihi,
  int jhi, char *title )

/******************************************************************************/
/*
  Purpose:

    I4MAT_PRINT_SOME prints some of an I4MAT.

  Discussion:

    An I4MAT is an MxN array of I4's, stored by (I,J) -> [I+J*M].

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    20 August 2010

  Author:

    John Burkardt

  Parameters:

    Input, int M, the number of rows of the matrix.
    M must be positive.

    Input, int N, the number of columns of the matrix.
    N must be positive.

    Input, int A[M*N], the matrix.

    Input, int ILO, JLO, IHI, JHI, designate the first row and
    column, and the last row and column to be printed.

    Input, char *TITLE, a title.
*/
{
# define INCX 10

  int i;
  int i2hi;
  int i2lo;
  int j;
  int j2hi;
  int j2lo;

  fprintf ( stdout, "\n" );
  fprintf ( stdout, "%s\n", title );

  if ( m <= 0 || n <= 0 )
  {
    fprintf ( stdout, "\n" );
    fprintf ( stdout, "  (None)\n" );
    return;
  }
/*
  Print the columns of the matrix, in strips of INCX.
*/
  for ( j2lo = jlo; j2lo <= jhi; j2lo = j2lo + INCX )
  {
    j2hi = j2lo + INCX - 1;
    j2hi = i4_min ( j2hi, n );
    j2hi = i4_min ( j2hi, jhi );

    fprintf ( stdout, "\n" );
/*
  For each column J in the current range...

  Write the header.
*/
    fprintf ( stdout, "  Col:" );
    for ( j = j2lo; j <= j2hi; j++ )
    {
      fprintf ( stdout, "  %6d", j - 1 );
    }
    fprintf ( stdout, "\n" );
    fprintf ( stdout, "  Row\n" );
    fprintf ( stdout, "\n" );
/*
  Determine the range of the rows in this strip.
*/
    i2lo = i4_max ( ilo, 1 );
    i2hi = i4_min ( ihi, m );

    for ( i = i2lo; i <= i2hi; i++ )
    {
/*
  Print out (up to INCX) entries in row I, that lie in the current strip.
*/
      fprintf ( stdout, "%5d:", i - 1 );
      for ( j = j2lo; j <= j2hi; j++ )
      {
        fprintf ( stdout, "  %6d", a[i-1+(j-1)*m] );
      }
      fprintf ( stdout, "\n" );
    }
  }

  return;
# undef INCX
}
/******************************************************************************/

int *i4vec_cum0_new ( int n, int a[] )

/******************************************************************************/
/*
  Purpose:

    I4VEC_CUM0_NEW computes the cumulutive sum of the entries of an I4VEC.

  Discussion:

    An I4VEC is a vector of I4's.

    This routine returns a vector of length N+1, with the first value
    being 0.

  Example:

    Input:

      A = (/ 1, 2, 3, 4 /)

    Output:

      A_CUM = (/ 0, 1, 3, 6, 10 /)

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    22 December 2010

  Author:

    John Burkardt

  Parameters:

    Input, int N, the number of entries in the vector.

    Input, int A[N], the vector to be summed.

    Output, int I4VEC_CUM0_NEW[N+1], the cumulative sum of the entries of A.
*/
{
  int *a_cum;
  int i;

  a_cum = ( int * ) malloc ( ( n + 1 ) * sizeof ( int ) );

  a_cum[0] = 0;

  for ( i = 1; i <= n; i++ )
  {
    a_cum[i] = a_cum[i-1] + a[i-1];
  }

  return a_cum;
}
/******************************************************************************/

void i4vec_print ( int n, int a[], char *title )

/******************************************************************************/
/*
  Purpose:

    I4VEC_PRINT prints an I4VEC.

  Discussion:

    An I4VEC is a vector of I4's.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    14 November 2003

  Author:

    John Burkardt

  Parameters:

    Input, int N, the number of components of the vector.

    Input, int A[N], the vector to be printed.

    Input, char *TITLE, a title.
*/
{
  int i;

  fprintf ( stdout, "\n" );
  fprintf ( stdout, "%s\n", title );
  fprintf ( stdout, "\n" );

  for ( i = 0; i < n; i++ )
  {
    fprintf ( stdout, "  %6d: %8d\n", i, a[i] );
  }
  return;
}
/******************************************************************************/

int i4vec_product ( int n, int a[] )

/******************************************************************************/
/*
  Purpose:

    I4VEC_PRODUCT multiplies the entries of an I4VEC.

  Discussion:

    An I4VEC is a vector of I4's.

  Example:

    Input:

      A = ( 1, 2, 3, 4 )

    Output:

      I4VEC_PRODUCT = 24

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    26 August 2008

  Author:

    John Burkardt

  Parameters:

    Input, int N, the number of entries in the vector.

    Input, int A[N], the vector

    Output, int I4VEC_PRODUCT, the product of the entries of A.
*/
{
  int i;
  int product;

  product = 1;
  for ( i = 0; i < n; i++ )
  {
    product = product * a[i];
  }

  return product;
}
/******************************************************************************/

int i4vec_sum ( int n, int a[] )

/******************************************************************************/
/*
  Purpose:

    I4VEC_SUM sums the entries of an I4VEC.

  Discussion:

    An I4VEC is a vector of I4's.

  Example:

    Input:

      A = ( 1, 2, 3, 4 )

    Output:

      I4VEC_SUM = 10

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    29 May 2003

  Author:

    John Burkardt

  Parameters:

    Input, int N, the number of entries in the vector.

    Input, int A[N], the vector to be summed.

    Output, int I4VEC_SUM, the sum of the entries of A.
*/
{
  int i;
  int sum;

  sum = 0;
  for ( i = 0; i < n; i++ )
  {
    sum = sum + a[i];
  }

  return sum;
}
/******************************************************************************/

void i4vec_transpose_print ( int n, int a[], char *title )

/******************************************************************************/
/*
  Purpose:

    I4VEC_TRANSPOSE_PRINT prints an I4VEC "transposed".

  Discussion:

    An I4VEC is a vector of I4's.

  Example:

    A = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 }
    TITLE = "My vector:  "

    My vector:      1    2    3    4    5
                    6    7    8    9   10
                   11

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    25 July 2010

  Author:

    John Burkardt

  Parameters:

    Input, int N, the number of components of the vector.

    Input, int A[N], the vector to be printed.

    Input, char *TITLE, a title.
*/
{
  int i;
  int ihi;
  int ilo;
  int title_len;

  title_len = strlen ( title );

  for ( ilo = 1; ilo <= n; ilo = ilo + 5 )
  {
    ihi = i4_min ( ilo + 5 - 1, n );
    if ( ilo == 1 )
    {
      printf ( "%s", title );
    }
    else
    {
      for ( i = 1; i <= title_len; i++ )
      {
        printf ( " " );
      }
    }
    for ( i = ilo; i <= ihi; i++ )
    {
      printf ( "%12d", a[i-1] );
    }
    printf ( "\n" );
  }

  return;
}
/******************************************************************************/

int *r8cvv_offset ( int m, int nr[] )

/******************************************************************************/
/*
  Purpose:

    R8CVV_OFFSET determines the row offsets of an R8CVV.

  Discussion:

    An R8CVV is a "vector of vectors" of R8's.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    16 November 2012

  Author:

    John Burkardt

  Parameters:

    Input, int M, the number of rows in the array.

    Input, int NR(M), the row sizes.

    Output, int R8CVV_OFFSET[M+1], the row offsets.
*/
{
  int i;
  int *roff;

  roff = ( int * ) malloc ( ( m + 1 ) * sizeof ( int ) );

  roff[0] = 0;
  for ( i = 0; i < m; i++ )
  {
    roff[i+1] = roff[i] + nr[i];
  }

  return roff;
}
/******************************************************************************/

void r8cvv_print ( int mn, double a[], int m, int roff[], char *title )

/******************************************************************************/
/*
  Purpose:

    R8CVV_PRINT prints an R8CVV.

  Discussion:

    An R8CVV is a "vector of vectors" of R8's.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    16 November 2012

  Author:

    John Burkardt

  Parameters:

    Input, int MN, the size of the cell array.

    Input, double A[MN], the cell array.

    Input, int M, the number of rows in the array.

    Input, int ROFF[M+1], the row offsets.

    Input, char *TITLE, a title.
*/
{
  int i;
  int k;
  int k1;
  int k2;
  int khi;
  int klo;

  printf ( "\n" );
  printf ( "%s\n", title );
  printf ( "\n" );

  for ( i = 0; i < m; i++ )
  {
    k1 = roff[i];
    k2 = roff[i+1];

    for ( klo = k1; klo < k2; klo = klo + 5 )
    {
      khi = i4_min ( klo + 5, k2 );
      if ( klo == k1 )
      {
        printf ( "%5d", i );
      }
      else
      {
        printf ( "     " );
      }
      printf ( "  " );
      for ( k = klo; k < khi; k++ )
      {
        printf ( "%14g", a[k] );
      }
      printf ( "\n" );
    }
  }
  return;
}
/******************************************************************************/

double *r8cvv_rget_new ( int mn, double a[], int m, int roff[], int i )

/******************************************************************************/
/*
  Purpose:

    R8CVV_RGET_NEW gets row I from an R8CVV.

  Discussion:

    An R8CVV is a "vector of vectors" of R8's.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    16 November 2012

  Author:

    John Burkardt

  Parameters:

    Input, int MN, the size of the cell array.

    Input, double A[MN], the cell array.

    Input, int M, the number of rows in the array.

    Input, int ROFF[M+1], the row offsets.

    Input, int I, the row.
    0 <= I < M.

    Output, double R8CVV_RGET_NEW[NR[I]], the value of A(I,*).
*/
{
  double *ai;
  int j;
  int k1;
  int k2;
  int nv;

  k1 = roff[i];
  k2 = roff[i+1];
  nv = k2 - k1;
  ai = ( double * ) malloc ( nv * sizeof ( double ) );
  for ( j = 0; j < nv; j++ )
  {
    ai[j] = a[k1+j];
  }

  return ai;
}
/******************************************************************************/

void r8cvv_rset ( int mn, double a[], int m, int roff[], int i, double ai[] )

/******************************************************************************/
/*
  Purpose:

    R8CVV_RSET sets row I from an R8CVV.

  Discussion:

    An R8CVV is a "vector of vectors" of R8's.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    16 November 2012

  Author:

    John Burkardt

  Parameters:

    Input, int MN, the size of the cell array.

    Input/output, double A[MN], the cell array.

    Input, int M, the number of rows in the array.

    Input, int ROFF[M+1], the row offsets.

    Input, int I, the row.
    0 <= I < M.

    Input, double AI[NR[I]], the new value of A(I,*).
*/
{
  int j;
  int k1;
  int k2;
  int nv;

  k1 = roff[i];
  k2 = roff[i+1];
  nv = k2 - k1;
  for ( j = 0; j < nv; j++ )
  {
    a[k1+j] = ai[j];
  }

  return;
}
/******************************************************************************/

double *r8mat_normal_01_new ( int m, int n, int *seed )

/******************************************************************************/
/*
  Purpose:

    R8MAT_NORMAL_01_NEW returns a unit pseudonormal R8MAT.

  Licensing:

    This code is distributed under the GNU LGPL license. 

  Modified:

    03 October 2005

  Author:

    John Burkardt

  Reference:

    Paul Bratley, Bennett Fox, Linus Schrage,
    A Guide to Simulation,
    Springer Verlag, pages 201-202, 1983.

    Bennett Fox,
    Algorithm 647:
    Implementation and Relative Efficiency of Quasirandom
    Sequence Generators,
    ACM Transactions on Mathematical Software,
    Volume 12, Number 4, pages 362-376, 1986.

    Peter Lewis, Allen Goodman, James Miller,
    A Pseudo-Random Number Generator for the System/360,
    IBM Systems Journal,
    Volume 8, pages 136-143, 1969.

  Parameters:

    Input, int M, N, the number of rows and columns in the array.

    Input/output, int *SEED, the "seed" value, which should NOT be 0.
    On output, SEED has been updated.

    Output, double R8MAT_NORMAL_01_NEW[M*N], the array of pseudonormal values.
*/
{
  double *r;

  r = r8vec_normal_01_new ( m * n, seed );

  return r;
}
/******************************************************************************/

void r8mat_transpose_print ( int m, int n, double a[], char *title )

/******************************************************************************/
/*
  Purpose:

    R8MAT_TRANSPOSE_PRINT prints an R8MAT, transposed.

  Discussion:

    An R8MAT is a doubly dimensioned array of R8 values, stored as a vector
    in column-major order.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    28 May 2008

  Author:

    John Burkardt

  Parameters:

    Input, int M, N, the number of rows and columns.

    Input, double A[M*N], an M by N matrix to be printed.

    Input, char *TITLE, a title.
*/
{
  r8mat_transpose_print_some ( m, n, a, 1, 1, m, n, title );

  return;
}
/******************************************************************************/

void r8mat_transpose_print_some ( int m, int n, double a[], int ilo, int jlo,
  int ihi, int jhi, char *title )

/******************************************************************************/
/*
  Purpose:

    R8MAT_TRANSPOSE_PRINT_SOME prints some of an R8MAT, transposed.

  Discussion:

    An R8MAT is a doubly dimensioned array of R8 values, stored as a vector
    in column-major order.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    20 August 2010

  Author:

    John Burkardt

  Parameters:

    Input, int M, N, the number of rows and columns.

    Input, double A[M*N], an M by N matrix to be printed.

    Input, int ILO, JLO, the first row and column to print.

    Input, int IHI, JHI, the last row and column to print.

    Input, char *TITLE, a title.
*/
{
# define INCX 5

  int i;
  int i2;
  int i2hi;
  int i2lo;
  int inc;
  int j;
  int j2hi;
  int j2lo;

  fprintf ( stdout, "\n" );
  fprintf ( stdout, "%s\n", title );

  if ( m <= 0 || n <= 0 )
  {
    fprintf ( stdout, "\n" );
    fprintf ( stdout, "  (None)\n" );
    return;
  }

  for ( i2lo = i4_max ( ilo, 1 ); i2lo <= i4_min ( ihi, m ); i2lo = i2lo + INCX )
  {
    i2hi = i2lo + INCX - 1;
    i2hi = i4_min ( i2hi, m );
    i2hi = i4_min ( i2hi, ihi );

    inc = i2hi + 1 - i2lo;

    fprintf ( stdout, "\n" );
    fprintf ( stdout, "  Row:" );
    for ( i = i2lo; i <= i2hi; i++ )
    {
      fprintf ( stdout, "  %7d     ", i - 1 );
    }
    fprintf ( stdout, "\n" );
    fprintf ( stdout, "  Col\n" );
    fprintf ( stdout, "\n" );

    j2lo = i4_max ( jlo, 1 );
    j2hi = i4_min ( jhi, n );

    for ( j = j2lo; j <= j2hi; j++ )
    {
      fprintf ( stdout, "%5d:", j - 1 );
      for ( i2 = 1; i2 <= inc; i2++ )
      {
        i = i2lo - 1 + i2;
        fprintf ( stdout, "  %14f", a[(i-1)+(j-1)*m] );
      }
      fprintf ( stdout, "\n" );
    }
  }

  return;
# undef INCX
}
/******************************************************************************/

double *r8mat_uniform_01_new ( int m, int n, int *seed )

/******************************************************************************/
/*
  Purpose:

    R8MAT_UNIFORM_01_NEW fills an R8MAT with pseudorandom values scaled to [0,1].

  Discussion:

    An R8MAT is a doubly dimensioned array of R8 values, stored as a vector
    in column-major order.

    This routine implements the recursion

      seed = 16807 * seed mod ( 2^31 - 1 )
      unif = seed / ( 2^31 - 1 )

    The integer arithmetic never requires more than 32 bits,
    including a sign bit.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    30 June 2009

  Author:

    John Burkardt

  Reference:

    Paul Bratley, Bennett Fox, Linus Schrage,
    A Guide to Simulation,
    Springer Verlag, pages 201-202, 1983.

    Bennett Fox,
    Algorithm 647:
    Implementation and Relative Efficiency of Quasirandom
    Sequence Generators,
    ACM Transactions on Mathematical Software,
    Volume 12, Number 4, pages 362-376, 1986.

    Philip Lewis, Allen Goodman, James Miller,
    A Pseudo-Random Number Generator for the System/360,
    IBM Systems Journal,
    Volume 8, pages 136-143, 1969.

  Parameters:

    Input, int M, N, the number of rows and columns.

    Input/output, int *SEED, the "seed" value.  Normally, this
    value should not be 0, otherwise the output value of SEED
    will still be 0, and R8_UNIFORM will be 0.  On output, SEED has
    been updated.

    Output, double R8MAT_UNIFORM_01_NEW[M*N], a matrix of pseudorandom values.
*/
{
  int i;
  int j;
  int k;
  double *r;

  r = ( double * ) malloc ( m * n * sizeof ( double ) );

  for ( j = 0; j < n; j++ )
  {
    for ( i = 0; i < m; i++ )
    {
      k = *seed / 127773;

      *seed = 16807 * ( *seed - k * 127773 ) - k * 2836;

      if ( *seed < 0 )
      {
        *seed = *seed + 2147483647;
      }
      r[i+j*m] = ( double ) ( *seed ) * 4.656612875E-10;
    }
  }

  return r;
}
/******************************************************************************/

void r8vec_copy ( int n, double a1[], double a2[] )

/******************************************************************************/
/*
  Purpose:

    R8VEC_COPY copies an R8VEC.

  Discussion:

    An R8VEC is a vector of R8's.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    03 July 2005

  Author:

    John Burkardt

  Parameters:

    Input, int N, the number of entries in the vectors.

    Input, double A1[N], the vector to be copied.

    Input, double A2[N], the copy of A1.
*/
{
  int i;

  for ( i = 0; i < n; i++ )
  {
    a2[i] = a1[i];
  }
  return;
}
/******************************************************************************/

void r8vec_direct_product ( int factor_index, int factor_order,
  double factor_value[], int factor_num, int point_num, double x[] )

/******************************************************************************/
/*
  Purpose:

    R8VEC_DIRECT_PRODUCT creates a direct product of R8VEC's.

  Discussion:

    An R8VEC is a vector of R8's.

    To explain what is going on here, suppose we had to construct
    a multidimensional quadrature rule as the product of K rules
    for 1D quadrature.

    The product rule will be represented as a list of points and weights.

    The J-th item in the product rule will be associated with
      item J1 of 1D rule 1,
      item J2 of 1D rule 2,
      ...,
      item JK of 1D rule K.

    In particular,
      X(J) = ( X(1,J1), X(2,J2), ..., X(K,JK))
    and
      W(J) = W(1,J1) * W(2,J2) * ... * W(K,JK)

    So we can construct the quadrature rule if we can properly
    distribute the information in the 1D quadrature rules.

    This routine carries out that task.

    Another way to do this would be to compute, one by one, the
    set of all possible indices (J1,J2,...,JK), and then index
    the appropriate information.  An advantage of the method shown
    here is that you can process the K-th set of information and
    then discard it.

  Example:

    Rule 1:
      Order = 4
      X(1:4) = ( 1, 2, 3, 4 )

    Rule 2:
      Order = 3
      X(1:3) = ( 10, 20, 30 )

    Rule 3:
      Order = 2
      X(1:2) = ( 100, 200 )

    Product Rule:
      Order = 24
      X(1:24) =
        ( 1, 10, 100 )
        ( 2, 10, 100 )
        ( 3, 10, 100 )
        ( 4, 10, 100 )
        ( 1, 20, 100 )
        ( 2, 20, 100 )
        ( 3, 20, 100 )
        ( 4, 20, 100 )
        ( 1, 30, 100 )
        ( 2, 30, 100 )
        ( 3, 30, 100 )
        ( 4, 30, 100 )
        ( 1, 10, 200 )
        ( 2, 10, 200 )
        ( 3, 10, 200 )
        ( 4, 10, 200 )
        ( 1, 20, 200 )
        ( 2, 20, 200 )
        ( 3, 20, 200 )
        ( 4, 20, 200 )
        ( 1, 30, 200 )
        ( 2, 30, 200 )
        ( 3, 30, 200 )
        ( 4, 30, 200 )

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    31 May 2009

  Author:

    John Burkardt

  Parameters:

    Input, int FACTOR_INDEX, the index of the factor being processed.
    The first factor processed must be factor 0.

    Input, int FACTOR_ORDER, the order of the factor.

    Input, double FACTOR_VALUE[FACTOR_ORDER], the factor values
    for factor FACTOR_INDEX.

    Input, int FACTOR_NUM, the number of factors.

    Input, int POINT_NUM, the number of elements in the direct product.

    Input/output, double X[FACTOR_NUM*POINT_NUM], the elements of the
    direct product, which are built up gradually.

  Local Parameters:

    Local, int START, the first location of a block of values to set.

    Local, int CONTIG, the number of consecutive values to set.

    Local, int SKIP, the distance from the current value of START
    to the next location of a block of values to set.

    Local, int REP, the number of blocks of values to set.
*/
{
  static int contig = 0;
  int i;
  int j;
  int k;
  static int rep = 0;
  static int skip = 0;
  int start;

  if ( factor_index == 0 )
  {
    contig = 1;
    skip = 1;
    rep = point_num;
    for ( j = 0; j < point_num; j++ )
    {
      for ( i = 0; i < factor_num; i++ )
      {
        x[i+j*factor_num] = 0.0;
      }
    }
  }

  rep = rep / factor_order;
  skip = skip * factor_order;

  for ( i = 0; i < factor_order; i++ )
  {
    start = 0 + i * contig;

    for ( k = 1; k <= rep; k++ )
    {
      for ( j = start; j < start + contig; j++ )
      {
        x[factor_index+j*factor_num] = factor_value[i];
      }
      start = start + skip;
    }
  }
  contig = contig * factor_order;

  return;
}
/******************************************************************************/

void r8vec_direct_product2 ( int factor_index, int factor_order,
  double factor_value[], int factor_num, int point_num, double w[] )

/******************************************************************************/
/*
  Purpose:

    R8VEC_DIRECT_PRODUCT2 creates a direct product of R8VEC's.

  Discussion:

    An R8VEC is a vector of R8's.

    To explain what is going on here, suppose we had to construct
    a multidimensional quadrature rule as the product of K rules
    for 1D quadrature.

    The product rule will be represented as a list of points and weights.

    The J-th item in the product rule will be associated with
      item J1 of 1D rule 1,
      item J2 of 1D rule 2,
      ...,
      item JK of 1D rule K.

    In particular,
      X(J) = ( X(1,J1), X(2,J2), ..., X(K,JK))
    and
      W(J) = W(1,J1) * W(2,J2) * ... * W(K,JK)

    So we can construct the quadrature rule if we can properly
    distribute the information in the 1D quadrature rules.

    This routine carries out that task for the weights W.

    Another way to do this would be to compute, one by one, the
    set of all possible indices (J1,J2,...,JK), and then index
    the appropriate information.  An advantage of the method shown
    here is that you can process the K-th set of information and
    then discard it.

  Example:

    Rule 1:
      Order = 4
      W(1:4) = ( 2, 3, 5, 7 )

    Rule 2:
      Order = 3
      W(1:3) = ( 11, 13, 17 )

    Rule 3:
      Order = 2
      W(1:2) = ( 19, 23 )

    Product Rule:
      Order = 24
      W(1:24) =
        ( 2 * 11 * 19 )
        ( 3 * 11 * 19 )
        ( 4 * 11 * 19 )
        ( 7 * 11 * 19 )
        ( 2 * 13 * 19 )
        ( 3 * 13 * 19 )
        ( 5 * 13 * 19 )
        ( 7 * 13 * 19 )
        ( 2 * 17 * 19 )
        ( 3 * 17 * 19 )
        ( 5 * 17 * 19 )
        ( 7 * 17 * 19 )
        ( 2 * 11 * 23 )
        ( 3 * 11 * 23 )
        ( 5 * 11 * 23 )
        ( 7 * 11 * 23 )
        ( 2 * 13 * 23 )
        ( 3 * 13 * 23 )
        ( 5 * 13 * 23 )
        ( 7 * 13 * 23 )
        ( 2 * 17 * 23 )
        ( 3 * 17 * 23 )
        ( 5 * 17 * 23 )
        ( 7 * 17 * 23 )

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    31 May 2009

  Author:

    John Burkardt

  Parameters:

    Input, int FACTOR_INDEX, the index of the factor being processed.
    The first factor processed must be factor 0.

    Input, int FACTOR_ORDER, the order of the factor.

    Input, double FACTOR_VALUE[FACTOR_ORDER], the factor values for
    factor FACTOR_INDEX.

    Input, int FACTOR_NUM, the number of factors.

    Input, int POINT_NUM, the number of elements in the direct product.

    Input/output, double W[POINT_NUM], the elements of the
    direct product, which are built up gradually.

  Local Parameters:

    Local, integer START, the first location of a block of values to set.

    Local, integer CONTIG, the number of consecutive values to set.

    Local, integer SKIP, the distance from the current value of START
    to the next location of a block of values to set.

    Local, integer REP, the number of blocks of values to set.
*/
{
  static int contig = 0;
  int i;
  int j;
  int k;
  static int rep = 0;
  static int skip = 0;
  int start;

  if ( factor_index == 0 )
  {
    contig = 1;
    skip = 1;
    rep = point_num;
    for ( i = 0; i < point_num; i++ )
    {
      w[i] = 1.0;
    }
  }

  rep = rep / factor_order;
  skip = skip * factor_order;

  for ( j = 0; j < factor_order; j++ )
  {
    start = 0 + j * contig;

    for ( k = 1; k <= rep; k++ )
    {
      for ( i = start; i < start + contig; i++ )
      {
        w[i] = w[i] * factor_value[j];
      }
      start = start + skip;
    }
  }

  contig = contig * factor_order;

  return;
}
/******************************************************************************/

double r8vec_dot_product ( int n, double a1[], double a2[] )

/******************************************************************************/
/*
  Purpose:

    R8VEC_DOT_PRODUCT computes the dot product of a pair of R8VEC's.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    26 July 2007

  Author:

    John Burkardt

  Parameters:

    Input, int N, the number of entries in the vectors.

    Input, double A1[N], A2[N], the two vectors to be considered.

    Output, double R8VEC_DOT_PRODUCT, the dot product of the vectors.
*/
{
  int i;
  double value;

  value = 0.0;
  for ( i = 0; i < n; i++ )
  {
    value = value + a1[i] * a2[i];
  }
  return value;
}
/******************************************************************************/

double *r8vec_normal_01_new ( int n, int *seed )

/******************************************************************************/
/*
  Purpose:

    R8VEC_NORMAL_01_NEW returns a unit pseudonormal R8VEC.

  Discussion:

    The standard normal probability distribution function (PDF) has
    mean 0 and standard deviation 1.

    This routine can generate a vector of values on one call.  It
    has the feature that it should provide the same results
    in the same order no matter how we break up the task.

    Before calling this routine, the user may call RANDOM_SEED
    in order to set the seed of the random number generator.

    The Box-Muller method is used, which is efficient, but
    generates an even number of values each time.  On any call
    to this routine, an even number of new values are generated.
    Depending on the situation, one value may be left over.
    In that case, it is saved for the next call.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    18 February 2012

  Author:

    John Burkardt

  Parameters:

    Input, int N, the number of values desired.  If N is negative,
    then the code will flush its internal memory; in particular,
    if there is a saved value to be used on the next call, it is
    instead discarded.  This is useful if the user has reset the
    random number seed, for instance.

    Input/output, int *SEED, a seed for the random number generator.

    Output, double R8VEC_NORMAL_01_NEW[N], a sample of the standard normal PDF.

  Local parameters:

    Local, int MADE, records the number of values that have
    been computed.  On input with negative N, this value overwrites
    the return value of N, so the user can get an accounting of
    how much work has been done.

    Local, double R[N+1], is used to store some uniform random values.
    Its dimension is N+1, but really it is only needed to be the
    smallest even number greater than or equal to N.

    Local, int SAVED, is 0 or 1 depending on whether there is a
    single saved value left over from the previous call.

    Local, int X_LO, X_HI, records the range of entries of
    X that we need to compute.  This starts off as 1:N, but is adjusted
    if we have a saved value that can be immediately stored in X(1),
    and so on.

    Local, double Y, the value saved from the previous call, if
    SAVED is 1.
*/
{
# define R8_PI 3.141592653589793

  int i;
  int m;
  static int made = 0;
  double *r;
  static int saved = 0;
  double *x;
  int x_hi;
  int x_lo;
  static double y = 0.0;

  x = ( double * ) malloc ( n * sizeof ( double ) );
/*
  I'd like to allow the user to reset the internal data.
  But this won't work properly if we have a saved value Y.
  I'm making a crock option that allows the user to signal
  explicitly that any internal memory should be flushed,
  by passing in a negative value for N.
*/
  if ( n < 0 )
  {
    made = 0;
    saved = 0;
    y = 0.0;
    return NULL;
  }
  else if ( n == 0 )
  {
    return NULL;
  }
/*
  Record the range of X we need to fill in.
*/
  x_lo = 1;
  x_hi = n;
/*
  Use up the old value, if we have it.
*/
  if ( saved == 1 )
  {
    x[0] = y;
    saved = 0;
    x_lo = 2;
  }
/*
  Maybe we don't need any more values.
*/
  if ( x_hi - x_lo + 1 == 0 )
  {
  }
/*
  If we need just one new value, do that here to avoid null arrays.
*/
  else if ( x_hi - x_lo + 1 == 1 )
  {
    r = r8vec_uniform_01_new ( 2, seed );

    x[x_hi-1] = sqrt ( - 2.0 * log ( r[0] ) ) * cos ( 2.0 * R8_PI * r[1] );
    y =         sqrt ( - 2.0 * log ( r[0] ) ) * sin ( 2.0 * R8_PI * r[1] );

    saved = 1;

    made = made + 2;

    free ( r );
  }
/*
  If we require an even number of values, that's easy.
*/
  else if ( ( x_hi - x_lo + 1 ) % 2 == 0 )
  {
    m = ( x_hi - x_lo + 1 ) / 2;

    r = r8vec_uniform_01_new ( 2*m, seed );

    for ( i = 0; i <= 2*m-2; i = i + 2 )
    {
      x[x_lo+i-1] = sqrt ( - 2.0 * log ( r[i] ) ) * cos ( 2.0 * R8_PI * r[i+1] );
      x[x_lo+i  ] = sqrt ( - 2.0 * log ( r[i] ) ) * sin ( 2.0 * R8_PI * r[i+1] );
    }
    made = made + x_hi - x_lo + 1;

    free ( r );
  }
/*
  If we require an odd number of values, we generate an even number,
  and handle the last pair specially, storing one in X(N), and
  saving the other for later.
*/
  else
  {
    x_hi = x_hi - 1;

    m = ( x_hi - x_lo + 1 ) / 2 + 1;

    r = r8vec_uniform_01_new ( 2*m, seed );

    for ( i = 0; i <= 2*m-4; i = i + 2 )
    {
      x[x_lo+i-1] = sqrt ( - 2.0 * log ( r[i] ) ) * cos ( 2.0 * R8_PI * r[i+1] );
      x[x_lo+i  ] = sqrt ( - 2.0 * log ( r[i] ) ) * sin ( 2.0 * R8_PI * r[i+1] );
    }

    i = 2*m - 2;

    x[x_lo+i-1] = sqrt ( - 2.0 * log ( r[i] ) ) * cos ( 2.0 * R8_PI * r[i+1] );
    y           = sqrt ( - 2.0 * log ( r[i] ) ) * sin ( 2.0 * R8_PI * r[i+1] );

    saved = 1;

    made = made + x_hi - x_lo + 2;

    free ( r );
  }

  return x;
# undef R8_PI
}
/******************************************************************************/

double r8vec_sum ( int n, double a[] )

/******************************************************************************/
/*
  Purpose:

    R8VEC_SUM returns the sum of an R8VEC.

  Discussion:

    An R8VEC is a vector of R8's.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    26 August 2008

  Author:

    John Burkardt

  Parameters:

    Input, int N, the number of entries in the vector.

    Input, double A[N], the vector.

    Output, double R8VEC_SUM, the sum of the vector.
*/
{
  int i;
  double value;

  value = 0.0;
  for ( i = 0; i < n; i++ )
  {
    value = value + a[i];
  }

  return value;
}
/******************************************************************************/

void r8vec_transpose_print ( int n, double a[], char *title )

/******************************************************************************/
/*
  Purpose:

    R8VEC_TRANSPOSE_PRINT prints an R8VEC "transposed".

  Discussion:

    An R8VEC is a vector of R8's.

  Example:

    A = (/ 1.0, 2.1, 3.2, 4.3, 5.4, 6.5, 7.6, 8.7, 9.8, 10.9, 11.0 /)
    TITLE = 'My vector:  '

    My vector:

        1.0    2.1    3.2    4.3    5.4
        6.5    7.6    8.7    9.8   10.9
       11.0

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    12 November 2010

  Author:

    John Burkardt

  Parameters:

    Input, int N, the number of components of the vector.

    Input, double A[N], the vector to be printed.

    Input, char *TITLE, a title.
*/
{
  int i;
  int ihi;
  int ilo;

  printf ( "\n" );
  printf ( "%s\n", title );
  printf ( "\n" );

  if ( n <= 0 )
  {
    printf ( "  (Empty)\n" );
    return;
  }

  for ( ilo = 0; ilo < n; ilo = ilo + 5 )
  {
    ihi = i4_min ( ilo + 5, n );
    for ( i = ilo; i < ihi; i++ )
    {
      printf ( "  %12f", a[i] );
    }
    printf ( "\n" );
  }

  return;
}
/******************************************************************************/

double *r8vec_uniform_01_new ( int n, int *seed )

/******************************************************************************/
/*
  Purpose:

    R8VEC_UNIFORM_01_NEW returns a unit pseudorandom R8VEC.

  Discussion:

    This routine implements the recursion

      seed = 16807 * seed mod ( 2^31 - 1 )
      unif = seed / ( 2^31 - 1 )

    The integer arithmetic never requires more than 32 bits,
    including a sign bit.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    19 August 2004

  Author:

    John Burkardt

  Reference:

    Paul Bratley, Bennett Fox, Linus Schrage,
    A Guide to Simulation,
    Second Edition,
    Springer, 1987,
    ISBN: 0387964673,
    LC: QA76.9.C65.B73.

    Bennett Fox,
    Algorithm 647:
    Implementation and Relative Efficiency of Quasirandom
    Sequence Generators,
    ACM Transactions on Mathematical Software,
    Volume 12, Number 4, December 1986, pages 362-376.

    Pierre L'Ecuyer,
    Random Number Generation,
    in Handbook of Simulation,
    edited by Jerry Banks,
    Wiley, 1998,
    ISBN: 0471134031,
    LC: T57.62.H37.

    Peter Lewis, Allen Goodman, James Miller,
    A Pseudo-Random Number Generator for the System/360,
    IBM Systems Journal,
    Volume 8, Number 2, 1969, pages 136-143.

  Parameters:

    Input, int N, the number of entries in the vector.

    Input/output, int *SEED, a seed for the random number generator.

    Output, double R8VEC_UNIFORM_01_NEW[N], the vector of pseudorandom values.
*/
{
  int i;
  int i4_huge = 2147483647;
  int k;
  double *r;

  if ( *seed == 0 )
  {
    fprintf ( stderr, "\n" );
    fprintf ( stderr, "R8VEC_UNIFORM_01_NEW - Fatal error!\n" );
    fprintf ( stderr, "  Input value of SEED = 0.\n" );
    exit ( 1 );
  }

  r = ( double * ) malloc ( n * sizeof ( double ) );

  for ( i = 0; i < n; i++ )
  {
    k = *seed / 127773;

    *seed = 16807 * ( *seed - k * 127773 ) - k * 2836;

    if ( *seed < 0 )
    {
      *seed = *seed + i4_huge;
    }

    r[i] = ( double ) ( *seed ) * 4.656612875E-10;
  }

  return r;
}
/******************************************************************************/


void sort_heap_external ( int n, int *indx, int *i, int *j, int isgn )

/******************************************************************************/
/*
  Purpose:

    SORT_HEAP_EXTERNAL externally sorts a list of items into ascending order.

  Discussion:

    The actual list is not passed to the routine.  Hence it may
    consist of integers, reals, numbers, names, etc.  The user,
    after each return from the routine, will be asked to compare or
    interchange two items.

    The current version of this code mimics the FORTRAN version,
    so the values of I and J, in particular, are FORTRAN indices.

  Licensing:

    This code is distributed under the GNU LGPL license. 

  Modified:

    05 February 2004

  Author:

    Original FORTRAN77 version by Albert Nijenhuis, Herbert Wilf.
    C version by John Burkardt.

  Reference:

    Albert Nijenhuis, Herbert Wilf,
    Combinatorial Algorithms,
    Academic Press, 1978, second edition,
    ISBN 0-12-519260-6.

  Parameters:

    Input, int N, the length of the input list.

    Input/output, int *INDX.
    The user must set INDX to 0 before the first call.
    On return,
      if INDX is greater than 0, the user must interchange
      items I and J and recall the routine.
      If INDX is less than 0, the user is to compare items I
      and J and return in ISGN a negative value if I is to
      precede J, and a positive value otherwise.
      If INDX is 0, the sorting is done.

    Output, int *I, *J.  On return with INDX positive,
    elements I and J of the user's list should be
    interchanged.  On return with INDX negative, elements I
    and J are to be compared by the user.

    Input, int ISGN. On return with INDX negative, the
    user should compare elements I and J of the list.  If
    item I is to precede item J, set ISGN negative,
    otherwise set ISGN positive.
*/
{
  static int i_save = 0;
  static int j_save = 0;
  static int k = 0;
  static int k1 = 0;
  static int n1 = 0;
/*
  INDX = 0: This is the first call.
*/
  if ( *indx == 0 )
  {

    i_save = 0;
    j_save = 0;
    k = n / 2;
    k1 = k;
    n1 = n;
  }
/*
  INDX < 0: The user is returning the results of a comparison.
*/
  else if ( *indx < 0 )
  {
    if ( *indx == -2 ) 
    {
      if ( isgn < 0 ) 
      {
        i_save = i_save + 1;
      }
      j_save = k1;
      k1 = i_save;
      *indx = -1;
      *i = i_save;
      *j = j_save;
      return;
    }

    if ( 0 < isgn ) 
    {
      *indx = 2;
      *i = i_save;
      *j = j_save;
      return;
    }

    if ( k <= 1 ) 
    {
      if ( n1 == 1 ) 
      {
        i_save = 0;
        j_save = 0;
        *indx = 0;
      }
      else 
      {
        i_save = n1;
        j_save = 1;
        n1 = n1 - 1;
        *indx = 1;
      }
      *i = i_save;
      *j = j_save;
      return;
    }
    k = k - 1;
    k1 = k;
  }
/*
  0 < INDX: the user was asked to make an interchange.
*/
  else if ( *indx == 1 ) 
  {
    k1 = k;
  }

  for ( ; ; )
  {

    i_save = 2 * k1;

    if ( i_save == n1 ) 
    {
      j_save = k1;
      k1 = i_save;
      *indx = -1;
      *i = i_save;
      *j = j_save;
      return;
    }
    else if ( i_save <= n1 ) 
    {
      j_save = i_save + 1;
      *indx = -2;
      *i = i_save;
      *j = j_save;
      return;
    }

    if ( k <= 1 ) 
    {
      break;
    }

    k = k - 1;
    k1 = k;
  }

  if ( n1 == 1 ) 
  {
    i_save = 0;
    j_save = 0;
    *indx = 0;
    *i = i_save;
    *j = j_save;
  }
  else 
  {
    i_save = n1;
    j_save = 1;
    n1 = n1 - 1;
    *indx = 1;
    *i = i_save;
    *j = j_save;
  }

  return;
}
/******************************************************************************/

int symmetric_sparse_size ( int nr, int dim, double nodes[], double x0 )

/******************************************************************************/
/*
  Purpose:

    SYMMETRIC_SPARSE_SIZE sizes a symmetric sparse rule.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    08 December 2012

  Author:

    John Burkardt.

  Reference:

    Florian Heiss, Viktor Winschel,
    Likelihood approximation by numerical integration on sparse grids,
    Journal of Econometrics,
    Volume 144, 2008, pages 62-80.

  Parameters:
 
    Input, int DIM, the dimension.

    Input, int NR, the dimension of the rule in the 
    positive orthant.

    Input, double NODES[NR*DIM], the nodes for the positive orthant.

    Input, double X0, the point of symmetry for the 1D rule, 
    typically 0.

    Output, int SYMMETRIC_SPARSE_SIZE, the dimension of the rule 
    when "unfolded" to the full space.
*/
{
  int count;
  int j;
  int nr2;
  int r;
/*
  Count the size of the full rule.
*/
  nr2 = 0;

  for ( r = 0; r < nr; r++ )
  {
    count = 1;
    for ( j = 0; j < dim; j++ )
    {
      if ( nodes[r+j*nr] != x0 )
      {
        count = 2 * count;
      }
    }
    nr2 = nr2 + count;
  }

  return nr2;
}
/******************************************************************************/

void tensor_product ( int d, int order1d[], int n1d, double x1d[], 
  double w1d[], int n, double xnd[], double wnd[] )

/******************************************************************************/
/*
  Purpose:

    TENSOR_PRODUCT generates a tensor product quadrature rule.

  Discussion:

    The Kronecker product of an K by L matrix A and an M by N matrix B
    is the K*M by L*N matrix formed by

      a(1,1) * B,  a(1,2) * B,  ..., a(1,l) * B
      a(2,1) * B,  a(2,2) * B,  ..., a(2,l) * B
      ..........   ..........   .... ..........
      a(k,1) * B,  a(k,2) * B,  ..., a(k,l) * B

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    09 December 2012

  Author:

    Original MATLAB version by Florian Heiss, Viktor Winschel.
    C version by John Burkardt.

  Reference:

    Florian Heiss, Viktor Winschel,
    Likelihood approximation by numerical integration on sparse grids,
    Journal of Econometrics,
    Volume 144, 2008, pages 62-80.

  Parameters:

    Input, int D, the spatial dimension.

    Input, int ORDER1D[D], the order of each 1D rule.

    Input, int N1D, the number of 1D items.

    Input, double X1D[N1D], the 1D nodes.

    Input, double W1D[N1D], the 1D weights.

    Input, int N, the number of N-dimensional items.

    Output, double XND[D*N], the nodes.

    Output, double WND[N], the weights.
*/
{
  int i;
  int i1;
  int i2;
/*
  Compute the weights.
*/
  i2 = -1;
  for ( i = 0; i < d; i++ )
  {
    i1 = i2 + 1;
    i2 = i2 + order1d[i];
    r8vec_direct_product2 ( i, order1d[i], w1d+i1, d, n, wnd );
  }
/*
  Compute the points.
*/
  i2 = -1;
  for ( i = 0; i < d; i++ )
  {
    i1 = i2 + 1;
    i2 = i2 + order1d[i];
    r8vec_direct_product ( i, order1d[i], x1d+i1, d, n, xnd );
  }

  return;
}
/******************************************************************************/

void tensor_product_cell ( int nc, double xc[], double wc[], int dim, int nr[], 
  int roff[], int np, double xp[], double wp[] )

/******************************************************************************/
/*
  Purpose:

    TENSOR_PRODUCT_CELL generates a tensor product quadrature rule.

  Discussion:

    The Kronecker product of an K by L matrix A and an M by N matrix B
    is the K*M by L*N matrix formed by

      a(1,1) * B,  a(1,2) * B,  ..., a(1,l) * B
      a(2,1) * B,  a(2,2) * B,  ..., a(2,l) * B
      ..........   ..........   .... ..........
      a(k,1) * B,  a(k,2) * B,  ..., a(k,l) * B

    The 1D factors are stored in a kind of cell array structure.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    10 December 2012

  Author:

    John Burkardt.

  Parameters:

    Input, int NC, the number of items in the cell arrays.

    Input, double XC[NC], a cell array containing points for 
    1D rules.

    Input, double WC[NC], a cell array containing weights for
    1D rules.

    Input, int DIM, the spatial dimension.

    Input, int NR[DIM], the length of each row of the 
    cell array.

    Input, int ROFF[DIM+1], offsets for the cell arrays.

    Input, int NP, the number of points in the product rule.

    Output, double XP[DIM*NP], the nodes.

    Output, double WP[NP], the weights.
*/
{
  int i;
  int n1d;
  double *w1d;
  double *x1d;
/*
  Compute the weights.
*/
  for ( i = 0; i < dim; i++ )
  {
    n1d = nr[i];
    w1d = r8cvv_rget_new ( nc, wc, dim, roff, i );
    r8vec_direct_product2 ( i, n1d, w1d, dim, np, wp );
    free ( w1d );
  }
/*
  Compute the points.
*/
  for ( i = 0; i < dim; i++ )
  {
    n1d = nr[i];
    x1d = r8cvv_rget_new ( nc, xc, dim, roff, i );
    r8vec_direct_product ( i, n1d, x1d, dim, np, xp );
    free ( x1d );
  }

  return;
}
/******************************************************************************/

