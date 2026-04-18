//	       Sample code showing off a few advanced features
//
// Reflection of the upper triangle of a matrix to the lower one,
// and vice versa
//
// $Id: sample_ult.cc,v 1.1 1998/12/04 18:32:41 oleg Exp oleg $

#include "LAStreams.h"
#include <iostream.h>

#if 0
		// Simple, but witch slight duplication of work
void upper_to_lower_triangle_reflector1(Matrix& m)
{
  // must be a square matrix
  assert( m.q_ncols() == m.q_nrows() );
  for(register int i=0; i<m.q_nrows(); i++)
    to_every(MatrixColumn(m,i)) = of_every(ConstMatrixRow(m,i));
}
#endif

		// No duplication of work; when compiled with
		// -fno-rtti -fno-exceptions flags, all stream constructions
		// and operations are inlined
static void upper_to_lower_triangle_reflector2(Matrix& m)
{
  // must be a square matrix
  assert( m.q_ncols() == m.q_nrows() );
  for(register int i=0; i<m.q_ncols()-1; i++)
  {
    LAStreamOut out_str(MatrixColumn(m,i+m.q_col_lwb()),IRange::from(i+1));
    LAStrideStreamIn in_str(ConstMatrixRow(m,i+m.q_col_lwb()),
			    IRange::from(i+1));
    while( !out_str.eof() )
      out_str.get() = in_str.get();
  }
}

main(void)
{
  cout << "\nChecking reflections of a triangular matrix\n" << endl;
  
  Matrix m(10,10);
  const int seed = 10;
  
  	// Filling in the upper triangle of the matrix
  	// m[i,j] = seed + (i-1) + j*(j-1)/2; j>= i
  {
    LAStreamOut m_str(m);
    for(register int j=0, curr=seed; j<m.q_ncols(); j++)
     {
      for(register int i=0; i<=j; i++)
	m_str.get() = curr++;
      m_str.ignore(m.q_ncols()-j-1);
     }
  }
  m.print("Original matrix with a filled-in upper rectangle");
  upper_to_lower_triangle_reflector2(m);
  m.print("Upon reflection onto the lower triangle");
  verify_matrix_identity(m, transposed(m));
  cout << "Done\n";
}
