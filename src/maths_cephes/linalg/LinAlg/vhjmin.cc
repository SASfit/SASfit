// This may look like C code, but it is really -*- C++ -*-
/*
 ************************************************************************
 *
 *		   Verify the Hooke-Jeeves minimization
 * on a well-known "tough" problems, including some of the DeJong test bed
 *
 * $Id: vhjmin.cc,v 4.2 1998/12/01 17:31:34 oleg Exp oleg $
 * First DeJong function: x1^2 + x2^2 + x3^2; xi in [-5.12,5.12]
 * Third DeJong function (step)
 *	30.0 + sum( floor(xi) ), i=0..4, xi in [-5.12, 5.12]
 * 	min is 0 at xi = -5+eps, eps in [0,0.12]
 *
 * $Id: vhjmin.cc,v 4.2 1998/12/01 17:31:34 oleg Exp oleg $
 *
 ************************************************************************
 */

#include "LAStreams.h"
#include "math_num.h"
#include "builtin.h"
#include <iostream.h>


class ATestFunction : public MultivariateFunctor
{
protected:
  int counter;			// Counts the calls to the function
  const char * const title;
  static const Vector dummy_vector;
  static void pr_vector(LAStreamIn vs);
public:
  ATestFunction(const char _title []) : counter(0), title(_title) {}
  void run(Vector& b0, Vector& h, const Vector& expected_min = dummy_vector);
  void run(Vector& b0, const double h, const Vector& expected_min = dummy_vector);
};


const Vector ATestFunction::dummy_vector = Vector(1,1);

			// Simplified printing of a vector
void ATestFunction::pr_vector(LAStreamIn vs)
{
  while( !vs.eof() )
     printf("%9.3g  ",vs.get());
}

/*
 *-----------------------------------------------------------------------
 * 			  Run a test
 */

void ATestFunction::run(Vector& b0, Vector& h, const Vector& expected_min)
{
  counter = 0;
  cout << title << endl;
  cout << "\nInitial guess         b0 = "; pr_vector(b0);
  cout << "\nFunction value at it  f0 = " << (*this)(b0);
  cout << "\nInitial steps         h0 = "; pr_vector(h);
  cout << "\n";
  printf("\nMinimum f value found  f = %g",hjmin(b0,h,*this));
  cout << "\n                  at  b = "; pr_vector(b0);
  cout << "\nFinal steps           h = "; pr_vector(h);
  cout << "\nNo. of iterations    ni = " << counter << "\n";

  if( &expected_min != &dummy_vector)
  {
    cout << "\nExact min location bm = "; pr_vector(expected_min);
    cout << endl;
    verify_matrix_identity(b0,expected_min);
  }
}

void ATestFunction::run(Vector& b0, const double h, const Vector& expected_min)
{
  counter = 0;
  cout << title << endl;
  cout << "\nInitial guess         b0 = "; pr_vector(b0);
  cout << "\nFunction value at it  f0 = " << (*this)(b0);
  cout << "\nInitial step          h0 = " << h;
  cout << "\n";
  printf("\nMinimum f value found  f  = %g",hjmin(b0,h,*this));
  cout << "\n                  at  b  = "; pr_vector(b0);
  cout << "\nNo. of iterations     ni = " << counter << "\n";

  if( &expected_min != &dummy_vector)
  {
    cout << "\nExact min location  bm = "; pr_vector(expected_min);
    cout << endl;
    verify_matrix_identity(b0,expected_min);
  }
}



/*
 *------------------------------------------------------------------------
 *			  Rosenbroke function
 *	(aka "Banana function" aka the second Dejong function)
 */

static void test1(void)
{
  struct Testf : public ATestFunction
  {
   double operator() (const Vector& v)
   {
    register double x1 = v(1);
    register double x2 = v(2);
    counter++;
    return 100*sqr(x2 - x1*x1) + sqr(1 - x1);
   }
   Testf(void) : ATestFunction("\n\n\tRosenbroke function\n\n\t"
   			       "f = 100*(x2-x1^2)^2 + (1-x1)^2\n") {}
  };
  const int n = 2;
  Vector b0(1,n,-1.2,1.,"END");			// Initial guess
  Vector bm(1,n,1.0,1.0,"END");			// Exact min location
  Vector h(1,n,10.,10.,"END");			// Initial step

  Testf().run(b0,h,bm);
}


/*
 *------------------------------------------------------------------------
 *			     Bocks function
 */

static void test2(void)
{
  struct Testf : public ATestFunction
  {
   double operator() (const Vector& v)
   {
    register double x1 = v(1);
    register double x2 = v(2);
    counter++;
    return sqr( exp(-1./10) - exp(-x1/10) + exp(-10./10) - exp(-x2/10) );
   }
   Testf(void) : ATestFunction("\n\n\tBocks function\n\n\t"
   	"f = [ exp(-1/10) - exp(-x1/10) + exp(-10/10) -exp(-x2/10) ]^2\n") {}
  };
  const int n = 2;
  Vector b0(1,n,0.,0.,"END");			// Initial guess
  Vector bm(1,n,10.0,1.0,"END");		// Exact min location
  double h0=10;					// Initial step

  Testf().run(b0,h0,bm);
}


/*
 *------------------------------------------------------------------------
 *			Mile & Cuntrell function
 */

static void test3(void)
{
  struct Testf : public ATestFunction
  {
   double operator() (const Vector& v)
   {
    register double x1 = v(1);
    register double x2 = v(2);
    register double x3 = v(3);
    register double x4 = v(4);
    counter++;
    return pow( exp(x1)-x2, 4L) + 100*pow(x2-x3,6L) + pow(atan(x3-x4),4L) +
	   pow(x1,8L);
   }
   Testf(void) : ATestFunction("\n\n\tMile & Cuntrell function\n\n\t"
   	"f = [ exp(x1)-x2 ]^4 +100(x2-x3)^6 + atan(x3-x4)^4 + x1^8\n") {}
  };
  const int n = 4;
  Vector b0(1,n,1.,2.,2.,2.,"END");		// Initial guess
  Vector bm(1,n,0.,1.,1.,1.,"END");		// Exact min location
  double h0 = 10;				// Initial step

  Testf().run(b0,h0,bm);
}

/*
 *------------------------------------------------------------------------
 *			  Powell function
 */

static void test4(void)
{
  struct Testf : public ATestFunction
  {
   double operator() (const Vector& v)
   {
    register double x1 = v(1);
    register double x2 = v(2);
    register double x3 = v(3);
    register double x4 = v(4);
    counter++;
    return sqr(x1+10*x2) + 5*sqr(x3-x4) + pow(x2-2*x3,4L) + 10*pow(x1-x4,4L);
   }
   Testf(void) : ATestFunction("\n\n\tPowell function\n\n\t"
   	"f = (x1+10*x2)^2 + 5(x3-x4)^2 + (x2-2x3)^4 + 10(x1-x4)^4\n") {}
  };
  const int n = 4;
  Vector b0(1,n,3.,-1.,0.,1.,"END");		// Initial guess
  Vector bm(1,n,0.,0.,0.,0.,"END");		// Exact min location
  Vector h0(1,n,10.,10.,10.,10.,"END");		// Initial step

  Testf().run(b0,h0,bm);
}

/*
 *------------------------------------------------------------------------
 *				Root module
 */

main()
{
  cout << "\n\n\n\t\tVerify HJMIN multidimensional minimizer\n";
  test1();
  test2();
  test3();
  test4();
}

