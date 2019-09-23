/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   8.08.2014
 */
#include <sasfit_oz.h>

//hard sphere potential with 3 Yukawa tails:
#define sigma   p[0] // Diameter of spheres in nm
#define kappa   p[1] // reciprocal Debye-Hückel screening length
					 // kappa^2=4 pi LB sum(rho_alpha Z_alpha^2,alpha)
#define Z		p[2] // effective or dressed surface charge number of a colloidal particle
#define LB 		p[3] // Bjerrum length LB = e^2/(epsilon kB T), for water at RT LB=0.71
#define A       p[4] // effective Hamaker constant
#define GHY     p[5] // Hydra parameter
#define DH 		p[6] // hydra length

#define kb GSL_CONST_MKSA_BOLTZMANN

double U_DLVO_Hydra(double r, double T, double *p) {
    double DLVO, Hydra;
    DLVO=U_DLVO(r,T,p);
    if (r<=sigma) {
		return DLVO;
	}
	Hydra= kb*T*2*GHY*exp(-(r-sigma)/DH);
	return DLVO-Hydra;
}

double U_Ref_DLVO_Hydra(double r, double T, double *p) {
    return U_Ref_DLVO(r,T,p);
}


double U_Pert_DLVO_Hydra(double r, double T, double *p) {
    if (r<=sigma) {
		return 0;
	} else {
        return U_DLVO_Hydra(r,T,p);
	}
}

double U_SR_DLVO_Hydra(double r, double T, double *p) {
	double a,UvdW;

    if (r<=sigma) {
		return GSL_POSINF;
	} else {
        a = 0.5*sigma;
        UvdW = -kb*T*A/6.0 * (
                  2.0*a*a/(r*r-4*a*a)
                + 2.0*gsl_pow_2(a/r)
                + log(1.0-4.0*gsl_pow_2(a/r))
                );
        return UvdW;
	}
}

double U_LR_DLVO_Hydra(double r, double T, double *p) {
	double a,Uel;
    if (r<=sigma) {
        a = 0.5*sigma;
		Uel =   kb*T*LB*gsl_pow_2(Z)/
                gsl_pow_2(1.0+kappa*a)*
    			1/sigma;
		return Uel;
	} else {
        a = 0.5*sigma;
		Uel =   kb*T*LB*gsl_pow_2(Z)/
                gsl_pow_2(1.0+kappa*a)*
    			exp(-kappa*(r-sigma))/r;
        return Uel;
	}
}

double U_R_DLVO_Hydra(double r, double T, double *p) {
	double a,Uel;

    if (r<=sigma) {
		return GSL_POSINF;
	} else {
        a = 0.5*sigma;
		Uel =   kb*T*LB*gsl_pow_2(Z)/
                gsl_pow_2(1.0+kappa*a)*
    			exp(-kappa*(r-sigma))/r;
        return Uel;
	}
}

double U_A_DLVO_Hydra(double r, double T, double *p) {
	double a, UvdW;

    if (r<=sigma) {
        return 0;
	} else {
        a = 0.5*sigma;
        UvdW = -kb*T*A/6.0 * (
                  2.0*a*a/(r*r-4*a*a)
                + 2.0*gsl_pow_2(a/r)
                + log(1.0-4.0*gsl_pow_2(a/r))
                );
        return UvdW;
	}
}
