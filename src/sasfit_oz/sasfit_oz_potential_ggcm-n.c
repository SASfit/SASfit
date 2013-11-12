/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher(joachim.kohlbrecher@psi.ch)
 *   Modified 27.09.2013
 */
#include <sasfit_oz.h>

// generalized Gaussian core model GGCM-n:
// Condensed Matter Physics, 2005, Vol 8, No.1(41), pp 135-148
// DOI:10.5488/CMP.8.1.135
// Phys. Rev. E 62, 7961–7972 (2000)
// DOI: 10.1103/PhysRevE.62.7961

#define sigma p[0]   // Diameter of spheres in nm
#define epsilon p[1] // energy parameter in units of kB*T
#define NEXP	p[2] // exponent n
#define AALPHA  p[3] // scaling parameter alpha in th eexponetial

double U_GGCM_n(double r, double T, double *p) {
    if (r<sigma) return kb*T*epsilon*exp(-AALPHA*pow(r/sigma,NEXP));
    else  return 0;
}
