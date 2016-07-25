/*
 * Author(s) of this file:
 *   Evgeniy Ponomarev (evgeniy.ponomarev@epfl.ch)
 *   Modified 13.09.2013
 *   Joachim Kohlbrecher(joachim.kohlbrecher@psi.ch)
 *   Modified 18.11.2013
 */

 // Oversteegen, S. & Lekkerkerker, H.
 // On the accuracy of the Derjaguin approximation for depletion potentials
 // Physica A: Statistical Mechanics and its Applications, 2004, 341, 23-39

#include <sasfit_oz.h>

//Depletion potential:
#define sigma1 p[0] //Diameter of bigger particles in nm
#define sigma2 p[1] //Diameter of smaller particles in nm
#define PHI2 p[2] // Volume fraction of smaller particles

#define kb GSL_CONST_MKSA_BOLTZMANN

double U_Depletion(double r, double T, double *p) {
    double  ro2, rt;
    rt=sigma2/sigma1;
    ro2= 6.0*PHI2/(M_PI*pow(sigma2,3.0));
    if (r<sigma1) {
        return GSL_POSINF;
    } else if (r>=sigma1&&r<sigma1+sigma2) {
        return (-kb*T*ro2*M_PI*gsl_pow_3(sigma1)*gsl_pow_3(1.+rt)/6.)*
        (1.-(3.*r/(2.*(1.+rt)*sigma1))
           +(gsl_pow_3(r)/(2.0*gsl_pow_3(1+rt)*gsl_pow_3(sigma1)))
        );
    } else {
        return 0.0;
    }
}

#define sigma_large p[0] //Diameter of bigger particles
#define sigma_small p[1] //Diameter of smaller particles
#define nb          p[2] // number density of depleting particles

double U_DepletionOfSpheresBySpheres(double r, double T, double *p) {
    double NI, NIImNIII, l,h;

    l = sigma_small;
    if (r<sigma_large)      return GSL_POSINF;
    if (r>sigma_large+l)    return 0.0;

    h = r-sigma_large;
    NI  = nb*2.0/3.0*M_PI*gsl_pow_2(0.5*l-0.5*h)*(1.5*sigma_large+l+h/2.0);
    NIImNIII = 0.0;
    return -kb*T*(NI+NIImNIII);
}

double U_R_DepletionOfSpheresBySpheres(double r, double T, double *p) {
    if (r<sigma_large)      return GSL_POSINF;
    if (r>sigma_large)    return 0.0;
}
double U_Ref_DepletionOfSpheresBySpheres(double r, double T, double *p) {
    return U_R_DepletionOfSpheresBySpheres(r,T,p);
}
double U_SR_DepletionOfSpheresBySpheres(double r, double T, double *p) {
    return U_R_DepletionOfSpheresBySpheres(r,T,p);
}

double U_A_DepletionOfSpheresBySpheres(double r, double T, double *p) {
    if (r<sigma_large) {
        U_DepletionOfSpheresBySpheres(sigma_large,T,p); 
    } else {
        U_DepletionOfSpheresBySpheres(r,T,p);
    }
}
double U_Pert_DepletionOfSpheresBySpheres(double r, double T, double *p) {
    return U_A_DepletionOfSpheresBySpheres(r,T,p);
}
double U_LR_DepletionOfSpheresBySpheres(double r, double T, double *p) {
    return U_A_DepletionOfSpheresBySpheres(r,T,p);
}


#define D p[1] //Diameter of depleting discs

double U_DepletionOfSpheresByDiscs(double r, double T, double *p) {
    double NI, NIImNIII, l,h,s;
    l = D;
    s=sigma_large;
    if (r<s)      return GSL_POSINF;
    if (r>s+D)    return 0.0;

    h = r-sigma_large;
    NI = nb*M_PI/6.0*s*D*D*1.5*gsl_pow_2(1.0-h/D);
    NIImNIII = M_PI/6.0*nb*D*D*s*(
                    -1.5*gsl_pow_2(1.0-h/D)
                    -3./4.*M_PI*h/D
                    +1.5*h/D*asin(h/D)
                    +(1.0+0.5*gsl_pow_2(h/D))*sqrt(1.0-gsl_pow_2(h/D))
                                   );
    return -kb*T*(NI+NIImNIII);
}
double U_R_DepletionOfSpheresByDiscs(double r, double T, double *p) {
    if (r<sigma_large)      return GSL_POSINF;
    if (r>sigma_large)    return 0.0;
}
double U_Ref_DepletionOfSpheresByDiscs(double r, double T, double *p) {
    return U_R_DepletionOfSpheresByDiscs(r,T,p);
}
double U_SR_DepletionOfSpheresByDiscs(double r, double T, double *p) {
    return U_R_DepletionOfSpheresByDiscs(r,T,p);
}
double U_A_DepletionOfSpheresByDiscs(double r, double T, double *p) {
    if (r<sigma_large) {
        U_DepletionOfSpheresByDiscs(sigma_large,T,p); 
    } else {
        U_DepletionOfSpheresByDiscs(r,T,p);
    }
}
double U_Pert_DepletionOfSpheresByDiscs(double r, double T, double *p) {
    return U_A_DepletionOfSpheresByDiscs(r,T,p);
}
double U_LR_DepletionOfSpheresByDiscs(double r, double T, double *p) {
    return U_A_DepletionOfSpheresByDiscs(r,T,p);
}
#define L p[1] //length of depleting discs

double U_DepletionOfSpheresByRods(double r, double T, double *p) {
    double NI, NIImNIII, l,h,s;
    l = L;
    s=sigma_large;
    if (r<s)      return GSL_POSINF;
    if (r>s+L)    return 0.0;

    h = r-sigma_large;
    NI = nb*M_PI/12.0*s*L*L*3.0*gsl_pow_2(1-h/L);
    NIImNIII = M_PI/12.0 *nb*L*L*s*(
                    -2
                    -gsl_pow_3(h/L)
                    +3.0*h/L
                                   );
    return -kb*T*(NI+NIImNIII);
}
double U_R_DepletionOfSpheresByRods(double r, double T, double *p) {
    if (r<sigma_large)      return GSL_POSINF;
    if (r>sigma_large)    return 0.0;
}
double U_Ref_DepletionOfSpheresByRods(double r, double T, double *p) {
    return U_R_DepletionOfSpheresByRods(r,T,p);
}
double U_SR_DepletionOfSpheresByRods(double r, double T, double *p) {
    return U_R_DepletionOfSpheresByRods(r,T,p);
}
double U_A_DepletionOfSpheresByRods(double r, double T, double *p) {
    if (r<sigma_large) {
        U_DepletionOfSpheresByRods(sigma_large,T,p); 
    } else {
        U_DepletionOfSpheresByRods(r,T,p);
    }
}
double U_Pert_DepletionOfSpheresByRods(double r, double T, double *p) {
    return U_A_DepletionOfSpheresByRods(r,T,p);
}
double U_LR_DepletionOfSpheresByRods(double r, double T, double *p) {
    return U_A_DepletionOfSpheresByRods(r,T,p);
}