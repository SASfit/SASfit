// This may look like C code, but it is really -*- C++ -*-
/*
 ************************************************************************
 *
 *			Linear Algebra Package
 *
 *		Basic Linear Algebra operations, level 1 & 2
 *		     concerning specifically vectors
 *
 * The present file is concerned with the operations which either
 *	- specifically defined for vectors, such as norms
 * 	- some BLAS 1 & 2 operations that can be implemented more 
 *	  efficiently than generic operations on n*1 matrices
 *
 * $Id: vector.cc,v 4.1 1997/12/29 20:45:23 oleg Exp oleg $
 *
 ************************************************************************
 */

#include "LinAlg.h"
#include <math.h>
#include "builtin.h"

/*
 *------------------------------------------------------------------------
 *		       Specific vector constructors
 */

#include <stdarg.h>
			// Make a vector and assign initial values
			// Argument list should contain DOUBLE values
			// to assign to vector elements. The list must
			// be terminated by the string "END"
			// Example: Vector foo(1,3,0.0,1.0,1.5,"END");
Vector::Vector(const int lwb, const int upb, double iv1, ... )
  : Matrix(lwb,upb,1,1)
{
  va_list args;
  va_start(args,iv1);			// Init 'args' to the beginning of
					// the variable length list of args
  register int i;
  (*this)(lwb) = iv1;
  for(i=lwb+1; i<=upb; i++)
    (*this)(i) = (double)va_arg(args,double);

  assure( strcmp((char *)va_arg(args,char *),"END") == 0,
	 "Vector: argument list must be terminated by \"END\" ");
}

				// Resize the vector for a specified number
				// of elements, trying to keep intact as many
				// elements of the old vector as possible.
				// If the vector is expanded, the new elements
				// will be zeroes
void Vector::resize_to(const int lwb, const int upb)
{
  is_valid();
  const int old_nrows = nrows;
  assure( (nrows = upb-lwb+1) > 0,
	 "can't resize vector to a non-positive number of elems" );

  row_lwb = lwb;
  if( old_nrows == nrows )
    return;					// The same number of elems

  nelems = nrows;
  assert( !ref_counter.q_engaged() );

				// If the vector is to grow, reallocate
				// and clear the newly added elements
  if( nrows > old_nrows )
    elements = (REAL *)realloc(elements,nelems*sizeof(REAL)),
    memset(elements+old_nrows,0,(nrows-old_nrows)*sizeof(REAL));

				// Vector is to shrink a lot (more than
				// 7/8 of the original size), reallocate
  else if( old_nrows - nrows > (old_nrows>>3) )
    elements = (REAL *)realloc(elements,nelems*sizeof(REAL));

				// If the vector shrinks only a little, don't
				// bother reallocating
  assert( elements != 0 );
}

/*
 *------------------------------------------------------------------------
 *		Multiplications specifically defined for vectors
 */

				// Compute the scalar product
double operator * (const Vector& v1, const Vector& v2)
{
  are_compatible(v1,v2);
  register REAL * v1p = v1.elements;
  register REAL * v2p = v2.elements;
  register double sum = 0;

  while( v1p < v1.elements + v1.nelems )
    sum += *v1p++ * *v2p++;

  return sum;
}

					// "Inplace" multiplication
					// target = A*target
					// A needn't be a square one (the
					// target will be resized to fit)
Vector& Vector::operator *= (const Matrix& A)
{
  A.is_valid();
  is_valid();

  if( A.ncols != nrows || A.col_lwb != row_lwb )
    A.info(), info(),
    _error("matrices and vector above cannot be multiplied");
  assert( !ref_counter.q_engaged() );

  const int old_nrows = nrows;
  const REAL * old_vector = elements;	// Save the old vector elem
  row_lwb = A.row_lwb;
  assert( (nrows = A.nrows) > 0 );

  nelems = nrows;			// Allocate new vector elements
  assert( (elements = (REAL *)malloc(nelems*sizeof(REAL))) != 0 );

  register REAL * tp = elements;	// Target vector ptr
  register REAL * mp = A.elements;	// Matrix row ptr
  while( tp < elements + nelems )
  {
    register double sum = 0;
    for( register const REAL * sp = old_vector; sp < old_vector + old_nrows; )
      sum += *sp++ * *mp, mp += A.nrows;
    *tp++ = sum;
    mp -= A.nelems -1;			// mp points to the beg of the next row
  }
  assert( mp == A.elements + A.nrows );

  free((REAL *)old_vector);
  return *this;
}

