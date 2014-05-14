/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   Modified 17.09.2013
 */
#include <sasfit_oz.h>

// Lennard Jones Potential:
// http://en.wikipedia.org/wiki/Lennard-Jones_potential
// Lennard-Jones, J. E. (1924),
// "On the Determination of Molecular Fields",
// Proc. R. Soc. Lond. A 106 (738): 463–477,
// doi:10.1098/rspa.1924.0082

#define sigma p[0] //Diameter of spheres in nm
#define epsilon p[1] //  is the strength of the interaction in units of kb*T

double U_Lennard_Jones(double r, double T, double *p) {
    double tmp;
    if (r == 0) {
        return GSL_POSINF;
    } else {
        tmp = gsl_pow_6(sigma/r);
        if (gsl_finite(gsl_pow_2(tmp))) {
            return 4*kb*T*epsilon*(gsl_pow_2(tmp)-tmp);
        } else {
            return GSL_POSINF;
        }
    }
}


double U_SR_Lennard_Jones(double r, double T, double *p) {
    double rm;
    rm = pow(2.0,1.0/6.0)*p[0];
    if (r <= rm) {
        return U_Lennard_Jones(r,T,p)-U_Lennard_Jones(rm,T,p);
    } else {
        return 0.0;
    }
}

double U_LR_Lennard_Jones(double r, double T, double *p) {
    double rm;
    rm = pow(2.0,1.0/6.0)*p[0];
    if (r <= rm) {
        return U_Lennard_Jones(rm,T,p);
    } else {
        return U_Lennard_Jones(r,T,p);
    }
}
