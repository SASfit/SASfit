// This may look like C code, but it is really -*- C++ -*-
/*
 ************************************************************************
 *
 *			  Numerical Math Package
 *
 *		    Brent's one-dimensional minimizer 
 *
 *	     finds a local minimum of a single argument function
 *			  over a given range
 *
 * Input
 *	double fminbr(ax,bx,f,tol)
 *	const double ax		        a and b, a < b, specify the interval
 *	const double bx  		the minimum is to be sought in
 *	UnivariateFunctor& f		The function under consideration
 *	const double tol		Acceptable tolerance for the minimum
 *					location. It is an optional parameter
 *					with default value DBL_EPSILON
 *
 * Output
 *	Fminbr returns an estimate to the location of the minimum
 *	with accuracy 3*SQRT_EPSILON*abs(x) + tol.
 *	The procedure can only determine a local minimum, which coincides with
 *	the global one if and only if the function under investigation is
 *	unimodular.
 *	If a function being examined possesses no local minimum within
 *	the given interval, Fminbr returns either the left or the right end
 *	point of the interval, wherever the function value is smaller.
 *
 * Algorithm
 *	G.Forsythe, M.Malcolm, C.Moler, Computer methods for mathematical
 *	computations. M., Mir, 1980, p.202 of the Russian edition
 *
 * The function makes use of a "gold section" procedure combined with
 * a parabolic interpolation.
 * At each step the code operates three abscissae - x,v, and w.
 * 	x - the last and the best approximation to the minimum location,
 *		i.e. f(x) <= f(a) or/and f(x) <= f(b)
 * 	    (if the function f has a local minimum in (a,b), then both
 *  	     conditions are met after one or two steps).
 *	v,w are previous approximations to the location of the minimum.
 *	They may coincide with a, b, or x (although the algorithm tries
 *	to make all u, v, and w distinct). 
 * Points x, v, and w are used to construct an interpolating parabola,
 * whose minimum is regarded as a new approximation to the minimum
 * of the function, provided the parabola's minimum falls within [a,b]
 * and reduces the current interval [a,b] to a larger extent than the
 * gold section procedure does.
 * When f(x) has a positive second derivative at the point of minimum
 * (which does not coincide with a or b) the procedure converges
 * superlinearly at a rate of about 1.324
 *
 * $Id: fminbr.cc,v 4.0 1997/06/11 18:32:54 oleg Exp oleg $
 *
 ************************************************************************
 */

#include "math_num.h"


double fminbr(				// An estimate to the min location
	const double ax,		// Specify the interval the minimum
	const double bx,		// to be sought in
	UnivariateFunctor& f,		// Function under investigation
	const double tol)		// Acceptable tolerance
{
  assure( tol > 0, "Tolerance must be positive");
  assure( bx > ax, 
	 "Left end point of the interval should be strictly less than the "
	 "right one" );
  
  static const double r = (3-sqrt(5.0))/2;	// The golden section ratio
  static const double sqrt_eps = sqrt(DBL_EPSILON);
  
  double a = ax, b = bx;		// Current interval
  double v = a + r*(b-a);		// First step - always gold section
  double fv = f(v);
  double x = v;				// the last and the best approximation
  double fx = fv;
  double w = v;				// a previous approx to the min
  double fw = fv;

  for(;;)		// Main iteration loop
  {
    const double range = b-a;		// Interval where the minimum
					// is searched in
    const double midpoint = (a+b)/2;
    const double tol_act =		// The effective tolerance
		sqrt_eps*abs(x) + tol/3;

       

    if( 2*abs(x-midpoint) + range <= 4*tol_act )
      return x;				// Acceptable approximation is found

					// Compute a new step with the gold
					// section
    double new_step = r * ( x < midpoint ? b-x : a-x );


    			// Decide on the interpolation  
    if( abs(x-w) >= tol_act  )		// If x and w are distinct
    {					// interpolatiom may be tried
      register double p; 		// Interpolation step is calcula-
      register double q;              	// ted as p/q; division operation
                                        // is delayed until last moment
      register double t;

      t = (x-w) * (fx-fv);
      q = (x-v) * (fx-fw);
      p = (x-v)*q - (x-w)*t;
      q = 2*(q-t);

      if( q > 0 )			// Formulas above computed new_step
	p = -p;				// = p/q with a wrong sign (on purpose).
      else				// Correct this, but in such a way so
	q = -q;				// that q would be positive

      if( abs(p) < abs(new_step*q) &&	// If x+p/q falls in [a,b] and is not
	 p > q*(a-x+2*tol_act) &&	// too close to a and b, and isn't
	 p < q*(b-x-2*tol_act)  )       // too large, it is accepted
	   new_step = p/q;
					// If p/q is too large then the
					// gold section procedure would
					// reduce [a,b] to larger extent
    }

    if( abs(new_step) < tol_act )	// Adjust the step to be not less
      new_step =  new_step > 0 ?	// than tolerance
	tol_act : -tol_act;

				// Obtain the next approximation to min
    				// and reduce the encompassing interval
    register const double t = x + new_step;  // Tentative point for the min
    register const double ft = f(t);
    if( ft <= fx )
    {                                 	// t is a better approximation
      ( t < x ? b : a ) = x;		// Reduce the interval so that
					// t would fall within it
      v = w;  w = x;  x = t;		// Assign the best approx to x
      fv=fw;  fw=fx;  fx=ft;
    }
    else                              	// x remains the better approx
    {
      ( t < x ? a : b )	= t;		// Reduce the interval encompassing x
      
      if( ft <= fw || w==x )
      {
	v = w;  w = t;
	fv=fw;  fw=ft;
      }
      else if( ft<=fv || v==x || v==w )
	v = t, fv = ft;
    }
  }		// ===== End of loop =====

}
