/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   18.11.2013
 */
#include <sasfit_oz.h>

//hard sphere potential with 3 Yukawa tails:
#define sigma   p[0] // Diameter of spheres in nm
#define K1      p[1] // depth of first Yukawa contribution
#define LAMBDA1 p[2] // inverse range of first Yukawa contribution
#define K2      p[3] // depth of second Yukawa contribution
#define LAMBDA2 p[4] // depth of second Yukawa contribution
#define K3      p[5] // depth of third Yukawa contribution
#define LAMBDA3 p[6] // depth of third Yukawa contribution

double U_HS_3Yukawa(double r, double T, double *p) {
    double Y1, Y2, Y3;
    if (r<sigma) {
        return GSL_POSINF;
    } else {
        Y1 = K1*exp(-LAMBDA1*(r-sigma));
        Y2 = K2*exp(-LAMBDA2*(r-sigma));
        Y3 = K3*exp(-LAMBDA3*(r-sigma));
        return kb*T*sigma/r*(Y1+Y2+Y3);
    }
}

