// This may look like C code, but it is really -*- C++ -*-
/*
 ************************************************************************
 *
 *			Linear Algebra Package
 *
 *		Basic linear algebra operations, level 1
 *		      Element-wise operations
 *
 * $Id: matrix1.cc,v 4.3 1998/12/19 03:14:20 oleg Exp oleg $
 *
 ************************************************************************
 */

#ifdef __GNUC__
#pragma implementation "LinAlg.h"
#endif

#include "LAStreams.h"
#include <math.h>
#include "iostream.h"

/*
 *------------------------------------------------------------------------
 *			Constructors and destructors
 */

void Matrix::allocate(void)
{
  valid_code = MATRIX_val_code;

  assure( (nelems = nrows * ncols) > 0,
  	"The number of matrix cols and rows has got to be positive");

  assure( !ref_counter.q_engaged(),
  	"An attempt to allocate Matrix data which are in use" );
  
  name = "";

  assert( (elements = (REAL *)calloc(nelems,sizeof(REAL))) != 0 );

}


Matrix::~Matrix(void)		// Dispose of the Matrix struct
{
  is_valid();
  free(elements);
  if( name[0] != '\0' )
    delete const_cast<char*>(name);
  valid_code = 0;
}

				// Set a new Matrix name
void Matrix::set_name(const char * new_name)
{
  if( name != 0 && name[0] != '\0' )	// Dispose of the previous matrix name
    delete const_cast<char*>(name);

  if( new_name == 0 || new_name[0] == '\0' )
    name = "";				// Matrix is anonymous now
  else
    name = new char[strlen(new_name)+1],
      strcpy(const_cast<char *>(name), new_name);
}

				// Erase the old matrix and create a
				// new one according to new boundaries
				// with indexation starting at 1
void Matrix::resize_to(const int nrows, const int ncols)
{
  is_valid();
  if( nrows == Matrix::nrows && ncols == Matrix::ncols )
    return;

#if 0
  if( ncols != 1 )
    free(index);
#endif
  assert( !ref_counter.q_engaged() );
  free(elements);

  const char * const old_name = name;
  Matrix::nrows = nrows;
  Matrix::ncols = ncols;
  allocate();
  name = old_name;
}
				// Erase the old matrix and create a
				// new one according to new boundaries
void Matrix::resize_to(const DimSpec& dimension_specs)
{
  is_valid();
  Matrix::row_lwb = dimension_specs.q_row_lwb();
  Matrix::col_lwb = dimension_specs.q_col_lwb();
  resize_to(dimension_specs.q_nrows(),dimension_specs.q_ncols());
}

#if 0
					// Routing constructor module
Matrix::Matrix(const Matrix& A, const MATRIX_CREATORS_2op op, const Matrix& B)
{
  A.is_valid();
  B.is_valid();
  switch(op)
  {
    case Mult:
         _AmultB(A,B);
	 break;

    case TransposeMult:
	 _AtmultB(A,B);
	 break;

    default:
	 _error("Operation %d is not yet implemented",op);
  }
}
#endif


			// Build a column index to facilitate direct
			// access to matrix elements
REAL * const * MatrixDABase::build_index(const Matrix& m)
{
  m.is_valid();
  if( m.ncols == 1 )		// Only one col - index is dummy actually
    return const_cast<REAL * const *>(&m.elements);

  REAL ** const index = (REAL **)calloc(m.ncols,sizeof(REAL *));
  assert( index != 0 );
  register REAL * col_p = &m.elements[0];
  for(register REAL** ip = index; ip<index+m.ncols; col_p += m.nrows)
    *ip++ = col_p;
  return index;
}

MatrixDABase::~MatrixDABase(void)
{
  if( ncols != 1 )
    free((void *)index);
}

/*
 *------------------------------------------------------------------------
 * 		    Making a matrix of a special kind	
 */

				// Make a unit matrix
				// (Matrix needn't be a square one)
				// The matrix is traversed in the
				// natural (that is, col by col) order
Matrix& Matrix::unit_matrix(void)
{
  is_valid();
  register REAL *ep = elements;

  for(register int j=0; j < ncols; j++)
    for(register int i=0; i < nrows; i++)
        *ep++ = ( i==j ? 1.0 : 0.0 );

  return *this;
}

				// Make a Hilbert matrix
				// Hilb[i,j] = 1/(i+j-1), i,j=1...max, OR
				// Hilb[i,j] = 1/(i+j+1), i,j=0...max-1
				// (Matrix needn't be a square one)
				// The matrix is traversed in the
				// natural (that is, col by col) order
Matrix& Matrix::hilbert_matrix(void)
{
  is_valid();
  register REAL *ep = elements;

  for(register int j=0; j < ncols; j++)
    for(register int i=0; i < nrows; i++)
        *ep++ = 1./(i+j+1);

  return *this;
}

			// Create an orthonormal (2^n)*(no_cols) Haar
			// (sub)matrix, whose columns are Haar functions
			// If no_cols is 0, create the complete matrix
			// with 2^n columns
			// E.g., the complete Haar matrix of the second order
			// is
			// column 1: [ 1  1  1  1]/2
			// column 2: [ 1  1 -1 -1]/2
			// column 3: [ 1 -1  0  0]/sqrt(2)
			// column 4: [ 0  0  1 -1]/sqrt(2)
			// Matrix m is assumed to be zero originally
void haar_matrix::fill_in(Matrix& m) const
{
  m.is_valid();
  assert( m.ncols <= m.nrows && m.ncols > 0 );
  register REAL * cp = m.elements;
     const REAL * const m_end = m.elements + m.nelems;

  double norm_factor = 1/sqrt((double)m.nrows);

  for(register int i=0; i<m.nrows; i++)	// First column is always 1
    *cp++ = norm_factor;	// (up to a normalization)

				// The other functions are kind of steps:
				// stretch of 1 followed by the equally
				// long stretch of -1
				// The functions can be grouped in families
				// according to their order (step size),
				// differing only in the location of the step
  int step_length = m.nrows/2;
  while( cp < m_end && step_length > 0 )
  {
    for(register int step_position=0; cp < m_end && step_position < m.nrows;
	step_position += 2*step_length, cp += m.nrows)
    {
      register REAL * ccp = cp + step_position;
      for(register int i=0; i<step_length; i++)
	*ccp++ = norm_factor;
      for(register int j=0; j<step_length; j++)
	*ccp++ = -norm_factor;
    }
    step_length /= 2;
    norm_factor *= sqrt(2.0);
  }
  assert( step_length != 0 || cp == m_end );
  assert( m.nrows != m.ncols || step_length == 0 );
}

haar_matrix::haar_matrix(const int order, const int no_cols)
    : LazyMatrix(1<<order,no_cols == 0 ? 1<<order : no_cols)
{
  assert(order > 0 && no_cols >= 0);
}

/*
 *------------------------------------------------------------------------
 * 			Collection-scalar arithmetics
 *	walk through the collection and do something with each
 *			element in turn, and the scalar
 */

				// For every element, do `elem OP value`
#define COMPUTED_VAL_ASSIGNMENT(OP,VALTYPE)				\
									\
void ElementWise::operator OP (const VALTYPE val)			\
{									\
  register REAL * ep = start_ptr;					\
  while( ep < end_ptr )							\
    *ep++ OP val;							\
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
bool ElementWiseConst::operator OP (const REAL val) const		\
{									\
  register const REAL * ep = start_ptr;					\
  while( ep < end_ptr )							\
    if( !(*ep++ OP val) )						\
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
void ElementWise::abs(void)
{
  for(register REAL * ep=start_ptr; ep < end_ptr; ep++)
    *ep = ::abs(*ep);
}

				// Square each element
void ElementWise::sqr(void)
{
  for(register REAL * ep=start_ptr; ep < end_ptr; ep++)
    *ep = *ep * *ep;
}

				// Take the square root of all the elements
void ElementWise::sqrt(void)
{
  for(register REAL * ep=start_ptr; ep < end_ptr; ep++)
    if( *ep >= 0 )
      *ep = ::sqrt(*ep);
    else
      _error("%d-th element, %g, is negative. Can't take the square root",
	     (ep-start_ptr), *ep );
}

				// Apply a user-defined action to each matrix
				// element. The matrix is traversed in the
				// natural (that is, col by col) order
Matrix& Matrix::apply(ElementAction& action)
{
  is_valid();
  register REAL * ep = elements;
  for(register int j=col_lwb; j<col_lwb+ncols; j++)
    for(register int i=row_lwb; i<row_lwb+nrows; i++)
      action.operation(*ep++,i,j);
  assert( ep == elements+nelems );

  return *this;
}

const Matrix& Matrix::apply(ConstElementAction& action) const
{
  is_valid();
  register const REAL * ep = elements;
  for(register int j=col_lwb; j<col_lwb+ncols; j++)
    for(register int i=row_lwb; i<row_lwb+nrows; i++)
      action.operation(*ep++,i,j);
  assert( ep == elements+nelems );

  return *this;
}


/*
 *------------------------------------------------------------------------
 * 		Element-wise operations on two groups of elements
 */

				// Check to see if two matrices are identical
bool Matrix::operator == (const Matrix& m2) const
{
  are_compatible(*this,m2);
  return (memcmp(elements,m2.elements,nelems*sizeof(REAL)) == 0);
}

				// For every element, do `elem OP another.elem`
#define TWO_GROUP_COMP(OP)					\
									\
bool ElementWiseConst::operator OP (const ElementWiseConst& another) const \
{									\
  sure_compatible_with(another);					\
  register const REAL * sp = another.start_ptr;				\
  register const REAL * tp = start_ptr;					\
  while( tp < end_ptr )							\
    if( !(*tp++ OP *sp++) )						\
      return false;							\
									\
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
void ElementWise::operator OP (const ElementWiseConst& another)		\
{									\
  sure_compatible_with(another);					\
  register const REAL * sp = another.start_ptr;				\
  register REAL * tp = start_ptr;					\
  while( tp < end_ptr )							\
    *tp++ OP *sp++;							\
}									\

TWO_GROUP_OP(=)
TWO_GROUP_OP(+=)
TWO_GROUP_OP(-=)
TWO_GROUP_OP(*=)
TWO_GROUP_OP(/=)

#undef TWO_GROUP_OP

#if 0
				// Modified addition
				//	Target += scalar*Source
Matrix& add(Matrix& target, const double scalar,const Matrix& source)
{
  are_compatible(target,source);

  register REAL * sp = source.elements;
  register REAL * tp = target.elements;
  for(; tp < target.elements+target.nelems;)
    *tp++ += scalar * *sp++;
  
  return target;
}
#endif

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
double ElementWiseConst::NAME (void) const				\
{									\
  register double norm = 0;						\
  register const REAL * ep = start_ptr;					\
  while( ep < end_ptr )							\
    OP(norm,*ep++);							\
  return norm;								\
}									\



REDUCE_ONE(sum,REDUCE_SUM)
REDUCE_ONE(sum_squares,REDUCE_SUMSQ)
REDUCE_ONE(sum_abs,REDUCE_SUMABS)
REDUCE_ONE(max_abs,REDUCE_MAXABS)

#undef REDUCE_ONE

#define REDUCE_DIFF_OF_TWO(NAME,OP)					\
									\
double ElementWiseConst::NAME (const ElementWiseConst& another) const	\
{									\
  sure_compatible_with(another);					\
  register double norm = 0;						\
  register const REAL * sp = another.start_ptr;				\
  register const REAL * tp = start_ptr;					\
  while( tp < end_ptr )							\
    OP(norm,*tp++ - *sp++);						\
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
 *			Compute matrix norms
 */

				// Row matrix norm
				// MAX{ SUM{ |M(i,j)|, over j}, over i}
				// The norm is induced by the infinity
				// vector norm
double Matrix::row_norm(void) const
{
  is_valid();
  register const REAL * ep = elements;
  register double norm = 0;

  while(ep < elements+nrows)		// Scan the matrix row-after-row
  {
    register int j;
    register double sum = 0;
    for(j=0; j<ncols; j++,ep+=nrows)	// Scan a row to compute the sum
      sum += ::abs(*ep);
    ep -= nelems - 1;			// Point ep to the beg of the next row
    norm = ::max(norm,sum);
  }
  assert( ep == elements + nrows );

  return norm;
}

				// Column matrix norm
				// MAX{ SUM{ |M(i,j)|, over i}, over j}
				// The norm is induced by the 1.
				// vector norm
double Matrix::col_norm(void) const
{
  is_valid();
  register const REAL * ep = elements;
  register double norm = 0;

  while(ep < elements+nelems)		// Scan the matrix col-after-col
  {					// (i.e. in the natural order of elems)
    register int i;
    register double sum = 0;
    for(i=0; i<nrows; i++)		// Scan a col to compute the sum
      sum += ::abs(*ep++);
    norm = ::max(norm,sum);
  }
  assert( ep == elements + nelems );

  return norm;
}


				// Square of the Euclidian norm
				// SUM{ m(i,j)^2 }
double Matrix::e2_norm(void) const
{
  return of_every(*this).sum_squares();
}

				// Square of the Euclidian norm of the
				// difference between two matrices
double e2_norm(const Matrix& m1, const Matrix& m2)
{
  return of_every(m1).sum_squares(of_every(m2));
}

/*
 *------------------------------------------------------------------------
 * 			Some service operations
 */

ostream& operator << (ostream& os, const DimSpec& dimspec)
{
  return os << dimspec.row_lwb << ':' << dimspec.nrows+dimspec.row_lwb-1 << 'x'
            << dimspec.col_lwb << ':' << dimspec.ncols+dimspec.col_lwb-1;
}

ostream& operator << (ostream& os, const AREALMark& mark)
{
  return (bool)mark ?
  	os << "stream mark at " << mark.offset
      : os << "invalid stream mark";
}

ostream& operator << (ostream& os, const rowcol& rc)
{
  return os << '(' << rc.row << ',' << rc.col << ')';
}

ostream& operator << (ostream& os, const IRange range)
{
  os << '[';
  if( range.lwb == -IRange::INF )
    os << "-INF";
  else if( range.lwb == IRange::INF )
    os << "INF";
  else
    os << range.lwb;
  os << ',';
  if( range.upb == -IRange::INF )
    os << "-INF";
  else if( range.upb == IRange::INF )
    os << "INF";
  else
    os << range.upb;
  return os << ']';
}

ostream& operator << (ostream& os, const RWWatchDog& wd)
{
  if( wd.q_exclusive() )
    return os << "is being exclusively held";
  else if ( !wd.q_engaged() )
    return os << "is not engaged";
  else
    return os << "is shared among " << wd.ref_count << " views";
}

volatile void RWWatchDog::access_violation(const char reason [])
{
  cerr << "RWWatchDog growls at " << reason
       << "\nwhile the guarded object " << *this << endl;
  _error("The program is terminated due to a reason above");
}

void Matrix::info(void) const	// Print some information about the matrix
{
  is_valid();
  cerr << "\nMatrix " << static_cast<const DimSpec&>(*this) << ' '
       << name << ' ' << ref_counter << endl;
}

  			// The matrix in question is not valid
  			// ==> crash the program
volatile void Matrix::invalid_matrix(void) const
{
  _error("An operation attempted on an invalid matrix reference: 0x%0x\n",
  	(long)this);
}

  	  		// Dump the current status of the stream
ostream& AREALBlockStreamIn::dump(ostream& os) const
{
  return
  os << "\nAREALBlockStreamIn: first_el_p " << hex << ((long int)first_el_p) << dec
      << ", last_el_p +" << (last_el_p - first_el_p)
      << "\ncurrent pos " << (curr_el_p - first_el_p)
      << ", last_col_el_p " << (last_col_el_p - first_el_p)
      << ", col_size " << col_size << ", eoc_jump " << eoc_jump << endl;
}


				// Print the Matrix as a table of elements
				// (zeros are printed as dots)
void Matrix::print(const char title []) const
{
  is_valid();
  cerr << title << " follows " << endl;
  info();

  const int cols_per_sheet = 6;
  ConstMatrixDA accessor(*this);
  
  for(register int sheet_counter=1;
      sheet_counter<=q_ncols(); sheet_counter += cols_per_sheet)
  {
    message("\n\n     |");
    for(register int j=q_col_lwb() + sheet_counter - 1;
        j<q_col_lwb() - 1 + sheet_counter+cols_per_sheet && j<=q_col_upb(); j++)
      message("   %6d  |",j);
    message("\n%s\n",_Minuses);
    for(register int i=q_row_lwb(); i<=q_row_upb(); i++)
    {
      message("%4d |",i);
      for(register int j=q_col_lwb() + sheet_counter - 1;
          j<q_col_lwb() - 1 + sheet_counter+cols_per_sheet && 
          j<=q_col_upb(); j++)
	message("%11.4g  ",accessor(i,j));
      message("\n");
    }
  }
  message("Done\n");
}

//#include <builtin.h>
void compare(			// Compare the two Matrices
	const Matrix& matrix1,	// and print out the result of comparison
	const Matrix& matrix2,
	const char title [] )
{
  are_compatible(matrix1,matrix2);

  cerr << "\n\nComparison of two Matrices:\n\t" << title;
  matrix1.info();
  matrix2.info();

  double norm1 = 0, norm2 = 0;		// Norm of the Matrices
  double ndiff = 0;			// Norm of the difference
  REAL difmax = -1;

  AREALMark max_mark;		// For the elements that differ most
  LAStreamIn m1_stream(matrix1), m2_stream(matrix2);
  while( !m1_stream.eof() )
  {
    const REAL mv1 = m1_stream.get();
    const REAL mv2 = m2_stream.get();
    const REAL diff = abs(mv1-mv2);

    if( diff > difmax )
      difmax = diff, max_mark = m1_stream.tell_prev();
    norm1 += abs(mv1);
    norm2 += abs(mv2);
    ndiff += diff;
  }
  assert( m2_stream.eof() );
  
  cerr << "\nMaximal discrepancy    \t\t" << difmax;
  cerr <<"\n   occured at the point\t\t" << m1_stream.get_pos(max_mark);
  const REAL mv1 = m1_stream.seek(max_mark).peek();
  const REAL mv2 = m2_stream.seek(max_mark).peek();
  cerr << "\n Matrix 1 element is    \t\t" << mv1;
  cerr << "\n Matrix 2 element is    \t\t" << mv2;
  cerr << "\n Absolute error v2[i]-v1[i]\t\t" << (mv2-mv1);
  cerr << "\n Relative error\t\t\t\t" << (mv2-mv1)/max((double)abs(mv2+mv1)/2,1e-7)
       << endl;

  cerr << "\n||Matrix 1||   \t\t\t" << norm1;
  cerr << "\n||Matrix 2||   \t\t\t" << norm2;
  cerr << "\n||Matrix1-Matrix2||\t\t\t\t" << ndiff;
  cerr << "\n||Matrix1-Matrix2||/sqrt(||Matrix1|| ||Matrix2||)\t" 
       << ndiff/max( sqrt(norm1*norm2), 1e-7 ) << endl;

}

/*
 *------------------------------------------------------------------------
 *			Service validation functions
 */

void verify_element_value(const Matrix& m,const REAL val)
{
  struct MaxDev : public ConstElementAction
  {
    int imax, jmax;
    double max_dev;
    const REAL expected_val;
    REAL found_val;
    MaxDev(const REAL val) : imax(-1), jmax(-1), max_dev(0), 
    	expected_val(val), found_val(0) {}
    void operation(const REAL element, const int i, const int j)
    	{ const double dev = ::abs(element - expected_val);
    	  if( dev >= max_dev )
    	    max_dev = dev, found_val = element, imax =i, jmax = j; }
  };

  MaxDev maxdev(val);
  m.apply(maxdev);
  
  if( maxdev.max_dev == 0 )
    return;
  else if( maxdev.max_dev < 1e-5 )
    message("Element (%d,%d) with value %g differs the most from what\n"
	    "was expected, %g, though the deviation %g is small\n",
	    maxdev.imax,maxdev.jmax,maxdev.found_val,val,maxdev.max_dev);
  else
    _error("A significant difference from the expected value %g\n"
	   "encountered for element (%d,%d) with value %g",
	   val,maxdev.imax,maxdev.jmax,maxdev.found_val);
}


void verify_matrix_identity(const Matrix& m1, const Matrix& m2)
{
  are_compatible(m1,m2);
  
  AREALMark max_mark;		// For the elements that differ most
  register double max_dev = 0;
  LAStreamIn m1_stream(m1), m2_stream(m2);
  while( !m1_stream.eof() )
  {
    register const double dev = abs(m1_stream.get() - m2_stream.get());
    if( dev >= max_dev )
      max_dev = dev, max_mark = m1_stream.tell_prev();
  }

  if( max_dev == 0 )
    return;
  cerr << "Two " << m1_stream.get_pos(max_mark)
       << " elements of matrices with values "
       << m1_stream.seek(max_mark).peek() << " and "
       << m2_stream.seek(max_mark).peek() << endl;
  if( max_dev < 1e-5 )
    cerr << "differ the most, although the deviation "
	 << max_dev << " is small" << endl;
  else
    _error("with a significant difference %g\n",max_dev);
}
