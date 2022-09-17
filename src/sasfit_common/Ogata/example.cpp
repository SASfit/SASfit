// bessel.cpp
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include "FBT.h"


double test( double x, double width ){ return x*exp(-x/width);} // test function to transform data allows to send anything else to the function
double exact( double qT){ return pow((1.+qT*qT),-1.5);} // test function exact


double test1( double x){ return x*exp(-x*x);} // test function to transform
double exact1( double qT ){ return exp(-qT*qT/4.)/2.;} // test function exact

int main( void )
{
  FBT ogata0 = FBT(); // Fourier Transform with Jnu, nu=0.0 and N=10
  FBT ogata1 = FBT(0.0, 1); // Fourier Transform with Jnu, nu=0.0 and N=10
  FBT ogata2 = FBT(0.0, 2); // Fourier Transform with Jnu, nu=0.0 and N=10
  double qT = 1.;
  double width = 1.;


  auto begin = std::chrono::high_resolution_clock::now();
  double res = ogata0.fbt(std::bind(test, std::placeholders::_1, width),qT);
  auto end = std::chrono::high_resolution_clock::now();

  std::cout << std::setprecision(30) << " FT( J0(x*qT) x*exp(-x) ) at qT= " << qT << std::endl;
  std::cout << std::setprecision(30) << "Numerical transformed = " << res << std::endl;
  auto overhead=std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count();
  std::cout<<"Calc time: "<<overhead<<" nanoseconds\n";


  begin = std::chrono::high_resolution_clock::now();
  res = ogata1.fbt(std::bind(test, std::placeholders::_1, width),qT);
  end = std::chrono::high_resolution_clock::now();

  std::cout << std::setprecision(30) << "Numerical untransformed = " << res << std::endl;
  overhead=std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count();
  std::cout<<"Calc time: "<<overhead<<" nanoseconds\n";


  begin = std::chrono::high_resolution_clock::now();
  res = ogata2.fbt(std::bind(test, std::placeholders::_1, width),qT);
  end = std::chrono::high_resolution_clock::now();

  std::cout << std::setprecision(30) << "Numerical untransformed (h=0.05) = " << res << std::endl;
  overhead=std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count();
  std::cout<<"Calc time: "<<overhead<<" nanoseconds\n";


  std::cout << std::setprecision(30) << "Exact = " << exact(qT) << std::endl;


  begin = std::chrono::high_resolution_clock::now();
  res = ogata0.fbt(test1,qT);
  end = std::chrono::high_resolution_clock::now();

  std::cout << std::setprecision(30) << " FT( J0(x*qT) x*exp(-x**2) ) at qT= " << qT << std::endl;
  std::cout << std::setprecision(30) << "Numerical transformed = " << res << std::endl;
  overhead=std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count();
  std::cout<<"Calc time: "<<overhead<<" nanoseconds\n";


  begin = std::chrono::high_resolution_clock::now();
  res = ogata1.fbt(test1,qT);
  end = std::chrono::high_resolution_clock::now();

  std::cout << std::setprecision(30) << "Numerical untransformed = " << res << std::endl;
  overhead=std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count();
  std::cout<<"Calc time: "<<overhead<<" nanoseconds\n";


  begin = std::chrono::high_resolution_clock::now();
  res = ogata2.fbt(test1,qT);
  end = std::chrono::high_resolution_clock::now();

  std::cout << std::setprecision(30) << "Numerical untransformed (h=0.05) = " << res << std::endl;
  overhead=std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count();
  std::cout<<"Calc time: "<<overhead<<" nanoseconds\n";


  std::cout << std::setprecision(30) << "Exact = " << exact1(qT) << std::endl;

}
