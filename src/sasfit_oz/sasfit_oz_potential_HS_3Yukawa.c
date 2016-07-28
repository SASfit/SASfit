/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   8.08.2014
 */
#include <sasfit_oz.h>

//hard sphere potential with 3 Yukawa tails:
#define sigma   p[0] // Diameter of spheres in nm
#define K1      p[1] // depth of first Yukawa contribution
#define LAMBDA1 p[2] // inverse range of first Yukawa contribution
#define K2      p[3] // depth of second Yukawa contribution
#define LAMBDA2 p[4] // depth of second Yukawa contribution
#define K3      p[5] // depth of third Yukawa contribution
#define LAMBDA3 p[6] // depth of third Yukawa contribution

#define kb GSL_CONST_MKSA_BOLTZMANN

double U_HS_3Yukawa(double r, double T, double *p) {
    double Y1, Y2, Y3;
    if (r<sigma) {
        return GSL_POSINF;
    } else {
        Y1 = K1*exp(-LAMBDA1*(r-sigma)/sigma);
        Y2 = K2*exp(-LAMBDA2*(r-sigma)/sigma);
        Y3 = K3*exp(-LAMBDA3*(r-sigma)/sigma);
        return kb*T*sigma/r*(Y1+Y2+Y3);
    }
}

double U_Ref_HS_3Yukawa(double r, double T, double *p) {
    double Y1, Y2, Y3;
    if (r<sigma) {
        return GSL_POSINF;
    } else {
        return 0;
    }
}


double U_Pert_HS_3Yukawa(double r, double T, double *p) {
    double Y1, Y2, Y3;
    if (r<sigma) {
        return U_HS_3Yukawa(sigma, T, p);
    } else {
        Y1 = K1*exp(-LAMBDA1*(r-sigma)/sigma);
        Y2 = K2*exp(-LAMBDA2*(r-sigma)/sigma);
        Y3 = K3*exp(-LAMBDA3*(r-sigma)/sigma);
        return kb*T*sigma/r*(Y1+Y2+Y3);
    }
}

double U_SR_HS_3Yukawa(double r, double T, double *p) {
    double Y1, Y2, Y3;
    if (r<sigma) {
        return GSL_POSINF;
    } else {
        return 0;
    }
}


double U_LR_HS_3Yukawa(double r, double T, double *p) {
    double Y1, Y2, Y3;
    if (r<sigma) {
        return U_HS_3Yukawa(sigma, T, p);
    } else {
        Y1 = K1*exp(-LAMBDA1*(r-sigma)/sigma);
        Y2 = K2*exp(-LAMBDA2*(r-sigma)/sigma);
        Y3 = K3*exp(-LAMBDA3*(r-sigma)/sigma);
        return kb*T*sigma/r*(Y1+Y2+Y3);
    }
}

double U_R_HS_3Yukawa(double r, double T, double *p) {
    double Y1, Y2, Y3;
    if (r<sigma) {
        return GSL_POSINF;
    } else {
        if (K1>0) Y1 = K1*exp(-LAMBDA1*(r-sigma)/sigma); else Y1=0;
        if (K2>0) Y2 = K2*exp(-LAMBDA2*(r-sigma)/sigma); else Y2=0;
        if (K3>0) Y3 = K3*exp(-LAMBDA3*(r-sigma)/sigma); else Y3=0;
        return kb*T*sigma/r*(Y1+Y2+Y3);
    }
}

double U_A_HS_3Yukawa(double r, double T, double *p) {
    double Y1, Y2, Y3;
    if (r<sigma) {
        if (K1<0) Y1 = K1; else Y1=0;
        if (K2<0) Y2 = K2; else Y2=0;
        if (K3<0) Y3 = K3; else Y3=0;
        return kb*T*sigma/r*(Y1+Y2+Y3);
    } else {
        if (K1<0) Y1 = K1*exp(-LAMBDA1*(r-sigma)/sigma); else Y1=0;
        if (K2<0) Y2 = K2*exp(-LAMBDA2*(r-sigma)/sigma); else Y2=0;
        if (K3<0) Y3 = K3*exp(-LAMBDA3*(r-sigma)/sigma); else Y3=0;
        return kb*T*sigma/r*(Y1+Y2+Y3);
    }
}