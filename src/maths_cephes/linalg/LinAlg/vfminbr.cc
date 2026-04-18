// This may look like C code, but it is really -*- C++ -*-
//
//			Verify FMINBR routine
// $Id: vfminbr.cc,v 4.1 1998/12/16 20:32:22 oleg Exp oleg $


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
  double run(const double a, const double b, const double expected_min);
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
  const double minloc = fminbr(a,b,*this);
  printf("\nFor function %s,\nthe minimum over [%g,%g] is\t%.9e\n",
  	title,a,b,minloc);
  printf("The min function value found\t%.4e\n"
	       "Total iterations\t\t%d\n",
	       (*this)(minloc), counter);
  return minloc;
}

			// Run a test and make sure the result is "right"
double ATestFunction::run(const double a, const double b,
	const double expected_min)
{
  const double found_minloc = run(a,b);
  printf("The min is expected value at\t%.7e\n",expected_min);
  assert( abs(expected_min-found_minloc) < FLT_EPSILON);
  return found_minloc;
}


main(void)
{
  printf("\n\n\t\tTesting Brent's one-dimensional minimizer\n\n");
  
  MakeTestFunction("x^3 - 2*x - 5, from the Forsythe book",
    Lambda((const double x),double,
		   return (sqr(x)-2)*x - 5))().run(0.0,1.0,0.816496593);

  MakeTestFunction("(x^3 - 2*x - 5)^2, from the Forsythe book, see vzeroin.cc",
  	Lambda((const double x),double, 
		   return sqr((sqr(x)-2)*x - 5)))().run(2.0,3.0,2.0945514815);

  MakeTestFunction("(cos(x)-x)^2 -2",
		Lambda((const double x),double, return sqr(cos(x)-x)-2)) fcos;
  fcos.run(2.0,3.0);
  fcos.run(-1.0,3.0,0.739085133);
  MakeTestFunction("(sin(x)-x)^2 +1",
		Lambda((const double x),double, return sqr(sin(x)-x)+1))().run(-1.0,3.0);
  MakeTestFunction("HUMPS function Matlab:Demos:humps.m",
    Lambda((const double x),double,
		   return -(1/(sqr(x-0.3) + .01) + 1/(sqr(x-0.9) + .04) - 6)))().run(0,2);
}
