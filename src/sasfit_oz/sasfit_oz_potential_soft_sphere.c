/*
 * Author(s) of this file:
 *   Evgeniy Ponomarev (evgeniy.ponomarev@epfl.ch)
 *   Modified 13.09.2013
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   16.09.2013
 */
#include <sasfit_oz.h>

//Soft sphere potential:
#define sigma p[0] // Diameter of spheres in nm
#define EPSILON p[1] // Hertz potential strength - unitless
 double U_Soft_Sphere(double r, double T, double *p) {
    if (r<p[0]) return kb*T*EPSILON*pow(1-(r/sigma),2.5);

    else  return 0;
}
