// This may look like C code, but it is really -*- C++ -*-
/*
 ************************************************************************
 *
 *	Verify application of SVD to solving sets of simultaneous
 *				linear equations
 *
 * $Id: vslesing.cc,v 1.2 1998/12/14 01:36:10 oleg Exp oleg $
 *
 ************************************************************************
 */

#include "LAStreams.h"
#include "svd.h"
#include <iostream.h>

					// Solve Ax=b and test the result
static void test_sle(const Matrix& A, const Vector& b, const Vector& x_true)
{
  SVD svd(A);
  cout << "\ncondition number " << svd.q_cond_number() << endl;
  Vector x = SVD_inv_mult(svd,b);
  Matrix solutions(2,x_true.q_upb());
  to_every(MatrixRow(solutions,1)) = of_every(x_true);
  to_every(MatrixRow(solutions,2)) = of_every(x);
  solutions.print("true vs. computed solution");
  cout << "\tchecking to see that Ax is indeed b\n";
  verify_matrix_identity(A*x,b);
}

static void test1(const int neqs, const int nvars)
{
  cout << "\n\nChecking solution of a set of linear equations\n"
          "with a Hilb+E of order " << neqs << "x" << nvars << endl;
  Vector x(nvars);
  Matrix A(neqs,nvars);
  A.hilbert_matrix(); to_every(MatrixDiag(A)) += 1;
  struct fill : public ElementAction
  {
    void operation(REAL& element, const int i, const int j) { element = i; }
  };
  x.apply(fill());
  Vector b = A * x;
  test_sle(A,b,x);
}

static void test2(const int dim)
{
  cout << "\n\nChecking solution of a set of linear equations\n"
          "with a Hilbert matrix of order " << dim << endl;
  Vector x(dim);
  Matrix A(dim,dim);
  A.hilbert_matrix();
  struct fill : public ElementAction
  {
    void operation(REAL& element, const int i, const int j) { element = i; }
  };
  x.apply(fill());
  Vector b = A * x;
  test_sle(A,b,x);
}


main(void)
{
 cout << "\n\nVerify application of SVD to solving sets of simultaneous "
         "equations" << endl;
 test1(10,10);
 test1(20,10);
 test2(10);
}
