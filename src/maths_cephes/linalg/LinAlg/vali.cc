// This may look like C code, but it is really -*- C++ -*-
//
//		Verification of the Aitken-Lagrange Interpolation
//

#include "LinAlg.h"
#include "math_num.h"
#include <iostream.h>

/*
 *------------------------------------------------------------------------
 *		Set of the test functions to interpolate
 */

static double (*F)(const double x);

static double f_exp(const double x)
{
  return exp(-x);
}

static double f_sinexp(const double x)
{
  return sin(x) * exp(-x/10.0);
}

/*
 *------------------------------------------------------------------------
 *	Set of tests for the interpolation over the uniform grid
 */

			// Verify that interpolation exactly at a node
			// really gives the function value at the node
static void test_u_exact(const double s, const int ia, const int ib)
{
  cout << "\nCheck to see that interpolation at a node gives an exact"
          " result\n";
  cout << "\nUniform grid [" << s*ia << ':' << s*ib << "] with the step " << s << endl;

  Vector y(ia,ib);
  Vector y_int(ia,ib);

  register int i;
  for(i=y.q_lwb(); i<=y.q_upb(); i++)
    y(i) = (*F)(s*i);

  for(i=y_int.q_lwb(); i<=y_int.q_upb(); i++)
    y_int(i) = ali(s*i,s*ia,s,y);

  //  assert(y == y_int);
  verify_matrix_identity(y,y_int);

  cout << "\nDone\n";
}

			// Check the precision for interpolation
			// at arbitrary points
static void test_u(const double s, const int ia, const int ib)
{
  cout << "\nCheck the precision of the interpolation at arbitrary points\n";
  cout << "\nUniform grid [" << s*ia << ':' << s*ib << "] with the step " << s
       << endl;

  Vector y(ia,ib);
  register int i;
  for(i=y.q_lwb(); i<=y.q_upb(); i++)
    y(i) = (*F)(s*i);

  cout << "\nPoint	Exact function value     Interpolated    Error\n";
  register double q;
  for(q=(ia-1)*s; q < (ib+2)*s; q += 1.1*s)
  {
    double yexact = (*F)(q);
    double yint = ali(q,ia*s,s,y);
    printf("%4.2g\t%12.6g\t\t%12.6g  %12.6g\n",q,yexact,yint,
		 abs(yexact-yint));
  }

  cout << "\nDone\n";
}

/*
 *------------------------------------------------------------------------
 *	Set of tests for the interpolation over the non-uniform grid
 */

			// Verify that interpolation exactly at a node
			// really gives the function value at the node
static void test_n_exact(const Vector& x)
{
  cout << "\nCheck to see that interpolation at a node gives an exact"
          " result\n";

  Vector y(x);
  Vector y_int(x);

  register int i;
  for(i=y.q_lwb(); i<=y.q_upb(); i++)
    y(i) = (*F)(x(i));

  for(i=y_int.q_lwb(); i<=y_int.q_upb(); i++)
    y_int(i) = ali(x(i),x,y);

  assert(y == y_int);

  cout << "\nDone\n";
}

			// Check the precision for interpolation
			// at arbitrary points
static void test_n(const Vector& x,const double s, const int ia, const int ib)
{
  cout << "\nCheck the precision of the interpolation at arbitrary points\n";

  Vector y(x);
  register int i;
  for(i=y.q_lwb(); i<=y.q_upb(); i++)
    y(i) = (*F)(x(i));

  cout << "\nPoint	Exact function value     Interpolated    Error\n";
  register double q;
  for(q=ia*s; q < ib*s; q += s)
  {
    double yexact = (*F)(q);
    double yint = ali(q,x,y);
    printf("%4.2g\t%12.6g\t\t%12.6g  %12.6g\n",q,yexact,yint,
		 abs(yexact-yint));
  }

  cout << "\nDone\n";
}

			// Check the precision for interpolation
			// on example in the book
static void test_n_book(void)
{
  cout << "\nCheck the precision of the interpolation at arbitrary points\n";
  cout << "\nExample from Fig. 4.11 of the book"
          "\n\tNumerical Methods and Software,"
	  "\n\tby D.Kahaner, C.Moler, and S.Nash - Prentice Hall, 1989\n";

  REAL xy[2][11] =
  { {0,  2,  3,  5,  6,  8,  9,    11, 12, 14, 15},	// abscissae
    {10, 10, 10, 10, 10, 10, 10.5, 15, 50, 60, 85}	// ordinates
  };

  Vector x(0,sizeof(xy[0])/sizeof(xy[0][0])-1);
  Vector y(x);

  for(register unsigned int i=0; i<sizeof(xy[0])/sizeof(xy[0][0]); i++)
    x(i) = xy[0][i], y(i) = xy[1][i];

  cout << "\n\t\tInterpolation nots";
  cout << "\nx ";
  for(register int i=x.q_lwb(); i<=x.q_upb(); i++)
    printf("%6.2g ",x(i));
  cout << "\ny ";
  for(register int i=y.q_lwb(); i<=y.q_upb(); i++)
    printf("%6.2g ",y(i));

  cout << "\n\nPoint	Interpolated value\n";
  for(register double q=0; q <= 16; q += 16/50.)
    printf("%7.4g   %12.6g\n",q,ali(q,x,y));

  cout << "\nDone\n";
}

/*
 *------------------------------------------------------------------------
 *				Root module
 */

main()
{
  cout << "\n\n\t\tVerification of the Aitken-Lagrange interpolation\n";

  cout << "\n------------- Interpolation over the table with uniform grid";

  cout << "\nFunction to interpolate: exp(-x)\n";
  F = f_exp;

  const double s = 0.1;
  test_u_exact(s,1,10);
  test_u(s,1,10);

  printf("\n------------- Interpolation over the table with non-uniform grid");

  printf("\nFunction to interpolate: sin(x) * exp(-x/10)\n");
  F = f_sinexp;

  register int i;
  Vector nodes(2,51);
  for(i=nodes.q_lwb(); i<=11; i++)
    nodes(i) = (i-1.)/10;
  for(i=12; i<=nodes.q_upb(); i++)
    nodes(i) = (i-6.)/5;
  cout << "\nGrids 0.1 .. 1 have the mesh 0.1, and 1..9 have the mesh 0.2\n";

  test_n_exact(nodes);
  test_n(nodes,0.27,0,15);
  test_n_book();
}
