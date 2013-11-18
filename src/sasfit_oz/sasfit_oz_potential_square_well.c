/*
 * Author(s) of this file:
 *   Evgeniy Ponomarev (evgeniy.ponomarev@epfl.ch)
 *   Modified 13.09.2013
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   26.10.2013
 */
#include <sasfit_oz.h>

//Sticky hard sphere:
#define SIGMA p[0] // Diameter of spheres in nm
#define EPSILON   p[1] // hight of square well
#define DELTA p[2] // width of the square well

 double U_Square_Well_Sphere(double d, double temp, double *p) {
    if (d<SIGMA) {
        return GSL_POSINF;
    } else if (d>=SIGMA&&d<=SIGMA+fabs(DELTA)) {
        return kb*temp*EPSILON;
    } else {
        return 0.0;
    }
}


 double U_SR_Square_Well_Sphere(double d, double temp, double *p) {
    if (d<SIGMA) {
        return GSL_POSINF;
    } else if (d>=SIGMA&&d<=SIGMA+fabs(DELTA)) {
        return kb*temp*EPSILON;
    } else {
        return 0.0;
    }
}

double U_LR_Square_Well_Sphere(double d, double T, double *p) {
    return 0;
}

double U_R_Square_Well_Sphere(double d, double temp, double *p) {
    if (d<SIGMA) {
        return GSL_POSINF;
    } else if (d>=SIGMA&&d<=SIGMA+fabs(DELTA)) {
        if (EPSILON >= 0) {
            return kb*temp*EPSILON;;
        } else {
            return 0;
        }
    } else {
        return 0.0;
    }
}

double U_A_Square_Well_Sphere(double d, double temp, double *p) {
    if (EPSILON >= 0.0) return 0.0;
    if (d<=SIGMA+fabs(DELTA)) {
        return kb*temp*EPSILON;;
    } else {
        return 0.0;
    }
}


double U_Ref_Square_Well_Sphere(double d, double temp, double *p) {
    if (d<SIGMA) {
        return GSL_POSINF;
    } else {
        return 0.0;
    }
}


double U_Pert_Square_Well_Sphere(double d, double temp, double *p) {
    if (d<=SIGMA+fabs(DELTA)) {
        return kb*temp*EPSILON;;
    } else {
        return 0.0;
    }
}
