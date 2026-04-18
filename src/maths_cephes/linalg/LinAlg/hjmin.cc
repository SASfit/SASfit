// This may look like C code, but it is really -*- C++ -*-
/*
 ************************************************************************
 *
 *			  Numerical Math Package
 *		Hooke-Jeeves multidimensional minimization
 *
 * Synopsis
 *	double hjmin(b,h,funct)
 *	Vector& b 			An initial guess as to the
 *					location of the minimum.
 *					On return, it contains the location
 *					of the minimum the function has found
 *	Vector& h			Initial values for steps along
 *					each direction
 *					On exit contains the actual steps
 *					right before the termination
 *	MultivariateFunctor& f		A function being optimized
 *
 * The hjmin function returns the value of the minimized function f() at the
 * point of the found minimum.
 *
 * An alternative "double hjmin(b,h0,funct)" where h0 is a "double const"
 * uses the same initial steps along each direction. No final steps are
 * reported, though.
 *
 * Algorithm
 *	Hooke-Jeeves method of direct search for a function minimum
 *	The method is of the 0. order (i.e. requiring no gradient computation)
 *	See
 *	B.Bondi. Methods of optimization. An Introduction - M.,
 *	"Radio i sviaz", 1988 - 127 p. (in Russian)
 *
 * $Id: hjmin.cc,v 4.2 1998/12/01 17:27:15 oleg Exp oleg $
 *
 ************************************************************************
 */


#include "LAStreams.h"
#include "math_num.h"
#include "std.h"

/*
 *------------------------------------------------------------------------
 *		Class to operate on points in a space (x,f(x))
 */

class FPoint
{
  Vector& x;				// A point in the function's domain
  double fval;				// Function value at the point
  MultivariateFunctor& fproc;		// Procedure to compute that value
  const bool free_x_on_destructing;	// The flag telling if this FPoint 
					// "owns" x, and has to dispose of
					// its dynamic memory on destruction

public:
  FPoint(Vector& b, MultivariateFunctor& f);
  FPoint(const FPoint& fp);
  ~FPoint(void);

  FPoint& operator = (const FPoint& fp);

  double f(void) const			{ return fval; }

  double fiddle_around(LAStreamIn h);// Examine the function in the
					// neighborhood of the current point.
					// h defines the radius of the region

					// Proceed in the direction the function
					// seems to decline
  friend void update_in_direction(FPoint& from, FPoint& to);

					// Decide whether the region embracing
					// the local min is small enough
  bool is_step_relatively_small(LAStreamIn hs, const double tau);
};

				// Construct FPoint from array b
inline FPoint::FPoint(Vector& b, MultivariateFunctor& f)
	: x(b), fval(f(b)), fproc(f), free_x_on_destructing(false)
{
}

				// Constructor by example
FPoint::FPoint(const FPoint& fp)
	: x(*(new Vector(fp.x))), 
	  fval(fp.fval), fproc(fp.fproc),
	  free_x_on_destructing(true)
{
}
				// Destructor
FPoint::~FPoint(void)
{
  if( free_x_on_destructing )
    delete &x;
}

				// Assignment; fproc is assumed the same and
				// is not copied
inline FPoint& FPoint::operator = (const FPoint& fp)
{
  x = fp.x;
  fval = fp.fval;
  return *this;
}

/*
 * Examine the function f in the vicinity of the current point x
 * by making tentative steps fro/back along each coordinate.
 * Should the function decrease, x is updated to pinpoint thus
 * found new local min.
 * The procedure returns the minimal function value found in
 * the region.
 *
 */
double FPoint::fiddle_around(LAStreamIn hs)
{
			// Perform a step along each coordinate
  for(LAStreamOut xs(x); !xs.eof(); xs.get() )
  {
    const double hi = hs.get();
    register const double xi_old = xs.peek();  	// Old value of x[i]
    register double fnew;

    if( xs.peek() = xi_old + hi, (fnew = fproc(x)) < fval )
      fval = fnew;			// Step caused f to decrease, OK
    else if( xs.peek() = xi_old - hi, (fnew = fproc(x)) < fval )
      fval = fnew;
    else					// No function decline has been
      xs.peek() = xi_old;	// found along this coord, back up
  }
  return fval;
}                                                

				// Proceed in the direction the function
				// seems to decline
				// to_new = (to - from) + to
				// from = to (before modification)
void update_in_direction(FPoint& from, FPoint& to)
{
  for(LAStreamOut tos(to.x), froms(from.x); !tos.eof(); )
  {
    register const double t = tos.peek();
    tos.get()  += (t - froms.peek());
    froms.get() = t;
  }
  from.fval = to.fval;
  to.fval = (to.fproc)(to.x);
}

				// Check to see if the point of minimum has
				// been located accurately enough
bool FPoint::is_step_relatively_small(LAStreamIn hs, const double tau)
{
  for(LAStreamIn xs(x); !hs.eof(); )
    if( !(hs.get() /(1 + abs(xs.get())) < tau) )
     return false;
  return true;
}

/*
 *------------------------------------------------------------------------
 *			    Root module
 */

double hjmin(Vector& b, Vector& h, MultivariateFunctor& ff)
{
				// Function Parameters
  const double tau = 10*DBL_EPSILON;    // Termination criterion
  const double threshold = 1e-8;        // Threshold for the function
                                        // decay to be treated as
                                        // significant
  const double step_reduce_factor = 10;   

  are_compatible(b,h);

  FPoint pmin(b,ff);			// Point of min
  FPoint pbase(pmin);			// Base point

  for(;;)			// Main iteration loop
  {                     	// pmin is the approximation to min so far
    if( pbase.fiddle_around(h) < pmin.f() - threshold )
    {   			      	// Function value dropped significantly
      do                              	// from pmin to the point pbase
	    update_in_direction(pmin,pbase);// Keep going in the same direction
      while( pbase.fiddle_around(h) < pmin.f() - threshold ); // while it works
      pbase = pmin;			// Save the best approx found
    }
    else                               	// Function didn't fall significantly
      if(                               // upon wandering around pbas
	      h *= 1/step_reduce_factor,	// Try to reduce the step then
	      pbase.is_step_relatively_small(h,tau) )
	return pmin.f();
  }
}

	    		// The same as above with the only difference
	    		// initial steps are given to be the same
	    		// along every direction. The final steps
	    		// aren't reported back though
double hjmin(Vector& b,	const double h0, MultivariateFunctor& f)
{
  Vector h(b.q_lwb(),b.q_upb()); h = h0;
  return hjmin(b,h,f);
}
