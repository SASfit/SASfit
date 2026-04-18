// This may look like C code, but it is really -*- C++ -*-
/*
 ************************************************************************
 *
 *			Linear Algebra Package
 *
 *		Basic linear algebra operations, level 2
 *	       Matrix transformations and multiplications
 *			   of various types
 *
 * $Id: matrix2.cc,v 4.2 1998/12/19 03:14:20 oleg Exp oleg $
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
 *				Transpositions
 */
				// Transpose a matrix
void LazyTransposedMatrix::fill_in(Matrix& m) const
{
  register const REAL * rsp = proto.elements;	// Row source pointer
  register REAL * tp = m.elements;

				// Matrix m is traversed in the
				// natural, column-wise way, whilst the source
				// (prototype) matrix is scanned row-by-row
  while( tp < m.elements + m.nelems )
  {
    register const REAL * sp = rsp++;	// sp = @proto[j,i] for i=0
					// Move tp to the next elem in the col,
    while( sp < proto.elements + proto.nelems )
       *tp++ = *sp, sp += proto.nrows; // sp to the next el in the curr row
  }
  assert( tp == m.elements + m.nelems && 
	  rsp == proto.elements + proto.nrows );

}

/*
 *------------------------------------------------------------------------
 *			General matrix multiplications
 */

			// Compute target = target * source inplace.
			// Strictly speaking, it can't be done inplace,
			// though only one row of the target matrix needs
			// to be saved.
			// "Inplace" multiplication is only possible
			// when the 'source' matrix is square
Matrix& Matrix::operator *= (const Matrix& source)
{
  is_valid();
  source.is_valid();

  if( row_lwb != source.col_lwb || ncols != source.nrows ||
      col_lwb != source.col_lwb || ncols != source.ncols )
    info(), source.info(),
    _error("matrices above are unsuitable for the inplace multiplication");

  					// One row of the old_target matrix
  REAL * const one_row = (REAL *)alloca(ncols*sizeof(REAL));
  const REAL * one_row_end = &one_row[ncols];

  register REAL * trp = elements;	// Pointer to the i-th row
  for(; trp < &elements[nrows]; trp++)	// Go row-by-row in the target
  {
    register REAL *wrp, *orp;		   	// work row pointers
    for(wrp=trp,orp=one_row; orp < one_row_end;)
      *orp++ = *wrp, wrp += nrows;		// Copy a row of old_target

    register REAL *scp=source.elements;		// source column pointer
    for(wrp=trp; wrp < elements+nelems; wrp += nrows)
    {
      register double sum = 0;			// Multiply a row of old_target
      for(orp=one_row; orp < one_row_end;)	// by each col of source
	sum += *orp++ * *scp++;			// to get a row of new_target
      *wrp = sum;
    }
  }

  return *this;
}

			// Compute C = A*B
			// The matrix C must be already
			// allocated, and it is *this
void Matrix::mult(const Matrix& A, const Matrix& B)
{
  A.is_valid();
  B.is_valid();
  is_valid();
  
  if( A.ncols != B.nrows || A.col_lwb != B.row_lwb )
    A.info(), B.info(),
    _error("matrices above cannot be multiplied");
  if( nrows != A.nrows || ncols != B.ncols ||
      row_lwb != A.row_lwb || col_lwb != B.col_lwb )
    A.info(),B.info(),info(),
    _error("product A*B is incompatible with the given matrix");

  register REAL * arp;			// Pointer to the i-th row of A
           REAL * bcp = B.elements;	// Pointer to the j-th col of B
  register REAL * cp = elements;	// C is to be traversed in the natural
  while( cp < elements + nelems )	// order, col-after-col
  {
    for(arp = A.elements; arp < A.elements + A.nrows; )
    {
      register double cij = 0;
      register REAL * bccp = bcp;		// To scan the jth col of B
      while( arp < A.elements + A.nelems )	// Scan the i-th row of A and
	cij += *bccp++ * *arp, arp += A.nrows;	// the j-th col of B
      *cp++ = cij;
      arp -= A.nelems - 1;			// arp points to (i+1)-th row
    }
    bcp += B.nrows;			// We're done with j-th col of both
  }					// B and C. Set bcp to the (j+1)-th col

  assert( cp == elements + nelems && bcp == B.elements + B.nelems );
}

#if 0
			// Create a matrix C such that C = A' * B
			// In other words,
			// c[i,j] = SUM{ a[k,i] * b[k,j] }
			// Note, matrix C needs to be allocated
void Matrix::_AtmultB(const Matrix& A, const Matrix& B)
{
  A.is_valid();
  B.is_valid();

  if( A.nrows != B.nrows || A.row_lwb != B.row_lwb )
    A.info(), B.info(),
    _error("matrices above are unsuitable for A'B multiplication");

  allocate(A.ncols,B.ncols,A.col_lwb,B.col_lwb);

  register REAL * acp;			// Pointer to the i-th col of A
           REAL * bcp = B.elements;	// Pointer to the j-th col of B
  register REAL * cp = elements;	// C is to be traversed in the natural
  while( cp < elements + nelems )	// order, col-after-col
  {
    for(acp = A.elements; acp<A.elements + A.nelems;)
    {					// Scan all cols of A
      register double cij = 0;			
      register REAL * bccp = bcp;		// To scan the jth col of B
      for(register int i=0; i<A.nrows; i++)	// Scan the i-th row of A and
	cij += *bccp++ * *acp++;			// the j-th col of B
      *cp++ = cij;
    }
    bcp += B.nrows;			// We're done with j-th col of both
  }					// B and C. Set bcp to the (j+1)-th col

  assert( cp == elements + nelems && bcp == B.elements + B.nelems );
}
#endif
