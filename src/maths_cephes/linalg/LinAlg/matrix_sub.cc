// This may look like C code, but it is really -*- C++ -*-
/*
 ************************************************************************
 *
 *			Linear Algebra Package
 *
 *		Basic linear algebra operations, levels 1 & 2
 *	    Operations on a single row, column, or the diagonal
 *			   	of a matrix
 *
 * $Id: matrix_sub.cc,v 4.2 1998/12/01 17:25:41 oleg Exp oleg $
 *
 ************************************************************************
 */

#include "LAStreams.h"
#include <math.h>
#include <iostream.h>

/*
 *------------------------------------------------------------------------
 *		Messing with a single row/column/diag of a matrix
 */

			// Constructing the MatrixColumn object
ConstMatrixColumn::ConstMatrixColumn(const Matrix& m, const int col)
  	: Matrix::ConstReference(m),
  	  DimSpec(m.q_row_lwb(),m.q_row_upb(),col,col),
  	  col_ptr(m.elements+(col-m.q_col_lwb())*m.q_nrows())
{
  if( col > m.q_col_upb() || col < m.q_col_lwb() )
    m.info(),
    _error("Column #%d is not within the above matrix",col);
}

			// Constructing the MatrixRow object
ConstMatrixRow::ConstMatrixRow(const Matrix& m, const int row)
  	: Matrix::ConstReference(m),
  	  DimSpec(row,row,m.q_col_lwb(),m.q_col_upb()),
  	  row_ptr(m.elements+(row-m.q_row_lwb())),
  	  stride(m.q_nrows()),
  	  end_ptr(m.elements+m.nelems)
{
  if( row > m.q_row_upb() || row < m.q_row_lwb() )
    m.info(),
    _error("Row #%d is not within the above matrix",row);
  assert( stride > 0 );
}

			// Constructing the MatrixDiag object
ConstMatrixDiag::ConstMatrixDiag(const Matrix& m)
  	: Matrix::ConstReference(m),
  	  DimSpec(1,::min(m.q_nrows(),m.q_ncols()),
  	          1,::min(m.q_nrows(),m.q_ncols())),
  	  start_ptr(m.elements),
  	  stride(m.q_nrows()+1),
  	  end_ptr(m.elements+m.nelems)
{
  assert( stride > 1 );
}

/*
 *------------------------------------------------------------------------
 * 	Collection-scalar arithmetics: walking with a stride
 */

				// For every element, do `elem OP value`
#define COMPUTED_VAL_ASSIGNMENT(OP,VALTYPE)				\
									\
void ElementWiseStride::operator OP (const VALTYPE val)			\
{									\
  for(register REAL * ep = start_ptr; ep < end_ptr; ep += stride)	\
    *ep OP val;								\
}									\

COMPUTED_VAL_ASSIGNMENT(=,REAL)
COMPUTED_VAL_ASSIGNMENT(+=,double)
COMPUTED_VAL_ASSIGNMENT(-=,double)
COMPUTED_VAL_ASSIGNMENT(*=,double)

#undef COMPUTED_VAL_ASSIGNMENT


				// is "element OP val" true for all
				// elements in the collection?

#define COMPARISON_WITH_SCALAR(OP)					\
									\
bool ElementWiseStrideConst::operator OP (const REAL val) const		\
{									\
  for(register const REAL * ep = start_ptr; ep < end_ptr; ep += stride)	\
    if( !(*ep OP val) )							\
      return false;							\
									\
  return true;								\
}									\


COMPARISON_WITH_SCALAR(==)
COMPARISON_WITH_SCALAR(!=)
COMPARISON_WITH_SCALAR(<)
COMPARISON_WITH_SCALAR(<=)
COMPARISON_WITH_SCALAR(>)
COMPARISON_WITH_SCALAR(>=)

#undef COMPARISON_WITH_SCALAR

/*
 *------------------------------------------------------------------------
 *	Apply algebraic functions to all elements of a collection
 */

				// Take an absolute value of a matrix
void ElementWiseStride::abs(void)
{
  for(register REAL * ep = start_ptr; ep < end_ptr; ep += stride)
    *ep = ::abs(*ep);
}

				// Square each element
void ElementWiseStride::sqr(void)
{
  for(register REAL * ep = start_ptr; ep < end_ptr; ep += stride )
    *ep = *ep * *ep;
}

				// Take the square root of all the elements
void ElementWiseStride::sqrt(void)
{
  for(register REAL * ep = start_ptr; ep < end_ptr; ep += stride )
    if( *ep >= 0 )
      *ep = ::sqrt(*ep);
    else
      _error("%d-th element, %g, is negative. Can't take the square root",
	     (ep-start_ptr), *ep );
}


/*
 *------------------------------------------------------------------------
 * 		Element-wise operations on two groups of elements
 */


				// For every element, do `elem OP another.elem`
#define TWO_GROUP_COMP(OP)					\
									\
bool ElementWiseStrideConst::operator OP (const ElementWiseStrideConst& another) const \
{									\
  register const REAL * sp = another.start_ptr;				\
  register const REAL * tp = start_ptr;					\
  for(; tp < end_ptr && sp < another.end_ptr;				\
      tp += stride, sp += another.stride)				\
    if( !(*tp OP *sp) )							\
      return false;							\
									\
  assure( tp >= end_ptr && sp >= another.end_ptr,			\
    "stride collections have different number of elements" );		\
  return true;								\
}									\

TWO_GROUP_COMP(==)
TWO_GROUP_COMP(!=)
TWO_GROUP_COMP(<)
TWO_GROUP_COMP(<=)
TWO_GROUP_COMP(>)
TWO_GROUP_COMP(>=)

#undef TWO_GROUP_COMP

				// For every element, do `elem OP another.elem`
#define TWO_GROUP_OP(OP)					\
									\
void ElementWiseStride::operator OP (const ElementWiseStrideConst& another) \
{									\
  register const REAL * sp = another.start_ptr;				\
  register REAL * tp = start_ptr; 					\
  for(; tp < end_ptr && sp < another.end_ptr;				\
      tp += stride, sp += another.stride)				\
    *tp OP *sp;								\
  assure( tp >= end_ptr && sp >= another.end_ptr,			\
    "stride collections have different number of elements" );		\
}									\

TWO_GROUP_OP(=)
TWO_GROUP_OP(+=)
TWO_GROUP_OP(-=)
TWO_GROUP_OP(*=)
TWO_GROUP_OP(/=)

#undef TWO_GROUP_OP


/*
 *------------------------------------------------------------------------
 *	Reduce a collection or a difference between two collections
 *		to a single number: a "norm"
 */

#define REDUCE_SUM(X,VAL) X += (VAL)
#define REDUCE_SUMSQ(X,VAL) X += sqr(VAL)
#define REDUCE_SUMABS(X,VAL) X += ::abs(VAL)
#define REDUCE_MAXABS(X,VAL) X = ::max((REAL)X,::abs(VAL))

#define REDUCE_ONE(NAME,OP)							\
									\
double ElementWiseStrideConst::NAME (void) const			\
{									\
  register double norm = 0;						\
  for(register const REAL * ep = start_ptr; ep < end_ptr; ep += stride)	\
    OP(norm,*ep);							\
  return norm;								\
}									\



REDUCE_ONE(sum,REDUCE_SUM)
REDUCE_ONE(sum_squares,REDUCE_SUMSQ)
REDUCE_ONE(sum_abs,REDUCE_SUMABS)
REDUCE_ONE(max_abs,REDUCE_MAXABS)

#undef REDUCE_ONE

#define REDUCE_DIFF_OF_TWO(NAME,OP)					\
									\
double ElementWiseStrideConst::NAME (const ElementWiseStrideConst& another) const	\
{									\
  register double norm = 0;						\
  register const REAL * sp = another.start_ptr;				\
  register const REAL * tp = start_ptr; 				\
  for(; tp < end_ptr && sp < another.end_ptr;				\
      tp += stride, sp += another.stride)				\
    OP(norm,*tp - *sp);							\
									\
  assure( tp >= end_ptr && sp >= another.end_ptr,			\
    "stride collections have different number of elements" );		\
  return norm;								\
}									\

REDUCE_DIFF_OF_TWO(sum_squares,REDUCE_SUMSQ)
REDUCE_DIFF_OF_TWO(sum_abs,REDUCE_SUMABS)
REDUCE_DIFF_OF_TWO(max_abs,REDUCE_MAXABS)

#undef REDUCE_DIFF_OF_TWO

#undef REDUCE_SUM
#undef REDUCE_SUMSQ
#undef REDUCE_SUMABS
#undef REDUCE_MAXABS


/*
 *------------------------------------------------------------------------
 *		   Multiplications with the diagonal matrix
 */

				// Multiply a matrix by the diagonal
				// of another matrix
				// matrix(i,j) *= diag(j)
Matrix& operator *= (Matrix& m, const ConstMatrixDiag& diag)
{
  LAStreamOut m_str(m);
  LAStrideStreamIn diag_str(diag);


  if( m.q_ncols() != diag.q_nrows() )
    m.info(), cerr << "\n and the diagonal " << diag << endl,
   _error("cannot be multiplied");

  			// Each column of m gets multiplied by the corresponding
  			// diag(i). Note that m is traversed column-wise
  while( !diag_str.eof() )
  {
    const REAL diag_el = diag_str.get();
    for(register int i=0; i<m.q_nrows(); i++)
      m_str.get() *= diag_el;
  }
  assert( m_str.eof() );
  return m;
}
