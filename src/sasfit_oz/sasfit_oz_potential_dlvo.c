/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher(joachim.kohlbrecher@psi.ch)
 *   Modified 27.09.2013
 */
#include <sasfit_oz.h>

// Derjaguin–Landau–Verwey–Overbeek (DLVO) potential.:
// Condensed Matter Physics, 2005, Vol 8, No.1(41), pp 135-148
// DOI:10.5488/CMP.8.1.135
// Phys. Rev. E 62, 7961–7972 (2000)
// DOI: 10.1103/PhysRevE.62.7961
//
// G. Nägele,
// The Physics of Colloid Soft Matter: Lecture Notes 14,
// Polish Academy of Sciences Publishing, Warsaw (2004)
// hera.physik.uni-konstanz.de/igk/news/workshops/Naegele.pdf?

#define sigma   p[0] // Diameter of spheres in nm
#define kappa   p[1] // reciprocal Debye–Hückel screening length
					 // kappa^2=4 pi LB sum(rho_alpha Z_alpha^2,alpha)
#define Z		p[2] // effective or dressed surface charge number of a colloidal particle
#define LB 		p[3] // Bjerrum length LB = e^2/(epsilon kB T), for water at RT LB=0.71

double U_DLVO(double r, double T, double *p) {
	double a;
	a = 0.5*sigma;
    if (r<=sigma) {
		return GSL_POSINF;
	} else {
		return kb*T*LB*gsl_pow_2(Z)*
    			gsl_pow_2(exp(kappa*a)/(1.0+kappa*a))*
    			exp(-kappa*r)/r;
	}
}
