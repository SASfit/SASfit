// This may look like C code, but it is really -*- C++ -*-
/*
 ************************************************************************
 *
 *			  Numerical Math Package
 *
 * The present package implements various algorithms of Numerical Math
 *
 * $Id: math_num.h,v 4.1 1998/10/04 22:12:22 oleg Exp oleg $
 *
 ************************************************************************
 */

#ifndef __GNUC__
#pragma once
#endif
#ifndef _math_num_h
#define _math_num_h 1

#if defined(__GNUC__)
#pragma interface
#endif

#include "myenv.h"
#include <float.h>
#include <math.h>
#include "builtin.h"
#include "std.h"

/*
 *------------------------------------------------------------------------
 *				Some constants
 * Compile and run the program epsilon.c to determine the values below for
 * your computer
 */

//#define EPSILON		2.22045e-16	// DBL_EPSILON
//#define SQRT_EPSILON	1.49012e-08

/*
 *------------------------------------------------------------------------
 *		Brent's minimum and zero finders for 
 *		  a function of a single argument
 */

			// A function of a single argument
struct UnivariateFunctor
{
  virtual double operator() (const double x) = 0;
};

				// Obtain a zero of function f
				// over the interval [ax,bx] with the
				// accuracy tol.
double zeroin(const double ax, const double bx, 
	      UnivariateFunctor& f, const double tol=DBL_EPSILON);

				// Find a minimum of function f
				// over the interval [a,b] with the
				// accuracy tol.
				// Returns an approx. to the min location
double fminbr(const double a, const double b, 
	      UnivariateFunctor& f, const double tol=DBL_EPSILON);

class Vector;			// Opaque class used by the routines below

/*
 *------------------------------------------------------------------------
 *			Interpolation of the function
 *			specified in the tabular form
 */


				// Aitken-Lagrange interpolation to the
				// point q over the table of function values
				// y[i] = y(x[i]), i = y.lwb..y.upb

				// Uniform mesh x[i] = x0 + s*(i-y.lwb)
double ali(const double q, const double x0, const double s, const Vector& y);
				// Nonuniform grid with nodes in x[i]
double ali(const double q, const Vector& x, const Vector& y);

/*
 *------------------------------------------------------------------------
 *			Multi-dimensional minimization
 */

			// A function of a _vector_ argument
struct MultivariateFunctor
{
  virtual double operator() (const Vector& x) = 0;
};

				// Find a local minimum of a given
				// function by the Hooke-Jeeves method
double hjmin(			// Return the function value at min
	Vector& b,			// Input: initial guess to min loc
					  // Output: loc for the min found
 	Vector& h,			// Input: initial values for the
 					  // 	  steps along each b(i)
 					  // Output: final steps right before
 					  // 	  the termination
 	MultivariateFunctor& f	// A function being optimized
	    );


	    			// The same as above with the only difference
	    			// initial steps are given to be the same
	    			// along every direction. The final steps
	    			// aren't reported back though
double hjmin(Vector& b,	const double h0,
 	     MultivariateFunctor& f);

#endif
