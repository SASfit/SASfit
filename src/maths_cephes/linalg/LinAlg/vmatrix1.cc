// This may look like C code, but it is really -*- C++ -*-
/*
 ************************************************************************
 *
 *		Verify less trivial Operations on Matrices
 *	    (norms, forcing of promises, transpositions)
 *
 * $Id: vmatrix1.cc,v 4.2 1998/10/25 22:44:47 oleg Exp oleg $
 *
 ************************************************************************
 */

#include "LinAlg.h"
#include "builtin.h"
#include <math.h>
#include "iostream.h"
#include <float.h>

/*
 *------------------------------------------------------------------------
 *			Verify the norm calculation
 */

static void test_norms(const int rsize, const int csize)
{
  cout << "\n---> Verify norm calculations\n";

  const double pattern = 10.25;

  if( rsize % 2 == 1 || csize %2 == 1 )
    _error("Sorry, size of the matrix to test must be even for this test\n");

  Matrix m(rsize,csize);

  cout << "\nAssign " << pattern << " to all the elements and check norms\n";
  m = pattern;
  assert( abs(of_every(m).sum() - pattern*m.q_no_elems())
  	<= FLT_EPSILON );
  assert( abs(of_every(m).sum_abs() - pattern*m.q_no_elems())
  	<= FLT_EPSILON );
  assert( abs(of_every(m).sum_squares() - pattern*pattern*m.q_no_elems())
  	<= FLT_EPSILON );
  assert( of_every(m).max_abs() == pattern );
  cout << "  1. (col) norm should be pattern*nrows\n";
  assert( m.norm_1() == pattern*m.q_nrows() );
  assert( m.norm_1() == m.col_norm() );
  cout << "  Inf (row) norm should be pattern*ncols\n";
  assert( m.norm_inf() == pattern*m.q_ncols() );
  assert( m.norm_inf() == m.row_norm() );
  cout << "  Square of the Eucl norm has got to be pattern^2 * no_elems\n";
  assert( m.e2_norm() == sqr(pattern)*m.q_no_elems() );
  assert( m.e2_norm() == of_every(m).sum_squares() );
  Matrix m1 = zero(m);
  assert( m.e2_norm() == e2_norm(m,m1) );
  
  assert( of_every(m).sum_squares(of_every(m1)) == m.e2_norm() );
  assert( of_every(m1).sum_squares(of_every(m)) == m.e2_norm() );
  assert( of_every(m).sum_squares(of_every(m)) == 0 );
  assert( of_every(m).sum_abs(of_every(m1)) == of_every(m).sum_abs() );
  assert( of_every(m1).sum_abs(of_every(m)) == of_every(m).sum_abs() );
  assert( of_every(m).sum_abs(of_every(m)) == 0 );
  assert( of_every(m).max_abs(of_every(m1)) == pattern );
  assert( of_every(m1).max_abs(of_every(m)) == pattern );
  assert( of_every(m).max_abs(of_every(m)) == 0 );
  cout << "\nDone\n";
}

/*
 *------------------------------------------------------------------------
 *			Test special matrix creation
 */

static void test_special_creation(const int dim)
{
  cout << "\n---> Check creating some special matrices of dimension " <<
    dim << "\n\n";

  {
    cout << "test creating Hilbert matrices" << endl;
    Matrix m(dim+1,dim);
    Matrix m1 = zero(m);
    m.hilbert_matrix();
    assert( !(m == m1) );
    assert( m != 0 );
    class MakeHilbert : public ElementAction
    {
      void operation(REAL& element, const int i, const int j)
      		{ element = 1./(i+j-1); }
      public: MakeHilbert(void) {}
    };
    m1.apply(MakeHilbert());
    assert( m1 != 0 );
    assert( m == m1 );
  }

  {
    cout << "test creating zero matrix and copy constructor" << endl;
    Matrix m(dim,dim+1);
    m.hilbert_matrix();
    assert( m != 0 );
    Matrix m1(m);			// Applying the copy constructor
    assert( m1 == m );
    Matrix m2 = zero(m);
    assert( m2 == 0 );
    assert( m != 0 );
  }

  {
    cout << "test creating unit matrices" << endl;
    class TestUnit : public ConstElementAction
    {
      int is_unit;
      void operation(const REAL element, const int i, const int j)
      { if( is_unit ) is_unit = i==j ? element == 1.0 : element == 0; }
      public: TestUnit(void) : is_unit(0==0) {}
      int is_indeed_unit(void) const 		{ return is_unit; }
    };
    Matrix m(dim,dim);
    {
      TestUnit test_unit;
      m.apply(test_unit);
      assert( !test_unit.is_indeed_unit() );
    }
    m.unit_matrix();
    {
      TestUnit test_unit;
      m.apply(test_unit);
      assert( test_unit.is_indeed_unit() );
    }
    m.resize_to(dim-1,dim);
    Matrix m2 = unit(m);
    {
      TestUnit test_unit;
      m2.apply(test_unit);
      assert( test_unit.is_indeed_unit() );
    }
    m.resize_to(dim,dim-2);
    m.unit_matrix();
    {
      TestUnit test_unit;
      m.apply(test_unit);
      assert( test_unit.is_indeed_unit() );
    }
  }

  {
    cout << "check to see that Haar matrix has *exactly* orthogonal columns"
					<< endl;
    const int order = 5;
    Matrix haar = haar_matrix(order);
    assert( haar.q_nrows() == (1<<order) && haar.q_nrows() == haar.q_ncols() );
    Vector colj(1<<order), coll(1<<order);
    for(register int j=haar.q_col_lwb(); j<=haar.q_col_upb(); j++)
    {
      colj = of_every(ConstMatrixColumn(haar,j));
      assert( abs(abs(colj*colj - 1.0)) <= FLT_EPSILON );
      for(register int l=j+1; l<=haar.q_col_upb(); l++)
      {
	    coll = of_every(ConstMatrixColumn(haar,l));
	    assert( colj*coll == 0 );
      }
    }
    cout << "make Haar (sub)matrix and test it *is* a submatrix" << endl;
    const int no_sub_cols = (1<<order) - 3;
    Matrix haar_sub = haar_matrix(order,no_sub_cols);
    assert( haar_sub.q_nrows() == (1<<order) && 
	    haar_sub.q_ncols() == no_sub_cols );
    for(register int j=haar_sub.q_col_lwb(); j<=haar_sub.q_col_upb(); j++)
    {
      colj = of_every(ConstMatrixColumn(haar,j));
      coll = of_every(ConstMatrixColumn(haar_sub,j));
      verify_matrix_identity(colj,coll);
    }
  }

  cout << "\nDone\n";
}


static void test_matrix_promises(const int dim)
{
  cout << "\n---> Check making/forcing promises, (lazy)matrices of dimension " <<
    dim << "\n\n";

  class hilbert_matrix_promise : public LazyMatrix
  {
    void fill_in(Matrix& m) const { m.hilbert_matrix(); }
  public:
    hilbert_matrix_promise(const int nrows, const int ncols)
	: LazyMatrix(nrows,ncols) {}
    hilbert_matrix_promise(const int _row_lwb, const int _row_upb,
	     const int _col_lwb, const int _col_upb)
	: LazyMatrix(_row_lwb,_row_upb,_col_lwb,_col_upb) {}
  };
  
  {
    cout << "make a promise and force it by a constructor" << endl;
    Matrix m = hilbert_matrix_promise(dim,dim+1);
    Matrix m1 = zero(m);
    assert( !(m1 == m) && m1 == 0 );
    m1.hilbert_matrix();
    verify_matrix_identity(m,m1);
  }
  
  {
    cout << "make a promise and force it by an assignment" << endl;
    Matrix m(-1,dim,0,dim);
    Matrix m1 = zero(m);
    m = hilbert_matrix_promise(-1,dim,0,dim);
    assert( !(m1 == m) && m1 == 0 );
    m1.hilbert_matrix();
    verify_matrix_identity(m,m1);
  }

  cout << "\nDone\n";
}

/*
 *------------------------------------------------------------------------
 *		       Verify matrix transposition
 */

static void test_transposition(const int msize)
{
  cout << "\n---> Verify matrix transpose\n"
          "for matrices of a characteristic size " << msize << endl;

  {
    cout << "\nCheck to see that a square UnitMatrix' stays the same";
    Matrix m(msize,msize);
    m.unit_matrix();
    assert( m == transposed(m) );
  }
  
  {
    cout << "\nTest a non-square UnitMatrix";
    Matrix m(msize,msize+1);
    m.unit_matrix();
    Matrix mt = transposed(m);
    assert( m.q_nrows() == mt.q_ncols() && m.q_ncols() == mt.q_nrows() );
    register int i,j;
    m.is_valid();
    ConstMatrixDA ma(m), mta(mt);
    for(i=ma.q_row_lwb(); i<=min(ma.q_row_upb(),ma.q_col_upb()); i++)
      for(j=ma.q_col_lwb(); j<=min(ma.q_row_upb(),ma.q_col_upb()); j++)
	assert( ma(i,j) == mta(i,j) );
  }

  {
    cout << "\nCheck to see that a symmetric (Hilbert)Matrix' stays the same";
    Matrix m(msize,msize);
    m.hilbert_matrix();
    Matrix mt =transposed(m);
    assert( m == mt );
  }

  {
    cout << "\nCheck transposing a non-symmetric matrix";
    Matrix m(msize+1,msize);
    m.hilbert_matrix();
    ((MatrixDA)m)(1,2) = M_PI;
    Matrix mt = transposed(m);
    assert( m.q_nrows() == mt.q_ncols() && m.q_ncols() == mt.q_nrows() );
    ConstMatrixDA mta(mt);
    assert( mta(2,1) == (REAL)M_PI && mta(1,2) != (REAL)M_PI );

    cout << "\nCheck double transposing a non-symmetric matrix";
    assert( m == transposed(mt) );
  }
  
  cout << "\nDone\n";
}



/*
 *------------------------------------------------------------------------
 *				Root module
 */

main()
{
  cout << "\n\n" << _Minuses << 
          "\n\t\tVerify less trivial Operations on Matrices\n";

  test_norms(40,20);
  test_special_creation(20);
  test_matrix_promises(20);
  test_transposition(20);

  cout << "\n\nAll test passed" << endl;
}

