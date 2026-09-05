#ifndef SQUAREWELL_COMMON_H
#define SQUAREWELL_COMMON_H

double sw_c0_hat(double kstar, double phi);
double sw_fhat_mayer(double k, double x, double lam);
double sw_Phat(int j, double k);
extern const double SW_PHAT0[4];

#endif
