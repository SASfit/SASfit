/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher(joachim.kohlbrecher@psi.ch)
 *   Modified 18.11.2013
 */
#include <sasfit_oz.h>

// Fermi Dirac ditribution model:
// C.N. Likos

#define sigma   p[0]    //Diameter of spheres in nm
#define epsilon p[1]    //energy parameter in units of kB*T
#define xi      p[2]    //"smoothing parameter", for xi=0 the PSM is recovered

#define kb GSL_CONST_MKSA_BOLTZMANN

double U_FDM(double r, double T, double *p) {
    if (xi == 0.0) {
        return U_PSM(r,T,p);
    } else {
        return kb*T*epsilon*
           (1.0+exp(-sigma/xi))/
           (1.0+exp((r-sigma)/xi));
    }
}
