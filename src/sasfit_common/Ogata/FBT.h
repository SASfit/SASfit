#ifndef __FBT_H
#define __FBT_H
#include <algorithm>    // std::copy
#include <vector>       // std::vector
#include <functional>   // std::bind

#include <gsl/gsl_sf_bessel.h>
#include <gsl/gsl_min.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>

class FBT {
private:
  double nu; // nu is Bessel function order
  int N;     // N is number of function calls
  double Q;  // a rough estimate where the function x*f(x) has maximum x = Q
  int option; // 0 - modified Ogata (default), 1 - unmodified Ogata, 2 - unmodified ogata with h=0.05
  constexpr static double nu_def = 0.0;
  const static int N_def = 10;
  const static int option_def = 0; // 0 - modified Ogata (default)
  constexpr static double Q_def = 1.; // a rough estimate where the maximum of the function f(x) is
  /// These are vectors needed in Ogata, will be initialized in the constructor
  std::vector<double> jn_zeros0;
  std::vector<double> zeros;
  std::vector<double> xi;
  std::vector<double> Jp1;
  std::vector<double> w;





  void acknowledgement();
  void citation();
  double get_ht(double hu);

  double ogatat(std::function<double (double) > f, double q, double h);
  double ogatau(std::function<double (double) > f, double q, double h);
  double get_hu(std::function<double (double) > f,  double q);

public:
  FBT(double _nu = nu_def, int option = option_def, int _N = N_def, double _Q = Q_def); // Constructor
  ~FBT(); // Deconstructor

  /// Perform fast fourier
  double fbt(std::function<double (double) > g,  double q);  // modified Ogata

};

#endif // #ifndef __FBT_H
