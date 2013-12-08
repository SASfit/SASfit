/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   14.11.2013
 */
#include <sasfit_oz.h>

// star polymer interaction potential:
// C.N.Likos, H.M.Harreis, Condensed Matter Physics, 2002, Vol. 5, No. 1(29), pp. 173–200

#define sigma p[0] // Diameter of spheres in nm
#define f p[1] // number of arm (functionality)
// valid for f >= 10
 double U_Star1(double r, double T, double *p) {
    double Ustar;
    if (r<=sigma) {
        Ustar =   5.0/18.0*pow(f,3.0/2.0)
                * (-log(r/sigma)+1.0/(1.0+sqrt(f)/2.0));
    } else {
        Ustar =   5.0/18.0*pow(f,3.0/2.0)
                * 1.0/(1.0+sqrt(f)/2.0)
                * sigma/r
                * exp(-sqrt(f)*(r-sigma)/(2.0*sigma));
    }
    return kb*T*Ustar;
}

// valid for f <= 10
 double U_Star2(double r, double T, double *p) {
    double Ustar;
    double tau, tau2, tau5;

    tau2 = 1.03/sigma;
    tau5 = 1.12/sigma;
    tau = (tau5-tau2)/3.0*f+tau2;
    if (r<=sigma) {
        Ustar =   5.0/18.0*pow(f,3.0/2.0)
                * (-log(r/sigma)+1.0/(2.0*gsl_pow_2(tau*sigma)));
    } else {
        Ustar =   5.0/18.0*pow(f,3.0/2.0)
                * 1.0/(2.0*gsl_pow_2(tau*sigma))
                * exp(-tau*tau*(r*r-sigma*sigma));
    }
    return kb*T*Ustar;
}
