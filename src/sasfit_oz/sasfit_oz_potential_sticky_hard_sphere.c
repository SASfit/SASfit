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
#define TAU   p[1] // stickiness parameter
#define DELTA p[2] // width of the square well

 double U_Sticky_Hard_Sphere(double d, double temp, double *p) {
    double energy;
    if (d<SIGMA) {
        return GSL_POSINF;
    } else if (d>=SIGMA&&d<=SIGMA+fabs(DELTA)) {
        energy = kb*temp*log(12.0*TAU*fabs(DELTA)/(SIGMA+fabs(DELTA)));
        return energy;
    } else {
        return 0.0;
    }
}


 double U_SR_Sticky_Hard_Sphere(double d, double temp, double *p) {
    double energy;
    if (d<SIGMA) {
        return GSL_POSINF;
    } else if (d>=SIGMA&&d<=SIGMA+fabs(DELTA)) {
        energy = kb*temp*log(12.0*TAU*fabs(DELTA)/(SIGMA+fabs(DELTA)));
        return energy;
    } else {
        return 0.0;
    }
}

double U_LR_Sticky_Hard_Sphere(double d, double T, double *p) {
    return 0;
}

double U_R_Sticky_Hard_Sphere(double d, double temp, double *p) {
    double energy;
    energy = kb*temp*log(12.0*TAU*fabs(DELTA)/(SIGMA+fabs(DELTA)));
    if (d<SIGMA) {
        return GSL_POSINF;
    } else if (d>=SIGMA&&d<=SIGMA+fabs(DELTA)) {
        if (energy >= 0) {
            return energy;
        } else {
            return 0;
        }
    } else {
        return 0.0;
    }
}

double U_A_Sticky_Hard_Sphere(double d, double temp, double *p) {
    double energy;
    energy = kb*temp*log(12.0*TAU*fabs(DELTA)/(SIGMA+fabs(DELTA)));
    if (energy >= 0.0) return 0.0;
    if (d<=SIGMA+fabs(DELTA)) {
        return energy;
    } else {
        return 0.0;
    }
}


double U_Ref_Sticky_Hard_Sphere(double d, double temp, double *p) {
    if (d<SIGMA) {
        return GSL_POSINF;
    } else {
        return 0.0;
    }
}


double U_Pert_Sticky_Hard_Sphere(double d, double temp, double *p) {
    double energy;
    energy = kb*temp*log(12.0*TAU*fabs(DELTA)/(SIGMA+fabs(DELTA)));
    if (d<=SIGMA+fabs(DELTA)) {
        return energy;
    } else {
        return 0.0;
    }
}
