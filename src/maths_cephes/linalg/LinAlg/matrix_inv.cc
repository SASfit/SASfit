// This may look like C code, but it is really -*- C++ -*-
/*
 ************************************************************************
 *
 *			Linear Algebra Package
 *
 *			Find the matrix inverse
 *		for matrices of general and special forms
 *
 * $Id: matrix_inv.cc,v 4.2 1998/12/19 03:14:20 oleg Exp oleg $
 *
 ************************************************************************
 */

#include "LinAlg.h"
#include <math.h>
#if defined(WIN32)
#include <malloc.h>
#else
#include <alloca.h>
#endif

/*
 *------------------------------------------------------------------------
 *
 *		The most general (Gauss-Jordan) matrix inverse
 *
 * This method works for any matrix (which of course must be square and
 * non-singular). Use this method only if none of specialized algorithms
 * (for symmetric, tridiagonal, etc) matrices is applicable/available.
 * Also, the matrix to invert has to be _well_ conditioned:
 * Gauss-Jordan eliminations (even with pivoting) perform poorly for
 * near-singular matrices (e.g., Hilbert matrices).
 *
 * The method inverts matrix inplace and returns the determinant if
 * determ_ptr was specified as not nil. determinant will be exactly zero
 * if the matrix turns out to be (numerically) singular. If determ_ptr is
 * nil and matrix happens to be singular, throw up.
 *
 * The algorithm perform inplace Gauss-Jordan eliminations with
 * full pivoting. It was adapted from my Algol-68 "translation" (ca 1986)
 * of a FORTRAN code described in
 * Johnson, K. Jeffrey, "Numerical methods in chemistry", New York,
 * N.Y.: Dekker, c1980, 503 pp, p.221
 *
 * Note, since it's much more efficient to perform operations on matrix
 * columns rather than matrix rows (due to the layout of elements in the
 * matrix), the present method implements a "transposed" algorithm.
 * Also, this algorithm was modified to avoid random access to matrix
 * elements.
 */

double Matrix::dummy_determinant_ref = 0;

static bool is_dummy_det_ref(double &determ_ref)
{ return &determ_ref == &Matrix::dummy_determinant_ref; }

Matrix& Matrix::invert(double &determ_ref)
{
  is_valid();
  if( nrows != ncols )
    info(),
    _error("Matrix to invert must be square");

  double determinant = 1;
  const double singularity_tolerance = 1e-35;

				// Locations of pivots (indices start with 0)
  struct Pivot { int row, col; } * const pivots = 
  			(Pivot*)alloca(ncols*sizeof(Pivot));
  bool * const was_pivoted = (bool*)alloca(nrows*sizeof(bool)); 
  memset(was_pivoted,false,nrows*sizeof(bool));

  for(register Pivot * pivotp = &pivots[0]; pivotp < &pivots[ncols]; pivotp++)
  {
    const REAL * old_ppos = 0;		// Location of a pivot to be
    int prow = 0, pcol = 0;		// Pivot's row and column indices
    {					// Look through all non-pivoted cols
      REAL max_value = 0;		// (and rows) for a pivot (max elem)
      register const REAL * cp = elements;  // column pointer
      for(register int j=0; j<ncols; j++)
	if( !was_pivoted[j] )
	{				// the following loop would increment
	  REAL curr_value = 0;		// cp by nrows
	  for(register int k=0; k<nrows; k++,cp++)
	    if( !was_pivoted[k] && (curr_value = abs(*cp)) > max_value )
	      max_value = curr_value, prow = k, pcol = j, old_ppos = cp;
	}
	else
	  cp += nrows;			// and this branch would too
      if( max_value < singularity_tolerance )
	if( !is_dummy_det_ref(determ_ref) )
	{
	  determ_ref = 0;
	  return *this;
	}
        else
	  _error("Matrix turns out to be singular: can't invert");
      pivotp->row = prow;
      pivotp->col = pcol;
    }

    REAL * const old_colp = const_cast<REAL*>(old_ppos) - prow;
    REAL * const new_colp = ( prow == pcol ? old_colp : elements + prow*nrows );
    if( prow != pcol )			// Swap prow-th and pcol-th columns to
    {					// bring the pivot to the diagonal
      register REAL * cr = new_colp;
      register REAL * cc = old_colp;
      for(register int k=0; k<nrows; k++)
      {
	REAL temp = *cr; *cr++ = *cc; *cc++ = temp;
      }
    }
    was_pivoted[prow] = true;

    {					// Normalize the pivot column and
      register REAL * pivot_cp = new_colp;
      const double pivot_val = pivot_cp[prow];	// pivot is at the diagonal
      determinant *= pivot_val;		// correct the determinant
      pivot_cp[prow] = 1;
      for(register int k=0; k<nrows; k++)
	*pivot_cp++ /= pivot_val;
    }

    {					// Perform eliminations
      register REAL * pivot_rp = elements + prow;	// pivot row
      register REAL * ep = elements;		// sweeps all matrix' columns
      for(register int k=0; k<ncols; k++, pivot_rp += nrows)
	if( k != prow )
	{
	  const double temp = *pivot_rp;
	  *pivot_rp = 0;
	  register const REAL * pivot_cp = new_colp;	// pivot column
	  for(register int l=0; l<nrows; l++)
	    *ep++ -= temp * *pivot_cp++;
	}
	else
	  ep += nrows;
    }
  }

  int no_swaps = 0;		// Swap exchanged *rows* back in place
  for(register const Pivot * pvp = &pivots[ncols-1];
      pvp >= &pivots[0]; pvp--)
    if( pvp->row != pvp->col )
    {
      no_swaps++;
      register REAL * rp = elements + pvp->row;
      register REAL * cp = elements + pvp->col;
      for(register int k=0; k<ncols; k++, rp += nrows, cp += nrows)
      {
	const REAL temp = *rp; *rp = *cp; *cp = temp;
      }
    }

  if( !is_dummy_det_ref(determ_ref) )
    determ_ref = ( no_swaps & 1 ? -determinant : determinant );

  return *this;
}

