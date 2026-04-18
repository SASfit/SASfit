// This may look like C code, but it is really -*- C++ -*-
/*
 ************************************************************************
 *
 *		Verify Advanced Operations on Matrices
 *	    (multiplication, inverse, determinant evaluation)
 *
 * $Id: vmatrix2.cc,v 4.3 1998/12/20 22:58:06 oleg Exp oleg $
 *
 ************************************************************************
 */

#include "LAStreams.h"
#include <math.h>
#include "builtin.h"
#include <iostream.h>
#include <float.h>

/*
 *------------------------------------------------------------------------
 *			Verify the determinant evaluation
 */

static void test_determinant(const int msize)
{
  cout << "\n---> Verify determinant evaluation\n"
          "for a square matrix of size " << msize << "\n";

  Matrix m(msize,msize);

  cout << "\nCheck to see that the determinant of the unit matrix is one";
  m.unit_matrix();
  cout << "\n	determinant is " << m.determinant();
  assert( m.determinant() == 1 );

  const double pattern = 2.5;
  cout << "\nCheck the determinant for the matrix with " << pattern <<
          "\n	at the diagonal";
  m.clear();
  to_every(MatrixDiag(m)) = pattern;
  cout << "\n	determinant is " << m.determinant();
  assert( m.determinant() == pow(pattern,(long)m.q_nrows()) );

  cout << "\nCheck the determinant of the transposed matrix";
  m.unit_matrix();
  MatrixRow(m,1)(2) = pattern;
  Matrix m_tran = transposed(m);
  assert( !(m == m_tran) );
  assert( of_every(m).sum_abs(of_every(m_tran)) == 2*pattern );
  assert( m.determinant() == m_tran.determinant() );

  {
    cout << "\nswap two rows/cols of a matrix and watch det's sign";
    m.unit_matrix();
    to_every(MatrixRow(m,3)) = pattern;
    const double det1 = m.determinant();
    MatrixRow row1(m,1);
    Vector vrow1(m.q_row_lwb(),m.q_row_upb()); vrow1 = of_every(row1);
    to_every(row1) = of_every(ConstMatrixRow(m,3));
    to_every(MatrixRow(m,3)) = of_every(vrow1);	// swapped rows 1 and 3
    assert( m.determinant() == -det1 );
    MatrixColumn col2(m,2);
    Vector vcol2(m.q_row_lwb(),m.q_row_upb()); vcol2 = of_every(col2);
    to_every(col2) = of_every(ConstMatrixColumn(m,4));
    to_every(MatrixColumn(m,4)) = of_every(vcol2); // swapped columns 2 and 4
    assert( m.determinant() == det1 );
  }

  cout << "\nCheck the determinant for the matrix with " << pattern <<
          "\n	at the anti-diagonal";
  {
    m.clear();
    LAStrideStreamOut m_str(m,m.q_nrows()-1);
    m_str.get();	// Skip the first element
    for(register int i=0; i<m.q_nrows(); i++)
      m_str.get() = pattern;
    assert( m.determinant() == pow(pattern,(long)m.q_nrows()) * 
	  ( m.q_nrows()*(m.q_nrows()-1)/2 & 1 ? -1 : 1 ) );
  }

  cout << "\nCheck the determinant for the singular matrix"
          "\n	defined as above with zero first row";
  to_every(MatrixRow(m,m.q_row_lwb())) = 0;
  cout << "\n	determinant is " << m.determinant();
  assert( m.determinant() == 0 );

  cout << "\nCheck out the determinant of the Hilbert matrix";
  Matrix H(3,3);
  H.hilbert_matrix();
  cout << "\n    3x3 Hilbert matrix: exact determinant 1/2160 ";
  cout << "\n                              computed    1/"<< 1/H.determinant();

  H.resize_to(4,4);
  H.hilbert_matrix();
  cout << "\n    4x4 Hilbert matrix: exact determinant 1/6048000 ";
  cout << "\n                              computed    1/"<< 1/H.determinant();

  H.resize_to(5,5);
  H.hilbert_matrix();
  cout << "\n    5x5 Hilbert matrix: exact determinant 3.749295e-12";
  cout << "\n                              computed    "<< H.determinant();

  H.resize_to(7,7);
  H.hilbert_matrix();
  cout << "\n    7x7 Hilbert matrix: exact determinant 4.8358e-25";
  cout << "\n                              computed    "<< H.determinant();

  H.resize_to(9,9);
  H.hilbert_matrix();
  cout << "\n    9x9 Hilbert matrix: exact determinant 9.72023e-43";
  cout << "\n                              computed    "<< H.determinant();

  H.resize_to(10,10);
  H.hilbert_matrix();
  cout << "\n    10x10 Hilbert matrix: exact determinant 2.16418e-53";
  cout << "\n                              computed    "<< H.determinant();

  cout << "\nDone\n";
}

/*
 *------------------------------------------------------------------------
 *			Verify matrix multiplications
 */

static void test_mm_multiplications(const int msize)
{
  cout << "\n---> Verify matrix multiplications\n"
          "for matrices of the characteristic size " << msize << "\n\n";

  {
    cout << "Test inline multiplications of the UnitMatrix" << endl;
    Matrix m(-1,msize,-1,msize);
    Matrix u = unit(m);
    m.hilbert_matrix(); MatrixRow(m,3)(1) = M_PI;
    u *= m;
    verify_matrix_identity(u,m);
  }

  {
    cout << "Test inline multiplications by a DiagMatrix" << endl;
    Matrix m(msize+3,msize);
    m.hilbert_matrix(); MatrixRow(m,3)(1) = M_PI;
    Vector v(msize);
    for(register int i=v.q_lwb(); i<=v.q_upb(); i++)
      v(i) = 1+i;
    Matrix diag(msize,msize);
    to_every(MatrixDiag(diag)) = of_every(v);
    Matrix ethc = m;
    MatrixDA eth(ethc);
    for(register int i=eth.q_row_lwb(); i<=eth.q_row_upb(); i++)
      for(register int j=eth.q_col_lwb(); j<=eth.q_col_upb(); j++)
	eth(i,j) *= v(j);
    m *= diag;
    verify_matrix_identity(m,eth);
  }

  {
    cout << "Test XPP = X where P is a permutation matrix\n";
    Matrix m(msize-1,msize);
    m.hilbert_matrix(); MatrixRow(m,2)(3) = M_PI;
    Matrix eth = m;
    Matrix p(msize,msize);
    LAStrideStreamOut p_str(p,msize-1);
    p_str.get();	// Skip the first element
    for(register int i=0; i<msize; i++)
      p_str.get() = 1;
    assert( p_str.get_pos(p_str.tell()) == rowcol(p.q_row_upb(),p.q_col_upb()) );
    m *= p;
    m *= p;
    verify_matrix_identity(m,eth);
  }

  {
    cout << "Test general matrix multiplication through inline mult" << endl;
    Matrix m(msize-2,msize);
    m.hilbert_matrix(); { MatrixDiag md(m); md(3) = M_PI; }
    Matrix mt = transposed(m);
    Matrix p(msize,msize);
    p.hilbert_matrix();
    to_every(MatrixDiag(p)) += 1;
    Matrix mp = m * p;
    Matrix m1 = m;
    m *= p;
    verify_matrix_identity(m,mp);
#if 0
    Matrix mp1(mt,Matrix::TransposeMult,p);
    verify_matrix_identity(m,mp1);
#endif
    assert( !(m1 == m) );
    Matrix mp2 = zero(m1);
    assert( mp2 == 0 );
    mp2.mult(m1,p);
    verify_matrix_identity(m,mp2);
  }

  {
    cout << "Check to see UU' = U'U = E when U is the Haar matrix" << endl;
    const int order = 5;
    const int no_sub_cols = (1<<order) - 5;
    Matrix haar_sub = haar_matrix(5,no_sub_cols);
    Matrix haar_sub_t = transposed(haar_sub);
    Matrix hsths = haar_sub_t * haar_sub;
    Matrix hsths1 = zero(hsths); hsths1.mult(haar_sub_t,haar_sub);
    Matrix hsths_eth = unit(hsths);
    assert( hsths.q_nrows() == no_sub_cols && hsths.q_ncols() == no_sub_cols );
    verify_matrix_identity(hsths,hsths_eth);
    verify_matrix_identity(hsths1,hsths_eth);
    
    Matrix haar = haar_matrix(5);
    Matrix unitm = unit(haar);
    Matrix haar_t = transposed(haar);
//    Matrix hth(haar,Matrix::TransposeMult,haar);
    Matrix hht = haar * haar_t;
    Matrix hht1 = haar; hht1 *= haar_t;
    Matrix hht2 = zero(haar); hht2.mult(haar,haar_t);
//    verify_matrix_identity(unitm,hth);
    verify_matrix_identity(unitm,hht);
    verify_matrix_identity(unitm,hht1);
    verify_matrix_identity(unitm,hht2);
  }
  cout << "\nDone\n";
}


static void test_vm_multiplications(const int msize)
{
  cout << "\n---> Verify vector-matrix multiplications\n"
          "for matrices of the characteristic size " << msize << "\n\n";
  {
    cout << "Check shrinking a vector by multiplying by a non-sq unit matrix"
         << endl;
    Vector vb(-2,msize);
    for(register int i=vb.q_lwb(); i<=vb.q_upb(); i++)
      vb(i) = M_PI - i;
    assert( vb != 0 );
    Matrix mc(1,msize-2,-2,msize);	// contracting matrix
    mc.unit_matrix();
    Vector v1 = vb;
    Vector v2 = vb;
    v1 *= mc;
    v2.resize_to(1,msize-2);
    verify_matrix_identity(v1,v2);
  }

  {
    cout << "Check expanding a vector by multiplying by a non-sq unit matrix"
         << endl;
    Vector vb(msize);
    for(register int i=vb.q_lwb(); i<=vb.q_upb(); i++)
      vb(i) = M_PI + i;
    assert( vb != 0 );
    Matrix me(2,msize+5,1,msize);	// expanding matrix
    me.unit_matrix();
    Vector v1 = vb;
    Vector v2 = vb;
    v1 *= me;
    v2.resize_to(v1);
    verify_matrix_identity(v1,v2);
  }

  {
    cout << "Check general matrix-vector multiplication" << endl;
    Vector vb(msize);
    for(register int i=vb.q_lwb(); i<=vb.q_upb(); i++)
      vb(i) = M_PI + i;
    Matrix vm(msize,1);
    to_every(MatrixColumn(vm,1)) = of_every(vb);
    Matrix m(0,msize,1,msize);
    m.hilbert_matrix();
    vb *= m;
    assert( vb.q_lwb() == 0 );
    Matrix mvm = m * vm;
    verify_matrix_identity(vb,mvm);
  }

  cout << "\nDone\n";
}

static void test_inversion(const int msize)
{
  cout << "\n---> Verify matrix inversion for square matrices\n"
          "of size " << msize << "\n\n";
  {
    cout << "Test invesion of a diagonal matrix" << endl;
    Matrix m(-1,msize,-1,msize);
    Matrix mi = zero(m);
    //LAStrideStreamOut m_str = MatrixDiag(m); the same as below, but
    MatrixDiag md(m); LAStrideStreamOut m_str(md); // gcc 2.8.1 fails to compile above
    // LAStrideStreamOut mi_str = MatrixDiag(mi); the same as below, but
    MatrixDiag mid(mi); LAStrideStreamOut mi_str(mid); // gcc 2.8.1 fails to compile above
    for(register int i=1; !m_str.eof(); ++i)
      mi_str.get() = 1/(m_str.get() = 1);
    assert( mi_str.eof() );
    Matrix mi1 = inverse(m);
    m.invert();
    verify_matrix_identity(m,mi);
    verify_matrix_identity(mi1,mi);
  }

  {
    cout << "Test invesion of an orthonormal (Haar) matrix" << endl;
    Matrix m = haar_matrix(3);
    Matrix morig = m;
    Matrix mt = transposed(m);
    double det = -1;		// init to a wrong val to see if it's changed
    m.invert(det);
    assert( abs(det-1) <= FLT_EPSILON );
    verify_matrix_identity(m,mt);
    Matrix mti = inverse(mt);
    verify_matrix_identity(mti,morig);
  }

  {
    cout << "Test invesion of a good matrix with diagonal dominance" << endl;
    Matrix m(msize,msize);
    m.hilbert_matrix();
    to_every(MatrixDiag(m)) += 1;
    Matrix morig = m;
    double det_inv = 0;
    const double det_comp = m.determinant();
    m.invert(det_inv);
    cout << "\tcomputed determinant             " << det_comp << endl;
    cout << "\tdeterminant returned by invert() " << det_inv << endl;

    cout << "\tcheck to see M^(-1) * M is E" << endl;
    Matrix unitm = unit(m);
    verify_matrix_identity(m * morig,unitm);

    cout << "\tcheck to see M * M^(-1) is E" << endl;
    Matrix mmi = morig; mmi *= m;
    verify_matrix_identity(mmi,unit(m));
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
          "\n\t\tVerify Advanced Operations on Matrices\n";

  test_determinant(20);
  test_mm_multiplications(20);
  test_vm_multiplications(20);

  test_inversion(20);
  cout << "\nAll tests passed" << endl;
}

