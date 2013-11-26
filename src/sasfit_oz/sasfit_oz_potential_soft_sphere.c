/*
 * Author(s) of this file:
 *   Evgeniy Ponomarev (evgeniy.ponomarev@epfl.ch)
 *   Modified 13.09.2013
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   25.11.2013
 */
#include <sasfit_oz.h>

//Soft sphere potential:
#define SIGMA   p[0]    // Diameter of spheres in nm
#define EPSILON p[1]    // energy scale parameter
#define NN      p[2]    // stiffness parameter

 double U_Soft_Sphere(double r, double T, double *p) {
    return kb*T*EPSILON*pow(SIGMA/r,NN);
 }

 double U_Parabolic_Sphere(double r, double T, double *p) {
    if (r<=SIGMA) {
        return kb*T*EPSILON*pow(1.0-(r/SIGMA),2.5);
    } else {
        return 0.0;
    }
}
