/*
 * Author(s) of this file:
 *   Evgeniy Ponomarev (evgeniy.ponomarev@epfl.ch)
 *   Modified 13.09.2013
 */
#include <sasfit_oz.h>

//Ionic Microgel:

#define sigma p[0] //diameter
#define Z p[1]  // charge of the microgel in units of e (charge of electron)
#define ED p[2] // dielectric constant of the solvent - unitless
#define KPi p[3] // inverse Debye screening length - unitless
#define EPSILON p[4] // Hertz potential strength - unitless

double U_Ionic_Microgel(double r, double T, double *p) {
    double el, kp, phiind, p1,p2,p3,p4,p5,p6;
    el= 4.80320425e-10;//charge of electron in statcoulombs http://en.wikipedia.org/wiki/Elementary_charge
    kp=KPi/sigma;
    if (r <= sigma ) {
        p1=(1-exp(-kp*r)+0.5*pow(kp,2)*pow(r,2)+(pow(kp,4)*pow(r,4)/24))*(1-(4/(pow(kp,2)*pow(sigma,2))));
        p2=4*exp(-kp*sigma)*sinh(kp*r)/(kp*sigma);
        p3=(exp(-kp*sigma)*sinh(kp*r)+pow(kp,2)*sigma*r+(pow(kp,4)*(pow(sigma,3)*r+pow(r,3)*sigma)/6))*(1+(4/(pow(kp,2)*pow(sigma,2))));
        p4=(4*r/sigma)*(1+0.5*pow(kp,2)*pow(sigma,2)+(pow(kp,4)*pow(sigma,4)/30));
        p5=(8*pow(r,3)/(3*pow(sigma,3)))*(0.25*pow(kp,2)*pow(sigma,2)+(pow(kp,4)*pow(sigma,4)/12));
        p6=pow(kp,4)*pow(r,6)/(180*pow(sigma,2));
        phiind=p1+p2+p3-p4-p5-p6;
        return  EPSILON*T*kb*pow(1-(r/sigma),2.5)
                +((2*pow(Z,2)*pow(el,2))/(ED*sigma))*(1.2-2*pow(r/sigma,2)+1.5*pow(r/sigma,3)-0.2*pow(r/sigma,5))
                -phiind*(72*pow(Z,2)*pow(el,2)/(ED*pow(kp,4)*pow(sigma,4)*r));
    } else  {
        return (144*pow(Z,2)*pow(el,2)/(ED*pow(kp,4)*pow(sigma,4)))*pow(cosh(0.5*kp*sigma)-(2*sinh(0.5*kp*sigma)/(kp*sigma)),2)*exp(-kp*r)/r;
    }
}

