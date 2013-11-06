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
    ro2= 6*PHI2/(M_PI*pow(sigma2,3));
    if (r<sigma1) return GSL_POSINF;
    else if (r>=sigma1&&r<sigma1+sigma2) return (-kb*T*ro2*M_PI*gsl_pow_3(sigma1)*gsl_pow_3(1.+rt)/6.)*(1.-(3.*r/(2.*(1.+rt)*sigma1))+(gsl_pow_3(r)/(2*gsl_pow_3(1+rt)*gsl_pow_3(sigma1))));
    else  return 0;
}
