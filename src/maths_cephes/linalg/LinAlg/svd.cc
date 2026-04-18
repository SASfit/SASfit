// This may look like C code, but it is really -*- C++ -*-
/*
 ************************************************************************
 *
 *			  Numerical Math Package
 *	Singular Value Decomposition of a rectangular matrix
 *			     A = U * Sig * V'
 *
 * where matrices U and V are orthogonal and Sig is a digonal matrix.
 *
 * The singular value decomposition is performed by constructing an SVD
 * object from an M*N matrix A with M>=N (that is, at least as many rows
 * as columns). Note, in case M > N, matrix Sig has to be a M*N diagonal
 * matrix. However, it has only N diag elements, which we store in a 1:N
 * Vector sig.
 *
 * Algorithm
 *	Bidiagonalization with Householder reflections followed by a
 * modification of a QR-algorithm. For more details, see
 *   G.E. Forsythe, M.A. Malcolm, C.B. Moler
 *   Computer methods for mathematical computations. - Prentice-Hall, 1977
 * However, in the persent implementation, matrices U and V are computed
 * right away rather than delayed until after all Householder reflections.
 *
 * This code is based for the most part on a Algol68 code I wrote
 * ca. 1987
 *
 * $Id: svd.cc,v 1.4 1998/12/19 03:14:21 oleg Exp oleg $
 *
 ************************************************************************
 */

#if defined(__GNUC__)
#pragma implementation
#endif

#include <math.h>
#include "svd.h"
#include <float.h>
#include "LAStreams.h"

/*
 *------------------------------------------------------------------------
 *				Bidiagonalization
 */

 /*
 *			Left Householder Transformations
 *
 * Zero out an entire subdiagonal of the i-th column of A and compute the
 * modified A[i,i] by multiplication (UP' * A) with a matrix UP'
 *   (1)  UP' = E - UPi * UPi' / beta
 *
 * where a column-vector UPi is as follows
 *   (2)  UPi = [ (i-1) zeros, A[i,i] + Norm, vector A[i+1:M,i] ]
 * where beta = UPi' * A[,i] and Norm is the norm of a vector A[i:M,i]
 * (sub-diag part of the i-th column of A). Note we assign the Norm the
 * same sign as that of A[i,i].
 * By construction, (1) does not affect the first (i-1) rows of A. Since
 * A[*,1:i-1] is bidiagonal (the result of the i-1 previous steps of
 * the bidiag algorithm), transform (1) doesn't affect these i-1 columns
 * either as one can easily verify.
 * The i-th column of A is transformed as
 *   (3)  UP' * A[*,i] = A[*,i] - UPi
 * (since UPi'*A[*,i]/beta = 1 by construction of UPi and beta)
 * This means effectively zeroing out A[i+1:M,i] (the entire subdiagonal
 * of the i-th column of A) and replacing A[i,i] with the -Norm. Thus
 * modified A[i,i] is returned by the present function.
 * The other (i+1:N) columns of A are transformed as
 *    (4)  UP' * A[,j] = A[,j] - UPi * ( UPi' * A[,j] / beta )
 * Note, due to (2), only elements of rows i+1:M actually  participate
 * in above transforms; the upper i-1 rows of A are not affected.
 * As was mentioned earlier,
 * (5)  beta = UPi' * A[,i] = (A[i,i] + Norm)*A[i,i] + A[i+1:M,i]*A[i+1:M,i]
 *	= ||A[i:M,i]||^2 + Norm*A[i,i] = Norm^2 + Norm*A[i,i]
 * (note the sign of the Norm is the same as A[i,i])
 * For extra precision, vector UPi (and so is Norm and beta) are scaled,
 * which would not affect (4) as easy to see.
 *
 * To satisfy the definition
 *   (6)  .SIG = U' A V
 * the result of consecutive transformations (1) over matrix A is accumulated
 * in matrix U' (which is initialized to be a unit matrix). At each step,
 * U' is left-multiplied by UP' = UP (UP is symmetric by construction,
 * see (1)). That is, U is right-multiplied by UP, that is, rows of U are
 * transformed similarly to columns of A, see eq. (4). We also keep in mind
 * that multiplication by UP at the i-th step does not affect the first i-1
 * columns of U.
 * Note that the vector UPi doesn't have to be allocated explicitly: its
 * first i-1 components are zeros (which we can always imply in computations),
 * and the rest of the components (but the UPi[i]) are the same as those
 * of A[i:M,i], the subdiagonal of A[,i]. This column, A[,i] is affected only
 * trivially as explained above, that is, we don't need to carry this
 * transformation explicitly (only A[i,i] is going to be non-trivially
 * affected, that is, replaced by -Norm, but we will use sig[i] to store
 * the result).
 *
 */
 
 inline double SVD::left_householder(Matrix& A, const int i)
 {					// Note that only UPi[i:M] matter
   IRange range = IRange::from(i - A.q_col_lwb());
   LAStreamOut UPi_str(MatrixColumn(A,i),range);
   register int j;
   REAL scale = 0;			// Compute the scaling factor
   while( !UPi_str.eof() )
     scale += abs(UPi_str.get());
   if( scale == 0 )			// If A[,i] is a null vector, no
     return 0;				// transform is required

   UPi_str.rewind();
   double Norm_sqr = 0;			// Scale UPi (that is, A[,i])
   while( !UPi_str.eof() )		// and compute its norm, Norm^2
     Norm_sqr += sqr(UPi_str.get() /= scale);
   UPi_str.rewind();
   double new_Aii = sqrt(Norm_sqr);	// new_Aii = -Norm, Norm has the
   if( UPi_str.peek() > 0 )		// same sign as Aii (that is, UPi[i])
     new_Aii = -new_Aii;
   const float beta = - UPi_str.peek()*new_Aii + Norm_sqr;
   UPi_str.peek() -= new_Aii;		// UPi[i] = A[i,i] - (-Norm)
   
   for(j=i+1; j<=N; j++)	// Transform i+1:N columns of A
   {
     LAStreamOut Aj_str(MatrixColumn(A,j),range);
     REAL factor = 0;
     while( !UPi_str.eof() )
       factor += UPi_str.get() * Aj_str.get();	// Compute UPi' * A[,j]
     factor /= beta;
     for(UPi_str.rewind(), Aj_str.rewind(); !UPi_str.eof(); )
       Aj_str.get() -= UPi_str.get() * factor;
     UPi_str.rewind();
   }

   for(j=1; j<=M; j++)	// Accumulate the transform in U
   {
     LAStrideStreamOut Uj_str(MatrixRow(U,j),range);
     REAL factor = 0;
     while( !UPi_str.eof() )
       factor += UPi_str.get() * Uj_str.get();	// Compute  U[j,] * UPi
     factor /= beta;
     for(UPi_str.rewind(),Uj_str.rewind(); !UPi_str.eof(); )
        Uj_str.get() -= UPi_str.get() * factor;
     UPi_str.rewind();
   }
   return new_Aii * scale;		// Scale new Aii back (our new Sig[i])
 }
 
/*
 *			Right Householder Transformations
 *
 * Zero out i+2:N elements of a row A[i,] of matrix A by right
 * multiplication (A * VP) with a matrix VP
 *   (1)  VP = E - VPi * VPi' / beta
 *
 * where a vector-column .VPi is as follows
 *   (2)  VPi = [ i zeros, A[i,i+1] + Norm, vector A[i,i+2:N] ]
 * where beta = A[i,] * VPi and Norm is the norm of a vector A[i,i+1:N]
 * (right-diag part of the i-th row of A). Note we assign the Norm the
 * same sign as that of A[i,i+1].
 * By construction, (1) does not affect the first i columns of A. Since
 * A[1:i-1,] is bidiagonal (the result of the previous steps of
 * the bidiag algorithm), transform (1) doesn't affect these i-1 rows
 * either as one can easily verify.
 * The i-th row of A is transformed as
 *  (3)  A[i,*] * VP = A[i,*] - VPi'
 * (since A[i,*]*VPi/beta = 1 by construction of VPi and beta)
 * This means effectively zeroing out A[i,i+2:N] (the entire right super-
 * diagonal of the i-th row of A, but ONE superdiag element) and replacing
 * A[i,i+1] with - Norm. Thus modified A[i,i+1] is returned as the result of
 * the present function.
 * The other (i+1:M) rows of A are transformed as
 *    (4)  A[j,] * VP = A[j,] - VPi' * ( A[j,] * VPi / beta )
 * Note, due to (2), only elements of columns i+1:N actually  participate
 * in above transforms; the left i columns of A are not affected.
 * As was mentioned earlier,
 * (5)  beta = A[i,] * VPi = (A[i,i+1] + Norm)*A[i,i+1]
 *			   + A[i,i+2:N]*A[i,i+2:N]
 *	= ||A[i,i+1:N]||^2 + Norm*A[i,i+1] = Norm^2 + Norm*A[i,i+1]
 * (note the sign of the Norm is the same as A[i,i+1])
 * For extra precision, vector VPi (and so is Norm and beta) are scaled,
 * which would not affect (4) as easy to see.
 *
 * The result of consecutive transformations (1) over matrix A is accumulated
 * in matrix V (which is initialized to be a unit matrix). At each step,
 * V is right-multiplied by VP. That is, rows of V are transformed similarly
 * to rows of A, see eq. (4). We also keep in mind that multiplication by
 * VP at the i-th step does not affect the first i rows of V.
 * Note that vector VPi doesn't have to be allocated explicitly: its
 * first i components are zeros (which we can always imply in computations),
 * and the rest of the components (but the VPi[i+1]) are the same as those
 * of A[i,i+1:N], the superdiagonal of A[i,]. This row, A[i,] is affected
 * only trivially as explained above, that is, we don't need to carry this
 * transformation explicitly (only A[i,i+1] is going to be non-trivially
 * affected, that is, replaced by -Norm, but we will use super_diag[i+1] to
 * store the result).
 *
 */
 
inline double SVD::right_householder(Matrix& A, const int i)
{
   IRange range = IRange::from(i+1 - A.q_row_lwb());// Note only VPi[i+1:N] matter
   LAStrideStreamOut VPi_str(MatrixRow(A,i),range);
   register int j;
   REAL scale = 0;			// Compute the scaling factor
   while( !VPi_str.eof() )
     scale += abs(VPi_str.get());
   if( scale == 0 )			// If A[i,] is a null vector, no
     return 0;				// transform is required
 
   VPi_str.rewind();
   double Norm_sqr = 0;			// Scale VPi (that is, A[i,])
   while( !VPi_str.eof() )		// and compute its norm, Norm^2
     Norm_sqr += sqr(VPi_str.get() /= scale);
   VPi_str.rewind();
   double new_Aii1 = sqrt(Norm_sqr);	// new_Aii1 = -Norm, Norm has the
   if( VPi_str.peek() > 0 )		// same sign as
     new_Aii1 = -new_Aii1; 		// Aii1 (that is, VPi[i+1])
   const float beta = - VPi_str.peek()*new_Aii1 + Norm_sqr;
   VPi_str.peek() -= new_Aii1;		// VPi[i+1] = A[i,i+1] - (-Norm)
   
   for(j=i+1; j<=M; j++)	// Transform i+1:M rows of A
   {
     LAStrideStreamOut Aj_str( MatrixRow(A,j),range );
     REAL factor = 0;
     while( !VPi_str.eof() )
       factor += VPi_str.get() * Aj_str.get();	// Compute A[j,] * VPi
     factor /= beta;
     for(VPi_str.rewind(), Aj_str.rewind(); !VPi_str.eof(); )
       Aj_str.get() -= VPi_str.get() * factor;
     VPi_str.rewind();
   }

   for(j=1; j<=N; j++)	// Accumulate the transform in V
   {
     LAStrideStreamOut Vj_str( MatrixRow(V,j), range );
     REAL factor = 0;
     while( !VPi_str.eof() )
       factor += VPi_str.get() * Vj_str.get();	// Compute  V[j,] * VPi
     factor /= beta;
     for(VPi_str.rewind(), Vj_str.rewind(); !VPi_str.eof(); )
       Vj_str.get() -= VPi_str.get() * factor;
     VPi_str.rewind();
   }
   return new_Aii1 * scale;		// Scale new Aii1 back
}

/*
 *------------------------------------------------------------------------
 *			  Bidiagonalization
 * This nethod turns matrix A into a bidiagonal one. Its N diagonal elements
 * are stored in a vector sig, while N-1 superdiagonal elements are stored
 * in a vector super_diag(2:N) (with super_diag(1) being always 0).
 * Matrices U and V store the record of orthogonal Householder
 * reflections that were used to convert A to this form. The method
 * returns the norm of the resulting bidiagonal matrix, that is, the
 * maximal column sum.
 */

double SVD::bidiagonalize(Vector& super_diag, const Matrix& _A)
{
  double norm_acc = 0;
  super_diag(1) = 0;			// No superdiag elem above A(1,1)
  Matrix A = _A;			// A being transformed
  A.resize_to(_A.q_nrows(),_A.q_ncols()); // Indexing from 1
  for(register int i=1; i<=N; i++)
  {
    const REAL& diagi = sig(i) = left_householder(A,i);
    if( i < N )
      super_diag(i+1) = right_householder(A,i);
    norm_acc = max(norm_acc,(double)abs(diagi)+abs(super_diag(i)));
  }
  return norm_acc;
}

/*
 *------------------------------------------------------------------------
 *		QR-diagonalization of a bidiagonal matrix
 *
 * After bidiagonalization we get a bidiagonal matrix J:
 *    (1)  J = U' * A * V
 * The present method turns J into a matrix JJ by applying a set of
 * orthogonal transforms
 *    (2)  JJ = S' * J * T
 * Orthogonal matrices S and T are chosen so that JJ were also a
 * bidiagonal matrix, but with superdiag elements smaller than those of J.
 * We repeat (2) until non-diag elements of JJ become smaller than EPS
 * and can be disregarded.
 * Matrices S and T are constructed as
 *    (3)  S = S1 * S2 * S3 ... Sn, and similarly T
 * where Sk and Tk are matrices of simple rotations
 *    (4)  Sk[i,j] = i==j ? 1 : 0 for all i>k or i<k-1
 *         Sk[k-1,k-1] = cos(Phk),  Sk[k-1,k] = -sin(Phk),
 *         SK[k,k-1] = sin(Phk),    Sk[k,k] = cos(Phk), k=2..N
 * Matrix Tk is constructed similarly, only with angle Thk rather than Phk.
 *
 * Thus left multiplication of J by SK' can be spelled out as
 *    (5)  (Sk' * J)[i,j] = J[i,j] when i>k or i<k-1,
 *                  [k-1,j] = cos(Phk)*J[k-1,j] + sin(Phk)*J[k,j]
 *                  [k,j] =  -sin(Phk)*J[k-1,j] + cos(Phk)*J[k,j]
 * That is, k-1 and k rows of J are replaced by their linear combinations;
 * the rest of J is unaffected. Right multiplication of J by Tk similarly
 * changes only k-1 and k columns of J.
 * Matrix T2 is chosen the way that T2'J'JT2 were a QR-transform with a
 * shift. Note that multiplying J by T2 gives rise to a J[2,1] element of
 * the product J (which is below the main diagonal). Angle Ph2 is then
 * chosen so that multiplication by S2' (which combines 1 and 2 rows of J)
 * gets rid of that elemnent. But this will create a [1,3] non-zero element.
 * T3 is made to make it disappear, but this leads to [3,2], etc.
 * In the end, Sn removes a [N,N-1] element of J and matrix S'JT becomes
 * bidiagonal again. However, because of a special choice
 * of T2 (QR-algorithm), its non-diag elements are smaller than those of J.
 *
 * All this process in more detail is described in
 *    J.H. Wilkinson, C. Reinsch. Linear algebra - Springer-Verlag, 1971
 *
 * If during transforms (1), JJ[N-1,N] turns 0, then JJ[N,N] is a singular
 * number (possibly with a wrong (that is, negative) sign). This is a
 * consequence of Frantsis' Theorem, see the book above. In that case, we can
 * eliminate the N-th row and column of JJ and carry out further transforms
 * with a smaller matrix. If any other superdiag element of JJ turns 0,
 * then JJ effectively falls into two independent matrices. We will process
 * them independently (the bottom one first).
 *
 * Since matrix J is a bidiagonal, it can be stored efficiently. As a matter
 * of fact, its N diagonal elements are in array Sig, and superdiag elements
 * are stored in array super_diag.
 */
 
				// Carry out U * S with a rotation matrix
				// S (which combines i-th and j-th columns
				// of U, i>j)
inline void SVD::rotate(Matrix& U, const int i, const int j,
		   const double cos_ph, const double sin_ph)
{
  LAStreamOut Ui(MatrixColumn (U,i));
  LAStreamOut Uj(MatrixColumn (U,j));
  while( !Ui.eof() )
  {
    REAL& Uil = Ui.get(); REAL& Ujl = Uj.get();
    const REAL Ujl_was = Ujl;
    Ujl =  cos_ph*Ujl_was + sin_ph*Uil;
    Uil = -sin_ph*Ujl_was + cos_ph*Uil;
  }
}

/*
 * A diagonal element J[l-1,l-1] turns out 0 at the k-th step of the
 * algorithm. That means that one of the original matrix' singular numbers
 * shall be zero. In that case, we multiply J by specially selected
 * matrices S' of simple rotations to eliminate a superdiag element J[l-1,l].
 * After that, matrix J falls into two pieces, which can be dealt with
 * in a regular way (the bottom piece first).
 * 
 * These special S transformations are accumulated into matrix U: since J
 * is left-multiplied by S', U would be right-multiplied by S. Transform
 * formulas for doing these rotations are similar to (5) above. See the
 * book cited above for more details.
 */
inline void SVD::rip_through(
	Vector& super_diag, const int k, const int l, const double eps)
{
  double cos_ph = 0, sin_ph = 1;	// Accumulate cos,sin of Ph
  				// The first step of the loop below
  				// (when i==l) would eliminate J[l-1,l],
  				// which is stored in super_diag(l)
  				// However, it gives rise to J[l-1,l+1]
  				// and J[l,l+2]
  				// The following steps eliminate these
  				// until they fall below
  				// significance
  for(register int i=l; i<=k; i++)
  {
    const double f = sin_ph * super_diag(i);
    super_diag(i) *= cos_ph;
    if( abs(f) <= eps )
      break;			// Current J[l-1,l] became unsignificant
    cos_ph = sig(i); sin_ph = -f;	// unnormalized sin/cos
    const double norm = (sig(i) = hypot(cos_ph,sin_ph)); // sqrt(sin^2+cos^2)
    cos_ph /= norm, sin_ph /= norm;	// Normalize sin/cos
    rotate(U,i,l-1,cos_ph,sin_ph);
  }
}

			// We're about to proceed doing QR-transforms
			// on a (bidiag) matrix J[1:k,1:k]. It may happen
			// though that the matrix splits (or can be
			// split) into two independent pieces. This function
			// checks for splitting and returns the lowerbound
			// index l of the bottom piece, J[l:k,l:k]
inline int SVD::get_submatrix_to_work_on(
	Vector& super_diag, const int k, const double eps)
{
  for(register int l=k; l>1; l--)
    if( abs(super_diag(l)) <= eps )
      return l;				// The breaking point: zero J[l-1,l]
    else if( abs(sig(l-1)) <= eps )	// Diagonal J[l,l] turns out 0
    {					// meaning J[l-1,l] _can_ be made
      rip_through(super_diag,k,l,eps);	// zero after some rotations
      return l;
    }
  return 1;			// Deal with J[1:k,1:k] as a whole
}

		// Diagonalize root module
void SVD::diagonalize(Vector& super_diag, const double eps)
{
  for(register int k=N; k>=1; k--)	// QR-iterate upon J[l:k,l:k]
  {
    register int l;
    while(l=get_submatrix_to_work_on(super_diag,k,eps),
    	  abs(super_diag(k)) > eps)	// until superdiag J[k-1,k] becomes 0
    {
      double shift;			// Compute a QR-shift from a bottom
      {					// corner minor of J[l:k,l:k] order 2
      	REAL Jk2k1 = super_diag(k-1),	// J[k-2,k-1]
      	     Jk1k  = super_diag(k),
      	     Jk1k1 = sig(k-1),		// J[k-1,k-1]
      	     Jkk   = sig(k),
      	     Jll   = sig(l);		// J[l,l]
      	shift = (Jk1k1-Jkk)*(Jk1k1+Jkk) + (Jk2k1-Jk1k)*(Jk2k1+Jk1k);
      	shift /= 2*Jk1k*Jk1k1;
      	shift += (shift < 0 ? -1 : 1) * sqrt(shift*shift+1);
      	shift = ( (Jll-Jkk)*(Jll+Jkk) + Jk1k*(Jk1k1/shift-Jk1k) )/Jll;
      }
      				// Carry on multiplications by T2, S2, T3...
      double cos_th = 1, sin_th = 1;
      REAL Ji1i1 = sig(l);	// J[i-1,i-1] at i=l+1...k
      for(register int i=l+1; i<=k; i++)
      {
      	REAL Ji1i = super_diag(i), Jii = sig(i);  // J[i-1,i] and J[i,i]
      	sin_th *= Ji1i; Ji1i *= cos_th; cos_th = shift;
      	double norm_f = (super_diag(i-1) = hypot(cos_th,sin_th));
      	cos_th /= norm_f, sin_th /= norm_f;
      					// Rotate J[i-1:i,i-1:i] by Ti
      	shift = cos_th*Ji1i1 + sin_th*Ji1i;	// new J[i-1,i-1]
      	Ji1i = -sin_th*Ji1i1 + cos_th*Ji1i;	// J[i-1,i] after rotation
      	const double Jii1 = Jii*sin_th;		// Emerged J[i,i-1]
      	Jii *= cos_th;				// new J[i,i]
        rotate(V,i,i-1,cos_th,sin_th); // Accumulate T rotations in V
        
        double cos_ph = shift, sin_ph = Jii1;// Make Si to get rid of J[i,i-1]
        sig(i-1) = (norm_f = hypot(cos_ph,sin_ph));	// New J[i-1,i-1]
        if( norm_f == 0 )		// If norm =0, rotation angle
          cos_ph = cos_th, sin_ph = sin_th; // can be anything now
        else
          cos_ph /= norm_f, sin_ph /= norm_f;
      					// Rotate J[i-1:i,i-1:i] by Si
        shift = cos_ph * Ji1i + sin_ph*Jii;	// New J[i-1,i]
        Ji1i1 = -sin_ph*Ji1i + cos_ph*Jii;	// New Jii, would carry over
        					// as J[i-1,i-1] for next i
        rotate(U,i,i-1,cos_ph,sin_ph);  // Accumulate S rotations in U
        				// Jii1 disappears, sin_th would
        cos_th = cos_ph, sin_th = sin_ph; // carry over a (scaled) J[i-1,i+1]
        				// to eliminate on the next i, cos_th
        				// would carry over a scaled J[i,i+1]
      }
      super_diag(l) = 0;		// Supposed to be eliminated by now
      super_diag(k) = shift;
      sig(k) = Ji1i1;
    }		// --- end-of-QR-iterations
    if( sig(k) < 0 )		// Correct the sign of the sing number
    {
      sig(k) = -sig(k);
      for(LAStreamOut Vk(MatrixColumn(V,k)); !Vk.eof(); )
        { REAL& vk = Vk.get(); vk = -vk; }
    }
  }
    
} 


/*
 *------------------------------------------------------------------------
 *				The root Module
 */

SVD::SVD(const Matrix& A)
   : M(A.q_nrows()), N(A.q_ncols()),
     U(A.q_nrows(),A.q_nrows()),
     V(A.q_ncols(),A.q_ncols()),
     sig(A.q_ncols())
{
  if( M < N )
    A.info(),
    _error("Matrix A should have at least as many rows as it has columns");
     
  U.unit_matrix(); V.unit_matrix();

  Vector super_diag(N);
  const double bidiag_norm = bidiagonalize(super_diag,A);
  const double eps = FLT_EPSILON * bidiag_norm;	// Significance threshold
  diagonalize(super_diag,eps);
}

/*
 *------------------------------------------------------------------------
 *		Print some info about the SVD that has been built
 */

				// Print the info about the SVD
void SVD::info(void) const
{
  U.is_valid();
  message("\nSVD of an %dx%d matrix",M,N);
}

				// Return min and max singular values
SVD::operator MinMax(void) const
{
  LAStreamIn sigs(sig);
  MinMax mm(sigs.get());
  while( !sigs.eof() )
    mm << sigs.get();
  return mm;
}

					// sig_max/sig_min
double SVD::q_cond_number(void) const
{
  return ((MinMax)(*this)).ratio();
}

/*
 *------------------------------------------------------------------------
 * 			class SVD_inv_mult
 * 	Solving an (overspecified) set of linear equations A*X=B
 * 		using a least squares method via SVD
 *
 * Matrix A is a rectangular M*N matrix with M>=N, B is a M*K matrix
 * with K either =1 (that is, B is merely a column-vector) or K>1.
 * If B is a unit matrix of the size that of A, the present LazyMatrix is
 * a regularized (pseudo)inverse of A.
 *
 * Algorithm
 *   Matrix A is decomposed first using SVD:
 *	(1) A = U*Sig*V'
 * where matrices U and V are orthogonal and Sig is diagonal.
 * The set of simultaneous linear equations AX=B can be written then as
 *	(2) Sig*V'*X = U'*B
 * (where we have used the fact that U'=inv(U)), or
 *	(3) Sig*Vx = Ub
 * where we introduced
 *	(4) Vx = V'*X and Ub = U'*b
 * Since Sig is a diag matrix, eq. (3) is solved trivially:
 * 	(5) Vx[i] = Ub[i]/sig[i], if sig[i] > tau
 *		  = 0	otherwise
 * In the latter case matrix Sig has an incomplete rank, or close to that.
 * That is, one or more singular values are _too_ small. This means that
 * there is linear dependence among the equations of the set. In that case,
 * we will print the corresponding "null coefficients", the corresponding
 * columns of V. Adding them to the solution X won't change A*X.
 *
 * Having computed Vx, X is simply recovered as
 *	(6) X = V*Vx
 * since V*V' = E.
 *
 * Threshold tau in (5) is chosen as N*FLT_EPSILON*max(Sig[i,i]) unless
 * specified otherwise by the user.
 */

SVD_inv_mult::SVD_inv_mult
	(const SVD& _svd, const Matrix& _B,const double _tau)
	: LazyMatrix(_svd.q_V().q_ncols(),_B.q_ncols()),
	  svd(_svd), B(_B), tau(_tau)
{
  if( svd.q_U().q_nrows() != B.q_nrows() )
    svd.info(), B.info(),
    _error("Unsuitable matrices for SVD*X=B set");
  MinMax sig_minmax = (MinMax)svd;
  if( tau == 0 )
   tau = svd.q_V().q_nrows() * sig_minmax.max() * FLT_EPSILON;
  are_zero_coeff = sig_minmax.min() < tau;
}

#if 0
				// Computing X in a special case where
				// B (and X) is a vector
inline void SVD_inv_mult::fill_in_vector(Matrix& X) const
{
  X.clear();
  bool x_is_vector = x.q_ncols() == 1;
  if( are_zero_coeff )
    message("\nSVD solver of AX=B detected a linear dependency among X"
    	    "\n  #  \tsingular value\tnull coefficients\n");
  for(register int i=1; i<=svd.q_V().q_nrows(); i++)
  {
     MatrixCol Ui(U,i);
     const double sigi = svd.q_sig()(i);
     if( sigi > tau )
       if( x_is_vector )
         X(i) = (Ui * B)/sigi;
       else
         X(i) =0, print null coeffs...
  }
  X *= V;			// ???
}
#endif
				// Computing X in a general case where
				// B (and X) is a rectangular matrix
void SVD_inv_mult::fill_in(Matrix& X) const
{
  if( are_zero_coeff )
    message("\nSVD solver of AX=B detected a linear dependency among X"
    	    "\n  #  singular value\tnull coefficients\n");
  Matrix Vx = zero(X);
  Vector Ui(svd.q_U().q_nrows());			// i-th col of U
  Vector Bj(B.q_nrows());				// j-th col of B
  const Matrix& V = svd.q_V();
  const Matrix& U = svd.q_U();
  LAStreamIn sigs(svd.q_sig());
  for(register int i=1; i<=V.q_ncols(); i++)
  {
    const double sigi = sigs.get();
    LAStrideStreamOut Vxi( MatrixRow(Vx,i) );
    if( sigi > tau )
    {
      to_every(Ui) = of_every(ConstMatrixColumn(U,i));
      for(register int j=1; j<=X.q_ncols(); j++)
        to_every(Bj) = of_every(ConstMatrixColumn(B,j)), Vxi.get() = Ui * Bj/sigi;
    }
    else
    {
      while( !Vxi.eof() ) Vxi.get() = 0;
      message(" %d %12.2g \t(",i,sigi);
      LAStreamIn Vi(ConstMatrixColumn(V,i));
      while( !Vi.eof() )
	message("%10g ",Vi.get());
      message(")\n");
    }
  }
  X.mult(V,Vx);				// that is, set X to be V*Vx
}

