/*
 * Author(s) of this file:
 *   Evgeniy Ponomarev (evgeniy.ponomarev@epfl.ch)
 *   Modified 13.09.2013
 */
#include <sasfit_oz.h>

//Depletion potential:
#define sigma1 p[0] //Diameter of bigger particles in nm
#define sigma2 p[1] //Diameter of smaller particles in nm
#define PHI2 p[2] // Volume fraction of smaller particles

double U_Depletion(double r, double T, double *p) {
    double  ro2, rt;
    rt=sigma2/sigma1;
    ro2= 6*PHI2/(Pi*pow(sigma2,3));
    if (r<sigma1) return GSL_POSINF;
    else if (r>=sigma1&&r<sigma1+sigma2) return (-kb*T*ro2*Pi*pow(sigma1,3)*pow(1+rt,3)/6)*(1-(3*r/(2*(1+rt)*sigma1))+(pow(r,3)/(2*pow(1+rt,3)*pow(sigma1,3))));
    else  return 0;
}
