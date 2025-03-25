# include <math.h>
# include <stdlib.h>
# include <stdio.h>
# include <time.h>

# include "filon_rule.h"

/******************************************************************************/

double filon_fun_cos ( int n, double *f ( int n, double x[] ), double a,
  double b, double t )

/******************************************************************************/
/*
  Purpose:

    filon_fun_cos() uses Filon's method on integrals with a cosine factor.

  Discussion:

    The integral to be approximated has the form:

      Integral ( A <= X <= B ) F(X) * COS(T*X) dX

    where T is user specified.

    The function is interpolated over each subinterval by
    a parabolic arc.

  Licensing:

    This code is distributed under the MIT license.

  Modified:

    20 May 2014

  Author:

    John Burkardt

  Reference:

    Milton Abramowitz, Irene Stegun,
    Handbook of Mathematical Functions,
    National Bureau of Standards, 1964,
    ISBN: 0-486-61272-4,
    LC: QA47.A34.

    Stephen Chase, Lloyd Fosdick,
    An Algorithm for Filon Quadrature,
    Communications of the Association for Computing Machinery,
    Volume 12, Number 8, August 1969, pages 453-457.

    Stephen Chase, Lloyd Fosdick,
    Algorithm 353:
    Filon Quadrature,
    Communications of the Association for Computing Machinery,
    Volume 12, Number 8, August 1969, pages 457-458.

    Philip Davis, Philip Rabinowitz,
    Methods of Numerical Integration,
    Second Edition,
    Dover, 2007,
    ISBN: 0486453391,
    LC: QA299.3.D28.

  Parameters:

    Input, int N, the number of data points.
    N must be odd, and greater than 1.

    Input, double *F ( int n, double x[] ), the function which evaluates the
    integrand.

    Input, double A, B, the limits of integration.

    Input, double T, the multiplier of the X argument of the cosine.

    Output, double FILON_FUN_COS, the approximate value of the integral.
*/
{
  double alpha;
  double beta;
  double c2n;
  double c2nm1;
  double cost;
  double *ftab;
  double gamma;
  double h;
  int i;
  double sint;
  double theta;
  double value;
  double *x;

  if ( a == b )
  {
    value = 0.0;
    return value;
  }

  if ( n <= 1 )
  {
/*
    fprintf ( stderr, "\n" );
    fprintf ( stderr, "FILON_FUN_COS - Fatal error!\n" );
    fprintf ( stderr, "  N < 2\n" );
    fprintf ( stderr, "  N = %d\n", n );
    exit ( 1 );
*/
	n=3;
  }

  if ( ( n % 2 ) != 1 )
  {
/*
    fprintf ( stderr, "\n" );
    fprintf ( stderr, "FILON_FUN_COS - Fatal error!\n" );
    fprintf ( stderr, "  N must be odd.\n" );
    fprintf ( stderr, "  N = %d\n", n );
    exit ( 1 );
*/
	n++;
  }
/*
  Set the X values.
*/
  x = ( double * ) malloc ( n * sizeof ( double ) );
  for ( i = 0; i < n; i++ )
  {
    x[i] = ( ( double ) ( n - i - 1 ) * a
           + ( double ) (     i     ) * b )
           / ( double ) ( n     - 1 );
  }

  h = ( b - a ) / ( double ) ( n - 1 );
  theta = t * h;
  sint = sin ( theta );
  cost = cos ( theta );

  if ( 6.0 * fabs ( theta ) <= 1.0 )
  {
    alpha = 2.0 * pow ( theta, 3 ) /   45.0
          - 2.0 * pow ( theta, 5 ) /  315.0
          + 2.0 * pow ( theta, 7 ) / 4725.0;

    beta =  2.0                    /     3.0
          + 2.0 * pow ( theta, 2 ) /    15.0
          - 4.0 * pow ( theta, 4 ) /   105.0
          + 2.0 * pow ( theta, 6 ) /   567.0
          - 4.0 * pow ( theta, 8 ) / 22275.0;

    gamma = 4.0                    /      3.0
          - 2.0 * pow ( theta, 2 ) /     15.0
          +       pow ( theta, 4 ) /    210.0
          -       pow ( theta, 6 ) /  11340.0;
  }
  else
  {
    alpha = ( pow ( theta, 2 ) + theta * sint * cost - 2.0 * sint * sint )
      / pow ( theta, 3 );

    beta = ( 2.0 * theta + 2.0 * theta * cost * cost
      - 4.0 * sint * cost ) / pow ( theta, 3 );

    gamma = 4.0 * ( sint - theta * cost ) / pow ( theta, 3 );
  }
/*
  Tabulate the function.
*/
  ftab = f ( n, x );

  c2n = 0.5 * ftab[0] * cos ( t * x[0] );
  for ( i = 2; i < n - 1; i = i + 2 )
  {
    c2n = c2n + ftab[i] * cos ( t * x[i] );
  }
  c2n = c2n + 0.5 * ftab[n-1] * cos ( t * x[n-1] );

  c2nm1 = 0.0;
  for ( i = 1; i <= n - 2; i = i + 2 )
  {
    c2nm1 = c2nm1 + ftab[i] * cos ( t * x[i] );
  }

  value = h * (
      alpha * ( ftab[n-1] * sin ( t * x[n-1] )
              - ftab[0]   * sin ( t * x[0] ) )
    + beta * c2n
    + gamma * c2nm1 );

  free ( ftab );
  free ( x );

  return value;
}
/******************************************************************************/

double filon_tab_cos ( int n, double ftab[], double a, double b, double t )

/******************************************************************************/
/*
  Purpose:

    FILON_TAB_COS uses Filon's method on integrals with a cosine factor.

  Discussion:

    The integral to be approximated has the form:

      Integral ( A <= X <= B ) F(X) * COS(T*X) dX

    where T is user specified.

    The function is interpolated over each subinterval by
    a parabolic arc.

  Licensing:

    This code is distributed under the MIT license.

  Modified:

    20 May 2014

  Author:

    John Burkardt

  Reference:

    Milton Abramowitz, Irene Stegun,
    Handbook of Mathematical Functions,
    National Bureau of Standards, 1964,
    ISBN: 0-486-61272-4,
    LC: QA47.A34.

    Stephen Chase, Lloyd Fosdick,
    An Algorithm for Filon Quadrature,
    Communications of the Association for Computing Machinery,
    Volume 12, Number 8, August 1969, pages 453-457.

    Stephen Chase, Lloyd Fosdick,
    Algorithm 353:
    Filon Quadrature,
    Communications of the Association for Computing Machinery,
    Volume 12, Number 8, August 1969, pages 457-458.

    Philip Davis, Philip Rabinowitz,
    Methods of Numerical Integration,
    Second Edition,
    Dover, 2007,
    ISBN: 0486453391,
    LC: QA299.3.D28.

  Parameters:

    Input, int N, the number of data points.
    N must be odd, and greater than 1.

    Input, double FTAB[N], contains the value of the function
    at A, A+H, A+2*H, ... , B-H, B, where H = (B-A)/(N-1).

    Input, double A, B, the limits of integration.

    Input, double T, the multiplier of the X argument of the cosine.

    Output, double FILON_TAB_COS, the approximate value of the integral.
*/
{
  double alpha;
  double beta;
  double c2n;
  double c2nm1;
  double cost;
  double gamma;
  double h;
  int i;
  double sint;
  double theta;
  double value;
  double *x;

  if ( a == b )
  {
    value = 0.0;
    return value;
  }

  if ( n <= 1 )
  {
/*
    fprintf ( stderr, "\n" );
    fprintf ( stderr, "FILON_FUN_COS - Fatal error!\n" );
    fprintf ( stderr, "  N < 2\n" );
    fprintf ( stderr, "  N = %d\n", n );
    exit ( 1 );
*/
	n=3;
  }

  if ( ( n % 2 ) != 1 )
  {
/*
    fprintf ( stderr, "\n" );
    fprintf ( stderr, "FILON_FUN_COS - Fatal error!\n" );
    fprintf ( stderr, "  N must be odd.\n" );
    fprintf ( stderr, "  N = %d\n", n );
    exit ( 1 );
*/
	n++;
  }
/*
  Set the X values.
*/
  x = ( double * ) malloc ( n * sizeof ( double ) );
  for ( i = 0; i < n; i++ )
  {
    x[i] = ( ( double ) ( n - i - 1 ) * a
           + ( double ) (     i     ) * b )
           / ( double ) ( n     - 1 );
  }

  h = ( b - a ) / ( double ) ( n - 1 );
  theta = t * h;
  sint = sin ( theta );
  cost = cos ( theta );

  if ( 6.0 * fabs ( theta ) <= 1.0 )
  {
    alpha = 2.0 * pow ( theta, 3 ) /   45.0
          - 2.0 * pow ( theta, 5 ) /  315.0
          + 2.0 * pow ( theta, 7 ) / 4725.0;

    beta =  2.0                    /     3.0
          + 2.0 * pow ( theta, 2 ) /    15.0
          - 4.0 * pow ( theta, 4 ) /   105.0
          + 2.0 * pow ( theta, 6 ) /   567.0
          - 4.0 * pow ( theta, 8 ) / 22275.0;

    gamma = 4.0                    /      3.0
          - 2.0 * pow ( theta, 2 ) /     15.0
          +       pow ( theta, 4 ) /    210.0
          -       pow ( theta, 6 ) /  11340.0;
  }
  else
  {
    alpha = ( pow ( theta, 2 ) + theta * sint * cost - 2.0 * sint * sint )
      / pow ( theta, 3 );

    beta = ( 2.0 * theta + 2.0 * theta * cost * cost
      - 4.0 * sint * cost ) / pow ( theta, 3 );

    gamma = 4.0 * ( sint - theta * cost ) / pow ( theta, 3 );
  }

  c2n = + 0.5 * ftab[0] * cos ( t * x[0] );
  for ( i = 2; i < n - 1; i = i + 2 )
  {
    c2n = c2n + ftab[i] * cos ( t * x[i] );
  }
  c2n = c2n + 0.5 * ftab[n-1] * cos ( t * x[n-1] );

  c2nm1 = 0.0;
  for ( i = 1; i <= n - 2; i = i + 2 )
  {
    c2nm1 = c2nm1 + ftab[i] * cos ( t * x[i] );
  }

  value = h * (
      alpha * ( ftab[n-1] * sin ( t * x[n-1] )
              - ftab[0]   * sin ( t * x[0] ) )
    + beta * c2n
    + gamma * c2nm1 );

  free ( x );

  return value;
}
/******************************************************************************/

double filon_fun_sin ( int n, double *f ( int n, double x[] ), double a,
  double b, double t )

/******************************************************************************/
/*
  Purpose:

    FILON_FUN_SIN uses Filon's method on integrals with a sine factor.

  Discussion:

    The integral to be approximated has the form

      Integral ( A <= X <= B ) F(X) * SIN(T*X) dX

    where T is user specified.

    The function is interpolated over each subinterval by
    a parabolic arc.

  Licensing:

    This code is distributed under the MIT license.

  Modified:

    20 May 2014

  Author:

    John Burkardt

  Reference:

    Milton Abramowitz, Irene Stegun,
    Handbook of Mathematical Functions,
    National Bureau of Standards, 1964,
    ISBN: 0-486-61272-4,
    LC: QA47.A34.

    Stephen Chase, Lloyd Fosdick,
    An Algorithm for Filon Quadrature,
    Communications of the Association for Computing Machinery,
    Volume 12, Number 8, August 1969, pages 453-457.

    Stephen Chase, Lloyd Fosdick,
    Algorithm 353:
    Filon Quadrature,
    Communications of the Association for Computing Machinery,
    Volume 12, Number 8, August 1969, pages 457-458.

    Philip Davis, Philip Rabinowitz,
    Methods of Numerical Integration,
    Second Edition,
    Dover, 2007,
    ISBN: 0486453391,
    LC: QA299.3.D28.

  Parameters:

    Input, int N, the number of data points,
    including the endpoints.  N must be odd, and greater than 1.

    Input, external F, the subroutine which evaluates the integrand,
    of the form subroutine F ( N, X, FX ).

    Input, double A, B, the limits of integration.

    Input, double T, multiplier of the X argument of the sine.

    Output, double FILON_FUN_SIN, the approximate value of the integral.
*/
{
  double alpha;
  double beta;
  double cost;
  double *ftab;
  double gamma;
  double h;
  int i;
  double s2n;
  double s2nm1;
  double sint;
  double theta;
  double value;
  double *x;

  if ( a == b )
  {
    value = 0.0;
    return value;
  }

  if ( n <= 1 )
  {
/*
    fprintf ( stderr, "\n" );
    fprintf ( stderr, "FILON_FUN_COS - Fatal error!\n" );
    fprintf ( stderr, "  N < 2\n" );
    fprintf ( stderr, "  N = %d\n", n );
    exit ( 1 );
*/
	n=3;
  }

  if ( ( n % 2 ) != 1 )
  {
/*
    fprintf ( stderr, "\n" );
    fprintf ( stderr, "FILON_FUN_COS - Fatal error!\n" );
    fprintf ( stderr, "  N must be odd.\n" );
    fprintf ( stderr, "  N = %d\n", n );
    exit ( 1 );
*/
	n++;
  }
/*
  Set the X values.
*/
  x = ( double * ) malloc ( n * sizeof ( double ) );
  for ( i = 0; i < n; i++ )
  {
    x[i] = ( ( double ) ( n - i - 1 ) * a
           + ( double ) (     i     ) * b )
           / ( double ) ( n     - 1 );
  }

  h = ( b - a ) / ( double ) ( n - 1 );
  theta = t * h;
  sint = sin ( theta );
  cost = cos ( theta );

  if ( 6.0 * fabs ( theta ) <= 1.0 )
  {
    alpha = 2.0 * pow ( theta, 3 ) /   45.0
          - 2.0 * pow ( theta, 5 ) /  315.0
          + 2.0 * pow ( theta, 7 ) / 4725.0;

    beta =  2.0                    /     3.0
          + 2.0 * pow ( theta, 2 ) /    15.0
          - 4.0 * pow ( theta, 4 ) /   105.0
          + 2.0 * pow ( theta, 6 ) /   567.0
          - 4.0 * pow ( theta, 8 ) / 22275.0;

    gamma = 4.0                    /      3.0
          - 2.0 * pow ( theta, 2 ) /     15.0
          +       pow ( theta, 4 ) /    210.0
          -       pow ( theta, 6 ) /  11340.0;
  }
  else
  {
    alpha = ( pow ( theta, 2 ) + theta * sint * cost
      - 2.0 * sint * sint ) / pow ( theta, 3 );

    beta = ( 2.0 * theta + 2.0 * theta * cost * cost
      - 4.0 * sint * cost ) / pow ( theta, 3 );

    gamma = 4.0 * ( sint - theta * cost ) / pow ( theta, 3 );
  }
/*
  Tabulate the function.
*/
  ftab = f ( n, x );

  s2n = + 0.5 * ftab[0] * sin ( t * x[0] );
  for ( i = 2; i < n - 1; i = i + 2 )
  {
    s2n = s2n + ftab[i] * sin ( t * x[i] );
  }
  s2n = s2n + 0.5 * ftab[n-1] * sin ( t * x[n-1] );

  s2nm1 = 0.0;
  for ( i = 1; i <= n - 2; i = i + 2 )
  {
    s2nm1 = s2nm1 + ftab[i] * sin ( t * x[i] );
  }

  value = h * (
      alpha * ( ftab[0]   * cos ( t * x[0] )
              - ftab[n-1] * cos ( t * x[n-1] ) )
    + beta * s2n
    + gamma * s2nm1 );

  free ( ftab );
  free ( x );

  return value;
}
/******************************************************************************/

double filon_tab_sin ( int n, double ftab[], double a, double b, double t )

/******************************************************************************/
/*
  Purpose:

    FILON_TAB_SIN uses Filon's method on integrals with a sine factor.

  Discussion:

    The integral to be approximated has the form

      Integral ( A <= X <= B ) F(X) * SIN(T*X) dX

    where T is user specified.

    The function is interpolated over each subinterval by
    a parabolic arc.

  Licensing:

    This code is distributed under the MIT license.

  Modified:

    20 May 2014

  Author:

    John Burkardt

  Reference:

    Milton Abramowitz, Irene Stegun,
    Handbook of Mathematical Functions,
    National Bureau of Standards, 1964,
    ISBN: 0-486-61272-4,
    LC: QA47.A34.

    Stephen Chase, Lloyd Fosdick,
    An Algorithm for Filon Quadrature,
    Communications of the Association for Computing Machinery,
    Volume 12, Number 8, August 1969, pages 453-457.

    Stephen Chase, Lloyd Fosdick,
    Algorithm 353:
    Filon Quadrature,
    Communications of the Association for Computing Machinery,
    Volume 12, Number 8, August 1969, pages 457-458.

    Philip Davis, Philip Rabinowitz,
    Methods of Numerical Integration,
    Second Edition,
    Dover, 2007,
    ISBN: 0486453391,
    LC: QA299.3.D28.

  Parameters:

    Input, int N, the number of data points,
    including the endpoints.  N must be odd, and greater than 1.

    Input, double FTAB[N], contains the value of the function
    at A, A+H, A+2*H, ... , B-H, B, where H = (B-A)/(N-1).

    Input, double A, B, the limits of integration.

    Input, double T, multiplier of the X argument of the sine.

    Output, double FILON_TAB_SIN, the approximate value of the integral.
*/
{
  double alpha;
  double beta;
  double cost;
  double gamma;
  double h;
  int i;
  double s2n;
  double s2nm1;
  double sint;
  double theta;
  double value;
  double *x;

  if ( a == b )
  {
    value = 0.0;
    return value;
  }

  if ( n <= 1 )
  {
/*
    fprintf ( stderr, "\n" );
    fprintf ( stderr, "FILON_FUN_COS - Fatal error!\n" );
    fprintf ( stderr, "  N < 2\n" );
    fprintf ( stderr, "  N = %d\n", n );
    exit ( 1 );
*/
	n=3;
  }

  if ( ( n % 2 ) != 1 )
  {
/*
    fprintf ( stderr, "\n" );
    fprintf ( stderr, "FILON_FUN_COS - Fatal error!\n" );
    fprintf ( stderr, "  N must be odd.\n" );
    fprintf ( stderr, "  N = %d\n", n );
    exit ( 1 );
*/
	n++;
  }
/*
  Set the X values.
*/
  x = ( double * ) malloc ( n * sizeof ( double ) );
  for ( i = 0; i < n; i++ )
  {
    x[i] = ( ( double ) ( n - i - 1 ) * a
           + ( double ) (     i     ) * b )
           / ( double ) ( n     - 1 );
  }

  h = ( b - a ) / ( double ) ( n - 1 );
  theta = t * h;
  sint = sin ( theta );
  cost = cos ( theta );

  if ( 6.0 * fabs ( theta ) <= 1.0 )
  {
    alpha = 2.0 * pow ( theta, 3 ) /   45.0
          - 2.0 * pow ( theta, 5 ) /  315.0
          + 2.0 * pow ( theta, 7 ) / 4725.0;

    beta =  2.0                    /     3.0
          + 2.0 * pow ( theta, 2 ) /    15.0
          - 4.0 * pow ( theta, 4 ) /   105.0
          + 2.0 * pow ( theta, 6 ) /   567.0
          - 4.0 * pow ( theta, 8 ) / 22275.0;

    gamma = 4.0                    /      3.0
          - 2.0 * pow ( theta, 2 ) /     15.0
          +       pow ( theta, 4 ) /    210.0
          -       pow ( theta, 6 ) /  11340.0;
  }
  else
  {
    alpha = ( pow ( theta, 2 ) + theta * sint * cost
      - 2.0 * sint * sint ) / pow ( theta, 3 );

    beta = ( 2.0 * theta + 2.0 * theta * cost * cost
      - 4.0 * sint * cost ) / pow ( theta, 3 );

    gamma = 4.0 * ( sint - theta * cost ) / pow ( theta, 3 );
  }

  s2n = + 0.5 * ftab[0] * sin ( t * x[0] );
  for ( i = 2; i < n - 1; i = i + 2 )
  {
    s2n = s2n + ftab[i] * sin ( t * x[i] );
  }
  s2n = s2n + 0.5 * ftab[n-1] * sin ( t * x[n-1] );

  s2nm1 = 0.0;
  for ( i = 1; i <= n - 2; i = i + 2 )
  {
    s2nm1 = s2nm1 + ftab[i] * sin ( t * x[i] );
  }

  value = h * (
      alpha * ( ftab[0]   * cos ( t * x[0] )
              - ftab[n-1] * cos ( t * x[n-1] ) )
    + beta * s2n
    + gamma * s2nm1 );

  free ( x );

  return value;
}
/******************************************************************************/
