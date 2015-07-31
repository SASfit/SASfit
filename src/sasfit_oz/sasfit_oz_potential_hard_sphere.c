/*
 * Author(s) of this file:
 *   Evgeniy Ponomarev (evgeniy.ponomarev@epfl.ch)
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   Modified 13.09.2013
 */
#include <sasfit_oz.h>

//Depletion potential:
#define sigma p[0] //Diameter of spheres in nm

double U_Hard_Sphere(double r, double T, double *p) {
    if (r<sigma) return GSL_POSINF;
    else  return 0;
}

double U_SR_Hard_Sphere(double r, double T, double *p) {
    if (r<sigma) return GSL_POSINF;
    else  return 0;
}

double U_LR_Hard_Sphere(double r, double T, double *p) {
    return 0;
}

double U_R_Hard_Sphere(double r, double T, double *p) {
    if (r<sigma) return GSL_POSINF;
    else  return 0;
}

double U_A_Hard_Sphere(double r, double T, double *p) {
    return 0;
}
