/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher(joachim.kohlbrecher@psi.ch)
 *   Modified 27.09.2013
 */
#include <sasfit_oz.h>

// penetrable spherical model:
// C.N. Likos, M. Watzlawek, H. Loewen, Phys. Rev. E 58 (1998) 3135.

#define sigma p[0]   //Diameter of spheres in nm
#define epsilon p[1] //energy parameter in units of kB*T

double U_PSM(double r, double T, double *p) {
    if (r<=sigma) return kb*T*epsilon;
    else  return 0;
}
