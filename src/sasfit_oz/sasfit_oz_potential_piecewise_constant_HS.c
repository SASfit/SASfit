/*
 * Author(s) of this file:
 *   Evgeniy Ponomarev (evgeniy.ponomarev@epfl.ch)
 *   Modified 13.09.2013
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   26.10.2013
 */
#include <sasfit_oz.h>

//Sticky hard sphere:
#define SIGMA    p[0] // Diameter of spheres in nm
#define EPSILON1 p[1] // strength of first segment piece
#define DELTA1   p[2] // width of first segment piece
#define EPSILON2 p[3] // strength of second segment piece
#define DELTA2   p[4] // width of second segment piece
#define EPSILON3 p[5] // strength of third segment piece
#define DELTA3   p[6] // width of third segment piece
#define kb GSL_CONST_MKSA_BOLTZMANN

 double U_Piecewise_Constant_HS(double d, double temp, double *p) {
    if (d<SIGMA) {
        return GSL_POSINF;
    } else if (d>=SIGMA&&d<SIGMA+fabs(DELTA1)) {
        return kb*temp*EPSILON1;
    } else if (d>=SIGMA+fabs(DELTA1)&&d<SIGMA+fabs(DELTA1)+fabs(DELTA2)) {
        return kb*temp*EPSILON2;
    } else if (d>=SIGMA+fabs(DELTA1)+fabs(DELTA2)&&d<SIGMA+fabs(DELTA1)+fabs(DELTA2)+fabs(DELTA3)) {
        return kb*temp*EPSILON3;
    } else {
        return 0.0;
    }
}


 double U_SR_Piecewise_Constant_HS(double d, double temp, double *p) {
    if (d<SIGMA) {
        return GSL_POSINF;
    } else if (d>=SIGMA&&d<SIGMA+fabs(DELTA1)) {
        return kb*temp*EPSILON1;
    } else if (d>=SIGMA+fabs(DELTA1)&&d<SIGMA+fabs(DELTA2)) {
        return kb*temp*EPSILON2;
    } else if (d>=SIGMA+fabs(DELTA2)&&d<SIGMA+fabs(DELTA3)) {
        return kb*temp*EPSILON3;
    } else {
        return 0.0;
    }
}

double U_LR_Piecewise_Constant_HS(double d, double T, double *p) {
    return 0;
}

double U_R_Piecewise_Constant_HS(double d, double temp, double *p) {
    if (d<SIGMA) {
        return GSL_POSINF;
    } else if (d>=SIGMA&&d<SIGMA+fabs(DELTA1)) {
        if (EPSILON1 >= 0) {
            return kb*temp*EPSILON1;
        } else {
            return 0;
        }
    } else if (d>=SIGMA+fabs(DELTA1)&&d<SIGMA+fabs(DELTA2)) {
        if (EPSILON2 >= 0) {
            return kb*temp*EPSILON2;
        } else {
            return 0;
        }
    } else if (d>=SIGMA+fabs(DELTA2)&&d<SIGMA+fabs(DELTA3)) {
        if (EPSILON3 >= 0) {
            return kb*temp*EPSILON3;
        } else {
            return 0;
        }
    } else {
        return 0.0;
    }
}

double U_A_Piecewise_Constant_HS(double d, double temp, double *p) {
    if (d>=SIGMA&&d<SIGMA+fabs(DELTA1)) {
        if (EPSILON1 < 0) {
            return kb*temp*EPSILON1;
        } else {
            return 0;
        }
    } else if (d>=SIGMA+fabs(DELTA1)&&d<SIGMA+fabs(DELTA2)) {
        if (EPSILON2 < 0) {
            return kb*temp*EPSILON2;
        } else {
            return 0;
        }
    } else if (d>=SIGMA+fabs(DELTA2)&&d<SIGMA+fabs(DELTA3)) {
        if (EPSILON3 < 0) {
            return kb*temp*EPSILON3;
        } else {
            return 0;
        }
    } else {
        return 0.0;
    }
}


double U_Ref_Piecewise_Constant_HS(double d, double temp, double *p) {
    if (d<SIGMA) {
        return GSL_POSINF;
    } else {
        return 0.0;
    }
}


double U_Pert_Piecewise_Constant_HS(double d, double temp, double *p) {
    if (d>=SIGMA&&d<SIGMA+fabs(DELTA1)) {
        return kb*temp*EPSILON1;
    } else if (d>=SIGMA+fabs(DELTA1)&&d<SIGMA+fabs(DELTA2)) {
        return kb*temp*EPSILON2;
    } else if (d>=SIGMA+fabs(DELTA2)&&d<SIGMA+fabs(DELTA3)) {
        return kb*temp*EPSILON3;
    } else {
        return 0.0;
    }
}
