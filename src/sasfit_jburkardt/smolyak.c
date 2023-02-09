#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "smolpack.h"
#include "smolpack_global.h"

#define uniw 32     /* total # of SP.nodes of quadrature formulae       */
#define fn 6        /* # of different basic formulas                  */
#define gesfn 50    /* # of basic formulae  (incl. multiplicities)   */

/*
  Tree definitions:
*/

struct tnode {
  int empty;
  double *coeff;
  int *computed;
  struct tnode *left;
  struct tnode *right;
};

struct tnode *root;

void formula (int,int);
double eval (int);                     /* sub-formula calculator      */
double fsum (int);                     /* sum(f(+-x_nu))      */
void init ( size_t dim );
double calccoeff (int);                /* coefficient calculator      */
double calccoeff2 (int,int);       /* coefficient calculator (slow)   */
double wl (int, int, int);             /* 'divide                     */
double we (int, int, int);             /*         and                 */
void sumind ( int, int );                /*            conquer'         */
double coeff ( );
struct tnode *talloc ( void );       /* node generator          */
void frei ( struct tnode *p );


/******************************************************************************/

double int_smolyak ( size_t dim, int qq, double *cubmin, double *cubmax, double (*ff) (double*, size_t, void*), void* pp,
  int print_stats )

/******************************************************************************/
/*
  Purpose:

    INT_SMOLYAK approximates a multidimensional integral.

  Discussion:

    A sparse grid is used, based on a "delayed" Clenshaw Curtis quadrature
    rule, using the Smolyak method.  The computation should be able to
    reach accuracy comparable to that achieved with the standard
    Clenshaw Curtis rule, but with considerably fewer function evaluations.

  Modified:

    28 April 2007

  Author:

    Knut Petras

  Parameters:

    Input, int DIM, the spatial dimension.
    1 <= DIM < MAXDIM = 40.

    Input, int QQ, ?
    QQ - DIM < 48

    Input, double ( *FF ) ( int D, double X[] ), the function to be integrated.

    Input, int PRINT_STATS, is nonzero if this routine should print out
    statistics for the number of function calls and weight computations.

    Output, double INT_SMOLYAK, the approximated value of the integral.
*/
{
/*
  Make the parameters global.
*/
  SP.d = dim;
  SP.q = qq;
  SP.f = ff;
  SP.cubparam=pp;
  SP.cubmin=cubmin;
  SP.cubmax=cubmax;

/*
  Initialize.
*/
  SP.wcount = 0;
  SP.count = 0;
  SP.quafo = 0.0;

  init ( dim );
/*
  Call the Smolyak algorithm.  Q-DIM = K, the number of stages.
*/
  formula ( 1, SP.q-dim );
/*
  Free the allocated memory.
*/
  frei ( root );
/*
  Print statistics, if desired.
*/
  if ( print_stats )
  {
    printf ( "\n" );
    printf ( "  Number of function calls =  %d\n", SP.count );
    printf ( "  Weight evaluations =        %d\n", SP.wcount );
  }

  return SP.quafo;
}
/******************************************************************************/

void formula ( int k, int l )

/******************************************************************************/
/*
  Purpose:

    FORMULA carries out the Smolyak algorithm.

  Discussion:

    If K == D+1
      evaluation.
    Else
      determine the required formula

    This routine constructs the quadrature result by adding contributions
    to the global variable SP.quafo.

  Modified:

    25 April 2007

  Author:

    Knut Petras

  Parameters:

    Input, int K, ???

    Input, int L, the index sum that may be distributed
    to the remaining dimensions.
*/
{
  int i;

  if ( k == SP.d + 1 )
  {
    SP.quafo = SP.quafo + eval ( 0 );
  }
  else
  {
    for ( i = 0; i <= l; i++ )
    {
      if ( SP.sw[i] < fn )
      {
        SP.indices[k] = SP.sw[i];
        formula ( k+1, l-i );
      }
    }
  }
  return;
}
/******************************************************************************/

double eval ( int k )

/******************************************************************************/
/*
  Purpose:

    EVAL calculates the value of a product formula.

  Discussion:

    USUAL:
      SP.summe = SP.summe + calccoeff2(0,q-d) * fsum(0);
    DAC:
      SP.summe = SP.summe + calccoeff(q-d) * fsum(0);
    TREE (recommended method):
      SP.summe = SP.summe +    coeff() * fsum(0);
    all coefficients 1 (speed test):
      SP.summe = SP.summe +  fsum(0);

  Modified:

    25 April 2007

  Author:

    Knut Petras

  Parameters:

    Input, int K, ???

    Output, double EVAL, the value of the product formula.
*/
{
  double dummy;
  int i;

  if ( k == 0 )
  {
    SP.summe = 0.0;
    dummy = eval ( 1 );
  }
/*
  Summation corresponding to one coefficient.
*/
  else if ( k == SP.d + 1 )
  {
    SP.summe = SP.summe + coeff ( ) * fsum ( 0 );
  }
/*
  Choice of the SP.nodes.
*/
  else
  {
    for ( i = 0; i <= SP.n[SP.indices[k]]; i++ )
    {
      SP.argind[k] = i;
      dummy = eval ( k + 1 );
    }
  }
  return SP.summe;
}
/******************************************************************************/

double coeff ( void )

/******************************************************************************/
/*
  Purpose:

    COEFF returns a coefficient.

  Discussion:

    The routine calculates the coefficient only if it determines that
    the value was not already computed and saved in the tree.

  Modified:

    25 April 2007

  Author:

    Knut Petras

  Parameters:

    Output, double COEFF, the value of the coefficient.
*/
{
  int i;
  int j;
  int l;
  struct tnode *p;
  struct tnode *pt;
  int r;

  p = root;
/*
  Initialize.
*/
  for ( i = 0; i < SP.maxind; i++ )
  {
    SP.anzw[i] = 0;
  }
/*
  Frequency SP.anzw[  ] of 1-dim SP.nodes
*/
  for ( i = 1; i <= SP.d; i++ )
  {
    SP.anzw[SP.lookind[SP.indices[i]][SP.argind[i]]]++;
  }
/*
  Search in the tree according to SP.anzw[..]
*/
  for ( j = SP.maxind-1; 1 <= j; j-- )
  {

/*
  SP.anzw[j] to the LEFT.
  Generate the node if it does not exist.
*/
    if ( p->left == NULL )
    {
      p->left = (struct tnode *) calloc ( maxdim, sizeof(struct tnode) );
      pt = (p->left+SP.anzw[j]);
      pt->left = NULL;
      pt->right = NULL;
      pt->empty = 1;
      p = pt;
    }
    else
    {
      p = ( p->left+SP.anzw[j] );
    }
/*
  one to the RIGHT
*/
    if ( p->right == NULL )
    {
      pt = talloc ( );
      pt->empty = 1;
      pt->left = NULL;
      pt->right = NULL;
/*
  leaf with coefficient.
*/
      if ( j == 1 )
      {
        pt->coeff = ( double * ) calloc ( maxdim, sizeof(double) );
        pt->computed = ( int * ) calloc ( maxdim, sizeof(int) );
        pt->empty = 0;
      }
      p->right = pt;
    }
    p = p->right;
  }
/*
  The coefficient must be computed.
*/
  if ( !*( p->computed+SP.anzw[0] ) )
  {
    SP.wcount++;
    *( p->coeff +SP.anzw[0] ) = calccoeff ( SP.q-SP.d );
    *( p->computed+SP.anzw[0] ) = 1;
  }
  return *( p->coeff +SP.anzw[0] );
}
/******************************************************************************/

void frei ( struct tnode *p )

/******************************************************************************/
/*
  Purpose:

    FREI frees the memory allocated for the tree structure.

  Modified:

    25 April 2007

  Author:

    Knut Petras

  Parameters:

    Input, struct tnode *P, a pointer to the tree structure to be freed.
*/
{
  int i;

  if ( !( p->empty ) )
  {
    free ( p->coeff );
    free ( p->computed );
  }

  if ( !( p->left == NULL ) )
  {
    for ( i = 0; i < SP.maxind; i++ )
    {
      if ( !( p->left+i == NULL ) )
      {
        frei ( p->left +i );
      }
    }
    free ( p->left );
  }

  if ( !( p->right == NULL ) )
  {
     frei ( p->right );
     free ( p->right );
  }
  return;
}
/******************************************************************************/

struct tnode *talloc ( )

/******************************************************************************/
/*
  Purpose:

    TALLOC allocates space for a new node on the tree.

  Modified:

    25 April 2007

  Author:

    Knut Petras

  Parameters:

    Output, struct tnode *TALLOC, a pointer to the new tree-node space.
*/
{
  return ( struct tnode * ) malloc ( sizeof(struct tnode) );
}
/******************************************************************************/

void sumind ( int r, int s )

/******************************************************************************/
/*
  Purpose:

    SUMIND sums formula SP.indices at division of dimensions R through S.

  Modified:

    25 April 2007

  Author:

    Knut Petras

  Parameters:

    Input, int R, S, the range over which the summation takes place.
*/
{
  int q;
/*
  R == S, one dimensional.
  Do the calculation.
*/
  if ( r == s )
  {
    SP.indsum[r][s] = SP.ninv[SP.indices[r]];
  }
/*
  R < S, compute average Q, split to [R,Q] + [Q+1,S],
  and call SUMIND recursively.
*/
  else
  {
    q = ( r + s ) / 2;
    sumind ( r, q );
    sumind ( q+1, s );
    SP.indsum[r][s] = SP.indsum[r][q] + SP.indsum[q+1][s];
  }
  return;
}
/******************************************************************************/

double calccoeff ( int l )

/******************************************************************************/
/*
  Purpose:

    CALCCOEFF calculates coefficients..

  Modified:

    25 April 2007

  Author:

    Knut Petras

  Parameters:

    Input, int L, ?

    Output, double CALCCOEFF, the value of the coefficient.
*/
{
  double value;
/*
  Calculate the subdivision parameters.
*/
  sumind ( 1, SP.d );
/*
  Start the divide and conquer process.
*/
  value = wl ( 1, SP.d, l );

  return value;
}
/******************************************************************************/

double wl ( int r, int s, int l )

/******************************************************************************/
/*
  Purpose:

    WL sums in dimensions S through R with sum of formula numbers <= L.

  Modified:

    25 April 2007

  Author:

    Knut Petras

  Parameters:

    Input, int R, S, specify the range.

    Output, double WL, ?
*/
{
  int i;
  int p;
  int q;
  double total;

  total = 0.0;
/*
  R = S, one dimensional.
  Do the calculation.
*/
  if ( r == s )
  {
    p = SP.lookind[SP.indices[r]][SP.argind[r]];

    for ( i = SP.ninv[SP.indices[r]]; i <= l; i++ )
    {
      if ( SP.sw[i] < fn )
      {
        if ( i == 0 )
        {
          total = total + SP.dnu[0][0];
        }
        else
        {
          if ( SP.indices[r] == 0 )
          {
            total = total + SP.dnu[SP.sw[i]][0];
          }
          else
          {
            total = total + SP.dnu[SP.sw[i]][SP.invlook[SP.sw[i]][p]];
          }
        }
      }
    }
  }
/*
  R < S, compute average Q, split to [R,Q] + [Q+1,S]
  and call WE and WL recursively.
*/
  else
  {
    q = ( r + s ) / 2;

    for ( i = SP.indsum[r][q]; i <= l-SP.indsum[q+1][s]; i++ )
    {
      total = total + we ( r, q, i ) * wl ( q+1, s, l-i );
    }
  }

  return total;
}
/******************************************************************************/

double we ( int r, int s, int l )

/******************************************************************************/
/*
  Purpose:

    WE sums in dimension S through R with sum of formula numbers <= L.

  Modified:

    25 April 2007

  Author:

    Knut Petras

  Parameters:

    Input, int R, S, specify the range.

    Output, double CC_WE, ?
*/
{
  int i;
  int q;
  double total;

  total = 0.0;
/*
  R = S, one dimensional.
  Do the calculation.
*/
  if ( r == s )
  {
    if ( SP.sw[l] < fn )
    {
      if ( SP.sw[l] == 0 )
      {
        total = SP.dnu[0][0];
      }
      else
      {
        if ( SP.indices[r] == 0 )
        {
          total = SP.dnu[SP.sw[l]][0];
        }
        else
        {
          total = SP.dnu[SP.sw[l]][SP.invlook[SP.sw[l]][SP.lookind[SP.indices[r]][SP.argind[r]]]];
        }
      }
    }
  }
/*
  R < S, compute average Q, split to [R,Q] + [Q+1,S] and call WE recursively.
*/
  else
  {
    q = ( r + s ) / 2;
    for ( i = SP.indsum[r][q]; i <= l-SP.indsum[q+1][s]; i++ )
    {
      total = total + we ( r, q, i ) * we ( q+1, s, l-i );
    }
  }
  return total;
}
/******************************************************************************/

double calccoeff2 ( int k, int l )

/******************************************************************************/
/*
  Purpose:

    CALCCOEFF2 ???

  Modified:

    25 April 2007

  Author:

    Knut Petras

  Parameters:

    Input, int K, ?

    Input, int L, ?

    Output, double CALCCOEFF2, ?
*/
{
  double dummy;
  int i;
  double wprod;

  if ( k == 0 )
  {
    SP.wcount++;
    SP.wsum = 0.0;
    dummy = calccoeff2 ( 1, l );
  }
  else if ( k == SP.d + 1 )
  {
    wprod = 1.0;

    for ( i = 1; i <= SP.d; i++ )
    {
      if ( SP.indices[i] == 0 )
      {
        wprod = wprod * SP.dnu[SP.wind[i]][0];
      }
      else
      {
        wprod = wprod *
          SP.dnu[SP.wind[i]][SP.invlook[SP.wind[i]][SP.lookind[SP.indices[i]][SP.argind[i]]]];
      }
    }
    SP.wsum = SP.wsum + wprod;
  }
  else
  {
    i = SP.indices[k];
    while ( SP.ninv[i] <= l )
    {
      SP.wind[k] = i;
      dummy = calccoeff2 ( k+1, l-SP.ninv[i] );
      i++;
    }
  }
  return SP.wsum;
}
/******************************************************************************/

double fsum ( int k )

/******************************************************************************/
/*
  Purpose:

    FSUM computes unweighted sums of function values: sum(f(+/- x_i) ).

  Discussion:

    The function is evaluated at SP.nodes for which the same weight applies.

  Modified:

    26 April 2007

  Author:

    Knut Petras

  Parameters:

    Input, int K, ?

    Output, double FSUM, the sum of the function at several SP.nodes.
*/
{
  double dummy;

  if ( k == 0 )
  {
    SP.ftotal = 0.0;
    dummy = fsum ( 1 );
  }
  else if ( k == SP.d + 1 )
  {
    SP.ftotal = SP.ftotal + (*SP.f) (SP.x_ab, SP.d, SP.cubparam );
  }
  else
  {
    if ( SP.indices[k] == 0 )
    {
      SP.x[k-1] = 0.5;
      SP.x_ab[k-1] = SP.cubmin[k-1] + (SP.cubmax[k-1]-SP.cubmin[k-1]) * SP.x[k-1];
      dummy = fsum ( k+1 );
    }
/*
  Use symmetry to get both X and -X.
*/
    else
    {
      SP.x[k-1] = SP.xnu[SP.indices[k]][2*SP.argind[k]+1];
      SP.x_ab[k-1] = SP.cubmin[k-1] + (SP.cubmax[k-1]-SP.cubmin[k-1]) * SP.x[k-1];
      dummy = fsum ( k+1 );
      SP.x[k-1] = 1.0 - SP.x[k-1];
      SP.x_ab[k-1] = SP.cubmin[k-1] + (SP.cubmax[k-1]-SP.cubmin[k-1]) * SP.x[k-1];
      dummy = fsum ( k+1 );
    }
  }
  return SP.ftotal;
}
/******************************************************************************/

void init ( size_t dim )

/******************************************************************************/
/*
  Purpose:

    INIT does initialization.

  Modified:

    28 April 2007

  Author:

    Knut Petras

  Parameters:

    Input, int DIM, the spatial dimension.

  Local Paremeters:

    Local, int NJ[FN], the number of SP.nodes in the 1D formulas.

    Local, int FREQ[FN], the frequencies of the basis formulas.
*/
{
  int formfakt;
  int freq[fn] = { 1, 2, 3, 6, 12, 24 };
  int i;
  int j;
  int maxform;
  int nj[fn] = { 1, 3, 7, 15, 31, 63 };
/*
  Some parameter calculations.
*/
  SP.n[0] = 0;
  j = 0;
  SP.ninv[0] = 0;

  for ( i = 1; i < fn; i++ )
  {
    SP.n[i] = ( nj[i] - nj[i-1] - 2 ) / 2;
    j = j + freq[i-1];
    SP.ninv[i] = j;
  }

  for ( i = 0; i < gesfn; i++ )
  {
    SP.sw[i] = fn;
  }

  for ( i = 0; i < fn; i++ )
  {
    SP.sw[SP.ninv[i]] = i;
  }
/*
  Number of the 'largest' used formula.
*/
  i = SP.q - dim;
  maxform = 0;
  while ( SP.ninv[maxform+1] <= i )
  {
    maxform = maxform + 1;
  }
/*
  Total number of used 1-dim SP.nodes.
*/
  SP.maxind = ( nj[maxform] + 1 ) / 2;
/*
  Table of 1-dim nodal numbers 0..SP.maxind-1 corresponding
  to a combination formula number/nodal number
  and inverse formula.
*/
  SP.lookind[0][0] = 0;
  for ( i = 1; i <= maxform; i++ )
  {
    formfakt = pow ( 2, maxform-i );
    for ( j = 0; j < (nj[i]+1)/4; j++ )
    {
      SP.lookind[i][j] = formfakt * ( 2 * j + 1 );
    }
/*
  In a linear ordering of all used SP.nodes, the (2J+1)-th
  node of the I-th basic formula is SP.lookind[I][J]-th node.
*/
    for ( j = 0; j < (nj[i]+1)/2; j++ )
    {
      SP.invlook[i][formfakt*j] = j;
    }
/*
  The SP.lookind[I][2^(MAXFORM-I)]-th node in a linear
  ordering of all used SP.nodes is the J-th node of the
  I-th basic formula.  Note that MAXFORM is the number
  of used different basic formulas.
*/
  }

/*
  Root of the coefficient TREE.
*/
  root = talloc ( );
  root->empty = 1;
  root->left = NULL;
  root->right = NULL;
/*
  One dimensional formulas (Deltas)
*/
  SP.xnu[0][0] = 0.5;
  SP.dnu[0][0] = 1.0;

  SP.xnu[1][0] =  5.000000000000000E-001;
  SP.xnu[1][1] =  8.8729833462074168851793E-001;
  SP.dnu[1][0] = -5.5555555555555555555556E-001;
  SP.dnu[1][1] =  2.7777777777777777777778E-001;

  SP.xnu[2][0] =  5.0000000000000000000000E-001;
  SP.xnu[2][1] =  7.1712187467340127900104E-001;
  SP.xnu[2][2] =  8.8729833462074168851793E-001;
  SP.xnu[2][3] =  9.8024563435401014171175E-001;
  SP.dnu[2][0] = -2.1898617511520737327189E-001;
  SP.dnu[2][1] =  2.0069870738798111145253E-001;
  SP.dnu[2][2] = -1.4353373284361105741349E-001;
  SP.dnu[2][3] =  5.2328113013233632596912E-002;

  SP.xnu[3][0] =  5.0000000000000000000000E-001;
  SP.xnu[3][1] =  6.1169334321448344081410E-001;
  SP.xnu[3][2] =  7.1712187467340127900104E-001;
  SP.xnu[3][3] =  8.1055147336861320147034E-001;
  SP.xnu[3][4] =  8.8729833462074168851793E-001;
  SP.xnu[3][5] =  9.4422961643612849944521E-001;
  SP.xnu[3][6] =  9.8024563435401014171175E-001;
  SP.xnu[3][7] =  9.9691598160637751110426E-001;
  SP.dnu[3][0] = -1.1270301943013372747934E-001;
  SP.dnu[3][1] =  1.0957842920079374820185E-001;
  SP.dnu[3][2] = -1.0038444269948660093556E-001;
  SP.dnu[3][3] =  8.5755954568195690393677E-002;
  SP.dnu[3][4] = -6.7036417312274610184300E-002;
  SP.dnu[3][5] =  4.6463597657562268842947E-002;
  SP.dnu[3][6] = -2.6526471514693762748452E-002;
  SP.dnu[3][7] =  8.5008598149701301695137E-003;

  SP.xnu[4][0] =  5.0000000000000000000000E-001;
  SP.xnu[4][1] =  5.5624447156659331287292E-001;
  SP.xnu[4][2] =  6.1169334321448344081410E-001;
  SP.xnu[4][3] =  6.6556769662898841654632E-001;
  SP.xnu[4][4] =  7.1712187467340127900104E-001;
  SP.xnu[4][5] =  7.6565987182218781198605E-001;
  SP.xnu[4][6] =  8.1055147336861320147034E-001;
  SP.xnu[4][7] =  8.5124810324576353930490E-001;
  SP.xnu[4][8] =  8.8729833462074168851793E-001;
  SP.xnu[4][9] =  9.1836296908443436775138E-001;
  SP.xnu[4][10] =  9.4422961643612849944521E-001;
  SP.xnu[4][11] =  9.6482742871487002833506E-001;
  SP.xnu[4][12] =  9.8024563435401014171175E-001;
  SP.xnu[4][13] =  9.9076557477687005343368E-001;
  SP.xnu[4][14] =  9.9691598160637751110426E-001;
  SP.xnu[4][15] =  9.9954906248383379883111E-001;
  SP.dnu[4][0] = -5.6377621538718997889636E-002;
  SP.dnu[4][1] =  5.5978436510476728440072E-002;
  SP.dnu[4][2] = -5.4789218672831429083502E-002;
  SP.dnu[4][3] =  5.2834946790117404871908E-002;
  SP.dnu[4][4] = -5.0157125382596721131319E-002;
  SP.dnu[4][5] =  4.6813554990632236808329E-002;
  SP.dnu[4][6] = -4.2877994543200514816583E-002;
  SP.dnu[4][7] =  3.8439810249501765521353E-002;
  SP.dnu[4][8] = -3.3603750473896758409784E-002;
  SP.dnu[4][9] =  2.8489754747061678706099E-002;
  SP.dnu[4][10] = -2.3232151026683275572245E-002;
  SP.dnu[4][11] =  1.7978551653564661048389E-002;
  SP.dnu[4][12] = -1.2897842450451543066137E-002;
  SP.dnu[4][13] =  8.2230249271939054668942E-003;
  SP.dnu[4][14] = -4.2835769453095770463562E-003;
  SP.dnu[4][15] =  1.2723903957809372077014E-003;

  SP.xnu[5][0] =  5.0000000000000000000000E-001;
  SP.xnu[5][1] =  5.2817215652329639498598E-001;
  SP.xnu[5][2] =  5.5624447156659331287292E-001;
  SP.xnu[5][3] =  5.8411762577610373249116E-001;
  SP.xnu[5][4] =  6.1169334321448344081410E-001;
  SP.xnu[5][5] =  6.3887491101091215753268E-001;
  SP.xnu[5][6] =  6.6556769662898841654632E-001;
  SP.xnu[5][7] =  6.9167966209936517345824E-001;
  SP.xnu[5][8] =  7.1712187467340127900104E-001;
  SP.xnu[5][9] =  7.4180901347292051378108E-001;
  SP.xnu[5][10] =  7.6565987182218781198605E-001;
  SP.xnu[5][11] =  7.8859785502602290742185E-001;
  SP.xnu[5][12] =  8.1055147336861320147034E-001;
  SP.xnu[5][13] =  8.3145483001239029773051E-001;
  SP.xnu[5][14] =  8.5124810324576353930490E-001;
  SP.xnu[5][15] =  8.6987802217634737933861E-001;
  SP.xnu[5][16] =  8.8729833462074168851793E-001;
  SP.xnu[5][17] =  9.0347026597510880592815E-001;
  SP.xnu[5][18] =  9.1836296908443436775138E-001;
  SP.xnu[5][19] =  9.3195396909684523857321E-001;
  SP.xnu[5][20] =  9.4422961643612849944521E-001;
  SP.xnu[5][21] =  9.5518557847850214624890E-001;
  SP.xnu[5][22] =  9.6482742871487002833506E-001;
  SP.xnu[5][23] =  9.7317142918670145257425E-001;
  SP.xnu[5][24] =  9.8024563435401014171175E-001;
  SP.xnu[5][25] =  9.8609143737429089828903E-001;
  SP.xnu[5][26] =  9.9076557477687005343368E-001;
  SP.xnu[5][27] =  9.9434237877371473996926E-001;
  SP.xnu[5][28] =  9.9691598160637751110426E-001;
  SP.xnu[5][29] =  9.9860312968611097953823E-001;
  SP.xnu[5][30] =  9.9954906248383379883111E-001;
  SP.xnu[5][31] =  9.9993644406017880596898E-001;
  SP.dnu[5][0] = -2.8188814180191987109744E-002;
  SP.dnu[5][1] =  2.8138849915627150636298E-002;
  SP.dnu[5][2] = -2.7989218255238568736295E-002;
  SP.dnu[5][3] =  2.7740702178279681993919E-002;
  SP.dnu[5][4] = -2.7394605263980886602235E-002;
  SP.dnu[5][5] =  2.6952749667633031963438E-002;
  SP.dnu[5][6] = -2.6417473395059144940870E-002;
  SP.dnu[5][7] =  2.5791626976024229388405E-002;
  SP.dnu[5][8] = -2.5078569652948020678807E-002;
  SP.dnu[5][9] =  2.4282165203336599357974E-002;
  SP.dnu[5][10] = -2.3406777495318230607005E-002;
  SP.dnu[5][11] =  2.2457265826816098707127E-002;
  SP.dnu[5][12] = -2.1438980012491308330637E-002;
  SP.dnu[5][13] =  2.0357755058472159466947E-002;
  SP.dnu[5][14] = -1.9219905124773999502032E-002;
  SP.dnu[5][15] =  1.8032216390391286320054E-002;
  SP.dnu[5][16] = -1.6801938573891486499334E-002;
  SP.dnu[5][17] =  1.5536775555843982439942E-002;
  SP.dnu[5][18] = -1.4244877374144904399846E-002;
  SP.dnu[5][19] =  1.2934839663607373455379E-002;
  SP.dnu[5][20] = -1.1615723310923858549074E-002;
  SP.dnu[5][21] =  1.0297116957956355574594E-002;
  SP.dnu[5][22] = -8.9892758695005258819409E-003;
  SP.dnu[5][23] =  7.7033752332797489010654E-003;
  SP.dnu[5][24] = -6.4518989979126939693347E-003;
  SP.dnu[5][25] =  5.2491234548106609491364E-003;
  SP.dnu[5][26] = -4.1115209485759406322653E-003;
  SP.dnu[5][27] =  3.0577534110586231698391E-003;
  SP.dnu[5][28] = -2.1084676488811257036154E-003;
  SP.dnu[5][29] =  1.2895248973428441362139E-003;
  SP.dnu[5][30] = -6.3981211766590320201509E-004;
  SP.dnu[5][31] =  1.8161074092276532984679E-004;

  return;
}


#undef uniw
#undef fn
#undef gesfn

#define uniw 1025    /* total # of SP.nodes of quadrature formulae     */
#define fn 12        /* # of different basic formulas                */
#define gesfn 12     /* # of basic formulae  (incl. multiplicities) */

/*
  Tree definitions:
*/

struct cc_tnode {
  double coeff;
  int computed;
  struct cc_tnode *left;
  struct cc_tnode *right;
};

struct cc_tnode *cc_root;


void cc_formula ( int, int );
double cc_eval ( int );
double cc_fsum ( int );
void cc_init ( int dim );
double cc_calccoeff ( int );
double cc_calccoeff2 ( int, int );
double cc_wl ( int, int, int );
double cc_we ( int, int, int );
void cc_sumind ( int, int );
int less ( const void *, const void * );
double cc_coeff ( void );
struct cc_tnode *cc_talloc ( void );
void cc_frei ( struct cc_tnode *p );



/******************************************************************************/

double cc_int_smolyak ( size_t dim, int qq, double *cubmin, double *cubmax, double (*ff) (double*, size_t , void*), void* cubparam,
  int print_stats )

/******************************************************************************/
/*
  Purpose:

    CC_INT_SMOLYAK approximates a multidimensional integral.

  Discussion:

    A sparse grid is used, based on a Clenshaw Curtis quadrature rule,
    using the Smolyak method.

  Modified:

    25 April 2007

  Author:

    Knut Petras

  Parameters:

    Input, int DIM, the spatial dimension.
    1 <= DIM < MAXDIM = 40.

    Input, int QQ, ?
    QQ - DIM < 48.

    Input, double ( *FF ) ( int D, double X[] ), the integrand function.

    Input, int PRINT_STATS, is nonzero if this routine should print out
    statistics for the number of function calls and weight computations.

    Output, double CC_INT_SMOLYAK, the approximated value of the integral.
*/
{
/*
  Make the parameters global.
*/
  SP.d = dim;
  SP.q = qq;
  SP.f = ff;
  SP.cubparam = cubparam;
  SP.cubmin = cubmin;
  SP.cubmax = cubmax;
/*
  Initialisation
*/
  SP.wcount = 0;
  SP.count = 0;
  SP.quafo = 0.0;

  cc_init ( dim );
/*
  Call the Smolyak algorithm.  Q-DIM = K, the number of stages.
*/
  cc_formula ( 1, SP.q-dim );
/*
  Free the allocated memory.
*/
  cc_frei ( cc_root );
/*
  Print statistics, if desired.
*/
  if ( print_stats )
  {
    printf ( "\n" );
    printf ( "  Number of function calls =  %d\n", SP.count );
    printf ( "  Weight evaluations =        %d\n", SP.wcount );
  }

  return SP.quafo;
}
/******************************************************************************/

void cc_formula ( int k, int l )

/******************************************************************************/
/*
  Purpose:

    CC_FORMULA carries out the Smolyak algorithm.

  Discussion:

    If K == D+1:
      evaluation.
    Else:
      determine the required formula

    This routine constructs the quadrature result by adding contributions
    to the global variable SP.quafo.

  Modified:

    25 April 2007

  Author:

    Knut Petras

  Parameters:

    Input, int K, ???

    Input, int L, the index sum that may be distributed
    to the remaining dimensions.
*/
{
  int i;

  if ( k == SP.d + 1 )
  {
    SP.quafo = SP.quafo + cc_eval ( 0 );
  }
  else
  {
    for ( i = 0; i <= l; i++ )
    {
      if ( SP.sw[i] < fn )
      {
        SP.indices[k] = SP.sw[i];
        cc_formula ( k+1, l-i );
      }
    }
  }
  return;
}
/******************************************************************************/

double cc_eval ( int k )

/******************************************************************************/
/*
  Purpose:

    CC_EVAL calculates the value of a product formula.

  Discussion:

    USUAL:
      SP.summe = SP.summe + cc_calccoeff2(0,q-d) * cc_fsum(0);
    TREE (recommended):
      SP.summe = SP.summe + cc_calccoeff(q-d) * cc_fsum(0);
    all coefficients 1 (speed test):
      SP.summe = SP.summe +     cc_fsum(0);

  Modified:

    25 April 2007

  Author:

    Knut Petras

  Parameters:

    Input, int K, ???

    Output, double CC_EVAL, the value of the product formula.
*/
{
  int i;

  if ( k == 0 )
  {
    SP.summe = 0.0;
    cc_eval ( 1 );
  }
/*
  Summation corresponding to one coefficient.
*/
  else if ( k == SP.d + 1 )
  {
    SP.summe = SP.summe + cc_coeff ( ) * cc_fsum ( 0 );
  }
/*
  Choice of the SP.nodes.
*/
  else
  {
    for ( i = 0; i <= SP.n[SP.indices[k]]; i++ )
    {
      SP.argind[k] = i;
      cc_eval ( k+1 );
    }
  }
  return SP.summe;
}
/******************************************************************************/

double cc_coeff ( )

/******************************************************************************/
/*
  Purpose:

    CC_COEFF returns a coefficient.

  Discussion:

    The routine calculates the coefficient only if it determines that
    the value was not already computed and saved in the tree.

  Modified:

    25 April 2007

  Author:

    Knut Petras

  Parameters:

    Output, double CC_COEFF, the value of the coefficient.
*/
{
  int i;
  int j;
  int num;
  struct cc_tnode *p;
  struct cc_tnode *pt;

  p = cc_root;
/*
  Initialize.
*/
  num = -1;
/*
  Frequency SP.anzw[  ] of 1-dim SP.nodes
*/
  for ( i = 1; i <= SP.d; i++ )
  {
    if ( ++SP.anzw[SP.lookind[SP.indices[i]][SP.argind[i]]] == 1 )
    {
      SP.nodes[++num] = SP.lookind[SP.indices[i]][SP.argind[i]];
    }
  }
  qsort ( SP.nodes, num+1, sizeof(int), less );
/*
  Search in the tree according to SP.anzw[..]
*/
  for ( j = num; 0 <= j; j-- )
  {
/*
  SP.nodes[j] to the LEFT
  Generate the node if it does not exist.
*/
    if ( p->left == NULL )
    {
      p->left = (struct cc_tnode *) calloc ( SP.maxind, sizeof(struct cc_tnode) );
      pt = (p->left+SP.nodes[j]);
      pt->left = NULL;
      pt->right = NULL;
      p = pt;
    }
    else
    {
      p = ( p->left+SP.nodes[j] );
    }
/*
  SP.anzw[SP.nodes[j]] to the RIGHT
*/
    if ( p->right == NULL )
    {
      p->right = (struct cc_tnode *) calloc ( SP.d+1, sizeof(struct cc_tnode) );
      pt = ( p->right+SP.anzw[SP.nodes[j]] );
      pt->left = NULL;
      pt->right = NULL;
      p = pt;
    }
    else
    {
      p = ( p->right+SP.anzw[SP.nodes[j]] );
    }
    SP.anzw[SP.nodes[j]]=0;
  }
/*
  The coefficient must be calculated.
*/
  if ( !( p->computed ) )
  {
    ( p->coeff ) = cc_calccoeff ( SP.q - SP.d );
    ( p->computed ) = 1;
  }

  return ( p->coeff );
}
/******************************************************************************/

void cc_frei ( struct cc_tnode *p )

/******************************************************************************/
/*
  Purpose:

    CC_FREI frees the memory allocated for the tree structure.

  Modified:

    25 April 2007

  Author:

    Knut Petras

  Parameters:

    Input, struct tnode *P, a pointer to the tree structure to be freed.
*/
{
  int i;

  if ( !( p->left == NULL ) )
  {
    for ( i = 0; i < SP.maxind; i++ )
    {
      if ( !( p->left+i == NULL ) )
      {
        cc_frei ( p->left + i );
      }
    }
    free ( p->left );
  }

  if ( !( p->right == NULL ) )
  {
    for ( i = 0; i <= SP.d; i++ )
    {
      if ( !( p->left+i == NULL ) )
      {
        cc_frei ( p->right + i );
      }
    }
    free ( p->right );
  }
}
/******************************************************************************/

struct cc_tnode *cc_talloc ( void )

/******************************************************************************/
/*
  Purpose:

    CC_TALLOC allocates space for a new tree-node.

  Modified:

    25 April 2007

  Author:

    Knut Petras

  Parameters:

    Output, struct tnode *CC_TALLOC, a pointer to the new tree-node space.
*/
{
  return (struct cc_tnode *) malloc (sizeof(struct cc_tnode));
}
/******************************************************************************/

int less ( const void *a, const void *b )

/******************************************************************************/
/*
  Purpose:

    LESS is an integer comparison function needed by QSORT.

  Modified:

    25 April 2007

  Author:

    Knut Petras
*/
{
  return ( *(int*)a - *(int*)b );
}
/******************************************************************************/

void cc_sumind ( int r, int s )

/******************************************************************************/
/*
  Purpose:

    CC_SUMIND sums formula SP.indices at divisions of dimension R through S.

  Modified:

    25 April 2007

  Author:

    Knut Petras

  Parameters:

    Input, int R, S, the range over which the summation takes place.
*/
{
  int q;
/*
  R == S, one dimensional.
  Do the calculation.
*/
  if ( r == s )
  {
    SP.indsum[r][s] = SP.ninv[SP.indices[r]];
  }
/*
  R < S, compute average Q, split to [R,Q] + [Q+1,S],
  and call CC_SUMIND recursively.
*/
  else
  {
    q = ( r + s ) / 2;
    cc_sumind ( r, q );
    cc_sumind ( q+1, s );
    SP.indsum[r][s] = SP.indsum[r][q] + SP.indsum[q+1][s];
  }
  return;
}
/******************************************************************************/

double cc_calccoeff (int l)

/******************************************************************************/
/*
  Purpose:

    CC_CALCCOEFF calculates coefficients.

  Modified:

    25 April 2007

  Author:

    Knut Petras

  Parameters:

    Input, int L, ?

    Output, double CC_CALCCOEFF, the value of the coefficient.
*/
{
  double value;

  SP.wcount++;
/*
  Calculate the subdivision parameters.
*/
  cc_sumind ( 1, SP.d );
/*
  Start the divide and conquer process.
*/
  value = cc_wl ( 1, SP.d, l );

  return value;
}
/******************************************************************************/

double cc_wl ( int r, int s, int l )

/******************************************************************************/
/*
  Purpose:

    CC_WL sums in dimensions S through R with sum of formula numbers <= L.

  Modified:

    25 April 2007

  Author:

    Knut Petras

  Parameters:

    Input, int R, S, specify the range.

    Output, double CC_WL, ?
*/
{
  int i;
  int p;
  int q;
  double total;

  total = 0.0;
/*
  R = S, one dimensional.
  Do the calculation.
*/
  if ( r == s )
  {
    p = SP.lookind[SP.indices[r]][SP.argind[r]];

    for ( i = SP.ninv[SP.indices[r]]; i <= l; i++ )
    {
      if ( SP.sw[i] < fn )
      {
        if ( i == 0 )
        {
          total = total + SP.dnu[0][0];
        }
        else
        {
          if ( SP.indices[r] == 0 )
          {
            total = total + SP.dnu[SP.sw[i]][0];
          }
          else
          {
            total = total + SP.dnu[SP.sw[i]][SP.invlook[SP.sw[i]][p]];
          }
        }
      }
    }
  }
/*
  R < S, compute average Q, split to [R,Q] + [Q+1,S]
  and call CC_WE and CC_WL recursively.
*/
  else
  {
    q = ( r + s ) / 2;
    for ( i = SP.indsum[r][q]; i <= l-SP.indsum[q+1][s]; i++ )
    {
      total = total + cc_we ( r, q, i ) * cc_wl ( q+1, s, l-i );
    }
  }
  return total;
}
/******************************************************************************/

double cc_we ( int r, int s, int l )

/******************************************************************************/
/*
  Purpose:

    CC_WE sums in dimension S...R with sum of formula numbers <= L.

  Modified:

    25 April 2007

  Author:

    Knut Petras

  Parameters:

    Input, int R, S, specify the range.

    Output, double CC_WE, ?
*/
{
  int i;
  int q;
  double total;

  total = 0.0;
/*
  R = S, one dimensional.
  Do the calculation.
*/
  if ( r == s )
  {
    if ( SP.sw[l] < fn )
    {
      if (SP.sw[l] == 0 )
      {
        total = SP.dnu[0][0];
      }
      else
      {
        if ( SP.indices[r] == 0 )
        {
          total = SP.dnu[SP.sw[l]][0];
        }
        else
        {
          total = SP.dnu[SP.sw[l]][SP.invlook[SP.sw[l]][SP.lookind[SP.indices[r]][SP.argind[r]]]];
        }
      }
    }
  }
/*
  R < S, compute average Q, split to [R,Q] + [Q+1,S] and call CC_WE recursively.
*/
  else
  {
    q = ( r + s ) / 2;
    for ( i = SP.indsum[r][q]; i <= l-SP.indsum[q+1][s]; i++ )
    {
      total = total + cc_we ( r, q, i ) * cc_we ( q+1, s, l-i );
    }
  }
  return total;
}
/******************************************************************************/

double cc_calccoeff2 ( int k, int l )

/******************************************************************************/
/*
  Purpose:

    CC_CALCCOEFF2 calculates coefficients.

  Discussion:

     This is the "USUAL" method, which is slow.

  Modified:

    25 April 2007

  Author:

    Knut Petras

  Parameters:

    Input, int K, ?

    Input, int L, ?

    Output, double CC_CALCCOEFF2, ?
*/
{
  int i;
  double wprod;

  if ( k == 0 )
  {
    SP.wcount++;
    SP.wsum = 0.0;
    cc_calccoeff2 ( 1, l );
  }
  else if ( k == SP.d + 1 )
  {
    wprod = 1.0;
    for ( i = 1; i <= SP.d; i++ )
    {
      if ( SP.indices[i] == 0 )
      {
        wprod = wprod * SP.dnu[SP.wind[i]][0];
      }
      else
      {
        wprod = wprod *
          SP.dnu[SP.wind[i]][SP.invlook[SP.wind[i]][SP.lookind[SP.indices[i]][SP.argind[i]]]];
      }
    }
    SP.wsum = SP.wsum + wprod;
  }
  else
  {
    i = SP.indices[k];
    while ( SP.ninv[i] <= l )
    {
      SP.wind[k] = i;
      cc_calccoeff2 ( k+1, l-SP.ninv[i] );
      i++;
    }
  }
  return SP.wsum;
}
/******************************************************************************/

double cc_fsum ( int k )

/******************************************************************************/
/*
  Purpose:

    CC_FSUM computes unweighted sums of function values: sum(f(+/- x_i) ).

  Discussion:

    The function is evaluated at SP.nodes for which the same weight applies.

  Modified:

    26 April 2007

  Author:

    Knut Petras

  Parameters:

    Input, int K, ?

    Output, double CC_FSUM, the sum of the function at several SP.nodes.
*/
{
  if ( k == 0 )
  {
    SP.ftotal = 0.0;
    cc_fsum ( 1 );
  }
  else if ( k == SP.d + 1 )
  {
    SP.ftotal = SP.ftotal + (*SP.f) ( SP.x_ab, SP.d, SP.cubparam );
  }
  else
  {
    if ( SP.indices[k] == 0 )
    {
      SP.x[k-1] = 0.5;
      SP.x_ab[k-1] = SP.cubmin[k-1] + (SP.cubmax[k-1]-SP.cubmin[k-1]) * SP.x[k-1];
      cc_fsum ( k+1 );
    }
/*
  Use symmetry to get both X and -X.
*/
    else
    {
      SP.x[k-1] = SP.xnu[SP.indices[k]][2*SP.argind[k]+1];
      SP.x_ab[k-1] = SP.cubmin[k-1] + (SP.cubmax[k-1]-SP.cubmin[k-1]) * SP.x[k-1];
      cc_fsum ( k+1 );
      SP.x[k-1] = 1.0 - SP.x[k-1];
      SP.x_ab[k-1] = SP.cubmin[k-1] + (SP.cubmax[k-1]-SP.cubmin[k-1]) * SP.x[k-1];
      cc_fsum ( k+1 );
    }
  }
  return SP.ftotal;
}
/******************************************************************************/

void cc_init ( int dim )

/******************************************************************************/
/*
  Purpose:

    CC_INIT does initialization.

  Modified:

    25 April 2007

  Author:

    Knut Petras

  Parameters:

    Input, int DIM, the spatial dimension.

  Local Parameters:

    Local, int NJ[FN], the number of SP.nodes in the basic formulas.

    Local, int FREQ[FN], the frequencies of the basis formulas.
*/
{
  int formfakt;
  int freq[fn] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  int i;
  int j;
  int maxform;
  int nj[fn] = { 1, 3, 5, 9, 17, 33, 65, 129, 257, 513, 1025, 2049 };
/*
  Some parameter calculations.
*/
  SP.n[0] = 0;
  j = 0;
  SP.ninv[0] = 0;

  for ( i = 1; i < fn; i++ )
  {
    SP.n[i] = ( nj[i] - nj[i-1] - 2 ) / 2;
    j = j + freq[i-1];
    SP.ninv[i] = j;
  }

  for ( i = 0; i < gesfn; i++ )
  {
    SP.sw[i] = fn;
  }

  for ( i = 0; i < fn; i++ )
  {
    SP.sw[SP.ninv[i]] = i;
  }
/*
  Number of the 'largest' used formula.
*/
  i = SP.q - dim;
  maxform = 0;
  while ( SP.ninv[maxform+1] <= i )
  {
    maxform = maxform + 1;
  }
/*
  Total number of used 1-dimensional SP.nodes.
*/
   SP.maxind = ( nj[maxform] + 1 ) / 2;
/*
  Table of 1-dim nodal numbers 0..SP.maxind-1 corresponding
  to a combination formula number/nodal number
  and inverse formula.
*/
  SP.lookind[0][0] = 0;
  if (maxform >= fn) {
    printf("maxform(%d) >= fn\n",maxform);
  }
  for ( i = 1; i <= maxform; i++ )
  {
    formfakt = pow ( 2, maxform-i );
    for ( j = 0; j < (nj[i]+1)/4; j++ )
    {
      SP.lookind[i][j] = formfakt * ( 2 * j + 1 );
    }
/*
  In a linear ordering of all used SP.nodes, the (2J+1)-th
  node of the I-th basic formula is SP.lookind[I][J]-th node.
*/
    for ( j = 0; j < (nj[i]+1)/2; j++ )
    {
      SP.invlook[i][formfakt*j] = j;
    }
/*
  The SP.lookind[I][2^(MAXFORM-I)]-th node in a linear
  ordering of all used SP.nodes is the J-th node of the
  I-th basic formula. Note that MAXFORM is the number
  of used different basic formulas.
*/
  }

  for ( i = 0; i < SP.maxind; i++ )
  {
    SP.anzw[i] = 0;
  }
/*
  Root of the coefficient TREE.
*/
  cc_root = cc_talloc ( );
  cc_root->left = NULL;
  cc_root->right = NULL;
/*
  One dimensional formulas (Deltas).
*/
  SP.xnu[0][0] = 0.5;
  SP.dnu[0][0] = 1.0;

  SP.xnu[1][0] =  5.0000000000000000000000E-001;
  SP.dnu[1][0] = -3.3333333333333333333333E-001;
  SP.xnu[1][1] =  1.0000000000000000000000E+000;
  SP.dnu[1][1] =  1.6666666666666666666667E-001;

  SP.xnu[2][0] =  5.0000000000000000000000E-001;
  SP.dnu[2][0] = -2.6666666666666666666667E-001;
  SP.xnu[2][1] =  8.5355339059327376220042E-001;
  SP.dnu[2][1] =  2.6666666666666666666667E-001;
  SP.xnu[2][2] =  1.0000000000000000000000E+000;
  SP.dnu[2][2] = -1.3333333333333333333333E-001;

  SP.xnu[3][0] =  5.0000000000000000000000E-001;
  SP.dnu[3][0] = -2.0317460317460317460317E-001;
  SP.xnu[3][1] =  6.9134171618254488586423E-001;
  SP.dnu[3][1] =  1.8085892936024489074800E-001;
  SP.xnu[3][2] =  8.5355339059327376220042E-001;
  SP.dnu[3][2] = -1.2698412698412698412698E-001;
  SP.xnu[3][3] =  9.6193976625564337806409E-001;
  SP.dnu[3][3] =  7.3109324608009077505967E-002;
  SP.xnu[3][4] =  1.0000000000000000000000E+000;
  SP.dnu[3][4] = -2.5396825396825396825397E-002;

  SP.xnu[4][0] =  5.0000000000000000000000E-001;
  SP.dnu[4][0] = -9.8620333914451561510385E-002;
  SP.xnu[4][1] =  5.9754516100806413392414E-001;
  SP.dnu[4][1] =  9.6256932306462823434815E-002;
  SP.xnu[4][2] =  6.9134171618254488586423E-001;
  SP.dnu[4][2] = -9.0122037241998212249758E-002;
  SP.xnu[4][3] =  7.7778511650980111237142E-001;
  SP.dnu[4][3] =  8.1586332140851651280960E-002;
  SP.xnu[4][4] =  8.5355339059327376220042E-001;
  SP.dnu[4][4] = -7.0204305498423145481969E-002;
  SP.xnu[4][5] =  9.1573480615127261853939E-001;
  SP.dnu[4][5] =  5.4452776290945465221825E-002;
  SP.xnu[4][6] =  9.6193976625564337806409E-001;
  SP.dnu[4][6] = -3.5368158836433160299261E-002;
  SP.xnu[4][7] =  9.9039264020161522456309E-001;
  SP.dnu[4][7] =  1.8684351418602805160439E-002;
  SP.xnu[4][8] =  1.0000000000000000000000E+000;
  SP.dnu[4][8] = -5.9757236227824463118581E-003;

  SP.xnu[5][0] =  5.0000000000000000000000E-001;
  SP.dnu[5][0] = -4.9115774020061115502709E-002;
  SP.xnu[5][1] =  5.4900857016478030099710E-001;
  SP.dnu[5][1] =  4.8849094104027790909994E-002;
  SP.xnu[5][2] =  5.9754516100806413392414E-001;
  SP.dnu[5][2] = -4.8110769333718733838254E-002;
  SP.xnu[5][3] =  6.4514233862723118381810E-001;
  SP.dnu[5][3] =  4.6971622219384367864630E-002;
  SP.xnu[5][4] =  6.9134171618254488586423E-001;
  SP.dnu[5][4] = -4.5383835684386179129782E-002;
  SP.xnu[5][5] =  7.3569836841299882427819E-001;
  SP.dnu[5][5] =  4.3288769220913717719321E-002;
  SP.xnu[5][6] =  7.7778511650980111237142E-001;
  SP.dnu[5][6] = -4.0768923313382396166555E-002;
  SP.xnu[5][7] =  8.1719664208182274910759E-001;
  SP.dnu[5][7] =  3.7941900220694235239865E-002;
  SP.xnu[5][8] =  8.5355339059327376220042E-001;
  SP.dnu[5][8] = -3.4764446368898811607848E-002;
  SP.xnu[5][9] =  8.8650522668136848040545E-001;
  SP.dnu[5][9] =  3.1136054772647002276480E-002;
  SP.xnu[5][10] =  9.1573480615127261853939E-001;
  SP.dnu[5][10] = -2.7175268138955310002953E-002;
  SP.xnu[5][11] =  9.4096063217417751485638E-001;
  SP.dnu[5][11] =  2.3131381418875874745787E-002;
  SP.xnu[5][12] =  9.6193976625564337806409E-001;
  SP.dnu[5][12] = -1.8943994814552314198613E-002;
  SP.xnu[5][13] =  9.7847016786610443246790E-001;
  SP.dnu[5][13] =  1.4228958338616845046813E-002;
  SP.xnu[5][14] =  9.9039264020161522456309E-001;
  SP.dnu[5][14] = -9.0672288522622305689737E-003;
  SP.xnu[5][15] =  9.9759236333609844312242E-001;
  SP.dnu[5][15] =  4.6965989814775073505799E-003;
  SP.xnu[5][16] =  1.0000000000000000000000E+000;
  SP.dnu[5][16] = -1.4720257604508078891381E-003;

  SP.xnu[6][0] =  5.0000000000000000000000E-001;
  SP.dnu[6][0] = -2.4545477133412920459841E-002;
  SP.xnu[6][1] =  5.2453383716370900712748E-001;
  SP.dnu[6][1] =  2.4514009215512776470302E-002;
  SP.xnu[6][2] =  5.4900857016478030099710E-001;
  SP.dnu[6][2] = -2.4423465782494744055039E-002;
  SP.xnu[6][3] =  5.7336523722768087582943E-001;
  SP.dnu[6][3] =  2.4277922428570526368567E-002;
  SP.xnu[6][4] =  5.9754516100806413392414E-001;
  SP.dnu[6][4] = -2.4073946686487987891549E-002;
  SP.xnu[6][5] =  6.2149008995163194497414E-001;
  SP.dnu[6][5] =  2.3808022292625096480201E-002;
  SP.xnu[6][6] =  6.4514233862723118381810E-001;
  SP.dnu[6][6] = -2.3484642696077293512203E-002;
  SP.xnu[6][7] =  6.6844492669611002534463E-001;
  SP.dnu[6][7] =  2.3108833755462788420151E-002;
  SP.xnu[6][8] =  6.9134171618254488586423E-001;
  SP.dnu[6][8] = -2.2677501658030163263296E-002;
  SP.xnu[6][9] =  7.1377754671514104716048E-001;
  SP.dnu[6][9] =  2.2187089619628657897993E-002;
  SP.xnu[6][10] =  7.3569836841299882427819E-001;
  SP.dnu[6][10] = -2.1643011740068303045395E-002;
  SP.xnu[6][11] =  7.5705137209661086329685E-001;
  SP.dnu[6][11] =  2.1051665555709051014101E-002;
  SP.xnu[6][12] =  7.7778511650980111237142E-001;
  SP.dnu[6][12] = -2.0409902127290338196635E-002;
  SP.xnu[6][13] =  7.9784965224621667173352E-001;
  SP.dnu[6][13] =  1.9713494356478049878254E-002;
  SP.xnu[6][14] =  8.1719664208182274910759E-001;
  SP.dnu[6][14] = -1.8969170260051826682893E-002;
  SP.xnu[6][15] =  8.3577947742350920031269E-001;
  SP.dnu[6][15] =  1.8185460143319591545877E-002;
  SP.xnu[6][16] =  8.5355339059327376220042E-001;
  SP.dnu[6][16] = -1.7358538724755168314629E-002;
  SP.xnu[6][17] =  8.7047556267747954558781E-001;
  SP.dnu[6][17] =  1.6482273284988164986153E-002;
  SP.xnu[6][18] =  8.8650522668136848040545E-001;
  SP.dnu[6][18] = -1.5565406220613190039060E-002;
  SP.xnu[6][19] =  9.0160376574032245490334E-001;
  SP.dnu[6][19] =  1.4620326598734168847759E-002;
  SP.xnu[6][20] =  9.1573480615127261853939E-001;
  SP.dnu[6][20] = -1.3641379581255961786964E-002;
  SP.xnu[6][21] =  9.2886430500013603495113E-001;
  SP.dnu[6][21] =  1.2617532490877382950568E-002;
  SP.xnu[6][22] =  9.4096063217417751485638E-001;
  SP.dnu[6][22] = -1.1561033951696775507141E-002;
  SP.xnu[6][23] =  9.5199464656172166579310E-001;
  SP.dnu[6][23] =  1.0493137214868716890627E-002;
  SP.xnu[6][24] =  9.6193976625564337806409E-001;
  SP.dnu[6][24] = -9.4039060861257112372603E-003;
  SP.xnu[6][25] =  9.7077203259151038920625E-001;
  SP.dnu[6][25] =  8.2674938286447948244324E-003;
  SP.xnu[6][26] =  9.7847016786610443246790E-001;
  SP.dnu[6][26] = -7.1029281224408466540478E-003;
  SP.xnu[6][27] =  9.8501562659727199630199E-001;
  SP.dnu[6][27] =  5.9616973571063858014202E-003;
  SP.xnu[6][28] =  9.9039264020161522456309E-001;
  SP.dnu[6][28] = -4.8259531686986793979595E-003;
  SP.xnu[6][29] =  9.9458825498239048672584E-001;
  SP.dnu[6][29] =  3.5963465808680570124707E-003;
  SP.xnu[6][30] =  9.9759236333609844312242E-001;
  SP.dnu[6][30] = -2.2808662570820510292518E-003;
  SP.xnu[6][31] =  9.9939772810258619635739E-001;
  SP.dnu[6][31] =  1.1757453376558516611831E-003;
  SP.xnu[6][32] =  1.0000000000000000000000E+000;
  SP.dnu[6][32] = -3.6665843117456020681827E-004;

  SP.xnu[7][0] =  5.0000000000000000000000E-001;
  SP.dnu[7][0] = -1.2271958004714136610937E-002;
  SP.xnu[7][1] =  5.1227061426145614401587E-001;
  SP.dnu[7][1] =  1.2268142798257477363976E-002;
  SP.xnu[7][2] =  5.2453383716370900712748E-001;
  SP.dnu[7][2] = -1.2256937421104935008784E-002;
  SP.xnu[7][3] =  5.3678228179983371176473E-001;
  SP.dnu[7][3] =  1.2238587713717221419827E-002;
  SP.xnu[7][4] =  5.4900857016478030099710E-001;
  SP.dnu[7][4] = -1.2212866790749503406615E-002;
  SP.xnu[7][5] =  5.6120533759960809924935E-001;
  SP.dnu[7][5] =  1.2179548744638215921839E-002;
  SP.xnu[7][6] =  5.7336523722768087582943E-001;
  SP.dnu[7][6] = -1.2138892718191796751179E-002;
  SP.xnu[7][7] =  5.8548094438015061318182E-001;
  SP.dnu[7][7] =  1.2091168119465737833822E-002;
  SP.xnu[7][8] =  5.9754516100806413392414E-001;
  SP.dnu[7][8] = -1.2036162324616851706718E-002;
  SP.xnu[7][9] =  6.0955062007843489861387E-001;
  SP.dnu[7][9] =  1.1973658752384507510477E-002;
  SP.xnu[7][10] =  6.2149008995163194497414E-001;
  SP.dnu[7][10] = -1.1903939927503801607263E-002;
  SP.xnu[7][11] =  6.3335637873744919316264E-001;
  SP.dnu[7][11] =  1.1827303730288832613218E-002;
  SP.xnu[7][12] =  6.4514233862723118381810E-001;
  SP.dnu[7][12] = -1.1743546667341256825678E-002;
  SP.xnu[7][13] =  6.5684087019944573832824E-001;
  SP.dnu[7][13] =  1.1652455630655716365471E-002;
  SP.xnu[7][14] =  6.6844492669611002534463E-001;
  SP.dnu[7][14] = -1.1554341297325752582504E-002;
  SP.xnu[7][15] =  6.7994751826749407438755E-001;
  SP.dnu[7][15] =  1.1449535671953029410590E-002;
  SP.xnu[7][16] =  6.9134171618254488586423E-001;
  SP.dnu[7][16] = -1.1337838423546977517159E-002;
  SP.xnu[7][17] =  7.0262065700249493545424E-001;
  SP.dnu[7][17] =  1.1219032698613150919920E-002;
  SP.xnu[7][18] =  7.1377754671514104716048E-001;
  SP.dnu[7][18] = -1.1093462840142682459267E-002;
  SP.xnu[7][19] =  7.2480566482730330002315E-001;
  SP.dnu[7][19] =  1.0961502002993784460127E-002;
  SP.xnu[7][20] =  7.3569836841299882427819E-001;
  SP.dnu[7][20] = -1.0822945697281002471771E-002;
  SP.xnu[7][21] =  7.4644909611489201843651E-001;
  SP.dnu[7][21] =  1.0677563987129853626319E-002;
  SP.xnu[7][22] =  7.5705137209661086329685E-001;
  SP.dnu[7][22] = -1.0525741760698995357821E-002;
  SP.xnu[7][23] =  7.6749880994354860533154E-001;
  SP.dnu[7][23] =  1.0367902667450739078364E-002;
  SP.xnu[7][24] =  7.7778511650980111237142E-001;
  SP.dnu[7][24] = -1.0203828640159577603940E-002;
  SP.xnu[7][25] =  7.8790409570892265037299E-001;
  SP.dnu[7][25] =  1.0033264025991788019882E-002;
  SP.xnu[7][26] =  7.9784965224621667173352E-001;
  SP.dnu[7][26] = -9.8566434379772712552330E-003;
  SP.xnu[7][27] =  8.0761579529031342274246E-001;
  SP.dnu[7][27] =  9.6744542119622592206553E-003;
  SP.xnu[7][28] =  8.1719664208182274910759E-001;
  SP.dnu[7][28] = -9.4864520253026664274132E-003;
  SP.xnu[7][29] =  8.2658642147688838204210E-001;
  SP.dnu[7][29] =  9.2923375978345433069219E-003;
  SP.xnu[7][30] =  8.3577947742350920031269E-001;
  SP.dnu[7][30] = -9.0926082536878941429147E-003;
  SP.xnu[7][31] =  8.4477027236853346230837E-001;
  SP.dnu[7][31] =  8.8878346943763999873851E-003;
  SP.xnu[7][32] =  8.5355339059327376220042E-001;
  SP.dnu[7][32] = -8.6777284634055823453972E-003;
  SP.xnu[7][33] =  8.6212354147573346047053E-001;
  SP.dnu[7][33] =  8.4619199292324968376878E-003;
  SP.xnu[7][34] =  8.7047556267747954558781E-001;
  SP.dnu[7][34] = -8.2409885038862754424820E-003;
  SP.xnu[7][35] =  8.7860442325324227378773E-001;
  SP.dnu[7][35] =  8.0156192937252895789335E-003;
  SP.xnu[7][36] =  8.8650522668136848040545E-001;
  SP.dnu[7][36] = -7.7854531825366824361481E-003;
  SP.xnu[7][37] =  8.9417321381330313100458E-001;
  SP.dnu[7][37] =  7.5500078623963339131136E-003;
  SP.xnu[7][38] =  9.0160376574032245490334E-001;
  SP.dnu[7][38] = -7.3099753255624178213370E-003;
  SP.xnu[7][39] =  9.0879240657579184825246E-001;
  SP.dnu[7][39] =  7.0662071892654706670154E-003;
  SP.xnu[7][40] =  9.1573480615127261853939E-001;
  SP.dnu[7][40] = -6.8182319642653331986034E-003;
  SP.xnu[7][41] =  9.2242678262485353662979E-001;
  SP.dnu[7][41] =  6.5653825834698731492124E-003;
  SP.xnu[7][42] =  9.2886430500013603495113E-001;
  SP.dnu[7][42] = -6.3085145009886665940009E-003;
  SP.xnu[7][43] =  9.3504349555435570932615E-001;
  SP.dnu[7][43] =  6.0487402640358205635282E-003;
  SP.xnu[7][44] =  9.4096063217417751485638E-001;
  SP.dnu[7][44] = -5.7854057257248494013661E-003;
  SP.xnu[7][45] =  9.4661215059775766017121E-001;
  SP.dnu[7][45] =  5.5175243871362709176984E-003;
  SP.xnu[7][46] =  9.5199464656172166579310E-001;
  SP.dnu[7][46] = -5.2462062019842562676748E-003;
  SP.xnu[7][47] =  9.5710487785176532731751E-001;
  SP.dnu[7][47] =  4.9730139246172895270352E-003;
  SP.xnu[7][48] =  9.6193976625564337806409E-001;
  SP.dnu[7][48] = -4.6969819512238174178555E-003;
  SP.xnu[7][49] =  9.6649639941736944385583E-001;
  SP.dnu[7][49] =  4.4165193373506679031807E-003;
  SP.xnu[7][50] =  9.7077203259151038920625E-001;
  SP.dnu[7][50] = -4.1331680576293592829934E-003;
  SP.xnu[7][51] =  9.7476409029651833359797E-001;
  SP.dnu[7][51] =  3.8493788944804140534721E-003;
  SP.xnu[7][52] =  9.7847016786610443246790E-001;
  SP.dnu[7][52] = -3.5636135545485494677848E-003;
  SP.xnu[7][53] =  9.8188803289771993334323E-001;
  SP.dnu[7][53] =  3.2729542193114946421542E-003;
  SP.xnu[7][54] =  9.8501562659727199630199E-001;
  SP.dnu[7][54] = -2.9797571834523881954933E-003;
  SP.xnu[7][55] =  9.8785106501926427223020E-001;
  SP.dnu[7][55] =  2.6886237342009231071956E-003;
  SP.xnu[7][56] =  9.9039264020161522456309E-001;
  SP.dnu[7][56] = -2.3968586809352113803092E-003;
  SP.xnu[7][57] =  9.9263882119447062238701E-001;
  SP.dnu[7][57] =  2.0977689935926733752529E-003;
  SP.xnu[7][58] =  9.9458825498239048672584E-001;
  SP.dnu[7][58] = -1.7953574018049862762798E-003;
  SP.xnu[7][59] =  9.9623976729935499907838E-001;
  SP.dnu[7][59] =  1.5017293495224856396994E-003;
  SP.xnu[7][60] =  9.9759236333609844312242E-001;
  SP.dnu[7][60] = -1.2121560850097517476459E-003;
  SP.xnu[7][61] =  9.9864522833934510806780E-001;
  SP.dnu[7][61] =  9.0154063347681179779507E-004;
  SP.xnu[7][62] =  9.9939772810258619635739E-001;
  SP.dnu[7][62] = -5.7109502827948170677293E-004;
  SP.xnu[7][63] =  9.9984940934810211005788E-001;
  SP.dnu[7][63] =  2.9403607691434877222928E-004;
  SP.xnu[7][64] =  1.0000000000000000000000E+000;
  SP.dnu[7][64] = -9.1580681216279092126007E-005;

  SP.xnu[8][0] =  5.0000000000000000000000E-001;
  SP.dnu[8][0] = -6.1359301355674465811749E-003;
  SP.xnu[8][1] =  5.0613576914285996303970E-001;
  SP.dnu[8][1] =  6.1354606620969036989772E-003;
  SP.xnu[8][2] =  5.1227061426145614401587E-001;
  SP.dnu[8][2] = -6.1340672061220411707548E-003;
  SP.xnu[8][3] =  5.1840361147067941616217E-001;
  SP.dnu[8][3] =  6.1317648891717112686340E-003;
  SP.xnu[8][4] =  5.2453383716370900712748E-001;
  SP.dnu[8][4] = -6.1285391642523178638978E-003;
  SP.xnu[8][5] =  5.3066036815110428889131E-001;
  SP.dnu[8][5] =  6.1243755695136940751512E-003;
  SP.xnu[8][6] =  5.3678228179983371176473E-001;
  SP.dnu[8][6] = -6.1192896435897003484674E-003;
  SP.xnu[8][7] =  5.4289865617221994523078E-001;
  SP.dnu[8][7] =  6.1132971541665829342525E-003;
  SP.xnu[8][8] =  5.4900857016478030099710E-001;
  SP.dnu[8][8] = -6.1063840561707702500148E-003;
  SP.xnu[8][9] =  5.5511110364694152940395E-001;
  SP.dnu[8][9] =  6.0985363163442700350171E-003;
  SP.xnu[8][10] =  5.6120533759960809924935E-001;
  SP.dnu[8][10] = -6.0897701181324650443179E-003;
  SP.xnu[8][11] =  5.6729035425356309315818E-001;
  SP.dnu[8][11] =  6.0801019474110239327653E-003;
  SP.xnu[8][12] =  5.7336523722768087582943E-001;
  SP.dnu[8][12] = -6.0695181860326963497934E-003;
  SP.xnu[8][13] =  5.7942907166693072084219E-001;
  SP.dnu[8][13] =  6.0580051515255410968758E-003;
  SP.xnu[8][14] =  5.8548094438015061318182E-001;
  SP.dnu[8][14] = -6.0455797431700362572977E-003;
  SP.xnu[8][15] =  5.9151994397757047925827E-001;
  SP.dnu[8][15] =  6.0322592389520580622363E-003;
  SP.xnu[8][16] =  5.9754516100806413392414E-001;
  SP.dnu[8][16] = -6.0180303688534750168532E-003;
  SP.xnu[8][17] =  6.0355568809610927485406E-001;
  SP.dnu[8][17] =  6.0028797180425504929793E-003;
  SP.xnu[8][18] =  6.0955062007843489861387E-001;
  SP.dnu[8][18] = -5.9868249745483861377796E-003;
  SP.xnu[8][19] =  6.1552905414033555982162E-001;
  SP.dnu[8][19] =  5.9698842858948452227527E-003;
  SP.xnu[8][20] =  6.2149008995163194497414E-001;
  SP.dnu[8][20] = -5.9520446460672194278797E-003;
  SP.xnu[8][21] =  6.2743282980225728577699E-001;
  SP.dnu[8][21] =  5.9332928176922680828567E-003;
  SP.xnu[8][22] =  6.3335637873744919316264E-001;
  SP.dnu[8][22] = -5.9136473539710391631393E-003;
  SP.xnu[8][23] =  6.3925984469252655260392E-001;
  SP.dnu[8][23] =  5.8931273547312435616220E-003;
  SP.xnu[8][24] =  6.4514233862723118381810E-001;
  SP.dnu[8][24] = -5.8717199868146503004829E-003;
  SP.xnu[8][25] =  6.5100297465961403350173E-001;
  SP.dnu[8][25] =  5.8494120911440515455896E-003;
  SP.xnu[8][26] =  6.5684087019944573832824E-001;
  SP.dnu[8][26] = -5.8262231678578068703499E-003;
  SP.xnu[8][27] =  6.6265514608113146706798E-001;
  SP.dnu[8][27] =  5.8021733593247310610614E-003;
  SP.xnu[8][28] =  6.6844492669611002534463E-001;
  SP.dnu[8][28] = -5.7772499051338284185200E-003;
  SP.xnu[8][29] =  6.7420934012471728420965E-001;
  SP.dnu[8][29] =  5.7514396140669005429112E-003;
  SP.xnu[8][30] =  6.7994751826749407438755E-001;
  SP.dnu[8][30] = -5.7247630224409077680699E-003;
  SP.xnu[8][31] =  6.8565859697591877170597E-001;
  SP.dnu[8][31] =  5.6972414154230381773451E-003;
  SP.xnu[8][32] =  6.9134171618254488586423E-001;
  SP.dnu[8][32] = -5.6688619939886755145709E-003;
  SP.xnu[8][33] =  6.9699602003052405429809E-001;
  SP.dnu[8][33] =  5.6396114102955337564541E-003;
  SP.xnu[8][34] =  7.0262065700249493545424E-001;
  SP.dnu[8][34] = -5.6095113361047417873575E-003;
  SP.xnu[8][35] =  7.0821478004881859128130E-001;
  SP.dnu[8][35] =  5.5785843127705846896864E-003;
  SP.xnu[8][36] =  7.1377754671514104716048E-001;
  SP.dnu[8][36] = -5.5468173772762507094602E-003;
  SP.xnu[8][37] =  7.1930811926926381882351E-001;
  SP.dnu[8][37] =  5.5141968832067627652703E-003;
  SP.xnu[8][38] =  7.2480566482730330002315E-001;
  SP.dnu[8][38] = -5.4807457501672539570134E-003;
  SP.xnu[8][39] =  7.3026935547912001181659E-001;
  SP.dnu[8][39] =  5.4464879060908243404913E-003;
  SP.xnu[8][40] =  7.3569836841299882427819E-001;
  SP.dnu[8][40] = -5.4114100811595006037953E-003;
  SP.xnu[8][41] =  7.4109188603956137425867E-001;
  SP.dnu[8][41] =  5.3754981666740742246332E-003;
  SP.xnu[8][42] =  7.4644909611489201843651E-001;
  SP.dnu[8][42] = -5.3387764594861294428642E-003;
  SP.xnu[8][43] =  7.5176919186285877934593E-001;
  SP.dnu[8][43] =  5.3012704264033219147411E-003;
  SP.xnu[8][44] =  7.5705137209661086329685E-001;
  SP.dnu[8][44] = -5.2629663262785217409418E-003;
  SP.xnu[8][45] =  7.6229484133923445310755E-001;
  SP.dnu[8][45] =  5.2238493971611872993393E-003;
  SP.xnu[8][46] =  7.6749880994354860533154E-001;
  SP.dnu[8][46] = -5.1839454644496285857595E-003;
  SP.xnu[8][47] =  7.7266249421102321115699E-001;
  SP.dnu[8][47] =  5.1432817143312206183013E-003;
  SP.xnu[8][48] =  7.7778511650980111237142E-001;
  SP.dnu[8][48] = -5.1018437426019339450149E-003;
  SP.xnu[8][49] =  7.8286590539180659869488E-001;
  SP.dnu[8][49] =  5.0596159087039973992432E-003;
  SP.xnu[8][50] =  7.8790409570892265037299E-001;
  SP.dnu[8][50] = -5.0166257460761322380136E-003;
  SP.xnu[8][51] =  7.9289892872821943016404E-001;
  SP.dnu[8][51] =  4.9729023772410321627246E-003;
  SP.xnu[8][52] =  7.9784965224621667173352E-001;
  SP.dnu[8][52] = -4.9284305088938107521096E-003;
  SP.xnu[8][53] =  8.0275552070216275696031E-001;
  SP.dnu[8][53] =  4.8831933527140235623190E-003;
  SP.xnu[8][54] =  8.0761579529031342274246E-001;
  SP.dnu[8][54] = -4.8372203660928237810457E-003;
  SP.xnu[8][55] =  8.1242974407119318854204E-001;
  SP.dnu[8][55] =  4.7905428722375966643298E-003;
  SP.xnu[8][56] =  8.1719664208182274910759E-001;
  SP.dnu[8][56] = -4.7431444189961922418201E-003;
  SP.xnu[8][57] =  8.2191577144489573253404E-001;
  SP.dnu[8][57] =  4.6950067447143432265896E-003;
  SP.xnu[8][58] =  8.2658642147688838204210E-001;
  SP.dnu[8][58] = -4.6461614939831908327966E-003;
  SP.xnu[8][59] =  8.3120788879508588055653E-001;
  SP.dnu[8][59] =  4.5966425172115754859451E-003;
  SP.xnu[8][60] =  8.3577947742350920031269E-001;
  SP.dnu[8][60] = -4.5464318773722228198026E-003;
  SP.xnu[8][61] =  8.4030049889772652529722E-001;
  SP.dnu[8][61] =  4.4955094402897996907120E-003;
  SP.xnu[8][62] =  8.4477027236853346230837E-001;
  SP.dnu[8][62] = -4.4439093630694917917486E-003;
  SP.xnu[8][63] =  8.4918812470448642677741E-001;
  SP.dnu[8][63] =  4.3916684323034778737840E-003;
  SP.xnu[8][64] =  8.5355339059327376220042E-001;
  SP.dnu[8][64] = -4.3387668266379229724763E-003;
  SP.xnu[8][65] =  8.5786541264190932706277E-001;
  SP.dnu[8][65] =  4.2851820426951740121900E-003;
  SP.xnu[8][66] =  8.6212354147573346047053E-001;
  SP.dnu[8][66] = -4.2309511577022037105246E-003;
  SP.xnu[8][67] =  8.6632713583620641730777E-001;
  SP.dnu[8][67] =  4.1761144143046653256344E-003;
  SP.xnu[8][68] =  8.7047556267747954558781E-001;
  SP.dnu[8][68] = -4.1206496101637617721417E-003;
  SP.xnu[8][69] =  8.7456819726172966273460E-001;
  SP.dnu[8][69] =  4.0645312447150292756975E-003;
  SP.xnu[8][70] =  8.7860442325324227378773E-001;
  SP.dnu[8][70] = -4.0077998335127022932478E-003;
  SP.xnu[8][71] =  8.8258363281122946294441E-001;
  SP.dnu[8][71] =  3.9504997466580752238050E-003;
  SP.xnu[8][72] =  8.8650522668136848040545E-001;
  SP.dnu[8][72] = -3.8926057733115525888929E-003;
  SP.xnu[8][73] =  8.9036861428604723915079E-001;
  SP.dnu[8][73] =  3.8340886075017528764544E-003;
  SP.xnu[8][74] =  8.9417321381330313100458E-001;
  SP.dnu[8][74] = -3.7749928723506968245152E-003;
  SP.xnu[8][75] =  8.9791845230444176813140E-001;
  SP.dnu[8][75] =  3.7153679478423029869624E-003;
  SP.xnu[8][76] =  9.0160376574032245490334E-001;
  SP.dnu[8][76] = -3.6551848076070826899373E-003;
  SP.xnu[8][77] =  9.0522859912629739586335E-001;
  SP.dnu[8][77] =  3.5944092792238683039007E-003;
  SP.xnu[8][78] =  9.0879240657579184825246E-001;
  SP.dnu[8][78] = -3.5330909727810748624502E-003;
  SP.xnu[8][79] =  9.1229465139251263223740E-001;
  SP.dnu[8][79] =  3.4712854610079782312676E-003;
  SP.xnu[8][80] =  9.1573480615127261853939E-001;
  SP.dnu[8][80] = -3.4089588433770333889584E-003;
  SP.xnu[8][81] =  9.1911235277741902159350E-001;
  SP.dnu[8][81] =  3.3460706564146434184577E-003;
  SP.xnu[8][82] =  9.2242678262485353662979E-001;
  SP.dnu[8][82] = -3.2826766754549820513849E-003;
  SP.xnu[8][83] =  9.2567759655263257113065E-001;
  SP.dnu[8][83] =  3.2188402877555681840392E-003;
  SP.xnu[8][84] =  9.2886430500013603495113E-001;
  SP.dnu[8][84] = -3.1545212986167095061169E-003;
  SP.xnu[8][85] =  9.3198642806079336895907E-001;
  SP.dnu[8][85] =  3.0896709908779296557536E-003;
  SP.xnu[8][86] =  9.3504349555435570932615E-001;
  SP.dnu[8][86] = -3.0243529194229427589409E-003;
  SP.xnu[8][87] =  9.3803504709770330354792E-001;
  SP.dnu[8][87] =  2.9586405688338363198131E-003;
  SP.xnu[8][88] =  9.4096063217417751485638E-001;
  SP.dnu[8][88] = -2.8924854962613047441444E-003;
  SP.xnu[8][89] =  9.4381981020142697388009E-001;
  SP.dnu[8][89] =  2.8258279447852385031887E-003;
  SP.xnu[8][90] =  9.4661215059775766017121E-001;
  SP.dnu[8][90] = -2.7587415185047335652973E-003;
  SP.xnu[8][91] =  9.4933723284697692152099E-001;
  SP.dnu[8][91] =  2.6913131141455107121406E-003;
  SP.xnu[8][92] =  9.5199464656172166579310E-001;
  SP.dnu[8][92] = -2.6234832713690129082713E-003;
  SP.xnu[8][93] =  9.5458399154526118828194E-001;
  SP.dnu[8][93] =  2.5551770959451047143772E-003;
  SP.xnu[8][94] =  9.5710487785176532731751E-001;
  SP.dnu[8][94] = -2.4864815311891625507520E-003;
  SP.xnu[8][95] =  9.5955692584502887195424E-001;
  SP.dnu[8][95] =  2.4175018833859955710666E-003;
  SP.xnu[8][96] =  9.6193976625564337806409E-001;
  SP.dnu[8][96] = -2.3481636110593708965163E-003;
  SP.xnu[8][97] =  9.6425304023660778296858E-001;
  SP.dnu[8][97] =  2.2783703935116757786991E-003;
  SP.xnu[8][98] =  9.6649639941736944385583E-001;
  SP.dnu[8][98] = -2.2082274599073300182729E-003;
  SP.xnu[8][99] =  9.6866950595628746160095E-001;
  SP.dnu[8][99] =  2.1378664158542930019558E-003;
  SP.xnu[8][100] =  9.7077203259151038920625E-001;
  SP.dnu[8][100] = -2.0671914231050476204775E-003;
  SP.xnu[8][101] =  9.7280366269026066286547E-001;
  SP.dnu[8][101] =  1.9960745598053936415904E-003;
  SP.xnu[8][102] =  9.7476409029651833359797E-001;
  SP.dnu[8][102] = -1.9246471168879591740743E-003;
  SP.xnu[8][103] =  9.7665302017709691845837E-001;
  SP.dnu[8][103] =  1.8530802002463219695554E-003;
  SP.xnu[8][104] =  9.7847016786610443246790E-001;
  SP.dnu[8][104] = -1.7812466748899198813882E-003;
  SP.xnu[8][105] =  9.8021525970778290559952E-001;
  SP.dnu[8][105] =  1.7089694205745448269297E-003;
  SP.xnu[8][106] =  9.8188803289771993334323E-001;
  SP.dnu[8][106] = -1.6364187055632937432244E-003;
  SP.xnu[8][107] =  9.8348823552242605454361E-001;
  SP.dnu[8][107] =  1.5638289514677474901184E-003;
  SP.xnu[8][108] =  9.8501562659727199630199E-001;
  SP.dnu[8][108] = -1.4910245982021604965549E-003;
  SP.xnu[8][109] =  9.8646997610278007273386E-001;
  SP.dnu[8][109] =  1.4177461020644323904505E-003;
  SP.xnu[8][110] =  9.8785106501926427223020E-001;
  SP.dnu[8][110] = -1.3442256971872101749416E-003;
  SP.xnu[8][111] =  9.8915868535981381655312E-001;
  SP.dnu[8][111] =  1.2708086765383113197422E-003;
  SP.xnu[8][112] =  9.9039264020161522456309E-001;
  SP.dnu[8][112] = -1.1972393146870275794749E-003;
  SP.xnu[8][113] =  9.9155274371560816359015E-001;
  SP.dnu[8][113] =  1.1231048601830500240179E-003;
  SP.xnu[8][114] =  9.9263882119447062238701E-001;
  SP.dnu[8][114] = -1.0487441461826293750209E-003;
  SP.xnu[8][115] =  9.9365070907892919119991E-001;
  SP.dnu[8][115] =  9.7472303595273357297118E-004;
  SP.xnu[8][116] =  9.9458825498239048672584E-001;
  SP.dnu[8][116] = -9.0064081213563870160763E-004;
  SP.xnu[8][117] =  9.9545131771389001255412E-001;
  SP.dnu[8][117] =  8.2575142478289506728989E-004;
  SP.xnu[8][118] =  9.9623976729935499907838E-001;
  SP.dnu[8][118] = -7.5059614131186328850771E-004;
  SP.xnu[8][119] =  9.9695348500117802077346E-001;
  SP.dnu[8][119] =  6.7627721589632042997332E-004;
  SP.xnu[8][120] =  9.9759236333609844312242E-001;
  SP.dnu[8][120] = -6.0210340581382563539842E-004;
  SP.xnu[8][121] =  9.9815630609138900631361E-001;
  SP.dnu[8][121] =  5.2638393252626951669697E-004;
  SP.xnu[8][122] =  9.9864522833934510806780E-001;
  SP.dnu[8][122] = -4.5007076293308481620496E-004;
  SP.xnu[8][123] =  9.9905905645007460356258E-001;
  SP.dnu[8][123] =  3.7614089259603763096433E-004;
  SP.xnu[8][124] =  9.9939772810258619635739E-001;
  SP.dnu[8][124] = -3.0339397288843162770178E-004;
  SP.xnu[8][125] =  9.9966119229417475044811E-001;
  SP.dnu[8][125] =  2.2553868611462237278683E-004;
  SP.xnu[8][126] =  9.9984940934810211005788E-001;
  SP.dnu[8][126] = -1.4282865758344266858457E-004;
  SP.xnu[8][127] =  9.9996235091957227046082E-001;
  SP.dnu[8][127] =  7.3515253416082291357568E-005;
  SP.xnu[8][128] =  1.0000000000000000000000E+000;
  SP.dnu[8][128] = -2.2889929935494797116411E-005;

  SP.xnu[9][0] =  5.0000000000000000000000E-001;
  SP.dnu[9][0] = -3.0679620123147538470861E-003;
  SP.xnu[9][1] =  5.0306794232457723767982E-001;
  SP.dnu[9][1] =  3.0679037931569238715367E-003;
  SP.xnu[9][2] =  5.0613576914285996303970E-001;
  SP.dnu[9][2] = -3.0677300690818604172216E-003;
  SP.xnu[9][3] =  5.0920336495290241046368E-001;
  SP.dnu[9][3] =  3.0674417781171607020085E-003;
  SP.xnu[9][4] =  5.1227061426145614401587E-001;
  SP.dnu[9][4] = -3.0670379999103934133716E-003;
  SP.xnu[9][5] =  5.1533740158831831296701E-001;
  SP.dnu[9][5] =  3.0665178176153880383109E-003;
  SP.xnu[9][6] =  5.1840361147067941616217E-001;
  SP.dnu[9][6] = -3.0658821823034341317202E-003;
  SP.xnu[9][7] =  5.2146912846747041153856E-001;
  SP.dnu[9][7] =  3.0651320507966350487163E-003;
  SP.xnu[9][8] =  5.2453383716370900712748E-001;
  SP.dnu[9][8] = -3.0642665192892607349767E-003;
  SP.xnu[9][9] =  5.2759762217484496990472E-001;
  SP.dnu[9][9] =  3.0632846863522516073740E-003;
  SP.xnu[9][10] =  5.3066036815110428889131E-001;
  SP.dnu[9][10] = -3.0621875218412407253724E-003;
  SP.xnu[9][11] =  5.3372195978183202894899E-001;
  SP.dnu[9][11] =  3.0609760024884800581464E-003;
  SP.xnu[9][12] =  5.3678228179983371176473E-001;
  SP.dnu[9][12] = -3.0596492398919452906933E-003;
  SP.xnu[9][13] =  5.3984121898571506057356E-001;
  SP.dnu[9][13] =  3.0582063468845582619248E-003;
  SP.xnu[9][14] =  5.4289865617221994523078E-001;
  SP.dnu[9][14] = -3.0566483132140806033952E-003;
  SP.xnu[9][15] =  5.4595447824856636431250E-001;
  SP.dnu[9][15] =  3.0549761366403602364481E-003;
  SP.xnu[9][16] =  5.4900857016478030099710E-001;
  SP.dnu[9][16] = -3.0531889430021104305908E-003;
  SP.xnu[9][17] =  5.5206081693602728956047E-001;
  SP.dnu[9][17] =  3.0512858582135822731203E-003;
  SP.xnu[9][18] =  5.5511110364694152940395E-001;
  SP.dnu[9][18] = -3.0492678930242629177017E-003;
  SP.xnu[9][19] =  5.5815931545595238362627E-001;
  SP.dnu[9][19] =  3.0471360673464839895979E-003;
  SP.xnu[9][20] =  5.6120533759960809924935E-001;
  SP.dnu[9][20] = -3.0448895200712222312794E-003;
  SP.xnu[9][21] =  5.6424905539689658631221E-001;
  SP.dnu[9][21] =  3.0425273889821953497658E-003;
  SP.xnu[9][22] =  5.6729035425356309315818E-001;
  SP.dnu[9][22] = -3.0400507069476649544723E-003;
  SP.xnu[9][23] =  5.7032911966642461535739E-001;
  SP.dnu[9][23] =  3.0374605171706022539003E-003;
  SP.xnu[9][24] =  5.7336523722768087582943E-001;
  SP.dnu[9][24] = -3.0347559703745845322968E-003;
  SP.xnu[9][25] =  5.7639859262922171386017E-001;
  SP.dnu[9][25] =  3.0319362149638248710916E-003;
  SP.xnu[9][26] =  5.7942907166693072084219E-001;
  SP.dnu[9][26] = -3.0290023070556924360045E-003;
  SP.xnu[9][27] =  5.8245656024498496070909E-001;
  SP.dnu[9][27] =  3.0259553143013044531323E-003;
  SP.xnu[9][28] =  5.8548094438015061318182E-001;
  SP.dnu[9][28] = -3.0227943980000938248376E-003;
  SP.xnu[9][29] =  5.8850211020607437809842E-001;
  SP.dnu[9][29] =  3.0195187158845159338278E-003;
  SP.xnu[9][30] =  5.9151994397757047925827E-001;
  SP.dnu[9][30] = -3.0161293484707515774060E-003;
  SP.xnu[9][31] =  5.9453433207490310637750E-001;
  SP.dnu[9][31] =  3.0126273890413251798215E-003;
  SP.xnu[9][32] =  5.9754516100806413392414E-001;
  SP.dnu[9][32] = -3.0090120081715301905721E-003;
  SP.xnu[9][33] =  6.0055231742104595577922E-001;
  SP.dnu[9][33] =  3.0052823715800113183104E-003;
  SP.xnu[9][34] =  6.0355568809610927485406E-001;
  SP.dnu[9][34] = -3.0014395853572445125414E-003;
  SP.xnu[9][35] =  6.0655515995804568698388E-001;
  SP.dnu[9][35] =  2.9974847696329761232167E-003;
  SP.xnu[9][36] =  6.0955062007843489861387E-001;
  SP.dnu[9][36] = -2.9934171029086019855768E-003;
  SP.xnu[9][37] =  6.1254195567989641799582E-001;
  SP.dnu[9][37] =  2.9892357574901639942493E-003;
  SP.xnu[9][38] =  6.1552905414033555982162E-001;
  SP.dnu[9][38] = -2.9849418662504991817663E-003;
  SP.xnu[9][39] =  6.1851180299718360343387E-001;
  SP.dnu[9][39] =  2.9805365774222176125928E-003;
  SP.xnu[9][40] =  6.2149008995163194497414E-001;
  SP.dnu[9][40] = -2.9760190760263622100161E-003;
  SP.xnu[9][41] =  6.2446380287286008405534E-001;
  SP.dnu[9][41] =  2.9713885394933959024712E-003;
  SP.xnu[9][42] =  6.2743282980225728577699E-001;
  SP.dnu[9][42] = -2.9666461287264426919225E-003;
  SP.xnu[9][43] =  6.3039705895763775914009E-001;
  SP.dnu[9][43] =  2.9617930213642824693067E-003;
  SP.xnu[9][44] =  6.3335637873744919316264E-001;
  SP.dnu[9][44] = -2.9568284074770132799104E-003;
  SP.xnu[9][45] =  6.3631067772497449224667E-001;
  SP.dnu[9][45] =  2.9517514680843142224070E-003;
  SP.xnu[9][46] =  6.3925984469252655260392E-001;
  SP.dnu[9][46] = -2.9465633934152232488866E-003;
  SP.xnu[9][47] =  6.4220376860563592180916E-001;
  SP.dnu[9][47] =  2.9412653918741614228222E-003;
  SP.xnu[9][48] =  6.4514233862723118381810E-001;
  SP.dnu[9][48] = -2.9358566570375140101976E-003;
  SP.xnu[9][49] =  6.4807544412181191206089E-001;
  SP.dnu[9][49] =  2.9303363718979919249857E-003;
  SP.xnu[9][50] =  6.5100297465961403350173E-001;
  SP.dnu[9][50] = -2.9247057573623797520517E-003;
  SP.xnu[9][51] =  6.5392482002076744684103E-001;
  SP.dnu[9][51] =  2.9189660540256539037624E-003;
  SP.xnu[9][52] =  6.5684087019944573832824E-001;
  SP.dnu[9][52] = -2.9131164573467978222019E-003;
  SP.xnu[9][53] =  6.5975101540800783895076E-001;
  SP.dnu[9][53] =  2.9071561505848128763309E-003;
  SP.xnu[9][54] =  6.6265514608113146706798E-001;
  SP.dnu[9][54] = -2.9010863867415497497928E-003;
  SP.xnu[9][55] =  6.6555315287993820086860E-001;
  SP.dnu[9][55] =  2.8949084401030803406994E-003;
  SP.xnu[9][56] =  6.6844492669611002534463E-001;
  SP.dnu[9][56] = -2.8886215062968043059187E-003;
  SP.xnu[9][57] =  6.7133035865599719879639E-001;
  SP.dnu[9][57] =  2.8822247670401728593589E-003;
  SP.xnu[9][58] =  6.7420934012471728420965E-001;
  SP.dnu[9][58] = -2.8757195089230953984746E-003;
  SP.xnu[9][59] =  6.7708176271024519117870E-001;
  SP.dnu[9][59] =  2.8691070415101419173849E-003;
  SP.xnu[9][60] =  6.7994751826749407438755E-001;
  SP.dnu[9][60] = -2.8623865587819171112715E-003;
  SP.xnu[9][61] =  6.8280649890238693500587E-001;
  SP.dnu[9][61] =  2.8555572389937163789734E-003;
  SP.xnu[9][62] =  6.8565859697591877170597E-001;
  SP.dnu[9][62] = -2.8486204039034130476827E-003;
  SP.xnu[9][63] =  6.8850370510820912836328E-001;
  SP.dnu[9][63] =  2.8415774000408008547146E-003;
  SP.xnu[9][64] =  6.9134171618254488586423E-001;
  SP.dnu[9][64] = -2.8344274178117895392232E-003;
  SP.xnu[9][65] =  6.9417252334941314581250E-001;
  SP.dnu[9][65] =  2.8271696299631747736226E-003;
  SP.xnu[9][66] =  6.9699602003052405429809E-001;
  SP.dnu[9][66] = -2.8198053951000747800898E-003;
  SP.xnu[9][67] =  6.9981209992282341427206E-001;
  SP.dnu[9][67] =  2.8123360985174384288398E-003;
  SP.xnu[9][68] =  7.0262065700249493545424E-001;
  SP.dnu[9][68] = -2.8047609249929249968751E-003;
  SP.xnu[9][69] =  7.0542158552895197109173E-001;
  SP.dnu[9][69] =  2.7970790395782537337694E-003;
  SP.xnu[9][70] =  7.0821478004881859128130E-001;
  SP.dnu[9][70] = -2.7892918395183293366953E-003;
  SP.xnu[9][71] =  7.1100013539989984297064E-001;
  SP.dnu[9][71] =  2.7814007508019288859402E-003;
  SP.xnu[9][72] =  7.1377754671514104716048E-001;
  SP.dnu[9][72] = -2.7734049503847625356166E-003;
  SP.xnu[9][73] =  7.1654690942657598424211E-001;
  SP.dnu[9][73] =  2.7653035932803975889377E-003;
  SP.xnu[9][74] =  7.1930811926926381882351E-001;
  SP.dnu[9][74] = -2.7570981172948650044249E-003;
  SP.xnu[9][75] =  7.2206107228521461582103E-001;
  SP.dnu[9][75] =  2.7487899911856449280095E-003;
  SP.xnu[9][76] =  7.2480566482730330002315E-001;
  SP.dnu[9][76] = -2.7403783817363979007119E-003;
  SP.xnu[9][77] =  7.2754179356317191176793E-001;
  SP.dnu[9][77] =  2.7318624314046334322452E-003;
  SP.xnu[9][78] =  7.3026935547912001181659E-001;
  SP.dnu[9][78] = -2.7232436206251078494749E-003;
  SP.xnu[9][79] =  7.3298824788398308895138E-001;
  SP.dnu[9][79] =  2.7145234631647842852971E-003;
  SP.xnu[9][80] =  7.3569836841299882427819E-001;
  SP.dnu[9][80] = -2.7057011131104478125473E-003;
  SP.xnu[9][81] =  7.3839961503166106667108E-001;
  SP.dnu[9][81] =  2.6967756976500700672258E-003;
  SP.xnu[9][82] =  7.4109188603956137425867E-001;
  SP.dnu[9][82] = -2.6877487420806947215704E-003;
  SP.xnu[9][83] =  7.4377508007421797732074E-001;
  SP.dnu[9][83] =  2.6786218076077768219712E-003;
  SP.xnu[9][84] =  7.4644909611489201843651E-001;
  SP.dnu[9][84] = -2.6693940329009395479900E-003;
  SP.xnu[9][85] =  7.4911383348639092620549E-001;
  SP.dnu[9][85] =  2.6600645269459946479441E-003;
  SP.xnu[9][86] =  7.5176919186285877934593E-001;
  SP.dnu[9][86] = -2.6506348623241212524454E-003;
  SP.xnu[9][87] =  7.5441507127155351846587E-001;
  SP.dnu[9][87] =  2.6411066503218974030491E-003;
  SP.xnu[9][88] =  7.5705137209661086329685E-001;
  SP.dnu[9][88] = -2.6314790112524793283343E-003;
  SP.xnu[9][89] =  7.5967799508279479368091E-001;
  SP.dnu[9][89] =  2.6217510327208734998519E-003;
  SP.xnu[9][90] =  7.6229484133923445310755E-001;
  SP.dnu[9][90] = -2.6119243372279200752868E-003;
  SP.xnu[9][91] =  7.6490181234314733410803E-001;
  SP.dnu[9][91] =  2.6020005890265711413439E-003;
  SP.xnu[9][92] =  7.6749880994354860533154E-001;
  SP.dnu[9][92] = -2.5919788868883216595876E-003;
  SP.xnu[9][93] =  7.7008573636494644064892E-001;
  SP.dnu[9][93] =  2.5818582935819227125788E-003;
  SP.xnu[9][94] =  7.7266249421102321115699E-001;
  SP.dnu[9][94] = -2.5716404844060730534211E-003;
  SP.xnu[9][95] =  7.7522898646830240148864E-001;
  SP.dnu[9][95] =  2.5613271797412143981075E-003;
  SP.xnu[9][96] =  7.7778511650980111237142E-001;
  SP.dnu[9][96] = -2.5509174533553278258287E-003;
  SP.xnu[9][97] =  7.8033078809866801191986E-001;
  SP.dnu[9][97] =  2.5404103394132684393535E-003;
  SP.xnu[9][98] =  7.8286590539180659869488E-001;
  SP.dnu[9][98] = -2.5298075691657026501440E-003;
  SP.xnu[9][99] =  7.8539037294348364011633E-001;
  SP.dnu[9][99] =  2.5191109225958067795780E-003;
  SP.xnu[9][100] =  7.8790409570892265037299E-001;
  SP.dnu[9][100] = -2.5083194446941655585469E-003;
  SP.xnu[9][101] =  7.9040697904788227253780E-001;
  SP.dnu[9][101] =  2.4974321369010661666093E-003;
  SP.xnu[9][102] =  7.9289892872821943016404E-001;
  SP.dnu[9][102] = -2.4864507898874207475744E-003;
  SP.xnu[9][103] =  7.9537985092943711421194E-001;
  SP.dnu[9][103] =  2.4753772470727360947545E-003;
  SP.xnu[9][104] =  7.9784965224621667173352E-001;
  SP.dnu[9][104] = -2.4642105205434640293430E-003;
  SP.xnu[9][105] =  8.0030823969193446332694E-001;
  SP.dnu[9][105] =  2.4529495744942922669797E-003;
  SP.xnu[9][106] =  8.0275552070216275696031E-001;
  SP.dnu[9][106] = -2.4415962628430370456021E-003;
  SP.xnu[9][107] =  8.0519140313815472635818E-001;
  SP.dnu[9][107] =  2.4301524966887995462078E-003;
  SP.xnu[9][108] =  8.0761579529031342274246E-001;
  SP.dnu[9][108] = -2.4186172506869995233041E-003;
  SP.xnu[9][109] =  8.1002860588164458932313E-001;
  SP.dnu[9][109] =  2.4069894468102596401173E-003;
  SP.xnu[9][110] =  8.1242974407119318854204E-001;
  SP.dnu[9][110] = -2.3952710064596421358765E-003;
  SP.xnu[9][111] =  8.1481911945746351268649E-001;
  SP.dnu[9][111] =  2.3834639131265800267554E-003;
  SP.xnu[9][112] =  8.1719664208182274910759E-001;
  SP.dnu[9][112] = -2.3715670990533485666761E-003;
  SP.xnu[9][113] =  8.1956222243188787190074E-001;
  SP.dnu[9][113] =  2.3595794384942418782344E-003;
  SP.xnu[9][114] =  8.2191577144489573253404E-001;
  SP.dnu[9][114] = -2.3475029250393805023850E-003;
  SP.xnu[9][115] =  8.2425720051105622254228E-001;
  SP.dnu[9][115] =  2.3353396198247459763313E-003;
  SP.xnu[9][116] =  8.2658642147688838204210E-001;
  SP.dnu[9][116] = -2.3230884071778084897971E-003;
  SP.xnu[9][117] =  8.2890334664853932846559E-001;
  SP.dnu[9][117] =  2.3107481075429168365134E-003;
  SP.xnu[9][118] =  8.3120788879508588055653E-001;
  SP.dnu[9][118] = -2.2983207919445134904359E-003;
  SP.xnu[9][119] =  8.3349996115181875332508E-001;
  SP.dnu[9][119] =  2.2858086050371464652582E-003;
  SP.xnu[9][120] =  8.3577947742350920031269E-001;
  SP.dnu[9][120] = -2.2732103771367526315220E-003;
  SP.xnu[9][121] =  8.3804635178765798018021E-001;
  SP.dnu[9][121] =  2.2605248681016603689726E-003;
  SP.xnu[9][122] =  8.4030049889772652529722E-001;
  SP.dnu[9][122] = -2.2477542322576382981779E-003;
  SP.xnu[9][123] =  8.4254183388635019068103E-001;
  SP.dnu[9][123] =  2.2349007043708896107318E-003;
  SP.xnu[9][124] =  8.4477027236853346230837E-001;
  SP.dnu[9][124] = -2.2219630539649617587763E-003;
  SP.xnu[9][125] =  8.4698573044482700450187E-001;
  SP.dnu[9][125] =  2.2089399727460338727372E-003;
  SP.xnu[9][126] =  8.4918812470448642677741E-001;
  SP.dnu[9][126] = -2.1958337049271716681049E-003;
  SP.xnu[9][127] =  8.5137737222861265122646E-001;
  SP.dnu[9][127] =  2.1826465828139016270612E-003;
  SP.xnu[9][128] =  8.5355339059327376220042E-001;
  SP.dnu[9][128] = -2.1693773075668591742171E-003;
  SP.xnu[9][129] =  8.5571609787260822076107E-001;
  SP.dnu[9][129] =  2.1560244942581146439806E-003;
  SP.xnu[9][130] =  8.5786541264190932706277E-001;
  SP.dnu[9][130] = -2.1425904844084198765056E-003;
  SP.xnu[9][131] =  8.6000125398069081453834E-001;
  SP.dnu[9][131] =  2.1290777162627651749597E-003;
  SP.xnu[9][132] =  8.6212354147573346047053E-001;
  SP.dnu[9][132] = -2.1154848141329806082789E-003;
  SP.xnu[9][133] =  8.6423219522411259824602E-001;
  SP.dnu[9][133] =  2.1018103069086152349076E-003;
  SP.xnu[9][134] =  8.6632713583620641730777E-001;
  SP.dnu[9][134] = -2.0880566418107310823377E-003;
  SP.xnu[9][135] =  8.6840828443868493754507E-001;
  SP.dnu[9][135] =  2.0742263725619284742791E-003;
  SP.xnu[9][136] =  8.7047556267747954558781E-001;
  SP.dnu[9][136] = -2.0603180370734401241657E-003;
  SP.xnu[9][137] =  8.7252889272073298120395E-001;
  SP.dnu[9][137] =  2.0463300672560261742360E-003;
  SP.xnu[9][138] =  8.7456819726172966273460E-001;
  SP.dnu[9][138] = -2.0322650255613498161396E-003;
  SP.xnu[9][139] =  8.7659339952180624124172E-001;
  SP.dnu[9][139] =  2.0181255920656599154460E-003;
  SP.xnu[9][140] =  8.7860442325324227378773E-001;
  SP.dnu[9][140] = -2.0039102074806224615628E-003;
  SP.xnu[9][141] =  8.8060119274213090701485E-001;
  SP.dnu[9][141] =  1.9896171944742944608089E-003;
  SP.xnu[9][142] =  8.8258363281122946294441E-001;
  SP.dnu[9][142] = -1.9752492415966489058673E-003;
  SP.xnu[9][143] =  8.8455166882278981967331E-001;
  SP.dnu[9][143] =  1.9608091677343953516155E-003;
  SP.xnu[9][144] =  8.8650522668136848040545E-001;
  SP.dnu[9][144] = -1.9462953041338586096454E-003;
  SP.xnu[9][145] =  8.8844423283661622502041E-001;
  SP.dnu[9][145] =  1.9317058502208166419543E-003;
  SP.xnu[9][146] =  8.9036861428604723915079E-001;
  SP.dnu[9][146] = -1.9170436330913763358181E-003;
  SP.xnu[9][147] =  8.9227829857778761651195E-001;
  SP.dnu[9][147] =  1.9023116247773850834389E-003;
  SP.xnu[9][148] =  8.9417321381330313100458E-001;
  SP.dnu[9][148] = -1.8875080330594512926111E-003;
  SP.xnu[9][149] =  8.9605328865010617589117E-001;
  SP.dnu[9][149] =  1.8726309180567779164966E-003;
  SP.xnu[9][150] =  8.9791845230444176813140E-001;
  SP.dnu[9][150] = -1.8576832597363868289705E-003;
  SP.xnu[9][151] =  8.9976863455395251675012E-001;
  SP.dnu[9][151] =  1.8426681998537922193724E-003;
  SP.xnu[9][152] =  9.0160376574032245490334E-001;
  SP.dnu[9][152] = -1.8275838066601492832115E-003;
  SP.xnu[9][153] =  9.0342377677189963610326E-001;
  SP.dnu[9][153] =  1.8124279824321045040934E-003;
  SP.xnu[9][154] =  9.0522859912629739586335E-001;
  SP.dnu[9][154] = -1.7972038765749763206713E-003;
  SP.xnu[9][155] =  9.0701816485297418082726E-001;
  SP.dnu[9][155] =  1.7819148198446199205467E-003;
  SP.xnu[9][156] =  9.0879240657579184825246E-001;
  SP.dnu[9][156] = -1.7665587224290818764739E-003;
  SP.xnu[9][157] =  9.1055125749555233953022E-001;
  SP.dnu[9][157] =  1.7511333072475110239609E-003;
  SP.xnu[9][158] =  9.1229465139251263223740E-001;
  SP.dnu[9][158] = -1.7356419124073211106993E-003;
  SP.xnu[9][159] =  9.1402252262887787603376E-001;
  SP.dnu[9][159] =  1.7200880802080465629774E-003;
  SP.xnu[9][160] =  9.1573480615127261853939E-001;
  SP.dnu[9][160] = -1.7044695412643448216269E-003;
  SP.xnu[9][161] =  9.1743143749319002815220E-001;
  SP.dnu[9][161] =  1.6887838140063111370561E-003;
  SP.xnu[9][162] =  9.1911235277741902159350E-001;
  SP.dnu[9][162] = -1.6730344477715195581518E-003;
  SP.xnu[9][163] =  9.2077748871844920480175E-001;
  SP.dnu[9][163] =  1.6572252229309168303883E-003;
  SP.xnu[9][164] =  9.2242678262485353662979E-001;
  SP.dnu[9][164] = -1.6413536654020103932180E-003;
  SP.xnu[9][165] =  9.2406017240164862563957E-001;
  SP.dnu[9][165] =  1.6254170595688080259740E-003;
  SP.xnu[9][166] =  9.2567759655263257113065E-001;
  SP.dnu[9][166] = -1.6094191925081533092020E-003;
  SP.xnu[9][167] =  9.2727899418270026038393E-001;
  SP.dnu[9][167] =  1.5933641140892605014380E-003;
  SP.xnu[9][168] =  9.2886430500013603495113E-001;
  SP.dnu[9][168] = -1.5772491159875254447077E-003;
  SP.xnu[9][169] =  9.3043346931888363967229E-001;
  SP.dnu[9][169] =  1.5610712135221755607061E-003;
  SP.xnu[9][170] =  9.3198642806079336895907E-001;
  SP.dnu[9][170] = -1.5448344629128301178852E-003;
  SP.xnu[9][171] =  9.3352312275784632574010E-001;
  SP.dnu[9][171] =  1.5285432210307693027376E-003;
  SP.xnu[9][172] =  9.3504349555435570932615E-001;
  SP.dnu[9][172] = -1.5121945103085920948591E-003;
  SP.xnu[9][173] =  9.3654748920914504931804E-001;
  SP.dnu[9][173] =  1.4957850351787560924706E-003;
  SP.xnu[9][174] =  9.3803504709770330354792E-001;
  SP.dnu[9][174] = -1.4793191584773668692475E-003;
  SP.xnu[9][175] =  9.3950611321431673891566E-001;
  SP.dnu[9][175] =  1.4628015891921237791189E-003;
  SP.xnu[9][176] =  9.4096063217417751485638E-001;
  SP.dnu[9][176] = -1.4462290387172280891885E-003;
  SP.xnu[9][177] =  9.4239854921546889005200E-001;
  SP.dnu[9][177] =  1.4295978502155960711555E-003;
  SP.xnu[9][178] =  9.4381981020142697388009E-001;
  SP.dnu[9][178] = -1.4129127382143468959736E-003;
  SP.xnu[9][179] =  9.4522436162237894497608E-001;
  SP.dnu[9][179] =  1.3961788185638745523988E-003;
  SP.xnu[9][180] =  9.4661215059775766017121E-001;
  SP.dnu[9][180] = -1.3793924412756530436874E-003;
  SP.xnu[9][181] =  9.4798312487809257795728E-001;
  SP.dnu[9][181] =  1.3625495269677488801461E-003;
  SP.xnu[9][182] =  9.4933723284697692152099E-001;
  SP.dnu[9][182] = -1.3456551965504554240432E-003;
  SP.xnu[9][183] =  9.5067442352301100728537E-001;
  SP.dnu[9][183] =  1.3287150398151584385599E-003;
  SP.xnu[9][184] =  9.5199464656172166579310E-001;
  SP.dnu[9][184] = -1.3117249841714145060813E-003;
  SP.xnu[9][185] =  9.5329785225745768266648E-001;
  SP.dnu[9][185] =  1.2946804523872817181453E-003;
  SP.xnu[9][186] =  9.5458399154526118828194E-001;
  SP.dnu[9][186] = -1.2775870387590750640419E-003;
  SP.xnu[9][187] =  9.5585301600271492570220E-001;
  SP.dnu[9][187] =  1.2604508900897204003163E-003;
  SP.xnu[9][188] =  9.5710487785176532731751E-001;
  SP.dnu[9][188] = -1.2432674359642891788471E-003;
  SP.xnu[9][189] =  9.5833952996052133155823E-001;
  SP.dnu[9][189] =  1.2260315076788327884964E-003;
  SP.xnu[9][190] =  9.5955692584502887195424E-001;
  SP.dnu[9][190] = -1.2087492558782954066443E-003;
  SP.xnu[9][191] =  9.6075701967102097173270E-001;
  SP.dnu[9][191] =  1.1914274884832548047630E-003;
  SP.xnu[9][192] =  9.6193976625564337806409E-001;
  SP.dnu[9][192] = -1.1740610437554529858470E-003;
  SP.xnu[9][193] =  9.6310512106915567098740E-001;
  SP.dnu[9][193] =  1.1566440436206248231392E-003;
  SP.xnu[9][194] =  9.6425304023660778296858E-001;
  SP.dnu[9][194] = -1.1391832990199142713524E-003;
  SP.xnu[9][195] =  9.6538348053949186597244E-001;
  SP.dnu[9][195] =  1.1216864112094991217887E-003;
  SP.xnu[9][196] =  9.6649639941736944385583E-001;
  SP.dnu[9][196] = -1.1041475094163715012435E-003;
  SP.xnu[9][197] =  9.6759175496947378882110E-001;
  SP.dnu[9][197] =  1.0865598555764048591464E-003;
  SP.xnu[9][198] =  9.6866950595628746160095E-001;
  SP.dnu[9][198] = -1.0689310529055247226374E-003;
  SP.xnu[9][199] =  9.6972961180109495598133E-001;
  SP.dnu[9][199] =  1.0512696664579231487730E-003;
  SP.xnu[9][200] =  9.7077203259151038920625E-001;
  SP.dnu[9][200] = -1.0335689660961692656549E-003;
  SP.xnu[9][201] =  9.7179672908098018074765E-001;
  SP.dnu[9][201] =  1.0158211581976201582454E-003;
  SP.xnu[9][202] =  9.7280366269026066286547E-001;
  SP.dnu[9][202] = -9.9803480834316079205440E-004;
  SP.xnu[9][203] =  9.7379279550887056732669E-001;
  SP.dnu[9][203] =  9.8021966893754062287236E-004;
  SP.xnu[9][204] =  9.7476409029651833359797E-001;
  SP.dnu[9][204] = -9.6236795537181936225175E-004;
  SP.xnu[9][205] =  9.7571751048450418477459E-001;
  SP.dnu[9][205] =  9.4447055980383308089215E-004;
  SP.xnu[9][206] =  9.7665302017709691845837E-001;
  SP.dnu[9][206] = -9.2653723313402003269713E-004;
  SP.xnu[9][207] =  9.7757058415288536074908E-001;
  SP.dnu[9][207] =  9.0857921408587052746717E-004;
  SP.xnu[9][208] =  9.7847016786610443246790E-001;
  SP.dnu[9][208] = -8.9058740567612089691310E-004;
  SP.xnu[9][209] =  9.7935173744793577768732E-001;
  SP.dnu[9][209] =  8.7255103640793521017232E-004;
  SP.xnu[9][210] =  9.8021525970778290559952E-001;
  SP.dnu[9][210] = -8.5448134047338587571168E-004;
  SP.xnu[9][211] =  9.8106070213452079771480E-001;
  SP.dnu[9][211] =  8.3639145189210743943130E-004;
  SP.xnu[9][212] =  9.8188803289771993334323E-001;
  SP.dnu[9][212] = -8.1827061316331720737097E-004;
  SP.xnu[9][213] =  9.8269722084884468727542E-001;
  SP.dnu[9][213] =  8.0010590531036572852420E-004;
  SP.xnu[9][214] =  9.8348823552242605454361E-001;
  SP.dnu[9][214] = -7.8191045306458414940739E-004;
  SP.xnu[9][215] =  9.8426104713720865811054E-001;
  SP.dnu[9][215] =  7.6369986022295776802480E-004;
  SP.xnu[9][216] =  9.8501562659727199630199E-001;
  SP.dnu[9][216] = -7.4546122724101614884027E-004;
  SP.xnu[9][217] =  9.8575194549312588776855E-001;
  SP.dnu[9][217] =  7.2717879809865704857974E-004;
  SP.xnu[9][218] =  9.8646997610278007273386E-001;
  SP.dnu[9][218] = -7.0886815939502798071752E-004;
  SP.xnu[9][219] =  9.8716969139278793025936E-001;
  SP.dnu[9][219] =  6.9054821741027633409824E-004;
  SP.xnu[9][220] =  9.8785106501926427223020E-001;
  SP.dnu[9][220] = -6.7220324530010469723393E-004;
  SP.xnu[9][221] =  9.8851407132887717574293E-001;
  SP.dnu[9][221] =  6.5381363269525272882488E-004;
  SP.xnu[9][222] =  9.8915868535981381655312E-001;
  SP.dnu[9][222] = -6.3539825468371378569953E-004;
  SP.xnu[9][223] =  9.8978488284272026721966E-001;
  SP.dnu[9][223] =  6.1698057356535305513530E-004;
  SP.xnu[9][224] =  9.9039264020161522456309E-001;
  SP.dnu[9][224] = -5.9854102647904932321285E-004;
  SP.xnu[9][225] =  9.9098193455477763203642E-001;
  SP.dnu[9][225] =  5.8005458363996407863602E-004;
  SP.xnu[9][226] =  9.9155274371560816359015E-001;
  SP.dnu[9][226] = -5.6154464957537864204775E-004;
  SP.xnu[9][227] =  9.9210504619346453659694E-001;
  SP.dnu[9][227] =  5.4304121936105433826372E-004;
  SP.xnu[9][228] =  9.9263882119447062238701E-001;
  SP.dnu[9][228] = -5.2451934753504281657970E-004;
  SP.xnu[9][229] =  9.9315404862229932393165E-001;
  SP.dnu[9][229] =  5.0594604861902790914289E-004;
  SP.xnu[9][230] =  9.9365070907892919119991E-001;
  SP.dnu[9][230] = -4.8735120497440404395743E-004;
  SP.xnu[9][231] =  9.9412878386537474570240E-001;
  SP.dnu[9][231] =  4.6877464906600921532825E-004;
  SP.xnu[9][232] =  9.9458825498239048672584E-001;
  SP.dnu[9][232] = -4.5018356032066226950880E-004;
  SP.xnu[9][233] =  9.9502910513114855275295E-001;
  SP.dnu[9][233] =  4.3153260589308059249633E-004;
  SP.xnu[9][234] =  9.9545131771389001255412E-001;
  SP.dnu[9][234] = -4.1286137933406444656086E-004;
  SP.xnu[9][235] =  9.9585487683454976143002E-001;
  SP.dnu[9][235] =  3.9422550864932247901687E-004;
  SP.xnu[9][236] =  9.9623976729935499907838E-001;
  SP.dnu[9][236] = -3.7558002377197524355581E-004;
  SP.xnu[9][237] =  9.9660597461739726655230E-001;
  SP.dnu[9][237] =  3.5685894626467126186237E-004;
  SP.xnu[9][238] =  9.9695348500117802077346E-001;
  SP.dnu[9][238] = -3.3811733238763903422800E-004;
  SP.xnu[9][239] =  9.9728228536712772605955E-001;
  SP.dnu[9][239] =  3.1943849851320878985138E-004;
  SP.xnu[9][240] =  9.9759236333609844312242E-001;
  SP.dnu[9][240] = -3.0075739809917895241561E-004;
  SP.xnu[9][241] =  9.9788370723382989699125E-001;
  SP.dnu[9][241] =  2.8196971993830594116149E-004;
  SP.xnu[9][242] =  9.9815630609138900631361E-001;
  SP.dnu[9][242] = -2.6315714532474331695924E-004;
  SP.xnu[9][243] =  9.9841014964558285748631E-001;
  SP.dnu[9][243] =  2.4445810646831564173562E-004;
  SP.xnu[9][244] =  9.9864522833934510806780E-001;
  SP.dnu[9][244] = -2.2577128254480201003796E-004;
  SP.xnu[9][245] =  9.9886153332209580492427E-001;
  SP.dnu[9][245] =  2.0690901804668110339610E-004;
  SP.xnu[9][246] =  9.9905905645007460356258E-001;
  SP.dnu[9][246] = -1.8800358114409088156122E-004;
  SP.xnu[9][247] =  9.9923779028664737610428E-001;
  SP.dnu[9][247] =  1.6932747382126024269014E-004;
  SP.xnu[9][248] =  9.9939772810258619635739E-001;
  SP.dnu[9][248] = -1.5070672823411912959905E-004;
  SP.xnu[9][249] =  9.9953886387632269144439E-001;
  SP.dnu[9][249] =  1.3171750624911970098294E-004;
  SP.xnu[9][250] =  9.9966119229417475044811E-001;
  SP.dnu[9][250] = -1.1259472961410483333640E-004;
  SP.xnu[9][251] =  9.9976470875054658153985E-001;
  SP.dnu[9][251] =  9.4079539020991087059008E-005;
  SP.xnu[9][252] =  9.9984940934810211005788E-001;
  SP.dnu[9][252] = -7.5870688870526311869387E-005;
  SP.xnu[9][253] =  9.9991529089791171100786E-001;
  SP.dnu[9][253] =  5.6394269412180066336702E-005;
  SP.xnu[9][254] =  9.9996235091957227046082E-001;
  SP.dnu[9][254] = -3.5710595623661778217105E-005;
  SP.xnu[9][255] =  9.9999058764130057132850E-001;
  SP.dnu[9][255] =  1.8379202995134049673791E-005;
  SP.xnu[9][256] =  1.0000000000000000000000E+000;
  SP.dnu[9][256] = -5.7221550395503410147616E-006;

  SP.xnu[10][0] =  5.0000000000000000000000E-001;
  SP.dnu[10][0] = -1.5339808151702637208246E-003;
  SP.xnu[10][1] =  5.0153397838148298813507E-001;
  SP.dnu[10][1] =  1.5339735668609438903701E-003;
  SP.xnu[10][2] =  5.0306794232457723767982E-001;
  SP.dnu[10][2] = -1.5339518802070554464518E-003;
  SP.xnu[10][3] =  5.0460187739102990965755E-001;
  SP.dnu[10][3] =  1.5339158136229514819550E-003;
  SP.xnu[10][4] =  5.0613576914285996303970E-001;
  SP.dnu[10][4] = -1.5338653092422575543550E-003;
  SP.xnu[10][5] =  5.0766960314249405052208E-001;
  SP.dnu[10][5] =  1.5338003093213433868591E-003;
  SP.xnu[10][6] =  5.0920336495290241046368E-001;
  SP.dnu[10][6] = -1.5337208726822420074030E-003;
  SP.xnu[10][7] =  5.1073704013773475370919E-001;
  SP.dnu[10][7] =  1.5336270583047911844682E-003;
  SP.xnu[10][8] =  5.1227061426145614401587E-001;
  SP.dnu[10][8] = -1.5335188088530125193168E-003;
  SP.xnu[10][9] =  5.1380407288948287080618E-001;
  SP.dnu[10][9] =  1.5333960670960975867369E-003;
  SP.xnu[10][10] =  5.1533740158831831296701E-001;
  SP.dnu[10][10] = -1.5332588924214860225849E-003;
  SP.xnu[10][11] =  5.1687058592568879241686E-001;
  SP.dnu[10][11] =  1.5331073443919508582689E-003;
  SP.xnu[10][12] =  5.1840361147067941616217E-001;
  SP.dnu[10][12] = -1.5329413661842294333518E-003;
  SP.xnu[10][13] =  5.1993646379386990556429E-001;
  SP.dnu[10][13] =  1.5327609010625973908073E-003;
  SP.xnu[10][14] =  5.2146912846747041153856E-001;
  SP.dnu[10][14] = -1.5325660089972901721027E-003;
  SP.xnu[10][15] =  5.2300159106545731440715E-001;
  SP.dnu[10][15] =  1.5323567501514324444611E-003;
  SP.xnu[10][16] =  5.2453383716370900712748E-001;
  SP.dnu[10][16] = -1.5321330681966781355199E-003;
  SP.xnu[10][17] =  5.2606585234014166061818E-001;
  SP.dnu[10][17] =  1.5318949068745090710771E-003;
  SP.xnu[10][18] =  5.2759762217484496990472E-001;
  SP.dnu[10][18] = -1.5316423267553114988398E-003;
  SP.xnu[10][19] =  5.2912913225021787980699E-001;
  SP.dnu[10][19] =  1.5313753886199652382452E-003;
  SP.xnu[10][20] =  5.3066036815110428889131E-001;
  SP.dnu[10][20] = -1.5310940366171015816725E-003;
  SP.xnu[10][21] =  5.3219131546492873040966E-001;
  SP.dnu[10][21] =  1.5307982149473691755056E-003;
  SP.xnu[10][22] =  5.3372195978183202894899E-001;
  SP.dnu[10][22] = -1.5304879847986472444143E-003;
  SP.xnu[10][23] =  5.3525228669480693151368E-001;
  SP.dnu[10][23] =  1.5301634075869829674045E-003;
  SP.xnu[10][24] =  5.3678228179983371176473E-001;
  SP.dnu[10][24] = -1.5298244279198843395363E-003;
  SP.xnu[10][25] =  5.3831193069601574613917E-001;
  SP.dnu[10][25] =  1.5294709904389444211239E-003;
  SP.xnu[10][26] =  5.3984121898571506057356E-001;
  SP.dnu[10][26] = -1.5291031569668863112392E-003;
  SP.xnu[10][27] =  5.4137013227468784655599E-001;
  SP.dnu[10][27] =  1.5287209895723672394876E-003;
  SP.xnu[10][28] =  5.4289865617221994523078E-001;
  SP.dnu[10][28] = -1.5283244333034882445770E-003;
  SP.xnu[10][29] =  5.4442677629126229828079E-001;
  SP.dnu[10][29] =  1.5279134332243595332201E-003;
  SP.xnu[10][30] =  5.4595447824856636431250E-001;
  SP.dnu[10][30] = -1.5274880518099295049272E-003;
  SP.xnu[10][31] =  5.4748174766481949946902E-001;
  SP.dnu[10][31] =  1.5270483517989607094112E-003;
  SP.xnu[10][32] =  5.4900857016478030099710E-001;
  SP.dnu[10][32] = -1.5265942786616587658734E-003;
  SP.xnu[10][33] =  5.5053493137741391249394E-001;
  SP.dnu[10][33] =  1.5261257778659967762113E-003;
  SP.xnu[10][34] =  5.5206081693602728956047E-001;
  SP.dnu[10][34] = -1.5256429125565824989411E-003;
  SP.xnu[10][35] =  5.5358621247840442458776E-001;
  SP.dnu[10][35] =  1.5251457461598541068545E-003;
  SP.xnu[10][36] =  5.5511110364694152940395E-001;
  SP.dnu[10][36] = -1.5246342245494064188778E-003;
  SP.xnu[10][37] =  5.5663547608878217450911E-001;
  SP.dnu[10][37] =  1.5241082935781717091746E-003;
  SP.xnu[10][38] =  5.5815931545595238362627E-001;
  SP.dnu[10][38] = -1.5235680170779262504608E-003;
  SP.xnu[10][39] =  5.5968260740549568229682E-001;
  SP.dnu[10][39] =  1.5230134591804520498512E-003;
  SP.xnu[10][40] =  5.6120533759960809924935E-001;
  SP.dnu[10][40] = -1.5224445661437683489186E-003;
  SP.xnu[10][41] =  5.6272749170577311927117E-001;
  SP.dnu[10][41] =  1.5218612841865904856848E-003;
  SP.xnu[10][42] =  5.6424905539689658631221E-001;
  SP.dnu[10][42] = -1.5212636778454703827775E-003;
  SP.xnu[10][43] =  5.6577001435144155555169E-001;
  SP.dnu[10][43] =  1.5206518119753233572674E-003;
  SP.xnu[10][44] =  5.6729035425356309315818E-001;
  SP.dnu[10][44] = -1.5200256331993559955299E-003;
  SP.xnu[10][45] =  5.6881006079324302247422E-001;
  SP.dnu[10][45] =  1.5193850880825948034766E-003;
  SP.xnu[10][46] =  5.7032911966642461535739E-001;
  SP.dnu[10][46] = -1.5187302418840958350426E-003;
  SP.xnu[10][47] =  5.7184751657514722740989E-001;
  SP.dnu[10][47] =  1.5180611601998423582806E-003;
  SP.xnu[10][48] =  5.7336523722768087582943E-001;
  SP.dnu[10][48] = -1.5173777899986955330268E-003;
  SP.xnu[10][49] =  5.7488226733866075861485E-001;
  SP.dnu[10][49] =  1.5166800781722013943996E-003;
  SP.xnu[10][50] =  5.7639859262922171386017E-001;
  SP.dnu[10][50] = -1.5159680907197938647460E-003;
  SP.xnu[10][51] =  5.7791419882713261787155E-001;
  SP.dnu[10][51] =  1.5152418939966284814247E-003;
  SP.xnu[10][52] =  5.7942907166693072084219E-001;
  SP.dnu[10][52] = -1.5145014352973685670833E-003;
  SP.xnu[10][53] =  5.8094319689005591882069E-001;
  SP.dnu[10][53] =  1.5137466618199437290487E-003;
  SP.xnu[10][54] =  5.8245656024498496070909E-001;
  SP.dnu[10][54] = -1.5129776403222092717806E-003;
  SP.xnu[10][55] =  5.8396914748736558902737E-001;
  SP.dnu[10][55] =  1.5121944379367921891110E-003;
  SP.xnu[10][56] =  5.8548094438015061318182E-001;
  SP.dnu[10][56] = -1.5113970022639613502915E-003;
  SP.xnu[10][57] =  5.8699193669373191397535E-001;
  SP.dnu[10][57] =  1.5105852807875243932095E-003;
  SP.xnu[10][58] =  5.8850211020607437809842E-001;
  SP.dnu[10][58] = -1.5097593410419964952274E-003;
  SP.xnu[10][59] =  5.9001145070284976133995E-001;
  SP.dnu[10][59] =  1.5089192509559961056737E-003;
  SP.xnu[10][60] =  5.9151994397757047925827E-001;
  SP.dnu[10][60] = -1.5080649584148315616608E-003;
  SP.xnu[10][61] =  5.9302757583172332405272E-001;
  SP.dnu[10][61] =  1.5071964111672873747377E-003;
  SP.xnu[10][62] =  5.9453433207490310637750E-001;
  SP.dnu[10][62] = -1.5063136775429980149673E-003;
  SP.xnu[10][63] =  5.9604019852494622083964E-001;
  SP.dnu[10][63] =  1.5054168262853409677864E-003;
  SP.xnu[10][64] =  5.9754516100806413392414E-001;
  SP.dnu[10][64] = -1.5045058055437024756523E-003;
  SP.xnu[10][65] =  5.9904920535897679308966E-001;
  SP.dnu[10][65] =  1.5035805633105202795872E-003;
  SP.xnu[10][66] =  6.0055231742104595577922E-001;
  SP.dnu[10][66] = -1.5026411687292552697567E-003;
  SP.xnu[10][67] =  6.0205448304640843709085E-001;
  SP.dnu[10][67] =  1.5016876913770868054488E-003;
  SP.xnu[10][68] =  6.0355568809610927485406E-001;
  SP.dnu[10][68] = -1.5007200796460951255080E-003;
  SP.xnu[10][69] =  6.0505591844023481085874E-001;
  SP.dnu[10][69] =  1.4997382817505972742770E-003;
  SP.xnu[10][70] =  6.0655515995804568698388E-001;
  SP.dnu[10][70] = -1.4987423676668630813884E-003;
  SP.xnu[10][71] =  6.0805339853810975497419E-001;
  SP.dnu[10][71] =  1.4977324078252205395236E-003;
  SP.xnu[10][72] =  6.0955062007843489861387E-001;
  SP.dnu[10][72] = -1.4967083508386098432168E-003;
  SP.xnu[10][73] =  6.1104681048660176704705E-001;
  SP.dnu[10][73] =  1.4956701451209743290442E-003;
  SP.xnu[10][74] =  6.1254195567989641799582E-001;
  SP.dnu[10][74] = -1.4946178615006793508668E-003;
  SP.xnu[10][75] =  6.1403604158544286962723E-001;
  SP.dnu[10][75] =  1.4935515712808819578986E-003;
  SP.xnu[10][76] =  6.1552905414033555982162E-001;
  SP.dnu[10][76] = -1.4924712232730693343423E-003;
  SP.xnu[10][77] =  6.1702097929177171159562E-001;
  SP.dnu[10][77] =  1.4913767660680491111396E-003;
  SP.xnu[10][78] =  6.1851180299718360343387E-001;
  SP.dnu[10][78] = -1.4902682713659025670492E-003;
  SP.xnu[10][79] =  6.2000151122437074328446E-001;
  SP.dnu[10][79] =  1.4891458113626608066472E-003;
  SP.xnu[10][80] =  6.2149008995163194497414E-001;
  SP.dnu[10][80] = -1.4880093350455362200078E-003;
  SP.xnu[10][81] =  6.2297752516789730579996E-001;
  SP.dnu[10][81] =  1.4868587911588986510693E-003;
  SP.xnu[10][82] =  6.2446380287286008405534E-001;
  SP.dnu[10][82] = -1.4856942522945304408141E-003;
  SP.xnu[10][83] =  6.2594890907710847524905E-001;
  SP.dnu[10][83] =  1.4845157915617785049293E-003;
  SP.xnu[10][84] =  6.2743282980225728577699E-001;
  SP.dnu[10][84] = -1.4833233581002187504742E-003;
  SP.xnu[10][85] =  6.2891555108107950280724E-001;
  SP.dnu[10][85] =  1.4821169007839086759607E-003;
  SP.xnu[10][86] =  6.3039705895763775914009E-001;
  SP.dnu[10][86] = -1.4808964931167137484002E-003;
  SP.xnu[10][87] =  6.3187733948741569180567E-001;
  SP.dnu[10][87] =  1.4796622091421687627184E-003;
  SP.xnu[10][88] =  6.3335637873744919316264E-001;
  SP.dnu[10][88] = -1.4784139981282791648469E-003;
  SP.xnu[10][89] =  6.3483416278645755326273E-001;
  SP.dnu[10][89] =  1.4771518090543092735035E-003;
  SP.xnu[10][90] =  6.3631067772497449224667E-001;
  SP.dnu[10][90] = -1.4758757163570202359985E-003;
  SP.xnu[10][91] =  6.3778590965547908153821E-001;
  SP.dnu[10][91] =  1.4745857950354721486242E-003;
  SP.xnu[10][92] =  6.3925984469252655260392E-001;
  SP.dnu[10][92] = -1.4732819944615599393878E-003;
  SP.xnu[10][93] =  6.4073246896287899204762E-001;
  SP.dnu[10][93] =  1.4719642636946323169885E-003;
  SP.xnu[10][94] =  6.4220376860563592180916E-001;
  SP.dnu[10][94] = -1.4706326781256242038489E-003;
  SP.xnu[10][95] =  6.4367372977236476323867E-001;
  SP.dnu[10][95] =  1.4692873137309604210017E-003;
  SP.xnu[10][96] =  6.4514233862723118381810E-001;
  SP.dnu[10][96] = -1.4679281199612439325855E-003;
  SP.xnu[10][97] =  6.4660958134712932530330E-001;
  SP.dnu[10][97] =  1.4665550459301068467069E-003;
  SP.xnu[10][98] =  6.4807544412181191206089E-001;
  SP.dnu[10][98] = -1.4651681680044381518819E-003;
  SP.xnu[10][99] =  6.4953991315402023837517E-001;
  SP.dnu[10][99] =  1.4637675631604071990705E-003;
  SP.xnu[10][100] =  6.5100297465961403350173E-001;
  SP.dnu[10][100] = -1.4623531809014651984258E-003;
  SP.xnu[10][101] =  6.5246461486770120324536E-001;
  SP.dnu[10][101] =  1.4609249703690093652720E-003;
  SP.xnu[10][102] =  6.5392482002076744684103E-001;
  SP.dnu[10][102] = -1.4594830089282036302314E-003;
  SP.xnu[10][103] =  6.5538357637480574791800E-001;
  SP.dnu[10][103] =  1.4580273745779223118089E-003;
  SP.xnu[10][104] =  6.5684087019944573832824E-001;
  SP.dnu[10][104] = -1.4565580168478880001075E-003;
  SP.xnu[10][105] =  6.5829668777808293362152E-001;
  SP.dnu[10][105] =  1.4550748848799867641745E-003;
  SP.xnu[10][106] =  6.5975101540800783895076E-001;
  SP.dnu[10][106] = -1.4535780570605591966085E-003;
  SP.xnu[10][107] =  6.6120383940053492419240E-001;
  SP.dnu[10][107] =  1.4520676124347679208081E-003;
  SP.xnu[10][108] =  6.6265514608113146706798E-001;
  SP.dnu[10][108] = -1.4505435005312723146414E-003;
  SP.xnu[10][109] =  6.6410492178954626305396E-001;
  SP.dnu[10][109] =  1.4490056704643703559507E-003;
  SP.xnu[10][110] =  6.6555315287993820086860E-001;
  SP.dnu[10][110] = -1.4474542016651041384514E-003;
  SP.xnu[10][111] =  6.6699982572100470232543E-001;
  SP.dnu[10][111] =  1.4458891742491752689695E-003;
  SP.xnu[10][112] =  6.6844492669611002534463E-001;
  SP.dnu[10][112] = -1.4443105377160448742544E-003;
  SP.xnu[10][113] =  6.6988844220341342891441E-001;
  SP.dnu[10][113] =  1.4427182411235002406392E-003;
  SP.xnu[10][114] =  6.7133035865599719879639E-001;
  SP.dnu[10][114] = -1.4411123649714773708329E-003;
  SP.xnu[10][115] =  6.7277066248199453276960E-001;
  SP.dnu[10][115] =  1.4394929904711816597823E-003;
  SP.xnu[10][116] =  6.7420934012471728420965E-001;
  SP.dnu[10][116] = -1.4378600670638955431776E-003;
  SP.xnu[10][117] =  6.7564637804278356280065E-001;
  SP.dnu[10][117] =  1.4362135437210799865860E-003;
  SP.xnu[10][118] =  6.7708176271024519117870E-001;
  SP.dnu[10][118] = -1.4345535020364716711948E-003;
  SP.xnu[10][119] =  6.7851548061671501630748E-001;
  SP.dnu[10][119] =  1.4328800243425080218149E-003;
  SP.xnu[10][120] =  6.7994751826749407438755E-001;
  SP.dnu[10][120] = -1.4311930599924195781292E-003;
  SP.xnu[10][121] =  6.8137786218369860810243E-001;
  SP.dnu[10][121] =  1.4294925578405823540388E-003;
  SP.xnu[10][122] =  6.8280649890238693500587E-001;
  SP.dnu[10][122] = -1.4277786006002041589725E-003;
  SP.xnu[10][123] =  6.8423341497668616585637E-001;
  SP.dnu[10][123] =  1.4260512717514981598869E-003;
  SP.xnu[10][124] =  6.8565859697591877170597E-001;
  SP.dnu[10][124] = -1.4243105205288270674881E-003;
  SP.xnu[10][125] =  6.8708203148572899855220E-001;
  SP.dnu[10][125] =  1.4225562956377275352249E-003;
  SP.xnu[10][126] =  6.8850370510820912836328E-001;
  SP.dnu[10][126] = -1.4207886809373646719469E-003;
  SP.xnu[10][127] =  6.8992360446202558528814E-001;
  SP.dnu[10][127] =  1.4190077610831415380310E-003;
  SP.xnu[10][128] =  6.9134171618254488586423E-001;
  SP.dnu[10][128] = -1.4172134851587415877036E-003;
  SP.xnu[10][129] =  6.9275802692195943203780E-001;
  SP.dnu[10][129] =  1.4154058016880561266173E-003;
  SP.xnu[10][130] =  6.9417252334941314581250E-001;
  SP.dnu[10][130] = -1.4135847957035644316010E-003;
  SP.xnu[10][131] =  6.9558519215112694434376E-001;
  SP.dnu[10][131] =  1.4117505530642021797109E-003;
  SP.xnu[10][132] =  6.9699602003052405429809E-001;
  SP.dnu[10][132] = -1.4099030226701108557841E-003;
  SP.xnu[10][133] =  6.9840499370835516429765E-001;
  SP.dnu[10][133] =  1.4080421528296197877628E-003;
  SP.xnu[10][134] =  6.9981209992282341427206E-001;
  SP.dnu[10][134] = -1.4061680297768081267422E-003;
  SP.xnu[10][135] =  7.0121732542970922054127E-001;
  SP.dnu[10][135] =  1.4042807406034770077082E-003;
  SP.xnu[10][136] =  7.0262065700249493545424E-001;
  SP.dnu[10][136] = -1.4023802339922528936517E-003;
  SP.xnu[10][137] =  7.0402208143248934041037E-001;
  SP.dnu[10][137] =  1.4004664580008132762428E-003;
  SP.xnu[10][138] =  7.0542158552895197109173E-001;
  SP.dnu[10][138] = -1.3985395000941132780557E-003;
  SP.xnu[10][139] =  7.0681915611921727373597E-001;
  SP.dnu[10][139] =  1.3965994486272076795154E-003;
  SP.xnu[10][140] =  7.0821478004881859128130E-001;
  SP.dnu[10][140] = -1.3946462520300619536439E-003;
  SP.xnu[10][141] =  7.0960844418161197821651E-001;
  SP.dnu[10][141] =  1.3926798580733722799689E-003;
  SP.xnu[10][142] =  7.1100013539989984297064E-001;
  SP.dnu[10][142] = -1.3907003554833014759612E-003;
  SP.xnu[10][143] =  7.1238984060455441667861E-001;
  SP.dnu[10][143] =  1.3887078339096707038788E-003;
  SP.xnu[10][144] =  7.1377754671514104716048E-001;
  SP.dnu[10][144] = -1.3867022414933991843835E-003;
  SP.xnu[10][145] =  7.1516324067004131695410E-001;
  SP.dnu[10][145] =  1.3846835256805621959568E-003;
  SP.xnu[10][146] =  7.1654690942657598424211E-001;
  SP.dnu[10][146] = -1.3826517764899868100272E-003;
  SP.xnu[10][147] =  7.1792853996112774551627E-001;
  SP.dnu[10][147] =  1.3806070848989713501879E-003;
  SP.xnu[10][148] =  7.1930811926926381882351E-001;
  SP.dnu[10][148] = -1.3785493987216937425263E-003;
  SP.xnu[10][149] =  7.2068563436585834643999E-001;
  SP.dnu[10][149] =  1.3764786650405837288714E-003;
  SP.xnu[10][150] =  7.2206107228521461582103E-001;
  SP.dnu[10][150] = -1.3743949751997875301379E-003;
  SP.xnu[10][151] =  7.2343442008118709767652E-001;
  SP.dnu[10][151] =  1.3722984215380675846230E-003;
  SP.xnu[10][152] =  7.2480566482730330002315E-001;
  SP.dnu[10][152] = -1.3701889515037807784375E-003;
  SP.xnu[10][153] =  7.2617479361688543706651E-001;
  SP.dnu[10][153] =  1.3680665117752219028797E-003;
  SP.xnu[10][154] =  7.2754179356317191176793E-001;
  SP.dnu[10][154] = -1.3659311950557876974700E-003;
  SP.xnu[10][155] =  7.2890665179943861095248E-001;
  SP.dnu[10][155] =  1.3637830950810509852194E-003;
  SP.xnu[10][156] =  7.3026935547912001181659E-001;
  SP.dnu[10][156] = -1.3616221588930034425526E-003;
  SP.xnu[10][157] =  7.3162989177593009869536E-001;
  SP.dnu[10][157] =  1.3594483327237657965767E-003;
  SP.xnu[10][158] =  7.3298824788398308895138E-001;
  SP.dnu[10][158] = -1.3572617106712762970091E-003;
  SP.xnu[10][159] =  7.3434441101791396684881E-001;
  SP.dnu[10][159] =  1.3550623879047123021884E-003;
  SP.xnu[10][160] =  7.3569836841299882427819E-001;
  SP.dnu[10][160] = -1.3528503110176067739475E-003;
  SP.xnu[10][161] =  7.3705010732527500719929E-001;
  SP.dnu[10][161] =  1.3506254257522270228547E-003;
  SP.xnu[10][162] =  7.3839961503166106667108E-001;
  SP.dnu[10][162] = -1.3483878276377919926748E-003;
  SP.xnu[10][163] =  7.3974687883007651333992E-001;
  SP.dnu[10][163] =  1.3461376133154200398390E-003;
  SP.xnu[10][164] =  7.4109188603956137425867E-001;
  SP.dnu[10][164] = -1.3438747288864520433996E-003;
  SP.xnu[10][165] =  7.4243462400039555091148E-001;
  SP.dnu[10][165] =  1.3415991195578856834608E-003;
  SP.xnu[10][166] =  7.4377508007421797732074E-001;
  SP.dnu[10][166] = -1.3393108823285024110058E-003;
  SP.xnu[10][167] =  7.4511324164414557711480E-001;
  SP.dnu[10][167] =  1.3370101153513411421596E-003;
  SP.xnu[10][168] =  7.4644909611489201843651E-001;
  SP.dnu[10][168] = -1.3346967641900808298697E-003;
  SP.xnu[10][169] =  7.4778263091288626557513E-001;
  SP.dnu[10][169] =  1.3323707734691932315184E-003;
  SP.xnu[10][170] =  7.4911383348639092620549E-001;
  SP.dnu[10][170] = -1.3300322416969475396531E-003;
  SP.xnu[10][171] =  7.5044269130562039312064E-001;
  SP.dnu[10][171] =  1.3276812685800335654414E-003;
  SP.xnu[10][172] =  7.5176919186285877934593E-001;
  SP.dnu[10][172] = -1.3253177990971588118054E-003;
  SP.xnu[10][173] =  7.5309332267257764552447E-001;
  SP.dnu[10][173] =  1.3229417772410623743751E-003;
  SP.xnu[10][174] =  7.5441507127155351846587E-001;
  SP.dnu[10][174] = -1.3205533030711775224855E-003;
  SP.xnu[10][175] =  7.5573442521898519975220E-001;
  SP.dnu[10][175] =  1.3181524778914412181460E-003;
  SP.xnu[10][176] =  7.5705137209661086329685E-001;
  SP.dnu[10][176] = -1.3157392460463299528784E-003;
  SP.xnu[10][177] =  7.5836589950882494075438E-001;
  SP.dnu[10][177] =  1.3133135508455748436674E-003;
  SP.xnu[10][178] =  7.5967799508279479368091E-001;
  SP.dnu[10][178] = -1.3108754939433158238108E-003;
  SP.xnu[10][179] =  7.6098764646857717134713E-001;
  SP.dnu[10][179] =  1.3084251782863224404311E-003;
  SP.xnu[10][180] =  7.6229484133923445310755E-001;
  SP.dnu[10][180] = -1.3059625475335124556444E-003;
  SP.xnu[10][181] =  7.6359956739095067423214E-001;
  SP.dnu[10][181] =  1.3034875442581385493600E-003;
  SP.xnu[10][182] =  7.6490181234314733410803E-001;
  SP.dnu[10][182] = -1.3010002717545794198915E-003;
  SP.xnu[10][183] =  7.6620156393859898572140E-001;
  SP.dnu[10][183] =  1.2985008346601438832431E-003;
  SP.xnu[10][184] =  7.6749880994354860533154E-001;
  SP.dnu[10][184] = -1.2959891758946685461041E-003;
  SP.xnu[10][185] =  7.6879353814782274125111E-001;
  SP.dnu[10][185] =  1.2934652372391263196139E-003;
  SP.xnu[10][186] =  7.7008573636494644064892E-001;
  SP.dnu[10][186] = -1.2909291236757883564448E-003;
  SP.xnu[10][187] =  7.7137539243225795329338E-001;
  SP.dnu[10][187] =  1.2883809415824723295590E-003;
  SP.xnu[10][188] =  7.7266249421102321115699E-001;
  SP.dnu[10][188] = -1.2858206330840808371752E-003;
  SP.xnu[10][189] =  7.7394702958655008280441E-001;
  SP.dnu[10][189] =  1.2832481391110291085633E-003;
  SP.xnu[10][190] =  7.7522898646830240148864E-001;
  SP.dnu[10][190] = -1.2806635663833976860082E-003;
  SP.xnu[10][191] =  7.7650835279001376588211E-001;
  SP.dnu[10][191] =  1.2780670230718976781062E-003;
  SP.xnu[10][192] =  7.7778511650980111237142E-001;
  SP.dnu[10][192] = -1.2754584504481686996097E-003;
  SP.xnu[10][193] =  7.7905926561027805784685E-001;
  SP.dnu[10][193] =  1.2728377885311572293181E-003;
  SP.xnu[10][194] =  7.8033078809866801191986E-001;
  SP.dnu[10][194] = -1.2702051458310854687504E-003;
  SP.xnu[10][195] =  7.8159967200691705750368E-001;
  SP.dnu[10][195] =  1.2675606323665209825979E-003;
  SP.xnu[10][196] =  7.8286590539180659869488E-001;
  SP.dnu[10][196] = -1.2649041884948787448723E-003;
  SP.xnu[10][197] =  7.8412947633506577489527E-001;
  SP.dnu[10][197] =  1.2622357532599238396658E-003;
  SP.xnu[10][198] =  7.8539037294348364011633E-001;
  SP.dnu[10][198] = -1.2595554370169311843485E-003;
  SP.xnu[10][199] =  7.8664858334902110641009E-001;
  SP.dnu[10][199] =  1.2568633516900421070930E-003;
  SP.xnu[10][200] =  7.8790409570892265037299E-001;
  SP.dnu[10][200] = -1.2541594366586841958931E-003;
  SP.xnu[10][201] =  7.8915689820582778167112E-001;
  SP.dnu[10][201] =  1.2514436299247455729073E-003;
  SP.xnu[10][202] =  7.9040697904788227253780E-001;
  SP.dnu[10][202] = -1.2487160437462195607461E-003;
  SP.xnu[10][203] =  7.9165432646884914719642E-001;
  SP.dnu[10][203] =  1.2459767920134822204160E-003;
  SP.xnu[10][204] =  7.9289892872821943016404E-001;
  SP.dnu[10][204] = -1.2432258130612285884014E-003;
  SP.xnu[10][205] =  7.9414077411132265239322E-001;
  SP.dnu[10][205] =  1.2404630437795958659234E-003;
  SP.xnu[10][206] =  7.9537985092943711421194E-001;
  SP.dnu[10][206] = -1.2376885983899054777594E-003;
  SP.xnu[10][207] =  7.9661614751989990402390E-001;
  SP.dnu[10][207] =  1.2349025928125770095446E-003;
  SP.xnu[10][208] =  7.9784965224621667173352E-001;
  SP.dnu[10][208] = -1.2321049642676499077365E-003;
  SP.xnu[10][209] =  7.9908035349817115586248E-001;
  SP.dnu[10][209] =  1.2292956484602471908261E-003;
  SP.xnu[10][210] =  8.0030823969193446332694E-001;
  SP.dnu[10][210] = -1.2264747616387762493990E-003;
  SP.xnu[10][211] =  8.0153329927017410084672E-001;
  SP.dnu[10][211] =  1.2236424218208771433896E-003;
  SP.xnu[10][212] =  8.0275552070216275696031E-001;
  SP.dnu[10][212] = -1.2207985650386219236077E-003;
  SP.xnu[10][213] =  8.0397489248388683362182E-001;
  SP.dnu[10][213] =  1.2179431257352390453005E-003;
  SP.xnu[10][214] =  8.0519140313815472635818E-001;
  SP.dnu[10][214] = -1.2150762222533482281955E-003;
  SP.xnu[10][215] =  8.0640504121470485196761E-001;
  SP.dnu[10][215] =  1.2121979747785931643461E-003;
  SP.xnu[10][216] =  8.0761579529031342274246E-001;
  SP.dnu[10][216] = -1.2093083180781501382860E-003;
  SP.xnu[10][217] =  8.0882365396890196620199E-001;
  SP.dnu[10][217] =  1.2064071852526091998711E-003;
  SP.xnu[10][218] =  8.1002860588164458932313E-001;
  SP.dnu[10][218] = -1.2034946968095353075064E-003;
  SP.xnu[10][219] =  8.1123063968707498625958E-001;
  SP.dnu[10][219] =  1.2005709751772226252774E-003;
  SP.xnu[10][220] =  8.1242974407119318854204E-001;
  SP.dnu[10][220] = -1.1976359537771604121785E-003;
  SP.xnu[10][221] =  8.1362590774757205675481E-001;
  SP.dnu[10][221] =  1.1946895642824263377372E-003;
  SP.xnu[10][222] =  8.1481911945746351268649E-001;
  SP.dnu[10][222] = -1.1917319294401275234706E-003;
  SP.xnu[10][223] =  8.1600936796990451095470E-001;
  SP.dnu[10][223] =  1.1887631739999979241003E-003;
  SP.xnu[10][224] =  8.1719664208182274910759E-001;
  SP.dnu[10][224] = -1.1857832299529189747547E-003;
  SP.xnu[10][225] =  8.1838093061814211520697E-001;
  SP.dnu[10][225] =  1.1827920274551628543991E-003;
  SP.xnu[10][226] =  8.1956222243188787190074E-001;
  SP.dnu[10][226] = -1.1797896915721185766608E-003;
  SP.xnu[10][227] =  8.2074050640429157599437E-001;
  SP.dnu[10][227] =  1.1767763494581939167606E-003;
  SP.xnu[10][228] =  8.2191577144489573253404E-001;
  SP.dnu[10][228] = -1.1737519315843231684476E-003;
  SP.xnu[10][229] =  8.2308800649165818241640E-001;
  SP.dnu[10][229] =  1.1707163664959472099326E-003;
  SP.xnu[10][230] =  8.2425720051105622254228E-001;
  SP.dnu[10][230] = -1.1676697816599217042280E-003;
  SP.xnu[10][231] =  8.2542334249819045753449E-001;
  SP.dnu[10][231] =  1.1646123067233350120147E-003;
  SP.xnu[10][232] =  8.2658642147688838204210E-001;
  SP.dnu[10][232] = -1.1615438705431029085906E-003;
  SP.xnu[10][233] =  8.2774642649980769265634E-001;
  SP.dnu[10][233] =  1.1584643999547358463574E-003;
  SP.xnu[10][234] =  8.2890334664853932846559E-001;
  SP.dnu[10][234] = -1.1553740249145139360573E-003;
  SP.xnu[10][235] =  8.3005717103371023927975E-001;
  SP.dnu[10][235] =  1.1522728776553420679504E-003;
  SP.xnu[10][236] =  8.3120788879508588055653E-001;
  SP.dnu[10][236] = -1.1491608853209734738390E-003;
  SP.xnu[10][237] =  8.3235548910167243406516E-001;
  SP.dnu[10][237] =  1.1460379729324452959696E-003;
  SP.xnu[10][238] =  8.3349996115181875332508E-001;
  SP.dnu[10][238] = -1.1429042730285493627356E-003;
  SP.xnu[10][239] =  8.3464129417331803286035E-001;
  SP.dnu[10][239] =  1.1397599205266600204847E-003;
  SP.xnu[10][240] =  8.3577947742350920031269E-001;
  SP.dnu[10][240] = -1.1366048407527808691004E-003;
  SP.xnu[10][241] =  8.3691450018937803045878E-001;
  SP.dnu[10][241] =  1.1334389568030857136534E-003;
  SP.xnu[10][242] =  8.3804635178765798018021E-001;
  SP.dnu[10][242] = -1.1302624038974826284840E-003;
  SP.xnu[10][243] =  8.3917502156493074343683E-001;
  SP.dnu[10][243] =  1.1270753197424077780333E-003;
  SP.xnu[10][244] =  8.4030049889772652529722E-001;
  SP.dnu[10][244] = -1.1238776277356816270362E-003;
  SP.xnu[10][245] =  8.4142277319262403408230E-001;
  SP.dnu[10][245] =  1.1206692489319376669400E-003;
  SP.xnu[10][246] =  8.4254183388635019068103E-001;
  SP.dnu[10][246] = -1.1174503213367444380723E-003;
  SP.xnu[10][247] =  8.4365767044587955409959E-001;
  SP.dnu[10][247] =  1.1142209855565925140628E-003;
  SP.xnu[10][248] =  8.4477027236853346230837E-001;
  SP.dnu[10][248] = -1.1109811629443995350581E-003;
  SP.xnu[10][249] =  8.4587962918207888745337E-001;
  SP.dnu[10][249] =  1.1077307723898146118155E-003;
  SP.xnu[10][250] =  8.4698573044482700450187E-001;
  SP.dnu[10][250] = -1.1044699547950114324756E-003;
  SP.xnu[10][251] =  8.4808856574573147239429E-001;
  SP.dnu[10][251] =  1.1011988537844310801589E-003;
  SP.xnu[10][252] =  8.4918812470448642677741E-001;
  SP.dnu[10][252] = -1.0979173885426023928669E-003;
  SP.xnu[10][253] =  8.5028439697162418339643E-001;
  SP.dnu[10][253] =  1.0946254756634540698707E-003;
  SP.xnu[10][254] =  8.5137737222861265122646E-001;
  SP.dnu[10][254] = -1.0913232590636133429008E-003;
  SP.xnu[10][255] =  8.5246704018795245442626E-001;
  SP.dnu[10][255] =  1.0880108855108218464677E-003;
  SP.xnu[10][256] =  8.5355339059327376220042E-001;
  SP.dnu[10][256] = -1.0846882718904425215006E-003;
  SP.xnu[10][257] =  8.5463641321943282565827E-001;
  SP.dnu[10][257] =  1.0813553323620811032006E-003;
  SP.xnu[10][258] =  8.5571609787260822076107E-001;
  SP.dnu[10][258] = -1.0780122139821208759543E-003;
  SP.xnu[10][259] =  8.5679243439039679645156E-001;
  SP.dnu[10][259] =  1.0746590667950108537397E-003;
  SP.xnu[10][260] =  8.5786541264190932706277E-001;
  SP.dnu[10][260] = -1.0712958052483053592428E-003;
  SP.xnu[10][261] =  8.5893502252786586810566E-001;
  SP.dnu[10][261] =  1.0679223409201882573587E-003;
  SP.xnu[10][262] =  8.6000125398069081453834E-001;
  SP.dnu[10][262] = -1.0645388241401583131016E-003;
  SP.xnu[10][263] =  8.6106409696460766062180E-001;
  SP.dnu[10][263] =  1.0611454083714967315927E-003;
  SP.xnu[10][264] =  8.6212354147573346047053E-001;
  SP.dnu[10][264] = -1.0577420054768110933917E-003;
  SP.xnu[10][265] =  8.6317957754217298840875E-001;
  SP.dnu[10][265] =  1.0543285242965767095064E-003;
  SP.xnu[10][266] =  8.6423219522411259824602E-001;
  SP.dnu[10][266] = -1.0509051185754853240192E-003;
  SP.xnu[10][267] =  8.6528138461391378058888E-001;
  SP.dnu[10][267] =  1.0474719453472194008491E-003;
  SP.xnu[10][268] =  8.6632713583620641730777E-001;
  SP.dnu[10][268] = -1.0440289137331148907993E-003;
  SP.xnu[10][269] =  8.6736943904798173228161E-001;
  SP.dnu[10][269] =  1.0405759296697039185018E-003;
  SP.xnu[10][270] =  8.6840828443868493754507E-001;
  SP.dnu[10][270] = -1.0371131504683929945082E-003;
  SP.xnu[10][271] =  8.6944366223030757396656E-001;
  SP.dnu[10][271] =  1.0336407368950781337332E-003;
  SP.xnu[10][272] =  8.7047556267747954558781E-001;
  SP.dnu[10][272] = -1.0301585951635519054533E-003;
  SP.xnu[10][273] =  8.7150397606756084675868E-001;
  SP.dnu[10][273] =  1.0266666281293836258799E-003;
  SP.xnu[10][274] =  8.7252889272073298120395E-001;
  SP.dnu[10][274] = -1.0231649968324595536691E-003;
  SP.xnu[10][275] =  8.7355030299009007216154E-001;
  SP.dnu[10][275] =  1.0196538659438250941464E-003;
  SP.xnu[10][276] =  8.7456819726172966273460E-001;
  SP.dnu[10][276] = -1.0161331385926738596672E-003;
  SP.xnu[10][277] =  8.7558256595484320560291E-001;
  SP.dnu[10][277] =  1.0126027143648846012866E-003;
  SP.xnu[10][278] =  8.7659339952180624124172E-001;
  SP.dnu[10][278] = -1.0090627582017117499911E-003;
  SP.xnu[10][279] =  8.7760068844826826379936E-001;
  SP.dnu[10][279] =  1.0055134388643810209780E-003;
  SP.xnu[10][280] =  8.7860442325324227378773E-001;
  SP.dnu[10][280] = -1.0019546562087246184609E-003;
  SP.xnu[10][281] =  8.7960459448919401674276E-001;
  SP.dnu[10][281] =  9.9838630634947506273420E-004;
  SP.xnu[10][282] =  8.8060119274213090701485E-001;
  SP.dnu[10][282] = -9.9480855831423826946967E-004;
  SP.xnu[10][283] =  8.8159420863169063585242E-001;
  SP.dnu[10][283] =  9.9122158515262025021541E-004;
  SP.xnu[10][284] =  8.8258363281122946294441E-001;
  SP.dnu[10][284] = -9.8762528324560280663922E-004;
  SP.xnu[10][285] =  8.8356945596791019059085E-001;
  SP.dnu[10][285] =  9.8401954502146023079919E-004;
  SP.xnu[10][286] =  8.8455166882278981967331E-001;
  SP.dnu[10][286] = -9.8040454379230200791405E-004;
  SP.xnu[10][287] =  8.8553026213090688660030E-001;
  SP.dnu[10][287] =  9.7678045710867279625312E-004;
  SP.xnu[10][288] =  8.8650522668136848040545E-001;
  SP.dnu[10][288] = -9.7314717766136015760871E-004;
  SP.xnu[10][289] =  8.8747655329743693917956E-001;
  SP.dnu[10][289] =  9.6950459396176099631735E-004;
  SP.xnu[10][290] =  8.8844423283661622502041E-001;
  SP.dnu[10][290] = -9.6585288381899840068781E-004;
  SP.xnu[10][291] =  8.8940825619073797668736E-001;
  SP.dnu[10][291] =  9.6219222951279172895043E-004;
  SP.xnu[10][292] =  8.9036861428604723915079E-001;
  SP.dnu[10][292] = -9.5852251981328493539376E-004;
  SP.xnu[10][293] =  8.9132529808328786922947E-001;
  SP.dnu[10][293] =  9.5484363906808219291951E-004;
  SP.xnu[10][294] =  8.9227829857778761651195E-001;
  SP.dnu[10][294] = -9.5115576981150737194093E-004;
  SP.xnu[10][295] =  8.9322760679954287876116E-001;
  SP.dnu[10][295] =  9.4745909929783459032459E-004;
  SP.xnu[10][296] =  8.9417321381330313100458E-001;
  SP.dnu[10][296] = -9.4375351212962044068883E-004;
  SP.xnu[10][297] =  8.9511511071865502751511E-001;
  SP.dnu[10][297] =  9.4003888822571946853526E-004;
  SP.xnu[10][298] =  8.9605328865010617589117E-001;
  SP.dnu[10][298] = -9.3631541509098678666722E-004;
  SP.xnu[10][299] =  8.9698773877716858244754E-001;
  SP.dnu[10][299] =  9.3258328521840809288674E-004;
  SP.xnu[10][300] =  8.9791845230444176813140E-001;
  SP.dnu[10][300] = -9.2884237877796930310128E-004;
  SP.xnu[10][301] =  8.9884542047169555418133E-001;
  SP.dnu[10][301] =  9.2509257097505424332992E-004;
  SP.xnu[10][302] =  8.9976863455395251675012E-001;
  SP.dnu[10][302] = -9.2133405454915556619744E-004;
  SP.xnu[10][303] =  9.0068808586157010971512E-001;
  SP.dnu[10][303] =  9.1756702751672583005579E-004;
  SP.xnu[10][304] =  9.0160376574032245490334E-001;
  SP.dnu[10][304] = -9.1379136533043498118568E-004;
  SP.xnu[10][305] =  9.0251566557148179896133E-001;
  SP.dnu[10][305] =  9.1000693817578672491812E-004;
  SP.xnu[10][306] =  9.0342377677189963610326E-001;
  SP.dnu[10][306] = -9.0621394431161485305210E-004;
  SP.xnu[10][307] =  9.0432809079408749597348E-001;
  SP.dnu[10][307] =  9.0241258758522920725866E-004;
  SP.xnu[10][308] =  9.0522859912629739586335E-001;
  SP.dnu[10][308] = -8.9860273842555264710981E-004;
  SP.xnu[10][309] =  9.0612529329260195652487E-001;
  SP.dnu[10][309] =  8.9478426166795742449458E-004;
  SP.xnu[10][310] =  9.0701816485297418082726E-001;
  SP.dnu[10][310] = -8.9095736139795577457825E-004;
  SP.xnu[10][311] =  9.0790720540336689450539E-001;
  SP.dnu[10][311] =  8.8712224762602226955854E-004;
  SP.xnu[10][312] =  9.0879240657579184825246E-001;
  SP.dnu[10][312] = -8.8327878542706243336945E-004;
  SP.xnu[10][313] =  9.0967376003839848041234E-001;
  SP.dnu[10][313] =  8.7942683392980807898544E-004;
  SP.xnu[10][314] =  9.1055125749555233953022E-001;
  SP.dnu[10][314] = -8.7556660337870244423643E-004;
  SP.xnu[10][315] =  9.1142489068791316602339E-001;
  SP.dnu[10][315] =  8.7169831030717155711583E-004;
  SP.xnu[10][316] =  9.1229465139251263223740E-001;
  SP.dnu[10][316] = -8.6782181407957887680471E-004;
  SP.xnu[10][317] =  9.1316053142283174015560E-001;
  SP.dnu[10][317] =  8.6393696773253333353196E-004;
  SP.xnu[10][318] =  9.1402252262887787603376E-001;
  SP.dnu[10][318] = -8.6004398802913883950641E-004;
  SP.xnu[10][319] =  9.1488061689726152123451E-001;
  SP.dnu[10][319] =  8.5614309841592256245815E-004;
  SP.xnu[10][320] =  9.1573480615127261853939E-001;
  SP.dnu[10][320] = -8.5223415216121132521639E-004;
  SP.xnu[10][321] =  9.1658508235095659321996E-001;
  SP.dnu[10][321] =  8.4831699579197497443857E-004;
  SP.xnu[10][322] =  9.1743143749319002815220E-001;
  SP.dnu[10][322] = -8.4439185298006808698231E-004;
  SP.xnu[10][323] =  9.1827386361175599226214E-001;
  SP.dnu[10][323] =  8.4045895450888479215301E-004;
  SP.xnu[10][324] =  9.1911235277741902159350E-001;
  SP.dnu[10][324] = -8.3651814713319109296089E-004;
  SP.xnu[10][325] =  9.1994689709799975229169E-001;
  SP.dnu[10][325] =  8.3256927041731092374001E-004;
  SP.xnu[10][326] =  9.2077748871844920480175E-001;
  SP.dnu[10][326] = -8.2861255536555244419560E-004;
  SP.xnu[10][327] =  9.2160411982092271858087E-001;
  SP.dnu[10][327] =  8.2464824055923784846618E-004;
  SP.xnu[10][328] =  9.2242678262485353662979E-001;
  SP.dnu[10][328] = -8.2067616578656228781992E-004;
  SP.xnu[10][329] =  9.2324546938702603915027E-001;
  SP.dnu[10][329] =  8.1669616315679161100573E-004;
  SP.xnu[10][330] =  9.2406017240164862563957E-001;
  SP.dnu[10][330] = -8.1270847146768186349422E-004;
  SP.xnu[10][331] =  9.2487088400042624473563E-001;
  SP.dnu[10][331] =  8.0871333760101134197375E-004;
  SP.xnu[10][332] =  9.2567759655263257113065E-001;
  SP.dnu[10][332] = -8.0471059388599451243418E-004;
  SP.xnu[10][333] =  9.2648030246518182887329E-001;
  SP.dnu[10][333] =  8.0070006444057672368442E-004;
  SP.xnu[10][334] =  9.2727899418270026038393E-001;
  SP.dnu[10][334] = -7.9668199635841841707318E-004;
  SP.xnu[10][335] =  9.2807366418759724050982E-001;
  SP.dnu[10][335] =  7.9265664537049182137915E-004;
  SP.xnu[10][336] =  9.2886430500013603495113E-001;
  SP.dnu[10][336] = -7.8862383581077710824270E-004;
  SP.xnu[10][337] =  9.2965090917850420239179E-001;
  SP.dnu[10][337] =  7.8458338322072570138226E-004;
  SP.xnu[10][338] =  9.3043346931888363967229E-001;
  SP.dnu[10][338] = -7.8053554353856300434376E-004;
  SP.xnu[10][339] =  9.3121197805552026934547E-001;
  SP.dnu[10][339] =  7.7648058194481025878973E-004;
  SP.xnu[10][340] =  9.3198642806079336895907E-001;
  SP.dnu[10][340] = -7.7241831419305630767463E-004;
  SP.xnu[10][341] =  9.3275681204528454141274E-001;
  SP.dnu[10][341] =  7.6834854660839567837109E-004;
  SP.xnu[10][342] =  9.3352312275784632574010E-001;
  SP.dnu[10][342] = -7.6427154457388958944243E-004;
  SP.xnu[10][343] =  9.3428535298567044767022E-001;
  SP.dnu[10][343] =  7.6018758337776395336052E-004;
  SP.xnu[10][344] =  9.3504349555435570932615E-001;
  SP.dnu[10][344] = -7.5609646955337865509652E-004;
  SP.xnu[10][345] =  9.3579754332797551742124E-001;
  SP.dnu[10][345] =  7.5199799950830088772001E-004;
  SP.xnu[10][346] =  9.3654748920914504931804E-001;
  SP.dnu[10][346] = -7.4789244872849064538209E-004;
  SP.xnu[10][347] =  9.3729332613908805631722E-001;
  SP.dnu[10][347] =  7.4378010333292700826725E-004;
  SP.xnu[10][348] =  9.3803504709770330354792E-001;
  SP.dnu[10][348] = -7.3966075993360643143052E-004;
  SP.xnu[10][349] =  9.3877264510363064583424E-001;
  SP.dnu[10][349] =  7.3553420425048781376109E-004;
  SP.xnu[10][350] =  9.3950611321431673891566E-001;
  SP.dnu[10][350] = -7.3140072259537540502759E-004;
  SP.xnu[10][351] =  9.4023544452608038540327E-001;
  SP.dnu[10][351] =  7.2726061271410378861615E-004;
  SP.xnu[10][352] =  9.4096063217417751485638E-001;
  SP.dnu[10][352] = -7.2311366052727367923798E-004;
  SP.xnu[10][353] =  9.4168166933286579736815E-001;
  SP.dnu[10][353] =  7.1895964021948060936027E-004;
  SP.xnu[10][354] =  9.4239854921546889005200E-001;
  SP.dnu[10][354] = -7.1479884972435979772864E-004;
  SP.xnu[10][355] =  9.4311126507444031582400E-001;
  SP.dnu[10][355] =  7.1063159929317997223716E-004;
  SP.xnu[10][356] =  9.4381981020142697388009E-001;
  SP.dnu[10][356] = -7.0645766330745491579840E-004;
  SP.xnu[10][357] =  9.4452417792733228127039E-001;
  SP.dnu[10][357] =  7.0227680348085147047197E-004;
  SP.xnu[10][358] =  9.4522436162237894497608E-001;
  SP.dnu[10][358] = -6.9808933024728339993358E-004;
  SP.xnu[10][359] =  9.4592035469617136389824E-001;
  SP.dnu[10][359] =  6.9389556733542594963745E-004;
  SP.xnu[10][360] =  9.4661215059775766017121E-001;
  SP.dnu[10][360] = -6.8969527665222292789102E-004;
  SP.xnu[10][361] =  9.4729974281569133921654E-001;
  SP.dnu[10][361] =  6.8548820640527076925555E-004;
  SP.xnu[10][362] =  9.4798312487809257795728E-001;
  SP.dnu[10][362] = -6.8127468050058410469393E-004;
  SP.xnu[10][363] =  9.4866229035270914061570E-001;
  SP.dnu[10][363] =  6.7705503722230739821447E-004;
  SP.xnu[10][364] =  9.4933723284697692152099E-001;
  SP.dnu[10][364] = -6.7282902496777741145600E-004;
  SP.xnu[10][365] =  9.5000794600808011435727E-001;
  SP.dnu[10][365] =  6.6859637729009177073738E-004;
  SP.xnu[10][366] =  9.5067442352301100728537E-001;
  SP.dnu[10][366] = -6.6435743264525519380711E-004;
  SP.xnu[10][367] =  9.5133665911862940337575E-001;
  SP.dnu[10][367] =  6.6011254507185782846575E-004;
  SP.xnu[10][368] =  9.5199464656172166579310E-001;
  SP.dnu[10][368] = -6.5586144830933298358889E-004;
  SP.xnu[10][369] =  9.5264837965905938717702E-001;
  SP.dnu[10][369] =  6.5160385997852467266833E-004;
  SP.xnu[10][370] =  9.5329785225745768266648E-001;
  SP.dnu[10][370] = -6.4734013428420164141856E-004;
  SP.xnu[10][371] =  9.5394305824383310601934E-001;
  SP.dnu[10][371] =  6.4307064235666774859994E-004;
  SP.xnu[10][372] =  9.5458399154526118828194E-001;
  SP.dnu[10][372] = -6.3879510199986370004245E-004;
  SP.xnu[10][373] =  9.5522064612903359846705E-001;
  SP.dnu[10][373] =  6.3451321347645448154517E-004;
  SP.xnu[10][374] =  9.5585301600271492570220E-001;
  SP.dnu[10][374] = -6.3022534807700219959726E-004;
  SP.xnu[10][375] =  9.5648109521419908231401E-001;
  SP.dnu[10][375] =  6.2593189551954478101278E-004;
  SP.xnu[10][376] =  9.5710487785176532731751E-001;
  SP.dnu[10][376] = -6.2163255624681226492739E-004;
  SP.xnu[10][377] =  9.5772435804413390978322E-001;
  SP.dnu[10][377] =  6.1732701156695682580430E-004;
  SP.xnu[10][378] =  9.5833952996052133155823E-001;
  SP.dnu[10][378] = -6.1301565135207031110018E-004;
  SP.xnu[10][379] =  9.5895038781069522882114E-001;
  SP.dnu[10][379] =  6.0869888558689853815599E-004;
  SP.xnu[10][380] =  9.5955692584502887195424E-001;
  SP.dnu[10][380] = -6.0437639575688645042977E-004;
  SP.xnu[10][381] =  9.6015913835455528322004E-001;
  SP.dnu[10][381] =  6.0004784242256514971725E-004;
  SP.xnu[10][382] =  9.6075701967102097173270E-001;
  SP.dnu[10][382] = -5.9571363571618905789157E-004;
  SP.xnu[10][383] =  9.6135056416693928521863E-001;
  SP.dnu[10][383] =  5.9137420777990325562703E-004;
  SP.xnu[10][384] =  9.6193976625564337806409E-001;
  SP.dnu[10][384] = -5.8702921934908399839160E-004;
  SP.xnu[10][385] =  9.6252462039133879515119E-001;
  SP.dnu[10][385] =  5.8267830821534174520015E-004;
  SP.xnu[10][386] =  9.6310512106915567098740E-001;
  SP.dnu[10][386] = -5.7832190666137165502493E-004;
  SP.xnu[10][387] =  9.6368126282520054363727E-001;
  SP.dnu[10][387] =  5.7396047112348998851916E-004;
  SP.xnu[10][388] =  9.6425304023660778296858E-001;
  SP.dnu[10][388] = -5.6959363956611204745234E-004;
  SP.xnu[10][389] =  9.6482044792159063272896E-001;
  SP.dnu[10][389] =  5.6522102472480364728301E-004;
  SP.xnu[10][390] =  9.6538348053949186597244E-001;
  SP.dnu[10][390] = -5.6084308316896729624264E-004;
  SP.xnu[10][391] =  9.6594213279083405335928E-001;
  SP.dnu[10][391] =  5.5646029805321846527809E-004;
  SP.xnu[10][392] =  9.6649639941736944385583E-001;
  SP.dnu[10][392] = -5.5207228228440016131240E-004;
  SP.xnu[10][393] =  9.6704627520212945736494E-001;
  SP.dnu[10][393] =  5.4767862094375237843344E-004;
  SP.xnu[10][394] =  9.6759175496947378882110E-001;
  SP.dnu[10][394] = -5.4327979731088942385055E-004;
  SP.xnu[10][395] =  9.6813283358513912328816E-001;
  SP.dnu[10][395] =  5.3887632402007626614362E-004;
  SP.xnu[10][396] =  9.6866950595628746160095E-001;
  SP.dnu[10][396] = -5.3446778632295050947125E-004;
  SP.xnu[10][397] =  9.6920176703155405609621E-001;
  SP.dnu[10][397] =  5.3005373868205364384084E-004;
  SP.xnu[10][398] =  9.6972961180109495598133E-001;
  SP.dnu[10][398] = -5.2563469384778541109176E-004;
  SP.xnu[10][399] =  9.7025303529663416189365E-001;
  SP.dnu[10][399] =  5.2121119709324957100187E-004;
  SP.xnu[10][400] =  9.7077203259151038920625E-001;
  SP.dnu[10][400] = -5.1678280305132924106362E-004;
  SP.xnu[10][401] =  9.7128659880072343964038E-001;
  SP.dnu[10][401] =  5.1234903216840901807174E-004;
  SP.xnu[10][402] =  9.7179672908098018074765E-001;
  SP.dnu[10][402] = -5.0791042982388697547750E-004;
  SP.xnu[10][403] =  9.7230241863074013282963E-001;
  SP.dnu[10][403] =  5.0346757756090489917862E-004;
  SP.xnu[10][404] =  9.7280366269026066286547E-001;
  SP.dnu[10][404] = -4.9901999599718622802085E-004;
  SP.xnu[10][405] =  9.7330045654164178502230E-001;
  SP.dnu[10][405] =  4.9456716765015593378252E-004;
  SP.xnu[10][406] =  9.7379279550887056732669E-001;
  SP.dnu[10][406] = -4.9010967415817028194730E-004;
  SP.xnu[10][407] =  9.7428067495786514407925E-001;
  SP.dnu[10][407] =  4.8564813752901444804236E-004;
  SP.xnu[10][408] =  9.7476409029651833359797E-001;
  SP.dnu[10][408] = -4.8118204045380588032004E-004;
  SP.xnu[10][409] =  9.7524303697474086087996E-001;
  SP.dnu[10][409] =  4.7671082299112416384510E-004;
  SP.xnu[10][410] =  9.7571751048450418477459E-001;
  SP.dnu[10][410] = -4.7223510723130018411994E-004;
  SP.xnu[10][411] =  9.7618750635988292926495E-001;
  SP.dnu[10][411] =  4.6775556051824794781806E-004;
  SP.xnu[10][412] =  9.7665302017709691845837E-001;
  SP.dnu[10][412] = -4.6327162308835415521386E-004;
  SP.xnu[10][413] =  9.7711404755455281489022E-001;
  SP.dnu[10][413] =  4.5878268726756540137735E-004;
  SP.xnu[10][414] =  9.7757058415288536074908E-001;
  SP.dnu[10][414] = -4.5428942046761422046946E-004;
  SP.xnu[10][415] =  9.7802262567499822163524E-001;
  SP.dnu[10][415] =  4.4979254105894006237269E-004;
  SP.xnu[10][416] =  9.7847016786610443246790E-001;
  SP.dnu[10][416] = -4.4529144155171807438477E-004;
  SP.xnu[10][417] =  9.7891320651376644516052E-001;
  SP.dnu[10][417] =  4.4078546036215585468246E-004;
  SP.xnu[10][418] =  9.7935173744793577768732E-001;
  SP.dnu[10][418] = -4.3627531591108817401005E-004;
  SP.xnu[10][419] =  9.7978575654099226416776E-001;
  SP.dnu[10][419] =  4.3176178428412225852928E-004;
  SP.xnu[10][420] =  9.8021525970778290559952E-001;
  SP.dnu[10][420] = -4.2724420409116753269255E-004;
  SP.xnu[10][421] =  9.8064024290566032087433E-001;
  SP.dnu[10][421] =  4.2272185255604747415761E-004;
  SP.xnu[10][422] =  9.8106070213452079771480E-001;
  SP.dnu[10][422] = -4.1819550579376890546374E-004;
  SP.xnu[10][423] =  9.8147663343684194317396E-001;
  SP.dnu[10][423] =  4.1366600552057860571794E-004;
  SP.xnu[10][424] =  9.8188803289771993334323E-001;
  SP.dnu[10][424] = -4.0913262916755705627248E-004;
  SP.xnu[10][425] =  9.8229489664490636191822E-001;
  SP.dnu[10][425] =  4.0459458411890758907091E-004;
  SP.xnu[10][426] =  9.8269722084884468727542E-001;
  SP.dnu[10][426] = -4.0005271209448754338941E-004;
  SP.xnu[10][427] =  9.8309500172270627771692E-001;
  SP.dnu[10][427] =  3.9550792987784121176509E-004;
  SP.xnu[10][428] =  9.8348823552242605454361E-001;
  SP.dnu[10][428] = -3.9095944507950994678791E-004;
  SP.xnu[10][429] =  9.8387691854673773262170E-001;
  SP.dnu[10][429] =  3.8640638489683310627664E-004;
  SP.xnu[10][430] =  9.8426104713720865811054E-001;
  SP.dnu[10][430] = -3.8184966608465838215611E-004;
  SP.xnu[10][431] =  9.8464061767827424302415E-001;
  SP.dnu[10][431] =  3.7729029183497501354316E-004;
  SP.xnu[10][432] =  9.8501562659727199630199E-001;
  SP.dnu[10][432] = -3.7272738959812194953617E-004;
  SP.xnu[10][433] =  9.8538607036447515106908E-001;
  SP.dnu[10][433] =  3.6815999389794387995698E-004;
  SP.xnu[10][434] =  9.8575194549312588776855E-001;
  SP.dnu[10][434] = -3.6358910785643290185415E-004;
  SP.xnu[10][435] =  9.8611324853946815285416E-001;
  SP.dnu[10][435] =  3.5901583482490042400167E-004;
  SP.xnu[10][436] =  9.8646997610278007273386E-001;
  SP.dnu[10][436] = -3.5443920961740503147500E-004;
  SP.xnu[10][437] =  9.8682212482540596265919E-001;
  SP.dnu[10][437] =  3.4985815887533363129314E-004;
  SP.xnu[10][438] =  9.8716969139278793025936E-001;
  SP.dnu[10][438] = -3.4527378582609305577578E-004;
  SP.xnu[10][439] =  9.8751267253349707342246E-001;
  SP.dnu[10][439] =  3.4068731081584413252032E-004;
  SP.xnu[10][440] =  9.8785106501926427223020E-001;
  SP.dnu[10][440] = -3.3609766082833202246658E-004;
  SP.xnu[10][441] =  9.8818486566501057465637E-001;
  SP.dnu[10][441] =  3.3150363590685748511232E-004;
  SP.xnu[10][442] =  9.8851407132887717574293E-001;
  SP.dnu[10][442] = -3.2690645620178693300636E-004;
  SP.xnu[10][443] =  9.8883867891225498997170E-001;
  SP.dnu[10][443] =  3.2230747988925587770293E-004;
  SP.xnu[10][444] =  9.8915868535981381655312E-001;
  SP.dnu[10][444] = -3.1770550742857355495324E-004;
  SP.xnu[10][445] =  9.8947408765953109735774E-001;
  SP.dnu[10][445] =  3.1309918897091351759161E-004;
  SP.xnu[10][446] =  9.8978488284272026721966E-001;
  SP.dnu[10][446] = -3.0848988239857957790091E-004;
  SP.xnu[10][447] =  9.9009106798405869634511E-001;
  SP.dnu[10][447] =  3.0387910981311668564352E-004;
  SP.xnu[10][448] =  9.9039264020161522456309E-001;
  SP.dnu[10][448] = -2.9926552188696074957395E-004;
  SP.xnu[10][449] =  9.9068959665687728715911E-001;
  SP.dnu[10][449] =  2.9464758951684379031374E-004;
  SP.xnu[10][450] =  9.9098193455477763203642E-001;
  SP.dnu[10][450] = -2.9002683437360152237670E-004;
  SP.xnu[10][451] =  9.9126965114372062795352E-001;
  SP.dnu[10][451] =  2.8540497560887321888224E-004;
  SP.xnu[10][452] =  9.9155274371560816359015E-001;
  SP.dnu[10][452] = -2.8078048479340106059378E-004;
  SP.xnu[10][453] =  9.9183120960586513719812E-001;
  SP.dnu[10][453] =  2.7615161602767665771436E-004;
  SP.xnu[10][454] =  9.9210504619346453659694E-001;
  SP.dnu[10][454] = -2.7152008783666298746564E-004;
  SP.xnu[10][455] =  9.9237425090095210927828E-001;
  SP.dnu[10][455] =  2.6688785910904181828739E-004;
  SP.xnu[10][456] =  9.9263882119447062238701E-001;
  SP.dnu[10][456] = -2.6225318484525797121242E-004;
  SP.xnu[10][457] =  9.9289875458378371235050E-001;
  SP.dnu[10][457] =  2.5761405357134969532359E-004;
  SP.xnu[10][458] =  9.9315404862229932393165E-001;
  SP.dnu[10][458] = -2.5297242326100518805079E-004;
  SP.xnu[10][459] =  9.9340470090709273848512E-001;
  SP.dnu[10][459] =  2.4833054850040545622628E-004;
  SP.xnu[10][460] =  9.9365070907892919119991E-001;
  SP.dnu[10][460] = -2.4368641905755933571689E-004;
  SP.xnu[10][461] =  9.9389207082228607711548E-001;
  SP.dnu[10][461] =  2.3903769333368565102620E-004;
  SP.xnu[10][462] =  9.9412878386537474570240E-001;
  SP.dnu[10][462] = -2.3438662456283303339480E-004;
  SP.xnu[10][463] =  9.9436084598016188380226E-001;
  SP.dnu[10][463] =  2.2973583784381289535765E-004;
  SP.xnu[10][464] =  9.9458825498239048672584E-001;
  SP.dnu[10][464] = -2.2508299335201567349769E-004;
  SP.xnu[10][465] =  9.9481100873160041731185E-001;
  SP.dnu[10][465] =  2.2042533212099530564424E-004;
  SP.xnu[10][466] =  9.9502910513114855275295E-001;
  SP.dnu[10][466] = -2.1576547721188962539628E-004;
  SP.xnu[10][467] =  9.9524254212822851899934E-001;
  SP.dnu[10][467] =  2.1110652655406062842514E-004;
  SP.xnu[10][468] =  9.9545131771389001255412E-001;
  SP.dnu[10][468] = -2.0644572381101178455958E-004;
  SP.xnu[10][469] =  9.9565542992305770947867E-001;
  SP.dnu[10][469] =  2.0177977180955674597236E-004;
  SP.xnu[10][470] =  9.9585487683454976143002E-001;
  SP.dnu[10][470] = -1.9711176532398019448124E-004;
  SP.xnu[10][471] =  9.9604965657109587855604E-001;
  SP.dnu[10][471] =  1.9244541880815317287615E-004;
  SP.xnu[10][472] =  9.9623976729935499907838E-001;
  SP.dnu[10][472] = -1.8777743914989716209987E-004;
  SP.xnu[10][473] =  9.9642520722993254539678E-001;
  SP.dnu[10][473] =  1.8310381869721687581835E-004;
  SP.xnu[10][474] =  9.9660597461739726655230E-001;
  SP.dnu[10][474] = -1.7842826684388457518487E-004;
  SP.xnu[10][475] =  9.9678206776029766689101E-001;
  SP.dnu[10][475] =  1.7375532281787474182422E-004;
  SP.xnu[10][476] =  9.9695348500117802077346E-001;
  SP.dnu[10][476] = -1.6908098553661371772961E-004;
  SP.xnu[10][477] =  9.9712022472659397317921E-001;
  SP.dnu[10][477] =  1.6440028266398580815135E-004;
  SP.xnu[10][478] =  9.9728228536712772605955E-001;
  SP.dnu[10][478] = -1.5971774494216440513014E-004;
  SP.xnu[10][479] =  9.9743966539740281029558E-001;
  SP.dnu[10][479] =  1.5503904982900477058276E-004;
  SP.xnu[10][480] =  9.9759236333609844312242E-001;
  SP.dnu[10][480] = -1.5035923621381498258079E-004;
  SP.xnu[10][481] =  9.9774037774596347088459E-001;
  SP.dnu[10][481] =  1.4567197593432216209306E-004;
  SP.xnu[10][482] =  9.9788370723382989699125E-001;
  SP.dnu[10][482] = -1.4098293139085325351852E-004;
  SP.xnu[10][483] =  9.9802235045062599494397E-001;
  SP.dnu[10][483] =  1.3629941252857208353454E-004;
  SP.xnu[10][484] =  9.9815630609138900631361E-001;
  SP.dnu[10][484] = -1.3161511168754763384716E-004;
  SP.xnu[10][485] =  9.9828557289527742354678E-001;
  SP.dnu[10][485] =  1.2692171094018171690690E-004;
  SP.xnu[10][486] =  9.9841014964558285748631E-001;
  SP.dnu[10][486] = -1.2222649151258494928279E-004;
  SP.xnu[10][487] =  9.9853003516974148949399E-001;
  SP.dnu[10][487] =  1.1753922205226654308357E-004;
  SP.xnu[10][488] =  9.9864522833934510806780E-001;
  SP.dnu[10][488] = -1.1285162535289322330371E-004;
  SP.xnu[10][489] =  9.9875572807015172984972E-001;
  SP.dnu[10][489] =  1.0815229594397089526825E-004;
  SP.xnu[10][490] =  9.9886153332209580492427E-001;
  SP.dnu[10][490] = -1.0345094227867500152007E-004;
  SP.xnu[10][491] =  9.9896264309929800631151E-001;
  SP.dnu[10][491] =  9.8761281292685727038015E-005;
  SP.xnu[10][492] =  9.9905905645007460356258E-001;
  SP.dnu[10][492] = -9.4071997778158146731742E-005;
  SP.xnu[10][493] =  9.9915077246694642036939E-001;
  SP.dnu[10][493] =  8.9366524327617262604006E-005;
  SP.xnu[10][494] =  9.9923779028664737610428E-001;
  SP.dnu[10][494] = -8.4658434550534472779146E-005;
  SP.xnu[10][495] =  9.9932010909013261120910E-001;
  SP.dnu[10][495] =  7.9968366893831133307150E-005;
  SP.xnu[10][496] =  9.9939772810258619635739E-001;
  SP.dnu[10][496] = -7.5279986563333116820750E-005;
  SP.xnu[10][497] =  9.9947064659342842531697E-001;
  SP.dnu[10][497] =  7.0567142632101600958836E-005;
  SP.xnu[10][498] =  9.9953886387632269144439E-001;
  SP.dnu[10][498] = -6.5850064467455216208897E-005;
  SP.xnu[10][499] =  9.9960237930918194774648E-001;
  SP.dnu[10][499] =  6.1163168831721289574374E-005;
  SP.xnu[10][500] =  9.9966119229417475044811E-001;
  SP.dnu[10][500] = -5.6481052097849504650473E-005;
  SP.xnu[10][501] =  9.9971530227773088600950E-001;
  SP.dnu[10][501] =  5.1756727287241163228480E-005;
  SP.xnu[10][502] =  9.9976470875054658153985E-001;
  SP.dnu[10][502] = -4.7023069937617741949087E-005;
  SP.xnu[10][503] =  9.9980941124758929855842E-001;
  SP.dnu[10][503] =  4.2348013295415137938180E-005;
  SP.xnu[10][504] =  9.9984940934810211005788E-001;
  SP.dnu[10][504] = -3.7687992030528700042118E-005;
  SP.xnu[10][505] =  9.9988470267560766082881E-001;
  SP.dnu[10][505] =  3.2936974381760690190527E-005;
  SP.xnu[10][506] =  9.9991529089791171100786E-001;
  SP.dnu[10][506] = -2.8153498515551469380993E-005;
  SP.xnu[10][507] =  9.9994117372710626281652E-001;
  SP.dnu[10][507] =  2.3522654984469086057002E-005;
  SP.xnu[10][508] =  9.9996235091957227046082E-001;
  SP.dnu[10][508] = -1.8969059620229907619279E-005;
  SP.xnu[10][509] =  9.9997882227598193316656E-001;
  SP.dnu[10][509] =  1.4099167258261650785636E-005;
  SP.xnu[10][510] =  9.9999058764130057132850E-001;
  SP.dnu[10][510] = -8.9278633568053509671736E-006;
  SP.xnu[10][511] =  9.9999764690478808575579E-001;
  SP.dnu[10][511] =  4.5948251014217808454541E-006;
  SP.xnu[10][512] =  1.0000000000000000000000E+000;
  SP.dnu[10][512] = -1.4305182958469602971744E-006;

  return;
}















