// This may look like C code, but it is really -*- C++ -*-
/*
 ************************************************************************
 *
 *			  Numerical Math Package
 *		     Aitken-Lagrange interpolation
 *
 * This package allows one to interpolate a function value for a given
 * argument value using function values tabulated over either uniform or
 * non-uniform grid. The latter is specified by a vector of node point
 * abscissae. The uniform grid is specified by the size of a grid mesh
 * and the abscissa of the first grid point.
 *
 * Synopsis
 *	double ali(q,x0,s,y)
 *	double q 			Argument value specified
 *	double x0 			Abscissae for the 1. grid point
 *	double s 			Grid mesh, >0
 *	VECTOR y			Vector of function values
 *					tabulated at points
 *					x0 + s*(i-y.q_lwb()))
 *                                      The vector must contain at 
 *					least 2 elements
 *
 *	double ali(q,x,y)
 *	const double q			Argument value specified
 *	const VECTOR x			Vector of grid node abscissae
 *	const VECTOR y			Vector of function values
 *					tabulated at points x[i]
 *                                      The vector must contain at 
 *					least 2 elements
 * Output
 *	Both functions return the interpolated value of y(q)
 *	Interpolating process finishes either
 *		- if the difference between two successive interpolated
 *		  values is absolutely less than EPSILON
 *		- if the absolute value of this difference stops
 *		  diminishing
 *		- after (N-1) steps, N being the no. of elements in vector y
 *
 * Algorithm
 *	Aitken scheme of Lagrange interpolation
 *	Algorithm is described in the book
 *		"Mathematical software for computers", Institute of
 *		 Mathematics of the Belorussian Academy of Sciences,
 *		 Minsk, 1974, issue #4,
 *		 p. 146 (description of ALI, DALI subroutines)
 *		 p. 180 (description of ATSE, DATSE subroutines)
 *	The book essentially describes IBM's SSP package.
 *
 * $Id: ali.cc,v 4.2 1998/12/01 17:26:27 oleg Exp oleg $
 *
 ************************************************************************
 */


#include "LAStreams.h"
#include "math_num.h"
#include "std.h"
#include <float.h>


//#define DEBUG

/*
 *------------------------------------------------------------------------
 *		     Class that handles the interpolation
 */

class ALInterp
{
  Vector arg;			// [1:n] Arranged table of arguments
  Vector val;			// [1:n] Arranged table of function values
  double q;           		// Argument value the function is to be
				// interpolated at

  ALInterp(const ALInterp&);    	// Deliberately unimplemented:
  void operator = (const ALInterp&);	// no copying/cloning allowed!

public:
				// Construct the arranged tables for the
				// uniform grid
  ALInterp(const double q, const double x0, const double s, const Vector& y);
				// Construct the arranged tables for the
				// non-uniform grid
  ALInterp(const double q, const Vector& x, const Vector& y);

  double interpolate(void);	// Perform actual interpolation
};

/*
 *------------------------------------------------------------------------
 *
 *		Arranging data for the Aitken-Lagrange interpolation
 *
 * Abscissae (arg) and ordinates (val) of the grid points should be arranged
 * in such a way that the distance abs(q-arg[i]) would increase as i
 * increases. 
 * Here q is the point the function is to be interpolated at.
 *
 */

				// Construct the arranged tables for the
				// uniform grid
ALInterp::ALInterp(const double _q, const double x0,
		   const double s, const Vector& y)
	: arg(y.q_no_elems()), val(y.q_no_elems()), q(_q)
{
  const int n = y.q_no_elems();
  assure( n > 1, "Vector y (function values) must have at least 2 points");
  assure( s > 0, "The grid mesh has to be positive");

			// First find the index of the grid node which
			// is closest to q. Assign index 1 to this
			// node. Then look at neighboring grid nodes
			// and assign indices to them
			// (kind of breadth-first search)
  int js = (int)( (q-x0)/s + 1.5 );	// Index j for the point x0+s*j
					// which is the closest to q
  if( js < 1 )				// Check for the case of extrapolation
    js = 1;				// to the left end
  else if( js > n )
    js = n;				// or to the right end

  					// Direction to the next closest
					// to q grid node
  bool right_pt_is_closer = q > x0 + (js-1)*s;

  register int jcurr = js, jleft = js, jright = js;
  					// Pick up elements x0+s*i
					// in the neighborhood of q
  for(LAStreamOut args(arg), vals(val); !args.eof(); )
  {
    args.get() = x0 + (jcurr-1)*s;
    vals.get() = y(jcurr-1+y.q_lwb());	// Once the closest to q point js
    if( jright >= n )			// is found, we pick up points
      right_pt_is_closer = false;	// alternatively to the right
    if( jleft <= 1 )			// and to the left of the js
      right_pt_is_closer = true;	// further and further
    if( right_pt_is_closer )
      jcurr = ++jright, right_pt_is_closer = false;
    else
      jcurr = --jleft, right_pt_is_closer = true;
  }
}


static inline int fsign(const float f)		// Return the sign of f
{ return f < 0 ? -1 : f==0 ? 0 : 1; }

				// Construct the arranged tables for a
				// non-uniform grid
ALInterp::ALInterp(const double _q, const Vector& x, const Vector& y)
	: arg(x.q_no_elems()), val(y.q_no_elems()), q(_q)
{
  assure( y.q_no_elems() > 1,
  	  "Vector y (function values) must have at least 2 points");
  are_compatible(x,y);

	                // Selection is done by sorting x,y arrays
			// in the way mentioned above. Fisrt an array
	                // of indices is created and sorted, then arg,
			// val arrays are filled in using the sorted indices
  class index_permutation
  {
    struct El { int x_ind; float x_to_q; };	// x_to_q = |x[x_ind]-q|
    El * const permutation;
    const int n;
    static int comparison_func(const void * ip, const void * jp)
      { return fsign(((const El*)ip)->x_to_q - ((const El*)jp)->x_to_q); }
  public:
    index_permutation(const double q, const Vector& x) :
	permutation(new El[x.q_no_elems()]), n(x.q_no_elems())
    {
      register El * pp = permutation;
      for(register int i=x.q_lwb(); i<=x.q_upb(); i++,pp++)
        pp->x_ind = i, pp->x_to_q = abs(q-x(i));
    				// Sort indices so that
				// |q-x[x_ind[i]]| < |q-x[x_ind[j]]|
				// for all i<j
      qsort(permutation,n,sizeof(permutation[0]),comparison_func);
    }
    ~index_permutation(void) { delete permutation; }
    				// Apply the permutation to x to get arg
    				// and to y to get val
    void apply(Vector& arg, Vector& val, const Vector& x, const Vector& y)
    {
      register const El* pp = permutation;
      for(LAStreamOut args(arg), vals(val); !args.eof(); pp++)
        args.get() = x(pp->x_ind), vals.get() = y(pp->x_ind);
      assert(pp==permutation+n);
    }
  };
  
  index_permutation(q,x).apply(arg,val,x,y);
 }

/*
 *------------------------------------------------------------------------
 *			Aitken - Lagrange process
 *
 *  arg and val tables are assumed to be arranged in the proper way
 *
 */

double ALInterp::interpolate()
{
  LAStreamIn args(arg);
  LAStreamOut vals(val);
  register double valp = vals.peek();	// The best approximation found so far
  register double diffp = DBL_MAX;	// abs(valp - prev. to valp)

#ifdef DEBUG
  arg.print("arg - interpolation nodes");
  val.print("Arranged table of function values");
#endif
			// Compute the j-th row of the Aitken scheme and
			// place it in the 'val' array
  for(register int j=2; j<=val.q_upb(); j++)
  {
    register double argj = (args.get(), args.peek());
    register REAL&  valj = (vals.get(), vals.peek());
    args.rewind(); vals.rewind();
    for(register int i=1; i<=j-1; i++)
    {
      double argi = args.get();
      valj = ( vals.get()*(q-argj) - valj*(q-argi) ) / (argi - argj);
    }

#ifdef DEBUG
    message("\nval(j) = %g, valp = %g, arg(j) = %g",valj,valp,argj);
#endif

    register double diff = abs( valj - valp );

    if( j>2 && diff == 0 )  		// An exact result has been achieved
      break;

    if( j>4 && diff > diffp )		// Difference stoped diminishing
      break;				// after the 4. step

    valp = valj;  diffp = diff;
  }

  return valp;
}



/* 
 *=======================================================================
 *				Root modules
 */

				// Uniform mesh x[i] = x0 + s*(i-y.lwb)
double ali(const double q, const double x0, const double s, const Vector& y)
{
  ALInterp al(q,x0,s,y);
  return al.interpolate();
}

				// Nonuniform grid with nodes in x[i]
double ali(const double q, const Vector& x, const Vector& y)
{
  ALInterp al(q,x,y);
  return al.interpolate();
}
