// This may look like C code, but it is really -*- C++ -*-
//
//			Verify ZEROIN routine
// $Id: vzeroin.cc,v 4.1 1998/04/28 18:49:54 oleg Exp oleg $

#include "math_num.h"
#include <stdio.h>

class ATestFunction : public UnivariateFunctor
{
protected:
  int counter;			// Counts the calls to the function
  const char * const title;
public:
  ATestFunction(const char _title []) : counter(0), title(_title) {}
  double run(const double a, const double b);
  double run(const double a, const double b, const double expected_root);
};

#define XY(X,Y) X##Y
#define MakeNameXY(FX,LINE) XY(FX,LINE)
#define MakeName(FX) MakeNameXY(FX,__LINE__)

#define Lambda(args,ret_type,body) \
class MakeName(__Lambda___) { \
public: ret_type operator() args { body; } }

#define MakeTestFunction(title,abstraction) \
class MakeName(TestFunction): public ATestFunction		\
{								\
abstraction lambda;						\
public: double operator() (const double x) { counter++; return lambda(x); } \
MakeName(TestFunction)(void) : ATestFunction(title) {}	\
}; MakeName(TestFunction)

			// Run a test
double ATestFunction::run(const double a, const double b)
{
  counter = 0;
  const double root = zeroin(a,b,*this);
  printf("\nFor function %s,\nthe root over [%g,%g] is\t%.9e\n",
  	title,a,b,root);
  printf("The function value at the root is\t%.4e\n"
	       "Total iterations\t\t%d\n",
	       (*this)(root), counter);
  return root;
}

			// Run a test and make sure the result is "right"
double ATestFunction::run(const double a, const double b,
	const double expected_root)
{
  const double found_root = run(a,b);
  printf("The expected value of the root is\t%.7e\n",expected_root);
  assert( abs(expected_root-found_root) < FLT_EPSILON);
  return found_root;
}


main(void)
{
  printf("\n\n\t\tTesting Brent's root finder\n\n");

  MakeTestFunction("x^3 - 2*x - 5, from the Forsythe book",
    Lambda((const double x),double,
	   return (sqr(x)-2)*x - 5))().run(2.0,3.0,2.0945514815);

  MakeTestFunction("cos(x)-x",
		   Lambda((const double x),double,return cos(x)-x)) fcos;
  fcos.run(2.0,3.0);
  fcos.run(-1.0,3.0,0.739085133);

  MakeTestFunction("sin(x)-x",
		   Lambda((const double x),double,return sin(x)-x))().run(-1.0,3.0,0.0);

  MakeTestFunction("HUMPS function Matlab:Demos:zerodemo.m",
   Lambda((const double x),double, 
	  return 1/(sqr(x-0.3) + .01) + 1/(sqr(x-0.9) + .04) - 6))().
		   run(0.4,1.6,1.29954968);
}
