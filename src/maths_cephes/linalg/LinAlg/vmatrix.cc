// This may look like C code, but it is really -*- C++ -*-
/*
 ************************************************************************
 *
 *		  Verify Primitive Operations on Matrices
 *	(creation, special cases and element-by-element operations)
 *
 *
 * $Id: vmatrix.cc,v 4.2 1998/11/24 20:08:09 oleg Exp oleg $
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
 *	   Test allocation functions and compatibility check
 */

static void test_allocation(void)
{

  cout << "\n\n---> Test allocation and compatibility check\n";

  Matrix m1(4,20);	m1.set_name("Matrix m1");
  Matrix m2(1,4,1,20);	m2.set_name("Matrix m2");
  Matrix m3(1,4,0,19);	m3.set_name("Matrix m3");
  Matrix m4(m1);	m4.set_name("Matrix m4");
  cout << "The following matrices have been allocated\n";
  m1.info(), m2.info(), m3.info(), m4.info();

  cout << "\nStatus information reported for matrix m3:\n";
  cout << "  Row lower bound ... " << m3.q_row_lwb() << "\n";
  cout << "  Row upper bound ... " << m3.q_row_upb() << "\n";
  cout << "  Col lower bound ... " << m3.q_col_lwb() << "\n";
  cout << "  Col upper bound ... " << m3.q_col_upb() << "\n";
  cout << "  No. rows ..........." << m3.q_nrows() << "\n";
  cout << "  No. cols ..........." << m3.q_ncols() << "\n";
  cout << "  No. of elements ...." << m3.q_no_elems() << "\n";
  cout << "  Name " << m3.q_name() << "\n";
  assert( m1.q_no_elems() == m1.q_nrows() * m1.q_ncols() );
  assert( m2.q_no_elems() == m2.q_nrows() * m2.q_ncols() );
  assert( m3.q_no_elems() == m3.q_nrows() * m3.q_ncols() );
  assert( m4.q_no_elems() == m4.q_nrows() * m4.q_ncols() );
  
  cout << "\nCheck matrices 1 & 2 for compatibility\n";
  are_compatible(m1,m2);
  assert( static_cast<const DimSpec&>(m1) == m2 );

  cout << "Check matrices 1 & 4 for compatibility\n";
  are_compatible(m1,m4);
  assert( static_cast<const DimSpec&>(m1) == m4 );

  assert( !(static_cast<const DimSpec&>(m1) == m3) );
#if 0
  cout << "Check matrices 1 & 3 for compatibility\n";
  are_compatible(m1,m3);
#endif

  cout << "m2 has to be compatible with m3 after resizing to m3\n";
  m2.resize_to(m3);
  assert( static_cast<const DimSpec&>(m2) == m3 );
  are_compatible(m2,m3);

  Matrix m5(DimSpec(m1.q_nrows()+1,m1.q_ncols()+5)); m5.set_name("Matrix m5");
  cout << "m1 has to be compatible with m5 after resizing to m5\n";
  m5.info();
  m1.resize_to(m5.q_nrows(),m5.q_ncols());
  assert( static_cast<const DimSpec&>(m1) == m5 );
  are_compatible(m1,m5);

  cout << "\nDone\n";

}

/*
 *------------------------------------------------------------------------
 *		     Test uniform element operations
 */

static void test_element_op(const int rsize, const int csize)
{
  const double pattern = 8.625;
  register int i,j;

  cout << "\n\n---> Test operations that treat each element uniformly\n";

  Matrix mc(-1,rsize-2,1,csize);
  MatrixDA m(mc);

  cout << "Writing zeros to m...\n";
  for(i=m.q_row_lwb(); i<=m.q_row_upb(); i++)
    for(j=m.q_col_lwb(); j<=m.q_col_upb(); j++)
      m(i,j) = 0;
  verify_element_value(m,0);

  cout << "Creating zero m1 ...\n";
  Matrix m1 = zero(m.get_container());
  verify_element_value(m1,0);

  cout << "Comparing m1 with 0 ...\n";
  assure(of_every(m1) == 0, "m1 is not zero!");
  assure(!(of_every(m1) != 0), "m1 is not zero!");

  cout << "Writing a pattern " << pattern << " by assigning to m(i,j)...\n";
  for(i=m.q_row_lwb(); i<=m.q_row_upb(); i++)
    for(j=m.q_col_lwb(); j<=m.q_col_upb(); j++)
      m(i,j) = pattern;
  verify_element_value(m,pattern);

  cout << "Writing the pattern by assigning to m1 as a whole ...\n";
  to_every(m1) = pattern;
  verify_element_value(m1,pattern);

  cout << "Comparing m and m1 ...\n";
  assure(m == m1, "m and m1 are unexpectedly different!");
  cout << "Comparing (m=0) and m1 ...\n";
  assert(!(mc.clear() == m1));

  cout << "Clearing m1 ...\n";
  m1.clear();
  verify_element_value(m1,0);

  cout << "\nClear m and add the pattern\n";
  m.get_container().clear();
  to_every(m) += pattern;
  verify_element_value(m,pattern);
  cout << "   add the doubled pattern with the negative sign\n";
  m += -2*pattern;
  verify_element_value(m,-pattern);
  cout << "   subtract the tripled pattern with the negative sign\n";
  m -= -3*pattern;
  verify_element_value(m,2*pattern);

  cout << "\nVerify comparison operations when all elems are the same\n";
  mc = pattern;
  assert( m == pattern && !(m != pattern) );
  assert( of_every(m) > 0 && of_every(m) >= pattern && 
          of_every(m) <= pattern );
  assert( of_every(m) > -pattern && of_every(m) >= -pattern );
  assert( of_every(m) <= pattern && !(of_every(m) < pattern) );
  m -= 2*pattern;
  assert( of_every(m)  < -pattern/2 && of_every(m) <= -pattern/2 );
  assert( of_every(m)  >= -pattern && !(of_every(m) > -pattern) );

  cout << "\nVerify comparison operations when not all elems are the same\n";
  to_every(m) = pattern; m(m.q_row_upb(),m.q_col_upb()) = pattern-1;
  assert( !(m == pattern) && !(of_every(m) != pattern) );
  assert( m != 0 ); 					// none of elements are 0
  assert( !(of_every(m) >= pattern) && of_every(m) <= pattern && 
          !(of_every(m)<pattern) );
  assert( !(of_every(m) <= pattern-1) && of_every(m) >= pattern-1 && 
          !(of_every(m)>pattern-1) );

  cout << "\nAssign 2*pattern to m by repeating additions\n";
  to_every(m) = 0; to_every(m) += pattern; m += pattern;
  cout << "Assign 2*pattern to m1 by multiplying by two \n";
  to_every(m1) = pattern; m1 *= 2;
  verify_element_value(m1,2*pattern);
  assert( m == m1 );
  cout << "Multiply m1 by one half returning it to the 1*pattern\n";
  m1 *= 1/2.;
  verify_element_value(m1,pattern);

  cout << "\nAssign -pattern to m and m1\n";
  mc.clear(); m -= pattern; to_every(m1) = -pattern;
  verify_element_value(m,-pattern);
  assert( m == m1 );
  assert( of_every(m).max_abs() == pattern );
  assert( of_every(m).sum_abs() == - of_every(m).sum() );
  assert( abs(of_every(m).sum_abs() - pattern*m.get_container().q_no_elems()) 
  	<= FLT_EPSILON );
  assert( abs(of_every(m).sum_squares() - pattern*pattern*mc.q_no_elems()) 
  	<= FLT_EPSILON );
  cout << "m = sqrt(sqr(m)); m1 = abs(m1); Now m and m1 have to be the same\n";
  to_every(m).sqr();
  verify_element_value(m,pattern*pattern);
  to_every(m).sqrt();
  verify_element_value(m,pattern);
  to_every(m1).abs();
  verify_element_value(m1,pattern);
  assert( m == m1 );

  cout << "\nCheck out to see that sin^2(x) + cos^2(x) = 1\n";
  class FillMatrix : public ElementAction
  {
    int no_elems, no_cols;
    void operation(REAL& element, const int i, const int j)
		{ element = 4*M_PI/no_elems * (i*no_cols+j); }
    public: FillMatrix(const Matrix& m) :
			no_elems(m.q_no_elems()), no_cols(m.q_ncols()) {}
  };
  m.get_container().apply(FillMatrix(m.ref()));
  m1 = m;
  class ApplyFunction : public ElementWiseAction
  {
    double (*func)(const double x);
    void  operator () (REAL& element) { element = func(element); }
    public: ApplyFunction(double (*_func)(const double x)) : func(_func) {}
  };
  to_every(m).apply(ApplyFunction(sin));
  to_every(m1).apply(ApplyFunction(cos));
  to_every(m).sqr();
  to_every(m1).sqr();
  to_every(m) += of_every(m1);
  verify_element_value(m,1);

  cout << "\nDone\n\n";
}

/*
 *------------------------------------------------------------------------
 *  		Test binary matrix element-by-element operations
 */

static void test_binary_ebe_op(const int rsize, const int csize)
{
  const double pattern = 4.25;

  cout << "\n---> Test Binary Matrix element-by-element operations\n";

  Matrix m(2,rsize+1,0,csize-1);
  Matrix m1 = zero(m);
  Matrix mp = zero(m);

  {
    MatrixDA mpa(mp);
    for(register int i=mp.q_row_lwb(); i<=mp.q_row_upb(); i++)
      for(register int j=mp.q_col_lwb(); j<=mp.q_col_upb(); j++)
        mpa(i,j) = (i-m.q_nrows()/2.)*j*pattern;
  }
  

  cout << "\nVerify assignment of a matrix to the matrix\n";
  m = pattern;
  m1.clear();
  m1 = m;
  verify_element_value(m1,pattern);
  assert( m1 == m );

  cout << "\nAdding the matrix to itself, uniform pattern " << pattern << "\n";
  m.clear(); to_every(m) = pattern;
  m1 = m; to_every(m1) += of_every(m1);
  verify_element_value(m1,2*pattern);
  cout << "  subtracting two matrices ...\n";
  to_every(m1) -= of_every(m);
  verify_element_value(m1,pattern);
  cout << "  subtracting the matrix from itself\n";
  to_every(m1) -= of_every(m1);
  verify_element_value(m1,0);
  cout << "  adding two matrices together\n";
  to_every(m1) += of_every(m);
  verify_element_value(m1,pattern);

  cout << "\nArithmetic operations on matrices with not the same elements\n";
  cout << "   adding mp to the zero matrix...\n";
  m.clear(); to_every(m) += of_every(mp);
  verify_matrix_identity(m,mp);
#if 0
  m1 = m;
  cout << "   making m = 3*mp and m1 = 3*mp, via add() and succesive mult\n";
  add(m,2,mp);
  m1 += m1; m1 += mp;
  verify_matrix_identity(m,m1);
  cout << "   clear both m and m1, by subtracting from itself and via add()\n";
  m1 -= m1;
  add(m,-3,mp);
  verify_matrix_identity(m,m1);
#endif

  cout << "\nTesting element-by-element multiplications and divisions\n";
  cout << "   squaring each element with sqr() and via multiplication\n";
  m = mp; m1 = mp;
  to_every(m).sqr();
  to_every(m1) *= of_every(m1);
  verify_matrix_identity(m,m1);
  cout << "   compare (m = pattern^2)/pattern with pattern\n";
  m = pattern; to_every(m1) = pattern;
  to_every(m).sqr();
  to_every(m) /= of_every(m1);
  verify_element_value(m,pattern);
  compare(m1,m,"Original matrix and matrix after squaring and dividing");

  cout << "\nDone\n";
}


/*
 *------------------------------------------------------------------------
 *				Root module
 */

main()
{
  cout << "\n\n" << _Minuses << 
          "\n\t\tVerify Basic operations on Matrices\n";

  test_allocation();
  test_element_op(20,10);
  test_binary_ebe_op(10,20);
  
  cout << "\n\nAll test passed" << endl;
}

