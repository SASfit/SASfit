// This may look like C code, but it is really -*- C++ -*-
/*
 ************************************************************************
 *
 *			  Numerical Math Package
 *
 *	Singular Value Decomposition of a rectangular matrix
 *			   A = U * Sig * V'
 *			and its applications
 *
 * In the decomposition above, matrices U and V are orthogonal; matrix
 * Sig is a diagonal matrix: its diagonal elements, which are all
 * non-negative, are singular values (numbers) of the original matrix A.
 * In another interpretation, the singular values are eigenvalues
 * of matrix A'A.
 *
 * $Id: svd.h,v 1.3 1998/12/19 03:14:30 oleg Exp oleg $
 *
 ************************************************************************
 */

#ifndef __GNUC__
#pragma once
#else
#pragma interface
#endif
#ifndef _svd_h
#define _svd_h 1


#include "LinAlg.h"

				// A class that holds U,V,Sig - the singular
				// value decomposition of a matrix
class SVD
{
  const int M,N;			// Dimensions of the problem (M>=N)
  Matrix U;				// M*M orthogonal matrix U
  Matrix V;				// N*N orthogonal matrix V
  Vector sig;				// Vector(1:N) of N onordered singular
  					// values
  
  					// Internal procedures used in SVD
 inline double left_householder(Matrix& A, const int i);
 inline double right_householder(Matrix& A, const int i);
 double bidiagonalize(Vector& super_diag, const Matrix& _A);

 inline void rotate(Matrix& U, const int i, const int j,
		    const double cos_ph, const double sin_ph);
 inline void rip_through(
	Vector& super_diag, const int k, const int l, const double eps);
 inline int get_submatrix_to_work_on(
	Vector& super_diag, const int k, const double eps);
 void diagonalize(Vector& super_diag, const double eps);
 
public:
  SVD(const Matrix& A);			// Decompose Matrix A, of M rows and
  					// N columns, M>=N
  
  					// Regularization: make all sig(i)
  					// that are smaller than min_sig
  					// exactly zeros
  void cut_off(const double min_sig);
  
  				// Inquiries
  const Matrix& q_U(void) const		{ return U; }
  const Matrix& q_V(void) const		{ return V; }
  const Vector& q_sig(void) const	{ return sig; }

  operator MinMax(void) const;		// Return min and max singular values
  double q_cond_number(void) const;	// sig_max/sig_min
  void info(void) const;		// Print the info about the SVD
};

/*
 *------------------------------------------------------------------------
 * An application of SVD: a regularized solution of a set of simultaneous
 * linear equations Ax=B
 * B can be either a vector (Mx1-matrix), or a full-blown matrix.
 * Note, if B=Unit(A), SVD_inv_mult class gives a (pseudo)inverse matrix;
 * btw, A doesn't even have to be a square matrix.
 * 
 * In the case of a rectangular MxN matrix A with M>N, the set
 * Ax=b is obviously overspecified. The solution x produced by a
 * SVD_inv_mult below is the least-norm solution, which is the solution
 * returned by a least-squares method.
 * 
 * tau is a regularization criterion: only singular values bigger than tau
 * would participate. If tau is not given, it's assumed to be
 * dim(sig)*max(sig)*FLT_EPSILON
 *
 * Use the SVD_inv_mult object as follows:
 * 	SVD svd(A);
 *	cout << "condition number of matrix A " << svd.q_cond_number();
 *	Vector x = SVD_inv_mult(svd,b);		// Solution of Ax=b
 *
 */

class SVD_inv_mult : public LazyMatrix
{
  const SVD& svd;
  const Matrix& B;
  double tau;			// smallness threshold for sig[i]
  bool are_zero_coeff;		// true if A had an incomplete rank
  void fill_in(Matrix& m) const;
public:	
  SVD_inv_mult(const SVD& _svd, const Matrix& _B,const double tau=0);
};

#endif
