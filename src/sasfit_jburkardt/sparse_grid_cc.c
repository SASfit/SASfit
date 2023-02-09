# include <float.h>
# include <math.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>

# include "sparse_grid_cc.h"
# include "jburkardt_utils.h"

/******************************************************************************/

int *abscissa_level_closed_nd ( int level_max, int dim_num, int test_num, 
  int test_val[] )

/******************************************************************************/
/*
  Purpose:

    ABSCISSA_LEVEL_CLOSED_ND: first level at which an abscissa is generated.

  Discussion:

    We need this routine because the sparse grid is generated as a sum of 
    product grids, and many points in the sparse grid will belong to several
    of these product grids, and we need to do something special the very 
    first time we encounter such a point - namely, count it.  So this routine 
    determines, for any point in the full product grid, the first level 
    at which that point would be included.


    We assume an underlying product grid.  In each dimension, this product
    grid has order 2^LEVEL_MAX + 1.

    We will say a sparse grid has total level LEVEL if each point in the
    grid has a total level of LEVEL or less.

    The "level" of a point is determined as the sum of the levels of the
    point in each spatial dimension.

    The level of a point in a single spatial dimension I is determined as
    the level, between 0 and LEVEL_MAX, at which the point's I'th index
    would have been generated.


    This description is terse and perhaps unenlightening.  Keep in mind
    that the product grid is the product of 1D grids,
    that the 1D grids are built up by levels, having
    orders (total number of points ) 1, 3, 5, 9, 17, 33 and so on,
    and that these 1D grids are nested, so that each point in a 1D grid
    has a first level at which it appears.

    Our procedure for generating the points of a sparse grid, then, is
    to choose a value LEVEL_MAX, to generate the full product grid,
    but then only to keep those points on the full product grid whose
    LEVEL is less than or equal to LEVEL_MAX.  


    Note that this routine is really just testing out the idea of
    determining the level.  Our true desire is to be able to start
    with a value LEVEL, and determine, in a straightforward manner,
    all the points that are generated exactly at that level, or
    all the points that are generated up to and including that level.

    This allows us to generate the new points to be added to one sparse
    grid to get the next, or to generate a particular sparse grid at once.

  Licensing:

    This code is distributed under the GNU LGPL license. 

  Modified:

    12 March 2013

  Author:

    John Burkardt

  Reference:

    Fabio Nobile, Raul Tempone, Clayton Webster,
    A Sparse Grid Stochastic Collocation Method for Partial Differential
    Equations with Random Input Data,
    SIAM Journal on Numerical Analysis,
    Volume 46, Number 5, 2008, pages 2309-2345.

  Parameters:

    Input, int LEVEL_MAX, controls the size of the final sparse grid.

    Input, int DIM_NUM, the spatial dimension.

    Input, int TEST_NUM, the number of points to be tested.

    Input, int TEST_VAL[DIM_NUM*TEST_NUM], the indices of the points 
    to be tested.  Normally, each index would be between 0 and 2^LEVEL_MAX.

    Output, int ABSCISSA_LEVEL_ND[TEST_NUM], the value of LEVEL at which the
    point would first be generated, assuming that a standard sequence of
    nested grids is used.
*/
{
  int j;
  int order;
  int *test_level;

  test_level = ( int * ) malloc ( test_num * sizeof ( int ) );

  if ( level_max == 0 )
  {
    for ( j = 0; j < test_num; j++ )
    {
      test_level[j] = 0;
    }
    return test_level;
  }

  order = i4_power ( 2, level_max ) + 1;

  for ( j = 0; j < test_num; j++ )
  {
    test_level[j] = index_to_level_closed ( dim_num, test_val+j*dim_num, 
      order, level_max );
  }

  return test_level;
}
/******************************************************************************/

double cc_abscissa ( int order, int i )

/******************************************************************************/
/*
  Purpose:

    CC_ABSCISSA returns the I-th abscissa of the Clenshaw Curtis rule.

  Discussion:

    Our convention is that the abscissas are numbered from left to
    right.

  Licensing:

    This code is distributed under the GNU LGPL license. 

  Modified:

    12 March 2013

  Author:

    John Burkardt

  Parameters:

    Input, int ORDER, the order of the rule.

    Input, int I, the index of the desired abscissa.  1 <= I <= ORDER.

    Output, double CC_ABSCISSA, the value of the I-th 
    abscissa in the rule of order ORDER.
*/
{
  double pi = 3.141592653589793;
  double value;

  if ( order < 1 )
  {
    fprintf ( stderr, "\n" );
    fprintf ( stderr, "CC_ABSCISSA - Fatal error!\n" );
    fprintf ( stderr, "  Input value of ORDER < 1.\n" );
    fprintf ( stderr, "  Input value of ORDER = %d\n", order );
    exit ( 1 );
  }

  if ( i < 1 || order < i )
  {
    fprintf ( stderr, "\n" );
    fprintf ( stderr, "CC_ABSCISSA - Fatal error!\n" );
    fprintf ( stderr, "  1 <= I <= ORDER is required.\n" );
    fprintf ( stderr, "  I = %d\n", i );
    fprintf ( stderr, "  ORDER = %d\n", order );
    exit ( 1 );
  }

  if ( order == 1 )
  {
    value = 0.0;
    return value;
  }

  value = cos ( ( double ) ( order - i ) * pi 
              / ( double ) ( order - 1 ) );

  if ( 2 * i - 1 == order )
  {
    value = 0.0;
  }

  return value;
}
/******************************************************************************/

double *cc_weights ( int n )

/******************************************************************************/
/*
  Purpose:

    CC_WEIGHTS computes Clenshaw Curtis weights.

  Licensing:

    This code is distributed under the GNU LGPL license. 

  Modified:

    12 March 2013

  Author:

    John Burkardt

  Reference:

    Charles Clenshaw, Alan Curtis,
    A Method for Numerical Integration on an Automatic Computer,
    Numerische Mathematik,
    Volume 2, Number 1, December 1960, pages 197-205.

  Parameters:

    Input, int N, the order of the rule.

    Output, double CC_WEIGHTS[N], the weights of the rule.
*/
{
  double b;
  int i;
  int j;
  double pi = 3.141592653589793;
  double theta;
  double *w;

  w = ( double * ) malloc ( n * sizeof ( double ) );

  if ( n == 1 )
  {
    w[0] = 2.0;
    return w;
  }

  for ( i = 1; i <= n; i++ )
  {
    theta = ( double ) ( i - 1 ) * pi / ( double ) ( n - 1 );

    w[i-1] = 1.0;

    for ( j = 1; j <= ( n - 1 ) / 2; j++ )
    {
      if ( 2 * j == ( n - 1 ) )
      {
        b = 1.0;
      }
      else
      {
        b = 2.0;
      }

      w[i-1] = w[i-1] - b * cos ( 2.0 * ( double ) ( j ) * theta ) 
           / ( double ) ( 4 * j * j - 1 );
    }
  }

  w[0] = w[0] / ( double ) ( n - 1 );
  for ( i = 1; i < n-1; i++ )
  {
    w[i] = 2.0 * w[i] / ( double ) ( n - 1 );
  }
  w[n-1] = w[n-1] / ( double ) ( n - 1 );

  return w;
}
/******************************************************************************/

void comp_next ( int n, int k, int a[], int *more, int *h,  int *t )

/******************************************************************************/
/*
  Purpose:

    COMP_NEXT computes the compositions of the integer N into K parts.

  Discussion:

    A composition of the integer N into K parts is an ordered sequence
    of K nonnegative integers which sum to N.  The compositions (1,2,1)
    and (1,1,2) are considered to be distinct.

    The routine computes one composition on each call until there are no more.
    For instance, one composition of 6 into 3 parts is
    3+2+1, another would be 6+0+0.

    On the first call to this routine, set MORE = FALSE.  The routine
    will compute the first element in the sequence of compositions, and
    return it, as well as setting MORE = TRUE.  If more compositions
    are desired, call again, and again.  Each time, the routine will
    return with a new composition.

    However, when the LAST composition in the sequence is computed 
    and returned, the routine will reset MORE to FALSE, signaling that
    the end of the sequence has been reached.

    This routine originally used a STATICE statement to maintain the
    variables H and T.  I have decided (based on an wasting an
    entire morning trying to track down a problem) that it is safer
    to pass these variables as arguments, even though the user should
    never alter them.  This allows this routine to safely shuffle
    between several ongoing calculations.


    There are 28 compositions of 6 into three parts.  This routine will
    produce those compositions in the following order:

     I         A
     -     ---------
     1     6   0   0
     2     5   1   0
     3     4   2   0
     4     3   3   0
     5     2   4   0
     6     1   5   0
     7     0   6   0
     8     5   0   1
     9     4   1   1
    10     3   2   1
    11     2   3   1
    12     1   4   1
    13     0   5   1
    14     4   0   2
    15     3   1   2
    16     2   2   2
    17     1   3   2
    18     0   4   2
    19     3   0   3
    20     2   1   3
    21     1   2   3
    22     0   3   3
    23     2   0   4
    24     1   1   4
    25     0   2   4
    26     1   0   5
    27     0   1   5
    28     0   0   6

  Licensing:

    This code is distributed under the GNU LGPL license. 

  Modified:

    02 July 2008

  Author:

    Original FORTRAN77 version by Albert Nijenhuis, Herbert Wilf.
    C version by John Burkardt.

  Reference:

    Albert Nijenhuis, Herbert Wilf,
    Combinatorial Algorithms for Computers and Calculators,
    Second Edition,
    Academic Press, 1978,
    ISBN: 0-12-519260-6,
    LC: QA164.N54.

  Parameters:

    Input, int N, the integer whose compositions are desired.

    Input, int K, the number of parts in the composition.

    Input/output, int A[K], the parts of the composition.

    Input/output, int *MORE.
    Set MORE = FALSE on first call.  It will be reset to TRUE on return
    with a new composition.  Each new call returns another composition until
    MORE is set to FALSE when the last composition has been computed
    and returned.

    Input/output, int *H, *T, two internal parameters needed for the
    computation.  The user should allocate space for these in the calling
    program, include them in the calling sequence, but never alter them!
*/
{
  int i;

  if ( !( *more ) )
  {
    *t = n;
    *h = 0;
    a[0] = n;
    for ( i = 1; i < k; i++ )
    {
       a[i] = 0;
    }
  }
  else
  {
    if ( 1 < *t )
    {
      *h = 0;
    }
    *h = *h + 1;
    *t = a[*h-1];
    a[*h-1] = 0;
    a[0] = *t - 1;
    a[*h] = a[*h] + 1;
  }

  *more = ( a[k-1] != n );

  return;
}
/******************************************************************************/

int index_to_level_closed ( int dim_num, int t[], int order, int level_max )

/******************************************************************************/
/*
  Purpose:

    INDEX_TO_LEVEL_CLOSED determines the level of a point given its index.

  Licensing:

    This code is distributed under the GNU LGPL license. 

  Modified:

    12 March 2013

  Author:

    John Burkardt

  Reference:

    Fabio Nobile, Raul Tempone, Clayton Webster,
    A Sparse Grid Stochastic Collocation Method for Partial Differential
    Equations with Random Input Data,
    SIAM Journal on Numerical Analysis,
    Volume 46, Number 5, 2008, pages 2309-2345.

  Parameters:

    Input, int DIM_NUM, the spatial dimension.

    Input, int T[DIM_NUM], the grid indices of a point in a 1D closed rule.
    0 <= T[I] <= ORDER.

    Input, int ORDER, the order of the rule.

    Input, int LEVEL_MAX, the level with respect to which the
    index applies.

    Output, int INDEX_TO_LEVEL_CLOSED, the first level on which
    the point associated with the given index will appear.
*/
{
  int dim;
  int level;
  int s;
  int value;

  value = 0;

  for ( dim = 0; dim < dim_num; dim++ )
  {
    s = t[dim];

    s = i4_modp ( s, order );

    if ( s == 0 )
    {
      level = 0;
    }
    else
    {
      level = level_max;

      while ( ( s % 2 ) == 0 )
      {
        s = s / 2;
        level = level - 1;
      }
    }

    if ( level == 0 )
    {
      level = 1;
    }
    else if ( level == 1 )
    {
      level = 0;
    }
    value = value + level;
  }
  return value;
}
/******************************************************************************/

void level_to_order_closed ( int dim_num, int level[], int order[] )

/******************************************************************************/
/*
  Purpose:

    LEVEL_TO_ORDER_CLOSED converts a level to an order for closed rules.

  Discussion:

    Sparse grids can naturally be nested.  A natural scheme is to use
    a series of one-dimensional rules arranged in a series of "levels"
    whose order roughly doubles with each step.

    The arrangement described here works naturally for the Clenshaw Curtis
    and Newton Cotes closed rules.  

    The idea is that we start with LEVEL = 0, ORDER = 1 indicating the single 
    point at the center, and for all values afterwards, we use the 
    relationship

      ORDER = 2^LEVEL + 1

    The following table shows how the growth will occur:

    Level    Order

    0          1
    1          3 =  2 + 1
    2          5 =  4 + 1
    3          9 =  8 + 1
    4         17 = 16 + 1
    5         33 = 32 + 1

    For the Clenshaw Curtis and Newton Cotes Closed rules, the point growth
    is nested.  If we have ORDER points on a particular LEVEL, the next
    level includes all these old points, plus ORDER-1 new points, formed
    in the gaps between successive pairs of old points.

    Level    Order = New + Old

    0          1   =  1  +  0
    1          3   =  2  +  1
    2          5   =  2  +  3
    3          9   =  4  +  5
    4         17   =  8  +  9
    5         33   = 16  + 17

    In this routine, we assume that a vector of levels is given,
    and the corresponding orders are desired.

  Licensing:

    This code is distributed under the GNU LGPL license. 

  Modified:

    12 March 2013

  Author:

    John Burkardt

  Reference:

    Fabio Nobile, Raul Tempone, Clayton Webster,
    A Sparse Grid Stochastic Collocation Method for Partial Differential
    Equations with Random Input Data,
    SIAM Journal on Numerical Analysis,
    Volume 46, Number 5, 2008, pages 2309-2345.

  Parameters:

    Input, int DIM_NUM, the spatial dimension.

    Input, int LEVEL[DIM_NUM], the nesting level.

    Output, int ORDER[DIM_NUM], the order (number of points) 
    of the rule.
*/
{
  int dim;

  for ( dim = 0; dim < dim_num; dim++ )
  {
    if ( level[dim] < 0 )
    {
      order[dim] = -1;
    }
    else if ( level[dim] == 0 )
    {
      order[dim] = 1;
    }
    else
    {
      order[dim] = i4_power ( 2, level[dim] ) + 1 ;
    }
  }
  return;
}
/******************************************************************************/

double monomial_int01 ( int dim_num, int expon[] )

/******************************************************************************/
/*
  Purpose:

    MONOMIAL_INT01 returns the integral of a monomial over the [0,1] hypercube.

  Discussion:

    This routine evaluates a monomial of the form

      product ( 1 <= dim <= dim_num ) x(dim)^expon(dim)

    where the exponents are nonnegative integers.  Note that
    if the combination 0^0 is encountered, it should be treated
    as 1.

  Licensing:

    This code is distributed under the GNU LGPL license. 

  Modified:

    12 March 2013

  Author:

    John Burkardt

  Parameters:

    Input, int DIM_NUM, the spatial dimension.

    Input, int EXPON[DIM_NUM], the exponents.

    Output, double MONOMIAL_INT01, the value of the integral of the
    monomial.
*/
{
  int dim;
  double value;

  value = 1.0;
  for ( dim = 0; dim < dim_num; dim++ )
  {
    value = value / ( double ) ( expon[dim] + 1 );
  }

  return value;
}
/******************************************************************************/

double monomial_quadrature ( int dim_num, int expon[], int point_num, 
  double weight[], double x[] )

/******************************************************************************/
/*
  Purpose:

    MONOMIAL_QUADRATURE applies a quadrature rule to a monomial.

  Licensing:

    This code is distributed under the GNU LGPL license. 

  Modified:

    12 March 2013

  Author:

    John Burkardt

  Parameters:

    Input, int DIM_NUM, the spatial dimension.

    Input, int EXPON[DIM_NUM], the exponents.

    Input, int POINT_NUM, the number of points in the rule.

    Input, double WEIGHT[POINT_NUM], the quadrature weights.

    Input, double X[DIM_NUM*POINT_NUM], the quadrature points.

    Output, double MONOMIAL_QUADRATURE, the quadrature error.
*/
{
  double exact;
  int point;
  double quad;
  double quad_error;
  double scale;
  double *value;
/*
  Get the exact value of the integral of the unscaled monomial.
*/
  scale = monomial_int01 ( dim_num, expon );
/*
  Evaluate the monomial at the quadrature points.
*/
  value = monomial_value ( dim_num, point_num, x, expon );
/*
  Compute the weighted sum and divide by the exact value.
*/
  quad = 0.0;
  for ( point = 0; point < point_num; point++ )
  {
    quad = quad + weight[point] * value[point];
  }
  quad = quad / scale;
/*
  Error:
*/
  exact = 1.0;
  quad_error = fabs ( quad - exact );

  free ( value );

  return quad_error;
}
/******************************************************************************/

double *monomial_value ( int dim_num, int point_num, double x[], int expon[] )

/******************************************************************************/
/*
  Purpose:

    MONOMIAL_VALUE evaluates a monomial.

  Discussion:

    This routine evaluates a monomial of the form

      product ( 1 <= dim <= dim_num ) x(dim)^expon(dim)

    where the exponents are nonnegative integers.  Note that
    if the combination 0^0 is encountered, it should be treated
    as 1.

  Licensing:

    This code is distributed under the GNU LGPL license. 

  Modified:

    12 March 2013

  Author:

    John Burkardt

  Parameters:

    Input, int DIM_NUM, the spatial dimension.

    Input, int POINT_NUM, the number of points at which the
    monomial is to be evaluated.

    Input, double X[DIM_NUM*POINT_NUM], the point coordinates.

    Input, int EXPON[DIM_NUM], the exponents.

    Output, double MONOMIAL_VALUE[POINT_NUM], the value of the monomial.
*/
{
  int dim;
  int point;
  double *value;

  value = ( double * ) malloc ( point_num * sizeof ( double ) );

  for ( point = 0; point < point_num; point++ )
  {
    value[point] = 1.0;
  }

  for ( dim = 0; dim < dim_num; dim++ )
  {
    if ( 0 != expon[dim] )
    {
      for ( point = 0; point < point_num; point++ )
      {
        value[point] = value[point] * pow ( x[dim+point*dim_num], expon[dim] );
      }
    }
  }

  return value;
}
/******************************************************************************/

int *multigrid_index0 ( int dim_num, int order_1d[], int order_nd )

/******************************************************************************/
/*
  Purpose:

    MULTIGRID_INDEX0 returns an indexed multidimensional grid.

  Discussion:

    For dimension DIM, the second index of INDX may vary from 
    0 to ORDER_1D[DIM]-1.

  Licensing:

    This code is distributed under the GNU LGPL license. 

  Modified:

    12 March 2013

  Author:

    John Burkardt

  Reference:

    Fabio Nobile, Raul Tempone, Clayton Webster,
    A Sparse Grid Stochastic Collocation Method for Partial Differential
    Equations with Random Input Data,
    SIAM Journal on Numerical Analysis,
    Volume 46, Number 5, 2008, pages 2309-2345.

  Parameters:

    Input, int DIM_NUM, the spatial dimension.

    Input, int ORDER_1D[DIM_NUM], the order of the
    rule in each dimension.

    Input, int ORDER_ND, the product of the entries of ORDER_1D.

    Output, int INDX[DIM_NUM*ORDER_ND], the indices of the points in
    the grid.  The second dimension of this array is equal to the
    product of the entries of ORDER_1D.
*/
{
  int *a;
  int dim;
  int more;
  int p;
  int *indx;

  indx = ( int * ) malloc ( dim_num * order_nd * sizeof ( int ) );
  a = ( int * ) malloc ( dim_num * sizeof ( int ) );
  more = 0;
  p = 0;

  for ( ; ; )
  {
    vec_colex_next2 ( dim_num, order_1d, a, &more );

    if ( !more )
    {
      break;
    }

    for ( dim = 0; dim < dim_num; dim++ )
    {
      indx[dim+p*dim_num] = a[dim];
    }
    p = p + 1;
  }

  free ( a );

  return indx;
}
/******************************************************************************/

void multigrid_scale_closed ( int dim_num, int order_nd, int level_max, 
  int level_1d[], int grid_index[] )

/******************************************************************************/
/*
  Purpose:

    MULTIGRID_SCALE_CLOSED renumbers a grid as a subgrid on a higher level.

  Discussion:

    This routine takes a grid associated with a given value of
    LEVEL, and multiplies all the indices by a power of 2, so that
    the indices reflect the position of the same points, but in
    a grid of level LEVEL_MAX.

  Licensing:

    This code is distributed under the GNU LGPL license. 

  Modified:

    12 March 2013

  Author:

    John Burkardt

  Reference:

    Fabio Nobile, Raul Tempone, Clayton Webster,
    A Sparse Grid Stochastic Collocation Method for Partial Differential
    Equations with Random Input Data,
    SIAM Journal on Numerical Analysis,
    Volume 46, Number 5, 2008, pages 2309-2345.

  Parameters:

    Input, int DIM_NUM, the spatial dimension.

    Input, int ORDER_ND, the number of points in the grid.

    Input, int LEVEL_MAX, the maximum value of LEVEL.

    Input, int LEVEL_1D[DIM_NUM], the level in each dimension.

    Input/output, int GRID_INDEX[DIM_NUM*POINT_NUM], the index
    values for each grid point.  On input, these indices are based in
    the level for which the grid was generated; on output, the
    indices are appropriate for the grid as a subgrid of a grid
    of level LEVEL_MAX.
*/
{
  int dim;
  int factor;
  int order;
  int order_max;

  for ( dim = 0; dim < dim_num; dim++ )
  {
    if ( level_1d[dim] == 0 )
    {
      if ( 0 == level_max )
      {
        order_max = 1;
      }
      else
      {
        order_max = i4_power ( 2, level_max ) + 1;
      }
      for ( order = 0; order < order_nd; order++ )
      {
        grid_index[dim+order*dim_num] = ( order_max - 1 ) / 2;
      }
    }
    else
    {
      factor = i4_power ( 2, level_max - level_1d[dim] );
      for ( order = 0; order < order_nd; order++ )
      {
        grid_index[dim+order*dim_num] = grid_index[dim+order*dim_num] * factor;
      }
    }
  }

  return;
}
/******************************************************************************/

double *product_weights_cc ( int dim_num, int order_1d[], int order_nd )

/******************************************************************************/
/*
  Purpose:

    PRODUCT_WEIGHTS_CC computes weights for a Clenshaw Curtis product rule.

  Discussion:

    This routine computes the weights for a quadrature rule which is
    a product of closed rules of varying order.

  Licensing:

    This code is distributed under the GNU LGPL license. 

  Modified:

    12 March 2013

  Author:

    John Burkardt

  Parameters:

    Input, int DIM_NUM, the spatial dimension.

    Input, int ORDER_1D[DIM_NUM], the order of the 1D rules.

    Input, int ORDER_ND, the order of the product rule.

    Output, double PRODUCT_WEIGHTS_CC[DIM_NUM*ORDER_ND], 
    the product rule weights.
*/
{
  int dim;
  int order;
  double *w_1d;
  double *w_nd;

  w_nd = ( double * ) malloc ( order_nd * sizeof ( double ) );

  for ( order = 0; order < order_nd; order++ )
  {
    w_nd[order] = 1.0;
  }

  for ( dim = 0; dim < dim_num; dim++ )
  {
    w_1d = cc_weights ( order_1d[dim] );

    r8vec_direct_product2 ( dim, order_1d[dim], w_1d, dim_num, 
      order_nd, w_nd );

    free ( w_1d ); 
  }

  return w_nd;
}
/******************************************************************************/

void r8mat_write ( char *output_filename, int m, int n, double table[] )

/******************************************************************************/
/*
  Purpose:

    R8MAT_WRITE writes an R8MAT file.

  Discussion:

    An R8MAT is an array of R8's.

  Licensing:

    This code is distributed under the GNU LGPL license. 

  Modified:

    01 June 2009

  Author:

    John Burkardt

  Parameters:

    Input, char *OUTPUT_FILENAME, the output filename.

    Input, int M, the spatial dimension.

    Input, int N, the number of points.

    Input, double TABLE[M*N], the data.
*/
{
  int i;
  int j;
  FILE *output;
/*
  Open the file.
*/
  output = fopen ( output_filename, "wt" );

  if ( !output )
  {
    fprintf ( stderr, "\n" );
    fprintf ( stderr, "R8MAT_WRITE - Fatal error!\n" );
    fprintf ( stderr, "  Could not open the output file.\n" );
    exit ( 1 );
  }
/*
  Write the data.
*/
  for ( j = 0; j < n; j++ )
  {
    for ( i = 0; i < m; i++ )
    {
      fprintf ( output, "  %24.16g", table[i+j*m] );
    }
    fprintf ( output, "\n" );
  }
/*
  Close the file.
*/
  fclose ( output );

  return;
}
/******************************************************************************/

void sparse_grid_cc ( int dim_num, int level_max, int point_num, 
  double grid_weight[], double grid_point[] )

/******************************************************************************/
/*
  Purpose:

    SPARSE_GRID_CC computes a sparse grid of Clenshaw Curtis points.

  Discussion:

    This program computes a quadrature rule and writes it to a file.

    The quadrature rule is associated with a sparse grid derived from
    a Smolyak construction using a closed 1D quadrature rule. 

    The user specifies:
    * the spatial dimension of the quadrature region,
    * the level that defines the Smolyak grid.
    * the closed 1D quadrature rule (Clenshaw-Curtis or Newton-Cotes Closed).

  Licensing:

    This code is distributed under the GNU LGPL license. 

  Modified:

    12 March 2013

  Author:

    John Burkardt

  Reference:

    Fabio Nobile, Raul Tempone, Clayton Webster,
    A Sparse Grid Stochastic Collocation Method for Partial Differential
    Equations with Random Input Data,
    SIAM Journal on Numerical Analysis,
    Volume 46, Number 5, 2008, pages 2309-2345.

  Parameters:

    Input, int DIM_NUM, the spatial dimension.

    Input, int LEVEL_MAX, controls the size of the final sparse grid.

    Input, int POINT_NUM, the number of points in the grid, as determined
    by SPARSE_GRID_CC_SIZE.

    Output, double GRID_WEIGHTS[POINT_NUM], the weights.

    Output, double GRID_POINTS[DIM_NUM*POINT_NUM], the points.
*/
{
  int dim;
  int *grid_index;
  int order_max;
  int point;
/*
  Determine the index vector, relative to the full product grid,
  that identifies the points in the sparse grid.
*/
  grid_index = sparse_grid_cc_index ( dim_num, level_max, point_num );
/*
  Compute the physical coordinates of the abscissas.
*/
  if ( 0 == level_max )
  {
    order_max = 1;
  }
  else
  {
    order_max = i4_power ( 2, level_max ) + 1;
  }

  for ( point = 0; point < point_num; point++ )
  {
    for ( dim = 0; dim < dim_num; dim++ )
    {
      grid_point[dim+point*dim_num] = 
        cc_abscissa ( order_max, grid_index[dim+point*dim_num] + 1 );
    }
  }
/*
  Gather the weights.
*/
  sparse_grid_cc_weights ( dim_num, level_max, point_num, grid_index, 
    grid_weight );

  free ( grid_index );

  return;
}
/******************************************************************************/

int *sparse_grid_cc_index ( int dim_num, int level_max, int point_num )

/******************************************************************************/
/*
  Purpose:

    SPARSE_GRID_CC_INDEX indexes the points forming a sparse grid.

  Discussion:

    The points forming the sparse grid are guaranteed to be a subset
    of a certain product grid.  The product grid is formed by DIM_NUM
    copies of a 1D rule of fixed order.  The orders of the 1D rule,
    (called ORDER_1D) and the order of the product grid, (called ORDER)
    are determined from the value LEVEL_MAX.

    Thus, any point in the product grid can be identified by its grid index,
    a set of DIM_NUM indices, each between 1 and ORDER_1D.

    This routine creates the GRID_INDEX array, listing (uniquely) the
    points of the sparse grid.  

    An assumption has been made that the 1D rule is closed (includes
    the interval endpoints) and nested (points that are part of a rule
    of a given level will be part of every rule of higher level).

  Licensing:

    This code is distributed under the GNU LGPL license. 

  Modified:

    12 March 2013

  Author:

    John Burkardt

  Reference:

    Fabio Nobile, Raul Tempone, Clayton Webster,
    A Sparse Grid Stochastic Collocation Method for Partial Differential
    Equations with Random Input Data,
    SIAM Journal on Numerical Analysis,
    Volume 46, Number 5, 2008, pages 2309-2345.

  Parameters:

    Input, int DIM_NUM, the spatial dimension.

    Input, int LEVEL_MAX, the maximum value of LEVEL.

    Input, int POINT_NUM, the total number of points in the grids.

    Output, int SPARSE_GRID_CC_INDEX[DIM_NUM*POINT_NUM], a list of point 
    indices, representing a subset of the product grid of level LEVEL_MAX,
    representing (exactly once) each point that will show up in a
    sparse grid of level LEVEL_MAX.
*/
{
  int dim;
  int *grid_index;
  int *grid_index2;
  int *grid_level;
  int h;
  int level;
  int *level_1d;
  int more;
  int *order_1d;
  int order_nd;
  int point;
  int point_num2;
  int t;

  grid_index = ( int * ) malloc ( dim_num * point_num * sizeof ( int ) );
/*
  The outer loop generates LEVELs from 0 to LEVEL_MAX.
*/
  point_num2 = 0;

  level_1d = ( int * ) malloc ( dim_num * sizeof ( int ) );
  order_1d = ( int * ) malloc ( dim_num * sizeof ( int ) );

  for ( level = 0; level <= level_max; level++ )
  {
/*
  The middle loop generates the next partition LEVEL_1D(1:DIM_NUM)
  that adds up to LEVEL.
*/
    more = 0;
    h = 0;
    t = 0;

    for ( ; ; )
    {
      comp_next ( level, dim_num, level_1d, &more, &h, &t );
/*
  Transform each 1D level to a corresponding 1D order.
*/
      level_to_order_closed ( dim_num, level_1d, order_1d );
/*
  The product of the 1D orders gives us the number of points in this grid.
*/
      order_nd = i4vec_product ( dim_num, order_1d );
/*
  The inner (hidden) loop generates all points corresponding to given grid.
*/
      grid_index2 = multigrid_index0 ( dim_num, order_1d, order_nd );
/*
  Adjust these grid indices to reflect LEVEL_MAX.
*/
      multigrid_scale_closed ( dim_num, order_nd, level_max, level_1d, 
        grid_index2 );
/*
  Determine the first level of appearance of each of the points.
*/
      grid_level = abscissa_level_closed_nd ( level_max, dim_num, order_nd, 
        grid_index2 );
/*
  Only keep those points which first appear on this level.
*/
      for ( point = 0; point < order_nd; point++ )
      {
        if ( grid_level[point] == level )
        {
          for ( dim = 0; dim < dim_num; dim++ )
          {
            grid_index[dim+point_num2*dim_num] =
              grid_index2[dim+point*dim_num];
          }
          point_num2 = point_num2 + 1;
        }
      }

      free ( grid_index2 );
      free ( grid_level );

      if ( !more )
      {
        break;
      }
    }
  }
  free ( level_1d );
  free ( order_1d );

  return grid_index;
}
/******************************************************************************/

int sparse_grid_cc_size_old ( int dim_num, int level_max )

/******************************************************************************/
/*
  Purpose:

    SPARSE_GRID_CC_SIZE_OLD sizes a sparse grid of Clenshaw Curtis points.

  Discussion:

    This function has been replaced by a new version which is much faster.

    This version is retained for historical interest.

    The grid is defined as the sum of the product rules whose LEVEL
    satisfies:

      0 <= LEVEL <= LEVEL_MAX.

    This routine works on an abstract set of nested grids.

  Licensing:

    This code is distributed under the GNU LGPL license. 

  Modified:

    12 March 2013

  Author:

    John Burkardt

  Reference:

    Fabio Nobile, Raul Tempone, Clayton Webster,
    A Sparse Grid Stochastic Collocation Method for Partial Differential
    Equations with Random Input Data,
    SIAM Journal on Numerical Analysis,
    Volume 46, Number 5, 2008, pages 2309-2345.

  Parameters:

    Input, int DIM_NUM, the spatial dimension.

    Input, int LEVEL_MAX, the maximum value of LEVEL.

    Output, int SPARSE_GRID_CC_SIZE, the number of points in the grid.
*/
{
  int *grid_index;
  int *grid_level;
  int h;
  int level;
  int *level_1d;
  int more;
  int *order_1d;
  int order_nd;
  int point;
  int point_num;
  int t;
/*
  Special case.
*/
  if ( level_max == 0 )
  {
    point_num = 1;
    return point_num;
  }
/*
  The outer loop generates LEVELs from 0 to LEVEL_MAX.
*/
  point_num = 0;

  level_1d = ( int * ) malloc ( dim_num * sizeof ( int ) );
  order_1d = ( int * ) malloc ( dim_num * sizeof ( int ) );

  for ( level = 0; level <= level_max; level++ )
  {
/*
  The middle loop generates the next partition that adds up to LEVEL.
*/
    more = 0;
    h = 0;
    t = 0;

    for ( ; ; )
    {
      comp_next ( level, dim_num, level_1d, &more, &h, &t );
/*
  Transform each 1D level to a corresponding 1D order.
*/
      level_to_order_closed ( dim_num, level_1d, order_1d );
/*
  The product of the 1D orders gives us the number of points in this grid.
*/
      order_nd = i4vec_product ( dim_num, order_1d );
/*
  The inner (hidden) loop generates all points corresponding to given grid.
*/
      grid_index = multigrid_index0 ( dim_num, order_1d, order_nd );
/*
  Adjust these grid indices to reflect LEVEL_MAX.
*/
      multigrid_scale_closed ( dim_num, order_nd, level_max, level_1d, 
        grid_index );
/*
  Determine the first level of appearance of each of the points.
*/
      grid_level = abscissa_level_closed_nd ( level_max, dim_num, order_nd, 
        grid_index );
/*
  Only keep those points which first appear on this level.
*/
      for ( point = 0; point < order_nd; point++ )
      {
        if ( grid_level[point] == level )
        {
          point_num = point_num + 1;
        }
      }

      free ( grid_index );
      free ( grid_level );

      if ( !more )
      {
        break;
      }
    }
  }

  free ( level_1d );
  free ( order_1d );

  return point_num;
}
/******************************************************************************/

void sparse_grid_cc_weights ( int dim_num, int level_max, int point_num, 
  int grid_index[], double grid_weight[] )

/******************************************************************************/
/*
  Purpose:

    SPARSE_GRID_CC_WEIGHTS gathers the weights.

  Licensing:

    This code is distributed under the GNU LGPL license. 

  Modified:

    12 March 2013

  Author:

    John Burkardt

  Reference:

    Fabio Nobile, Raul Tempone, Clayton Webster,
    A Sparse Grid Stochastic Collocation Method for Partial Differential
    Equations with Random Input Data,
    SIAM Journal on Numerical Analysis,
    Volume 46, Number 5, 2008, pages 2309-2345.

  Parameters:

    Input, int DIM_NUM, the spatial dimension.

    Input, int LEVEL_MAX, the maximum value of LEVEL.

    Input, int POINT_NUM, the total number of points in the grids.

    Input, int GRID_INDEX[DIM_NUM*POINT_NUM], a list of point indices,
    representing a subset of the product grid of level LEVEL_MAX,
    representing (exactly once) each point that will show up in a
    sparse grid of level LEVEL_MAX.

    Output, double GRID_WEIGHT[POINT_NUM], the weights
    associated with the sparse grid points.
*/
{
  int all_equal;
  int coeff;
  int dim;
  int found;
  int *grid_index2;
  double *grid_weight2;
  int h;
  int level;
  int *level_1d;
  int level_min;
  int more;
  int order_nd;
  int *order_1d;
  int point;
  int point2;
  int t;

  if ( level_max == 0 )
  {
    for ( point = 0; point < point_num; point++ )
    { 
      grid_weight[point] = pow ( 2.0, dim_num );
    }
    return;
  }

  level_1d = ( int * ) malloc ( dim_num * sizeof ( int ) );
  order_1d = ( int * ) malloc ( dim_num * sizeof ( int ) );

  for ( point = 0; point < point_num; point++ )
  { 
    grid_weight[point] = 0.0;
  }

  level_min = i4_max ( 0, level_max + 1 - dim_num );

  for ( level = level_min; level <= level_max; level++ )
  {
/*
  The middle loop generates the next partition LEVEL_1D(1:DIM_NUM)
  that adds up to LEVEL.
*/
    more = 0;
    h = 0;
    t = 0;

    for ( ; ; )
    {
      comp_next ( level, dim_num, level_1d, &more, &h, &t );
/*
  Transform each 1D level to a corresponding 1D order.
*/
      level_to_order_closed ( dim_num, level_1d, order_1d );
/*
  The product of the 1D orders gives us the number of points in this grid.
*/
      order_nd = i4vec_product ( dim_num, order_1d );
/*
  Generate the indices of the points corresponding to the grid.
*/
      grid_index2 = multigrid_index0 ( dim_num, order_1d, order_nd );
/*
  Compute the weights for this grid.
*/
      grid_weight2 = product_weights_cc ( dim_num, order_1d, order_nd );
/*
  Adjust the grid indices to reflect LEVEL_MAX.
*/
      multigrid_scale_closed ( dim_num, order_nd, level_max, level_1d, 
        grid_index2 );
/*
  Now determine the coefficient.
*/
      coeff = i4_mop ( level_max - level ) 
        * i4_choose ( dim_num - 1, level_max - level );

      for ( point2 = 0; point2 < order_nd; point2++ )
      {
        found = 0;

        for ( point = 0; point < point_num; point++ )
        {
          all_equal = 1;
          for ( dim = 0; dim < dim_num; dim++ )
          {
            if ( grid_index2[dim+point2*dim_num] !=
                 grid_index[dim+point*dim_num] )
            {
              all_equal = 0;
              break;
            }
          }
          if ( all_equal )
          {
            grid_weight[point] = grid_weight[point] 
              + ( double ) ( coeff ) * grid_weight2[point2];
            found = 1;
            break;
          }
        }
        if ( !found )
        {
          fprintf ( stderr, "\n" );
          fprintf ( stderr, "SPARSE_GRID_CC_WEIGHTS - Fatal error!\n" );
          fprintf ( stderr, "  Could not find a match for a point.\n" );
          exit ( 1 );
        }
      }

      free ( grid_index2 );
      free ( grid_weight2 );

      if ( !more )
      {
        break;
      }
    }
  }
  free ( level_1d );
  free ( order_1d );

  return;
}
/******************************************************************************/

int sparse_grid_ccs_size ( int dim_num, int level_max )

/******************************************************************************/
/*
  Purpose:

    SPARSE_GRID_CCS_SIZE sizes a sparse grid using Clenshaw Curtis Slow rules.

  Discussion:

    The grid is defined as the sum of the product rules whose LEVEL
    satisfies:

      0 <= LEVEL <= LEVEL_MAX.

    This calculation is much faster than a previous method.  It simply
    computes the number of new points that are added at each level in the
    1D rule, and then counts the new points at a given DIM_NUM dimensional
    level vector as the product of the new points added in each dimension.

    This approach will work for nested families, and may be extensible
    to other families, and to mixed rules.

  Licensing:

    This code is distributed under the GNU LGPL license. 

  Modified:

    22 December 2009

  Author:

    John Burkardt

  Reference:

    Fabio Nobile, Raul Tempone, Clayton Webster,
    A Sparse Grid Stochastic Collocation Method for Partial Differential
    Equations with Random Input Data,
    SIAM Journal on Numerical Analysis,
    Volume 46, Number 5, 2008, pages 2309-2345.

  Parameters:

    Input, int DIM_NUM, the spatial dimension.

    Input, int LEVEL_MAX, the maximum value of LEVEL.

    Output, int SPARSE_GRID_CC_SIZE, the number of points in the grid.
*/
{
  int dim;
  int h;
  int l;
  int level;
  int *level_1d;
  int more;
  int *new_1d;
  int o;
  int p;
  int point_num;
  int t;
  int v;
/*
  Special case.
*/
  if ( level_max < 0 )
  {
    point_num = 0;
    return point_num;
  }

  if ( level_max == 0 )
  {
    point_num = 1;
    return point_num;
  }
/*
  Construct the vector that counts the new points in the 1D rule.
*/
  new_1d = ( int * ) malloc ( ( level_max + 1 ) * sizeof ( int ) );

  new_1d[0] = 1;
  new_1d[1] = 2;

  p = 3;
  o = 3;

  for ( l = 2; l <= level_max; l++ )
  {
    p = 2 * l + 1;
    if ( o < p )
    {
      new_1d[l] = o - 1;
      o = 2 * o - 1;
    }
    else
    {
      new_1d[l] = 0;
    }
  }
/*
  Count the number of points by counting the number of new points 
  associated with each level vector.
*/
  level_1d = ( int * ) malloc ( dim_num * sizeof ( int ) );

  point_num = 0;

  for ( level = 0; level <= level_max; level++ )
  {
    more = 0;
    h = 0;
    t = 0;

    for ( ; ;)
    {
      comp_next ( level, dim_num, level_1d, &more, &h, &t );

      v = 1;
      for ( dim = 0; dim < dim_num; dim++ )
      {
        v = v * new_1d[level_1d[dim]];
      }

      point_num = point_num + v;

      if ( !more )
      {
        break;
      }
    }
  }
  free ( level_1d );
  free ( new_1d );

  return point_num;
}
/******************************************************************************/

int sparse_grid_cfn_size ( int dim_num, int level_max )

/******************************************************************************/
/*
  Purpose:

    SPARSE_GRID_CFN_SIZE sizes a sparse grid using Closed Fully Nested rules.

  Discussion:

    The grid is defined as the sum of the product rules whose LEVEL
    satisfies:

      0 <= LEVEL <= LEVEL_MAX.

    This calculation is much faster than a previous method.  It simply
    computes the number of new points that are added at each level in the
    1D rule, and then counts the new points at a given DIM_NUM dimensional
    level vector as the product of the new points added in each dimension.

    This approach will work for nested families, and may be extensible
    to other families, and to mixed rules.

  Licensing:

    This code is distributed under the GNU LGPL license. 

  Modified:

    12 March 2013

  Author:

    John Burkardt

  Reference:

    Fabio Nobile, Raul Tempone, Clayton Webster,
    A Sparse Grid Stochastic Collocation Method for Partial Differential
    Equations with Random Input Data,
    SIAM Journal on Numerical Analysis,
    Volume 46, Number 5, 2008, pages 2309-2345.

  Parameters:

    Input, int DIM_NUM, the spatial dimension.

    Input, int LEVEL_MAX, the maximum value of LEVEL.

    Output, int SPARSE_GRID_CFN_SIZE, the number of points in the grid.
*/
{
  int dim;
  int h;
  int j;
  int l;
  int level;
  int *level_1d;
  int more;
  int *new_1d;
  int point_num;
  int t;
  int v;
/*
  Special case.
*/
  if ( level_max < 0 )
  {
    point_num = 0;
    return point_num;
  }

  if ( level_max == 0 )
  {
    point_num = 1;
    return point_num;
  }
/*
  Construct the vector that counts the new points in the 1D rule.
*/
  new_1d = ( int * ) malloc ( ( level_max + 1 ) * sizeof ( int ) );

  new_1d[0] = 1;
  new_1d[1] = 2;

  j = 1;
  for ( l = 2; l <= level_max; l++ )
  {
    j = j * 2;
    new_1d[l] = j;
  }
/*
  Count the number of points by counting the number of new points 
  associated with each level vector.
*/
  level_1d = ( int * ) malloc ( dim_num * sizeof ( int ) );

  point_num = 0;

  for ( level = 0; level <= level_max; level++ )
  {
    more = 0;
    h = 0;
    t = 0;

    for ( ; ;)
    {
      comp_next ( level, dim_num, level_1d, &more, &h, &t );

      v = 1;
      for ( dim = 0; dim < dim_num; dim++ )
      {
        v = v * new_1d[level_1d[dim]];
      }

      point_num = point_num + v;

      if ( !more )
      {
        break;
      }
    }
  }
  free ( level_1d );
  free ( new_1d );

  return point_num;
}
/******************************************************************************/

void vec_colex_next2 ( int dim_num, int base[], int a[], int *more )

/******************************************************************************/
/*
  Purpose:

    VEC_COLEX_NEXT2 generates vectors in colex order.

  Discussion:

    The vectors are produced in colexical order, starting with

    (0,        0,        ...,0),
    (1,        0,        ...,0),
     ...
    (BASE(1)-1,0,        ...,0)

    (0,        1,        ...,0)
    (1,        1,        ...,0)
    ...
    (BASE(1)-1,1,        ...,0)

    (0,        2,        ...,0)
    (1,        2,        ...,0)
    ...
    (BASE(1)-1,BASE(2)-1,...,BASE(DIM_NUM)-1).

  Examples:

    DIM_NUM = 2,
    BASE = { 3, 3 }

    0   0
    1   0
    2   0
    0   1
    1   1
    2   1
    0   2
    1   2
    2   2

  Licensing:

    This code is distributed under the GNU LGPL license. 

  Modified:

    12 March 2013

  Author:

    John Burkardt

  Parameters:

    Input, int DIM_NUM, the spatial dimension.

    Input, int BASE[DIM_NUM], the bases to be used in each dimension.
    In dimension I, entries will range from 0 to BASE[I]-1.

    Output, int A[DIM_NUM], the next vector.

    Input/output, int *MORE.  Set this variable false before
    the first call.  On return, MORE is TRUE if another vector has
    been computed.  If MORE is returned FALSE, ignore the output 
    vector and stop calling the routine.
*/
{
  int i;

  if ( !( *more ) )
  {
    for ( i = 0; i < dim_num; i++ )
    {
      a[i] = 0;
    }
    *more = 1;
  }
  else
  {
    for ( i = 0; i < dim_num; i++ )
    {
      a[i] = a[i] + 1;

      if ( a[i] < base[i] )
      {
        return;
      }
      a[i] = 0;
    }
    *more = 0;
  }

  return;
}

