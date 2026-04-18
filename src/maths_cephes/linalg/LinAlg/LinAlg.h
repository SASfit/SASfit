// This may look like C code, but it is really -*- C++ -*-
/*
 ************************************************************************
 *
 *			  Linear Algebra Package
 *
 * The present package implements all the basic algorithms dealing
 * with vectors, matrices, matrix columns, etc.
 * Matrix is a basic object in the package; vectors, symmetric matrices,
 * etc. are considered matrices of a special type.
 *
 * Matrix elements are arranged in memory in a COLUMN-wise
 * fashion (in FORTRAN's spirit). In fact, this makes it very easy to
 * pass the matrices to FORTRAN procedures implementing more
 * elaborate algorithms.
 *
 * Matrix and vector indices always start with 1, spanning up to 
 * a given limit, unless specified otherwise by a user. 
 *
 * The present package provides all facilities to completely AVOID returning
 * matrices. If one really needs to return a matrix, return
 * a LazyMatrix object instead. The conversion is completely transparent
 * to the end user, e.g. "Matrix m = haar_matrix(5);" and _is_ efficient.
 *
 * Container and views: object of a class Matrix is the only one that
 * owns the data, that is, a 2D grid of values. All other classes provide
 * different views of this grid (as a long 1D array of a matrix stretched
 * column-by-column, a view of a single matrix row, column, or a diagonal,
 * a view of a certain matrix block, etc).
 *
 * The views are designed to be as lightweight as possible: most of
 * the view functionality could be inlined. That is why views do not
 * have any virtual functions. The view classes are supposed to be
 * final, in JAVA parlance. It makes little sense to inherit from them.
 * It is unnecessary as views are designed to be flexible and efficient,
 * they provide a wide variety of access to the matrix with safety and
 * as efficient as possible. So flexible that many former Matrix methods
 * can be implemented outside of the Matrix class as they no longer need
 * a priviledged access to Matrix internals, without any loss of performance.
 *
 * Importance of Matrix streams: a sequential view/access to a matrix or
 * its parts. Many of Linear Algebra algorithms actually require only
 * sequential access to a matrix or its rows/columns, which is simpler and
 * faster than a random access. That's why LinAlg stresses streams. There are
 * two kinds of sequential views: ElementWise are transient views that
 * typically exist only for the duration of a elementwise action.
 * That's why it doesn't require a "locking" of a matrix. On the other hand,
 * LAStream and the ilk are more permanent views of the matrix, with
 * roughly the same functionality. An application itself can traverse the
 * stream at its own convenience, bookmarking certain spots and possibly
 * returning to them later. LAStream does assert a shared lock on the matrix,
 * to prevent the element matrix from moving/disappearing.
 *
 * $Id: LinAlg.h,v 4.3 1998/12/22 21:02:55 oleg Exp oleg $
 *
 ************************************************************************
 */

#ifndef __GNUC__
#pragma once
#endif
#ifndef _LinAlg_h
#define _LinAlg_h 1

#if defined(__GNUC__)
#pragma interface
#define __unused(X) X __attribute__((unused))
class ostream;			// An Opaque class
#else
#include <iostream.h>	  // When ostream is a template, can't make it opaque
#define __unused(X) X
#endif

#include "myenv.h"
#include "std.h"
#if defined(__GNUC__)
#include <values.h>
#else
#include <limits.h>
#define MAXINT INT_MAX
#endif
#include <math.h>
#include "builtin.h"
#include "minmax.h"
#if defined(index)
#undef index
#endif

/*
 *------------------------------------------------------------------------
 *			Auxiliary classes
 */

typedef float REAL;			// Scalar field of the Linear Vector
					// space

			// Dimensions specifier of a 2D object
class DimSpec
{
protected:
  int nrows;				// No. of rows
  int ncols;				// No. of columns
  int row_lwb;				// Lower bound of the row index
  int col_lwb;				// Lower bound of the col index

public:

  DimSpec(const int _nrows, const int _ncols)	// Indices start with 1
  	: nrows(_nrows), ncols(_ncols), row_lwb(1), col_lwb(1)
  	  { assert(nrows>0 && ncols>0); }
  DimSpec(const int _row_lwb, const int _row_upb,	// Or with low:upper
	  const int _col_lwb, const int _col_upb)	// boundary specif.
   	: nrows(_row_upb-_row_lwb+1), 
   	  ncols(_col_upb-_col_lwb+1),
   	  row_lwb(_row_lwb), col_lwb(_col_lwb)
  	  { assert(nrows>0 && ncols>0); }
  
				// Status inquires
  int q_row_lwb(void) const			{ return row_lwb; }
  int q_row_upb(void) const			{ return nrows+row_lwb-1; }
  int q_nrows(void) const			{ return nrows; }
  int q_col_lwb(void) const			{ return col_lwb; }
  int q_col_upb(void) const			{ return ncols+col_lwb-1; }
  int q_ncols(void) const			{ return ncols; }
  
  bool operator == (const DimSpec& another_dim_spec) const
  	{ return nrows == another_dim_spec.nrows &&
  		 ncols == another_dim_spec.ncols &&
  		 row_lwb == another_dim_spec.row_lwb &&
  		 col_lwb == another_dim_spec.col_lwb; }
 
  friend ostream& operator << (ostream& os, const DimSpec& dimspec);
};

				// A 2D index, a pair representing
				// a row,col location in some 2D structure
struct rowcol
{
  int row, col;
  rowcol(const int _row, const int _col) : row(_row), col(_col) {}
  bool operator == (const rowcol another) const
  	{ return row == another.row && col == another.col; }
  friend ostream& operator << (ostream& os, const rowcol& rc);
};

		// A Range of indices: lwb:upb
		// if lwb > upb, the range is "empty"
		// If lwb is not specified, it defaults to -IRange::INF
		// (meaning the range spans from the earliest possible
		// element in a collection the range is being applied to)
		// If upb is not specified, it defaults to IRange::INF
		// meaning the range spans through the end of the collection,
		// whatever that may be.
struct IRange
{
  const int lwb, upb;
  enum { INF = MAXINT };
  IRange(const int _lwb, const int _upb) : lwb(_lwb), upb(_upb) {}
  static IRange from(const int lwb) { return IRange(lwb,INF); }
  static IRange through(const int upb) { return IRange(-INF,upb); }
  bool q_empty(void) const	{ return lwb > upb; }
  bool q_proper(void) const
  	{ return abs(lwb) != INF && abs(upb) != INF && lwb <= upb; }
  friend ostream& operator << (ostream& os, const IRange range);
  int no_less_than(const int strict_lwb) const
   	{ if( lwb == -IRange::INF ) return strict_lwb;
	  assert(lwb >= strict_lwb); return lwb; }
  int no_more_than(const int strict_upb) const
   	{ if( upb == IRange::INF ) return strict_upb;
	  assert(upb <= strict_upb); return upb; }
};

		// The same as above, but with a given stride
struct IRangeStride
{
  const int lwb, upb, stride;
  IRangeStride(const int _lwb, const int _upb, const int _stride)
  	: lwb(_lwb), upb(_upb), stride(_stride) {}
  IRangeStride(const IRange range)
  	: lwb(range.lwb), upb(range.upb), stride(1) {}
  bool q_empty(void) const	{ return lwb > upb; }
  bool q_proper(void) const
  	{ return abs(lwb) != IRange::INF && abs(upb) != IRange::INF
		&& lwb <= upb && stride > 0; }
};


		// A cut from DimSpec in two dimensions
class DimSpecSubranged : public DimSpec
{
protected:
   int min_offset;	// An offset to a[imin,jmin] from the original DimSpec
   int max_offset;	// An offset to a[imax,jmax] from the original DimSpec
   const int original_nrows;	// The number of rows in the original DimSpec

public:
 DimSpecSubranged(const DimSpec& orig, const IRange row_range,
		  const IRange col_range)
 	: DimSpec(row_range.no_less_than(orig.q_row_lwb()),
		  row_range.no_more_than(orig.q_row_upb()),
		  col_range.no_less_than(orig.q_col_lwb()),
		  col_range.no_more_than(orig.q_col_upb())),
	  original_nrows(orig.q_nrows())
	  { min_offset = (col_lwb - orig.q_col_lwb())*original_nrows +
	  		 (row_lwb - orig.q_row_lwb());
	    max_offset = min_offset + (ncols-1)*original_nrows + nrows - 1;
	  }
		// we are sure row_lwb is within [orig.row_lwb,orig.row_upb]
	  	// and so is the new row_upb (and the same for col)

  int q_min_offset(void) const		{ return min_offset; }
  int q_max_offset(void) const		{ return max_offset; }
  int q_row_diff(void) const		{ return original_nrows - nrows; }
};
				// An interface describing an operation to apply
				// to a matrix element.
				// This interface is used by Matrix::apply()
				// and similar for-each-type iterators. They
				// call a concrete instance of this interface
				// on each element of a matrix etc. collection
				// under consideration.
				// The operation receives the (i,j) index of
				// the current element matrix and its _value_:
				// thus this interface may not modify the
				// element itself.
class ConstElementAction
{
public:
  virtual void operation(const REAL element, const int i, const int j) = 0;
};

				// The same as above, only an operation
				// is allowed to modify a matrix element
				// it is being applied to.
class ElementAction
{
public:
  virtual void operation(REAL& element, const int i, const int j) = 0;

//private:			// Those aren't implemented; but making them
				// private forbids the assignement
//  ElementAction(const ElementAction&);
//  void operator= (const ElementAction&);
};


				// An interface that performs an operation
				// on two elements of Matrices being jointly
				// traversed. The operation receives the values
				// of the two current elements in both matrices
				// and the element's index (which must be
				// the same for each matrix involved).
				// As interface recieves elements' _values_
				// it may not modify the elements.
				// This interface is being used to compute
				// some property of two matrices being jointly
				// traversed.
class ConstElementAction2
{
public:
  virtual void operation(const REAL element1, const REAL element2,
			 const int i, const int j) = 0;
};

				// Lazy matrix constructor
				// That is, a promise of a matrix rather than
				// a matrix itself. The promise is forced
				// by a Matrix constructor or assignment
				// operator, see below.
				// It's highly recommended a function never
				// return Matrix itself. Use LazyMatrix
				// instead
class LazyMatrix : public DimSpec
{
  friend class Matrix;
  virtual void fill_in(Matrix& m) const = 0;

				// Not implemented: cloning is prohibited
  //  LazyMatrix(const LazyMatrix&);
  LazyMatrix& operator = (const LazyMatrix&);
public:
  LazyMatrix(const int nrows, const int ncols)	// Indices start with 1
    : DimSpec(nrows,ncols) {}
  LazyMatrix(const int row_lwb, const int row_upb,// Or with low:upper
	     const int col_lwb, const int col_upb)// boundary specif.
    : DimSpec(row_lwb,row_upb,col_lwb,col_upb) {}
  LazyMatrix(const DimSpec& dims) : DimSpec(dims) {}
};

				// A pair specifying extreme values
				// of some domain/result
class MinMax
{
  REAL min_val, max_val;		// Invariant: max_val >= min_val
public:
  MinMax(REAL _min_val, REAL _max_val)
    : min_val(_min_val), max_val(_max_val) {}
  explicit MinMax(REAL val)
    : min_val(val), max_val(val) {}
  MinMax& operator << (REAL val)
  	{ if( val > max_val ) max_val = val;
	  else if(val < min_val) min_val = val; return *this; }
  REAL min(void) const		{ return min_val; }
  REAL max(void) const		{ return max_val; }
  double ratio(void) const	{ return max_val/min_val; }
  friend ostream& operator << (ostream& os, const MinMax& mx);
};

                // The following class watches over exclusive (write)
                // and shared (read) access to a data structure the object
                // is a member of.
                // The ref_count field tells how the object is being
                // currently accessed:
                //      = 0: the object is not engaged
                //      =-1: exclusive (write access)
                //      >0 : the object is being currently read (viewed)
                //      by one or several readers
                // The watchdog watches over obvious blunders as trying
                // to get a read access while the object is being exclusively
                // held, or trying to grab the write access or destroy the
                // object while it is engaged in some other activity.
                // Note, if an object contains a pointer to some data
                // structure, using this pointer requires a read (shared)
                // access, even if the data structure would be modified.
                // Indeed, as long as all 'readers' access the data structure
                // at the same place in memory, they certainly are manipulating
                // the same (and the real) thing. Modifying a data structure
                // _pointer_ on the other hand requires an exclusive access
                // to the object (pointer holder). Thus read/write access
                // is meant a shared/exclusive access to
                // object's data and object's layout (but not the contents
                // of the data itself)
class RWWatchDog
{
  int ref_count;                        // see explanation above

  RWWatchDog(const RWWatchDog&);        // Deliberately unimplemented:
  void operator = (const RWWatchDog&);  // no cloning allowed!

  volatile void access_violation(const char reason []);

public:
  bool q_engaged(void) const            { return ref_count != 0; }
  bool q_shared(void) const             { return ref_count > 0; }
  bool q_exclusive(void) const          { return ref_count == -1; }

  void get_exclusive(void)
        { if( q_engaged() ) access_violation("get_exclusive"); ref_count = -1; }
  void release_exclusive(void)
        { if(!q_exclusive()) access_violation("release_exclusive");
          ref_count = 0; }

  void get_shared(void)
        { if( q_exclusive() ) access_violation("get_shared"); ref_count++; }
  void release_shared(void)
        { if( !q_shared() ) access_violation("release_shared"); ref_count--; }

  RWWatchDog(void) : ref_count(0) {}
  ~RWWatchDog(void)
  	{ if( ref_count!=0 ) access_violation("destroying"); 
  	  ref_count = -1; }

  				// Dump the current status
  friend ostream& operator << (ostream& os, const RWWatchDog& wd);
};

#if 0
class MinMaxLoc : public MinMax
{
};
#endif


/*
 *------------------------------------------------------------------------
 *	Special kinds of matrices that have to be forward-declared
 */
 
class Matrix;
class MatrixColumn;
class ConstMatrixColumn;
class MatrixRow;
class MatrixDiag;
				// Create an orthogonal (2^n)*(no_cols) Haar
				// (sub)matrix, whose columns are Haar
				// functions
				// If no_cols is 0, create the complete matrix
				// with 2^n columns
class haar_matrix : public LazyMatrix
{
  void fill_in(Matrix& m) const;
public:
  haar_matrix(const int n, const int no_cols=0);
};

				// Lazy transposition
class LazyTransposedMatrix : public LazyMatrix
{
  const Matrix& proto;
  void fill_in(Matrix& m) const;
  inline LazyTransposedMatrix(const Matrix & _proto);
public:
  friend inline const LazyTransposedMatrix transposed(const Matrix & proto);
};

/*
 *------------------------------------------------------------------------
 *	Data classes: Matrix and (its particular case) Vector
 *
 * Note that view classes (below) may "borrow" matrix data area
 * for a while; to make sure they "return" the data, we use a limited form
 * of reference counting. That is, the destruction/reallocation of a data
 * class are only possible when the reference counter is 1
 * (that is, there are no oustanding views). Otherwise it is a grave error.
 *
 */

class Matrix : public DimSpec
{
public:
  friend class ElementWiseConst;
  friend class ElementWiseStrideConst;
  friend class LAStreamIn;
  friend class LAStreamOut;
  friend class LAStrideStreamIn;
  friend class LAStrideStreamOut;
  friend class LABlockStreamIn;
  friend class LABlockStreamOut;
  friend class MatrixDABase;
  class ConstReference;			// Forward decl of nested classes
  class Reference;
  friend class ConstReference;
  
  friend class Vector;
  friend class ConstMatrixColumn;
  friend class ConstMatrixRow;
  friend class ConstMatrixDiag;

private:			// Private part
  int valid_code;			// Validation code
  enum { MATRIX_val_code = 5757 };	// Matrix validation code value
  RWWatchDog ref_counter;

protected:			// May be used in derived classes
  const char * name;				// Name for the matrix
  int nelems;				// Total no of elements, nrows*ncols
  REAL * elements;			// Elements themselves

  void allocate(void);

  friend void haar_matrix::fill_in(Matrix& m) const;
  friend void LazyTransposedMatrix::fill_in(Matrix& m) const;

public:			// Public interface

				// Constructors and destructors
					// Make a blank matrix
  Matrix(const int nrows, const int ncols);	// Indices start with 1
  Matrix(const int row_lwb, const int row_upb,	// Or with low:upper
	 const int col_lwb, const int col_upb);	// boundary specif.
  Matrix(const DimSpec& dimension_specs);
  inline Matrix(const Matrix&  another); // A real copy constructor, expensive


					// Construct a matrix applying a spec
					// operation to two prototypes
					// Example: Matrix A(10,12), B(12,5);
					// Matrix C(A,Matrix::Mult,B);
//  enum MATRIX_CREATORS_2op { Mult, 		// A*B
//			     TransposeMult, 	// A'*B
//			     InvMult, 		// A^(-1)*B
//			     AtBA }; 		// A'*B*A
//  Matrix(const Matrix& A, const MATRIX_CREATORS_2op op, const Matrix& B);
//  Matrix(const Vector& x, const Vector& y);	// x'*y (diad) matrix

  Matrix(const LazyMatrix& lazy_constructor);//Make a matrix using given recipe
  explicit Matrix(const char * file_name);	// Read the matrix from the file
					// (not yet implemented!)

  ~Matrix(void);			// Destructor

  void set_name(const char * name);	// Set a new matrix name

					// Erase the old matrix and create a
					// new one according to new boundaries
  void resize_to(const int nrows, const int ncols);	// Indexation @ 1
  void resize_to(const int row_lwb, const int row_upb,	// Or with low:upper
		 const int col_lwb, const int col_upb);	// boundary specif.
  void resize_to(const DimSpec& dimension_specs);	// Like other matrix m


  void is_valid(void) const
  { if( valid_code != MATRIX_val_code ) invalid_matrix(); }

				// Status inquires
  int q_no_elems(void) const		{ return nelems; }
  const char * q_name(void) const	{ return name; }

  class ConstReference
  {
    friend class Reference;
    Matrix& m;
    ConstReference(const ConstReference&);	// Not implemented and forbidden
    void operator = (const ConstReference&);	// reference isn't transferable
  public:
    ConstReference(const Matrix& _m)
    	: m(const_cast<Matrix&>(_m))
    	{ m.is_valid(); m.ref_counter.get_shared(); }
   ~ConstReference(void) { m.is_valid(); m.ref_counter.release_shared(); }
    /*explicit*/ operator const Matrix& (void) const { return m; }
    const Matrix& ref(void) const { return m; }
  };

  class Reference : public ConstReference
  {
    Reference(const Reference&);	// Not implemented and forbidden
    void operator = (const Reference&);	// reference isn't transferable
  public:
    Reference(Matrix& _m) : ConstReference(_m) {}
    /*explicit*/ operator Matrix& (void) const { return m; }
    Matrix& ref(void) const { return m; }
  };

					// Apply a user-defined action
  Matrix& apply(ElementAction& action);		// to each matrix element
  const Matrix& apply(ConstElementAction& action) const; // to each matrix element

  					// Although the following method does
  					// not need a priviledged access to a
  					// Matrix, it is required to be a member
  					// of the class
  inline Matrix& operator = (const REAL val);
					// Invert the matrix returning the
					// determinant if desired
					// determinant = 0 if the matrix is
					// singular
					// If determ_ptr=dummy_determinant_ref
					// and the matrix *is*
					// singular, throw up
  static double dummy_determinant_ref;
  Matrix& invert(double &determ_ref = dummy_determinant_ref);

				// Element-wise operations on two matrices
  inline Matrix& operator = (const Matrix& source);	// Assignment
  inline Matrix& operator = (const ConstReference& ref)
  		{ return *this = ref.ref(); }
  Matrix& operator = (const LazyMatrix& source);// Force the promise of a matrix
  Matrix& clear(void);			// Clear the matrix (fill out with 0)
 

					// Comparisons
  bool operator == (const Matrix& im2) const;
  friend inline void are_compatible(const Matrix& im1, const Matrix& im2);


				// True matrix operations
				// (on a matrix as a whole)  
  Matrix& operator *= (const Matrix& source);	// Inplace multiplication
						// possible only for square src
  inline Matrix& operator *= (const ConstReference& ref)
  		{ return *this *= ref.ref(); }

  
  void mult(const Matrix& A, const Matrix& B);  // Compute A*B
 
				// Compute matrix norms
  double row_norm(void) const;		// MAX{ SUM{ |M(i,j)|, over j}, over i}
  double norm_inf(void) const		// Alias, shows the norm is induced
         { return row_norm(); }		// 	by the vector inf-norm
  double col_norm(void) const;		// MAX{ SUM{ |M(i,j)|, over i}, over j}
  double norm_1(void) const		// Alias, shows the norm is induced
         { return col_norm(); }		// 	by the vector 1-norm
  double e2_norm(void) const;		// SUM{ m(i,j)^2 }, Note it's square
					// of the Frobenius rather than 2. norm

  friend double e2_norm(const Matrix& m1, const Matrix& m2);
					// e2_norm(m1-m2)

  double determinant(void) const;	// Matrix must be a square one

  double asymmetry_index(void) const;	// How far is the matrix from being
					// symmetrical (0 means complete symm)
					// (not yet implemented)

				// Make matrices of a special kind
  Matrix& unit_matrix(void);		// Matrix needn't be a square
  Matrix& hilbert_matrix(void);		// Hilb[i,j] = 1/(i+j-1); i,j=1..max

				// I/O: write, read, print 
  					// Write to a file
					// "| command name" is OK as a file
					// name
  void write(const char * file_name,const char * title = "") const;
  void info(void) const;		// Print the info about the Matrix
  void print(const char title []) const;// Print the Matrix as a table
  volatile void invalid_matrix(void) const;   // The matrix in question is not valid
  					// ==> crash the program
};

void compare(const Matrix& im1, const Matrix& im2, const char title[]);


/*
 *------------------------------------------------------------------------
 *		   Vector as a n*1 matrix (that is, a col-matrix)
 */
class Vector : public Matrix
{
public:
  Vector(const int n);		// Create a blank vector of a given size
				// (indexation starts at 1)
				// Create a general lwb:upb vector blank vector
  Vector(const int lwb, const int upb);
//  Vector(const Vector& another);	// a copy constructor (to be inferred)

					// Make a vector and assign init vals
  Vector(const int lwb, const int upb,  // lower and upper bounds
	 double iv1, ...    		// DOUBLE numbers. The last arg of
	 ); 				// the list must be string "END"
					// E.g: Vector f(1,2,0.0,1.5,"END");

  Vector(const LazyMatrix& lazy_constructor);//Make a vector using given recipe

				// Resize the vector (keeping as many old
				// elements as possible), expand by zeros
  void resize_to(const int n);			// Indexation @ 1
  void resize_to(const int lwb, const int upb); 	// lwb:upb specif
  void resize_to(const Vector& v);			// like other vector

  				// Unlike Matrix, Vector permits a direct
  				// access to its elements without further ado
  				// Still, streams/of_every() are more efficient
  				// for uniform access
  inline REAL& operator () (const int index);
  inline REAL  operator () (const int index) const
	{ return (const_cast<Vector&>(*this))(index); }

				// Listed below are specific vector operations
				// (unlike n*1 matrices)

					// Status inquires
  int q_lwb(void) const		{ return q_row_lwb(); }
  int q_upb(void) const		{ return q_row_upb(); }

					// Compute the scalar product
  friend double operator * (const Vector& v1, const Vector& v2);

					// "Inplace" multiplication
					// target = A*target
					// A needn't be a square one (the
					// target will be resized to fit)
  Vector& operator *= (const Matrix& A);

					// Vector norms
  inline double norm_1(void) const;   		// SUM{ |v[i]| }
  inline double norm_2_sqr(void) const;	       	// SUM{ v[i]^2 }
  inline double norm_inf(void) const;		// MAX{ |v[i]| }

  Vector& operator = (const Vector& v)
  	{ Matrix::operator =(v); return *this; }
  Vector& operator = (const LazyMatrix& source);// Force the promise of a vector

  					// Although the following methods do
  					// not need a priviledged access to
  					// Vector, they are required to be
  					// members of the class
  inline Vector& operator = (const ElementWiseConst& stream);
  inline Vector& operator = (const ElementWiseStrideConst& stream);
  inline Vector& operator = (const REAL val)
  	{ Matrix::operator =(val); return *this; }
};

				// Service functions (useful in the
				// verification code). They print some detail
				// info if the validation condition fails
void verify_element_value(const Matrix& m,const REAL val);
void verify_matrix_identity(const Matrix& m1, const Matrix& m2);
inline void verify_element_value(const Matrix::ConstReference& m,const REAL val)
	{ verify_element_value(m.ref(),val); }
inline void verify_matrix_identity(const Matrix::ConstReference& m1,
				   const Matrix& m2)
	{ verify_matrix_identity(m1.ref(),m2); }
inline void verify_matrix_identity(const Matrix& m1,
				   const Matrix::ConstReference& m2)
	{ verify_matrix_identity(m1,m2.ref()); }
inline void verify_matrix_identity(const Matrix::ConstReference& m1,
				   const Matrix::ConstReference& m2)
	{ verify_matrix_identity(m1.ref(),m2.ref()); }

				// Multiply by the diagonal
				// of another matrix
Matrix& operator *= (Matrix& m, const ConstMatrixDiag& diag);

				// Compute the product of two matrices
class LazyMatrixProduct : public LazyMatrix
{
  const Matrix& A;
  const Matrix& B;
  void fill_in(Matrix& m) const { m.mult(A,B); }
  LazyMatrixProduct(const Matrix& _A, const Matrix& _B)
	: LazyMatrix(_A.q_row_lwb(),_A.q_row_upb(),
		     _B.q_col_lwb(),_B.q_col_upb()),
	  A(_A), B(_B) {}
  
public:
  friend const LazyMatrixProduct operator * (const Matrix& A, const Matrix& B)
    { return LazyMatrixProduct(A,B); }
};

				// Make a zero matrix
class LazyZeroMatrix : public LazyMatrix
{
  void fill_in(Matrix& m) const { __unused(Matrix& m1) = m; }
  LazyZeroMatrix(const Matrix& proto)
    : LazyMatrix(static_cast<const DimSpec&>(proto)) {}
public:
  friend const LazyZeroMatrix zero(const Matrix& proto)
    { proto.is_valid(); return proto; }
};

				// Make a unit matrix
class LazyUnitMatrix : public LazyMatrix
{
  void fill_in(Matrix& m) const { m.unit_matrix(); }
  LazyUnitMatrix(const Matrix& proto)
    : LazyMatrix(static_cast<const DimSpec&>(proto)) {}
public:
  friend const LazyUnitMatrix unit(const Matrix& proto)
    { proto.is_valid(); return proto; }
};

				// Make an inverse matrix
class LazyInverseMatrix : public LazyMatrix
{
  const Matrix& proto;
  void fill_in(Matrix& m) const { m = proto; m.invert(); }
  LazyInverseMatrix(const Matrix& _proto)
    : LazyMatrix(static_cast<const DimSpec&>(_proto)),
      proto(_proto) {}
public:
  friend const LazyInverseMatrix inverse(const Matrix& proto)
    { proto.is_valid(); return proto; }
};


				// Make a transposed matrix
inline LazyTransposedMatrix::LazyTransposedMatrix(const Matrix & _proto)
  	: LazyMatrix(_proto.q_col_lwb(),_proto.q_col_upb(),
  		     _proto.q_row_lwb(),_proto.q_row_upb()),
  	  proto(_proto) {}
inline const LazyTransposedMatrix transposed(const Matrix & proto)
  	{ proto.is_valid(); return proto; }

/*
 *------------------------------------------------------------------------
 *	Matrix Views: provide access to elements of a Matrix
 *	(or groups of elements: rows, columns, diagonals, etc)
 * Although views can be created and exist for some time, one should
 * not attempt to pass around views (especially return them as a result
 * of the function) when the data object itself (a matrix) could be
 * disposed of. Thanks to the reference counting, this situation would
 * be detected, and the program would crash.
 */
 
/*
 *------------------------------------------------------------------------
 */

/*
 * Functions/procedures (generally grouped together under a ElementWise
 * class umbrella) that perform a variety of operation on each element
 * of some structure in turn.
 *
 * ElementWiseConst and ElementWise classes are largely a syntactic
 * sugar; you can't explicitly construct an object of this class, on stack
 * or on heap. The only way the class can be used is within a pattern
 *      to_every(matrix) = 1;
 *	if( of_every(matrix) > 1 ) ...
 * etc. Besides, this is the only way it makes sense.
 *
 * Note that the ElementWise classes are friends of Matrix, so they are
 * privy of Matrix internals and can use them to make processing faster.
 */


				// A functor describing an operation to apply 
				// to every element of a collection in
				// turn
				// This operation receives only the elements's
				// value, and thus it may not modify the
				// element under consideration.
class ElementWiseConstAction
{
				// Those aren't implemented; but making them
				// private forbids the assignement
  ElementWiseConstAction(const ElementWiseConstAction&);
  ElementWiseConstAction& operator= (const ElementWiseConstAction&);
public:
  ElementWiseConstAction(void) {}
  virtual void operator () (const REAL element) = 0;
};

				// The same as above but this functor is given
				// a reference to an element, which it can
				// modify
class ElementWiseAction
{
				// Those aren't implemented; but making them
				// private forbids the assignement
  ElementWiseAction(const ElementWiseAction&);
  ElementWiseAction& operator= (const ElementWiseAction&);
public:
  ElementWiseAction(void) {}
  virtual void operator () (REAL& element) = 0;
};

class ElementWiseConst
{
  friend class ElementWise;
  friend class ElementWiseStrideConst;
  friend class ElementWiseStride;

  REAL * const start_ptr;		// Pointer to the beginning of
  					// the group of elements
  REAL * const end_ptr;			// Points after the end of the group
  
  void operator=(const ElementWiseConst&);// is not implemented, ergo, is
                                        // not allowed
  ElementWiseConst(const ElementWiseConst&);// Cloning is not allowed, either
  
				// A private constructor, to make
				// sure the object can't be constructed
				// and left, while the matrix would disappear
				
				// Use of_every() friend to do
				// the actual construction
  ElementWiseConst(const Matrix& m)
  	: start_ptr(const_cast<REAL*>(m.elements)),
  	  end_ptr(const_cast<REAL*>(m.elements+m.nelems))
  	{ m.is_valid(); }

  inline ElementWiseConst(const ConstMatrixColumn& mc);
  
  				// Just a helper class to disambiguate 
  				// type conversion from ElementWiseConst
  				// to ElementWiseStrideConst and others
  struct ElementWiseConstRef
  {
    const ElementWiseConst& ref;
    ElementWiseConstRef(const ElementWiseConstRef&);
    void operator = (const ElementWiseConstRef&);
    ElementWiseConstRef(const ElementWiseConst& _ref) : ref(_ref) {}
  };

public:

					// No public constructors...

                                        // ElementWiseConst(Matrix& m) would be
                                        // implicitly called
  friend inline ElementWiseConst of_every(const Matrix& m) { return m; }
  friend inline ElementWiseConst of_every(const Matrix::ConstReference& m)
  	{ return m.ref(); }
  
  friend inline ElementWiseConst of_every(const ConstMatrixColumn& mc);
 				
  				// Every ElementWiseConst is a
  				// ElementWiseStrideConst with a stride of 1
  inline operator ElementWiseStrideConst (void);
  
				// Comparisons
				// Find out if the predicate
				// "element op val" is true for ALL
				// elements of the group
  bool	  operator ==  (const REAL val) const;	// ? all elems == val
  bool	  operator !=  (const REAL val) const;	// ? all elems != val
  bool	  operator <   (const REAL val) const;	// ? all elems <  val
  bool	  operator <=  (const REAL val) const;	// ? all elems <= val
  bool	  operator >   (const REAL val) const;	// ? all elems >  val
  bool	  operator >=  (const REAL val) const;	// ? all elems >= val
  
				// Find out if the predicate
				// "element op another.elem" is true for ALL
				// elements of the two groups
  bool	  operator ==  (const ElementWiseConst& another) const;
  bool	  operator !=  (const ElementWiseConst& another) const;
  bool	  operator <   (const ElementWiseConst& another) const;
  bool	  operator <=  (const ElementWiseConst& another) const;
  bool	  operator >   (const ElementWiseConst& another) const;
  bool	  operator >=  (const ElementWiseConst& another) const;
				
  void sure_compatible_with(const ElementWiseConst& another) const
  	{ assure(end_ptr-start_ptr == another.end_ptr-another.start_ptr,
  		"Two groups of elements to operate on have different sizes"); }
  
  				// Data reduction: reduce a collection
  				// to one value: a "norm" of the collection
  double sum(void) const;
  double sum_squares(void) const;
  double sum_abs(void) const;
  double max_abs(void) const;
  //double foldr(const double seed, Reductor& reductor);
  
  				// Reduce a difference between two collections
  				// to a single value: a "norm" of the
  				// difference
  double sum_squares(const ElementWiseConst& another) const;
  double sum_abs(const ElementWiseConst& another) const;
  double max_abs(const ElementWiseConst& another) const;
  //double foldr(const double seed, Reductor2& reductor);
  
  				// Just let the user do what he wants
  				// with each element in turn
  				// (without modifying them, of course)
  ElementWiseConstAction& apply(ElementWiseConstAction& functor) const;
};


class ElementWise : public ElementWiseConst
{
  void operator=(const ElementWise&);	// is not implemented, ergo, is
                                        // not allowed
  ElementWise(const ElementWise&);	// Cloning is not allowed, either
  
				// A private constructor, to make
				// sure the object can't be constructed
				// and left, while the matrix would disappear
				
				// Use a to_every() friend to do
				// the actual construction
  ElementWise(Matrix& m) : ElementWiseConst(m) {}
  inline ElementWise(const MatrixColumn& mc);
public:

					// No public constructors...

                                        // ElementWise(Matrix& m) would be
                                        // implicitly called
  friend inline ElementWise to_every(Matrix& m) { return m; }
  friend inline ElementWise to_every(const Matrix::Reference& m)
  	{ return m.ref(); }
//  friend inline ElementWiseConst of_every(const ConstMatrixColumn& mc);
  friend inline ElementWise to_every(const MatrixColumn& mc); // see below

   				// Every ElementWise is a
  				// ElementWiseStride with a stride of 1
  inline operator ElementWiseStride (void);
  				// The same as the conversion operator above,
  				// use when a compiler (gcc 2.8.1) fails to
  				// apply it
  inline ElementWiseStride with_stride (void);

				// group-scalar arithmetics
				// Modify every element of the group
				// according to the operation
  void operator =   (const REAL val);	// Assignment to all the elems
  void operator +=  (const double val);	// Add to elements
  void operator -=  (const double val);	// Take from elements
  void operator *=  (const double val);	// Multiply elements by a val

				// Other element-wise matrix operations
  void abs(void);			// Take an absolute value of a matrix
  void sqr(void);			// Square each element
  void sqrt(void);			// Take the square root

  void operator = (const ElementWiseConst& another);
  void operator += (const ElementWiseConst& another);
  void operator -= (const ElementWiseConst& another);
  void operator *= (const ElementWiseConst& another);
  void operator /= (const ElementWiseConst& another);

  ElementWiseAction& apply(ElementWiseAction& functor);
};

inline Vector& Vector::operator = (const ElementWiseConst& stream)
  	{ to_every(*this) = stream; return *this; }


inline double Vector::norm_1(void) const
	{ return of_every(*this).sum_abs(); }
inline double Vector::norm_2_sqr(void) const
	{ return of_every(*this).sum_squares(); }
inline double Vector::norm_inf(void) const
	{ return of_every(*this).max_abs(); }

//---------------------------------------------------------------------------
//			The same, only with a stride

class ElementWiseStrideConst
{
  friend class ElementWiseStride;
  friend class ElementWiseConst;

  REAL * const start_ptr;		// Pointer to the beginning of
  					// the group of elements
  REAL * const end_ptr;			// Points after the end of the group
  const int stride;
  
  void operator=(const ElementWiseStrideConst&);// is not implemented, ergo, is
                                        // not allowed
  ElementWiseStrideConst(const ElementWiseStrideConst&);// Cloning is not allowed, either
  
				// A private constructor, to make
				// sure the object can't be constructed
				// and left, while the matrix would disappear
				
				// Use of_every() friend to do
				// the actual construction
  ElementWiseStrideConst(const ElementWiseConst::ElementWiseConstRef& ewc)
  	: start_ptr(ewc.ref.start_ptr),
  	  end_ptr(ewc.ref.end_ptr),
  	  stride(1)
  	  {}
  	  
  inline ElementWiseStrideConst(const ConstMatrixRow& mr);
  inline ElementWiseStrideConst(const ConstMatrixDiag& md);

public:

					// No public constructors...
 
 friend inline ElementWiseStrideConst of_every(const ConstMatrixRow& mr);
 friend inline ElementWiseStrideConst of_every(const ConstMatrixDiag& md);

				// Comparisons
				// Find out if the predicate
				// "element op val" is true for ALL
				// elements of the group
  bool	  operator ==  (const REAL val) const;	// ? all elems == val
  bool	  operator !=  (const REAL val) const;	// ? all elems != val
  bool	  operator <   (const REAL val) const;	// ? all elems <  val
  bool	  operator <=  (const REAL val) const;	// ? all elems <= val
  bool	  operator >   (const REAL val) const;	// ? all elems >  val
  bool	  operator >=  (const REAL val) const;	// ? all elems >= val
  
				// Find out if the predicate
				// "element op another.elem" is true for ALL
				// elements of the two groups
  bool	  operator ==  (const ElementWiseStrideConst& another) const;
  bool	  operator !=  (const ElementWiseStrideConst& another) const;
  bool	  operator <   (const ElementWiseStrideConst& another) const;
  bool	  operator <=  (const ElementWiseStrideConst& another) const;
  bool	  operator >   (const ElementWiseStrideConst& another) const;
  bool	  operator >=  (const ElementWiseStrideConst& another) const;
				
//  void sure_compatible_with(const ElementWiseStrideConst& another) const
//  	{ assure(end_ptr-start_ptr == another.end_ptr-another.start_ptr,
//  		"Two groups of elements to operate on have different sizes"); }
  
  				// Data reduction: reduce a collection
  				// to one value: a "norm" of the collection
  double sum(void) const;
  double sum_squares(void) const;
  double sum_abs(void) const;
  double max_abs(void) const;
  //double foldr(const double seed, Reductor& reductor);
  
  				// Reduce a difference between two collections
  				// to a single value: a "norm" of the
  				// difference
  double sum_squares(const ElementWiseStrideConst& another) const;
  double sum_abs(const ElementWiseStrideConst& another) const;
  double max_abs(const ElementWiseStrideConst& another) const;
  //double foldr(const double seed, Reductor2& reductor);
  
  				// Just let the user do what he wants
  				// with each element in turn
  				// (without modifying them, of course)
  ElementWiseConstAction& apply(ElementWiseConstAction& functor) const;
};
  
class ElementWiseStride : public ElementWiseStrideConst
{
  friend class ElementWise;
  void operator=(const ElementWiseStride&);	// is not implemented, ergo, is
                                        // not allowed
  ElementWiseStride(const ElementWiseStride&);	// Cloning is not allowed, either
  
				// A private constructor, to make
				// sure the object can't be constructed
				// and left, while the matrix would disappear
				
				// Use a to_every() friend to do
				// the actual construction
  inline ElementWiseStride(const MatrixRow& mr);
  inline ElementWiseStride(const MatrixDiag& md);
  ElementWiseStride(const ElementWiseConst::ElementWiseConstRef& ewc)
  	: ElementWiseStrideConst(ewc) {}
public:

					// No public constructors...

                                // The return statement of the friends below
                                // would implicitly call a constructor
  friend inline ElementWiseStride to_every(const MatrixRow& mr);
  friend inline ElementWiseStride to_every(const MatrixDiag& md);
 
				// group-scalar arithmetics
				// Modify every element of the group
				// according to the operation
  void operator =   (const REAL val);	// Assignment to all the elems
  void operator +=  (const double val);	// Add to elements
  void operator -=  (const double val);	// Take from elements
  void operator *=  (const double val);	// Multiply elements by a val

				// Other element-wise matrix operations
  void abs(void);			// Take an absolute value of a coll
  void sqr(void);			// Square each element
  void sqrt(void);			// Take the square root

  void operator = (const ElementWiseStrideConst& another);
  void operator += (const ElementWiseStrideConst& another);
  void operator -= (const ElementWiseStrideConst& another);
  void operator *= (const ElementWiseStrideConst& another);
  void operator /= (const ElementWiseStrideConst& another);

  ElementWiseAction& apply(ElementWiseAction& functor);
};

inline ElementWiseConst::operator ElementWiseStrideConst (void)
	{ return ElementWiseConstRef(*this); }

inline ElementWise::operator ElementWiseStride (void)
	{ return ElementWiseConstRef(*this); }

inline ElementWiseStride ElementWise::with_stride (void)
	{ return ElementWiseConstRef(*this); }

inline Vector& Vector::operator = (const ElementWiseStrideConst& stream)
  	{  to_every(*this).with_stride() = stream; return *this; }


/*
 *------------------------------------------------------------------------
 *	MatrixDA: a view of a matrix that provides a direct access
 *	to its elements. A column index is built to speed up access
 */

				// A mixin class that builds a row index
				// for a matrix grid
class MatrixDABase : public DimSpec
{
  REAL * const * const index;		// index[i] = &matrix(0,i) (col index)
  MatrixDABase(const MatrixDABase&);	// Not implemented and forbidden:
  void operator = (const MatrixDABase&);// no cloning/assignment allowed
  
  static REAL * const * build_index(const Matrix& m);

protected:
  MatrixDABase(const Matrix& m)
        : DimSpec(m),
          index( build_index(m) )
          {}
  inline const REAL& operator () (const int rown, const int coln) const;
public:
  ~MatrixDABase(void);
};

class ConstMatrixDA : public Matrix::ConstReference, public MatrixDABase
{
  ConstMatrixDA(const ConstMatrixDA&);	// Not implemented and forbidden:
  void operator = (const ConstMatrixDA&);// no cloning/assignment allowed
public:
  ConstMatrixDA(const Matrix& m)
        : Matrix::ConstReference(m), MatrixDABase(m)
          {}
				// Individual element manipulations
  const REAL operator () (const int rown, const int coln) const
  			{ return MatrixDABase::operator() (rown,coln); }
  const REAL operator () (const rowcol rc) const
  		{ return operator () (rc.row,rc.col); }
  				// I wish the cast to const Matrix& worked,
  				// as given by the following operator. Alas,
  				// an explicit member function seems to
  				// be needed	
//  const Matrix& get_container(void) const
//  	{ return Matrix::ConstReference::operator const Matrix& (); }
};
 
class MatrixDA : public Matrix::Reference, public MatrixDABase
{
  MatrixDA(const MatrixDA&);		// Not implemented and forbidden:
  void operator = (const MatrixDA&);	// no cloning/assignment allowed

public:
  MatrixDA(Matrix& m)
        : Matrix::Reference(m), MatrixDABase(m)
          {}
				// Individual element manipulations
  REAL& operator () (const int rown, const int coln) const
  	{ return const_cast<REAL&>(MatrixDABase::operator()(rown,coln)); }
  REAL& operator () (const rowcol rc) const
  		{ return operator () (rc.row,rc.col); }
  	
  				// I wish the cast to Matrix& worked by itself,
  				// as given by the following operator. Alas,
  				// an explicit member function seems to
  				// be needed
#if defined (__GNUC__)
  Matrix& get_container(void) { return Matrix::Reference::operator Matrix& (); }
#else
  Matrix& get_container(void) { return *this; }
#endif
};
 
/*
 *------------------------------------------------------------------------
 *		MatrixRow, MatrixCol, MatrixDiag
 * They are special kind of views, of selected parts of the matrix
 * they are implemented as special kind of streams that walk only selected
 * parts of the matrix
 */

class ConstMatrixColumn : protected Matrix::ConstReference,
			  public DimSpec
{
  friend class ElementWiseConst;
  friend class LAStreamIn;
  friend class LAStreamOut;
  const REAL * const col_ptr;		// Pointer to the column under
  					// consideration
  ConstMatrixColumn(const ConstMatrixColumn&);	// Not implemented and forbidden:
  void operator = (const ConstMatrixColumn&);// no cloning/assignment allowed

protected:
				// Access the i-th element of the column
  inline const REAL& get_ref(const int index) const;

public:					// Take a col of the matrix
  ConstMatrixColumn(const Matrix& m, const int col);

  const REAL operator () (const int i) const
  			{ return get_ref(i); }
};
 
inline ElementWiseConst::ElementWiseConst(const ConstMatrixColumn& mc)
  	: start_ptr(const_cast<REAL*>(mc.col_ptr)),
  	  end_ptr(const_cast<REAL*>(mc.col_ptr)+mc.nrows)
  	{ }

				// Access the i-th element of the column
inline const REAL& ConstMatrixColumn::get_ref(const int index) const
{ register const int offset = index - row_lwb;
  if( offset >= nrows || offset < 0 )
      _error("MatrixColumn index %d is out of row boundaries [%d,%d]",
	      index,q_row_lwb(),q_row_upb());
  return col_ptr[offset];
}


inline ElementWiseConst of_every(const ConstMatrixColumn& mc)
		{ return mc; }

class MatrixColumn : public ConstMatrixColumn
{
  friend class ElementWise;
  friend class LAStreamOut;
  MatrixColumn(const MatrixColumn&);	// Not implemented and forbidden:
  void operator = (const MatrixColumn&);// no cloning/assignment allowed
public:					// Take a col of the matrix
  MatrixColumn(Matrix& m, const int col) :
  	ConstMatrixColumn(m,col) {}

  REAL& operator () (const int i)
  			{ return const_cast<REAL&>(get_ref(i)); }
};

inline ElementWise::ElementWise(const MatrixColumn& mc) : ElementWiseConst(mc) {}
inline ElementWise to_every(const MatrixColumn& mc) { return mc; }

			// A view of a single row of a matrix
class ConstMatrixRow : protected Matrix::ConstReference,
		       public DimSpec
{
  friend class ElementWiseStrideConst;
  friend class LAStrideStreamIn;
  friend class LAStrideStreamOut;
  const REAL * const row_ptr;		// Pointer to the row under
  					// consideration
  const int stride;			// if ptr=@a[row,i], then
					//    ptr+stride = @a[row,i+1]
					// Since elements of a[] are stored
					// col after col, stride = nrows
  const REAL * const end_ptr;		// Points after the end of the matrix
  
  ConstMatrixRow(const ConstMatrixRow&);	// Not implemented and forbidden:
  void operator = (const ConstMatrixRow&);	// no cloning/assignment allowed

protected:
				// Access the i-th element of the row
  inline const REAL& get_ref(const int index) const;

public:					// Take a row of the matrix
  ConstMatrixRow(const Matrix& m, const int row);

  const REAL operator () (const int i) const
  			{ return get_ref(i); }
};
 
inline
ElementWiseStrideConst::ElementWiseStrideConst(const ConstMatrixRow& mr)
  	: start_ptr(const_cast<REAL*>(mr.row_ptr)),
  	  end_ptr(const_cast<REAL*>(mr.end_ptr)),
  	  stride(mr.stride)
  	{ }

                                // The return statement of_every below
                                // would implicitly call a constructor
inline ElementWiseStrideConst of_every(const ConstMatrixRow& mr)
	{ return mr; }


				// Access the i-th element of the row
inline const REAL& ConstMatrixRow::get_ref(const int index) const
{ register const int offset = index - col_lwb;
  if( offset >= ncols || offset < 0 )
      _error("MatrixRow index %d is out of column boundaries [%d,%d]",
	      index,q_col_lwb(),q_col_upb());
  return row_ptr[stride*offset];
}


class MatrixRow : public ConstMatrixRow
{
  friend class ElementWiseStride;
  friend class LAStrideStreamOut;
  MatrixRow(const MatrixRow&);		// Not implemented and forbidden:
  void operator = (const MatrixRow&);	// no cloning/assignment allowed
public:					// Take a col of the matrix
  MatrixRow(Matrix& m, const int row) :
  	ConstMatrixRow(m,row) {}

  REAL& operator () (const int i)
  			{ return const_cast<REAL&>(get_ref(i)); }
};

inline ElementWiseStride::ElementWiseStride(const MatrixRow& mr)
	: ElementWiseStrideConst(mr) {}

inline ElementWiseStride to_every(const MatrixRow& mr) { return mr; }

				// A view of the matrix' main diagonal
				// ConstMatrixDiag is described as a Matrix
				// 1:n x 1:n, where n=min(nrows,ncols)
class ConstMatrixDiag : protected Matrix::ConstReference,
		        public DimSpec
{
  friend class ElementWiseStrideConst;
  friend class LAStrideStreamIn;
  friend class LAStrideStreamOut;
  const REAL * const start_ptr;		// Pointer to the upper left corner
  const int stride;			// if ptr=@a[i,i], then
					//    ptr+stride = @a[i+1,i+1]
					// Since elements of a[] are stored
					// col after col, stride = nrows+1
  const REAL * const end_ptr;		// Points after the end of the matrix
  
  ConstMatrixDiag(const ConstMatrixDiag&);	// Not implemented and forbidden:
  void operator = (const ConstMatrixDiag&);	// no cloning/assignment allowed

protected:
				// Access the i-th element of the diag
				// Note that numbering always starts with 1,
				// regardless of col_lwb and row_lwb of the
				// original matrix
  inline const REAL& get_ref(const int index) const;

public:
  explicit ConstMatrixDiag(const Matrix& m);

  const REAL operator () (const int i) const
  			{ return get_ref(i); }
};
 
inline
ElementWiseStrideConst::ElementWiseStrideConst(const ConstMatrixDiag& md)
  	: start_ptr(const_cast<REAL*>(md.start_ptr)),
  	  end_ptr(const_cast<REAL*>(md.end_ptr)),
  	  stride(md.stride)
  	{ }

                                // The return statement of of_every below
                                // would implicitly call a constructor
inline ElementWiseStrideConst of_every(const ConstMatrixDiag& md)
	{ return md; }


				// Access the i-th element of the diagonal
inline const REAL& ConstMatrixDiag::get_ref(const int index) const
{ if( index > ncols || index < 1 )
      _error("MatrixDiag index %d is out of diag boundaries [%d,%d]",
	      index,1,ncols);
  return start_ptr[stride*(index-1)];
}


class MatrixDiag : public ConstMatrixDiag
{
  friend class ElementWiseStride;
  friend class LAStrideStreamOut;
  MatrixDiag(const MatrixDiag&);	// Not implemented and forbidden:
  void operator = (const MatrixDiag&);	// no cloning/assignment allowed
public:					// Take a col of the matrix
  explicit MatrixDiag(Matrix& m) : ConstMatrixDiag(m) {}

  REAL& operator () (const int i)
  			{ return const_cast<REAL&>(get_ref(i)); }
};

inline ElementWiseStride::ElementWiseStride(const MatrixDiag& md)
	: ElementWiseStrideConst(md) {}

inline ElementWiseStride to_every(const MatrixDiag& md) { return md; }


/*
 *------------------------------------------------------------------------
 *			Inline Matrix operations
 */

inline Matrix::Matrix(const int no_rows, const int no_cols)
	: DimSpec(no_rows,no_cols)
{
  allocate();
}

inline Matrix::Matrix(const int row_lwb, const int row_upb,
		      const int col_lwb, const int col_upb)
	: DimSpec(row_lwb,row_upb,col_lwb,col_upb)
{
  allocate();
}

inline Matrix::Matrix(const DimSpec& dimension_specs)
	: DimSpec(dimension_specs)
{
  allocate();
}

inline Matrix::Matrix(const LazyMatrix& lazy_constructor)
	: DimSpec(lazy_constructor)
{
  allocate();
  lazy_constructor.fill_in(*this);
}


				// Force a promise of a matrix
				// That is, apply a lazy_constructor
				// to the current matrix
inline Matrix& Matrix::operator = (const LazyMatrix& lazy_constructor)
{
  is_valid();
  if( !(static_cast<const DimSpec&>(lazy_constructor) == *this) )
    info(),
    _error("The matrix above is incompatible with the assigned "
    	   "Lazy matrix");
      
  lazy_constructor.fill_in(*this);
  return *this;
}
					// Copy constructor, expensive: use
					// sparingly
inline Matrix::Matrix(const Matrix& another)
	: DimSpec(another)
{
  another.is_valid();
  allocate();
  *this = another;
}

				// Resize the matrix to new dimensions
inline void Matrix::resize_to(const int row_lwb, const int row_upb,
		 const int col_lwb, const int col_upb)
{
  resize_to(DimSpec(row_lwb,row_upb,col_lwb,col_upb));
}

inline const REAL& MatrixDABase::operator () (const int rown, const int coln) const
{
  register int arown = rown-row_lwb;		// Effective indices
  register int acoln = coln-col_lwb;

  if( arown >= nrows || arown < 0 )
    _error("Row index %d is out of Matrix boundaries [%d,%d]",
	   rown,row_lwb,nrows+row_lwb-1);
  if( acoln >= ncols || acoln < 0 )
    _error("Col index %d is out of Matrix boundaries [%d,%d]",
	   coln,col_lwb,ncols+col_lwb-1);
  
  return (index[acoln])[arown];
}

inline Matrix& Matrix::clear(void)	// Clean the Matrix
{
  is_valid();
  memset(elements,0,nelems*sizeof(REAL));
  return *this;
}

inline void are_compatible(const Matrix& m1, const Matrix& m2)
{
  m1.is_valid();
  m2.is_valid();
  
  if( !(static_cast<const DimSpec&>(m1) == m2) )
    m1.info(), m2.info(), _error("The matrices above are incompatible");
}

				// Assignment
inline Matrix& Matrix::operator = (const Matrix& source)
{
  are_compatible(*this,source);
  memcpy(elements,source.elements,nelems*sizeof(REAL));
  return *this;
}

				// Apply a user-defined action to each
				// element of a collection
inline ElementWiseConstAction&
ElementWiseConst::apply(ElementWiseConstAction& functor) const
{
  register const REAL * ep = start_ptr;
  while( ep < end_ptr )
    functor(*ep++);
  return functor;
}

inline ElementWiseAction&
ElementWise::apply(ElementWiseAction& functor)
{
  register REAL * ep = start_ptr;
  while( ep < end_ptr )
    functor(*ep++);
  return functor;
}

inline ElementWiseConstAction&
ElementWiseStrideConst::apply(ElementWiseConstAction& functor) const
{
  for(register const REAL * ep=start_ptr; ep < end_ptr; ep += stride )
    functor(*ep);
  return functor;
}

inline ElementWiseAction&
ElementWiseStride::apply(ElementWiseAction& functor)
{
  for(register REAL * ep=start_ptr; ep < end_ptr; ep += stride )
    functor(*ep);
  return functor;
}

			// Create a blank vector of a given size
			// (indexation starts at 1)
inline Vector::Vector(const int n)
	: Matrix(n,1) {}
  
			// Create a general lwb:upb vector blank vector
inline Vector::Vector(const int lwb, const int upb) : Matrix(lwb,upb,1,1) {}

			// Force the promise of a vector
inline Vector& Vector::operator = (const LazyMatrix& source)
  	{ Matrix::operator =(source); return *this; }

				// Resize the vector for a specified number
				// of elements, trying to keep intact as many
				// elements of the old vector as possible.
				// If the vector is expanded, the new elements
				// will be zeroes
inline void Vector::resize_to(const int n)	{ Vector::resize_to(1,n); }

inline void Vector::resize_to(const Vector& v)
{
  Vector::resize_to(v.q_lwb(),v.q_upb());
}

				// Get access to a vector element
inline REAL& Vector::operator () (const int ind)
{
  is_valid();
  register const int aind = ind - row_lwb;
  if( aind >= nelems || aind < 0 )
    _error("Requested element %d is out of Vector boundaries [%d,%d]",
	   ind,row_lwb,nrows+row_lwb-1);
  
  return elements[aind];
}

				// Make a vector following a recipe
inline Vector::Vector(const LazyMatrix& lazy_constructor)
  : Matrix(lazy_constructor)
{
  assure(ncols == 1,
         "cannot make a vector from a promise of a full matrix");
}

				// The following is just a syntactic sugar
				// to avoid typing of_every and to_every
inline Matrix& Matrix::operator = (const REAL val)
	{ to_every(*this) = val; return *this; }
	
inline Matrix& operator -= (Matrix& m, const double val)
	{ to_every(m) -= val; return m; }
inline Matrix& operator += (Matrix& m, const double val)
	{ to_every(m) += val; return m; }
inline Matrix& operator *= (Matrix& m, const double val)
	{ to_every(m) *= val; return m; }
	
inline Matrix::Reference& operator -= (Matrix::Reference& m, const double val)
	{ to_every(m) -= val; return m; }
inline Matrix::Reference& operator += (Matrix::Reference& m, const double val)
	{ to_every(m) += val; return m; }
inline Matrix::Reference& operator *= (Matrix::Reference& m, const double val)
	{ to_every(m) *= val; return m; }


inline bool operator ==  (const Matrix& m, const REAL val)
	{ return of_every(m) == val; }
inline bool operator !=  (const Matrix& m, const REAL val)
	{ return of_every(m) != val; }
inline bool operator ==  (const Matrix::ConstReference& m, const REAL val)
	{ return of_every(m) == val; }
inline bool operator !=  (const Matrix::ConstReference& m, const REAL val)
	{ return of_every(m) != val; }

#endif
