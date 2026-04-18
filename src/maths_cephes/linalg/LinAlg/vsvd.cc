// This may look like C code, but it is really -*- C++ -*-
/*
 ************************************************************************
 *
 *	Verify Singular Vector Decomposition of a Rectangular Matrix
 *
 * $Id: vsvd.cc,v 1.3 1998/12/14 00:45:11 oleg Exp oleg $
 *
 ************************************************************************
 */

#include "LAStreams.h"
#include "svd.h"
#include <iostream.h>

				// SVD-decompose matrix A and test we can
				// compose it back
static void test_svd_expansion(const Matrix& A)
{
  cout << "\n\nSVD-decompose matrix A and check if we can compose it back\n"
       << endl;
  A.print("original matrix");
  SVD svd(A);
  svd.q_U().print("left factor U");
  svd.q_sig().print("Vector of Singular values");
  svd.q_V().print("right factor V");
  
  {
    cout << "\tchecking that U is orthogonal indeed, i.e., U'U=E and UU'=E"
         << endl;
    Matrix E = unit(svd.q_U());
    Matrix ut = transposed(svd.q_U());
    verify_matrix_identity(ut * svd.q_U(),E);
    verify_matrix_identity(svd.q_U() * ut,E);
  }
  
  {
    cout << "\tchecking that V is orthogonal indeed, i.e., V'V=E and VV'=E"
         << endl;
    Matrix E = unit(svd.q_V());
    Matrix vt = transposed(svd.q_V());
    verify_matrix_identity(vt * svd.q_V(),E);
    verify_matrix_identity(svd.q_V() * vt,E);
  }
  
  {
    cout << "\tchecking that U*Sig*V' is indeed A" << endl;
    Matrix S = zero(A);
    to_every(MatrixDiag(S)) = of_every(svd.q_sig());
    Matrix vt = transposed(svd.q_V());
    S *= vt;
    //Matrix Acomp = svd.q_U() * S;
    compare(A,svd.q_U() * S,"Original A and composed USigV'");
  }

  cout << "\nDone" << endl;
}

				// Make a matrix from an array
				// (read it row-by-row)
class MakeMatrix : public LazyMatrix
{
  const REAL * array;
  const int no_elems;
  void fill_in(Matrix& m) const;
public:
  MakeMatrix(const int nrows, const int ncols,
  	     const REAL * _array, const int _no_elems)
    : LazyMatrix(nrows,ncols), array(_array), no_elems(_no_elems) {}
  MakeMatrix(const int row_lwb, const int row_upb,
  	     const int col_lwb, const int col_upb,
  	     const REAL * _array, const int _no_elems)
    : LazyMatrix(row_lwb,row_upb,col_lwb,col_upb),
      array(_array), no_elems(_no_elems) {}
};

void MakeMatrix::fill_in(Matrix& m) const
{
  assert( m.q_nrows() * m.q_ncols() == no_elems );
  register const REAL * ap = array;
  for(register int i=m.q_row_lwb(); i<=m.q_row_upb(); i++)
    for(LAStrideStreamOut mstr(MatrixRow(m,i)); !mstr.eof(); )
       mstr.get() = *ap++;
}

static void test1(void)
{
  cout << "\nRotated by PI/2 Matrix Diag(1,4,9)\n" << endl;
  
  REAL array[] = {0,-4,0,  1,0,0,  0,0,9 };
  test_svd_expansion(MakeMatrix(3,3,array,sizeof(array)/sizeof(array[0])));
}

static void test2(void)
{
  cout << "\nExample from the Forsythe, Malcolm, Moler's book\n" << endl;
  
  REAL array[] = 
       { 1,6,11, 2,7,12, 3,8,13, 4,9,14, 5,10,15};
  test_svd_expansion(MakeMatrix(5,3,array,sizeof(array)/sizeof(array[0])));
}

static void test3(void)
{
  cout << "\nExample from the Wilkinson, Reinsch's book\n" <<
          "Singular numbers are 0, 19.5959, 20, 0, 35.3270\n" << endl;
  
  REAL array[] = 
      { 22, 10,  2,   3,  7,    14,  7, 10,  0,  8,
        -1, 13, -1, -11,  3,    -3, -2, 13, -2,  4,
         9,  8,  1,  -2,  4,     9,  1, -7,  5, -1,
         2, -6,  6,   5,  1,     4,  5,  0, -2,  2 };
  test_svd_expansion(MakeMatrix(8,5,array,sizeof(array)/sizeof(array[0])));
}

static void test4(void)
{
  cout << "\nExample from the Wilkinson, Reinsch's book\n" <<
          "Ordered singular numbers are Sig[21-k] = sqrt(k*(k-1))\n" << endl;
  Matrix A(21,20);
  struct fill_in : public ElementAction
  {
    void operation(REAL& element, const int i, const int j)
    	{ element = j>i ? 0 : i==j ? 21-j : -1; }
  };
  A.apply(fill_in());
  test_svd_expansion(A);
}

static void test5(void)
{
  cout << "\nTest by W. Meier <wmeier@manu.com> to catch an obscure "
       << "bug in QR\n" << endl;
  cout << "expect singular values to be\n" 
       << "1.4666e-024   1.828427   3.828427   4.366725  7.932951\n" << endl;

  REAL array[] = 
      {  1,  2,  0,  0,  0,
         0,  2,  3,  0,  0,
         0,  0,  0,  4,  0,
         0,  0,  0,  4,  5,
         0,  0,  0,  0,  5 };
  test_svd_expansion(MakeMatrix(5,5,array,sizeof(array)/sizeof(array[0])));
}

main(void)
{
 cout << "\n\nTesting Singular Value Decompositions of rectangular matrices"
      << endl;
 test1();
 test2();
 test3();
 test4();
 test5();
}
