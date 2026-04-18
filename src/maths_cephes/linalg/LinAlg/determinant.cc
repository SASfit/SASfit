// This may look like C code, but it is really -*- C++ -*-
/*
 ************************************************************************
 *
 *			Linear Algebra Package
 *
 *	    Compute the determinant of a general square matrix
 *
 * Synopsis
 *	Matrix A;
 *	double A.determinant();
 * The matrix is assumed to be square. It is not altered.
 *
 * Method
 *	Gauss-Jordan transformations of the matrix with a slight
 *	modification to take advantage of the *column*-wise arrangement
 *	of Matrix elements. Thus we eliminate matrix's columns rather than
 *	rows in the Gauss-Jordan transformations. Note that determinant
 *	is invariant to matrix transpositions.
 *	The matrix is copied to a special object of type MatrixPivoting,
 *	where all Gauss-Jordan eliminations with full pivoting are to
 *	take place.
 *
 * $Id: determinant.cc,v 4.2 1998/10/11 22:01:09 oleg Exp oleg $
 *
 ************************************************************************
 */

#include "LinAlg.h"
#include <math.h>

/*
 *------------------------------------------------------------------------
 *			Class MatrixPivoting
 *
 * It is a descendant of a Matrix which keeps additional information
 * about what is being/has been pivoted 
 */

class MatrixPivoting : public Matrix
{
  typedef REAL * INDEX;			// wanted to have typeof(index[0])
  INDEX * const row_index;		// row_index[i] = ptr to the i-th
  					// matrix row, or 0 if the row
					// has been pivoted. Note,
  INDEX * const col_index;		// col_index[j] = ptr to the j-th
					// matrix col, or 0 if the col
					// has been pivoted.

				// Information about the pivot that was
				// just picked up
  double pivot_value;			// Value of the pivoting element
  INDEX pivot_row;			// pivot's location (ptrs)
  INDEX pivot_col;
  int pivot_odd;			// parity of the pivot
  					// (0 for even, 1 for odd)

  void pick_up_pivot(void);		// Pick up a pivot from
					// not-pivoted rows and cols

  MatrixPivoting(const MatrixPivoting&);    // Deliberately unimplemented:
  void operator = (const MatrixPivoting&);  // no copying/cloning allowed!

public:
  MatrixPivoting(const Matrix& m);	// Construct an object 
					// for a given matrix
  ~MatrixPivoting(void);

  double pivoting_and_elimination(void);// Perform the pivoting, return
					// the pivot value times (-1)^(pi+pj)
					// (pi,pj - pivot el row & col)
};


/*
 *------------------------------------------------------------------------
 *		Constructing and decomissioning of MatrixPivoting
 */

MatrixPivoting::MatrixPivoting(const Matrix& m)
  : Matrix(m), row_index(new INDEX[nrows]), 
    col_index(new INDEX[ncols]), pivot_value(0),
    pivot_row(0), pivot_col(0), pivot_odd(false)
{
  assert( row_index != 0 && col_index != 0);
  register INDEX rp = elements;		// Fill in the row_index
  for(register INDEX * rip = row_index; rip<row_index+nrows;)
    *rip++ = rp++;
  register INDEX cp = elements;		// Fill in the col_index
  for(register INDEX * cip = col_index; cip<col_index+ncols; cp += nrows)
    *cip++ = cp;
}

MatrixPivoting::~MatrixPivoting(void)
{
  is_valid();
  delete row_index;
  delete col_index;
}

/*
 *------------------------------------------------------------------------
 *				Pivoting itself
 */

			// Pick up a pivot, an element with the largest
			// abs value from yet not-pivoted rows and cols
void MatrixPivoting::pick_up_pivot(void)
{
  register REAL max_elem = -1;		// Abs value of the largest element
  INDEX * prpp = 0;			// Position of the pivot in row_index
  INDEX * pcpp = 0;			// Position of the pivot in index

  int col_odd = 0;			// Parity of the current column
  
  for(register INDEX * cpp = col_index; cpp < col_index + ncols; cpp++)
  {
    register const REAL * cp = *cpp;	// Column pointer for the curr col
    if( cp == 0 )			// skip over already pivoted col
      continue;
    int row_odd = 0;			// Parity of the current row
    for(register INDEX * rip = row_index; rip < row_index + nrows; rip++,cp++)
      if( *rip )
      {					// only if the row hasn't been pivoted
	const REAL v = *cp;
	if( ::abs(v) > max_elem )
	{
	  max_elem = ::abs(v);		// Note the local max of col elements
	  pivot_value = v;
	  prpp = rip;
	  pcpp = cpp;
	  pivot_odd = row_odd ^ col_odd;
	}
      	row_odd ^= 1;			// Toggle parity for the next row
      }
    col_odd ^= 1;
  }

  assure( max_elem >= 0 && prpp != 0 && pcpp != 0,
	 "All the rows and columns have been already pivoted and eliminated");
	 			// Note the position of the pivot and mark
	 			// the corresponding rows/cols as pivoted
  pivot_row = *prpp, *prpp = 0;
  pivot_col = *pcpp, *pcpp = 0;
}

			// Perform pivoting and gaussian elemination,
			// return the pivot value times pivot_parity
			// The procedure places zeros to the pivot_row of
			// all not yet pivoted columns
			// A[i,j] -= A[i,pivot_col]/pivot*A[pivot_row,j]
double MatrixPivoting::pivoting_and_elimination(void)
{
  pick_up_pivot();
  if( pivot_value == 0 )
    return 0;

  assert( pivot_row != 0 && pivot_col != 0 );
  
  register REAL * pcp;				// Pivot column pointer
  register const INDEX * rip;			// Current ptr in row_index
  					// Divide the pivoted column by pivot
  for(pcp=pivot_col,rip=row_index; rip<row_index+nrows; pcp++,rip++)
    if( *rip )				// Skip already pivoted rows
      *pcp /= pivot_value;

      					// Eliminate all the elements from
      					// the pivot_row in all not pivoted
      					// columns
  const REAL * prp = pivot_row;		// Pivot row ptr
  for(register const INDEX * cpp = col_index; cpp < col_index + ncols; cpp++,prp+=nrows)
  {
    register REAL * cp = *cpp;		// A[*,j]
    if( cp == 0 )			// skip over already pivoted col
      continue;
    const double fac = *prp; 		// fac = A[pivot_row,j]
    					// Do elimination stepping over pivoted rows
    for(pcp=pivot_col,rip=row_index; rip<row_index+nrows; pcp++,cp++,rip++)
      if( *rip )
        *cp -= *pcp * fac;
  }
        
  return pivot_odd ? -pivot_value : pivot_value;
}


/*
 *------------------------------------------------------------------------
 *				Root module
 */

double Matrix::determinant(void) const
{
  is_valid();

  if( nrows != ncols )
    info(), _error("Can't obtain determinant of a non-square matrix");

  if( row_lwb != col_lwb )
    info(), _error("Row and col lower bounds are inconsistent");

  MatrixPivoting mp(*this);

  register double det = 1;
  register int k;

  for(k=0; k<ncols && det != 0; k++)
    det *= mp.pivoting_and_elimination();

  return det;
}
