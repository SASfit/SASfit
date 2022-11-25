/*
 * Author(s) of this file:
 *   Evgeniy Ponomarev (evgeniy.ponomarev@epfl.ch)
 *   Modified 13.09.2013
 *   Joachim Kohlbrecher(joachim.kohlbrecher@psi.ch)
 *   Modified 18.11.2013
 */
#include <sasfit_oz.h>

//Ionic Microgel:
// Likos, C. N.
// Wyss, A. F.-N. H. M.; Mattsson, J. & Weitz, D. A. (Eds.)
// Microgel Suspensions: Fundamentals and Applications
// Chapter 7. Structure and Thermodynamics of Ionic Microgels Microgel Suspensions,
// Wiley-VCH Verlag GmbH & Co. KGaA, 2011, 163-193

#define sigma   p[0] // diameter
#define Z       p[1] // charge of the microgel in units of e (charge of electron)
#define ED      p[2] // dielectric constant of the solvent - unitless
#define KPi     p[3] // inverse Debye screening length - unitless
#define EPSILON p[4] // Hertz potential strength - unitless

#define kb GSL_CONST_MKSA_BOLTZMANN

double U_Ionic_Microgel(double r, double T, double *p) {
    double el, kp, phiind, p1,p2,p3,p4,p5,p6;
    el= 4.80320425e-10;//charge of electron in statcoulombs http://en.wikipedia.org/wiki/Elementary_charge
    kp=KPi/sigma;
    if (r <= sigma ) {
        p1=(1.0-exp(-kp*r)+0.5*gsl_pow_2(kp*r)+gsl_pow_4(kp*r)/24.0)
             *(1.0-4.0/gsl_pow_2(kp*sigma));
        p2=4.0*exp(-kp*sigma)*sinh(kp*r)/(kp*sigma);
        p3=(exp(-kp*sigma)*sinh(kp*r)
                 +gsl_pow_2(kp)*sigma*r
                 +gsl_pow_4(kp)*(gsl_pow_3(sigma)*r+gsl_pow_3(r)*sigma)/6.0 )
            *(1.0+4.0/gsl_pow_2(kp*sigma));
        p4=(4.0*r/sigma)*(1.0+0.5*gsl_pow_2(kp*sigma)+gsl_pow_4(kp*sigma)/30.0 );
        p5=(8.0*gsl_pow_3(r/sigma)/3.0)*(0.25*gsl_pow_2(kp*sigma)+gsl_pow_4(kp*sigma)/12.0);
        p6=gsl_pow_4(kp)*gsl_pow_6(r)/(180.0*gsl_pow_2(sigma));
        phiind=p1+p2+p3-p4-p5-p6;
        return  EPSILON*T*kb*pow(1.0-(r/sigma),2.5)
                +(2*gsl_pow_2(Z*el))/(ED*sigma)*
                    (1.2-2.0*gsl_pow_2(r/sigma)+1.5*gsl_pow_3(r/sigma)-0.2*gsl_pow_5(r/sigma))
                -phiind*(72.0*gsl_pow_2(Z*el)/(ED*gsl_pow_4(kp*sigma)*r));
    } else  {
        return (144.0*gsl_pow_2(Z*el)/(ED*gsl_pow_4(kp*sigma)))
               *gsl_pow_2(cosh(0.5*kp*sigma)-(2*sinh(0.5*kp*sigma)/(kp*sigma)))
               *exp(-kp*r)/r;
    }
}


